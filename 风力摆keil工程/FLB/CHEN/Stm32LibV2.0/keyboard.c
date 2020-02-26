#include "keyboard.h"


#define SCL			PBout(6)			//SCL引脚
#define SDA			PBout(7)			//SDA引脚
#define READ_SDA	PBin(7)
#define INT			PBin(8)
//IO方向设置（推挽输出）
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8u<<(4*7);}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3u<<(4*7);}
#define SCL_OUT() {GPIOB->CRL&=0XF0FFFFFF;GPIOB->CRL|=3u<<(4*6);}
#define delay()	delay_us(2)


__inline static bool I2C_Start()
{
	SDA_OUT();     //sda线输出
	SDA=1;
	if(!READ_SDA)
		return false;	
	SCL=1;
	delay();
 	SDA=0;//START:when CLK is high,DATA change form high to low 
	if(READ_SDA)
		return false;
	delay();
	SCL=0;//钳住I2C总线，准备发送或接收数据 
	return true;
}

__inline static void I2C_Stop(void)
{
	SDA_OUT();//sda线输出
	SCL=0;
	SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay();
	SCL=1; 
	SDA=1;//发送I2C总线结束信号
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
    SCL=0;//拉低时钟开始数据传输
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
	SDA_IN();//SDA设置为输入
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

//检查是否有按键消息
bool KEY_ReadINT(void)
{
	if(INT)
		return false;
	else
		return true;
}

//读取键值码
//如果按键未松开返回ture否则返回false
bool KEY_ReadData(uint8_t *data)
{
	bool ret;
	uint8_t key;
	I2C_Start();
	I2C_SendByte(0x4f);					//发送字节1
	I2C_Ack();							//发送应答信号
	key = I2C_ReadByte();				//读取键值码
	I2C_Ack();							//发送应答信号
	I2C_Stop();							//发送停止信号
	if((key&0x40)&&key!=0xff)
		ret = true;						//按键未松开
	else
		ret = false;
	key &= 0xbf;						//10111111b
	if(data != NULL)
		*data = key;
	return ret;
}

//阻塞式读取键值码
uint8_t KEY_GetKey(void)
{
	uint8_t key;
	while(KEY_ReadINT()==false);
	//等待按键松开
	while(KEY_ReadData(&key));
	while(KEY_ReadINT()!=false);
	return key;
}

//键盘码转数字
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
