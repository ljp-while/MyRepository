#include "i2c_slave.h"
#include "includes.h"
#include "sensor_mode.h"

struct sensor_info SENSOR;
uint8_t i2c_rx_buf[IIC_RX_NUM_MAX];
uint32_t i2c_tx_buf[4];
uint8_t iicrxhead=0;
uint8_t iicrxtail=0;
uint8_t iicrxdata=0;
uint8_t cursorx=0;
uint8_t cursory=0;
uint8_t customCharData[8]; //Records incoming custom character data
uint8_t customCharSpot = 0 ; //Keeps track of where we are in custCharData array
uint8_t customCharNumber = 0; //LCDs can store 8 custom chars, this keeps track
uint8_t symbol=0;
/********1602屏幕的模式********/
enum Mode_1602
{
	MODE_NORMAL=0,
	MODE_COMMAND,
	MODE_SETTING,
	MODE_CONTRAST,
	MODE_TWI,
	MODE_RECORD_CUSTOM_CHAR
}CurrentMode=MODE_NORMAL;
/********1602屏幕的模式********/


unsigned char DataIndex = 0;	//收到的字节数据数索引
static int Offset = 32;			//发送模式时的移位
int32_t temp = 0;
uint32_t temp1 = 0;
I2C_DEVICE_TypeDef i2c_dev;

//I2C接收中断用到变量
uint8_t I2CFirstByteFlag = 0;  //接收第一个字节数据标志
uint8_t I2CFirstWordFlag = 0;   //接收第一个字的标志
uint16_t RegisterAddress = 0;  //接收寄存器地址
uint32_t UniqueID = 0;          //接收唯一ID

uint8_t dummy = 0;

uint8_t ReadSensorFlag = 0;

extern i2c_detect_focus_motor_t *i2c_detect_focus_motor;




void DataProcess(void)
{
	iicrxdata=i2c_rx_buf[iicrxtail];
	iicrxtail=(iicrxtail+1)%IIC_RX_NUM_MAX;
	if(CurrentMode==MODE_NORMAL)
	{
		if(iicrxdata==SPECIAL_SETTING)
		{
			CurrentMode=MODE_SETTING;
		}
		else if(iicrxdata == SPECIAL_COMMAND)
		{
			
		}
		else
		{
			LCD1602DisplayChar(iicrxdata);			//没有指令  直接打印
		}
	}
	else
	{
		if(CurrentMode==MODE_SETTING)
		{
			CurrentMode=MODE_NORMAL;
			if(iicrxdata>=3&&iicrxdata<=7)			//发送线宽
			{
			}
			else if(iicrxdata==8)			//系统软件重置
			{
			}
			else if(iicrxdata==9)		//启用/禁用启动画面
			{
			}
			else if(iicrxdata==10)		//将当前显示的文本另存为启动画面
			{
			}
			else if(iicrxdata>=11&&iicrxdata<=23)	//改变波特率
			{	
			}
			else if(iicrxdata==24)				//改变对比度
			{
				CurrentMode=MODE_CONTRAST;
			}
			else if(iicrxdata==25)				//改变IIC的地址
			{
				CurrentMode=MODE_TWI;
			}
			else if(iicrxdata==26)				//启动时启用/禁用忽略RX引脚（忽略紧急复位）
			{
				
			}
			else if(iicrxdata>=27&&iicrxdata<=34)		//将一行像素数据记录到自定义字符。
			{
				CurrentMode=MODE_RECORD_CUSTOM_CHAR;
				customCharNumber=iicrxdata-27;
			}
			else if(iicrxdata>=35&&iicrxdata<=42)		//显示自定义字符0到7
			{
				customCharNumber=iicrxdata-35;
				LCD1602DisplayChar(customCharNumber);
			}
			else if(iicrxdata==43)				//功能未确定
			{
				
			}
			else if(iicrxdata==44)				//功能未确定
			{
				
			}
			else if(iicrxdata==45)				//清除显示。将光标移到原始位置。
			{
				LCD1602_ClearScreen();			//清屏
				LCD1602_Set_Cursor(0,0);		//将光标移到原始位置
			}
			else if(iicrxdata==46)				//当设置更改时启用系统消息
			{
				
			}
			else if(iicrxdata==47)				//禁止显示系统消息。
			{
				
			}
			else if(iicrxdata==48)				//开机时启用启动画面
			{
				
			}
			else if(iicrxdata==49)				//开机时禁用启动画面
			{
				
			}
		}
		else if (CurrentMode == MODE_TWI)			//更改IIC地址
		{
			Config_I2C(iicrxdata);					
			CurrentMode=MODE_NORMAL;
		}
		else if (CurrentMode == MODE_CONTRAST)		//更改对比度
		{
			ChangeContrast(iicrxdata);
			CurrentMode=MODE_NORMAL;
		}
		else if(CurrentMode == MODE_RECORD_CUSTOM_CHAR)		//用户自定义字符
		{
			customCharData[customCharSpot]= iicrxdata;
			customCharSpot++;
			if(customCharSpot>7)			//存储到8个字符后  处理自定义字符
			{
				customCharSpot=0;
				WriteCGRAM(customCharNumber,customCharData);
				CurrentMode=MODE_NORMAL;
			}
			
		}
	}
}

/*写入用户自定义字符*/
void WriteCGRAM(uint8_t num,uint8_t* buff)
{
	uint8_t count=0;
	symbol=num&0x07;
	symbol<<=3;
	LCD1602_Write_Cmd(symbol | 0x40);
	for(count=0;count<8;count++)
	{
		LCD1602_Write_Dat(buff[count]);		//写入数据
	}
}
	

