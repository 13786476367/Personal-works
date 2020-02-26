#include "sys.h"


/*
 ��Cortex-M3������һ�������DWT(Data Watchpoint and Trace)����������һ��32λ�ļĴ�����CYCCNT������һ�����ϵ�
 ����������¼�����ں�ʱ�����еĸ�������ܼ�¼��ʱ��Ϊ��60s=2��32�η�/72000000(�����ں�Ƶ��Ϊ72M���ں���һ�ε�ʱ����Ϊ1/72M=14ns)
 ��CYCCNT���֮�󣬻���0���¿�ʼ���ϼ�����
 ʹ��CYCCNT�����Ĳ������裺
 1����ʹ��DWT���裬����������ں˵��ԼĴ���DEMCR��λ24���ƣ�д1ʹ��
 2��ʹ��CYCCNT�Ĵ���֮ǰ������0
 3��ʹ��CYCCNT�Ĵ����������DWT_CTRL(�����Ϻ궨��ΪDWT_CR)��λ0���ƣ�д1ʹ��
 */


#define  DWT_CR      *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/* ��ʼ��ʱ��� */
//DWT��ʱ����ʼ����ʹ����ʱ����֮ǰ����ʹ��
void DWT_Init(void)
{
	/* ʹ��DWT���� */
	DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

	/* DWT CYCCNT�Ĵ���������0 */
	DWT_Write(0);

	/* ʹ��Cortex-M3 DWT CYCCNT�Ĵ��� */
	DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}
__INLINE void DWT_Write(uint32_t timer)
{
	DWT_CYCCNT = timer;
}
__INLINE uint32_t DWT_Read(void)
{ 
	return DWT_CYCCNT;
}

__INLINE void delay_us(uint32_t tick)
{
	uint32_t start = DWT_Read();
	tick = tick * (SystemCoreClock / 1000000);
	while(DWT_Read()-start<tick);
}

__INLINE void delay_ms(uint32_t tick)
{
	uint32_t start = DWT_Read();
	tick = tick * 1000 * (SystemCoreClock / 1000000);
	while(DWT_Read()-start<tick);
}

//����time��׼��clock����
clock_t clock()
{
	return DWT_Read();
}
