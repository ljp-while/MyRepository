#ifndef __I2C_BUS_H
#define __I2C_BUS_H

#include "includes.h"
#include "sensor_mode.h"

#define timeSlot  		 3
#define timeSlot_10    2
#define timeSlot_50    10
#define timeSlot_100   20

#define I2C_PORT       GPIOA
#define PIN_I2C_SDA    GPIO_Pin_2
#define PIN_I2C_SCL    GPIO_Pin_3

//GPIOA1
#define SCCB_SIC_H()      GPIOA->BSRR |= GPIO_Pin_3 	//SCL H
#define SCCB_SIC_L()      GPIOA->BRR  |= GPIO_Pin_3   //SCL L

//GPIOA0
#define SCCB_SID_H()      GPIOA->BSRR |= GPIO_Pin_2   //SDA	H
#define SCCB_SID_L()      GPIOA->BRR  |= GPIO_Pin_2   //SDA	L

#define SCCB_DATA_IN     {GPIOA->MODER &= 0XFFFFFFCF;GPIOA->MODER |= 0X00000000;}
#define SCCB_DATA_OUT    {GPIOA->MODER &= 0XFFFFFFCF;GPIOA->MODER |= 0X00000010;}
#define SCCB_SID_STATE	  GPIO_ReadInputDataBit(I2C_PORT,PIN_I2C_SDA)



void i2c_bus_init(void);
void Soft_I2C_Init(void);
void i2c_bus_release(void);
void i2c_bus_probe(void);
void i2c_bus_start(void);
void i2c_bus_stop(void);
void i2c_bus_send_noack(void);
void i2c_bus_send_ack(void);
uint8_t i2c_bus_write_byte(uint8_t data);
uint8_t i2c_bus_read_byte(void);

uint8_t wrSensorManyReg8_8(uint8_t, uint8_t, uint8_t,uint8_t);
uint8_t wrSensor8_8(uint8_t sensor_addr, uint8_t regDat);
uint8_t wrSensor8_16(uint8_t sensor_addr, uint16_t regDat);
uint8_t wrSensorReg8_8(uint8_t sensor_addr, uint8_t regID, uint8_t regDat);
uint8_t wrSensorReg8_16(uint8_t sensor_addr, uint8_t regID, uint16_t regDat);
uint8_t wrSensorReg16_8(uint8_t sensor_addr, uint16_t regID, uint8_t regDat);
uint8_t wrSensorReg16_16(uint8_t sensor_addr, uint16_t regID, uint16_t regDat);
uint8_t rdSensorReg8_8(uint8_t sensor_addr, uint8_t regID, uint8_t* regDat);
uint8_t rdSensorReg8_16(uint8_t sensor_addr, uint8_t regID, uint16_t* regDat);
uint8_t rdSensorReg16_8(uint8_t sensor_addr, uint16_t regID, uint8_t* regDat);
uint8_t rdSensorReg16_16(uint8_t sensor_addr, uint16_t regID, uint16_t* regDat);
uint8_t i2c_detect(uint8_t *address);
uint8_t i2c_detect_given_address(uint8_t address);
uint8_t sccb_write_reg(I2C_RW_STATE *state);
uint8_t sccb_read_reg(I2C_RW_STATE *state);
uint8_t  sccb_write_regs(I2C_RW_STATE *state, const struct sensor_regs *regs, uint32_t regs_size);
int sccb_write_addr_data(uint8_t slaveAddr,uint8_t *data,uint32_t length);
int sccb_read_addr_data(uint8_t slaveAddr,uint8_t *data,uint32_t length);
#endif
