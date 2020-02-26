#include "mpu6050.h"
#include <math.h>


//本文件使用的宏定义
#define mpu6050_delay delay_ms	//延时函数

#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define	ACCEL_XOUT_H	0x3B	//X轴加速度高八位
#define	ACCEL_XOUT_L	0x3C	//X轴加速度低八位
#define	ACCEL_YOUT_H	0x3D	//Y轴加速度高八位
#define	ACCEL_YOUT_L	0x3E	//Y轴加速度低八位
#define	ACCEL_ZOUT_H	0x3F	//Z轴加速度高八位
#define	ACCEL_ZOUT_L	0x40	//Z轴加速度低八位

#define	TEMP_OUT_H		0x41	//陀螺仪温度高八位
#define	TEMP_OUT_L		0x42	//陀螺仪温度低八位

#define	GYRO_XOUT_H		0x43	//X轴角速度高八位
#define	GYRO_XOUT_L		0x44	//X轴角速度低八位	
#define	GYRO_YOUT_H		0x45	//Y轴角速度高八位
#define	GYRO_YOUT_L		0x46	//Y轴角速度低八位
#define	GYRO_ZOUT_H		0x47	//Z轴角速度高八位
#define	GYRO_ZOUT_L		0x48	//Z轴角速度低八位

//由本文件维护的静态变量
static MPU_6AXIS g_6axis;		//六轴数据
static short g_oldtemp;			//陀螺仪温度原始值

static float q[4]={1.0f,0,0,0};		//四元数
static float R11,R21;				//四元数3D旋转矩阵(1,1),(2,1)项
static float vecxZ, vecyZ, veczZ;	//机体坐标系下的Z方向向量


static float invSqrt(float x)	/*快速开平方求倒（近似值）*/
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f375a86 - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

static bool mpu6050_WriteReg(unsigned char slave_addr, unsigned char reg_addr,unsigned char bitstart,unsigned char length,unsigned char data)
{
	unsigned char b,mask; 
	if(i2c_read(slave_addr,reg_addr,1,&b)==0)
	{
		mask = (0xFF << (bitstart + 1)) | 0xFF >> ((8 - bitstart) + length - 1);
		data <<= (8 - length);
		data >>= (7 - bitstart);
		b &= mask;
		b |= data;
		if(i2c_write(slave_addr,reg_addr,1,&b)==0)
			return true;
	}
	return false;
}

bool mpu6050_init()//如果初始化失败返回false，否则返回true
{
	unsigned char who=0;
	if(mpu6050_WriteReg(MPU_ADDRESS,0x6B,6,1,0)==false)	//清除休眠标志位，进入工作状态
		return false;
	mpu6050_delay(200);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x6B,3,4,1)==false)	//设置X轴陀螺为系统时钟源
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x1B,4,2,3)==false)	//设置陀螺仪灵敏度为+-2000°/S
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x1C,4,2,1)==false)	//设置加速度计量程为+-4g
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x1A,2,3,3)==false)	//配置数字低通滤波器加速度：带宽44Hz，延迟4.9ms。角速度：带宽42Hz，延迟4.8ms。
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0X6A,5,1,0)==false)	//使能I2C模式
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x37,1,1,1)==false)		//使能I2C旁通模式
		return false;
	i2c_read(MPU_ADDRESS,0X75,1,&who);
	if(who==0)//自校验
		return false;
	return true;
}

#define FIFO_SIZE	5
static bool mpu6050_read(void)
{
	unsigned char buff[14];		//缓冲区
	register int i;	//临时计数值，尽量用寄存器保存
	int tempax,tempay,tempaz,tempgx,tempgy,tempgz;
	static MPU_6AXIS fifo[FIFO_SIZE];
	static int count=0;
	if(i2c_read(MPU_ADDRESS,0x3b,14,buff))//如果读取失败
		return false;
	if(count >= FIFO_SIZE)
		count = 0;
	fifo[count].acc.x = ((buff[0] << 8) | buff[1]);		//提取加速度原始值
	fifo[count].acc.y = ((buff[2] << 8) | buff[3]);
	fifo[count].acc.z = ((buff[4] << 8) | buff[5]);
	
	g_oldtemp = (buff[6]<<8 | buff[7]);					//提取温度原始值
	
	fifo[count].gyro.x = ((buff[8] << 8) | buff[9]);	//提取角速度原始值
	fifo[count].gyro.y = ((buff[10] << 8) | buff[11]);
	fifo[count].gyro.z = ((buff[12] << 8) | buff[13]);
	count++;
	
	//为加速度和角速度进行滑动滤波
	tempax=0;tempay=0;tempaz=0;
	tempgx=0;tempgy=0;tempgz=0;
	for(i=0;i<FIFO_SIZE;i++)		
	{
		tempax += fifo[i].acc.x;
		tempay += fifo[i].acc.y;
		tempaz += fifo[i].acc.z;
		tempgx += fifo[i].gyro.x;
		tempgy += fifo[i].gyro.y;
		tempgz += fifo[i].gyro.z;
	}
	g_6axis.acc.x = tempax/FIFO_SIZE;
	g_6axis.acc.y = tempay/FIFO_SIZE;
	g_6axis.acc.z = tempaz/FIFO_SIZE;
	g_6axis.gyro.x = tempgx/FIFO_SIZE;
	g_6axis.gyro.y = tempgy/FIFO_SIZE;
	g_6axis.gyro.z = tempgz/FIFO_SIZE;
	return true;
}

