#include "i2c_bus.h"
#include "includes.h"

void i2c_bus_init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* 使能GPIOA时钟 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* 配置i2c管脚PA0和PA1 PA0: SDA  PA1: SCL*/  
  GPIO_InitStructure.GPIO_Pin = PIN_I2C_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(I2C_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_I2C_SCL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(I2C_PORT, &GPIO_InitStructure);
	//GPIO_SetBits(I2C_PORT,PIN_I2C_SCL);
}


void i2c_bus_release(void){
//	GPIOA->MODER &= 0XFFFFFFF0;
}
void i2c_bus_probe(void){
//	GPIOA->MODER &= 0XFFFFFFF0;
//	GPIOA->MODER |= 0X00000005;
}

void i2c_bus_start(void){
	SCCB_SID_H();             
    //delay_us(timeSlot_50);
	delay_us(4);
    SCCB_SIC_H();	       
    delay_us(timeSlot_10);
    SCCB_SID_L();
    delay_us(timeSlot_10);
    SCCB_SIC_L();	     
    delay_us(timeSlot_10);
	
}
void i2c_bus_stop(void){
	SCCB_SID_L();
	delay_us(timeSlot);
	SCCB_SIC_H();	
	delay_us(timeSlot);  
	SCCB_SID_H();	
	delay_us(timeSlot);
}
void i2c_bus_send_noack(void){
  SCCB_SID_H();	
	delay_us(timeSlot);	
	SCCB_SIC_H();	
	delay_us(timeSlot);	
	SCCB_SIC_L();	
	delay_us(timeSlot);	
	SCCB_SID_L();	
	delay_us(timeSlot);
	

}
void i2c_bus_send_ack(void){
	SCCB_SID_L();	
	delay_us(timeSlot);	
	SCCB_SIC_L();	
	delay_us(timeSlot);	
	SCCB_SIC_H();	
	delay_us(timeSlot);	
	SCCB_SIC_L();	
	delay_us(timeSlot);	
	SCCB_SID_L();	
	delay_us(timeSlot);
}
uint8_t i2c_bus_write_byte(uint8_t data){
	uint8_t i;
	uint8_t tem;

	for(i = 0; i < 8; i++) 
	{
		if((data<<i) & 0x80)
		{
			SCCB_SID_H();
		}
		else 
		{
			SCCB_SID_L();
		}
		delay_us(timeSlot);
		SCCB_SIC_H();	
		delay_us(timeSlot);
		SCCB_SIC_L();	
	//	delay_us(timeSlot);
	}
	delay_us(timeSlot);
	SCCB_DATA_IN;
	delay_us(timeSlot);
	SCCB_SIC_H();	
	delay_us(20);//500us
	if(SCCB_SID_STATE)
	{
		tem = 0;             
	}
	else 
	{
		tem = 1;                
	}

	SCCB_SIC_L();	
	delay_us(timeSlot);	
	SCCB_DATA_OUT;
	return tem;  
}
uint8_t i2c_bus_read_byte(void){
	uint8_t i;
	uint8_t read = 0;
	
	SCCB_DATA_IN;
	delay_us(20);//50us
	for(i = 8; i > 0; i--)
	{		     
		delay_us(timeSlot);
		SCCB_SIC_H();
		//delay_us(timeSlot);
		read = read << 1;
		if(SCCB_SID_STATE)
		{
			read += 1; 
		}
        delay_us(timeSlot);
		SCCB_SIC_L();
	}	
    SCCB_DATA_OUT;
	return read;
}

void Soft_I2C_Init(void){
	i2c_bus_init();
	i2c_bus_release();
}

uint8_t wrSensor8_8(uint8_t sensor_addr, uint8_t regDat)
{
	i2c_bus_probe();
	delay_us(timeSlot);
	i2c_bus_start();                          
	if(i2c_bus_write_byte(sensor_addr) == 0)         
	{
		i2c_bus_stop();  
		i2c_bus_release();		
		return 1;
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regDat)==0)                    
	{
		i2c_bus_stop(); 
		i2c_bus_release();		
		return 2;
	}
	i2c_bus_stop();
	i2c_bus_release();	
	return 0;
}

