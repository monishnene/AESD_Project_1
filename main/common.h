/***************************************
* common.h
* Author: Monish Nene and Sanika Dongre
* Date created: 03/25/19
***************************************/

#ifndef COMMON_H
#define COMMON_H
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "logger.h"
#define STR_SIZE 200
#define LOGPORT 8002
#define PORT_ADDRESS 8003

typedef enum
{
	LOG_INFO=0,
	LOG_DATA=1,
	LOG_ERROR=2,
}logtype_t;

typedef enum               //enum to turn on/off leds according to the tasks
{
	logger_led=0,
	light_led=1,
	temperature_led=2,
	error_led=3,
}led_t;
 
typedef enum              //enum with id values  assigned according to tasks
{
	kelvin_id=0,
	celcius_id=1,
	fahrenheit_id=2,
	luminosity_id=2,
	light_id=3,
	temperature_id=8,
	remote_id=5,
	default_id=6,
	project_id=7,
}header_t;

typedef struct
{
	header_t header;
	struct timespec timestamp;
	int32_t data[3];  
}log_t;

#define LOG_SIZE sizeof(log_t)

uint8_t* logfile;
static uint8_t* logtype[]={"LOG_INFO","LOG_DATA","LOG_ERROR"};
static uint8_t condition=1;
static uint8_t logger_ready_id[]="check if logger is ready";
static uint8_t trigger_sem_id[]="sem_trigger";
static uint8_t i2c_sem_id[]="sem_i2c";
static uint8_t logfile_sem_id[]="sem_logfile";
static uint8_t shm_temp_id[]="temperature";
static uint8_t shm_light_id[]="light";
static uint8_t led_sem_id[]="sem_leds";
#endif


