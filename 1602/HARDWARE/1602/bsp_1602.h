#ifndef  __BSP_1602_H
#define  __BSP_1602_H

#include "includes.h"


#define LCD_DATA_PORT GPIOA
 
//1602指令/数据 引脚
#define	LCD_RS_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_5)
#define	LCD_RS_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_5)
 
//1602读写引脚
#define	LCD_RW_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_6)
#define	LCD_RW_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_6)
 
//1602使能引脚
#define	LCD_EN_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_7)
#define	LCD_EN_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_7)

#define LCD_D4_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_0)
#define LCD_D4_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_0)

#define LCD_D5_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_1)
#define LCD_D5_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_1)

#define LCD_D6_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_2)
#define LCD_D6_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_2)

#define LCD_D7_HIGH()	GPIO_SetBits(LCD_DATA_PORT, GPIO_Pin_3)
#define LCD_D7_LOW()	GPIO_ResetBits(LCD_DATA_PORT, GPIO_Pin_3)


//1602数据端口	PD4~7
#define	DATAOUT(x)	GPIO_Write(LCD_DATA_PORT, x)
 
 
void GPIO_Configuration(void);
void LCD1602_Wait_Ready(void);
void LCD1602_Write_Cmd(uint8_t cmd);
void LCD1602_Write_Dat(uint8_t dat);
void LCD1602_ClearScreen(void);
void LCD1602_Set_Cursor(uint8_t x, uint8_t y);
void LCD1602_Show_Str(uint8_t x, uint8_t y, uint8_t *str);
void LCD1602_Init(void);
void DataPinHigh(void);
void DataPinOut(uint8_t);

#endif /*__BSP_1602_H*/
