/*********************
*Temperature task
*Author: Sanika Dongre
*Date created: 03/25/19
***********************/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <time.h>
#define TEMP_SLAVE_ADDR	(0x48)
#define TEMP_REG_ADDR	(00)

void i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd, &regval, sizeof(regval))<0)
	{
		perror("write function has been failed");
	}
}

int32_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	return read(fd, buffer, size);
}

float get_temperature()
{
	float data=0;
	int32_t error=0,temp_fd=0;
	uint8_t buffer[2];
	//file open
	temp_fd=open("/dev/i2c-2", O_RDWR);
	ioctl(temp_fd, I2C_SLAVE, TEMP_SLAVE_ADDR);
	//sensor tasks
	i2c_write(temp_fd,TEMP_REG_ADDR);
	error = i2c_read(temp_fd,buffer,sizeof(buffer));
	data = (((buffer[0] << 8) | buffer[1]) >> 4)*0.0625;
	return data;
}

void main()
{
	float celcius=0,kelvin=0,fahrenheit=0;
	celcius = get_temperature();
	fahrenheit=celcius*1.8+32; // celcius to Fahrenheit
	kelvin=celcius+273.15; // celcius to kelvin
	printf("\ntemperature value in celcius is %f\n", celcius);
	printf("\ntemperature value in fahrenheit is %f\n", fahrenheit);
	printf("\ntemperature value in kelvin is %f\n", kelvin);
}