/*改变1602屏幕的对比度*/
void ChangeContrast(uint8_t data)
{
	TIM_SetCompare4(TIM3,data*2);			//改变占空比  调节对比度
}

/*1602显示一个字符*/
void LCD1602DisplayChar(uint8_t incoming)
{
	LCD1602_Set_Cursor(cursorx, cursory);			//设置光标
	LCD1602_Write_Dat(incoming);					//写入数据
	cursorx++;										//处理下次光标的位置
	if(cursorx>=LCD_WIGHT)
	{
		cursorx=0;
		cursory++;
		if(cursory>=LCD_HIGHT)
		{
			cursory=0;
		}
	}
}


/*********IIC初始化配置*********/
void Config_I2C(uint8_t I2Cx_OWN_ADDRESS7)
{
	I2C_InitTypeDef I2C_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_Cmd(I2C1, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Pin = PIN_I2C1_SDA | PIN_I2C1_SCL;
	GPIO_Init(I2C1_PORT, &GPIO_InitStructure);

	GPIO_PinAFConfig(I2C1_PORT, GPIO_PinSource9, GPIO_AF_4);
	GPIO_PinAFConfig(I2C1_PORT, GPIO_PinSource10, GPIO_AF_4);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7<<1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_Timing = 0x20B22536;								//100KHz
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;  /*!< Enable Nostretch */

	NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	I2C_ITConfig(I2C2, I2C_IT_ERRI|I2C_IT_TCI|I2C_IT_STOPI|\
	I2C_IT_NACKI|I2C_IT_ADDRI|I2C_IT_RXI|I2C_IT_TXI, ENABLE);
	I2C1->CR1|=0x7F<<1;		//开启中断
	I2C_Cmd(I2C1, ENABLE);
}

	
void I2C1_IRQHandler(void)
{
//	uint8_t ljp=I2C1->RXDR;
	/*!< Read Event Flag */
	temp = I2C1->ISR;	//读取中断状态寄存器的值
	i2c_dev.I2C_EventStatus = (uint32_t)(temp & I2C_STATUS_EVT_MASK);
	i2c_dev.I2C_ErrorStatus = (uint32_t)(temp & I2C_STATUS_ERR_MASK);
	
	if(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == SET)	//ADDR=1  //地址匹配中断
	{
		//revSta = 0;
		if ((I2C1->ISR & I2C_ISR_DIR) != 0)  /*!< Slave enter transmitter mode, Host read data */
		{
			I2C1->ISR |= I2C_ISR_TXE;   /*!< Transmit data register empty */
			i2c_dev.I2C_DeviceFlag |= I2C_DEVICE_FLAG_DIR_SEND;
			Offset = 32;	//避免主机一个读取未结束忽然断电，再次读取时数据错位
		}
		else
		{  /*!< Host write data (slave receive data from host) */
			i2c_dev.I2C_DeviceFlag &= ~I2C_DEVICE_FLAG_DIR_SEND;
		}
		I2C_ClearFlag(I2C1, I2C_ICR_ADDRCF);
	}
	if ((i2c_dev.I2C_EventStatus & I2C_ISR_TXIS) != 0) // slave send interrupt.
	{
//		I2C_TX_Handler(RegisterAddress, (uint32_t *)&(I2C1->TXDR));	//进入发送数据处理函数
	}
	
	if ((i2c_dev.I2C_EventStatus & I2C_ISR_RXNE) != 0)  //slave receive interrupt.
    {
		//收到数据, 传入处理函数 		
		i2c_rx_buf[iicrxhead] = I2C1->RXDR;	//每次收到8位数据
		iicrxhead=(iicrxhead+1)%IIC_RX_NUM_MAX; //字节数据索引加1
	}
	if ((i2c_dev.I2C_EventStatus & I2C_ISR_STOPF) != 0)  //stop signal interrupt
	{	
		I2CFirstByteFlag = 0;  //clear I2CFirstByteFlag in order to receive next register address
        I2CFirstWordFlag = 0;
        ReadSensorFlag = 0; //读取真实寄存器的标志清零
        temp1 = 0;	
		I2C_ClearITPendingBit(I2C1, I2C_IT_STOPF);  /*!< set STOPCF */
	}
	
		/* ==================================================== */
	/*                      非应答中断                       */
	/* ==================================================== */
	if ((i2c_dev.I2C_EventStatus & I2C_ISR_NACKF) != 0)
	{
		I2C_ClearITPendingBit(I2C1, I2C_IT_NACKF);
		if (I2CFirstByteFlag != 0)
		{
			I2CFirstByteFlag = 0;
		}
		i2c_dev.I2C_DeviceFlag |= I2C_DEVICE_FLAG_NACK;
	}
	
	/* ==================================================== */
	/*                       错误中断                        */
	/* ==================================================== */
	//
	//总线错误
	//
	if ((i2c_dev.I2C_ErrorStatus & I2C_ISR_BERR) != 0)
	{
		I2C1->ICR = I2C_ICR_BERRCF;
		i2c_dev.I2C_DeviceFlag |= I2C_DEVICE_FLAG_ERROR;
	}
	//
	//仲裁失败
	//
	if ((i2c_dev.I2C_ErrorStatus & I2C_ISR_ARLO) != 0)
	{
		I2C1->ICR = I2C_ICR_ARLOCF;
		i2c_dev.I2C_DeviceFlag |= I2C_DEVICE_FLAG_ERROR;
	}
	//
	//溢出
	//
	if ((i2c_dev.I2C_ErrorStatus & I2C_ISR_OVR) != 0)
	{
		I2C1->ICR = I2C_ICR_OVRCF;
		i2c_dev.I2C_DeviceFlag |= I2C_DEVICE_FLAG_ERROR;
	}
}



