
/******************************************
* bist_test.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/30/19
*******************************************/


/*******************************************
* Includes
*******************************************/
#include "common.h"
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <time.h>

/*******************************************
* Macros
*******************************************/
#define ID_VALUE (0x50)
#define ID_REGISTER (0x8A)
#define ID_VAL (0X07)
#define LUX_SLAVE_ADDR (0x39)
#define POWER_ADDR (0x80)
#define TIMING_REG (0X81)
#define TIMING_VAL (0X12)
#define START_COMMAND (0X80)
#define POWER_ON_CMD (0x3)
#define CONTROL_VAL(0X09)
#define TLL (0x82)
#define TLH (0x83)
#define THL (0X84)
#define THH (0X85)
#define INTERRUPT_REG (0X86)
#define INTERRUPT_VALUE (0X05)
#define POWER_OFF_CMD (0x00)
#define CH0_L (0x8C)
#define CH0_H (0x8D)
#define CH1_L (0x8E)
#define CH1_H (0x8F)
#define slave_addr     (0x48)
#define tempregaddr    (00)
#define inicond        (0xA060)
#define configregaddr  (0x01)
#define tlowregaddr    (0x02)
#define thighregaddr   (0x03)
#define highmask       (0x00FF)

typedef enum //error or success enum
{
	error=0,
	success=1
}error_check;

/***************************
* Global variables
***************************/
int val;
int16_t celcius=0,kelvin=0,fahrenheit=0;

/***********************
*i2c_file function
************************/

uint8_t i2c_file(int32_t fd)
{
	fd=open("/dev/i2c-2", O_RDWR);
	if (fd<0)
	{
		return error;
	}
	if(ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR)<0)
	{
		return error;
	}
	return success;
}

/********************
* Write operation 
***********************/

uint8_t i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd, &regval, sizeof(regval))!=sizeof(regval))
	{
		return error;
	}
	return success;
}

/***************************
* Read operation
****************************/

uint8_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	if(read(fd, buffer, size)!=size)
	{
		return error;
	}
	return success;

}

/***************************
* id register test
* checks if it is reads as 50
* to identify the sensor
****************************/

uint8_t id_reg_test(int32_t fd) //identification register- startup light test
{
	uint8_t comm=ID_REGISTER;
	uint8_t value;
	int32_t check=0;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=50)
	{
		return error;
	}
	return value;
}

int main()
{
	float lux_output;
	uint8_t op, id_op, op1, readop;
	int32_t error=0;	
	int32_t fd;
	uint8_t powerval=0,sensor_id=0, timer=0, interr=0;
	//i2c_init
	op=i2c_file(fd); //i2c file
	//sensor connection test
	if(op==1)
	{
		printf("The initialization is done and the  light sensor is connected\n");
	}
	else
	{
		perror("The light sensor is not connected thus I/O error\n");
	}
	//identification test
	id_op=id_reg_test(fd);
	if(id_op==50)
	{
		printf("The identification register matches - BIST Successfull\n");
	}
	else
	{
		perror(" Identification register isn't correct- BIST test failed\n");
	}
	//power on	
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
	error=i2c_read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		printf("the value of power is %x\n", powerval);
		printf("The light sensor is powered up and ready to read data\n");
	}
	else
	{
		perror("The light sensor has not been powered up\n");
	}
	//i2c test
	op1=i2c_write(fd,TIMING_REG);
	error=i2c_read(fd,&readop,1);
	if(readop==81)
	{
		printf("The I2C works for light - BIST Successfull\n");
	}
	else
	{
		perror("The I2C fails for light- BIST is not successfull\n");
	}
	//temp sensor
	op=i2c_file(fd); //i2c file
	//sensor connection test
	if(op==1)
	{
		printf("The initialization is done and the  temp sensor is connected\n");
	}
	else
	{
		perror("The temp sensor is not connected thus I/O error\n");
	}	
	op1=i2c_write(fd,configregaddr);
	error=i2c_read(fd,&readop,1);
	if(readop==81)
	{
		printf("The I2C works for temp - BIST Successfull\n");
	}
	else
	{
		perror("The I2C fails for temp - BIST is not successfull\n");
	}
	
}
