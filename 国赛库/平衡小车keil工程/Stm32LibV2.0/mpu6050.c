#include "mpu6050.h"
#include <math.h>


//���ļ�ʹ�õĺ궨��
#define mpu6050_delay delay_ms	//��ʱ����

#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)

#define	ACCEL_XOUT_H	0x3B	//X����ٶȸ߰�λ
#define	ACCEL_XOUT_L	0x3C	//X����ٶȵͰ�λ
#define	ACCEL_YOUT_H	0x3D	//Y����ٶȸ߰�λ
#define	ACCEL_YOUT_L	0x3E	//Y����ٶȵͰ�λ
#define	ACCEL_ZOUT_H	0x3F	//Z����ٶȸ߰�λ
#define	ACCEL_ZOUT_L	0x40	//Z����ٶȵͰ�λ

#define	TEMP_OUT_H		0x41	//�������¶ȸ߰�λ
#define	TEMP_OUT_L		0x42	//�������¶ȵͰ�λ

#define	GYRO_XOUT_H		0x43	//X����ٶȸ߰�λ
#define	GYRO_XOUT_L		0x44	//X����ٶȵͰ�λ	
#define	GYRO_YOUT_H		0x45	//Y����ٶȸ߰�λ
#define	GYRO_YOUT_L		0x46	//Y����ٶȵͰ�λ
#define	GYRO_ZOUT_H		0x47	//Z����ٶȸ߰�λ
#define	GYRO_ZOUT_L		0x48	//Z����ٶȵͰ�λ

//�ɱ��ļ�ά���ľ�̬����
static MPU_6AXIS g_6axis;		//��������
static short g_oldtemp;			//�������¶�ԭʼֵ

static float q[4]={1.0f,0,0,0};		//��Ԫ��
static float R11,R21;				//��Ԫ��3D��ת����(1,1),(2,1)��
static float vecxZ, vecyZ, veczZ;	//��������ϵ�µ�Z��������


static float invSqrt(float x)	/*���ٿ�ƽ���󵹣�����ֵ��*/
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

bool mpu6050_init()//�����ʼ��ʧ�ܷ���false�����򷵻�true
{
	unsigned char who=0;
	if(mpu6050_WriteReg(MPU_ADDRESS,0x6B,6,1,0)==false)	//������߱�־λ�����빤��״̬
		return false;
	mpu6050_delay(200);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x6B,3,4,1)==false)	//����X������Ϊϵͳʱ��Դ
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x1B,4,2,3)==false)	//����������������Ϊ+-2000��/S
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x1C,4,2,1)==false)	//���ü��ٶȼ�����Ϊ+-4g
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x1A,2,3,3)==false)	//�������ֵ�ͨ�˲������ٶȣ�����44Hz���ӳ�4.9ms�����ٶȣ�����42Hz���ӳ�4.8ms��
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0X6A,5,1,0)==false)	//ʹ��I2Cģʽ
		return false;
	mpu6050_delay(50);
	if(mpu6050_WriteReg(MPU_ADDRESS,0x37,1,1,1)==false)		//ʹ��I2C��ͨģʽ
		return false;
	i2c_read(MPU_ADDRESS,0X75,1,&who);
	if(who==0)//��У��
		return false;
	return true;
}

