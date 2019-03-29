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
#define STR_SIZE 30
#define	LED_COUNT 4

uint8_t *path[]={"/sys/class/gpio/gpio53/value","/sys/class/gpio/gpio54/value","/sys/class/gpio/gpio55/value","/sys/class/gpio/gpio56/value"};

void led_on(uint8_t led)
{
	if(led<LED_COUNT)
	{
		FILE* fptr=fopen(path[led],"w");
		uint8_t data='1',error=0;
		error=fwrite(&data,1,1,fptr);
		fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	return;
}

void led_off(uint8_t led)
{
	if(led<LED_COUNT)
	{
		FILE* fptr=fopen(path[led],"w");
		uint8_t data='0',error=0;
		error=fwrite(&data,1,1,fptr);
		fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	return;
}

void led_toggle(uint8_t led)
{
	printf("LED%d Toggle\n",led);
	if(led<LED_COUNT)
	{
		FILE* fptr=fopen(path[led],"w+");
		uint8_t data='1',error=0,prev=0;
		error=fread(&prev,1,1,fptr);
		data=(prev=='0')?'1':'0';
		error=fwrite(&data,1,1,fptr);
		fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	return;
}

#ifdef LED_TEST
void main()
{
	uint8_t led=0,i=0;
	led_off(0);	
	led_off(1);		
	led_off(2);		
	led_off(3);
	for(i=0;;i++)
	{
		usleep(1e5);
		led_toggle(led);		
		led=(led==LED_COUNT-1)?0:(led+1);
	}
}
#endif
