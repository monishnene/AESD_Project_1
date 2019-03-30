#ifndef LIGHT_READ_H
#define LIGHT_READ_H
#include "common.h"
#include "bbgled.h"
#include <math.h>
#define ID_VALUE (0x50)
#define ID_REGISTER (0x8A)
#define LUX_SLAVE_ADDR (0x39)
#define POWER_ADDR (0x80)
#define TIMING_REG (0X81)
#define TIMING_VAL (0X12)
#define POWER_ON_CMD (0x3)
#define POWER_OFF_CMD (0x00)
#define CH0_L (0x8C)
#define CH0_H (0x8D)
#define CH1_L (0x8E)
#define CH1_H (0x8F)

void* light_read(void* ptr);
void light_init(void);

#endif
