#include "oled.h"
#include "oledfont.h"
#include <stdarg.h>

#define OLED_SCL 	PBout(5)		//D0(SCL)
#define OLED_SDA 	PBout(4)		//D1(SDA)
#define OLED_RST	PBout(3)		//RST
#define OLED_DC		PAout(15)		//D/C
#define OLED_CS		PCout(12)		//CS

#define delay_ms delay_ms


//��ʾ������
static uint8_t g_X;
static uint8_t g_Y;


//��OLEDд��һ���ֽ�����
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
//��OLEDд��һ���ֽ�����
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

//OLED��ʼ��
void OLED_Init(void)
{
	OLED_RST=0;				//��λOLED
	delay_ms(100);
	OLED_RST=1;
	OLED_WriteCmd(0xAE);	//�ر���ʾ
	OLED_WriteCmd(0xD5);	//����ʱ�ӷ�Ƶ����
	OLED_WriteCmd(80);		//[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WriteCmd(0xA8);	//��������·��
	OLED_WriteCmd(0x3F);	//Ĭ��0X3F(1/64)
	OLED_WriteCmd(0xD3);	//����Ĭ��ƫ��
	OLED_WriteCmd(0x00);	//Ĭ��Ϊ0
	OLED_WriteCmd(0x40);	//������ʾ��ʼ��[5:0],����
	OLED_WriteCmd(0x8D);	//��ɱ�����
	OLED_WriteCmd(0x14); 	//bit2������/�ر�
	OLED_WriteCmd(0x20); 	//�����ڴ��ַģʽ
	OLED_WriteCmd(0x02); 	//[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WriteCmd(0xA1); 	//���ض�������,bit0:0,0->0;1,0->127;
	OLED_WriteCmd(0xC0); 	//����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WriteCmd(0xDA); 	//����COMӲ����������
	OLED_WriteCmd(0x12); 	//[5:4]����		 
	OLED_WriteCmd(0x81); 	//�Աȶ�����
	OLED_WriteCmd(0xEF); 	//1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WriteCmd(0xD9); 	//����Ԥ�������
	OLED_WriteCmd(0xf1); 	//[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WriteCmd(0xDB); 	//����VCOMH ��ѹ����
	OLED_WriteCmd(0x30); 	//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	OLED_WriteCmd(0xA4); 	//ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WriteCmd(0xA6); 	//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WriteCmd(0xAF); 	//������ʾ

	OLED_ClearAll();
}

//���ASCII�ַ�
int OLED_Putc(int ch)
{
	switch(ch)
	{
		default:
		{
			uint8_t dat = ch-' ';
			if(dat>'~')			//����Ƿ�ASCII�ַ�
			{
				return 0;
			}
			else
			{
				const uint8_t (*dptr)[8] = (const uint8_t (*)[8])ascii[dat];
				if(g_X >= 16)	//��ʾλ��+1		
				{
					g_X=0;
					g_Y++; 
				}
				if(g_Y>=4)		//������ʾ��Χ
				{
					return -1;
				}
				
				for(uint8_t i=0;i<2;i++)
				{
					if(i==0)	//�����ַ��ϰ벿��
					{
						OLED_WriteCmd(0xb7-g_Y*2);
					}
					else		//�����ַ��°벿��
					{
						OLED_WriteCmd(0xb6-g_Y*2);
					}
					OLED_WriteCmd((g_X*8)&0x0f);      			//������ʾλ�á��е͵�ַ
					OLED_WriteCmd(((g_X*8)>>4)|0x10);      		//������ʾλ�á��иߵ�ַ
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

//ʹ��������ģ��ʾ����
int OLED_GB2312(const uint8_t font[][16],const uint8_t n)
{
	for(uint8_t i=0;i<n;i++)
	{
		//�Ե�ǰ��ʾλ�ý��м��,ȷ���и������㹻��λ�÷���һ��������
		if(g_X>14)
		{
			g_X=0;		
			g_Y++;		//����һ��
		}
		if(g_Y>=4)
		{
			return -1;
		}
		else
		{
			
			for(uint8_t j=0;j<2;j++)
			{
				if(j==0)	//�����ַ��ϰ벿��
				{
					OLED_WriteCmd(0xb7-g_Y*2);
				}
				else		//�����ַ��°벿��
				{
					OLED_WriteCmd(0xb6-g_Y*2);
				}
				OLED_WriteCmd((g_X*8)&0x0f);      			//������ʾλ�á��е͵�ַ
				OLED_WriteCmd(((g_X*8)>>4)|0x10);      		//������ʾλ�á��иߵ�ַ
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

//����д��λ��(��,��)
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

//���ָ��λ��ָ�����ȵ��ַ�
void OLED_ClearXY(const uint8_t line,const uint8_t list,const uint8_t n)
{
	if(OLED_SetXY(line,list))		//���ù�굽ָ��λ��
	{
		for(uint8_t i=0;i<n;i++)
		{
			OLED_Putc(' ');
		}
		OLED_SetXY(line,list);		//���ù�굽ָ��λ��
	}
}

//�൱�ڻ�ɾ
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

//���һ��
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

//�����Ļ
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
//������ʾλ�õ��ַ����������
int OLED_PutString(const char *str)
{
	uint8_t n=0;
	for(uint8_t i=0;str[i]!='\0';i++)
	{
		if((uint8_t)str[i]>0x7f)	//����
		{
			#ifdef USING_IN_GB2312	//ʹ���ڲ��ֿ�
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
			#ifdef USING_EX_GB2312	//ʹ���ⲿ�ֿ�
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
		else					//Ӣ��
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

//����ʾλ�õ��ַ����������
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

//������ʾλ�õ�printf�����ض���
int OLED_Printf(const char *format,...)
{
	char buff[64];
	va_list args;
	va_start(args,format);
	vsnprintf(buff,sizeof(buff),format,args);
	return OLED_PutString(buff);
}
//����ʾλ�õ�printf�����ض���
int OLED_PrintfAt(uint8_t line,uint8_t list,const char *format,...)
{
	char buff[17];
	va_list args;
	va_start(args,format);
	vsnprintf(buff,sizeof(buff),format,args);
	return OLED_PutStringAt(line,list,buff);
}

