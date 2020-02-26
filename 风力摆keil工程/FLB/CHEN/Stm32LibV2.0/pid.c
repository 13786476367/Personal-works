#include "pid.h"

//控制对象编号
static uint8_t no=0;


//限幅 
int constrain(int source ,int min ,int max)
{
	if(source > max)
		return max;
	else if(source < min)
		return min;
	else
		return source;
}
//初始化PID控制对象
void PidInit(pHPID_CTR obj,int expect,float Kp,float Ki,float Kd)
{
	obj->no = no++;			//根据初始化顺序生成控制系统标号
	obj->expect = expect;
	obj->Argv.Kp = Kp;
	obj->Argv.Ki = Ki;
	obj->Argv.Kd = Kd;
	
	obj->Data.error_Kp = 0;
	obj->Data.error_Ki = 0;
	obj->Data.error_Kd = 0;

}

//单级PID控制
int PidControl(pHPID_CTR obj,int input)
{
	//计算本次偏差
	int error = obj->expect - input;					
	//计算误差D
	obj->Data.error_Kd =  error - obj->Data.error_Kp;	
	//计算误差P
	obj->Data.error_Kp = error;							
	//计算误差I
	obj->Data.error_Ki += error;	
	
	PidControlHeadCallBack(obj);
	
	//计算输出期望P
	obj->feedback.back_Kp = obj->Data.error_Kp * obj->Argv.Kp;	
	//计算输出期望I	
	obj->feedback.back_Ki = obj->Data.error_Ki * obj->Argv.Ki;	
	//计算输出期望D
	obj->feedback.back_Kd = obj->Data.error_Kd  * obj->Argv.Kd;
	
	return PidControlCallBack(obj);
}



//融合PID控制器
int PidFuseControl(pHPID_CTR obj,int inputP,int inputD)
{	
	obj->Data.error_Kp = obj->expect - inputP;	//计算误差P
	
	obj->Data.error_Ki += obj->Data.error_Kp;	//计算误差I
	
	obj->Data.error_Kd = 0-inputD;	//计算误差D
	
	PidControlHeadCallBack(obj);
	
	//计算输出期望P
	obj->feedback.back_Kp = obj->Data.error_Kp * obj->Argv.Kp;	
	//计算输出期望I	
	obj->feedback.back_Ki = obj->Data.error_Ki * obj->Argv.Ki;	
	//计算输出期望D
	obj->feedback.back_Kd = obj->Data.error_Kd * obj->Argv.Kd;
	
	return PidControlCallBack(obj);
}



//串级PID控制
int PidDoubleControl(pHDPID_CTR obj,int Oninput,int Ininput)
{
	//将外环输出作为内环期望
	obj->In.expect = PidControl(&obj->Out,Oninput);
	
	//内环处理
	return PidControl(&obj->In,Ininput);			//将内环输出值返回
}


//对三个PID的误差值进行处理
__weak void PidControlHeadCallBack(pHPID_CTR obj)
{
	obj->Data.error_Ki = constrain(obj->Data.error_Ki,-1000,1000);
}
//三个PID的输出值进行处理
__weak int PidControlCallBack(pHPID_CTR obj)
{
	return constrain(obj->feedback.back_Kp+obj->feedback.back_Ki+obj->feedback.back_Kd,-5500,5500);
}
