#include "sys.h"
#include "includes.h"


uint32_t Lock_Code;


void GetLockCode(void)
{
    u32 CpuID[3];
    //获取CPU唯一ID
    CpuID[0]=*(vu32*)(0x1ffff7ac);
    CpuID[1]=*(vu32*)(0x1ffff7b0);
    CpuID[2]=*(vu32*)(0x1ffff7b4);
    //加密算法,很简单的加密算法
    Lock_Code=(((CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3)) & 0X00FFFFFF) | 0XAD000000;
    _DEBUG_PRINT_("Lock_Code = 0x%x\r\n", Lock_Code);
}

void system_init(void)
{
//	Soft_I2C_Init();
	delay_init();
	GPIO_Configuration();
	LCD1602_Init();
	Config_I2C(0x0c);
	TIM3_PWM_Init(999, 799);		//PWM调节  周期为1秒
}

