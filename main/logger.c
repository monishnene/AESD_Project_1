#include "logger.h"

uint8_t* str;
int32_t n;
sem_t* sem_logfile;
sem_t* sem_temp;
sem_t* sem_light;
int32_t	shm_light,shm_temp;
time_t present_time;
struct tm *time_and_date;

static void log_temperature (void)
{	
	int32_t error=0;
	printf("Logging Temperature...\n");
	log_t temp_data;
      	//sem_getvalue(sem_temp,&error);
	//printf("sem_temp = %d\n",error);
	sem_wait(sem_temp);
	uint8_t* shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	present_time=(time_t)temp_data.timestamp.tv_sec;
	time_and_date = localtime(&present_time);
	sprintf(str,"\nTime & Date : %s(%ld ns), log ID: %d\nTemperature: %d°C, %d°K, %d°F\n",asctime(time_and_date),temp_data.timestamp.tv_nsec,temp_data.log_id,temp_data.data[celcius_id],temp_data.data[kelvin_id],temp_data.data[fahrenheit_id]);
	printf("Writing to file\n");
	sem_wait(sem_logfile);
	FILE* fptr=fopen(logfile,"a");
	n=fwrite(str,1,strlen(str),fptr);
	fclose(fptr);
	sem_post(sem_logfile);	
	printf("...Temperature Logging Done\n");
}

static void log_luminosity(void)
{		
	int32_t error=0;
	printf("Logging Luminosity...\n");
	log_t light_data;
      	//sem_getvalue(sem_light,&error);
	//printf("sem_light = %d\n",error);	
	sem_wait(sem_light);
	uint8_t* shm_ptr=shmat(shm_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
	present_time=(time_t)light_data.timestamp.tv_sec;
	time_and_date = localtime(&present_time);
	sprintf(str,"\nTime & Date : %s(%ld ns), log ID: %d\nLuminosity: %d%%\n",asctime(time_and_date),light_data.timestamp.tv_nsec,light_data.log_id,light_data.data[luminosity_id]);
	printf("Writing to file\n");
	sem_wait(sem_logfile);	
	FILE* fptr=fopen(logfile,"a");
	n=fwrite(str,1,strlen(str),fptr);
	fclose(fptr);
	sem_post(sem_logfile);	
	printf("...Luminosity Logging Done\n");
}

void logger_init(void)
{
	FILE* fptr;
	int32_t error=0;
	str=(uint8_t*)calloc(STR_SIZE,1);
	printf("Logger Init\n");
	sem_logfile = sem_open(logfile_sem_id, 0);
	sem_temp = sem_open(shm_temp_id,0);
	sem_light = sem_open(shm_light_id,0);
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);	
      	//sem_getvalue(sem_logfile,&error);
	//printf("sem_logfile = %d\n",error);
	sem_wait(sem_logfile);	
	fptr=fopen(logfile,"w");
	if(fptr==NULL)
	{
		printf("File opening error");
	}	
	fclose(fptr);
	sem_post(sem_logfile);
}

void* logger(void* ptr)
{	
	FILE* fptr;
	int32_t error=0;
	printf("Logging...\n");
	led_toggle(logger_led);
	if(fptr==NULL)
	{
		printf("File opening error");
	}
	log_temperature();
	bzero(str,STR_SIZE);
	log_luminosity();	
	bzero(str,STR_SIZE);	
	printf("...Logging Done\n");	
	return ptr;
}