uint8_t wrSensor8_16(uint8_t sensor_addr, uint16_t regDat)
{
//	i2c_bus_probe();
//	delay_us(timeSlot);
//	i2c_bus_start();                          
//	if(i2c_bus_write_byte(sensor_addr) == 0)         
//	{
//		i2c_bus_stop();  
//		i2c_bus_release();		
//		return 1;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regDat >> 8)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 2;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regDat)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 3;
//	}
//	i2c_bus_stop();
//	i2c_bus_release();	
	return 0;
}
uint8_t wrSensorReg8_8(uint8_t sensor_addr, uint8_t regID, uint8_t regDat)
{
	i2c_bus_probe();
	delay_us(timeSlot);
	i2c_bus_start();                          
	if(i2c_bus_write_byte(sensor_addr) == 0)         
	{
		i2c_bus_stop();  
		i2c_bus_release();		
		return 1;
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regID) == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();		
		return 2;                                       
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regDat)==0)                    
	{
		i2c_bus_stop(); 
		i2c_bus_release();		
		return 3;
	}
	i2c_bus_stop();
	i2c_bus_release();	
	return 0;
}

/*
*	连续发送
*/
uint8_t wrSensorManyReg8_8(uint8_t sensor_addr, uint8_t regID, uint8_t regDat,uint8_t lenght)
{
	uint8_t count=0;
	i2c_bus_probe();
	delay_us(timeSlot);
	i2c_bus_start(); 
	if(i2c_bus_write_byte(sensor_addr) == 0)         
	{
		i2c_bus_stop();  
		i2c_bus_release();		
		return 1;
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regID) == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();		
		return 2;                                       
	}
	delay_us(timeSlot);
	for(count=0;count<lenght;count++)
	{
		if(i2c_bus_write_byte(regDat)==0)                    
		{
			i2c_bus_stop(); 
			i2c_bus_release();		
			return 3;
		}		
	}
	i2c_bus_stop();
	i2c_bus_release();	
	return 0;
}

uint8_t wrSensorReg8_16(uint8_t sensor_addr, uint8_t regID, uint16_t regDat)
{
//	i2c_bus_probe();
//	delay_us(timeSlot);
//	i2c_bus_start();                          
//	if(i2c_bus_write_byte(sensor_addr) == 0)         
//	{
//		i2c_bus_stop();  
//		i2c_bus_release();		
//		return 1;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regID) == 0)
//	{
//		i2c_bus_stop();
//		i2c_bus_release();		
//		return 2;                                       
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regDat >> 8)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 3;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regDat)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 4;
//	}
//	i2c_bus_stop();
//	i2c_bus_release();	
	return 0;
}

uint8_t wrSensorReg16_8(uint8_t sensor_addr, uint16_t regID, uint8_t regDat)
{
	i2c_bus_probe();
	delay_us(timeSlot);
	i2c_bus_start();                          
	if(i2c_bus_write_byte(sensor_addr) == 0)         
	{
		i2c_bus_stop();  
		i2c_bus_release();		
		return 1;
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regID >> 8) == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();		
		return 2;                                       
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regID)==0)                    
	{
		i2c_bus_stop(); 
		i2c_bus_release();		
		return 3;
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regDat)==0)                    
	{
		i2c_bus_stop(); 
		i2c_bus_release();		
		return 4;
	}
	i2c_bus_stop();
	i2c_bus_release();	
	return 0;
}

uint8_t wrSensorReg16_16(uint8_t sensor_addr, uint16_t regID, uint16_t regDat)
{
//	i2c_bus_probe();
//	delay_us(timeSlot);
//	i2c_bus_start();                          
//	if(i2c_bus_write_byte(sensor_addr) == 0)         
//	{
//		i2c_bus_stop();  
//		i2c_bus_release();		
//		return 1;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regID >> 8) == 0)
//	{
//		i2c_bus_stop();
//		i2c_bus_release();		
//		return 2;                                       
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regID)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 3;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regDat >> 8)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 4;
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regDat)==0)                    
//	{
//		i2c_bus_stop(); 
//		i2c_bus_release();		
//		return 5;
//	}
//	i2c_bus_stop();
//	i2c_bus_release();	
	return 0;
}

uint8_t rdSensorReg8_8(uint8_t sensor_addr, uint8_t regID, uint8_t* regDat)
{
	i2c_bus_probe();
	delay_us(timeSlot);
	i2c_bus_start();
	if(i2c_bus_write_byte(sensor_addr) == 0)                 
	{
		i2c_bus_stop();  
		i2c_bus_release();		
		return 1;                                        
	}
	delay_us(timeSlot);
	if(i2c_bus_write_byte(regID)==0)//ID
	{
		i2c_bus_stop();  
    i2c_bus_release();		
		return 2;                                       
	}
	i2c_bus_stop();                                   
	delay_us(timeSlot);	
	i2c_bus_start();
	if(i2c_bus_write_byte(sensor_addr|0x01)==0)                    
	{
		i2c_bus_stop();
		i2c_bus_release();		
		return 3;                                          
	}
	delay_us(timeSlot);
	*regDat = i2c_bus_read_byte();                    
	i2c_bus_send_noack();                                
	i2c_bus_stop();   
	i2c_bus_release();	
	return 0;                
}

