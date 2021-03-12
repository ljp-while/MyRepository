#include "i2c_slave.h"
#include "includes.h"
#include "sensor_mode.h"

struct sensor_info SENSOR;
uint8_t i2c_rx_buf[4];
uint32_t i2c_tx_buf[4];


unsigned char revSta = 0;  		//接收状态寄存器
unsigned char revcmd = 0;  		//接收的命令
unsigned char DataIndex = 0;	//收到的字节数据数索引
static int Offset = 32;			//发送模式时的移位
int32_t temp = 0;
uint32_t temp1 = 0;
I2C_DEVICE_TypeDef i2c_dev;
CMD_TypeDef command;

//I2C接收中断用到变量
uint8_t I2CFirstByteFlag = 0;  //接收第一个字节数据标志
uint8_t I2CFirstWordFlag = 0;   //接收第一个字的标志
uint16_t RegisterAddress = 0;  //接收寄存器地址
uint32_t UniqueID = 0;          //接收唯一ID

uint8_t dummy = 0;

uint8_t ReadSensorFlag = 0;

extern i2c_detect_focus_motor_t *i2c_detect_focus_motor;


struct hardware_info {
	  int device_id;
	  int version;
};
struct hardware_info stm32f103c8 = {
    .device_id = 0x0030,
    .version = 0x01,
};


