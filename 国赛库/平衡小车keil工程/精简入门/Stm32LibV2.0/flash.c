#include "flash.h"


//GPIO���ŵĳ�ʼ�����û����

#define SPI_SCK 	PAout(5)		//CLK       �������

#define SPI_MOSI	PAout(7)		//DI0(MOSI) �������
#define READ_MOSI 	PAin(7)

#define SPI_MISO 	PAin(6)		  //D0��MISO  �����������������

#define SPI_CS		PBout(14)		//SS				ͨ��I/O�����������
#define READ_CS   	PBin(14)

#define delay()	__asm("nop")


/* WIP(busy)��־��FLASH�ڲ�����д�� */
#define WIP_Flag                  0x01
#define Dummy_Byte                0xFF

//����һ���ֽڵ�flash
uint8_t FLASH_SendData(uint8_t data)  
{  
	
	uint8_t RecevieData=0,i;  
	for(i=0;i<8;i++)  
	{
		SPI_SCK = 0;
		RecevieData <<= 1;
		if(data&0x80)//����һλ����
			SPI_MOSI = 1;//���������Ϊ��
		else
			SPI_MOSI = 0;//���������Ϊ��
		SPI_SCK = 1;      //�ɼ�����
		data<<=1;

		if(SPI_MISO)
			RecevieData |= 1;	
		delay();	
	}  

	return RecevieData;  
}

//����ָ��
void FLASH_SendCmd(uint8_t cmd)  
{  
	uint8_t i;  
	for(i=0;i<8;i++)  
	{
		SPI_SCK = 0;
		if(cmd&0x80)//����һλ����
			SPI_MOSI = 1;//���������Ϊ��
		else
			SPI_MOSI = 0;//���������Ϊ��
		SPI_SCK = 1;      //�ɼ�����
		cmd<<=1;
		delay();
	}
}


//flashдʹ��
void FLASH_WriteEnable(void)
{
	SPI_CS = 0;
	FLASH_SendCmd(W25X_WriteEnable);
	SPI_CS = 1;
}


//�Ⱥ�flashæ״̬
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
*flash��ȡһ���ֽ�
*��ڲ�����ReadAddr����ȡ�ĵ�ַ
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
*flashд��һ���ֽ�
*��ڲ�����data��  Ҫд�������
*��ڲ�����WriteAddr��д���ַ
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
*flashҳд��
*��ڲ�����pBuffer��  Ҫд�������ָ��
*��ڲ�����WriteAddr��д���ַ
*��ڲ�����NumByteToWrite��д�����ݳ���
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
*flashҳ��ȡ
*��ڲ�����pBuffer��  �洢��ȡ���ݵ�����ָ��
*��ڲ�����ReadAddr�� ��ȡ��ַ
*��ڲ�����NumByteToWrite����ȡ���ݳ���
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


//ȫƬ����
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
* Description:  ��ȡ�豸ID
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

//����״̬�Ĵ���
void FLASH_WriteStaus(uint8_t data)
{
	FLASH_WriteEnable();

	SPI_CS = 0;
	FLASH_SendCmd(W25X_WriteStatusReg);
	FLASH_SendData(data);
	SPI_CS = 1;

}



//��flash״̬�Ĵ���
uint8_t FLASH_ReadStaus(void)
{
	uint8_t FLASH_Status = 0;
	SPI_CS = 0;
	FLASH_SendCmd(W25X_ReadStatusReg);
	FLASH_Status = FLASH_SendData(Dummy_Byte);
	SPI_CS = 1;
	return FLASH_Status;
}
