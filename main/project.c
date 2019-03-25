#include "common.h"

int32_t shm_temp=0,shm_light=0;
uint8_t* shm_ptr;
int32_t celcius=0,fahrenheit=0,kelvin=0,luminosity=0;

static void get_temperature (void)
{
	log_t temp_data;
	sem_t* sem_temp = sem_open(shm_temp_id, O_CREAT, 0644,1);
	if(!fork())
	{
		execvp(temp_exec[0],temp_exec);
	}
	else
	{
		wait(NULL);
	}
	//sem_wait(sem_temp);
	shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	//sem_post(sem_temp);
    	sem_unlink(shm_temp_id);
	printf("temperature = %d\n",temp_data.data[celcius_id]);
}

static void get_luminosity(void)
{
	log_t light_data;
	sem_t* sem_light = sem_open(shm_light_id, O_CREAT, 0644,1);
	if(!fork())
	{
		execvp(light_exec[0],light_exec);
	}
	else
	{
		wait(NULL);
	}
	//sem_wait(sem_light);
	shm_ptr=shmat(shm_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	//sem_post(sem_light);
    	sem_unlink(shm_light_id);
	printf("luminosity = %d\n",light_data.data[luminosity_id]);
}

void main(void)
{
	key_t key_temp = ftok(shm_temp_id,project_id);
	key_t key_light = ftok(shm_light_id,project_id);
	shm_temp=shmget(key_temp,LOG_SIZE,0666|IPC_CREAT);
	shm_light=shmget(key_light,LOG_SIZE,0666|IPC_CREAT);
	get_temperature();
	get_luminosity();
}
