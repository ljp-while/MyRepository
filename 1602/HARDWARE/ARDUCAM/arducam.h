#ifndef __ARDUCAM_H
#define __ARDUCAM_H
#include "includes.h"
#include "sensor_mode.h"
#include "i2c_slave.h"

uint8_t arducamInit(struct sensor_info *sensor);
uint8_t arducamStart(struct sensor_info *sensor);
uint8_t arducamStop(struct sensor_info *sensor);
void parsingComd(CMD_TypeDef command);
uint16_t readSensorID(uint16_t register_addr);// ∂¡»°sensor id
int detect_focus_motor(void);

#endif
