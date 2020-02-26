/********************************************************
			湖南工程学院创新基地
			作者:CHEN
			技术支持:QQ742010094
			日期:2019.7.2
			匿名飞控V6通讯协议
*********************************************************/
#ifndef __ANOTC_V6_H
#define __ANOTC_V6_H
#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

typedef struct				//ANOTC协议对象
{
	uint8_t buff[256];		//帧数据缓冲区
	uint8_t size;			//数据长度
	uint8_t drive_name;		//设备名称
}ANOTC,*const pANOTC;

int ANOTC_V6_Printf(uint8_t mode,pANOTC obj,const char *format,...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__ANOTC_V6_H
