#include "oled.h"
#include "oledfont.h"
#include <stdarg.h>

#define OLED_SCL 	PBout(5)		//D0(SCL)
#define OLED_SDA 	PBout(4)		//D1(SDA)
#define OLED_RST	PBout(3)		//RST
#define OLED_DC		PAout(15)		//D/C
#define OLED_CS		PCout(12)		//CS

#define delay_ms delay_ms


//显示屏坐标
static uint8_t g_X;
static uint8_t g_Y;


//向OLED写入一个字节命令
static __INLINE void OLED_WriteCmd(uint8_t cmd)
{
	uint8_t i;
	OLED_CS = 0;
	OLED_DC = 0;
	for(i=0;i<8;i++)
	{
		OLED_SCL=0;
		if(cmd&0x80)
			OLED_SDA=1;
		else
			OLED_SDA=0;
		OLED_SCL=1;
		cmd<<=1;			
	}
	OLED_CS = 1;
}
//向OLED写入一个字节数据
static __INLINE void OLED_WriteData(uint8_t dat)
{
	uint8_t i;
	OLED_CS = 0;
	OLED_DC = 1;
	for(i=0;i<8;i++)
	{
		OLED_SCL=0;
		if(dat&0x80)
			OLED_SDA=1;
		else
			OLED_SDA=0;
		OLED_SCL=1;
		dat<<=1;			
	}
	OLED_CS = 1;
}

//OLED初始化
void OLED_Init(void)
{
	OLED_RST=0;				//复位OLED
	delay_ms(100);
	OLED_RST=1;
	OLED_WriteCmd(0xAE);	//关闭显示
	OLED_WriteCmd(0xD5);	//设置时钟分频因子
	OLED_WriteCmd(80);		//[3:0],分频因子;[7:4],震荡频率
	OLED_WriteCmd(0xA8);	//设置驱动路数
	OLED_WriteCmd(0x3F);	//默认0X3F(1/64)
	OLED_WriteCmd(0xD3);	//设置默认偏倚
	OLED_WriteCmd(0x00);	//默认为0
	OLED_WriteCmd(0x40);	//设置显示开始行[5:0],行数
	OLED_WriteCmd(0x8D);	//电荷泵设置
	OLED_WriteCmd(0x14); 	//bit2，开启/关闭
	OLED_WriteCmd(0x20); 	//设置内存地址模式
	OLED_WriteCmd(0x02); 	//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WriteCmd(0xA1); 	//段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WriteCmd(0xC0); 	//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WriteCmd(0xDA); 	//设置COM硬件引脚配置
	OLED_WriteCmd(0x12); 	//[5:4]配置		 
	OLED_WriteCmd(0x81); 	//对比度设置
	OLED_WriteCmd(0xEF); 	//1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WriteCmd(0xD9); 	//设置预充电周期
	OLED_WriteCmd(0xf1); 	//[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WriteCmd(0xDB); 	//设置VCOMH 电压倍率
	OLED_WriteCmd(0x30); 	//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	OLED_WriteCmd(0xA4); 	//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WriteCmd(0xA6); 	//设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WriteCmd(0xAF); 	//开启显示

	OLED_ClearAll();
}

//输出ASCII字符
int OLED_Putc(int ch)
{
	switch(ch)
	{
		default:
		{
			uint8_t dat = ch-' ';
			if(dat>'~')			//如果是非ASCII字符
			{
				return 0;
			}
			else
			{
				const uint8_t (*dptr)[8] = (const uint8_t (*)[8])ascii[dat];
				if(g_X >= 16)	//显示位置+1		
				{
					g_X=0;
					g_Y++; 
				}
				if(g_Y>=4)		//超出显示范围
				{
					return -1;
				}
				
				for(uint8_t i=0;i<2;i++)
				{
					if(i==0)	//绘制字符上半部分
					{
						OLED_WriteCmd(0xb7-g_Y*2);
					}
					else		//绘制字符下半部分
					{
						OLED_WriteCmd(0xb6-g_Y*2);
					}
					OLED_WriteCmd((g_X*8)&0x0f);      			//设置显示位置—列低地址
					OLED_WriteCmd(((g_X*8)>>4)|0x10);      		//设置显示位置—列高地址
					for(uint8_t j=0;j<8;j++)
					{
						OLED_WriteData(dptr[i][j]);
					}
				}
				g_X++;
			}
			
		}
	}
	return ch;
}

