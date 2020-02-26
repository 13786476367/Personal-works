#include "menu.h"
#include <ctype.h>


static bool GetParam(float *receive)	//��ȡ�����Ĳ���
{
	#define SZ 4
	uint8_t key;
	uint8_t i=0;
	char buff[SZ]={0};
	while((key=KEY_GetKey())!=KEY_ESC)
	{
		key=KEY_Num(key);

		switch(key)
		{
			case KEY_ENTER:		//ȷ��
			{
				if(i>0)
				{
					sscanf(buff,"%f",receive);
					return true;
				}
				else
				{
					return false;
				}
			}
			case KEY_DEL:		//��ɾ
			{
				if(i>0)
				{
					buff[--i]=0;
					OLED_BackSpace(1);
				}
			}break;
			default:			//����
			{
				if(i>=SZ-1)		//�ַ�������Χ
				{
					break;
				}
				if(isdigit(key)!=0&&i<SZ-1)
				{
					buff[i++]=key;
					buff[i]=0;
					OLED_Putc(key);
				}
			}
		}
	}
	return false;
}


static void ChangeParam()
{
	uint8_t key;
	float receive;
	while(1)
	{
		OLED_PrintfAt(0,0,"�˵�>>�����޸�  ");
		OLED_PrintfAt(1,0,"1.�޸�Kp        ");
		OLED_PrintfAt(2,0,"2.�޸�Ki        ");
		OLED_PrintfAt(3,0,"3.�޸�Kd        ");
		key=KEY_GetKey();
		if(key==KEY_ESC)
			break;
		switch(KEY_Num(key))
		{
			case '1':
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,0,"�����޸�>>Kp");
//				OLED_PrintfAt(1,0,"��ǰKp=%f",g_posx.Argv.Kp);
				OLED_SetXY(2,0);
				OLED_Printf("Kp=");
				OLED_PrintfAt(3,0,"Enter        Del");
				if(GetParam(&receive))		//�����޸ĳɹ�
				{
					OLED_ClearLine(2);
					OLED_PrintfAt(2,0,"�޸ĳɹ�!");
					KEY_GetKey();
				}				
			}break;
			case '2':
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,0,"�����޸�>>Ki");
//				OLED_PrintfAt(1,0,"��ǰKi=%f",g_posx.Argv.Ki);
				OLED_SetXY(2,0);
				OLED_Printf("Ki=");
				OLED_PrintfAt(3,0,"Enter        Del");
				if(GetParam(&receive))		//�����޸ĳɹ�
				{
					OLED_PrintfAt(2,0,"�޸ĳɹ�!");
					KEY_GetKey();
				}
			}break;
			case '3':
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,0,"�����޸�>>Kd");
//				OLED_PrintfAt(1,0,"��ǰKd=%f",g_posx.Argv.Kd);
				OLED_SetXY(2,0);
				OLED_Printf("Kd=");
				OLED_PrintfAt(3,0,"Enter        Del");
				if(GetParam(&receive))		//�����޸ĳɹ�
				{
					OLED_PrintfAt(2,0,"�޸ĳɹ�!");
					KEY_GetKey();
				}
			}break;
			default:
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,3,"�������!");
				OLED_PrintfAt(3,0,"�밴�����������");
				KEY_GetKey();
			}
		}
	}
}

//ģʽѡ��1(����)
void SelectMode_0(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0/*-500*/}};
	static HPID_CTR pid_gyrox={.no=0,.expect=0,.Argv={.Kp=20,.Ki=0,.Kd=400}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//���ת��
	float A=atanf(30/64.0f)*180.0f/3.141593f;
	tim_cnt += 5;
	pid_pitch.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_gyrox,g_mpu6050.axis.gyro.x);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,moto1,(short)g_mpu6050.angle.pitch*100);
}



//ģʽѡ��2(��������)
volatile float SelectMode_1_L=20;
void SelectMode_1(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0/*-500*/}};
	static HPID_CTR pid_gyrox={.no=0,.expect=0,.Argv={.Kp=20,.Ki=0,.Kd=400}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//���ת��
	float A=atanf(SelectMode_1_L/64.0f)*180.0f/3.141593f;
	tim_cnt += 5;
	pid_pitch.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_gyrox,g_mpu6050.axis.gyro.x);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,moto1,(short)g_mpu6050.angle.pitch*100);

}

