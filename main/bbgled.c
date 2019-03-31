/*********************
*LED task
*Author: Sanika Dongre
*Date created: 03/25/19
***********************/
#include "bbgled.h"

sem_t* sem_led;

void led_init(void)
{	
	printf("LED Init\n");
	sem_led = sem_open(led_sem_id,0);
	system(led_init_cmd);
}

void led_on(uint8_t led)
{
	sem_wait(sem_led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w");
		uint8_t data='1',error=0;
		error=fwrite(&data,1,1,fptr);
		//fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	sem_post(sem_led);
	return;
}

void led_off(uint8_t led)
{
	sem_wait(sem_led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w");
		uint8_t data='0',error=0;
		error=fwrite(&data,1,1,fptr);
		//fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	sem_post(sem_led);
	return;
}

void led_toggle(uint8_t led)
{
	FILE* fptr;
	//printf("LED%d Toggle\n",led);
	sem_wait(sem_led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		fptr=fopen(path,"w+");
		if(fptr==NULL)
		{
			printf("file not found for %s\n",path);
		}
		uint8_t data='1',error=0,prev=0;
		error=fread(&prev,1,1,fptr);
		data=(prev=='0')?'1':'0';
		error=fwrite(&data,1,1,fptr);
		//fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	sem_post(sem_led);
	//printf("LED%d Toggle Done\n",led);
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