uint8_t rdSensorReg8_16(uint8_t sensor_addr, uint8_t regID, uint16_t* regDat)
{	
//	i2c_bus_probe();
//	delay_us(timeSlot);
//	i2c_bus_start();
//	if(i2c_bus_write_byte(sensor_addr) == 0)                 
//	{
//		i2c_bus_stop(); 
//    i2c_bus_release();		
//		return 1;                                        
//	}
//	delay_us(timeSlot);
//	if(i2c_bus_write_byte(regID)==0)//ID
//	{
//		i2c_bus_stop();
//    i2c_bus_release();		
//		return 2;                                       
//	}
//	i2c_bus_stop();                                   
//	delay_us(timeSlot);	
//	i2c_bus_start();
//	if(i2c_bus_write_byte(sensor_addr|0x01)==0)                    
//	{
//		i2c_bus_stop();
//		i2c_bus_release();		
//		return 3;                                          
//	}
//	delay_us(timeSlot);
//	*regDat = i2c_bus_read_byte();  
//	i2c_bus_send_ack();
//	*regDat = (*regDat << 8) | i2c_bus_read_byte();
//	i2c_bus_send_noack();                                
//	i2c_bus_stop();
//	i2c_bus_release();	
	return 0;                
}

uint8_t rdSensorReg16_8(uint8_t sensor_addr, uint16_t regID, uint8_t* regDat)
{
	i2c_bus_probe();
	i2c_bus_start();                  
	if(i2c_bus_write_byte(sensor_addr) == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();
		return(1);
	}
	delay_us(timeSlot);
  if(i2c_bus_write_byte(regID>>8)  == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();
		return(2);
	}
	delay_us(timeSlot);
  if(i2c_bus_write_byte(regID)  == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();
		return(3);
	}
	delay_us(timeSlot);
	i2c_bus_stop();
	delay_us(timeSlot);
	i2c_bus_start();                 
	if(i2c_bus_write_byte(sensor_addr|0x01) == 0)
	{
		i2c_bus_stop();
		i2c_bus_release();
		return(4);
	}
	delay_us(timeSlot);
	*regDat=i2c_bus_read_byte();
	i2c_bus_send_noack();
	i2c_bus_stop();
	i2c_bus_release();
  return(0);
}

uint8_t rdSensorReg16_16(uint8_t sensor_addr, uint16_t regID, uint16_t* regDat)
{
//	i2c_bus_probe();
//	i2c_bus_start();                  
//	if(i2c_bus_write_byte(sensor_addr) == 0)
//	{
//		i2c_bus_stop();
//		i2c_bus_release();
//		return(1);
//	}
//	delay_us(timeSlot);
//  if(i2c_bus_write_byte(regID>>8)  == 0)
//	{
//		i2c_bus_stop();
//		i2c_bus_release();
//		return(2);
//	}
//	delay_us(timeSlot);
//  if(i2c_bus_write_byte(regID)  == 0)
//	{
//		i2c_bus_stop();
//		i2c_bus_release();
//		return(3);
//	}
//	delay_us(timeSlot);
//	i2c_bus_stop();
//	delay_us(timeSlot);
//	i2c_bus_start();                 
//	if(i2c_bus_write_byte(sensor_addr|0x01) == 0)
//	{
//		i2c_bus_stop();
//		i2c_bus_release();
//		return(4);
//	}
//	delay_us(timeSlot);
//	*regDat = i2c_bus_read_byte();  
//	i2c_bus_send_ack();
//	*regDat = (*regDat << 8) | i2c_bus_read_byte();
//	i2c_bus_send_noack();                                
//	i2c_bus_stop(); 
//	i2c_bus_release();	
	return 0;
}

// 检测总线上所有的I2C设备地址
uint8_t i2c_detect(uint8_t *address){
    uint8_t i =0, sensor = 0;
    for(sensor = 0; sensor <= 255; sensor++){
        i2c_bus_probe();
        i2c_bus_start();                  
        if(i2c_bus_write_byte(sensor) == 1){    //有ack
            address[i++] = sensor;
        }
		i2c_bus_stop();
		i2c_bus_release();  
    }
    return i;
}

