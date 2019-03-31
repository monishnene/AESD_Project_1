#include "common.h"
#include <sys/socket.h>
#include <netinet/in.h>

void remote_server(void)
{
	int sockfd, conn, fork_child=1, serverlen, data=10, received, input, send_data, temp=20;
	struct hostent* hostptr;
	struct sockaddr_in server_addr, client_addr;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
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
	puts("socket binding successfull\n");
	if(listen(sockfd,5)<0)
	{
		perror("socket listening error\n");
	}
	puts("socket listening successfull\n");
	while((condition)&&(fork_child))
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
		fork_child=fork();
		if(fork_child==0)
		{
			received=read(conn,&input,sizeof(input));
			printf("%d received data from client\n", received);
			if(received==sizeof(input))
			{
				printf("The data received by server from client is %d\n",input);
				if(input==2)
				{
					send_data=send(conn,(void*)&temp,sizeof(temp),0);
					printf("%d data sent to client\n", send_data);
				}
			}
		}
	}
	close(sockfd);
	printf("\n socket closed from server side\n");
	return;
}
