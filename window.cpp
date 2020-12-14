#include "window.h"

Window::Window(QWidget *parent)
    : QWidget(parent)
{

    buttonTopLayout = new QHBoxLayout;
    buttonDownLayot = new QHBoxLayout;
    graf_tablLayout = new QHBoxLayout;
    button_tablLayout = new QVBoxLayout;
    openButton = new QPushButton("Открыть");
    saveButton = new QPushButton("Записать");
    printButton = new QPushButton("Печать");
    table = new QTableWidget;

    chart = new QtCharts::QChart;
    graphic = new QtCharts::QChartView(chart);
    line = new QtCharts::QLineSeries;
    checkDot = new QtCharts::QScatterSeries;
    uncheckDot = new QtCharts::QScatterSeries;

    table->setFixedSize(180, 600);

    buttonTopLayout->addWidget(openButton);buttonTopLayout->addWidget(saveButton);
    buttonDownLayot->addWidget(printButton);//buttonDownLayot->addWidget(insertRowButton);
    button_tablLayout->addLayout(buttonTopLayout);
    button_tablLayout->addWidget(table);
    button_tablLayout->addLayout(buttonDownLayot);
    graf_tablLayout->addLayout(button_tablLayout);
    graf_tablLayout->addWidget(graphic);


    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(table, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(reCalc()));

    saveButton->setEnabled(FALSE);
    printButton->setEnabled(FALSE);

    setLayout(graf_tablLayout);
    setWindowTitle("Турбинный режим");
    resize(800, 600);
}

Window::~Window()
{
}


void Window::createTabl()
{
    table->clear();
    table->setColumnCount(3); // Указываем число колонок
    table->setShowGrid(true); // Включаем сетку
       // Разрешаем выделение только одного элемента
    table->setSelectionMode(QAbstractItemView::SingleSelection);
       // Разрешаем выделение построчно
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
       // Устанавливаем заголовки колонок

    headers = new QStringList();
    headers->append(" ");
    headers->append("n1'");
    headers->append("E");
    table->setHorizontalHeaderLabels(*headers);
    table->setColumnWidth(0,10);
    table->setColumnWidth(1,50);
    table->setColumnWidth(2,50);

    /* Выполняем заполнение QTableWidget записями с помощью цикла
         * */
        for(int i = 0; i<N; i++)
        {
            table->insertRow(i);    // Вставляем строку
            QTableWidgetItem *item = new QTableWidgetItem();// Создаём элемент, который будет выполнять роль чекбокса
            item->data(Qt::CheckStateRole);
            item->setCheckState(Qt::Checked);
            table->setItem(i,0, item);// Устанавливаем чекбокс в первую колонку
            // Далее забираем все данные из протокола и устанавливаем в остальные поля
            table->setItem(i,1, new QTableWidgetItem(QString().setNum(vn1[i])));
            table->setItem(i,2, new QTableWidgetItem(QString().setNum(vE[i])));
        }

        saveButton->setEnabled(TRUE);
        printButton->setEnabled(TRUE);

}

void Window::createLine()
{

//    if(!chart->series().isEmpty())
//    {
//        chart->removeSeries(line);
//    }

    line->clear();
    qreal x, step;

    x = checkDot->at(0).x();
    step = 0.02*x;
    do
    {
        line->append(x, (a[0]+a[1]*x+a[2]*x*x+a[3]*x*x*x));
        x = x + step;

    }while(x <= checkDot->at(N-1).x());

    chart->addSeries(line);
//    line->setMarkerShape(QScatterSeries::MarkerShapeCircle);
//    line->setMarkerSize(8.0);uncheckDot->setMarkerSize(8.0);
    line->setColor(Qt::black);

}

void Window::createDot()
{
    double minX=1000.0, minY=200.0, maxX=0.0, maxY=0.0;
    double X,Y;

    checkDot->clear(); uncheckDot->clear();
    N=table->rowCount();

    for(int row = 0; row < table->rowCount(); row++)
    {
        X=table->item(row, 1)->text().toFloat();
        Y=table->item(row, 2)->text().toFloat();

        if(table->item(row,0)->checkState() == Qt::Checked)
        {

            checkDot->append(X,Y);


        }else
        {
            uncheckDot->append(X, Y);
            N--;

        }
        if(X<minX)minX=X;
        if(X>maxX)maxX=X;
        if(Y<minY)minY=Y;
        if(Y>maxY)maxY=Y;
    }
    if(!chart->series().isEmpty())
    {
       chart->removeSeries(checkDot);chart->removeSeries(uncheckDot);

    }
    chart->addSeries(checkDot); chart->addSeries(uncheckDot);

    maxY = ceil(maxY);if(maxY>100.0)maxY=100.0;
    minY = floor(minY);
    maxX = ceil(maxX);
    minX = floor(minX);

    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(minY, maxY);
    chart->axes(Qt::Horizontal).first()->setRange(minX, maxX);

    checkDot->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    checkDot->setMarkerSize(8.0);uncheckDot->setMarkerSize(8.0);
    checkDot->setColor(Qt::green);uncheckDot->setColor(Qt::red);


    chart->legend()->hide();


}


