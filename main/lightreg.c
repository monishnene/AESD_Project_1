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
	if(write(fd, &regval, sizeof(regval))<0)
	{
		perror("write function has been failed");
	}
}

int32_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	return read(fd, buffer, size);
}

uint8_t cmdreg_write(int32_t fd)
{
	uint8_t comm= START_COMMAND;
	if(write(fd, &comm,1) < 0)
	{
		perror("error in write function");
		exit(-1);
	}
}
void control_reg_write(int32_t fd)
{
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
}

void control_reg_read(int32_t fd)
{
	int32_t error=0;
	uint8_t powerval=0;
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
	error=i2c_read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		printf("powerval is %x'n", powerval);		
		//return powerval;
		
	}
	//else return 0;
}

void id_reg_write(int32_t fd)
{
	i2c_write(fd,ID_REGISTER);

}
uint8_t id_reg_read(int32_t fd)
{
	uint8_t sensor_id=0;
	int32_t error=0;
	error=i2c_read(fd,&sensor_id,1);
	if(sensor_id==ID_VALUE)
	{
		return sensor_id;
	}
	else return 0;
}

void timing_reg_write(int32_t fd)
{
	i2c_write(fd,TIMING_REG);
	i2c_write(fd,TIMING_VAL);
}

uint8_t timing_reg_read(int32_t fd)
{
	int32_t error=0;
	uint8_t timer=0;
	error=i2c_read(fd,&timer,1);
	if(timer==TIMING_VAL)
	{
		return timer;
	}
	else return 0;
}

void int_control_reg_write(int32_t fd)
{
	i2c_write(fd,INTERRUPT_REG);
	i2c_write(fd,INTERRUPT_VALUE);
}

uint8_t int_control_reg_read(int32_t fd)
{
	int32_t error=0;
	uint8_t interr=0;
	error=i2c_read(fd,&interr,1);
	if(interr==INTERRUPT_VALUE)
	{
		return interr;
	}
	else return 0;
}

void int_threshold_write(int32_t fd, int *data_buff)
{
	uint8_t value_ob;
	value_ob = TLL;
	write(fd, &value_ob,1) ;
	value_ob = data_buff[0];
	write(fd, &value_ob, 1);
	value_ob = TLH;
  	write(fd, &value_ob, 1);
	value_ob = data_buff[1];
	write(fd, &value_ob, 1);
	value_ob = THL;
	write(fd, &value_ob, 1);
	value_ob = data_buff[2];
 	write(fd, &value_ob, 1);
	value_ob = THH;
	write(fd, &value_ob, 1);
	value_ob = data_buff[3];
	write(fd, &value_ob, 1);
}

int* int_threshold_read(int fd, int *data_buff)
{
	uint8_t value_ob;
	value_ob = TLL;
	write(fd,&value_ob,1);
	read(fd, &value_ob,1) ;
	data_buff[0] = value_ob;
	value_ob = TLH;
	write(fd, &value_ob, 1);
	read(fd,&value_ob,1);
	data_buff[1]= value_ob;
	value_ob = THL;
	write(fd,&value_ob,1);
  	read(fd, &value_ob, 1);
	data_buff[2]= value_ob;
	value_ob = THH;
	write(fd,&value_ob,1);
	read(fd,&value_ob,1);
	data_buff[3] = value_ob;
	return data_buff;
}

uint16_t channel0_read(int fd)
{
	uint8_t lsbval;	
	uint16_t msbval, outval;
	int value_ob;
	value_ob =  CH0_L;
	write(fd, &value_ob, 1);
	read(fd, &lsbval, 1);
	value_ob =  CH0_H;
  	write(fd, &value_ob, 1);
	read(fd, &msbval, 1);
	outval = msbval<<8 | lsbval;
	return outval;
}

uint16_t channel1_read(int fd)
{
	int value_ob;
	uint8_t lsbval;
	uint16_t msbval, outval;
	value_ob =  CH1_L;
	write(fd, &value_ob, 1);
  	read(fd, &lsbval, 1);
	value_ob =  CH1_H;
	write(fd, &value_ob, 1);
	read(fd, &msbval, 1);
	outval = msbval<<8 | lsbval;
	return outval;
}

void poweroff_func(int32_t fd)
{
	uint8_t sensor_id;
	int32_t error=0;	
	sensor_id=0;
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_OFF_CMD);
	i2c_write(fd,ID_REGISTER);
	error=i2c_read(fd,&sensor_id,1);
	if(sensor_id!=ID_VALUE)
	{
		printf("LUX power off\n");
	}	
}

static int16_t get_luminosity()
{
	uint8_t sensor_id=0, powerval=0, timer=0;
	int32_t error=0,fd=0;
	uint8_t ch0_l=0,ch1_l=0,ch0_h=0,ch1_h=0;
	uint16_t ch0=0,ch1=0;
	float adcval=0.0;
	int16_t lux_output=0;
	//read channels
	i2c_write(fd,CH0_L);
	i2c_read(fd,&ch0_l,1);
	i2c_write(fd,CH0_H);
	i2c_read(fd,&ch0_h,1);
	i2c_write(fd,CH1_L);
	i2c_read(fd,&ch1_l,1);
	i2c_write(fd,CH1_H);
	i2c_read(fd,&ch1_h,1);	
	printf("ch0h=%d,ch0l=%d,ch1h=%d,ch1l=%d\n",ch0_h,ch0_l,ch1_h,ch1_l);
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
	//power off
	poweroff_func(fd);
	//file open	
	return (int16_t)lux_output;
}

int main()
{
	int16_t lux_output;	
	int32_t fd;
	int s_output[4] = {10,1,2,3};
	int* readop;
	uint8_t powerval=0,sensor_id=0, timer=0, interr=0;
	//i2c init
	i2c_file(fd);	
	//power on	
	//control_reg_write(fd);
	control_reg_read(fd);
	//printf("the value of power is %x\n", powerval);
	//identification register
	id_reg_write(fd);
	sensor_id= id_reg_read(fd);
	printf("LUX power on sensor_id=%x\n",sensor_id);
	//timing reg
	timing_reg_write(fd);
	timer= timing_reg_read(fd);
	printf("the timer value is %x\n",timer);
	//interrupt control reg
	int_control_reg_write(fd);
	interr=int_control_reg_read(fd);
	printf(" The interrupt control register value is %x\n", interr);
	//interrupt threshold registers
	int_threshold_write(fd, s_output);
	readop = int_threshold_read(fd, s_output);
	for(int i=0; i<4; i++)
	{
		printf("The output threshold buffer is %d\n", *(readop+i));
	}
	lux_output = get_luminosity(fd);
	printf("The obtained lux value is %f\n", lux_output);
}
