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
#define inicond        (0xA060)
#define configregaddr  (01)
#define tlowregaddr    (0x02)
#define thighregaddr   (03)
#define highmask       (0x00FF)

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

uint16_t configreg_write_func(uint16_t regval)
{

	uint8_t buffer1,buffer2;
	buffer1 = (inicond|regval)>>8;
	buffer2 = (inicond|regval);
	uint8_t buffer[3] = {configregaddr,buffer2,buffer1};	
	write_func(configregaddr);
	val = write(temp_fd, &buffer, sizeof(buffer));
	return regval;

}
 
uint16_t configreg_read_func()
{
	
	uint16_t buffer = 0;
	write_func(configregaddr);
	val =  read(temp_fd,&buffer,sizeof(buffer));
	return buffer;
}
uint16_t tfhighreg_read_func()
{
	uint8_t buffer[2] = {0}, valmsb, vallsb;
	uint16_t high;
	write_func(thighregaddr);
	val = read(temp_fd,buffer,sizeof(buffer));
	valmsb = buffer[0];
	vallsb = buffer[1];
	high = ((valmsb << 8 ) | vallsb) >> 4;
	return high;
}

uint16_t thighreg_write_func(uint16_t regval)
{
	uint8_t buffer1,buffer2;
	buffer1 = (80|regval)>>8;
	buffer2 = (80|regval);
	uint8_t buffer[3] = {thighregaddr,buffer2,buffer1};
	write_func(thighregaddr);
	val = write(temp_fd, &buffer, sizeof(buffer));
	return regval;
}
	

uint16_t tlowreg_read_func()
{
	
	uint16_t buffer = 0;
	write_func(tlowregaddr);
	val =  read(temp_fd,&buffer,sizeof(buffer));
	return buffer;
}

uint16_t tlowreg_write_func(uint16_t regval)
{
	int storeval;
	uint8_t buffer1,buffer2;
	write_func(regval);
	buffer1 = (regval)>>8;
	buffer2 = (regval)&highmask;
	uint8_t buffer[3] = {tlowregaddr,buffer1,buffer2};
	storeval = write(val, &buffer, sizeof(buffer));
	if(storeval<0)
	{
		perror("error in storeval\n");		
		return -1;
	}
	return 0;
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