// 检测指定地址是否存在, address:8位从机地址
uint8_t i2c_detect_given_address(uint8_t address){
    i2c_bus_probe();
    i2c_bus_start();                  
    if(i2c_bus_write_byte(address) == 1){    //有ack
        i2c_bus_stop();
        i2c_bus_release();  
        return 0;
    }
    i2c_bus_stop();
    i2c_bus_release(); 
    return 1;
}

uint8_t sccb_write_reg(I2C_RW_STATE *state)
{
  switch(state->i2c_mode){
    case I2C_MODE_8_8:
       if(wrSensorReg8_8(state->chip_address, state->register_address, state->register_value)){
          return 1; 
        }
        break;
    case I2C_MODE_8_16:
        if(wrSensorReg8_16(state->chip_address, state->register_address, state->register_value)){
					return 1; 
        }
        break;
    case I2C_MODE_16_8:
     if(wrSensorReg16_8(state->chip_address, state->register_address, state->register_value)){
            return 1; 
        }
        break;
    case I2C_MODE_16_16:
     if(wrSensorReg16_16(state->chip_address, state->register_address, state->register_value)){
          return 1; 
        }
        break;
    default:
        return 1;
    }
    return 0;
}


uint8_t  sccb_read_reg(I2C_RW_STATE *state)
{
    uint8_t data[2];
    uint16_t val = 0;
    switch(state->i2c_mode){
    case I2C_MODE_8_8:
       if(rdSensorReg8_8(state->chip_address, state->register_address, &data[0])){
            return 1; 
        }
        break;
    case I2C_MODE_8_16:
        if(rdSensorReg8_16(state->chip_address, state->register_address, &val)){
            return 1; 
        }
        data[0] = val>>8;
        data[1] = val;
        break;
    case I2C_MODE_16_8:
     if(rdSensorReg16_8(state->chip_address, state->register_address, &data[0])){
            return 1; 
        }
        break;
    case I2C_MODE_16_16:
        if(rdSensorReg16_16(state->chip_address, state->register_address, &val)){
            return 1; 
        }
        data[0] = val>>8;
        data[1] = val;
        break;
    default:
        return 1;
    }
    if((state->i2c_mode == I2C_MODE_8_16 || state->i2c_mode == I2C_MODE_16_16)){
        state->register_value = 0;
        state->register_value |= data[0];
        state->register_value <<= 8;
        state->register_value |= data[1];
    }else{
        state->register_value = data[0];
    }

	return 0;
}

uint8_t  sccb_write_regs(I2C_RW_STATE *state, const struct sensor_regs *regs, uint32_t regs_size) {

    for (int i = 0; i < regs_size; i++) {
        if (regs[i].reg == 0xFFFE) {
            delay_us(regs[i].data * 1000);
            continue;
        }
        state->register_address = regs[i].reg;
        state->register_value = regs[i].data;
        if (sccb_write_reg(state))
            return 1;
    }
    return 0;
}

int sccb_write_addr_data(uint8_t slaveAddr,uint8_t *data,uint32_t length){
	i2c_bus_probe();
	delay_us(timeSlot);
	i2c_bus_start();                          
	if(i2c_bus_write_byte(slaveAddr) == 0)         
	{
		i2c_bus_stop();  
		i2c_bus_release();		
		return 1;
	}
	delay_us(timeSlot);
    for(int i = 0; i < length; i++){
        if(i2c_bus_write_byte(data[i])==0)                    
        {
            i2c_bus_stop(); 
            i2c_bus_release();		
            return 2;
        }
        delay_us(timeSlot);
    }
	i2c_bus_stop();
	i2c_bus_release();	
	return 0;

}

int sccb_read_addr_data(uint8_t slaveAddr,uint8_t *data,uint32_t length){
	i2c_bus_probe();
	delay_us(timeSlot);	
	i2c_bus_start();
	if(i2c_bus_write_byte(slaveAddr|0x01)==0)                    
	{
		i2c_bus_stop();
		i2c_bus_release();		
		return 1;                                          
	}
	delay_us(timeSlot);
    for(int i = 0; i < length; i++){
        data[i] = i2c_bus_read_byte();  
        i2c_bus_send_ack();
    }
	i2c_bus_send_noack();                                 
	i2c_bus_stop();
	i2c_bus_release();	
	return 0;    
}



