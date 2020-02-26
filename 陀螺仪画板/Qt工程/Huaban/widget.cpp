#include "widget.h"
#include "ui_widget.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QPushButton>
#include <QThread>

QSplineSeries series;
QVector<QPointF> points;
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //禁止主窗口改变大小
    this->setFixedSize(this->width(), this->height());
    //波特率只允许输入数字
    ui->baudrateBox->setValidator(new QIntValidator(0,100000000,ui->baudrateBox));
    //检索串口
    for(const QSerialPortInfo &info:QSerialPortInfo::availablePorts())
        ui->portNameBox->addItem(info.portName());
    //连接信号和槽
    connect(&this->serial, &QSerialPort::readyRead, this, &Widget::serialPort_readMessage);
    connect(ui->pushButton_2,&QPushButton::released,this,&Widget::on_pushButton2_released);
    QChart *chart = new QChart();
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
   // for(uint32_t i=0;i<100;i++)
   //     lineX.append(i,sin(0.6f*i));
   // chart.addSeries(&lineX);
   // chart.createDefaultAxes();

    series.setUseOpenGL();

    axisX->setRange(-100,100);
    axisX->setTickCount(11);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("ROLL");
    axisX->setMinorTickCount(1);

    axisY->setRange(-100,100);
    axisY->setTickCount(11);
    axisY->setLabelFormat("%g");
    axisY->setTitleText("PITCH");
    axisY->setMinorTickCount(1);

    chart->addSeries(&series);  //把曲线添加到图表
    chart->setAxisX(axisX,&series);
    chart->setAxisY(axisY,&series);
    //chart->setBackgroundBrush(QBrush(Qt::blue));

    QPen pen;
    pen = series.pen();
    qDebug()<<pen.color();
    pen.setWidth(3);
    series.setPen(pen);
//    for(int i=0;i<=360;i+=20)
//        series.append(100*cos(i/180.0*3.1415926),100*sin(i/180.0*3.1415926));

    ui->widget->setRenderHint(QPainter::Antialiasing);//抗锯齿
    ui->widget->setChart(chart);


}

Widget::~Widget()
{
    delete ui;
}

void Widget::serialPort_readMessage()
{
    anotcMessage.readMessage(this->serial.readAll());
}

void Widget::paintEvent(QPaintEvent *)
{
//    QPainter painter(this);
//    QPen pen(QColor(255,0,0),3);
//    painter.setPen(pen);
//    painter.drawLine(0,0,100,100);
}
void Widget::on_pushButton_released()
{
    if(ui->portNameBox->currentIndex()!=-1)
    {
        if(this->serial.isOpen())
        {
            this->serial.close();//关闭串口
            if(!this->serial.isOpen())
            {
                ui->baudrateBox->setEnabled(true);
                ui->portNameBox->setEnabled(true);
                ui->pushButton->setText("打开串口");
                qDebug()<<"串口关闭成功";
            }
        }
        else
        {
            this->serial.setPortName(ui->portNameBox->currentText());	 //	选取串口
            this->serial.open(QIODevice::ReadWrite);  	//打开串口
            bool ok;
            this->serial.setBaudRate(ui->baudrateBox->currentText().toInt(&ok));   //设置波特率
            this->serial.setDataBits(QSerialPort::Data8);  //设置数据位数
            this->serial.setParity(QSerialPort::NoParity); 	//设置奇偶校验
            this->serial.setStopBits(QSerialPort::OneStop);  	//设置停止位
            this->serial.setFlowControl(QSerialPort::NoFlowControl);	//设置流控制
            if(this->serial.isOpen())
            {
                ui->pushButton->setText("关闭串口");
                ui->baudrateBox->setEnabled(false);
                ui->portNameBox->setEnabled(false);
                qDebug()<<"串口打开成功,波特率："<<this->serial.baudRate();
            }
        }
    }

}

void Widget::on_pushButton2_released()
{
    points.clear();
    series.replace(points);
}
