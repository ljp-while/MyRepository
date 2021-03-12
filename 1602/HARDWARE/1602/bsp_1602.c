#include "bsp_1602.h" 


void GPIO_Configuration(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);//打开时钟GPIOA时钟
       // RS RW E IO初始化GPIOA5~7 	dataIO GPIOA0~3 初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);	
}

void DataPinHigh(void)
{
	LCD_D4_HIGH();
	LCD_D5_HIGH();
	LCD_D6_HIGH();
	LCD_D7_HIGH();
}

/* 检测液晶忙不忙 */
void LCD1602_Wait_Ready(void)
{
	int8_t sta;
	DataPinHigh();
	delay_us(2);
	LCD_RS_LOW();    	 //RS 0
	delay_us(2);
	LCD_RW_HIGH();     //RW 1
	delay_us(2);
	do
	{
		LCD_EN_HIGH();   //EN 1
		delay_ms(5);	//延时5MS
 		sta = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);//读取状态字
		LCD_EN_LOW();  //EN  0
	}while(sta & 0x80);//bit7等于1表示忙,重复检测到其为0停止
}
 
/*向1602写入一字节命令,cmd-待写入命令 */
//写入指令PA4-7 RS-0  RW-0
void LCD1602_Write_Cmd(uint8_t cmd)
{
	LCD1602_Wait_Ready();  //判断忙不忙
	LCD_RS_LOW();   
	delay_us(2);
	LCD_RW_LOW();
	delay_us(2);
	DataPinOut(cmd);  //先传高四位
	delay_us(2);
	LCD_EN_HIGH();
	delay_us(2);
	LCD_EN_LOW();
	delay_us(2);
	
	DataPinOut(cmd<<4); //低四位左移到高四位
	LCD_EN_HIGH();
	delay_us(2);
	LCD_EN_LOW();
}
 
/* 向1602写入一字节数据*/
void LCD1602_Write_Dat(u8 dat)
{
	LCD1602_Wait_Ready(); //判断忙不忙
	LCD_RS_HIGH();   //1
	delay_us(2);
	LCD_RW_LOW();   //0
	delay_us(2);
	DataPinOut(dat);   //先传高四位
	LCD_EN_HIGH();  
	delay_us(2);
	LCD_EN_LOW();
	delay_us(2);
	DataPinOut(dat<<4); //低四位左移到高四位
	LCD_EN_HIGH(); 
	delay_us(2);
	LCD_EN_LOW();
	delay_us(2);
}
 
void DataPinOut(uint8_t data)
{
	if(data&0x80)
	{
		LCD_D7_HIGH();
	}
	else
	{
		LCD_D7_LOW();
	}
	
	if(data&0x40)
	{
		LCD_D6_HIGH();
	}
	else
	{
		LCD_D6_LOW();
	}
	
	if(data&0x20)
	{
		LCD_D5_HIGH();
	}
	else
	{
		LCD_D5_LOW();
	}
	
	if(data&0x10)
	{
		LCD_D4_HIGH();
	}
	else
	{
		LCD_D4_LOW();
	}
}
/*清屏*/
void LCD1602_ClearScreen(void)
{
	LCD1602_Write_Cmd(0x01);
	
}
 
/* 设置RAM其实地址,即光标位置,（X,Y）对应屏幕上的字符坐标 */
void LCD1602_Set_Cursor(uint8_t x, uint8_t y)
{
	u8 addr;
	
	if (y == 0)
		addr = 0x00 + x;
	else
		addr = 0x40 + x;
	LCD1602_Write_Cmd(addr | 0x80);
}
 
/* 在液晶上显示字符串,（X,Y）-对应屏幕上的其实坐标，str-字符串指针 */
void LCD1602_Show_Str(uint8_t x, uint8_t y, uint8_t *str)
{
	LCD1602_Set_Cursor(x, y);
	while(*str != '\0')
	{
		LCD1602_Write_Dat(*str++);
	}
}
 
/* 初始化液晶 */
void LCD1602_Init(void)
{
	LCD1602_Write_Cmd(0x28);	//16*2显示，5*7点阵，4位数据口
	LCD1602_Write_Cmd(0x0C);	//开显示，光标关闭
	LCD1602_Write_Cmd(0x06);	//文字不动，地址自动+1
	LCD1602_Write_Cmd(0x01);	//清屏
}

