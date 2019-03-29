#include "common.h"
#include <math.h>
#define ID_VALUE (0x50)
#define ID_REGISTER (0x8A)
#define LUX_SLAVE_ADDR (0x39)
#define POWER_ADDR (0x80)
#define TIMING_REG (0X81)
#define TIMING_VAL (0X12)
#define POWER_ON_CMD (0x3)
#define POWER_OFF_CMD (0x00)
#define CH0_L (0x8C)
#define CH0_H (0x8D)
#define CH1_L (0x8E)
#define CH1_H (0x8F)

static int16_t get_luminosity()
{
	led_toggle(light_led);
	uint8_t sensor_id=0, powerval=0, timer=0;
	int32_t error=0,fd=0;
	uint8_t ch0_l=0,ch1_l=0,ch0_h=0,ch1_h=0;
	uint16_t ch0=0,ch1=0;
	float adcval=0.0;
	int16_t lux_output=0;
	//i2c init
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);	
	//power on	
	error=write(fd,POWER_ADDR,1);
	error=write(fd,POWER_ON_CMD,1);
	error=read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		printf("the value of power is %x\n", powerval);
	}
	error=write(fd,ID_REGISTER,1);
	error=read(fd,&sensor_id,1);
	if(sensor_id==ID_VALUE)
	{
		printf("LUX power on sensor_id=%x\n",sensor_id);
	}
	error=write(fd,TIMING_REG,1);
	error=write(fd,TIMING_VAL,1);
	error=read(fd,&timer,1);
	if(timer==TIMING_VAL)
	{
		printf("the timer value is %x\n",timer);
	}
	//read channels
	error=write(fd,CH0_L,1);
	error=read(fd,&ch0_l,1);
	error=write(fd,CH0_H,1);
	error=read(fd,&ch0_h,1);
	error=write(fd,CH1_L,1);
	error=read(fd,&ch1_l,1);
	error=write(fd,CH1_H,1);
	error=read(fd,&ch1_h,1);	
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
	sensor_id=0;
	error=write(fd,POWER_ADDR,1);
	error=write(fd,POWER_OFF_CMD,1);
	error=write(fd,ID_REGISTER,1);
	error=read(fd,&sensor_id,1);
	if(sensor_id!=ID_VALUE)
	{
		printf("LUX power off\n");
	}	
	//file open	
	return (int16_t)lux_output;
}

void main(void)
{		
	//declare variables
	log_t log_data;	
	uint8_t* shm_ptr;
	int32_t shm_light;
	sem_t* sem_light;
	//data collection
	log_data.data[luminosity_id]=get_luminosity();
	printf("luminosity dummy = %d\n",log_data.data[luminosity_id]);
	clock_gettime(CLOCK_REALTIME,&log_data.timestamp);
	log_data.header=light_id;
	log_data.log_id=rand();
	//semaphore and memory init
	sem_light = sem_open(shm_light_id,0);
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	//shared memory send
	sem_wait(sem_light);
	shm_ptr=shmat(shm_light,(void*)0,0);	
	memcpy(shm_ptr,&log_data,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
}

