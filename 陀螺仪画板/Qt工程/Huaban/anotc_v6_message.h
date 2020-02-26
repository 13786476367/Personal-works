#ifndef ANOTC_V6_MESSAGE_H
#define ANOTC_V6_MESSAGE_H
#include "ui_widget.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QPushButton>
#include <QThread>
class ANOTC_V6_MESSAGE
{
public:
    ANOTC_V6_MESSAGE();
    void readMessage(QByteArray buff);
    void funCmd_0(uint8_t message[],uint8_t size);
    void funCmd_1(uint8_t message[],uint8_t size);
    void funCmd_2(uint8_t message[],uint8_t size);
    void funCmd_3(uint8_t message[],uint8_t size);
    void funCmd_4(uint8_t message[],uint8_t size);
    void funCmd_5(uint8_t message[],uint8_t size);
    void funCmd_6(uint8_t message[],uint8_t size);
    void funCmd_7(uint8_t message[],uint8_t size);
private:
    uint8_t frame[40];//帧数据
    uint8_t frame_len;//帧数据长度
    uint8_t state;    //接收状态
    uint8_t frame_size;//帧长度
};

#endif // ANOTC_V6_MESSAGE_H
