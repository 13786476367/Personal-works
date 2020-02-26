#include "anotc_v6.h"
#include <stdarg.h>

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
//写入协议头
static __INLINE void ANOTC_V6_head(pANOTC obj,uint8_t mode)
{
	uint8_t *dptr=(uint8_t*)&obj->buff[obj->size];
	dptr[0] = 0xaa;	
	if(obj->drive_name==0)
		dptr[1] = 1;
	else
		dptr[1] = obj->drive_name;		//匿名设备名称
	dptr[2] = 0xaf;
	dptr[3] = mode;					//写入功能字
	obj->size += 5;
}


//写一个字节
static void ANOTC_WriteByte(pANOTC obj,uint8_t *dat)
{
	obj->buff[obj->size++]=*dat;
}

//写两个字节
static void ANOTC_WriteWord(pANOTC obj,uint16_t *dat)
{
	obj->buff[obj->size++]=BYTE1(*dat);
	obj->buff[obj->size++]=BYTE0(*dat);
}

//写四个字节
static void ANOTC_WriteDWord(pANOTC obj,uint32_t *dat)
{
	obj->buff[obj->size++]=BYTE3(*dat);
	obj->buff[obj->size++]=BYTE2(*dat);
	obj->buff[obj->size++]=BYTE1(*dat);
	obj->buff[obj->size++]=BYTE0(*dat);	
}

//写入校验
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
	ANOTC_V6_head(obj,mode);//写入协议头
	
	while(*format)
	{
		switch(*format++)		//填充协议内容
		{
			case 'h'://short类型
			{
				short dat = va_arg(argp,int);
				ANOTC_WriteWord(obj,(uint16_t*)&dat);
			}break;
			case 'd'://int类型
			{
				int dat = va_arg(argp,int);
				ANOTC_WriteDWord(obj,(uint32_t*)&dat);
			}break;
			case 'c'://char类型	
			{
				uint8_t dat = va_arg(argp,int);
				ANOTC_WriteByte(obj,(uint8_t*)&dat);
			}break;
			case 'f'://float类型
			{
				short dat = va_arg(argp,double)*100;
				ANOTC_WriteWord(obj,(uint16_t*)&dat);
			}break;
			case 'u'://uint16类型
			{
				uint16_t dat = va_arg(argp,int)*100;
				ANOTC_WriteWord(obj,(uint16_t*)&dat);
			}break;
		}
	}
	ANOTC_V6_WriteSum(obj,start);//写入校验
	va_end(argp);
	start = obj->size-start;
	return start;
}
