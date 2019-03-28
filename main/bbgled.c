/*********************
*LED task
*Author: Sanika Dongre
*Date created: 03/25/19
***********************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/gpio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#define  STR_SIZE 30

void led_on(uint8_t led)
{
	uint8_t* str=malloc(STR_SIZE);
	sprintf(str,"/sys/class/gpio/gpio%d/value",led);
	FILE* fptr=fopen(str,"w");
	uint8_t data='1',error=0;
	error=fwrite(&data,1,1,fptr);
	free(str);
	fclose(fptr);
}

void led_off(uint8_t led)
{
	uint8_t* str=malloc(STR_SIZE);
	sprintf(str,"/sys/class/gpio/gpio%d/value",led);
	FILE* fptr=fopen(str,"w");
	uint8_t data='0',error=0;
	error=fwrite(&data,1,1,fptr);
	free(str);
	fclose(fptr);
}

void led_toggle(uint8_t led)
{
	uint8_t* str=malloc(STR_SIZE);
	sprintf(str,"/sys/class/gpio/gpio%d/value",led);
	FILE* fptr=fopen(str,"w+");
	uint8_t data='1',error=0,prev=0;
	error=fread(&prev,1,1,fptr);
	data=(prev=='0')?'1':'0';
	error=fwrite(&data,1,1,fptr);
	free(str);
	fclose(fptr);
}

void main()
{
	uint8_t led=53,i=0;
	led_off(53);	
	led_off(54);		
	led_off(55);		
	led_off(56);
	for(i=0;i<255;i++)
	{
		usleep(1e5);
		led_toggle(led);		
		led=(led==56)?53:(led+1);
	}
}
