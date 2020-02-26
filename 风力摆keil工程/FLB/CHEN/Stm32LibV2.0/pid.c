#include "pid.h"

//���ƶ�����
static uint8_t no=0;


//�޷� 
int constrain(int source ,int min ,int max)
{
	if(source > max)
		return max;
	else if(source < min)
		return min;
	else
		return source;
}
//��ʼ��PID���ƶ���
void PidInit(pHPID_CTR obj,int expect,float Kp,float Ki,float Kd)
{
	obj->no = no++;			//���ݳ�ʼ��˳�����ɿ���ϵͳ���
	obj->expect = expect;
	obj->Argv.Kp = Kp;
	obj->Argv.Ki = Ki;
	obj->Argv.Kd = Kd;
	
	obj->Data.error_Kp = 0;
	obj->Data.error_Ki = 0;
	obj->Data.error_Kd = 0;

}

//����PID����
int PidControl(pHPID_CTR obj,int input)
{
	//���㱾��ƫ��
	int error = obj->expect - input;					
	//�������D
	obj->Data.error_Kd =  error - obj->Data.error_Kp;	
	//�������P
	obj->Data.error_Kp = error;							
	//�������I
	obj->Data.error_Ki += error;	
	
	PidControlHeadCallBack(obj);
	
	//�����������P
	obj->feedback.back_Kp = obj->Data.error_Kp * obj->Argv.Kp;	
	//�����������I	
	obj->feedback.back_Ki = obj->Data.error_Ki * obj->Argv.Ki;	
	//�����������D
	obj->feedback.back_Kd = obj->Data.error_Kd  * obj->Argv.Kd;
	
	return PidControlCallBack(obj);
}



//�ں�PID������
int PidFuseControl(pHPID_CTR obj,int inputP,int inputD)
{	
	obj->Data.error_Kp = obj->expect - inputP;	//�������P
	
	obj->Data.error_Ki += obj->Data.error_Kp;	//�������I
	
	obj->Data.error_Kd = 0-inputD;	//�������D
	
	PidControlHeadCallBack(obj);
	
	//�����������P
	obj->feedback.back_Kp = obj->Data.error_Kp * obj->Argv.Kp;	
	//�����������I	
	obj->feedback.back_Ki = obj->Data.error_Ki * obj->Argv.Ki;	
	//�����������D
	obj->feedback.back_Kd = obj->Data.error_Kd * obj->Argv.Kd;
	
	return PidControlCallBack(obj);
}



//����PID����
int PidDoubleControl(pHDPID_CTR obj,int Oninput,int Ininput)
{
	//���⻷�����Ϊ�ڻ�����
	obj->In.expect = PidControl(&obj->Out,Oninput);
	
	//�ڻ�����
	return PidControl(&obj->In,Ininput);			//���ڻ����ֵ����
}


//������PID�����ֵ���д���
__weak void PidControlHeadCallBack(pHPID_CTR obj)
{
	obj->Data.error_Ki = constrain(obj->Data.error_Ki,-1000,1000);
}
//����PID�����ֵ���д���
__weak int PidControlCallBack(pHPID_CTR obj)
{
	return constrain(obj->feedback.back_Kp+obj->feedback.back_Ki+obj->feedback.back_Kd,-5500,5500);
}
