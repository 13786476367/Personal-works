#include "i2c.h"


//GPIO���ŵĳ�ʼ�����û����
//GPIO����Ϊ��©���
//8 9
#define SCL			PBout(10)			//SCL����
#define SDA			PBout(11)			//SDA����
#define READ_SDA	PBin(11)
//IO�������ã����������
#define SCL_OUT() {GPIOB->CRH&=0XFFFFF0FF;GPIOB->CRH|=3u<<(4*2);}
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8u<<(4*3);}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3u<<(4*3);}
#define delay()	delay_us(5)//__asm("nop")

//����һ����ʼ�ź�
bool I2C_Start()
{
	SCL_OUT();
	SDA_OUT();
	SDA = 1;
	delay();
//	if(!READ_SDA)		//��������Ƿ���ȷ��ʼ��
//		return false;
	SCL = 1;
	delay();
	SDA = 0;
	delay();
//	if(READ_SDA)		//��������Ƿ���ȷ��ʼ��
//		return false;	
	SCL = 0;
	delay();
	return true;		//��ʼ�źŷ��ͳɹ�
}


//����һ��ֹͣ�ź�
void I2C_Stop()
{
	SDA_OUT();
	SDA = 0;
	delay();
	SCL = 1;
	delay();
	SDA = 1;
	delay();	
}

//����һ���ֽ���Ϣ
void I2C_SendByte(uint8_t data)
{
	uint8_t count;
	SDA_OUT();
	for(count=0;count<8;count++)
	{
		if(data&0x80)//����һλ����
			SDA = 1;
		else
			SDA = 0;
		delay();
		SCL = 1;	//Ŀ�����һλ����
		delay();
		SCL = 0;
		data <<= 1;
		delay();
	}
}


//����һ���ֽ���Ϣ
unsigned char I2C_ReadByte()
{
	uint8_t data,count;
	SDA_IN();
	for(count=0;count<8;count++)
	{
		delay();
		data <<= 1;
		SCL = 1;
		delay();
		if(READ_SDA)	//����һλ����
			data++;
		SCL = 0;
		
	}
	delay();
	return data;
}

//����true��ȷӦ�𣬷���false����Ӧ��
bool I2C_WaitAck()
{
	uint8_t count;
	SDA = 1; 	//�ͷ�SDA����
	SDA_IN();
	SCL = 1;	//����Ӧ���ź�
	delay();	//�ȴ��ź��ȶ�
	for(count=0;count<50;count++)
	{
		if(!READ_SDA)	//�����⵽Ӧ���ź�
		{
			SCL = 0;
			return true;
		}
		delay();
	}
	return false;	
}

//����һ��Ӧ���ź�
void I2C_Ack()
{
	SDA_OUT();
	SDA = 0;
	delay();
	SCL = 1;
	delay();
	SCL = 0;
	delay();
	SDA = 1;//�ͷ�SDA���ߣ����������䣩
}

//����һ����Ӧ��
void I2C_NAck()
{
	SDA_OUT();
	SDA = 1;
	delay();
	SCL = 1;
	delay();
	SCL = 0;
	delay();	
}

bool I2c_WriteByte(unsigned char slave_addr, unsigned char reg_addr,unsigned char const dat)
{
	if(I2C_Start())
	{
		I2C_SendByte(slave_addr<<1);//�����豸��ַ
		if(I2C_WaitAck()==false)	//�������Ӧ��
		{
			I2C_Stop();
			return false;
		}
		I2C_SendByte(reg_addr);		//����Ŀ��Ĵ�����ַ������Ϊд
		I2C_WaitAck();				//����ǰ���Ѽ���Ӧ���źţ��˴����ظ����
		I2C_Stop();
		return true;
	}
	return false;
}

int i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data)
{
	uint8_t count;
	if(I2C_Start())
	{
		I2C_SendByte(slave_addr<<1);//�����豸��ַ
		if(I2C_WaitAck()==false)	//�������Ӧ��
		{
			I2C_Stop();
			return 1;
		}
		I2C_SendByte(reg_addr);		//����Ŀ��Ĵ�����ַ������Ϊд
		I2C_WaitAck();				//����ǰ���Ѽ���Ӧ���źţ��˴����ظ����
		for(count=0;count<length;count++)
		{
			I2C_SendByte(data[count]);	//��������
			if(I2C_WaitAck()==false)	//���Ŀ���豸ֹͣ����
			{
				I2C_Stop();
				return 0;
			}
		}
		I2C_Stop();
		return 0;
	}
	return 1;
}


int i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data)
{
	if(I2C_Start())
	{
		I2C_SendByte(slave_addr<<1);//�����豸��ַ������Ϊд
		if(I2C_WaitAck()==false)	//�������Ӧ��
		{
			I2C_Stop();
			return 1;
		}
		I2C_SendByte(reg_addr);		//����Ŀ��Ĵ�����ַ������Ϊд
		I2C_WaitAck();				//����ǰ���Ѽ���Ӧ���źţ��˴����ظ����
		I2C_Start();				//���·�����ʼ�źţ��ı����ݴ��䷽��
		I2C_SendByte((slave_addr<<1)+1);	//�����豸��ַ������Ϊ��
		I2C_WaitAck();
		for(;length>0;length--)
		{
			*data = I2C_ReadByte();
			if(length==1)
				I2C_NAck();
			else
				I2C_Ack();
			data++;
		}
		I2C_Stop();
		return 0;
	}
	return 1;
}
