#include "sys.h"


/*
 在Cortex-M3里面有一个外设叫DWT(Data Watchpoint and Trace)，该外设有一个32位的寄存器叫CYCCNT，它是一个向上的
 计数器，记录的是内核时钟运行的个数，最长能记录的时间为：60s=2的32次方/72000000(假设内核频率为72M，内核跳一次的时间大概为1/72M=14ns)
 当CYCCNT溢出之后，会清0重新开始向上计数。
 使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 */


#define  DWT_CR      *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004
#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/* 初始化时间戳 */
//DWT定时器初始化，使用延时函数之前必须使能
void DWT_Init(void)
{
	/* 使能DWT外设 */
	DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

	/* DWT CYCCNT寄存器计数清0 */
	DWT_Write(0);

	/* 使能Cortex-M3 DWT CYCCNT寄存器 */
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

//重载time标准库clock函数
clock_t clock()
{
	return DWT_Read();
}
