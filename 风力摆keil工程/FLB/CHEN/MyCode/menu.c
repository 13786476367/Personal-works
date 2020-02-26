#include "menu.h"
#include <ctype.h>


static bool GetParam(float *receive)	//获取按键的参数
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
			case KEY_ENTER:		//确认
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
			case KEY_DEL:		//回删
			{
				if(i>0)
				{
					buff[--i]=0;
					OLED_BackSpace(1);
				}
			}break;
			default:			//数字
			{
				if(i>=SZ-1)		//字符超出范围
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
		OLED_PrintfAt(0,0,"菜单>>参数修改  ");
		OLED_PrintfAt(1,0,"1.修改Kp        ");
		OLED_PrintfAt(2,0,"2.修改Ki        ");
		OLED_PrintfAt(3,0,"3.修改Kd        ");
		key=KEY_GetKey();
		if(key==KEY_ESC)
			break;
		switch(KEY_Num(key))
		{
			case '1':
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,0,"参数修改>>Kp");
//				OLED_PrintfAt(1,0,"当前Kp=%f",g_posx.Argv.Kp);
				OLED_SetXY(2,0);
				OLED_Printf("Kp=");
				OLED_PrintfAt(3,0,"Enter        Del");
				if(GetParam(&receive))		//参数修改成功
				{
					OLED_ClearLine(2);
					OLED_PrintfAt(2,0,"修改成功!");
					KEY_GetKey();
				}				
			}break;
			case '2':
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,0,"参数修改>>Ki");
//				OLED_PrintfAt(1,0,"当前Ki=%f",g_posx.Argv.Ki);
				OLED_SetXY(2,0);
				OLED_Printf("Ki=");
				OLED_PrintfAt(3,0,"Enter        Del");
				if(GetParam(&receive))		//参数修改成功
				{
					OLED_PrintfAt(2,0,"修改成功!");
					KEY_GetKey();
				}
			}break;
			case '3':
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,0,"参数修改>>Kd");
//				OLED_PrintfAt(1,0,"当前Kd=%f",g_posx.Argv.Kd);
				OLED_SetXY(2,0);
				OLED_Printf("Kd=");
				OLED_PrintfAt(3,0,"Enter        Del");
				if(GetParam(&receive))		//参数修改成功
				{
					OLED_PrintfAt(2,0,"修改成功!");
					KEY_GetKey();
				}
			}break;
			default:
			{
				OLED_ClearAll();
				OLED_PrintfAt(0,3,"输入错误!");
				OLED_PrintfAt(3,0,"请按任意键继续！");
				KEY_GetKey();
			}
		}
	}
}

//模式选择1(画线)
void SelectMode_0(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0/*-500*/}};
	static HPID_CTR pid_gyrox={.no=0,.expect=0,.Argv={.Kp=20,.Ki=0,.Kd=400}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//电机转速
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



//模式选择2(画定长线)
volatile float SelectMode_1_L=20;
void SelectMode_1(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0/*-500*/}};
	static HPID_CTR pid_gyrox={.no=0,.expect=0,.Argv={.Kp=20,.Ki=0,.Kd=400}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//电机转速
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

//模式选择3(画定向线)
volatile float SelectMode_2_Angle=45;
void SelectMode_2(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0}};
	static HPID_CTR pid_roll={.no=0,.Argv={.Kp=-3.5,.Ki=0,.Kd=-3.0}};
	
	const uint32_t priod = 1250;
	const float Phase[19]={0,0,0,0,-0.165};
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//电机转速
	tim_cnt += 5;
	uint8_t offset=SelectMode_2_Angle/10.0f;
	float A=atanf(SelectMode_1_L/64.0f)*180.0f/3.141593f;
	float Ax = A*cosf(SelectMode_2_Angle/180.0f*3.141593f);
	float Ay = A*sinf(SelectMode_2_Angle/180.0f*3.141593f);
	pid_pitch.expect = Ay*sinf(2.0f*3.1415926f*(float)tim_cnt/priod+Phase[offset])*100+0.5f;//蓝色
	pid_roll.expect = Ax*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;//绿色
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_roll,g_mpu6050.angle.roll*100);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,(short)g_mpu6050.angle.roll*100,(short)g_mpu6050.angle.pitch*100);

}

