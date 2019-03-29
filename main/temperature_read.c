#include "common.h"
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

static int32_t get_temperature()
{
	int32_t data=0;
	int32_t error=0,fd=0;
	uint8_t buffer[2];
	//file open
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, TEMP_SLAVE_ADDR);
	//sensor tasks
	i2c_write(fd,TEMP_REG_ADDR);
	error = i2c_read(fd,buffer,sizeof(buffer));
	data = (((buffer[0] << 8) | buffer[1]) >> 4)/16.0;
	return data;
}

void main(void)
{		
	led_toggle(temperature_led);
	//declare variables
	log_t log_data;	
	uint8_t* shm_ptr;
	int32_t shm_temp=0,celcius=0;
	sem_t* sem_temp;
	//data collection
	celcius = get_temperature();
	log_data.data[celcius_id]=celcius;
	clock_gettime(CLOCK_REALTIME,&log_data.timestamp);
	log_data.header=temperature_id;
	log_data.data[fahrenheit_id]=(celcius*1.8)+32.0; // celcuis to Fahrenheit
	log_data.data[kelvin_id]=celcius+273.15; // celcius to kelvin
	log_data.log_id=rand();
	//semaphore and memory init
	sem_temp = sem_open(shm_temp_id,0);
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
	//shared memory send
	sem_wait(sem_temp);
	shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(shm_ptr,&log_data,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
}
