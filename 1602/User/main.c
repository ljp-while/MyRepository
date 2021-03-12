#include "includes.h"

int main(void)
{	
	uint8_t count=0;
	uint8_t str[] = "ATOM123ALIENTEK";
//	uint8_t buffer[]={0x01,0x03,0x07,0x0f,0x1f,0x00,0x00,0x00};
	system_init();			//系统初始化函数
	LCD1602_ClearScreen();
	LCD1602_Show_Str(0, 0, str);     
 	LCD1602_Show_Str(0, 1, "I love STM32");
	while(1) 
	{	
		while(iicrxhead!=iicrxtail)			//接受到数据
		{
			DataProcess();					//处理数据
		}		
	}	  
}

