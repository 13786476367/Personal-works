/********************************************************
			���Ϲ���ѧԺ���»���
			����:CHEN
			����֧��:QQ742010094
			����:2019.7.2
			�����ɿ�V6ͨѶЭ��
*********************************************************/
#ifndef __ANOTC_V6_H
#define __ANOTC_V6_H
#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

typedef struct				//ANOTCЭ�����
{
	uint8_t buff[256];		//֡���ݻ�����
	uint8_t size;			//���ݳ���
	uint8_t drive_name;		//�豸����
}ANOTC,*const pANOTC;

int ANOTC_V6_Printf(uint8_t mode,pANOTC obj,const char *format,...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__ANOTC_V6_H