#define FIFO_SIZE	5
static bool mpu6050_read(void)
{
	unsigned char buff[14];		//������
	register int i;	//��ʱ����ֵ�������üĴ�������
	int tempax,tempay,tempaz,tempgx,tempgy,tempgz;
	static MPU_6AXIS fifo[FIFO_SIZE];
	static int count=0;
	if(i2c_read(MPU_ADDRESS,0x3b,14,buff))//�����ȡʧ��
		return false;
	if(count >= FIFO_SIZE)
		count = 0;
	fifo[count].acc.x = ((buff[0] << 8) | buff[1]);		//��ȡ���ٶ�ԭʼֵ
	fifo[count].acc.y = ((buff[2] << 8) | buff[3]);
	fifo[count].acc.z = ((buff[4] << 8) | buff[5]);
	
	g_oldtemp = (buff[6]<<8 | buff[7]);					//��ȡ�¶�ԭʼֵ
	
	fifo[count].gyro.x = ((buff[8] << 8) | buff[9]);	//��ȡ���ٶ�ԭʼֵ
	fifo[count].gyro.y = ((buff[10] << 8) | buff[11]);
	fifo[count].gyro.z = ((buff[12] << 8) | buff[13]);
	count++;
	
	//Ϊ���ٶȺͽ��ٶȽ��л����˲�
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

bool ImuUpdata(float dt)		//������Ԫ����3D��ת����,�ɹ�����trueʧ�ܷ���false
{
	static float exInt = 0.0f,eyInt = 0.0f,ezInt = 0.0f;	//��������ۼ�
	const float Kp = 2.0f;		//��������
	const float Ki = 0.002f;		//��������
	const float halfT =0.5f * dt;
	
	float normalise;			
	FLOAT_3AXIS acc;
	FLOAT_3AXIS gyro;
	if(mpu6050_read()==false)
		return false;
	if(g_6axis.acc.x==0||g_6axis.acc.y==0||g_6axis.acc.z==0)	//�����ǰ�����������˶�
		return false;
	
	
	//��ȡ�����ƵĽ��ٶ�
	gyro.x = g_6axis.gyro.x*MPU6500_DEG_PER_LSB_2000*DEG2RAD;
	gyro.y = g_6axis.gyro.y*MPU6500_DEG_PER_LSB_2000*DEG2RAD;
	gyro.z = g_6axis.gyro.z*MPU6500_DEG_PER_LSB_2000*DEG2RAD;
		
	//��λ�����ټƲ���ֵ
	normalise = invSqrt(g_6axis.acc.x*g_6axis.acc.x+g_6axis.acc.y*g_6axis.acc.y+g_6axis.acc.z*g_6axis.acc.z);	//���ٿ�������invSqrt
	acc.x = g_6axis.acc.x*normalise;
	acc.y = g_6axis.acc.y*normalise;
	acc.z = g_6axis.acc.z*normalise;
		
	//���ټƶ�ȡ�ķ������������ټƷ���Ĳ�ֵ����������˼���
	float ex, ey, ez;
	ex = (acc.y * veczZ - acc.z * vecyZ);
	ey = (acc.z * vecxZ - acc.x * veczZ);
	ez = (acc.x * vecyZ - acc.y * vecxZ);
		
	//����ۼƣ�����ֳ������
	exInt += Ki * ex * dt;  
	eyInt += Ki * ey * dt;
	ezInt += Ki * ez * dt;
		
	/*�ò���������PI����������ƫ�����������ݶ����е�ƫ����*/
	gyro.x += Kp * ex + exInt;
	gyro.y += Kp * ey + eyInt;
	gyro.z += Kp * ez + ezInt;
	
	//һ������������Ԫ���˶�ѧ���̵���ɢ����ʽ�ͻ���
	q[0] += (-q[1] * gyro.x - q[2] * gyro.y - q[3] * gyro.z) * halfT;
	q[1] += (q[0] * gyro.x + q[2] * gyro.z - q[3] * gyro.y) * halfT;
	q[2] += (q[0] * gyro.y - q[1] * gyro.z + q[3] * gyro.x) * halfT;
	q[3] += (q[0] * gyro.z + q[1] * gyro.y - q[2] * gyro.x) * halfT;
		
	//��λ����Ԫ��
	normalise = invSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	q[0] *= normalise;
	q[1] *= normalise;
	q[2] *= normalise;
	q[3] *= normalise;
	
	//��Ԫ����ƽ��
	float q0s, q1s, q2s, q3s;	//��Ԫ����ƽ��
	q0s = q[0] * q[0];
	q1s = q[1] * q[1];
	q2s = q[2] * q[2];
	q3s = q[3] * q[3];
		
	R11 = q0s + q1s - q2s - q3s;	//����(1,1)��
	R21 = 2.0f * (q[1] * q[2] + q[0] * q[3]);	//����(2,1)��
		
	//��������ϵ�µ�Z��������
	vecxZ = 2.0f * (q[1] * q[3] - q[0] * q[2]);//����(3,1)��
	vecyZ = 2.0f * (q[0] * q[1] + q[2] * q[3]);//����(3,2)��
	veczZ = q0s - q1s - q2s + q3s;	//����(3,3)��
		
	if (vecxZ>1)
		vecxZ=1;
	
	if (vecxZ<-1)
		vecxZ=-1;
	
	return true;
}
void mpuGet_6Axis(MPU_6AXIS *const out)		//��ȡ����ԭʼ����
{
	*out = g_6axis;
}

float mpuGet_Temp(float *const out)
{
	return (*out = g_oldtemp/340.0f+36.53f);
}

void mpuGet_Angle(MPU_ANGLE *const out)		//��ȡ�Ƕ�
{
	out->pitch = -asinf(vecxZ) * RAD2DEG;		//����ƫ����
	out->roll = atan2f(vecyZ, veczZ) * RAD2DEG;	//���㷭����
	out->yaw = atan2f(R21, R11) * RAD2DEG;		//����ƫ����
}
