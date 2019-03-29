#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include "bbgled.h"

typedef enum
{
	logger_led=0,
	light_led=1,
	temperature_led=2,
	error_led=3,
}led_t;

typedef enum
{
	kelvin_id=0,
	celcius_id=1,
	fahrenheit_id=2,
	luminosity_id=2,
	light_id=3,
	temperature_id=4,
	remote_id=5,
	default_id=6,
	project_id=7,
}header_t;

typedef struct
{
	header_t header;
	uint32_t log_id;
	struct timespec timestamp;
	int32_t data[3];  
}log_t;

#define LOG_SIZE sizeof(log_t)


static uint8_t i2c_sem_id[]="sem_i2c";
static uint8_t logfile_sem_id[]="sem_log";
static uint8_t logfile[]="log.txt";
static uint8_t shm_temp_id[]="temperature_shm";
static uint8_t shm_light_id[]="light_shm";
char * const temp_exec[]={"./temperature_dummy.elf",NULL};
char * const light_exec[]={"./light_dummy.elf",NULL};
char * const logger_exec[]={"./logger.elf",NULL};
#endif


