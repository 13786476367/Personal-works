#ifndef __SYSTEMINIT_H
#define __SYSTEMINIT_H
#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include <stm32f10x.h>
#include "main.h"
	 
void USART3_DMA_Config(void *SendBuff);
void InitSystem(void);
	 
	 
	 
	 
	 
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__SYSTEMINIT_H
