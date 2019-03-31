#include "server.h"
int32_t server_temp=0,server_light=0;
sem_t* sem_temp;
sem_t* sem_light;
uint8_t* shm_ptr;

static int16_t find_temperature (void)
{
	int32_t error=0;
	log_t temp_data;
	//join thread  here temperature
      	sem_wait(sem_temp);
	shm_ptr=shmat(server_temp,(void*)0,0);
	//printf("Shared Memory Access\n");
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	printf("temperature = %d\n",temp_data.data[celcius_id]);
	return temp_data.data[celcius_id]; 
}

static int16_t find_luminosity(void)
{
	int32_t error=0;
	log_t light_data;
	//join thread  here light
      	sem_wait(sem_light);
	shm_ptr=shmat(server_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
	printf("luminosity = %d\n",light_data.data[luminosity_id]);
	return light_data.data[luminosity_id];
}

void remote_server(void)
{
	int32_t sockfd=0,conn=0, fork_child=1, serverlen=0, size=0, temp=0, light=0;
	struct hostent* hostptr;
	struct sockaddr_in server_addr, client_addr;
	sockfd = socket(AF_INET, SOCK_STREAM,0);	
	server_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT); 
	server_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	sem_temp = sem_open(shm_temp_id,0);
	sem_light = sem_open(shm_light_id,0);
	if(sockfd < 0)
	{
		perror("socket creation error\n");
	}
	puts("socket created successfull\n");
	memset((char*)&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port= htons(PORT_ADDRESS);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverlen = sizeof(struct sockaddr_in);
	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
	{
		perror("socket binding error\n");
	}
	printf("socket binding successful\n");
	if(listen(sockfd,5)<0)
	{
		perror("socket listening error\n");
	}
	printf("socket listening successful\n");
	while(condition)
	{
		conn=accept(sockfd,(struct sockaddr*)&client_addr,&serverlen);
		if(conn<0)
		{
			perror("connection accept failed\n");
		}
		else
		{
			printf("connection accepted\n");
		}
		size=recv(conn,&size,sizeof(size),0);
		if(!fork())
		{
			temp=find_temperature();
			light=find_luminosity();
			size=write(conn,&light,sizeof(light));
			size=write(conn,&temp,sizeof(temp));
			printf("Data sent to client\n");
			break;
		}
	}
	close(sockfd);
	return;
}
