#ifndef __MENU_H
#define __MENU_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"
#include "oled.h"
#include "keyboard.h"
#include "pid.h"
#include "math.h"
#include "stdlib.h"
#include "mpu6050.h"
#include "anotc_v6.h"

extern void (*fun)(pANOTC anotc);
void SelectMode_0(pANOTC anotc);
void SelectMode_1(pANOTC anotc);
void SelectMode_2(pANOTC anotc);
void SelectMode_3(pANOTC anotc);
void SelectMode_4(pANOTC anotc);
void SelectMode_5(pANOTC anotc);
void SetPwm(short moto1,short moto2);
extern MPU6050 g_mpu6050;
void Menu(void);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__MENU_H