//模式选择4(停止)
void SelectMode_3(pANOTC anotc)
{
	static HPID_CTR pid_gyroy={.no=0,.expect=0,.Argv={.Kp=-20,.Ki=0,.Kd=-400}};
	static HPID_CTR pid_gyrox={.no=0,.expect=0,.Argv={.Kp=20,.Ki=0,.Kd=400}};
	short moto1=0,moto2=0;//电机转速
	
	moto1 = PidControl(&pid_gyroy,g_mpu6050.axis.gyro.y);
	moto2 = PidControl(&pid_gyrox,g_mpu6050.axis.gyro.x);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll*100,g_mpu6050.angle.pitch*100,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z);
}

//模式选择5(画圆)
volatile float SelectMode_4_R=14.5;
void SelectMode_4(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0}};
	static HPID_CTR pid_roll={.no=0,.Argv={.Kp=-3.5,.Ki=0,.Kd=-3.0}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//电机转速
	tim_cnt += 5;
	float A=atanf(SelectMode_4_R/64.0f)*180.0f/3.141593f;
	pid_pitch.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod+1.50)*100+0.5f;//蓝色
	pid_roll.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;//绿色
	moto1 = PidControl(&pid_pitch,g_mpu6050.angle.pitch*100);
	moto2 = PidControl(&pid_roll,g_mpu6050.angle.roll*100);
	SetPwm(moto1,moto2);
	anotc->size = 0;
	ANOTC_V6_Printf(1,anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
	ANOTC_V6_Printf(2,anotc,"hhhhhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z,pid_pitch.expect,(short)g_mpu6050.angle.roll*100,(short)g_mpu6050.angle.pitch*100);
}

//模式选择6(画其它图形)
void SelectMode_5(pANOTC anotc)
{
	static HPID_CTR pid_pitch={.no=0,.Argv={.Kp=3.5,.Ki=0,.Kd=3.0}};
	static HPID_CTR pid_roll={.no=0,.Argv={.Kp=-3.5,.Ki=0,.Kd=-.0}};
	
	const uint32_t priod = 1250;
	static uint32_t tim_cnt;
	short moto1=0,moto2=0;//电机转速
	tim_cnt += 5;
	float A=atanf(20.0f/64.0f)*180.0f/3.141593f;
	pid_pitch.expect = A*sinf(3.0f*2.0f*3.1415926f*(float)tim_cnt/priod+1.5f)*100+0.5f;//蓝色
	pid_roll.expect = A*sinf(2.0f*3.1415926f*(float)tim_cnt/priod)*100+0.5f;//绿色
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
		OLED_PrintfAt(0,0,"菜单>>比赛项目");
		OLED_PrintfAt(1,0,"请输入题号:");
		OLED_SetXY(1,11);
		num=KEY_Num(KEY_GetKey());
		if(num==KEY_ESC)
			break;
		if(num>='1'&&num<='8')
		{
			OLED_Printf("%c",num);
			OLED_PrintfAt(2,0,"是否确认?");
			OLED_PrintfAt(3,0,"Enter        ESC");
			uint8_t key;
			while((key=KEY_GetKey())!=KEY_ESC)
			{
				if(key==KEY_ENTER)
				{
					switch(num)
					{
						case '1'://(画线)
						{
							fun = SelectMode_0;
						}break;
						case '2'://(画定长线)
						{
							OLED_ClearLine(1);
							OLED_ClearLine(2);
							OLED_PrintfAt(1,0,"请输入直线长度:");
							OLED_SetXY(2,0);
							OLED_Printf("L=");
							float cm;
							if(GetParam(&cm))
							{
								SelectMode_1_L = constrain(cm/2,15,30);
								fun = SelectMode_1;
							}
						}break;
						case '3'://(画定向线)
						{
							OLED_ClearLine(1);
							OLED_ClearLine(2);
							OLED_PrintfAt(1,0,"请输入直线角度:");
							OLED_SetXY(2,0);
							OLED_Printf("Rad=");
							float angle;
							if(GetParam(&angle))
							{
								SelectMode_2_Angle = constrain(angle,0,360);
								fun = SelectMode_2;
							}
						}break;
						case '4'://(停止)
						{
							fun = SelectMode_3;
						}break;
						case '5'://(画圆)
						{
							OLED_ClearLine(1);
							OLED_ClearLine(2);
							OLED_PrintfAt(1,0,"请输入圆的半径:");
							OLED_SetXY(2,0);
							OLED_Printf("R=");
							float r;
							if(GetParam(&r))
							{
								SelectMode_4_R = constrain(r,15,35);
								fun = SelectMode_4;
							}
						}break;
						case '6'://(画其它图形)
						{
							fun = SelectMode_5;
						}break;
					}
					OLED_ClearLine(3);
					OLED_PrintfAt(3,0,"选择成功！");
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
			OLED_PrintfAt(0,3,"输入错误!");
			OLED_PrintfAt(3,0,"请按任意键继续！");
			KEY_GetKey();
		}
	}
}

//其它功能0//区域校准
static void OtherFun_0()
{
	uint8_t key;
	OLED_ClearAll();
	OLED_PrintfAt(0,0,"其他功能>>校准");
	OLED_PrintfAt(1,0,"请输入校准区域:");
	OLED_PrintfAt(3,0,"Enter        Del");
	OLED_SetXY(2,0);
	while((key=KEY_GetKey())!=KEY_ESC)
	{
		
	}
}

//其它功能1//参数读取
static void OtherFun_1()
{
	static bool state=true;
	uint8_t key;
	OLED_ClearAll();
	while(1)
	{
		OLED_PrintfAt(0,0,"其他功能>>控制");
		if(state==false)
		{
			OLED_PrintfAt(1,0,"当前状态:关闭");
		}
		else
		{
			OLED_PrintfAt(1,0,"当前状态:开启");
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
//其它功能2//参数保存
static void OtherFun_2()
{
	uint8_t key;
	OLED_ClearAll();
	while(1)
	{
		OLED_PrintfAt(0,0,"其他功能>>保存");
		OLED_PrintfAt(1,0,"是否确定保存参数");
		OLED_PrintfAt(3,0,"Enter        Del");
		key=KEY_GetKey();
		if(key==KEY_ESC||key==KEY_DEL)		//退出
			return;
		else if(key==KEY_ENTER)	//确认
		{
			//执行参数保存
		}
		else
		{
			OLED_ClearAll();
			OLED_PrintfAt(0,3,"输入错误!");
			OLED_PrintfAt(3,0,"请按任意键继续！");
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
		OLED_PrintfAt(0,0,"菜单>>其它功能");
		OLED_PrintfAt(1,0,"1.区域校准");
		OLED_PrintfAt(2,0,"2.舵机控制");
		OLED_PrintfAt(3,0,"3.参数保存");
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
				OLED_PrintfAt(0,3,"输入错误!");
				OLED_PrintfAt(3,0,"请按任意键继续！");
				KEY_GetKey();
			}
		}
	}
}


void Menu(void)
{
	KEY_Init();
	OLED_PrintfAt(0,0,"      菜单      ");
	OLED_PrintfAt(1,0,"1.参数修改      ");		
	OLED_PrintfAt(2,0,"2.比赛项目      ");
	OLED_PrintfAt(3,0,"3.其它功能      ");
	if(KEY_ReadINT()!=false)	//检测是否有键盘消息
	{
		uint8_t key;
		KEY_ReadData(&key);
		key=KEY_Num(key);
		switch(key)
		{
			case '1':ChangeParam();break;//修改参数
			case '2':SelectMode();break; //比赛项目
			case '3':OtherFun();break;//其它功能
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
