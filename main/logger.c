#include "common.h"
#define STR_SIZE 100

uint8_t* str;
int32_t n;
FILE* fptr;
time_t present_time;
struct tm *time_and_date;

static void log_temperature (void)
{
	log_t temp_data;
	int32_t	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
	sem_t* sem_temp = sem_open(shm_temp_id,0);
	sem_wait(sem_temp);
	uint8_t* shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	present_time=(time_t)temp_data.timestamp.tv_sec;
	time_and_date = localtime(&present_time);
	sprintf(str,"\nTime & Date : %s(%ld ns), log ID: %d\nTemperature: %d°C, %d°K, %d°F\n",asctime(time_and_date),temp_data.timestamp.tv_nsec,temp_data.log_id,temp_data.data[celcius_id],temp_data.data[kelvin_id],temp_data.data[fahrenheit_id]);	
	n=fwrite(str,1,strlen(str),fptr);
}

static void log_luminosity(void)
{
	log_t light_data;	
	int32_t	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	sem_t* sem_light = sem_open(shm_light_id,0);
	sem_wait(sem_light);
	uint8_t* shm_ptr=shmat(shm_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);	
	present_time=(time_t)light_data.timestamp.tv_sec;
	time_and_date = localtime(&present_time);
	sprintf(str,"\nTime & Date : %s(%ld ns), log ID: %d\nLuminosity: %d%%\n",asctime(time_and_date),light_data.timestamp.tv_nsec,light_data.log_id,light_data.data[luminosity_id]);
	n=fwrite(str,1,strlen(str),fptr);
}

void main(void)
{		
	led_toggle(logger_led);
	str=(uint8_t*)calloc(STR_SIZE,1);
	uint32_t n=0,customer_id=0;	
	sem_t* sem_logfile = sem_open(logfile_sem_id, 0);
	sem_wait(sem_logfile);
	fptr=fopen(logfile,"a");
	if(fptr==NULL)
	{
		printf("File opening error");
	}	
	log_temperature();
	bzero(str,STR_SIZE);
	log_luminosity();
	fclose(fptr);	
	sem_post(sem_logfile);
	free(str);
}

