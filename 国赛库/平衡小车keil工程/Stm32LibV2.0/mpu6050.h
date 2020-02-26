/********************************************************
			���Ϲ���ѧԺ���»���
			����:CHEN
			����֧��:QQ742010094
			����:2019.7.2
*********************************************************/
#ifndef __MPU6050_H
#define __MPU6050_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "i2c.h"


#define	MPU_ADDRESS		0x68



#define MPU6500_DEG_PER_LSB_2000 (float)((2 * 2000.0) / 65536.0)		//�������ٶȼ�����
#define MPU6500_G_PER_LSB_8      (float)((2 * 8) / 65536.0)				//�������ٶȼ�����
#define DEG2RAD		0.017453293f	//��ת���� ��/180
#define RAD2DEG		57.29578f		//����ת�� 180/��

typedef struct	//����
{
	short x;
	short y;
	short z;
}BASE_DATA,*const pBASE_DATA;
typedef struct	//����
{
	BASE_DATA gyro;	//���ٶ�
	BASE_DATA acc;	//������ٶ�
}MPU_6AXIS,*const pMPU_6AXIS;

typedef struct
{
	float x;
	float y;
	float z;
}FLOAT_3AXIS,*const pFLOAT_3AXIS;


typedef struct //�˲�������ֵ
{
	float pitch;	//������
	float roll;		//������
	float yaw;		//ƫ����	
}MPU_ANGLE,*const MPU_pANGLE;

typedef struct	//mpu6050�ṹ��
{
	MPU_6AXIS axis;		//��������
	float temp;			//�¶�
	MPU_ANGLE angle;	//��̬��
	FLOAT_3AXIS velo;	//�����ٶ�
}MPU6050,*const pMPU6050;


bool mpu6050_init(void);//�����ʼ��ʧ�ܷ���false�����򷵻�true
bool ImuUpdata(float dt);		//������Ԫ����3D��ת����,�ɹ�����trueʧ�ܷ���false
void mpuGet_6Axis(MPU_6AXIS *const out);		//��ȡ����ԭʼ����
void mpuGet_Angle(MPU_ANGLE *const out);		//��ȡ�Ƕ�
float mpuGet_Temp(float *const out);			//��ȡ�¶�

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__MPU6050_H