/* 函数声明 */
int I2C_RX_Handler(uint16_t RegisterAddress, uint8_t DataIndex, uint8_t RevDataByte);
int I2C_TX_Handler(uint16_t RegisterAddress, uint32_t *SendDataByte);

	
void Config_I2C(void)
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
		I2C_TX_Handler(RegisterAddress, (uint32_t *)&(I2C1->TXDR));	//进入发送数据处理函数
	}
	
	if ((i2c_dev.I2C_EventStatus & I2C_ISR_RXNE) != 0)  //slave receive interrupt.
    {
	 	if (I2CFirstWordFlag != 0)  // receive data 
		{
			I2C_RX_Handler(RegisterAddress, DataIndex, I2C1->RXDR);	 //收到数据, 传入处理函数 
			DataIndex++; //字节数据索引加1
        }
        else
		{
			I2CFirstByteFlag ++;
            if(I2CFirstByteFlag == 1)
            {
                RegisterAddress = I2C1->RXDR; 
				I2CFirstWordFlag = 1;				
            }
            else if(I2CFirstByteFlag == 2){
                RegisterAddress = (RegisterAddress << 8) | I2C1->RXDR;   //receive register address
            }  
			DataIndex = 0;
		}
	}
	if ((i2c_dev.I2C_EventStatus & I2C_ISR_STOPF) != 0)  //stop signal interrupt
	{	
		I2CFirstByteFlag = 0;  //clear I2CFirstByteFlag in order to receive next register address
        I2CFirstWordFlag = 0;
        ReadSensorFlag = 0; //读取真实寄存器的标志清零
		DataIndex = 0;
        temp1 = 0;
//        if(DataIndex == 4){	//收到4个字节当做1次收取结束，如果是收1个字节当做1次收取结束，此处DataIndex为1
//			revSta = 1;
//			revcmd = RegisterAddress;
//			DataIndex = 0;
//		}
//		else {
//			revSta = 0;
//		}		
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


int I2C_RX_Handler(uint16_t RegisterAddress, uint8_t DataIndex, uint8_t RevDataByte){
	if(DataIndex >= 4){
		dummy = RevDataByte;	//超过4个字节的数据当错误数据不处理
		return -1;
	}
	command.RegisterAddress = RegisterAddress;
	i2c_rx_buf[DataIndex] = RevDataByte;	//每次收到8位数据
	command.beginOperate = 1;
//	if(DataIndex == 1)
//	{
//		switch(RegisterAddress)
//		{
//			case Commond_Register:
//				command.RegisterAddress = RegisterAddress;
//				command.beginOperate = 1;
//				break;
//			case DATA_Register:
//				command.RegisterAddress = RegisterAddress;
//				break;
//			default:
//				break;
//		}
//	}
//	if(DataIndex == 3){
//	    /* 收到的32位数据 */
//		uint32_t RevData = ((((i2c_rx_buf[0] << 24) | (i2c_rx_buf[1] << 16)) | (i2c_rx_buf[2] << 8))) | i2c_rx_buf[3];
//		switch(RegisterAddress){    
//			case PIXFORMAT_INDEX_REG:   //根据主机写下来的索引号设置 pixformat_index 的值 
//                 SENSOR.pixformat_index = RevData;
//                break;
//			case RESOLUTION_INDEX_REG:       //根据主机写下来的索引号设置 resolution_index 的值 
//                 SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index = RevData;
//			break;
//			case CTRL_INDEX_REG: 
//				SENSOR.sensor_ctrl_index = RevData;     //根据主机写下来的索引号设置控制所索引的值
//			break;
//			case CTRL_ID_REG:  //这里要看主机写下来的命令是什么控制切换索引值
//				SENSOR.sensor_ctrl_index = 0;
//				while(SENSOR.sensor_ctrl_index < SENSOR.num_ctrls){
//					if(SENSOR.ctrls[SENSOR.sensor_ctrl_index].id == RevData){
//						 break;
//					}else{
//						SENSOR.sensor_ctrl_index++;
//					}
//				}	
//			break;
//			case CTRL_VALUE_REG: //调用指定索引值的控制函数写控制数据到sensor
//				command.RegisterAddress = RegisterAddress;
//			    command.index = SENSOR.sensor_ctrl_index;
//			    command.value = RevData;
//			    command.beginOperate = 1;
//			break;
//			case STREAM_ON:
//				command.RegisterAddress = RegisterAddress;
//			    command.value = RevData;
//			    command.beginOperate = 1;
//			break;
//			case SENSOR_WR_REG:
//				command.RegisterAddress = RegisterAddress;
//				command.sensor_address = (RevData>>16) & 0XFFFF;
//			    command.sensor_value = RevData & 0XFFFF;
//			    command.beginOperate = 1;
//			break;
//            case UNIQUE_ID_REG:
//                UniqueID = RevData;
////                UpdateUniqueID(UniqueID);
//            break;
//			default:
//				dummy = RevData;
//			break;
//		}
//	}
	return 0;
}

int I2C_TX_Handler(uint16_t RegisterAddress, uint32_t *SendDataByte){
	Offset -= 8;
    if(RegisterAddress & 0x8000){
        if(ReadSensorFlag == 0){
            command.RegisterAddress = SENSOR_RD_REG;
            command.sensor_address = RegisterAddress & 0X7FFF;
            //parsingComd(command);   //读取寄存器数据
            temp1 = SENSOR.sensor_i2c_state.register_value;
            if(temp1 == 0XFFFE){
                temp1 = ERROR_DATA;
            }
            *SendDataByte = (temp1 >> Offset) & 0xff;
            ReadSensorFlag = 1;
        }else{
            *SendDataByte = (temp1 >> Offset) & 0xff;
        }        
    }
    else{
        switch(RegisterAddress){
            case PIXFORMAT_INDEX_REG:
//                if(SENSOR.pixformat_index >= SENSOR.num_pixformat_types){  //如果pixformat_index索引号大于sensor的所有num_pixformat_types数量返回ERROR
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;
//                }else{
//                  *SendDataByte = (SENSOR.pixformat_index>> Offset) & 0xff;  //返回当前索引号
//                }
                break;
            case RESOLUTION_INDEX_REG:
//                if(SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index >= SENSOR.pixformat_types[SENSOR.pixformat_index].num_resolutions){  
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;
//                }else{
//                  *SendDataByte = (SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index>> Offset) & 0xff;  //返回当前索引号
//                }
                break;
            case PIXFORMAT_TYPE_REG:
//                if(SENSOR.pixformat_index >= SENSOR.num_pixformat_types){  //如果pixformat_index索引号大于sensor的所有num_pixformat_types数量返回ERROR
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;
//                }else{
//                    *SendDataByte = (SENSOR.pixformat_types[SENSOR.pixformat_index].image_id >> Offset) & 0xff;;
//                }
                break;
            case BAYER_ORDER_REG:
//                *SendDataByte = (SENSOR.pixformat_types[SENSOR.pixformat_index].\
//                resolutions[SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index].order >> Offset) & 0xff; //返回当前索引号对应的模式下的order
                break;
            case MIPI_LANES_REG:
//                *SendDataByte = (SENSOR.pixformat_types[SENSOR.pixformat_index].\
//                resolutions[SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index].data_lanes >> Offset) & 0xff;//返回当前索引号对应的模式下的lans
                break;
            case FORMAT_WIDTH_REG:
//                if(SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index >= SENSOR.pixformat_types[SENSOR.pixformat_index].num_resolutions){
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;
//                }else{
//                  *SendDataByte = (SENSOR.pixformat_types[SENSOR.pixformat_index].resolutions[SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index].width >> Offset) & 0xff; //返回当前索引号对应的模式下的宽度
//                }
                break;
            case FORMAT_HEIGHT_REG:
//                if(SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index >= SENSOR.pixformat_types[SENSOR.pixformat_index].num_resolutions){
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;
//                }else{
//                  *SendDataByte = (SENSOR.pixformat_types[SENSOR.pixformat_index].resolutions[SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index].height >> Offset) & 0xff; //返回当前索引号对应的模式下的高度
//                }
                break;
            case CTRL_INDEX_REG: 
//              if(SENSOR.sensor_ctrl_index>= SENSOR.num_ctrls){   //如果当前索引号大于sensor的所有控制数量返回ERROR
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;  
//                }else{
//                  *SendDataByte = (SENSOR.sensor_ctrl_index>> Offset) & 0xff;  //返回当前索引号对应的控制的索引号
//                }
                break;
            case CTRL_ID_REG:	
//                 if(SENSOR.sensor_ctrl_index>= SENSOR.num_ctrls){
//                    *SendDataByte = (ERROR_DATA >> Offset) & 0xff;  //如果当前索引号大于sensor的所有控制数量返回ERROR
//                }else{
//                    *SendDataByte = (SENSOR.ctrls[SENSOR.sensor_ctrl_index].id >> Offset) & 0xff;  //返回当前索引号对应的控制ID
//                }
                break;
            case CTRL_MIN_REG:
//                *SendDataByte = (SENSOR.ctrls[SENSOR.sensor_ctrl_index].min >> Offset) & 0xff;//返回当前控制的最小值
                break;
            case CTRL_MAX_REG:
               // *SendDataByte = (SENSOR.ctrls[SENSOR.sensor_ctrl_index].max >> Offset) & 0xff; // 返回当前控制的最大值
                break;
            case CTRL_STEP_REG:
              //  *SendDataByte = (SENSOR.ctrls[SENSOR.sensor_ctrl_index].step >> Offset) & 0xff; // 返回当前控制的步长
                break;
            case CTRL_DEF_REG:	
              //  *SendDataByte = (SENSOR.ctrls[SENSOR.sensor_ctrl_index].def >> Offset) & 0xff;  // 返回当前控制的默认值
                break;
            case CTRL_VALUE_REG:	
             //   *SendDataByte = (SENSOR.ctrls[SENSOR.sensor_ctrl_index].ctrl_value >> Offset) & 0xff; // 返回当前控制的控制值
                break;		
            case DEVICE_ID_REG:	
             //   *SendDataByte = (stm32f103c8.device_id>> Offset) & 0xff;
                break;
            case DEVICE_VERSION_REG:	
              //  *SendDataByte = (stm32f103c8.version >> Offset) & 0xff;
                break;
            case SENSOR_ID_REG:	
//                if(temp1 == 0){
////                    if(i2c_detect_focus_motor != NULL){
////                        i2c_detect_focus_motor();
////                    }
//                    temp1 = readSensorID(SENSOR_ID_ADDR);
//                    if(temp1 == 0xfffe){
//                        temp1 = ERROR_DATA;
//                    }
//                }
//                *SendDataByte = (temp1 >> Offset) & 0xff;
                break;
            case FOCUS_MOTOR_REG:
               // *SendDataByte  = (SENSOR.focus_motor >> Offset) & 0xff;
                break;
            case FIRMWARE_SENSOR_ID_REG:
               // *SendDataByte = (SENSOR.sensor_id >> Offset) & 0xff;
                break;
            case UNIQUE_ID_REG:
                //UniqueID = GetUniqueID();
              //  *SendDataByte = (UniqueID >> Offset) & 0xff;
                break;
            default:
             //   *SendDataByte = (ERROR_DATA >> Offset) & 0xff;
                 break;
        }
    }
    if(Offset < 0){		
        Offset = 32;
        
    }
    return 0;
}


