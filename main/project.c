/******************************************
* project.c
* Main task
* Author: Monish Nene and Sanika Dongre
* Date created: 03/28/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include "common.h"
#include "light_read.h"
#include "logger.h"
#include "temperature_read.h"
#include "server.h"

/*******************************************
* Macros
*******************************************/

#define PERIOD 1
#define DEBUG 1
#ifndef DEBUG
#define printf(fmt, ...) (0)
#endif

/*******************************************
* Global variables
*******************************************/

uint8_t* msg;
int32_t shm_temp=0,shm_light=0;
uint8_t trigger=1;
uint8_t* shm_ptr;
void* ptr;
sem_t* sem_temp;
sem_t* sem_light;
sem_t* sem_trigger;
sem_t* sem_logger_ready;
int32_t celcius=0,fahrenheit=0,kelvin=0,luminosity=0;
timer_t timerid;
struct sigevent signal_event;
struct itimerspec timer_data;
struct sigaction signal_action;
pthread_t thread_light,thread_temperature,thread_logger,thread_server;
sigset_t mask;

/*****************************
* Temperature run function
* runs the temperature task
* and exits the thread
********************************/
void* temperature_run(void* ptr)
{
	heartbeat_check[temperature_heart]=1;
	temperature_read();
	pthread_exit(ptr);
}

/*****************************
* light run function
* runs the light task
* and exits the thread
********************************/
void* light_run(void* ptr)
{
	heartbeat_check[light_heart]=1;
	light_read();
	pthread_exit(ptr);
}

/*****************************
* logger run function
* runs the logger task
* and exits the thread
********************************/
void* logger_run(void* ptr)
{
	logger();
	pthread_exit(ptr);
}

/**************************************
* Remote Socket server run function
* runs the socket server task
* and exits the thread
***************************************/
void* server_run(void* ptr)
{	
	remote_server();
	pthread_exit(ptr);	
}

/*****************************
* log file setup function
********************************/
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
		sprintf(new_filename,"backup_%d_%s",counter++,logfile); //to create backup files
		fptr=fopen(new_filename,"r");	
	}
	rename(logfile,new_filename);
	return;
}

/*****************************
* System end function
* unlinks the linked memories for
* different tasks
********************************/
void system_end(int sig)
{
	condition=0;
	sem_unlink(shm_temp_id);	
    	sem_unlink(shm_light_id);
	sem_unlink(trigger_sem_id);
	sem_unlink(led_sem_id);
	sem_unlink(logfile_sem_id);
	sem_unlink(i2c_sem_id);
	sem_unlink(logger_ready_id);
}

/*************************************
* join threads based on trigger value
***************************************/
void heartbeat(void)
{
	uint8_t i=0,count=0,dummy='?',reply=0;
	int32_t socket_desc=0,query=0,error=0;
	struct sockaddr_in sock_heartbeat;
	sock_heartbeat.sin_addr.s_addr = INADDR_ANY;
        sock_heartbeat.sin_family = AF_INET;
	sock_heartbeat.sin_port = htons(LOGPORT);	
	query = socket(AF_INET, SOCK_STREAM, 0);
        error = connect(query, (struct sockaddr *)&sock_heartbeat, sizeof(sock_heartbeat));
	error = write(query,&dummy,sizeof(dummy));
	error = read(query,&reply,sizeof(reply));
	if(dummy==reply)
	{
		heartbeat_check[logger_heart]=1;
	}	
	close(query);	
	reply=0;	
	sock_heartbeat.sin_port = htons(PORT_ADDRESS);	
	query = socket(AF_INET, SOCK_STREAM, 0);
        error = connect(query, (struct sockaddr *)&sock_heartbeat, sizeof(sock_heartbeat));
	error = write(query,&dummy,sizeof(dummy));
	error = read(query,&reply,sizeof(reply));
	if(dummy==reply)
	{
		heartbeat_check[server_heart]=1;
	}	
	close(query);
	for(i=0;i<TOTAL_HEARTS;i++)
	{
		if(!heartbeat_check[i])
		{
			sprintf(msg,"%s thread is dead",thread_names[i]);
			log_creator(LOG_ERROR,msg);
			bzero(msg,STR_SIZE);
		}
		count+=heartbeat_check[i];
	}
	if(count==TOTAL_HEARTS)
	{
		log_creator(LOG_INFO,"All threads are alive");
	}
	for(i=0;i<TOTAL_HEARTS;i++)
	{
		heartbeat_check[i]=0;
	}
}


