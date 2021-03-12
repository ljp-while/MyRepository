#ifndef __SENSOR_MODE_
#define __SENSOR_MODE_

#include "stm32f0xx.h"
#include "stdio.h"

#define NUM_ELEMENTS(a) (sizeof(a) / sizeof(a[0]))
#define ERROR_DATA	0XFFFFFFFE
#define sccb_write_reg(state)   i2c_write_reg(state)
#define sccb_read_reg(state)   i2c_read_reg(state)


/*寄存器定义*/
#define DEVICE_REG_BASE      0x0100
#define PIXFORMAT_REG_BASE   0x0200
#define FORMAT_REG_BASE      0x0300
#define CTRL_REG_BASE        0x0400
#define SENSOR_REG_BASE      0x0500

#define STREAM_ON					     (DEVICE_REG_BASE |0x0000)
#define DEVICE_VERSION_REG     (DEVICE_REG_BASE |0x0001)
#define SENSOR_ID_REG          (DEVICE_REG_BASE |0x0002)
#define DEVICE_ID_REG          (DEVICE_REG_BASE |0x0003) 
#define FOCUS_MOTOR_REG        (DEVICE_REG_BASE |0x0004)
#define FIRMWARE_SENSOR_ID_REG  (DEVICE_REG_BASE |0x0005)
#define UNIQUE_ID_REG           (DEVICE_REG_BASE |0x0006)

/* 像素格式 */
#define PIXFORMAT_INDEX_REG			(PIXFORMAT_REG_BASE |0x0000)
#define PIXFORMAT_TYPE_REG			(PIXFORMAT_REG_BASE |0x0001)
#define BAYER_ORDER_REG			    (PIXFORMAT_REG_BASE |0x0002)
#define MIPI_LANES_REG				  (PIXFORMAT_REG_BASE |0x0003)

/* 分辨率 */                                  
#define RESOLUTION_INDEX_REG		(FORMAT_REG_BASE |0x0000)
#define FORMAT_WIDTH_REG			  (FORMAT_REG_BASE |0x0001)
#define FORMAT_HEIGHT_REG			  (FORMAT_REG_BASE |0x0002)

/* 控制 */                                     
#define CTRL_INDEX_REG			    (CTRL_REG_BASE |0x0000)
#define CTRL_ID_REG							(CTRL_REG_BASE |0x0001)
#define CTRL_MIN_REG						(CTRL_REG_BASE |0x0002)
#define CTRL_MAX_REG						(CTRL_REG_BASE |0x0003)
#define CTRL_STEP_REG						(CTRL_REG_BASE |0x0004)
#define CTRL_DEF_REG						(CTRL_REG_BASE |0x0005)
#define CTRL_VALUE_REG					(CTRL_REG_BASE |0x0006)

//#define SENSOR_DUMMY_REG				    (SENSOR_REG_BASE |0x0000)
#define SENSOR_RD_REG				    		(SENSOR_REG_BASE |0x0001)
#define SENSOR_WR_REG				    		(SENSOR_REG_BASE |0x0002)


/* 升级 */
#define UPDATE_FLAG_REG                 0X5454
#define UPDATE_REG  				    0X5555
#define CHECK_SUM_REG                   0X5656                      


#define Commond_Register       0x00
#define DATA_Register          0x40


typedef uint32_t format_width_t;
typedef uint32_t format_height_t;

typedef enum{
	RAW8 = 0x2A,
	RAW10 = 0x2B,
	YUV420_8BIT = 0x18,
	YUV420_10BIT = 0x19,
    YUV422_8BIT = 0X1E,
}pixformat_type_t;

struct pixformat_type{
    pixformat_type_t image_id;  
    int resolution_index;
    struct sensor_mode *resolutions;
    int num_resolutions;
};

enum{
	BAYER_ORDER_BGGR = 0,
	BAYER_ORDER_GBRG,
	BAYER_ORDER_GRBG,
	BAYER_ORDER_RGGB,
	BAYER_ORDER_GRAY,
};

enum yuv_order {
  YUV_ORDER_YUYV = 0,
  YUV_ORDER_YVYU = 1,
  YUV_ORDER_UYVY = 2,
  YUV_ORDER_VYUY = 3,
};

typedef enum{
	LANES_1 = 1,
	LANES_2,
}mipi_lanes_t;

struct sensor_regs {
    uint16_t reg;
    uint16_t data;
};
struct sensor_mode {
    const struct sensor_regs *regs;
    int num_regs;
    int width;
    int height;
    int order;
    mipi_lanes_t data_lanes; 
};

enum i2cMode {
    I2C_MODE_8_8 = 0x11,
    I2C_MODE_8_16 = 0x12,
    I2C_MODE_16_8 = 0x21,
    I2C_MODE_16_16 = 0x22,
};
typedef struct {
    enum i2cMode i2c_mode;
    uint16_t chip_address;
    uint16_t register_address;
    uint16_t register_value;
} I2C_RW_STATE;

//这个数据结构提供了这个sensor有多少种mode
//启动寄存器
//停止寄存器
//i2c信息（芯片地址/寄存器地址/寄存器和数据的位数）
//sensor ID
//所支持的所有的控制以及控制数量
struct sensor_info {
    int pixformat_index;
    struct pixformat_type *pixformat_types;
    int num_pixformat_types;
	struct sensor_regs *start;
    int num_start_regs;
	struct sensor_regs *stop;
    int num_stop_regs;
    I2C_RW_STATE sensor_i2c_state;
    int sensor_id;
	int sensor_ctrl_index;
    struct sensor_ctrl *ctrls;
    int num_ctrls;
    int subdev_info_index;
    struct subdev_info *subdev_infos;
    int num_subdev_infos;
    uint32_t update_flag;
    uint8_t focus_motor;
};
struct subdev_info{
    struct sensor_regs *regs;
    int num_regs;
    uint16_t i2c_addr;
    enum i2cMode i2c_mode;
};

typedef int(sensor_set_ctrl_cb)(
    I2C_RW_STATE *state, struct sensor_ctrl *sensor_ctrl, int value);
typedef int(sensor_get_ctrl_cb)(
    I2C_RW_STATE *state, struct sensor_ctrl *sensor_ctrl, int *value);	

typedef uint8_t(i2c_detect_focus_motor_t)(void);

typedef struct sensor_ctrl {
    uint32_t id; 								  /* 控制ID */
    uint32_t min;								  /* 控制最小值 */
    uint32_t max;                 /* 控制最大值 */
    uint32_t def;         			  /* 控制默认值 */
    uint32_t step; 								/* 控制步进值 */
		uint32_t ctrl_value;						/* 控制值 */	
	  sensor_set_ctrl_cb *setter;
    sensor_get_ctrl_cb *getter;
}sensor_ctrl_t;	/* 传感器控制定义 */


#ifdef  ARDUCAM_SN9C2285
	#define SENSOR  sn9c2285
    #define SENSOR_ID_ADDR  0x0003
	extern struct sensor_info sn9c2285;
    
  
#endif

#endif
