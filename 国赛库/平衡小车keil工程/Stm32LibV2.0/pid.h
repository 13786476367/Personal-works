/********************************************************
			���Ϲ���ѧԺ���»���
			����:CHEN
			����֧��:QQ742010094
			����:2019.1.1
*********************************************************/
#ifndef __PID_H
#define __PID_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

typedef struct			//PID�����ṹ��
{
	float Kp;
	float Ki;
	float Kd;
}PID_ARGV,*const pPID_ARGV;

typedef struct			//PID���ݽṹ��
{
	int error_Kp;
	int error_Ki;
	int error_Kd;
}PID_DATA,*const pPID_DATA;

typedef struct			//PID�������ݽṹ��
{
	int back_Kp;
	int back_Ki;
	int back_Kd;
}PID_FEEDBACK,*const pPID_FEEDBACK;

typedef struct 				//����PID���
{
	uint8_t no;				//PIDΨһ���
	int expect;			//PID�������
	PID_ARGV Argv;			//PID����
	PID_DATA Data;			//PID����
	PID_FEEDBACK feedback;	//PID��������
}HPID_CTR,*const pHPID_CTR;

typedef struct			//����PID���			
{
	HPID_CTR Out;		//�⻷
	HPID_CTR In;		//�ڻ�
}HDPID_CTR,*const pHDPID_CTR;



//�޷�����
int constrain(int source ,int min ,int max);
//��ʼ��PID���󣬴���ϵͳ���ʼ������
void PidInit(pHPID_CTR obj,int expect,float Kp,float Ki,float Kd);
//����PID����
int PidControl(pHPID_CTR obj,int input);
//�ں�PID������
int PidFuseControl(pHPID_CTR obj,int inputP,int inputD);
//����PID���ƣ���������ʱ�����⻷����Ϊ0���������ڻ��������⻷��
int PidDoubleControl(pHDPID_CTR obj,int Oninput,int Ininput);
//PID�ص����������ڲ�ͬϵͳ��ֻ��Ҫ�ı��������ص��������ɣ�
void PidControlHeadCallBack(pHPID_CTR obj);
int PidControlCallBack(pHPID_CTR obj);		

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__PID_H