void Window::openFile()
{

    fileNameOpen = QFileDialog::getOpenFileName(0, "Открыть файл", "", "*.*");
    fileOpen.setFileName(fileNameOpen);
    if (!fileOpen.open(QIODevice::ReadOnly))
    {
        qDebug() << "Ошибка открытия для чтения";
    }
    for (int i = 0; i < 14; i++) // пропускаем шапку файла
    {
        rowFile.append(fileOpen.readLine());
        rowFile.clear();

    }

    dataString.clear();
    do
    {
        rowFile.clear();
        rowFile.append(fileOpen.readLine());    //читаем строчку
        rowFile.replace(QRegExp("[^0123456789. ]+"), " ");   // заменяем в ней пробелами все кроме чисел

        vQ1.append(rowFile.mid(34,6).toFloat());
        vE.append(rowFile.mid(49,5).toFloat());
        vn1.append(rowFile.mid(41,6).toFloat());
    }while(!fileOpen.atEnd());

    N = rowFile.mid(2,2).toInt();

    fileOpen.close();

    createTabl();
    reCalc();
    //reCalc();

}

void Window::saveFile()
{

    fileNameSave = QFileDialog::getSaveFileName(0, "Сохранить файл", "", "*.*");
    fileSave.setFileName(fileNameSave);
    if(!fileSave.open(QIODevice::WriteOnly))
    {
        qDebug() << "Ошибка открытия для записи";
    }

    fileSave.close();
}

void Window::reCalc()
{

    createDot();
    solve();
    createLine();



}

void Window::freematrix(){
   //free memory for matrixes
   int i;
   for(i=0; i<K+1; i++){
       delete [] sums[i];
   }
   delete [] a;
   delete [] b;
   delete [] x;
   delete [] y;
   delete [] sums;
}

void Window::allocmatrix(){
   //allocate memory for matrixes
   int i,j,k;
   a = new double[K+1];
   b = new double[K+1];
   x = new double[N];
   y = new double[N];
   sums = new double*[K+1];
   if(x==NULL || y==NULL || a==NULL || sums==NULL){
       qDebug()<<"Not enough memory to allocate."<< N<< K;
       exit(-1);
   }
   for(i=0; i<K+1; i++){
       sums[i] = new double[K+1];
       if(sums[i]==NULL){
       qDebug()<<"Not enough memory to allocate for equations."<< K+1;
       }
   }
   for(i=0; i<K+1; i++){
       a[i]=0;
       b[i]=0;
       for(j=0; j<K+1; j++){
       sums[i][j] = 0;
       }
   }
   for(k=0; k<N; k++){
       x[k]=0;
       y[k]=0;
   }
}

void Window::readmatrix(){
   int i=0,j=0, k=0;
   //read x, y matrixes from input file
   for(k=0; k<N; k++){
       x[k] = checkDot->at(k).x();
       y[k] = checkDot->at(k).y();
   }
   //init square sums matrix
   for(i=0; i<K+1; i++){
       for(j=0; j<K+1; j++){
       sums[i][j] = 0;
       for(k=0; k<N; k++){
           sums[i][j] += pow(x[k], i+j);
           //qDebug()<<"sums "<<sums[i][j];
       }
       }
   }
   //init free coefficients column
   for(i=0; i<K+1; i++){
       for(k=0; k<N; k++){
       b[i] += pow(x[k], i) * y[k];
       }
   }
}

void Window::printresult(){
   //print polynom parameters
   int i=0;
   //qDebug()<<"\n";
   for(i=0; i<K+1; i++){
       qDebug()<<"a["<<i<<"]"<<a[i];
   }
}

void Window::diagonal(){
   int i, j, k;
   double temp=0;
   for(i=0; i<K+1; i++){
       if(sums[i][i]==0){
       for(j=0; j<K+1; j++){
           if(j==i) continue;
           if(sums[j][i] !=0 && sums[i][j]!=0){
           for(k=0; k<K+1; k++){
               temp = sums[j][k];
               sums[j][k] = sums[i][k];
               sums[i][k] = temp;
           }
           temp = b[j];
           b[j] = b[i];
           b[i] = temp;
           break;
           }
       }
       }
   }
}

void Window::solve(){
   int i=0,j=0, k=0;


//   count_num_lines();
   qDebug()<<"Number of points: N="<< N;
//   do{
//       printf("\nInput power of approximation polinom K<N: ");
//       scanf("%d", &K);
//   }while(K>=N);
   allocmatrix();

   //read data
   readmatrix();
   //check if there are 0 on main diagonal and exchange rows in that case
   diagonal();

   //process rows
   for(k=0; k<K+1; k++){
       for(i=k+1; i<K+1; i++){
       if(sums[k][k]==0){
           qDebug()<<"Solution is not exist.\n";
           exit(-1);
       }
       double M = sums[i][k] / sums[k][k];
       for(j=k; j<K+1; j++){
           sums[i][j] -= M * sums[k][j];
       }
       b[i] -= M*b[k];
       }
   }

   for(i=(K+1)-1; i>=0; i--){
       double s = 0;
       for(j = i; j<K+1; j++){
       s = s + sums[i][j]*a[j];
       }
       a[i] = (b[i] - s) / sums[i][i];
   }
   printresult();
   freematrix();
}
