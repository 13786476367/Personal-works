/********************************************************
			���Ϲ���ѧԺ���»���
			����:CHEN
			����֧��:QQ742010094
			����:2019.6.8
			�ַ�ȡģ����:����,˳��,����ʽ,16*16
*********************************************************/
#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"

//ʹ���ڲ������ֿ�
//#define USING_IN_GB2312
//ʹ���ⲿ�����ֿ�
//#define USING_EX_GB2312

#ifdef USING_EX_GB2312
#include "flash.h"
#define FontAddr	0x40000
#endif //USING_EX_GB2312

//OLED��ʼ��
void OLED_Init(void);
//���ASCII�ַ�
int OLED_Putc(int ch);
//ʹ��������ģ��ʾ����
int OLED_GB2312(const uint8_t font[][16],const uint8_t n);
//����д��λ��(��,��)
bool OLED_SetXY(uint8_t line,uint8_t list);
//���ָ��λ��ָ�����ȵ��ַ�
void OLED_ClearXY(const uint8_t line,const uint8_t list,const uint8_t n);
//�൱�ڻ�ɾ
void OLED_BackSpace(uint8_t sz);
//���һ��
void OLED_ClearLine(const uint8_t line);
//�����Ļ
void OLED_ClearAll(void);
//������ʾλ�õ��ַ����������
int OLED_PutString(const char *str);
//����ʾλ�õ��ַ����������
int OLED_PutStringAt(uint8_t line,uint8_t list,const char *str);
//������ʾλ�õ�printf�����ض���
int OLED_Printf(const char *format,...);
//����ʾλ�õ�printf�����ض���
int OLED_PrintfAt(uint8_t line,uint8_t list,const char *format,...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__OLED_H
