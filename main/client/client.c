/******************************************
* client.c
* Remote socket task
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/


/*******************************************
* Includes
*******************************************/

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

/*******************************************
* Macros
*******************************************/

#define IP_ADDR "10.0.0.152" //ip addr
#define PORT_ADDRESS 12048

int main(void)
{
	int sockfd, operation, send_data, received, data, acc_conn, data_f;
	struct sockaddr_in server_addr;
	struct hostent* hostptr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	//socket create
	if(sockfd<0)
	{
		perror("socket creation failed\n");
	}
	puts("socket creation successful\n");
	memset((char*)&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port = htons(PORT_ADDRESS);
	hostptr=gethostbyname(IP_ADDR);
	memcpy(&server_addr.sin_addr,hostptr->h_addr,hostptr->h_length);
	// connection accept
	acc_conn = connect(sockfd,(struct  sockaddr*)&server_addr, sizeof(server_addr));
	if((acc_conn)<0)
	{
		printf("server is not connection ready\n");
		exit(-1);
	}
	received = write(sockfd,&data,sizeof(data));
	received = read(sockfd,&data,sizeof(data)); // receive lux data
	if(data>-10)
	{
		printf("Light=%d\n",data);  // print lux data
	}
	else
	{
		printf("LUX Error\n"); // error
	}
	received = read(sockfd,&data,sizeof(data)); //recieve temp data
	if(data<1e5)
	{
		printf("Temperature: %d°C %d°F %d°K\n",data,((data*9)/5)+32,data+273); // print temperature data 
	}		
	else	
	{	
		printf("Temperature Error\n"); // error
	}
	//close socket
	close(sockfd);
	return 0;
}
