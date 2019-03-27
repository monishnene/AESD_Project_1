#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/i2c-dev.h>
#include <math.h>

#define mask 0x0A
#define greater 0x02    
#define low_time 0x00  
#define timingreg 0x81
#define gain_max 0x10  
#define wr_data 0x80
#define interr 0x86
#define th1 0x82
#define th2 0x83
#define th3 0x84
#define channel0_low 0x8C
#define channel0_high 0x8D
#define channel1_low 0x8E
#define channel1_high 0x8F
#define th4 0x85
#define read_data 0x03
int slave_address = 0x39;
uint8_t value;
uint8_t init_light_sensor()
{
	int output;
	output = open("/dev/i2c-2", O_RDWR);
	if(ioctl(output, I2C_SLAVE, slave_address) < 0)
	{
		perror("error in ioctl\n");
		exit(-1);
	}
	return output;
}


uint8_t cmdreg_write_func(int light_fd)
{
	value = wr_data;
	if(write(light_fd, &value, 1) < 0)
	{
		perror("error in write function");
		exit(-1);
	}
}

uint8_t ctrl_reg_write_func(int light_fd)
{
	uint8_t value_r;
	value = wr_data;
	write(light_fd, &value, 1);
	value_r = read_data;
	write(light_fd, &value_r, 1);
	return value;
}

uint8_t reading_func(int light_fd)
{
	read(light_fd, &value, 1);
	printf("\n The output data is %x",value);
	return value;
}

uint8_t timing_reg_write_func(int light_fd)
{
	value = timingreg;
	write(light_fd, &value, 1);
	value = greater|gain_max; 
	write(light_fd, &value, 1);
	return value;
}

uint8_t timing_reg_write_func_interrupt(int light_fd)
{
	value = interr;
	write(light_fd, &value, 1);
	value = low_time; 
	write(light_fd, &value, 1);
	return value;
}

uint8_t id_reg_read_func(int light_fd)
{
	value = wr_data | mask;
	write(light_fd, &value, 1);
	read(light_fd, &value, 1);
	printf("\ntiming identification is %x",value);
	return value;
}

int th_write_func(int light_fd, int *data_buff)
{
	int value_ob;
	value_ob = th1;
	write(light_fd, &value_ob,1) ;
	value_ob = data_buff[0];
	write(light_fd, &value_ob, 1);
	value_ob = th2;
  	write(light_fd, &value_ob, 1);
	value_ob = data_buff[1];
	write(light_fd, &value_ob, 1);
	value_ob = th3;
	write(light_fd, &value_ob, 1);
	value_ob = data_buff[2];
 	write(light_fd, &value_ob, 1);
	value_ob = th4;
	write(light_fd, &value_ob, 1);
	value_ob = data_buff[3];
	write(light_fd, &value_ob, 1);
	return 0;

}

void th_read_func(int light_fd, int *data_buff)
{
	int value_ob;
	value_ob = th1;
	read(light_fd, &value_ob,1) ;
	value_ob = data_buff[0];
	read(light_fd, &value_ob, 1);
	printf("\n first parameter is %x \n", data_buff[0]);
	value_ob = th2;
  	read(light_fd, &value_ob, 1);
	value_ob = data_buff[1];
	read(light_fd, &value_ob, 1);
	printf("\n second parameter is %x \n", data_buff[1]);
	value_ob = th3;
	read(light_fd, &value_ob, 1);
	value_ob = data_buff[2];
 	read(light_fd, &value_ob, 1);
	printf("\n third parameter is %x \n", data_buff[2]);
	value_ob = th4;
	read(light_fd, &value_ob, 1);
	value_ob = data_buff[3];
	read(light_fd, &value_ob, 1);
	printf("\n the fourth parameter is %x \n", data_buff[3]);

}

uint16_t channel0_read(int light_fd)
{
	uint8_t lsbval;	
	uint16_t msbval, outval;
	int value_ob;
	value_ob =  channel0_low;
	write(light_fd, &value_ob, 1);
	read(light_fd, &lsbval, 1);
	value_ob =  channel0_high;
  	write(light_fd, &value_ob, 1);
	read(light_fd, &msbval, 1);
	outval = msbval<<8 | lsbval;
	return outval;
}

uint16_t channel1_read(int light_fd)
{
	int value_ob;
	uint8_t lsbval;
	uint16_t msbval, outval;
	value_ob =  channel1_low;
	write(light_fd, &value_ob, 1);
  	read(light_fd, &lsbval, 1);
	value_ob =  channel1_high;
	write(light_fd, &value_ob, 1);
	read(light_fd, &msbval, 1);
	outval = msbval<<8 | lsbval;
	return outval;
}

float get_luminosity(int light_fd)
{
	float datach0,datach1;
	float adcval,lux_output;
	datach0=0;
	datach1=0;
	datach0 = (float)channel0_read(light_fd);
	datach1 = (float)channel1_read(light_fd);
	adcval = datach0/datach1;
	if(adcval>0 && adcval <= 0.5)
	{
		lux_output = (0.0304 * datach0) - (0.062 * datach0 * pow(adcval, 1.4));
	}
	else if((adcval>0.5) && (adcval<=0.61))
	{
		lux_output = (0.0224 * datach0) - (0.031 * datach1);
	} 
	else if((adcval>0.61)&&(adcval<=0.80))
	{
        	lux_output = (0.0128 * datach0) - (0.0153 * datach1);
	}
	else if((adcval>0.80) && (adcval<=1.30))
	{
        	lux_output = (0.00146 * datach0) - (0.00112 * datach1);
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
	int main_fd, s_output[5] = {0,0,0,0,0};
	init_light_sensor();
	ctrl_reg_write_func(main_fd);
	reading_func(main_fd);
	timing_reg_write_func(main_fd);
	reading_func(main_fd);
	timing_reg_write_func_interrupt(main_fd);
	reading_func(main_fd);
	id_reg_read_func(main_fd);
	th_write_func(main_fd, s_output);
	th_read_func(main_fd, s_output);
	lux_output = get_luminosity(main_fd);
	printf("The obtained lux value is %f\n", lux_output);
}
