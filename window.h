#ifndef WINDOW_H
#define WINDOW_H

#include <math.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QCheckBox>
#include <QPointF>


class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();
    
    double *x, *y;                   // координаты точки из протокола
    double *a, *b;                   // неизвестные коэффициенты полинома
    double **sums;                   // суммы степеней x,y при неизвестных коэффициентах полинома
    int N;                          // число заданных точек
    int K=3;                      // степень аппроксимирующего полинома
    
    void allocmatrix();             //выделяет память для массивов a, b, x, y, sums;
    void diagonal();                // делает так, чтобы на главной диагонали не было нулей,
                                    // чтобы не пришлось однажды делить на ноль
                                    // в процессе приведения системы к треугольному виду
    void readmatrix();              // прочитывает координаты точек и вычисляет sumsij
    void solve();                   // расчет полинома

    QHBoxLayout *buttonTopLayout, *graf_tablLayout, *buttonDownLayot;
    QVBoxLayout *button_tablLayout;
    QPushButton *openButton, *saveButton, *printButton;
    QTableWidget *table;
    QtCharts::QChartView *graphic;
    QtCharts::QChart *chart;
    QStringList *headers;
    QtCharts::QLineSeries *line;
    QtCharts::QScatterSeries *checkDot, *uncheckDot;

    QString fileNameOpen, fileNameSave;
    QFile fileOpen, fileSave;
    QByteArray fileConsist;
    QString rowFile;
    QString rowBrowser;
    QVector<QString> dataString;
    float Hh, Qv, n, M, Q1, n1, E, SIG, Th, N1, M1, Pa, Re;
    QByteArray dataWrite;
    QVector<float> vQ1, vn1, vE;

    void createTabl();
    void createLine();
    void createDot();

    void printresult();             // распечатывает получившийся столбец решений
    void freematrix();              // освобождает память, которая была выделена ранее




public slots:
    void openFile();
    void saveFile();
    void reCalc();

    //void checkUncheck();




};
#endif // WINDOW_H
