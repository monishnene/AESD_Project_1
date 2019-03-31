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

#define slave_addr     (0x48)
#define tempregaddr    (00)
#define inicond        (0xA060)
#define configregaddr  (0x01)
#define tlowregaddr    (0x02)
#define thighregaddr   (0x03)
#define highmask       (0x00FF)

typedef enum
{
	error=0,
	success=1
}error_check;

int val;
int16_t celcius=0,kelvin=0,fahrenheit=0;
uint8_t i2c_file(int32_t fd)
{
	fd=open("/dev/i2c-2", O_RDWR);
	if (fd<0)
	{
		return error;
	}
	if(ioctl(fd, I2C_SLAVE, slave_addr)<0)
	{
		return error;
	}
	return success;
}
uint8_t i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd, &regval, sizeof(regval))!=sizeof(regval))
	{
		return error;
	}
	return success;
}

uint8_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	if(read(fd, buffer, size)!=size)
	{
		return error;
	}
	return success;

}

void main()
{
	uint16_t regval, buffer;
	int fd;
	uint8_t op;
	uint8_t data;
	int32_t error=0;
	op=i2c_file(fd);
	if(op==1)
	{
		printf("\n The sensor is connected - BIST Successful\n");
	}
	perror("The sensor is not connected - BIST Unsuccessful\n");
	i2c_write(fd,configregaddr);
	error=i2c_read(fd,&data,1);
	if(data==01)
	{
		printf("The I2C write/read function works properly and the sensor is ready to be read - BIST Successful\n");
	}
	perror("The I2C write/read function doesn't work properly - BIST Unsuccessful\n");
}