//使用中文字模显示汉字
int OLED_GB2312(const uint8_t font[][16],const uint8_t n)
{
	for(uint8_t i=0;i<n;i++)
	{
		//对当前显示位置进行检查,确保有该行有足够的位置放下一个中文字
		if(g_X>14)
		{
			g_X=0;		
			g_Y++;		//换下一行
		}
		if(g_Y>=4)
		{
			return -1;
		}
		else
		{
			
			for(uint8_t j=0;j<2;j++)
			{
				if(j==0)	//绘制字符上半部分
				{
					OLED_WriteCmd(0xb7-g_Y*2);
				}
				else		//绘制字符下半部分
				{
					OLED_WriteCmd(0xb6-g_Y*2);
				}
				OLED_WriteCmd((g_X*8)&0x0f);      			//设置显示位置—列低地址
				OLED_WriteCmd(((g_X*8)>>4)|0x10);      		//设置显示位置—列高地址
				for(uint8_t k=0;k<16;k++)
				{
					OLED_WriteData(font[i+j][k]);
				}
			}
			g_X+=2;
		}
	}
	return n;
}

//设置写入位置(行,列)
bool OLED_SetXY(uint8_t line,uint8_t list)
{
	if(list>=16||line>=4)
	{
		return false;
	}
	else
	{
		g_X = list;
		g_Y = line;
		return true;
	}
}

//清除指定位置指定长度的字符
void OLED_ClearXY(const uint8_t line,const uint8_t list,const uint8_t n)
{
	if(OLED_SetXY(line,list))		//设置光标到指定位置
	{
		for(uint8_t i=0;i<n;i++)
		{
			OLED_Putc(' ');
		}
		OLED_SetXY(line,list);		//设置光标到指定位置
	}
}

//相当于回删
void OLED_BackSpace(uint8_t sz)
{
	uint8_t x;
	if(g_X>=sz)
	{
		g_X-=sz;
	}
	else
	{
		sz = g_X;
		g_X = 0;
	}
	x = g_X;
	for(uint8_t i=0;i<sz;i++)
	{
		OLED_Putc(' ');
	}
	g_X = x;
}

//清除一行
void OLED_ClearLine(const uint8_t line)
{
	if(OLED_SetXY(line,0))
	{
		for(uint8_t i=0;i<16;i++)
		{
			OLED_Putc(' ');
		}
		g_X=0;
	}
}

//清除屏幕
void OLED_ClearAll(void)
{
	g_X=0;
	g_Y=0;
	for(uint8_t i=0;i<64;i++)
	{
		OLED_Putc(' ');
	}
	g_X=0;
	g_Y=0;
}
//不带显示位置的字符串输出函数
int OLED_PutString(const char *str)
{
	uint8_t n=0;
	for(uint8_t i=0;str[i]!='\0';i++)
	{
		if((uint8_t)str[i]>0x7f)	//中文
		{
			#ifdef USING_IN_GB2312	//使用内部字库
			uint32_t point;
			point = ((str[i]-0xa1)*94+str[i+1]-0xa1)*2;
			if(OLED_GB2312(&gb2312[point],1)!=-1)
			{
				n+=2;
			}
			else
			{
				break;
			}
			#else 
			#ifdef USING_EX_GB2312	//使用外部字库
			uint8_t buff[2][16];
			FLASH_PageRead((uint8_t*)buff,FontAddr+((str[i]-0xa1)*94+str[i+1]-0xa1)*32,32);	
			if(OLED_GB2312((const uint8_t (*)[16])buff,1)!=-1)
			{
				n+=2;
			}
			else
			{
				break;
			}
			
			#endif //USING_EX_GB2312
			#endif //USING_IN_GB2312
			i++;
		}
		else					//英文
		{
			if(OLED_Putc(str[i])!=-1)
			{
				n++;
			}
			else
			{
				break;
			}
		}	
	}
	return n;
}

//带显示位置的字符串输出函数
int OLED_PutStringAt(uint8_t line,uint8_t list,const char *str)
{
	uint8_t x,y;
	x=g_X;
	y=g_Y;
	if(OLED_SetXY(line,list))
	{
		int n;
		uint8_t sz=16-list;
		char buff[17];
		strncpy(buff,str,sz);
		buff[sz]='\0';
		n=OLED_PutString(buff);
		g_X=x;
		g_Y=y;
		return n;
	}
	return -1;
}

//不带显示位置的printf函数重定义
int OLED_Printf(const char *format,...)
{
	char buff[64];
	va_list args;
	va_start(args,format);
	vsnprintf(buff,sizeof(buff),format,args);
	return OLED_PutString(buff);
}
//带显示位置的printf函数重定义
int OLED_PrintfAt(uint8_t line,uint8_t list,const char *format,...)
{
	char buff[17];
	va_list args;
	va_start(args,format);
	vsnprintf(buff,sizeof(buff),format,args);
	return OLED_PutStringAt(line,list,buff);
}

