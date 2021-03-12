#ifndef __I2C_SLAVE_H
#define __I2C_SLAVE_H

#include "stm32f0xx.h"

typedef enum
{
  I2C_OK                                          = 0,
  I2C_FAIL                                        = 1
}I2C_Status;

/* Exported constants --------------------------------------------------------*/
#define I2C_TIMING                                0x00210507
#define I2C_TIMEOUT                               0x1000

#define I2C_INT 	1				//采用中断方式发送接收
#define I2C_WHILE	2				//采用循环等待方式发送接收
#define I2C_MODE	I2C_INT			//I2C模式，中断还是循环
						
#define I2Cx_OWN_ADDRESS7 	 0x0c	//自身地址  0c
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



typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;


typedef struct {
	uint8_t beginOperate;
  uint16_t index ;
	uint16_t RegisterAddress;
	uint32_t value;
	uint16_t sensor_address;
	uint16_t sensor_value;
} CMD_TypeDef;
extern CMD_TypeDef command;


typedef enum {NOEVENT = 0, EVENT_OPCOD_NOTYET_READ = 1,EVENT_OPCOD_READ =2} EventStatus;

extern uint8_t i2c_rx_buf[4];
extern EventStatus i2c_event;
extern uint8_t I2C_RX_BUF[1];			 //接收数据buffer		
extern uint8_t I2C_TX_BUF[1];			 //发送数据buffer
extern unsigned char revSta; 			 //接收状态寄存器  
extern unsigned char revcmd;  		     //接收的命令
extern uint16_t RegisterAddress; 		 //接收寄存器地址

extern u32 I2CxRecCnt;					 //接收数据个数计数
extern u8 ReadDat[2];					 //数据
extern u32 I2CxRecCntCopy;
extern u32 IdleCnt;
extern u8  I2CxFlage;
extern uint8_t Overflow;				 //溢出标志
	

extern uint32_t CalculateCheckSum; //计算累加检验和
extern uint32_t ReceiveCheckSum;   //接收累加检验和

extern uint32_t UniqueID;          //接收唯一ID

void I2C_Slave_Init(void);      /* i2C外设配置初始化 */

extern unsigned char revSta; 				 //接收状态寄存器  
extern unsigned char revcmd;  		  		 //接收的命令
extern uint16_t RegisterAddress; 			 //接收寄存器地址
	

void Config_I2C(void);



#endif /*__I2C_SLAVE_H*/
