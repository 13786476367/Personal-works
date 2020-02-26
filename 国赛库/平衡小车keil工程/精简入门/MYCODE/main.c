/********************************************************
			���Ϲ���ѧԺ���»���
			����:CHEN
			����֧��:QQ742010094
			����:2019.7.2
*********************************************************/
#include "systemInit.h"
#include "sys.h"
#include "oled.h"
#include "mpu6050.h"
#include "pid.h"
#include "anotc_v6.h"

#define PWMR   TIM1->CCR1  		//PA8
#define AIN2   PBout(15)
#define AIN1   PBout(14)
#define BIN1   PBout(13)
#define BIN2   PBout(12)
#define PWML   TIM1->CCR4	  	//PA11

#define USING_ENCODER		//ʹ���ٶȻ�

MPU6050 g_mpu6050;			//MPU6050ȫ�ֶ���
ANOTC g_anotc;			//��λ��ͨѶ����
HPID_CTR g_blance;			//ƽ�⻷����ȫ�ֶ���

#ifdef USING_ENCODER
HPID_CTR g_velocity;		//�ٶȻ�ȫ�ֶ���
#endif //USING_ENCODER

void Set_Pwm(short moto1,short moto2)
{
    	if(moto1>0)
		{
			AIN2=0;
			AIN1=1;
			PWMR = moto1;
		}
		else
		{
			AIN2=1;
			AIN1=0;
			PWMR = -moto1;
		}
		
		 if(moto2>0)
		 {
			 BIN1=0;
			 BIN2=1;
			 PWML = moto2;
		 }
		else 
		{
			BIN1=1;
			BIN2=0;
			PWML = -moto2;
		}	
}

int main()
{
	bool openFlag=false;
#ifndef USING_ENCODER
	//��ʼ��ƽ�⻷PID����
	PidInit(&g_blance,-500,-3.5f,-0.4f,-1.1f);		//��ȷ������ֵ������Kp������Kd���ٵ���Ki			
#else
	PidInit(&g_blance,-600,-3.0f,0,-1.1f);		//��ȷ������ֵ������Kp������Kd���ٵ���Ki
	PidInit(&g_velocity,0,-120.0f,-0.4f,-0.0f);			//�ٶȻ�ȫ�ֶ���
#endif //USING_ENCODER
	__disable_irq();//�����ж�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//�򿪸��ö˿�ʱ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//�����жϷ���2
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);     //����JTAG
	DWT_Init();
	
	//��ʼ��DMA
	USART3_DMA_Config(&g_anotc.buff);
	InitSystem();
	while(mpu6050_init()==false);
	OLED_Init();
	TIM_Cmd(TIM3,ENABLE);	//������ʱ��
	__enable_irq();//�����ж�
	
	while(1)
	{
		OLED_PrintfAt(0,0,"ROLL:%.2f  ",g_mpu6050.angle.roll);
		OLED_PrintfAt(1,0,"TEMP:%.2f  ",g_mpu6050.temp);
		OLED_PrintfAt(2,0,"PWML:%d  ",PWML);
		OLED_PrintfAt(3,0,"PWMR:%d  ",PWMR);	
		if(GPIO_ReadInputDataBit(KEY2_GPIO_Port,KEY2_Pin)==RESET)
		{
			delay_ms(3);
			if(GPIO_ReadInputDataBit(KEY2_GPIO_Port,KEY2_Pin)==RESET)
			{
				if(openFlag==false)
				{
					openFlag = true;
					TIM_Cmd(TIM1,ENABLE);	//�������
				}
				else
				{
					openFlag = false;
					TIM_Cmd(TIM1,DISABLE);	//�رյ��
				}
				while(GPIO_ReadInputDataBit(KEY2_GPIO_Port,KEY2_Pin)==RESET);
			}
		}
		delay_ms(30);
	}
}

/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;    
   switch(TIMX)
	 {
		case 2:  Encoder_TIM= (short)TIM2 -> CNT;  TIM2 -> CNT=0;break;
		case 3:  Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0;break;	
		case 4:  Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0;break;	
		default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
}
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		short blance=0,velocity=0,moto;
		ImuUpdata(0.005f);
		mpuGet_Angle(&g_mpu6050.angle);			//��ȡ��ǰ��̬�Ƕ�
		mpuGet_Temp(&g_mpu6050.temp);			//��ȡ��ǰ�¶�
		mpuGet_6Axis(&g_mpu6050.axis);			//��ȡ��������
		
		g_anotc.size=0;
		ANOTC_V6_Printf(1,&g_anotc,"fff",g_mpu6050.angle.roll,g_mpu6050.angle.pitch,g_mpu6050.angle.yaw);
		ANOTC_V6_Printf(2,&g_anotc,"hhhhhh",g_mpu6050.axis.acc.x,g_mpu6050.axis.acc.y,g_mpu6050.axis.acc.z,g_mpu6050.axis.gyro.x,g_mpu6050.axis.gyro.y,g_mpu6050.axis.gyro.z);
		DMA_SetCurrDataCounter(DMA1_Channel2,g_anotc.size);//����ȷ��ÿ��ѭ��Ҫ�����������ֵ
		USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
		
		//ƽ�⻷PID����
		blance = PidFuseControl(&g_blance,g_mpu6050.angle.roll*100,g_mpu6050.axis.gyro.x);
		
#ifdef USING_ENCODER
		short encoder_left,encoder_right;//���ұ�����
		encoder_left = Read_Encoder(2);
		encoder_right = Read_Encoder(4);
		velocity = PidControl(&g_velocity,encoder_left+encoder_right);		//�ٶȻ�ȫ�ֶ���
#endif //USING_ENCODER
		
		moto = constrain(blance+velocity,-7000,7000);
		//���õ��ת��
		Set_Pwm(moto,moto);
		
		if(g_mpu6050.angle.roll*100>(4500+g_blance.expect)||g_mpu6050.angle.roll*100<(-4500+g_blance.expect))
		{
			TIM_Cmd(TIM1,DISABLE);	//�رյ��
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	
	}
}
