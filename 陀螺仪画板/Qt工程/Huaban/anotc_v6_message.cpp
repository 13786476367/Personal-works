#include "anotc_v6_message.h"

ANOTC_V6_MESSAGE::ANOTC_V6_MESSAGE()
{

}

void ANOTC_V6_MESSAGE::readMessage(QByteArray buff)
{
    //static uint8_t frame[40],frame_len;
    //static uint8_t state=0,frame_size=0;
    for(uint8_t ch:buff)
    {
        if(state==0 && ch==0xaa)//帧头
        {
            frame[0] = ch;
            //qDebug()<<state<<':'<<ch;
            state = 1;

        }
        else if(state==1 && ch==0x01)//发送设备
        {
            frame[1] = ch;
            //qDebug()<<state<<':'<<ch;
            state = 2;
        }
        else if(state==2 && ch==0xaf)//目标设备
        {
            frame[2] = ch;
            //qDebug()<<state<<':'<<ch;
            state = 3;
        }
        else if(state==3 && ch<8)//功能字
        {
           frame[3] = ch;
           //qDebug()<<state<<':'<<ch;
           state = 4;
        }
        else if(state==4 && ch<sizeof(frame))//数据长度
        {
            frame[4] = ch;
            frame_size = 5;
            frame_len = ch;
            //qDebug()<<state<<"frame_len:"<<ch;
            state = 5;
        }
        else if(state==5 && frame_len>0)//数据
        {
            frame_len--;
            frame[frame_size++]=ch;
            //qDebug()<<state<<':'<<ch;
            if(frame_len==0)
                state = 6;
        }
        else if(state==6)//校验
        {
            //qDebug()<<state<<"sum:"<<ch;
            state = 0;
            frame[frame_size++]=ch;

            //帧检查
            uint8_t sum=0;
            for(uint8_t i=0;i<frame_size-1;i++)
                sum += frame[i];
            //qDebug()<<state<<':'<<ch;
            if(sum == frame[frame_size-1])
            {
                frame[frame_size]=0;
                //qDebug()<<"receve ok!";
                switch(frame[3])
                {
                case 0:funCmd_0(frame,frame_size);break;
                case 1:funCmd_1(frame,frame_size);break;
                case 2:funCmd_2(frame,frame_size);break;
                case 3:funCmd_3(frame,frame_size);break;
                case 4:funCmd_4(frame,frame_size);break;
                case 5:funCmd_5(frame,frame_size);break;
                case 6:funCmd_6(frame,frame_size);break;
                case 7:funCmd_7(frame,frame_size);break;
                }
            }
        }
        else
        {
            state = 0;
            //qDebug()<<state<<':'<<ch;
            frame_size = 0;
        }
    }
}

#include <QtCharts/QSplineSeries>
QT_CHARTS_USE_NAMESPACE
extern QSplineSeries series;
extern QVector<QPointF> points;
//限幅
int constrain(int source ,int min ,int max)
{
    if(source > max)
        return max;
    else if(source < min)
        return min;
    else
        return source;
}
void ANOTC_V6_MESSAGE::funCmd_0(uint8_t message[],uint8_t size)
{

}
void ANOTC_V6_MESSAGE::funCmd_1(uint8_t message[],uint8_t size)
{
    int16_t roll = constrain((int16_t)(message[5]<<8 | message[6])/30,-100,100);
    int16_t pitch = constrain((int16_t)(message[7]<<8 | message[8])/30,-100,100);
    qDebug()<<"roll:"<<roll<<"  "<<"pitch:"<<pitch;
    if(points.size()>=1000)
        points.pop_front();
    points.push_back(QPointF(roll,pitch));
    series.replace(points);
    //series.append(roll,pitch);
}
void ANOTC_V6_MESSAGE::funCmd_2(uint8_t message[],uint8_t size)
{

}
void ANOTC_V6_MESSAGE::funCmd_3(uint8_t message[],uint8_t size)
{

}
void ANOTC_V6_MESSAGE::funCmd_4(uint8_t message[],uint8_t size)
{

}
void ANOTC_V6_MESSAGE::funCmd_5(uint8_t message[],uint8_t size)
{

}
void ANOTC_V6_MESSAGE::funCmd_6(uint8_t message[],uint8_t size)
{

}
void ANOTC_V6_MESSAGE::funCmd_7(uint8_t message[],uint8_t size)
{

}
