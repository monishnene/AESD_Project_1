#ifndef TEMPERATURE_READ_H
#define TEMPERATURE_READ_H
#include "common.h"
#include "bbgled.h"
#define TEMP_SLAVE_ADDR	(0x48)
#define TEMP_REG_ADDR	(00)

void* temperature_read(void* ptr);
void temperature_init(void);

#endif
