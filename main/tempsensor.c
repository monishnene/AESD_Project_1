#include <stdio.h>
#include <errno.h>
#include <string.h>
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
#define configregaddr  (0x01)
#define tlowregaddr    (0x02)
#define thighregaddr   (03)
#define highmask       (0x00FF)

int val;
int16_t celcius=0,kelvin=0,fahrenheit=0;

int temp_file_func()
{
	int output;
	output= open("/dev/i2c-2", O_RDWR);
	ioctl(output, I2C_SLAVE, slave_addr);
	return output;
}

void write_func(int temp_fd, uint8_t regval)
{
	val= write(temp_fd, &regval, sizeof(regval));
	if(val<0)
	{
		perror("write function has been failed");
	}
}

uint16_t configreg_write_func(int temp_fd, uint16_t regval)
{

	uint8_t buffer1,buffer2;
	buffer1 = (inicond|regval)>>8;
	buffer2 = (inicond|regval);
	uint8_t buffer[3] = {configregaddr,buffer2,buffer1};	
	write_func(temp_fd, configregaddr);
	val = write(temp_fd, &buffer, sizeof(buffer));
	return regval;

}
 
uint16_t configreg_read_func(int temp_fd)
{
	
	uint16_t buffer = 0;
	write_func(temp_fd, configregaddr);
	val =  read(temp_fd,&buffer,sizeof(buffer));
	return buffer;
}
uint16_t thighreg_read_func(int temp_fd)
{
	uint8_t buffer[2] = {0}, valmsb, vallsb;
	uint16_t high;
	write_func(temp_fd, thighregaddr);
	val = read(temp_fd,buffer,sizeof(buffer));
	valmsb = buffer[0];
	vallsb = buffer[1];
	high = ((valmsb << 8 ) | vallsb) >> 4;
	return high;
}

uint16_t thighreg_write_func(int temp_fd, uint16_t regval)
{
	uint8_t buffer1,buffer2;
	buffer1 = (80|regval)>>8;
	buffer2 = (80|regval);
	uint8_t buffer[3] = {thighregaddr,buffer2,buffer1};
	write_func(temp_fd, thighregaddr);
	val = write(temp_fd, &buffer, sizeof(buffer));
	return regval;
}
	

uint16_t tlowreg_read_func(int temp_fd)
{
	
	uint16_t buffer = 0;
	write_func(temp_fd, tlowregaddr);
	val =  read(temp_fd,&buffer,sizeof(buffer));
	return buffer;
}

uint16_t tlowreg_write_func(int temp_fd, uint16_t regval)
{
	int storeval;
	uint8_t buffer1,buffer2;
	write_func(temp_fd, regval);
	buffer1 = (regval)>>8;
	buffer2 = (regval)& 0x00FF ;
	uint8_t buffer[3] = {tlowregaddr,buffer1,buffer2};
	storeval = write(val, &buffer, sizeof(buffer));
	if(storeval<0)
	{
		return -1;
	}
	return 0;
}

/*int config_reg_fault_bits(int temp_fd, uint16_t regval)
{
	int faultbitsval;	
	write_func(temp_fd,regval);	
	uint8_t buffer[2]={configregaddr,regval};
	faultbitsval = write(val, &buffer, sizeof(buffer));
	return 0;
}

uint16_t config_reg_fault_bits_read_func(int temp_fd)
{
	int optval;		
	uint8_t buffer[1]={0};	
	uint16_t op_fault=0;
	write_func(temp_fd,regval);
	optval = read(val, &op_fault,sizeof(op_fault));
	return faultbitsval;
}
	
int config_reg_EM_func(int temp_fd, uint16_t regval)
{
	int optval;	
	write_func(temp_fd,regval);
	uint8_t buffer[2] = {controlregaddr,regval};
	optval=write(val, &buffer, sizeof(buffer));
	return 0;
}

uint16_t config_reg_EM_read_func(int temp_fd)
{
	int d_read;	
	uint8_t buffer[1]={0};
	uint16_t em_read=0;
	write_func(temp_fd,regval);
	em_read=read(val,&em_read,sizeof(em_read));
	return em_read;
}*/
		
int get_temperature(int temp_fd)
{
	int data, h_bit=0;
	uint8_t buffer[2], valmsb, vallsb;
	write_func(temp_fd, tempregaddr);
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
	uint16_t regval, buffer;
	int temp_fd;
	temp_fd = temp_file_func();
	regval = 11;
	regval = configreg_write_func(temp_fd, regval);
	buffer= configreg_read_func(temp_fd);
	printf("The config register value is %d\n", buffer);
	regval = 10;
	thighreg_write_func(temp_fd, regval);
	buffer = thighreg_read_func(temp_fd);
	printf("The thigh register value is %d\n", buffer);
	regval = 13;
	tlowreg_write_func(temp_fd, regval);
	buffer = tlowreg_read_func(temp_fd);
	printf("The tlow register value is %d\n", buffer);
	celcius = get_temperature(temp_fd);
	fahrenheit=celcius*1.8+32; // celcius to Fahrenheit
	kelvin=celcius+273.15; // celcius to kelvin
	printf("\ntemperature value in celcius is %d\n", celcius);
	printf("\ntemperature value in fahrenheit is %d\n", fahrenheit);
	printf("\ntemperature value in kelvin is %d\n", kelvin);
}
