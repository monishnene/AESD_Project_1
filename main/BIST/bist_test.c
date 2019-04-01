/******************************************
* bist_test.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

#include "bist_test.h"

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

uint8_t i2c_readword(int32_t fd,uint8_t* buff)
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
	i2c_readword(fd,buffer);
	return ((uint16_t)buffer[0]<<8|buffer[1]);
}

int bist_check()
{
	uint8_t op,op1,op2,op3,op4,op5;
	uint16_t op6;
	int32_t error=0;	
	int32_t fd;
	uint8_t powerval=0,sensor_id=0, timer=0, interr=0;
	//i2c_init
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);	
	//power on	
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
	error=i2c_read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		printf("the value of power is %x\n", powerval);
		printf("Device powered on - BIST Successful\n");
	}
	else
	{
		printf("The device is not powered on - BIST Unsuccessful\n");
	}
	//test identification reg
	op3=id_reg_test(fd);
	if(op3==1)
	{
<<<<<<< HEAD
		printf("The I2C works for light - BIST Successfull\n");
	}
	else
	{
		perror("The I2C fails for light- BIST is not successfull\n");
=======
		printf("test identification register successful - BIST Successful\n");
	}
	else
	{
		printf("The device identification register failed to configure - BIST Unsuccessful\n");
>>>>>>> 8c4b43d6cd81bdbbe4f8a5b3a37c53b3c0e8b84b
	}
	//test timing reg
	op4=timing_reg_test(fd);
	if(op4==1)
	{
		printf("test timing register successful - BIST Successful\n");
	}
	else
	{
<<<<<<< HEAD
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
=======
		printf("test timing register fails - BIST Unsuccessful\n");
>>>>>>> 8c4b43d6cd81bdbbe4f8a5b3a37c53b3c0e8b84b
	}
	//i2c_init for temp
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, slave_addr);
	printf("The temperature sensor is connected and it works properly - BIST Successful\n");	
	uint8_t conn= configregaddr;
	write(fd,&conn,1);
	//conn=6;
	//write(fd,&conn,1);
	op6=register_read(fd,configregaddr);
	if(op6==24736)
	{
		printf("The temperature sensor I2C works- BIST successful\n");
	}
	else
	{
		printf("The temperature sensor I2C doesn't work- BIST unsuccessful\n");
	}
	printf("BIST test completed\n");
	return success;
}

