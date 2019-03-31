#include "common.h"
#include "light_read.h"
#include "logger.h"
#include "temperature_read.h"
#include "server.h"
#define PERIOD 5
#define DEBUG 1
#ifndef DEBUG
#define printf(fmt, ...) (0)
#endif
int32_t shm_temp=0,shm_light=0;
uint8_t trigger=1;
uint8_t* shm_ptr;
void* ptr;
sem_t* sem_temp;
sem_t* sem_light;
int32_t celcius=0,fahrenheit=0,kelvin=0,luminosity=0;
timer_t timerid;
struct sigevent signal_event;
struct itimerspec timer_data;
struct sigaction signal_action;
pthread_t thread_light,thread_temperature,thread_logger,thread_server;
sigset_t mask;

void* temperature_run(void* ptr)
{
	//printf("Temperature run\n");
	temperature_read();
	//printf("Exiting temperature thread\n");
	pthread_exit(ptr);
}


void* light_run(void* ptr)
{
	//printf("Light run\n");
	light_read();
	//printf("Exiting light thread\n");
	pthread_exit(ptr);
}


void* logger_run(void* ptr)
{
	//printf("logger_run\n");
	logger();
	//printf("Exiting log thread\n");
	pthread_exit(ptr);
}

void* server_run(void* ptr)
{
	//printf("server_run\n");
	remote_server();
	//printf("Exiting server thread\n");
	pthread_exit(ptr);
}

static void find_temperature(void)
{
	int32_t error=0;
	log_t temp_data;
	//join thread  here temperature
      	sem_wait(sem_temp);
	shm_ptr=shmat(shm_temp,(void*)0,0);
	//printf("Shared Memory Access\n");
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	printf("temperature = %d\n",temp_data.data[celcius_id]);
	return;
}

static void find_luminosity(void)
{
	int32_t error=0;
	log_t light_data;
	//join thread  here light
      	sem_wait(sem_light);
	shm_ptr=shmat(shm_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
	printf("luminosity = %d\n",light_data.data[luminosity_id]);
	return;
}

void logfile_setup(void)
{
	FILE* fptr=fopen(logfile,"r");
	if(fptr==NULL)
	{
		return;
	}
	uint8_t* new_filename=malloc(STR_SIZE);
	uint32_t counter=1;
	while(fptr!=NULL)
	{
		fclose(fptr);
		sprintf(new_filename,"backup_%d_%s",counter++,logfile);
		fptr=fopen(new_filename,"r");	
	}
	rename(logfile,new_filename);
	return;
}

void system_end(int sig)
{
	condition=0;
	sem_unlink(shm_temp_id);	
    	sem_unlink(shm_light_id);
	sem_unlink(led_sem_id);
	sem_unlink(logfile_sem_id);
	sem_unlink(i2c_sem_id);
}

int32_t bist(void)
{
	int32_t error=0;
	find_temperature();
	find_luminosity();
	return error;
}

static void join_threads(int sig, siginfo_t *si, void *uc)
{
	trigger=1;
}

int32_t timer_init(void)
{	
	int32_t error=0;
	printf("Timer Init\n");
	// Timer init	
	signal_action.sa_flags = SA_SIGINFO;
	signal_action.sa_sigaction = join_threads;//Function to be executed
	sigemptyset(&signal_action.sa_mask);
	error=sigaction(SIGRTMIN, &signal_action, NULL);
        signal_event.sigev_notify = SIGEV_SIGNAL;
        signal_event.sigev_signo = SIGRTMIN;
        signal_event.sigev_value.sival_ptr = &timerid;
        error=timer_create(CLOCK_REALTIME, &signal_event, &timerid);
	/* Start the timer */
        timer_data.it_value.tv_sec = PERIOD;
        timer_data.it_value.tv_nsec = 0;
        timer_data.it_interval.tv_sec = timer_data.it_value.tv_sec;
        timer_data.it_interval.tv_nsec = timer_data.it_value.tv_nsec;
	error=timer_settime(timerid, 0, &timer_data, NULL);
}


int32_t system_init(void)
{
	int32_t error=0;
	condition=1;
	ptr=NULL;
	printf("System Init\n");
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT); 
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	sem_open(shm_temp_id, O_CREAT, 0644,1);
	sem_open(led_sem_id, O_CREAT, 0644,1);
	sem_open(shm_light_id, O_CREAT, 0644,1);
	sem_open(logfile_sem_id, O_CREAT, 0644,1);
	sem_open(i2c_sem_id, O_CREAT, 0644,1);	
	logfile_setup();	
	logger_init();
	led_init();
	temperature_init();
	light_init();
	//Signal Handling
	//signal(SIGUSR1,kill_first_thread);
	//signal(SIGUSR2,break_condition);
	signal(SIGINT,system_end);
	error=timer_init();
	printf("System Init Done\n");
}

int32_t main(int32_t argc, uint8_t **argv)
{
	int32_t error=0;
	ptr=&error;
	if(argc<2)
	{
		printf("%s <logfilename>\n",argv[0]);	
		return 0;
	}
	logfile=argv[1];		
	error=system_init();
	error=bist();
	error=pthread_create(&thread_server,NULL,server_run,NULL);
	if(error)
	{
		printf("Error Creating Server Thread\n");
		kill(getpid(),SIGINT);
	}
	error=pthread_join(thread_server,NULL);
	if(error)
	{	
		printf("Error Joining Server Thread %d\n",error);
	}
	while(condition)
	{
		if(trigger)
		{
			//pthread creation
			error = pthread_create(&thread_temperature,NULL,temperature_run,NULL);
			if(error)
			{
				printf("Error Creating Temperature Thread\n");
				kill(getpid(),SIGINT);
			}
			error = pthread_create(&thread_light,NULL,light_run,NULL);
			if(error)
			{
				printf("Error Creating Light Thread\n");
				kill(getpid(),SIGINT);
			}
			error = pthread_create(&thread_logger,NULL,logger_run,NULL);
			if(error)
			{
				printf("Error Creating Logger Thread\n");
				kill(getpid(),SIGINT);
			}
			printf("Joining Threads\n");
			error=pthread_join(thread_temperature,NULL);
			if(error)
			{
				printf("Error Joining Temperature Thread %d\n",error);
			}
			error=pthread_join(thread_light,NULL);
			if(error)
			{
				printf("Error Joining light Thread %d\n",error);
			}
			error=pthread_join(thread_logger,NULL);
			if(error)
			{
				printf("Error Joining logger Thread %d\n",error);
			}
			trigger=0;
		}
	}
}
