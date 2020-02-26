#ifndef __FLASH_H
#define __FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif //__cplusplus

#include "sys.h"



#define FLASH_ID    0XEF3013    //W25x40C

#define SPI_FLASH_PageSize          256
#define SPI_FLASH_PerWritePageSize  256

/*命令定义-开头*******************************/
#define W25X_WriteEnable		    0x06 
#define W25X_WriteDisable		    0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			    0x03 
#define W25X_FastReadData		    0x0B 
#define W25X_FastReadDual		    0x3B 
#define W25X_PageProgram		    0x02 
#define W25X_BlockErase			    0xD8 
#define W25X_SectorErase		    0x20 
#define W25X_ChipErase			    0xC7 
#define W25X_PowerDown			    0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			    0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F	 
	
	
//发送一个字节到flash
uint8_t FLASH_SendData(uint8_t data);
//flash写使能
void FLASH_WriteEnable(void);
//读取flash一个字节
uint8_t FLASH_ReadByte(uint32_t ReadAddr);
//写入一个字节到flash
void FLASH_WriteByte(uint8_t data,uint32_t WriteAddr);
//块擦除
void FLASH_BlockErase(uint32_t addr);
//全片擦除
void FLASH_ChipkErase(void);
//读取设备ID
uint32_t FLASH_NewReadDeviceID(void);
//flash写入
void FLASH_PageWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
//flash读取
void FLASH_PageRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
//设置状态寄存器
void FLASH_WriteStaus(uint8_t data);
//读取状态寄存器
uint8_t FLASH_ReadStaus(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__FLASH_H
