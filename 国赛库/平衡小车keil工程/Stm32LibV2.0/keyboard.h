#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus
#include "sys.h"

//键值码宏定义
#define		KEY_F1			0X07
#define		KEY_F2			0X06
#define		KEY_F3			0X05
#define		KEY_MENU		0X0F
#define		KEY_SHIFT		0X0E
#define		KEY_DEL			0X0D
#define		KEY_ENTER		0X17
#define		KEY_ESC			0X15

#define		KEY_NUM0		0X16
#define		KEY_NUM1		0X2F
#define		KEY_NUM2		0X2E
#define		KEY_NUM3		0X2D
#define		KEY_NUM4		0X27
#define 	KEY_NUM5		0X26
#define		KEY_NUM6		0X25
#define		KEY_NUM7		0X1F
#define		KEY_NUM8		0X1E
#define		KEY_NUM9		0X1D


//初始化键盘
bool KEY_Init(void);
//检查是否有按键消息
bool KEY_ReadINT(void);
//读取键值码
bool KEY_ReadData(uint8_t *data);
//阻塞式读取键值码
uint8_t KEY_GetKey(void);
//键盘码转数字
int KEY_Num(uint8_t key);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__KEYBOARD_H
