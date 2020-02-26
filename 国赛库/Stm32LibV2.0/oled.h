/********************************************************
			湖南工程学院创新基地
			作者:CHEN
			技术支持:QQ742010094
			日期:2019.6.8
			字符取模设置:阴码,顺向,列行式,16*16
*********************************************************/
#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

//使用内部中文字库
//#define USING_IN_GB2312
//使用外部中文字库
//#define USING_EX_GB2312

#ifdef USING_EX_GB2312
#include "flash.h"
#define FontAddr	0x40000
#endif //USING_EX_GB2312

//OLED初始化
void OLED_Init(void);
//输出ASCII字符
int OLED_Putc(int ch);
//使用中文字模显示汉字
int OLED_GB2312(const uint8_t font[][16],const uint8_t n);
//设置写入位置(行,列)
bool OLED_SetXY(uint8_t line,uint8_t list);
//清除指定位置指定长度的字符
void OLED_ClearXY(const uint8_t line,const uint8_t list,const uint8_t n);
//相当于回删
void OLED_BackSpace(uint8_t sz);
//清除一行
void OLED_ClearLine(const uint8_t line);
//清除屏幕
void OLED_ClearAll(void);
//不带显示位置的字符串输出函数
int OLED_PutString(const char *str);
//带显示位置的字符串输出函数
int OLED_PutStringAt(uint8_t line,uint8_t list,const char *str);
//不带显示位置的printf函数重定义
int OLED_Printf(const char *format,...);
//带显示位置的printf函数重定义
int OLED_PrintfAt(uint8_t line,uint8_t list,const char *format,...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__OLED_H
