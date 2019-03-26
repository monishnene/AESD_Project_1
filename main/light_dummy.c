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
	key_t key_light;
	int32_t shm_light;
	sem_t* sem_light;
	//data collection
	log_data.data[luminosity_id]=get_luminosity();
	printf("luminosity dummy = %d\n",log_data.data[luminosity_id]);
	clock_gettime(CLOCK_REALTIME,&log_data.timestamp);
	log_data.header=light_id;
	log_data.log_id=rand();
	//semaphore and memory init
	sem_light = sem_open(shm_light_id, O_CREAT, 0644, 1);
	key_light = ftok(shm_light_id,project_id);
	shm_light=shmget(key_light,LOG_SIZE,0666|IPC_CREAT);
	//shared memory send
	sem_wait(sem_light);
	shm_ptr=shmat(shm_light,(void*)0,0);	
	memcpy(shm_ptr,&log_data,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
}

