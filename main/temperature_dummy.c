#include "common.h"

static int16_t get_temperature()
{
	srand(time(NULL));
	return ((rand()%128)-100);
}

void main(void)
{		
	//declare variables
	log_t log_data;	
	uint8_t* shm_ptr;
	key_t key_temp;
	int32_t shm_temp;
	sem_t* sem_temp;
	//data collection
	int32_t celcius = get_temperature();
	printf("temperature dummy = %d\n",celcius);
	log_data.data[celcius_id]=celcius;
	clock_gettime(CLOCK_REALTIME,&log_data.timestamp);
	log_data.header=temperature_id;
	log_data.data[fahrenheit_id]=celcius*1.8+32; // celcuis to Fahrenheit
	log_data.data[kelvin_id]=celcius+273.15; // celcius to kelvin
	log_data.log_id=rand();
	//semaphore and memory init
	sem_temp = sem_open(shm_temp_id, O_CREAT, 0644, 1);
	key_temp = ftok(shm_temp_id,project_id);
	shm_temp=shmget(key_temp,LOG_SIZE,0666|IPC_CREAT);
	//shared memory send
	//sem_wait(sem_temp);
	shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(shm_ptr,&log_data,LOG_SIZE);
	shmdt(shm_ptr);
	//sem_post(sem_temp);
}

