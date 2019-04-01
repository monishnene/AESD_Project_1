/******************************************
* bist_test.c
* Advanced Embedded Software Development Project 1 
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
* @brief This file has built in self test funcitons
*******************************************/

#include "bist_test.h"
sem_t* sem_i2c;

/***********************************************************************
 * 
 * This funtion has the initialization and main loop with switch case for udp client
 * @param fd file descriptor
 * @brief 
***********************************************************************/
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
uint8_t i2c_writeb(int32_t fd,uint8_t regval)
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

uint8_t i2c_readb(int32_t fd,uint8_t* buffer,uint32_t size)
{
	if(read(fd, buffer, size)!=size)
	{
		return error;
	}
	return success;
}

/*************************
* command register test
****************************/

uint8_t cmdreg_write_test(int32_t fd)
{
	uint8_t comm=START_COMMAND; //sending stard command = 80
	if(write(fd, &comm, 1) < 0)
	{
		return error;
	}
	return success;
}

/************************************************
* identification register test
* To write and read the identification register
************************************************/

uint8_t id_reg_test(int32_t fd)
{
	uint8_t comm=ID_REGISTER;
	uint8_t value;
	int32_t check=0;
	write(fd,&comm,1);
	comm=ID_VAL;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=ID_VAL)
	{
		return error;
	}
	return success;
}

/******************************************
* timing register test
* To write and read the timing register
******************************************/

uint8_t timing_reg_test(int32_t fd)
{
	uint8_t value;
	uint8_t check=0;
	uint8_t comm=TIMING_REG;
	write(fd,&comm,1);
	comm=TIMING_VAL;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=TIMING_VAL)
	{
		return error;
	}
	return success;
}

uint8_t i2c_readbword(int32_t fd,uint8_t* buff)
{
	if(read(fd, buff, 2)!=2)
	{
		return error;
	}
	return success;

}

uint16_t register_read(int32_t fd, uint8_t regval)
{
	uint8_t* buffer=malloc(sizeof(uint8_t)*2);
	write(fd,&regval,sizeof(regval));
	i2c_readbword(fd,buffer);
	return ((uint16_t)buffer[0]<<8|buffer[1]);
}
int bist_check()
{
	uint8_t op,op1,op2,op3,op4,op5;
	uint16_t op6;
	int32_t error=0;	
	int32_t fd;
	uint8_t powerval=0,sensor_id=0, timer=0, interr=0;	
	sem_i2c = sem_open(i2c_sem_id,0);
	//i2c_init
	sem_wait(sem_i2c);
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);	
	//power on	
	i2c_writeb(fd,POWER_ADDR);
	i2c_writeb(fd,POWER_ON_CMD);
	error=i2c_readb(fd,&powerval,1);
	sem_post(sem_i2c);
	//test timing reg
	op4=timing_reg_test(fd);
	if(op4==1)
	{
		log_creator(LOG_INFO,"LUX sensor test timing register successful - BIST Successful");
	}
	else
	{
		log_creator(LOG_ERROR,"LUX sensor test timing register fails - BIST Unsuccessful");
	}
	//i2c_init for temp	
	sem_wait(sem_i2c);
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, slave_addr);
	log_creator(LOG_INFO,"Temperature sensor is connected and it works properly - BIST Successful");	
	uint8_t conn= configregaddr;
	write(fd,&conn,1);
	//conn=6;
	//write(fd,&conn,1);
	op6=register_read(fd,configregaddr);	
	sem_post(sem_i2c);
	if(op6==24736)
	{
		log_creator(LOG_INFO,"Temperature sensor I2C works- BIST successful");
	}
	else
	{
		log_creator(LOG_ERROR,"Temperature sensor I2C doesn't work- BIST unsuccessful");
	}
	return success;
}

