/********************************************************
			湖南工程学院创新基地
			作者:CHEN
			技术支持:QQ742010094
			日期:2019.1.1
*********************************************************/
#ifndef __PID_H
#define __PID_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

typedef struct			//PID参数结构体
{
	float Kp;
	float Ki;
	float Kd;
}PID_ARGV,*const pPID_ARGV;

typedef struct			//PID数据结构体
{
	int error_Kp;
	int error_Ki;
	int error_Kd;
}PID_DATA,*const pPID_DATA;

typedef struct			//PID反馈数据结构体
{
	int back_Kp;
	int back_Ki;
	int back_Kd;
}PID_FEEDBACK,*const pPID_FEEDBACK;

typedef struct 				//单级PID句柄
{
	uint8_t no;				//PID唯一标号
	int expect;			//PID输出期望
	PID_ARGV Argv;			//PID参数
	PID_DATA Data;			//PID数据
	PID_FEEDBACK feedback;	//PID各环反馈
}HPID_CTR,*const pHPID_CTR;

typedef struct			//串级PID句柄			
{
	HPID_CTR Out;		//外环
	HPID_CTR In;		//内环
}HDPID_CTR,*const pHDPID_CTR;



//限幅函数
int constrain(int source ,int min ,int max);
//初始化PID对象，串级系统需初始化两次
void PidInit(pHPID_CTR obj,int expect,float Kp,float Ki,float Kd);
//单级PID控制
int PidControl(pHPID_CTR obj,int input);
//融合PID控制器
int PidFuseControl(pHPID_CTR obj,int inputP,int inputD);
//串级PID控制（整定参数时，令外环输入为0，先整定内环后整定外环）
int PidDoubleControl(pHDPID_CTR obj,int Oninput,int Ininput);
//PID回调函数（对于不同系统，只需要改变这两个回调函数即可）
void PidControlHeadCallBack(pHPID_CTR obj);
int PidControlCallBack(pHPID_CTR obj);		

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__PID_H
