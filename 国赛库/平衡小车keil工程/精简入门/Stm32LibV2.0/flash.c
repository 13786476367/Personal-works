#include "flash.h"


//GPIO引脚的初始化由用户完成

#define SPI_SCK 	PAout(5)		//CLK       推挽输出

#define SPI_MOSI	PAout(7)		//DI0(MOSI) 推挽输出
#define READ_MOSI 	PAin(7)

#define SPI_MISO 	PAin(6)		  //D0（MISO  上拉输入或下拉输入

#define SPI_CS		PBout(14)		//SS				通用I/O（推挽输出）
#define READ_CS   	PBin(14)

#define delay()	__asm("nop")


/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                  0x01
#define Dummy_Byte                0xFF

//发送一个字节到flash
uint8_t FLASH_SendData(uint8_t data)  
{  
	
	uint8_t RecevieData=0,i;  
	for(i=0;i<8;i++)  
	{
		SPI_SCK = 0;
		RecevieData <<= 1;
		if(data&0x80)//发送一位数据
			SPI_MOSI = 1;//传输的数据为高
		else
			SPI_MOSI = 0;//传输的数据为低
		SPI_SCK = 1;      //采集数据
		data<<=1;

		if(SPI_MISO)
			RecevieData |= 1;	
		delay();	
	}  

	return RecevieData;  
}

//发送指令
void FLASH_SendCmd(uint8_t cmd)  
{  
	uint8_t i;  
	for(i=0;i<8;i++)  
	{
		SPI_SCK = 0;
		if(cmd&0x80)//发送一位数据
			SPI_MOSI = 1;//传输的数据为高
		else
			SPI_MOSI = 0;//传输的数据为低
		SPI_SCK = 1;      //采集数据
		cmd<<=1;
		delay();
	}
}


//flash写使能
void FLASH_WriteEnable(void)
{
	SPI_CS = 0;
	FLASH_SendCmd(W25X_WriteEnable);
	SPI_CS = 1;
}


//等候flash忙状态
void FLASH_WaitBusy(void)
{
	uint8_t Flash_Status = 0;
	SPI_CS = 0;
	FLASH_SendCmd(W25X_ReadStatusReg);
	do
	{
		Flash_Status = FLASH_SendData(Dummy_Byte);
	}
	while((Flash_Status & WIP_Flag) == SET);
	SPI_CS = 1;
}


/*
*flash读取一个字节
*入口参数：ReadAddr：读取的地址
*/
uint8_t FLASH_ReadByte(uint32_t ReadAddr)
{
	uint32_t RecevieData=0;
	SPI_CS = 0;
	FLASH_SendCmd(W25X_ReadData);
	FLASH_SendData((ReadAddr & 0xFF0000) >> 16);
	FLASH_SendData((ReadAddr& 0xFF00) >> 8);
	FLASH_SendData(ReadAddr & 0xFF);
	
	RecevieData = FLASH_SendData(Dummy_Byte);
	
	SPI_CS = 1;
	
	return RecevieData;
}


/*
*flash写入一个字节
*入口参数：data，  要写入的数据
*入口参数：WriteAddr，写入地址
*/
void FLASH_WriteByte(uint8_t data,uint32_t WriteAddr)
{
	FLASH_WriteEnable();
	FLASH_WaitBusy();
	SPI_CS = 0;
	
	FLASH_SendCmd(W25X_PageProgram);
	FLASH_SendData((WriteAddr&0XFF0000)>>16);
	FLASH_SendData((WriteAddr&0XFF00)>>8);
	FLASH_SendData(WriteAddr&0XFF);
	
	FLASH_SendData(data);
	
	SPI_CS = 1;
	FLASH_WaitBusy();
}

/*
*flash页写入
*入口参数：pBuffer，  要写入的数据指针
*入口参数：WriteAddr，写入地址
*入口参数：NumByteToWrite，写入数据长度
*/
void FLASH_PageWrite(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	FLASH_WriteEnable();
	FLASH_WaitBusy();
	SPI_CS = 0;	
	FLASH_SendCmd(W25X_PageProgram);
	FLASH_SendData((WriteAddr&0XFF0000)>>16);
	FLASH_SendData((WriteAddr&0XFF00)>>8);
	FLASH_SendData(WriteAddr&0XFF);
	
	while(NumByteToWrite--)
	{
		FLASH_SendData(*pBuffer);
		pBuffer++;
	}
	
	SPI_CS = 1;
	FLASH_WaitBusy();
}	


/*
*flash页读取
*入口参数：pBuffer，  存储读取数据的数据指针
*入口参数：ReadAddr， 读取地址
*入口参数：NumByteToWrite，读取数据长度
*/
void FLASH_PageRead(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{
	SPI_CS = 0;
	FLASH_SendCmd(W25X_ReadData);
	FLASH_SendData((ReadAddr&0XFF0000)>>16);
	FLASH_SendData((ReadAddr&0XFF00)>>8);
	FLASH_SendData(ReadAddr&0XFF);
	while(NumByteToRead--)
	{
		*pBuffer = FLASH_SendData(Dummy_Byte);
		pBuffer++;
	}
	
	SPI_CS = 1;
}

/* 
* Function:     FLASH_BlockErase
* Description:  BlockErase:      64KByte = 256Byte
*/
void FLASH_BlockErase(uint32_t addr)
{
	FLASH_WriteEnable();
	FLASH_WaitBusy();
	SPI_CS = 0;
	FLASH_SendCmd(W25X_BlockErase);
	FLASH_SendData((addr>>16)&0xFF);
	FLASH_SendData((addr>>8)&0xFF); 
	FLASH_SendData(addr&0xFF);
	SPI_CS = 1;
	FLASH_WaitBusy();
}


//全片擦除
void FLASH_ChipkErase(void)
{
	FLASH_WriteEnable();
	FLASH_WaitBusy();
	SPI_CS = 0;
	FLASH_SendCmd(W25X_ChipErase);
	SPI_CS = 1;
	FLASH_WaitBusy();
}


/*
* Function:  FLASH_ReadDeviceID
* Description:  读取设备ID
*/
uint32_t FLASH_NewReadDeviceID(void)  
{
	FLASH_WriteEnable();
	SPI_CS = 0;
	uint32_t Temp;
	uint8_t Temp0 = 0, Temp1 = 0, Temp2 = 0;
  
	FLASH_SendCmd(W25X_JedecDeviceID);  
	Temp0 = FLASH_SendData(Dummy_Byte);  
	Temp1 = FLASH_SendData(Dummy_Byte);  
	Temp2 = FLASH_SendData(Dummy_Byte);    
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	SPI_CS = 1;
	return Temp;  
} 

//设置状态寄存器
void FLASH_WriteStaus(uint8_t data)
{
	FLASH_WriteEnable();

	SPI_CS = 0;
	FLASH_SendCmd(W25X_WriteStatusReg);
	FLASH_SendData(data);
	SPI_CS = 1;

}



//读flash状态寄存器
uint8_t FLASH_ReadStaus(void)
{
	uint8_t FLASH_Status = 0;
	SPI_CS = 0;
	FLASH_SendCmd(W25X_ReadStatusReg);
	FLASH_Status = FLASH_SendData(Dummy_Byte);
	SPI_CS = 1;
	return FLASH_Status;
}