//ģʽѡ��3(��������)
volatile float SelectMode_2_Angle=45;
void SelectMode_2(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0}};
	static HPID_CTR pid_roll={.no=0,.Argv={.Kp=-3.5,.Ki=0,.Kd=-3.0}};
	
	const uint32_t priod = 1250;
	const float Phase[19]={0,0,0,0,-0.165};
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//���ת��
	tim_cnt += 5;
	uint8_t offset=SelectMode_2_Angle/10.0f;
	float A=atanf(SelectMode_1_L/64.0f)*180.0f/3.141593f;
	float Ax = A*cosf(SelectMode_2_Angle/180.0f*3.141593f);
	float Ay = A*sinf(SelectMode_2_Angle/180.0f*3.141593f);
	pid_pitch.expect = Ay*sinf(2.0f*3.1415926f*(float)tim_cnt/priod+Phase[offset])*100+0.5f;//��ɫ
	pid_roll.expect = Ax*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;//��ɫ
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_roll,g_mpu6050.angle.roll*100);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,(short)g_mpu6050.angle.roll*100,(short)g_mpu6050.angle.pitch*100);

}

//ģʽѡ��4(ֹͣ)
void SelectMode_3(pANOTC anotc)
{
	static HPID_CTR pid_gyroy={.no=0,.expect=0,.Argv={.Kp=-20,.Ki=0,.Kd=-400}};
	static HPID_CTR pid_gyrox={.no=0,.expect=0,.Argv={.Kp=20,.Ki=0,.Kd=400}};
	short moto1=0,moto2=0;//���ת��
	
	moto1 = PidControl(&pid_gyroy,g_mpu6050.axis.gyro.y);
	moto2 = PidControl(&pid_gyrox,g_mpu6050.axis.gyro.x);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll*100,g_mpu6050.angle.pitch*100,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z);
}

//ģʽѡ��5(��Բ)
volatile float SelectMode_4_R=14.5;
void SelectMode_4(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0}};
	static HPID_CTR pid_roll={.no=0,.Argv={.Kp=-3.5,.Ki=0,.Kd=-3.0}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//���ת��
	tim_cnt += 5;
	float A=atanf(SelectMode_4_R/64.0f)*180.0f/3.141593f;
	pid_pitch.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod+1.50)*100+0.5f;//��ɫ
	pid_roll.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;//��ɫ
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_roll,g_mpu6050.angle.roll*100);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,(short)g_mpu6050.angle.roll*100,(short)g_mpu6050.angle.pitch*100);
}

//ģʽѡ��6(������ͼ��)
void SelectMode_5(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0}};
	static HPID_CTR pid_roll={.no=0,.Argv={.Kp=-3.5,.Ki=0,.Kd=-.0}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//���ת��
	tim_cnt += 5;
	float A=atanf(20.0f/64.0f)*180.0f/3.141593f;
	pid_pitch.expect = A*sinf(3.0f*2.0f*3.1415926f*(float)tim_cnt/priod+1.5f)*100+0.5f;//��ɫ
	pid_roll.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;//��ɫ
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_roll,g_mpu6050.angle.roll*100);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,(short)g_mpu6050.angle.roll*100,(short)g_mpu6050.angle.pitch*100);


}





static void SelectMode()
{
	uint8_t num;
	while(1)
	{
		OLED_ClearAll();
		OLED_PrintfAt(0,0,"�˵�>>������Ŀ");
		OLED_PrintfAt(1,0,"���������:");
		OLED_SetXY(1,11);
		num=KEY_Num(KEY_GetKey());
		if(num==KEY_ESC)
			break;
		if(num>='1'&&num<='8')
		{
			OLED_Printf("%c",num);
			OLED_PrintfAt(2,0,"�Ƿ�ȷ��?");
			OLED_PrintfAt(3,0,"Enter        ESC");
			uint8_t key;
			while((key=KEY_GetKey())!=KEY_ESC)
			{
				if(key==KEY_ENTER)
				{
					switch(num)
					{
						case '1'://(����)
						{
							fun = SelectMode_0;
						}break;
						case '2'://(��������)
						{
							OLED_ClearLine(1);
							OLED_ClearLine(2);
							OLED_PrintfAt(1,0,"������ֱ�߳���:");
							OLED_SetXY(2,0);
							OLED_Printf("L=");
							float cm;
							if(GetParam(&cm))
							{
								SelectMode_1_L = constrain(cm/2,15,30);
								fun = SelectMode_1;
							}
						}break;
						case '3'://(��������)
						{
							OLED_ClearLine(1);
							OLED_ClearLine(2);
							OLED_PrintfAt(1,0,"������ֱ�߽Ƕ�:");
							OLED_SetXY(2,0);
							OLED_Printf("Rad=");
							float angle;
							if(GetParam(&angle))
							{
								SelectMode_2_Angle = constrain(angle,0,360);
								fun = SelectMode_2;
							}
						}break;
						case '4'://(ֹͣ)
						{
							fun = SelectMode_3;
						}break;
						case '5'://(��Բ)
						{
							OLED_ClearLine(1);
							OLED_ClearLine(2);
							OLED_PrintfAt(1,0,"������Բ�İ뾶:");
							OLED_SetXY(2,0);
							OLED_Printf("R=");
							float r;
							if(GetParam(&r))
							{
								SelectMode_4_R = constrain(r,15,35);
								fun = SelectMode_4;
							}
						}break;
						case '6'://(������ͼ��)
						{
							fun = SelectMode_5;
						}break;
					}
					OLED_ClearLine(3);
					OLED_PrintfAt(3,0,"ѡ��ɹ���");
					KEY_GetKey();
					break;
				}
			}
			if(key==KEY_ESC)
				return;
		}
		else
		{
			OLED_ClearAll();
			OLED_PrintfAt(0,3,"�������!");
			OLED_PrintfAt(3,0,"�밴�����������");
			KEY_GetKey();
		}
	}
}

