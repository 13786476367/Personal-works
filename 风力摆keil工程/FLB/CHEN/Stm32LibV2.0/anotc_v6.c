#include "anotc_v6.h"
#include <stdarg.h>

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
//д��Э��ͷ
static __INLINE void ANOTC_V6_head(pANOTC obj,uint8_t mode)
{
	uint8_t *dptr=(uint8_t*)&obj->buff[obj->size];
	dptr[0] = 0xaa;	
	if(obj->drive_name==0)
		dptr[1] = 1;
	else
		dptr[1] = obj->drive_name;		//�����豸����
	dptr[2] = 0xaf;
	dptr[3] = mode;					//д�빦����
	obj->size += 5;
}


//дһ���ֽ�
static void ANOTC_WriteByte(pANOTC obj,uint8_t *dat)
{
	obj->buff[obj->size++]=*dat;
}

//д�����ֽ�
static void ANOTC_WriteWord(pANOTC obj,uint16_t *dat)
{
	obj->buff[obj->size++]=BYTE1(*dat);
	obj->buff[obj->size++]=BYTE0(*dat);
}

//д�ĸ��ֽ�
static void ANOTC_WriteDWord(pANOTC obj,uint32_t *dat)
{
	obj->buff[obj->size++]=BYTE3(*dat);
	obj->buff[obj->size++]=BYTE2(*dat);
	obj->buff[obj->size++]=BYTE1(*dat);
	obj->buff[obj->size++]=BYTE0(*dat);	
}

//д��У��
static __INLINE void ANOTC_V6_WriteSum(pANOTC obj,uint8_t start)
{
	uint8_t sum=0;
	obj->buff[start+4] = obj->size-5-start;
	for(uint8_t i=start;i!=obj->size;i++)
	{
		sum += obj->buff[i];
	}
	obj->buff[obj->size++] = sum;
}


int ANOTC_V6_Printf(uint8_t mode,pANOTC obj,const char *format,...)
{
	uint8_t start=obj->size;
	va_list argp;
	va_start(argp,format);
	ANOTC_V6_head(obj,mode);//д��Э��ͷ
	
	while(*format)
	{
		switch(*format++)		//���Э������
		{
			case 'h'://short����
			{
				short dat = va_arg(argp,int);
				ANOTC_WriteWord(obj,(uint16_t*)&dat);
			}break;
			case 'd'://int����
			{
				int dat = va_arg(argp,int);
				ANOTC_WriteDWord(obj,(uint32_t*)&dat);
			}break;
			case 'c'://char����	
			{
				uint8_t dat = va_arg(argp,int);
				ANOTC_WriteByte(obj,(uint8_t*)&dat);
			}break;
			case 'f'://float����
			{
				short dat = va_arg(argp,double)*100;
				ANOTC_WriteWord(obj,(uint16_t*)&dat);
			}break;
			case 'u'://uint16����
			{
				uint16_t dat = va_arg(argp,int)*100;
				ANOTC_WriteWord(obj,(uint16_t*)&dat);
			}break;
		}
	}
	ANOTC_V6_WriteSum(obj,start);//д��У��
	va_end(argp);
	start = obj->size-start;
	return start;
}
