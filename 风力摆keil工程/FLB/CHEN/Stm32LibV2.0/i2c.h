/********************************************************
			湖南工程学院创新基地
			作者:CHEN
			技术支持:QQ742010094
			日期:2019.7.2
*********************************************************/
#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

bool I2C_Start(void);				//发起I2C总线启动信号
void I2C_Stop(void);				//发起I2C总线停止信号
void I2C_SendByte(unsigned char);	//I2C发送一个字节数据
unsigned char I2C_ReadByte(void);	//I2C接受一个字节数据
bool I2C_WaitAck(void);				//I2C等待应答信号
void I2C_Ack(void);					//I2C发送应答信号
void I2C_NAck(void);				//I2C发送非应答信号
bool I2c_WriteByte(unsigned char slave_addr, unsigned char reg_addr,unsigned char const dat);
//DMP支持（如果成功返回0）
int i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data);
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__I2C_H
