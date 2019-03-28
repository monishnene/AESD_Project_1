/*********************
*LED task
*Author: Sanika Dongre
*Date created: 03/25/19
***********************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define LED0_LOC "/sys/class/leds/beaglebone:green:usr0/brightness"
#define LED0_INIT_LOC "/sys/class/leds/beaglebone:green:usr0/trigger"

void init_led(void)
{
	FILE* fdptr = fopen(LED0_INIT_LOC, "w+");
	if(fdptr==NULL)
	{
		perror("error in file open\n");		
		exit(-1);
	}
	else
	{
		fprintf(fdptr,"none");
		fclose(fdptr);
		
	}
}

void on_led(void)
{
	FILE *fdptr = fopen(LED0_LOC, "w+");
	if(fdptr==NULL)
	{
		perror("error in file open\n");		
		exit(-1);
	}
	else
	{
		fputc('1',fdptr);
		fclose(fdptr);
	}
	
}

void off_led(void)
{
	FILE* fdptr = fopen(LED0_LOC, "w+");
	if(fdptr==NULL)
	{
		perror("error in file open\n");
		exit(-1);
	}
	else
	{
		fputc('0', fdptr);
		fclose(fdptr);
		
	}
	
}

void main()
{
	init_led();
	on_led();
	usleep(500);
	off_led();
	usleep(500);
	on_led();
}
