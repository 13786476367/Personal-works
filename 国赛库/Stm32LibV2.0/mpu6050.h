/********************************************************
			湖南工程学院创新基地
			作者:CHEN
			技术支持:QQ742010094
			日期:2019.7.2
*********************************************************/
#ifndef __MPU6050_H
#define __MPU6050_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "i2c.h"


#define	MPU_ADDRESS		0x68



#define MPU6500_DEG_PER_LSB_2000 (float)((2 * 2000.0) / 65536.0)		//物理化角速度计数据
#define MPU6500_G_PER_LSB_8      (float)((2 * 8) / 65536.0)				//物理化加速度计数据
#define DEG2RAD		0.017453293f	//度转弧度 π/180
#define RAD2DEG		57.29578f		//弧度转度 180/π

typedef struct	//基类
{
	short x;
	short y;
	short z;
}BASE_DATA,*const pBASE_DATA;
typedef struct	//六轴
{
	BASE_DATA gyro;	//角速度
	BASE_DATA acc;	//三轴加速度
}MPU_6AXIS,*const pMPU_6AXIS;

typedef struct
{
	float x;
	float y;
	float z;
}FLOAT_3AXIS,*const pFLOAT_3AXIS;


typedef struct //滤波后的输出值
{
	float pitch;	//俯仰角
	float roll;		//翻滚角
	float yaw;		//偏航角	
}MPU_ANGLE,*const MPU_pANGLE;

typedef struct	//mpu6050结构体
{
	MPU_6AXIS axis;		//六轴数据
	float temp;			//温度
	MPU_ANGLE angle;	//姿态角
	FLOAT_3AXIS velo;	//三轴速度
}MPU6050,*const pMPU6050;


bool mpu6050_init(void);//如果初始化失败返回false，否则返回true
bool ImuUpdata(float dt);		//更新四元数和3D旋转矩阵,成功返回true失败返回false
void mpuGet_6Axis(MPU_6AXIS *const out);		//获取六轴原始数据
void mpuGet_Angle(MPU_ANGLE *const out);		//获取角度
float mpuGet_Temp(float *const out);			//获取温度

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__MPU6050_H