//��������0//����У׼
static void OtherFun_0()
{
	uint8_t key;
	OLED_ClearAll();
	OLED_PrintfAt(0,0,"��������>>У׼");
	OLED_PrintfAt(1,0,"������У׼����:");
	OLED_PrintfAt(3,0,"Enter        Del");
	OLED_SetXY(2,0);
	while((key=KEY_GetKey())!=KEY_ESC)
	{
		
	}
}

//��������1//������ȡ
static void OtherFun_1()
{
	static bool state=true;
	uint8_t key;
	OLED_ClearAll();
	while(1)
	{
		OLED_PrintfAt(0,0,"��������>>����");
		if(state==false)
		{
			OLED_PrintfAt(1,0,"��ǰ״̬:�ر�");
		}
		else
		{
			OLED_PrintfAt(1,0,"��ǰ״̬:����");
		}
		OLED_PrintfAt(3,0,"Enter        Del");
		
		if(KEY_ReadINT()!=false)
		{
			while(KEY_ReadData(&key));
			extern TIM_HandleTypeDef htim2;
			if(key==KEY_ENTER)
			{

				state=true;
			}
			else if(key==KEY_DEL)
			{

				state=false;
			}
			else if(key==KEY_ESC)
				break;
		}
	}
	
}
//��������2//��������
static void OtherFun_2()
{
	uint8_t key;
	OLED_ClearAll();
	while(1)
	{
		OLED_PrintfAt(0,0,"��������>>����");
		OLED_PrintfAt(1,0,"�Ƿ�ȷ���������");
		OLED_PrintfAt(3,0,"Enter        Del");
		key=KEY_GetKey();
		if(key==KEY_ESC||key==KEY_DEL)		//�˳�
			return;
		else if(key==KEY_ENTER)	//ȷ��
		{
			//ִ�в�������
		}
		else
		{
			OLED_ClearAll();
			OLED_PrintfAt(0,3,"�������!");
			OLED_PrintfAt(3,0,"�밴�����������");
			KEY_GetKey();
		}

	}
}
static void OtherFun()
{
	uint8_t key;
	while(1)
	{
		OLED_ClearAll();
		OLED_PrintfAt(0,0,"�˵�>>��������");
		OLED_PrintfAt(1,0,"1.����У׼");
		OLED_PrintfAt(2,0,"2.�������");
		OLED_PrintfAt(3,0,"3.��������");
		key=KEY_GetKey();
		if(key==KEY_ESC)
			break;
		switch(KEY_Num(key))
		{
			case '1':OtherFun_0();break;
			case '2':OtherFun_1();break;
			case '3':OtherFun_2();break;
			default:
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,3,"�������!");
				OLED_PrintfAt(3,0,"�밴�����������");
				KEY_GetKey();
			}
		}
	}
}


void Menu(void)
{
	KEY_Init();
	OLED_PrintfAt(0,0,"      �˵�      ");
	OLED_PrintfAt(1,0,"1.�����޸�      ");		
	OLED_PrintfAt(2,0,"2.������Ŀ      ");
	OLED_PrintfAt(3,0,"3.��������      ");
	if(KEY_ReadINT()!=false)	//����Ƿ��м�����Ϣ
	{
		uint8_t key;
		KEY_ReadData(&key);
		key=KEY_Num(key);
		switch(key)
		{
			case '1':ChangeParam();break;//�޸Ĳ���
			case '2':SelectMode();break; //������Ŀ
			case '3':OtherFun();break;//��������
		}
	}
}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	extern volatile bool g_mode;
//	if(GPIO_Pin==GPIO_PIN_13)
//	{
//		g_mode = !g_mode;
//		if(g_mode!=false)
//		{
//			KEY_Init();
//		}
//	}
//}
