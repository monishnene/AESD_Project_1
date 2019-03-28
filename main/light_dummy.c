#include "common.h"

static int16_t get_luminosity()
{
	srand(time(NULL));
	return rand()%100;
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

