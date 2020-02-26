/********************************************************
			���Ϲ���ѧԺ���»���
			����:CHEN
			����֧��:QQ742010094
			����:2019.7.2
*********************************************************/
#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

bool I2C_Start(void);				//����I2C���������ź�
void I2C_Stop(void);				//����I2C����ֹͣ�ź�
void I2C_SendByte(unsigned char);	//I2C����һ���ֽ�����
unsigned char I2C_ReadByte(void);	//I2C����һ���ֽ�����
bool I2C_WaitAck(void);				//I2C�ȴ�Ӧ���ź�
void I2C_Ack(void);					//I2C����Ӧ���ź�
void I2C_NAck(void);				//I2C���ͷ�Ӧ���ź�
bool I2c_WriteByte(unsigned char slave_addr, unsigned char reg_addr,unsigned char const dat);
//DMP֧�֣�����ɹ�����0��
int i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data);
int i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__I2C_H
