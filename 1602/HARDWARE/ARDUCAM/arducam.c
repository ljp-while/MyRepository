#include "arducam.h"
#include "i2c_bus.h"


int32_t value= 0;


uint8_t  arducamInit(struct sensor_info *sensor){
    //如果是双目配置，先将I2C_RW_STATE中的信息暂存，然后使用subdev_infos中的信息对CPLD进行I2C写
    if(SENSOR.num_subdev_infos != 0){
        I2C_RW_STATE temp = sensor->sensor_i2c_state;
        sensor->sensor_i2c_state.chip_address = SENSOR.subdev_infos[SENSOR.subdev_info_index].i2c_addr;
        sensor->sensor_i2c_state.i2c_mode = SENSOR.subdev_infos[SENSOR.subdev_info_index].i2c_mode;
        sccb_write_regs(&sensor->sensor_i2c_state, sensor->subdev_infos[sensor->subdev_info_index].regs,sensor->subdev_infos[sensor->subdev_info_index].num_regs); 
        //写结束后恢复原有的I2C_RW_STATE信息
        sensor->sensor_i2c_state = temp;
    }
    return sccb_write_regs(&sensor->sensor_i2c_state, sensor->pixformat_types\
        [sensor->pixformat_index].resolutions[sensor->pixformat_types[sensor->pixformat_index].resolution_index].regs,\
		sensor->pixformat_types[sensor->pixformat_index].resolutions[sensor->pixformat_types[sensor->pixformat_index].resolution_index].num_regs); 

}
uint8_t arducamStart(struct sensor_info *sensor){
return sccb_write_regs(&sensor->sensor_i2c_state, sensor->start,\
												   sensor->num_start_regs); 
}

uint8_t arducamStop(struct sensor_info *sensor){
return sccb_write_regs(&sensor->sensor_i2c_state, sensor->stop,\
												   sensor->num_stop_regs); 
}

void parsingComd(CMD_TypeDef command){
//	delay_us(1);
//    int ret = 0;	
//	switch(command.RegisterAddress){
//		case CTRL_VALUE_REG:
//            if(SENSOR.pixformat_index >= SENSOR.num_pixformat_types){
//                SENSOR.pixformat_index = 0;
//                SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index = 0;
//            }
////            if(SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index >= SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index){
////                SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index = 0;
////            }
//			SENSOR.ctrls[command.index].setter(&SENSOR.sensor_i2c_state, &SENSOR.ctrls[SENSOR.sensor_ctrl_index], command.value);
//			SENSOR.ctrls[command.index].getter(&SENSOR.sensor_i2c_state, &SENSOR.ctrls[SENSOR.sensor_ctrl_index],  &value);
//			SENSOR.ctrls[command.index].ctrl_value = value;
//			break;
//		case STREAM_ON:
//            if(SENSOR.pixformat_index >= SENSOR.num_pixformat_types){
//                SENSOR.pixformat_index = 0;
//                SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index = 0;
//            }
////            if(SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index >= SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index){
////                SENSOR.pixformat_types[SENSOR.pixformat_index].resolution_index = 0;
////            }
//			if(command.value){
//				arducamInit(&SENSOR);
//				arducamStart(&SENSOR);		
//			}else{
//				arducamStop(&SENSOR);	
//			}
//            break;
//		case SENSOR_WR_REG:
//			SENSOR.sensor_i2c_state.register_address = command.sensor_address;
//		    SENSOR.sensor_i2c_state.register_value = command.sensor_value;
//			sccb_write_reg(&SENSOR.sensor_i2c_state);
//			break;
//        case SENSOR_RD_REG:
//            SENSOR.sensor_i2c_state.register_address = command.sensor_address;
//            ret = sccb_read_reg(&SENSOR.sensor_i2c_state);
//            // 如果读取失败，返回0XFFFE
//            if(ret != 0){
//                SENSOR.sensor_i2c_state.register_value = 0XFFFE;
//            }
//            break;
//        default:
//            break;
//	}
}

uint16_t readSensorID(uint16_t sensor_id_addr){
    uint16_t id = 0;
    switch(SENSOR.sensor_i2c_state.i2c_mode){
        case I2C_MODE_8_8:
        case I2C_MODE_16_8:
            SENSOR.sensor_i2c_state.register_address = sensor_id_addr;
            sccb_read_reg(&SENSOR.sensor_i2c_state);
            id = SENSOR.sensor_i2c_state.register_value;
            SENSOR.sensor_i2c_state.register_address = sensor_id_addr + 1;
            sccb_read_reg(&SENSOR.sensor_i2c_state);
            id = (id << 8) | SENSOR.sensor_i2c_state.register_value;
            break;
        case I2C_MODE_8_16:
        case I2C_MODE_16_16:
            SENSOR.sensor_i2c_state.register_address = sensor_id_addr;
            sccb_read_reg(&SENSOR.sensor_i2c_state);
            id = SENSOR.sensor_i2c_state.register_value;
            break;
        default:
            return 0XFFFE;
    }
    
    if(id == SENSOR.sensor_id){
        return id;
    }
    else{
        return 0XFFFE;
    }
}

int detect_focus_motor(void){
    if(i2c_detect_given_address(0x0c << 1) == 0){
        return 0;
    }
    return 1;
}



