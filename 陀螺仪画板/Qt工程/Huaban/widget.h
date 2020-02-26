#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <anotc_v6_message.h>

#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QSplineSeries>
#include <QGridLayout>
QT_CHARTS_USE_NAMESPACE
namespace Ui {
class Widget;
}

class Widget : public QWidget
{

    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void serialPort_readMessage();
    void paintEvent(QPaintEvent *event);
signals:
    void drawlineEvent();
private slots:
    void on_pushButton_released();
    void on_pushButton2_released();
private:
    Ui::Widget *ui;
    QSerialPort serial;
    ANOTC_V6_MESSAGE anotcMessage;
    //QChartView chartView;
    //QLineSeries series;

};

#endif // WIDGET_H