static void join_threads(int sig, siginfo_t *si, void *uc)
{
	int32_t error = pthread_create(&thread_temperature,NULL,temperature_run,NULL);
	if(error)
	{
		log_creator(LOG_ERROR,"Error Creating Temperature Thread");
	}
	error = pthread_create(&thread_light,NULL,light_run,NULL);
	if(error)
	{	
		log_creator(LOG_ERROR,"Error Creating Light Thread");
	}
	log_creator(LOG_INFO,"Created new threads to measure temperature and luminosity. Joining threads");
	//pthread join
	error=pthread_join(thread_temperature,NULL);
	if(error)
	{			
		sprintf(msg,"Error Joining Temperature Thread %d",error);
		log_creator(LOG_ERROR,msg);
		bzero(msg,STR_SIZE);
	}
	error=pthread_join(thread_light,NULL);
	if(error)
	{	
		sprintf(msg,"Error Joining light Thread %d",error);
		log_creator(LOG_ERROR,msg);
		bzero(msg,STR_SIZE);
	}
	heartbeat();
}

/*************************
* Timer init
* Setting up the timer
***************************/
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

/**********************************
* system init
* To initialize the shared memory
***********************************/

int32_t system_init(void)
{
	int32_t error=0;
	condition=1;
	ptr=NULL;
	printf("System Init\n");
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT); 
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	sem_logger_ready=sem_open(logger_ready_id, O_CREAT, 0644,0);
	sem_open(shm_temp_id, O_CREAT, 0644,1);
	sem_open(led_sem_id, O_CREAT, 0644,1);
	sem_open(shm_light_id, O_CREAT, 0644,1);
	sem_open(logfile_sem_id, O_CREAT, 0644,1);
	sem_open(i2c_sem_id, O_CREAT, 0644,1);
	sem_trigger=sem_open(trigger_sem_id, O_CREAT, 0644,1);		
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
}

int32_t main(int32_t argc, uint8_t **argv)
{
	int32_t error=0;
	msg=(uint8_t*)malloc(STR_SIZE);
	ptr=&error;
	if(argc<2)
	{
		printf("%s <logfilename>\n",argv[0]);	 //log file name as command line argument
		return 0;
	}
	logfile=argv[1];		
	error=system_init();	
	//pthread creation logger
	error = pthread_create(&thread_logger,NULL,logger_run,NULL);
	if(error)
	{
		printf("Error Creating Logger Thread\n");
		kill(getpid(),SIGINT);
	}
	if(!fork())
	{
		error=pthread_join(thread_logger,NULL);
	}
	if(error)
	{
		printf("Error Joining logger Thread %d\n",error);
		kill(getpid(),SIGINT);
	}
	sem_wait(sem_logger_ready);	
	//error=bist();	
	//pthread creation server
	error=pthread_create(&thread_server,NULL,server_run,NULL);   //error checks
	if(error)
	{
		log_creator(LOG_ERROR,"Error Creating Server Thread");
	}
	if(!fork())
	{
		error=pthread_join(thread_server,NULL);
	}
	if(error)
	{	
		sprintf(msg,"Error Joining Server Thread %d",error);
		log_creator(LOG_ERROR,msg);
		bzero(msg,STR_SIZE);
	}
	while(condition)
	{
	
	}
	free(msg);
}
