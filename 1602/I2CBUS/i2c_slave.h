#ifndef __I2C_SLAVE_H
#define __I2C_SLAVE_H

#include "stm32f0xx.h"
/***************************************/

#define SPECIAL_SETTING  '|'
#define SPECIAL_COMMAND 254 //0xFE: The command to do special HD77480 commands
#define LCD_WIGHT    16
#define LCD_HIGHT    2

/***************************************/

/* Exported constants --------------------------------------------------------*/
#define I2C_TIMING                                0x00210507
#define I2C_TIMEOUT                               0x1000

#define I2C_INT 	1				//采用中断方式发送接收
#define I2C_WHILE	2				//采用循环等待方式发送接收
#define I2C_MODE	I2C_INT			//I2C模式，中断还是循环
						
//#define I2Cx_OWN_ADDRESS7 	 0x0c	//自身地址  0c
#define I2C1_REC_LEN 2				//定义接收缓冲区大小
#define I2C1_TX_LEN  5				//定义发送缓冲区大小

#define I2C1_PORT       GPIOA



#define PIN_I2C1_SDA    GPIO_Pin_10
#define PIN_I2C1_SCL    GPIO_Pin_9

/******************************************************************************
                              定义I2C总线操作结构体
******************************************************************************/

#define I2C_DEVICE_FLAG_DIR_SEND                 ((uint32_t)0x80000000)
#define I2C_DEVICE_FLAG_READ                     ((uint32_t)0x40000000)
#define I2C_DEVICE_FLAG_NACK                     ((uint32_t)0x20000000)
#define I2C_DEVICE_FLAG_WRITE                    ((uint32_t)0x10000000)
#define I2C_DEVICE_FLAG_ERROR                    ((uint32_t)0x08000000)

__packed typedef struct
{
	uint32_t I2C_DeviceFlag;  //操作过程中各种标识集合
	
	uint32_t I2C_EventStatus;
	uint32_t I2C_ErrorStatus;
} I2C_DEVICE_TypeDef;


#define I2C_STATUS_EVT_MASK                      (uint32_t)(0x000000FE)
#define I2C_STATUS_ERR_MASK                      (uint32_t)(0x00000700)	//bit8\9\10,硬件置位的错误标志位


#define IIC_RX_NUM_MAX   128

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;


typedef enum {NOEVENT = 0, EVENT_OPCOD_NOTYET_READ = 1,EVENT_OPCOD_READ =2} EventStatus;

extern unsigned char DataIndex;	//收到的字节数据数索引
extern uint8_t i2c_rx_buf[IIC_RX_NUM_MAX];
extern EventStatus i2c_event;
extern uint8_t I2C_RX_BUF[1];			 //接收数据buffer		
extern uint8_t I2C_TX_BUF[1];			 //发送数据buffer

extern uint16_t RegisterAddress; 		 //接收寄存器地址

extern uint8_t Overflow;				 //溢出标志
extern uint8_t iicrxhead;
extern uint8_t iicrxtail;
extern uint16_t RegisterAddress; 			 //接收寄存器地址
extern uint32_t UniqueID;          //接收唯一ID

void I2C_Slave_Init(void);      /* i2C外设配置初始化 */
void Config_I2C(uint8_t);
void DataProcess(void);
int I2C_RX_Handler(uint8_t DataIndex, uint8_t RevDataByte);
void LCD1602DisplayChar(uint8_t);
void ChangeContrast(uint8_t);
void WriteCGRAM(uint8_t,uint8_t*);
#endif /*__I2C_SLAVE_H*/