bool ImuUpdata(float dt)		//更新四元数和3D旋转矩阵,成功返回true失败返回false
{
	static float exInt = 0.0f,eyInt = 0.0f,ezInt = 0.0f;	//积分误差累计
	const float Kp = 2.0f;		//比例增益
	const float Ki = 0.002f;		//积分增益
	const float halfT =0.5f * dt;
	
	float normalise;			
	FLOAT_3AXIS acc;
	FLOAT_3AXIS gyro;
	if(mpu6050_read()==false)
		return false;
	if(g_6axis.acc.x==0||g_6axis.acc.y==0||g_6axis.acc.z==0)	//如果当前是自由落体运动
		return false;
	
	
	//获取弧度制的角速度
	gyro.x = g_6axis.gyro.x*MPU6500_DEG_PER_LSB_2000*DEG2RAD;
	gyro.y = g_6axis.gyro.y*MPU6500_DEG_PER_LSB_2000*DEG2RAD;
	gyro.z = g_6axis.gyro.z*MPU6500_DEG_PER_LSB_2000*DEG2RAD;
		
	//单位化加速计测量值
	normalise = invSqrt(g_6axis.acc.x*g_6axis.acc.x+g_6axis.acc.y*g_6axis.acc.y+g_6axis.acc.z*g_6axis.acc.z);	//快速开根号求倒invSqrt
	acc.x = g_6axis.acc.x*normalise;
	acc.y = g_6axis.acc.y*normalise;
	acc.z = g_6axis.acc.z*normalise;
		
	//加速计读取的方向与重力加速计方向的差值，用向量叉乘计算
	float ex, ey, ez;
	ex = (acc.y * veczZ - acc.z * vecyZ);
	ey = (acc.z * vecxZ - acc.x * veczZ);
	ez = (acc.x * vecyZ - acc.y * vecxZ);
		
	//误差累计，与积分常数相乘
	exInt += Ki * ex * dt;  
	eyInt += Ki * ey * dt;
	ezInt += Ki * ez * dt;
		
	/*用叉积误差来做PI修正陀螺零偏，即抵消陀螺读数中的偏移量*/
	gyro.x += Kp * ex + exInt;
	gyro.y += Kp * ey + eyInt;
	gyro.z += Kp * ez + ezInt;
	
	//一阶龙塔法，四元数运动学方程的离散化形式和积分
	q[0] += (-q[1] * gyro.x - q[2] * gyro.y - q[3] * gyro.z) * halfT;
	q[1] += (q[0] * gyro.x + q[2] * gyro.z - q[3] * gyro.y) * halfT;
	q[2] += (q[0] * gyro.y - q[1] * gyro.z + q[3] * gyro.x) * halfT;
	q[3] += (q[0] * gyro.z + q[1] * gyro.y - q[2] * gyro.x) * halfT;
		
	//单位化四元数
	normalise = invSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	q[0] *= normalise;
	q[1] *= normalise;
	q[2] *= normalise;
	q[3] *= normalise;
	
	//四元数的平方
	float q0s, q1s, q2s, q3s;	//四元数的平方
	q0s = q[0] * q[0];
	q1s = q[1] * q[1];
	q2s = q[2] * q[2];
	q3s = q[3] * q[3];
		
	R11 = q0s + q1s - q2s - q3s;	//矩阵(1,1)项
	R21 = 2.0f * (q[1] * q[2] + q[0] * q[3]);	//矩阵(2,1)项
		
	//机体坐标系下的Z方向向量
	vecxZ = 2.0f * (q[1] * q[3] - q[0] * q[2]);//矩阵(3,1)项
	vecyZ = 2.0f * (q[0] * q[1] + q[2] * q[3]);//矩阵(3,2)项
	veczZ = q0s - q1s - q2s + q3s;	//矩阵(3,3)项
		
	if (vecxZ>1)
		vecxZ=1;
	
	if (vecxZ<-1)
		vecxZ=-1;
	
	return true;
}
void mpuGet_6Axis(MPU_6AXIS *const out)		//获取六轴原始数据
{
	*out = g_6axis;
}

float mpuGet_Temp(float *const out)
{
	return (*out = g_oldtemp/340.0f+36.53f);
}

void mpuGet_Angle(MPU_ANGLE *const out)		//获取角度
{
	out->pitch = -asinf(vecxZ) * RAD2DEG;		//计算偏航角
	out->roll = atan2f(vecyZ, veczZ) * RAD2DEG;	//计算翻滚角
	out->yaw = atan2f(R21, R11) * RAD2DEG;		//计算偏航角
}
