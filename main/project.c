#include "common.h"

int32_t shm_temp=0,shm_light=0;
uint8_t* shm_ptr;
int32_t celcius=0,fahrenheit=0,kelvin=0,luminosity=0;

static void get_temperature (void)
{
	log_t temp_data;
	sem_t* sem_temp = sem_open(shm_temp_id, 0);
	if(!fork())
	{
		execvp(temp_exec[0],temp_exec);
	}
	else
	{
		wait(NULL);
	}
	sem_wait(sem_temp);
	shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	printf("temperature = %d\n",temp_data.data[celcius_id]);
}

static void get_luminosity(void)
{
	log_t light_data;
	sem_t* sem_light = sem_open(shm_light_id, 0);
	if(!fork())
	{
		execvp(light_exec[0],light_exec);
	}
	else
	{
		wait(NULL);
	}
	sem_wait(sem_light);
	shm_ptr=shmat(shm_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
	printf("luminosity = %d\n",light_data.data[luminosity_id]);
}

int32_t system_init(void)
{
	sem_open(shm_temp_id, O_CREAT, 0644,1);
	sem_open(shm_light_id, O_CREAT, 0644,1);
	sem_open(logfile_sem_id, O_CREAT, 0644,1);
	sem_open(i2c_sem_id, O_CREAT, 0644,1);
}

int32_t system_end(void)
{
	sem_unlink(shm_temp_id);	
    	sem_unlink(shm_light_id);
	sem_unlink(logfile_sem_id);
	sem_unlink(i2c_sem_id);
}


int32_t main(int32_t argc, uint8_t **argv)
{
	int32_t error=0;	
	if(argc>1)
	{
		if(!strcmp("init",argv[1]))
		{
			error=system_init();
			if(error<0)
			{
				return error;		
			}
		}
	}
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	get_temperature();
	get_luminosity();	
	if(!fork())
	{
		execvp(logger_exec[0],logger_exec);
	}
	else
	{
		wait(NULL);
	}
	if(argc>1)
	{
		if (!strcmp("end",argv[1]))
		{
			error=system_end();
			if(error<0)
			{
				return error;		
			}	
		}
	}
}
