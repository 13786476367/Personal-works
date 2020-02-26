#include "pid.h"

//限幅
short constrain(short source ,short min ,short max)
{
	if(source > max)
		return max;
	else if(source < min)
		return min;
	else
		return source;
}

void PidInit(pHPID_CTR obj,short expect,float Kp,float Ki,float Kd)
{
	static uint8_t no=0;
	
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
short PidControl(pHPID_CTR obj,short input)
{
	obj->Data.error_Kd = obj->Data.error_Kp;	//计算误差D
	
	obj->Data.error_Kp = obj->expect - input;	//计算误差P
	
	obj->Data.error_Ki += obj->Data.error_Kp;	//计算误差I
	
	PidControlHeadCallBack(obj);
	
	//计算输出期望P
	obj->feedback.back_Kp = obj->Data.error_Kp * obj->Argv.Kp;	
	//计算输出期望I	
	obj->feedback.back_Ki = obj->Data.error_Ki * obj->Argv.Ki;	
	//计算输出期望D
	obj->feedback.back_Kd = (obj->Data.error_Kp-obj->Data.error_Kd) * obj->Argv.Kd;
	
	return PidControlCallBack(obj);
}

//融合PID控制器
short PidFuseControl(pHPID_CTR obj,short inputP,short inputD)
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
short PidDoubleControl(pHDPID_CTR obj,short Oninput,short Ininput)
{
	
	//将外环输出作为内环期望
	obj->In.expect = PidControl(&obj->Out,Oninput);
	
	//内环处理
	return PidControl(&obj->In,Ininput);			//将内环输出值返回
}

//对三个PID的误差值进行处理
__weak void PidControlHeadCallBack(pHPID_CTR obj)
{
	switch(obj->no)
	{
		case 0:
			if(obj->Data.error_Kp>(3000+obj->expect)||obj->Data.error_Kp<(-3000+obj->expect))
				obj->Data.error_Ki = 0;
			else
				obj->Data.error_Ki = constrain(obj->Data.error_Ki,-15000,15000);
			break;
		default:break;
	}
}
//三个PID的输出值进行处理
__weak short PidControlCallBack(pHPID_CTR obj)
{
	switch(obj->no)
	{
		case 0:
			return constrain(obj->feedback.back_Kp+obj->feedback.back_Ki+obj->feedback.back_Kd,-7000,7000);
		default:
			return constrain(obj->feedback.back_Kp+obj->feedback.back_Ki+obj->feedback.back_Kd,-7000,7000);
	}
}
