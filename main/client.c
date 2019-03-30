#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#define IP_ADDR "10.0.0.152"

int main(void)
{
	int sockfd, operation, send_data, received;
	float data;
	struct sockaddr_in server_addr;
	struct hostent* hostptr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("socket creation failed\n");
	}
	puts("socket creation successfull\n");
	memset((char*)&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port = htons(10001);
	hostptr=gethostbyname(IP_ADDR);
	memcpy(&server_addr.sin_addr,hostptr->h_addr,hostptr->h_length);
	if((connect(sockfd,(struct  sockaddr*)&server_addr, sizeof(server_addr)))<0)
	{
		printf("server is not connection ready\n");
		exit(-1);
	}
	while(1)
	{
		printf("\n Enter the operation to be performed: 1) Get lux 2) Get temp\n");
		scanf("%d",&operation);
		send(sockfd,(void*)&data,sizeof(data)+1,0);
		received = read(sockfd,&data,sizeof(data));
		if(received==sizeof(data))
		{
			printf("The received data is %f\n", data);
			if(data<0)
			{
				switch(operation)
				{
					case 1:
					printf("Lux value is %f\n",data);
					break;
					
					case 2:
					printf("Temperature value is %f\n", data);
					break;
					
					default:
					printf("The operation is invalid\n");
					break;
				}
			}
			else
			{
				printf("data read from the server is faulty\n");
			}
		 }
		
	  }
	  return 0;
}
	
