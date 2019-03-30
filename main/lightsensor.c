#include "common.h"
#include <math.h>
#define ID_VALUE (0x50)
#define ID_REGISTER (0x8A)
#define LUX_SLAVE_ADDR (0x39)
#define POWER_ADDR (0x80)
#define TIMING_REG (0X81)
#define TIMING_VAL (0X12)
#define START_COMMAND (0X80)
#define POWER_ON_CMD (0x3)
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

void i2c_file(int32_t fd)
{
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);
}

void i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd, &regval, sizeof(regval))!=sizeof(regval))
	{
		perror("write function has been failed");
	}
}

int32_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	return read(fd, buffer, size);
}

float get_luminosity(int32_t fd)
{
	uint8_t sensor_id=0, powerval=0, timer=0;
	int32_t error=0;
	uint8_t databuff=1, dataop;
	uint8_t regval;
	uint16_t ch0_l=0,ch1_l=0,ch0_h=0,ch1_h=0;
	uint16_t ch0=0,ch1=0;
	float adcval=0.0;
	int16_t lux_output=0;
	//i2c init
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);	
	//power on	
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
	error=i2c_read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		printf("the value of power is %x\n", powerval);
	}
	//read channels
	uint8_t addr = 0x8C;
	if(write(fd,&addr,1)!=1)
	{
		perror("errror in write ch0l\n");
	}
	if(read(fd,&ch0_l,1)!=1)
	{
		perror("error in read ch0l\n");
	}
	addr=0x8D;
	if(write(fd,&addr,1)!=1)
	{
		perror("error in write ch0h\n");
	}
	if(read(fd,&ch0_h,1)!=1)
	{
		perror("error in read choh\n");
	}
	addr=0x8E;
	if(write(fd,&addr,1)!=1)
	{
		perror("error in write ch1l\n");
	}
	if(read(fd,&ch1_l,1)!=1)
	{
		perror("error in read ch1li\n");
	}
	addr=0x8F;
	if(write(fd,&addr,1)!=1)
	{
		perror("error in write ch1h\n");
	}
	if(read(fd,&ch1_h,1)!=1)
	{
		perror("error in read ch1h\n");
	}
	printf("ch0l=%d,ch1l=%d, ch0h=%d, ch1h=%d\n",ch0_l,ch1_l, ch0_l,ch0_h);
	ch1=(ch1_h<<8)|ch1_l;
	ch0=(ch0_h<<8)|ch0_l;
	adcval = (float)ch1/(float)ch0;	
	printf("ch0=%d,ch1=%d,adcval=%f\n",ch0,ch1,adcval);
	if(adcval>0 && adcval <= 0.5)
	{
		lux_output = (0.0304 * ch0) - (0.062 * ch0 * pow(adcval, 1.4));
	}
	else if(adcval<0.61)
	{
		lux_output = (0.0224 * ch0) - (0.031 * ch1);
	} 
	else if(adcval<0.80)
	{
        	lux_output = (0.0128 * ch0) - (0.0153 * ch1);
	}
	else if(adcval<1.30)
	{
        	lux_output = (0.00146 * ch0) - (0.00112 * ch1);
	}
    	else
	{
		lux_output=0;
	}	
	return lux_output;
}

int main()
{
	float lux_output;
	int32_t fd;
	lux_output = get_luminosity(fd);
	printf("The obtained lux value is %f\n", lux_output);
}
