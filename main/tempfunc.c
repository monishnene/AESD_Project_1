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

#define slave_addr     (0x48)
#define tempregaddr    (00)

int val;
int temp_fd;
int16_t celcius=0,kelvin=0,fahrenheit=0;

int temp_file_func()
{
	int output;
	output= open("/dev/i2c-2", O_RDWR);
	ioctl(output, I2C_SLAVE, slave_addr);
	return output;
}

void write_func(uint8_t regval)
{
	val= write(temp_fd, &regval, sizeof(regval));
	if(val<0)
	{
		perror("write function has been failed");
	}
}

int get_temperature()
{
	int data, h_bit=0;
	uint8_t buffer[2], valmsb, vallsb;
	write_func(tempregaddr);
	val = read(temp_fd, &buffer, sizeof(buffer));
	valmsb = buffer[0];
	vallsb = buffer[1];
	data = ((valmsb << 8) | vallsb) >> 4;
	if(h_bit!=0)
	{	
		return data;
	}
	else 
	{
		data = data/16;
		return data;
	}
}
void main()
{
	temp_fd = temp_file_func();
	celcius = get_temperature();
	fahrenheit=celcius*1.8+32; // celcius to Fahrenheit
	kelvin=celcius+273.15; // celcius to kelvin
	printf("\ntemperature value in celcius is %d\n", celcius);
	printf("\ntemperature value in fahrenheit is %d\n", fahrenheit);
	printf("\ntemperature value in kelvin is %d\n", kelvin);
}
