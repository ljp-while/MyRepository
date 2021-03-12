#include "i2c_bus.h"
#include "delay.h"
#include "sys.h"

int main(void)
{	
	uint8_t value=0;
	uint16_t count=1;
	uint8_t buffer[]="123456789";
	uint8_t table[]={0x10,0x06,0x09,0x08,0x08,0x09,0x06,0x00};             //要显示的内容放入数组table
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级 	LED_Init();			     //LED端口初始化
	Soft_I2C_Init();

	IIC_SeadChar(0x18,'|');		//清屏
	IIC_SeadChar(0x18,'-');
//	delay_ms(500);
//	IIC_SendString(0x18,buffer,sizeof(buffer)-1);
//	

//	

//	delay_ms(1000);
//	delay_ms(1000);
//	for(count=0;count<8;count++)
//	{
//		IIC_SeadChar(0x18,'|');
//		IIC_SeadChar(0x18,27+count);		
//		IIC_SendString(0x18,table,sizeof(table));
//	}
//	delay_ms(10);	
//	for(count=0;count<8;count++)
//	{
//		IIC_SeadChar(0x18,'|');	
//		IIC_SeadChar(0x18,35+count);		
//	}		
	
/**********改变IIC地址**************/
		
//	IIC_SeadChar(0x18,'|');
//	IIC_SeadChar(0x18,25);
//	IIC_SeadChar(0x18,0x0d);		
		
/**********改变IIC地址**************/
	while(1) 
	{	
/**********自定义对比度*************/	
//		IIC_SeadChar(0x18,'|');
//		IIC_SeadChar(0x18,24);
//		IIC_SeadChar(0x18,count);
//		delay_ms(200);
//		count+=10;
//		if(count>=250)
//		{
//			count=1;
//		}
/**********自定义对比度**************/
		
		
/********改变IIC地址+自定义对比度********/
//		IIC_SeadChar(0x1a,'|');
//		IIC_SeadChar(0x1a,24);
//		IIC_SeadChar(0x1a,count);
//		delay_ms(200);
//		count+=10;
//		if(count>=250)
//		{
//			count=1;
//		}
//		  
/********改变IIC地址+自定义对比度********/
	}


}


