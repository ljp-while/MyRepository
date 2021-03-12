#ifndef __SYS_H
#define __SYS_H	
#include "stm32f0xx.h"
#include "includes.h" 

#define _DEBUG_ 0

#if _DEBUG_
#define _DEBUG_PRINT_(...) printf(__VA_ARGS__)
#else
#define _DEBUG_PRINT_(...)
#endif

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

extern uint32_t Lock_Code;

void system_init(void);

#endif
