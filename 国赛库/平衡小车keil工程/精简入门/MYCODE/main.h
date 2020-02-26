#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include <stm32f10x.h>
	 
#define KEY2_Pin GPIO_Pin_5
#define KEY2_GPIO_Port GPIOA
#define BIN2_Pin GPIO_Pin_12
#define BIN2_GPIO_Port GPIOB
#define BIN1_Pin GPIO_Pin_13
#define BIN1_GPIO_Port GPIOB
#define AIN1_Pin GPIO_Pin_14
#define AIN1_GPIO_Port GPIOB
#define AIN2_Pin GPIO_Pin_15
#define AIN2_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_Pin_15
#define OLED_DC_GPIO_Port GPIOA
#define OLED_REST_Pin GPIO_Pin_3
#define OLED_REST_GPIO_Port GPIOB
#define OLED_D1_Pin GPIO_Pin_4
#define OLED_D1_GPIO_Port GPIOB
#define OLED_D0_Pin GPIO_Pin_5
#define OLED_D0_GPIO_Port GPIOB
#define SCL_Pin GPIO_Pin_8
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_Pin_9
#define SDA_GPIO_Port GPIOB
	 
	 
	 
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__MAIN_H
