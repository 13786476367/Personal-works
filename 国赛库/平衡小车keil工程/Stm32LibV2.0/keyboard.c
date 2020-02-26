#include "keyboard.h"


#define SCL			PBout(6)			//SCL����
#define SDA			PBout(7)			//SDA����
#define READ_SDA	PBin(7)
#define INT			PBin(8)
//IO�������ã����������
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8u<<(4*7);}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3u<<(4*7);}
#define SCL_OUT() {GPIOB->CRL&=0XF0FFFFFF;GPIOB->CRL|=3u<<(4*6);}
#define delay()	delay_us(2)


__inline static bool I2C_Start()
{
	SDA_OUT();     //sda�����
	SDA=1;
	if(!READ_SDA)
		return false;	
	SCL=1;
	delay();
 	SDA=0;//START:when CLK is high,DATA change form high to low 
	if(READ_SDA)
		return false;
	delay();
	SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	return true;
}

__inline static void I2C_Stop(void)
{
	SDA_OUT();//sda�����
	SCL=0;
	SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay();
	SCL=1; 
	SDA=1;//����I2C���߽����ź�
	delay();							   	
}

__inline static void I2C_Ack(void)
{
	SCL=0;
	SDA_OUT();
	SDA=0;
	delay();
	SCL=1;
	delay();
	SCL=0;
	delay();
	SDA=1;
}


__inline static void I2C_SendByte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		SDA = (txd&0x80)!=0?1:0;
        txd<<=1; 	  
		delay();   
		SCL=1;
		delay(); 
		SCL=0;	
		delay();
    }	 
}

__inline static uint8_t I2C_ReadByte()
{
	SDA_IN();//SDA����Ϊ����
	unsigned char i,receive=0;
    for(i=0;i<8;i++ )
	{
        SCL=0; 
        delay();
		SCL=1;
        receive<<=1;
        if(READ_SDA)
			receive++;   
		delay(); 
    }
	SCL=0; 	
    return receive;
}
bool KEY_Init(void)
{
	SCL_OUT();
	SDA_OUT();
	if(!I2C_Start())
		return false;
	I2C_SendByte(0x48);
	I2C_Ack();
	I2C_SendByte(9);
	I2C_Ack();
	I2C_Stop();
	return true;
}

//����Ƿ��а�����Ϣ
bool KEY_ReadINT(void)
{
	if(INT)
		return false;
	else
		return true;
}

//��ȡ��ֵ��
//�������δ�ɿ�����ture���򷵻�false
bool KEY_ReadData(uint8_t *data)
{
	bool ret;
	uint8_t key;
	I2C_Start();
	I2C_SendByte(0x4f);					//�����ֽ�1
	I2C_Ack();							//����Ӧ���ź�
	key = I2C_ReadByte();				//��ȡ��ֵ��
	I2C_Ack();							//����Ӧ���ź�
	I2C_Stop();							//����ֹͣ�ź�
	if((key&0x40)&&key!=0xff)
		ret = true;						//����δ�ɿ�
	else
		ret = false;
	key &= 0xbf;						//10111111b
	if(data != NULL)
		*data = key;
	return ret;
}

//����ʽ��ȡ��ֵ��
uint8_t KEY_GetKey(void)
{
	uint8_t key;
	while(KEY_ReadINT()==false);
	//�ȴ������ɿ�
	while(KEY_ReadData(&key));
	while(KEY_ReadINT()!=false);
	return key;
}

//������ת����
int KEY_Num(uint8_t key)
{
	switch(key)
	{
		case KEY_NUM0:return '0';
		case KEY_NUM1:return '1';
		case KEY_NUM2:return '2';
		case KEY_NUM3:return '3';
		case KEY_NUM4:return '4';
		case KEY_NUM5:return '5';
		case KEY_NUM6:return '6';
		case KEY_NUM7:return '7';
		case KEY_NUM8:return '8';
		case KEY_NUM9:return '9';
		default:return key;
	}
}
