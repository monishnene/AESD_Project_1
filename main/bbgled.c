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

#define INIT_LOC "/sys/class/gpio/export"
#define FILE_DIR "/sys/class/gpio/gpio%d/direction"
#define FILE_VAL "/sys/class/gpio/gpio%d/value"

int low=0, high=1;
void pin_init_func(int dir,int pinno)
{
	FILE* fdptr;
	char* name= (char*)malloc(100*sizeof(char));
	int i=0;
	int acc_array[5] = {53,54,55,56,60};
	for(i=0;i<5;i++)
	{
		if(pinno==acc_array[i])
		{
			fdptr=fopen(INIT_LOC,"w");
			if(fdptr!=NULL)
			{
				fprintf(fdptr,"%d",pinno);
				fclose(fdptr);
				sprintf(name, FILE_DIR, pinno);
				fdptr= fopen(name,"w");
				if(fdptr!=NULL)
				{
					if(dir==0)
					{
						fprintf(fdptr, "in");
					}
					else if(dir==1)
					{
						fprintf(fdptr, "out");
					}
					else
					{
						printf("enter a proper direction - out or in\n");
					}
					fclose(fdptr);
				}
				else
				{
					perror("file opening error\n");
				}
			}
			else
			{
				perror("file opening error\n");
			}
		}
		else
		{
			printf("The pin number is not accessible\n");
		}
	}
	free(name);
}

void pin_write_func(int cond, int pinno)
{
	FILE *fdptr;
	char* name= (char*)malloc(100*sizeof(char));
	int i=0;
	int acc_array[5] = {53,54,55,56,60};
	for(i=0;i<5;i++)
	{
		if(pinno==acc_array[i])
		{
			sprintf(name,FILE_VAL,pinno);
			fdptr=fopen(name,"w");
			if(fdptr!=NULL)
			{
				if(cond==0)
				{
					fprintf(fdptr,"%d",low);
				}
				else if(cond==1)
				{
					fprintf(fdptr,"%d",high);
				}
				else
				{
					printf("enter a proper condition - 1 (high) or 0 (low)\n");
				}
				fclose(fdptr);
			}
			else
			{
				perror("file opening error\n");
			}
		}
		else
		{
			printf("The pin number is not accessible\n");
		}
	}
	free(name);
}

int pin_read_func(int pinno)
{
	FILE* fdptr;
	char* name = (char*)malloc(100*sizeof(char));
	int cond; 
	int i=0;
	int acc_array[5] = {53,54,55,56,60};
	for(i=0;i<5;i++)
	{
		if(pinno==acc_array[i])
		{
			sprintf(name,FILE_VAL,pinno);
			fdptr=fopen(name,"r");
			if(fdptr!=NULL)
			{
				fscanf(fdptr,"%d",&cond);
				fclose(fdptr);
			}
			else
			{
				perror("file opening error\n");
			}
		}
		else
		{
			printf("The pin number is not accessible\n");
		}
	}
	free(name);
	return cond;	
}

void main()
{
	int cond, pinno;
	pinno=53;
	pin_init_func(0,pinno);
	pin_write_func(1,pinno);
	cond = pin_read_func(pinno);
	printf("%d is The value of GPIO\n",cond);
	usleep(500);
	pin_init_func(1,pinno);
	pin_write_func(0,pinno);
	cond = pin_read_func(pinno);
	printf("%d is The value of GPIO for %d GPIO PIN\n",cond, pinno);
	pinno=54;
	pin_init_func(1,pinno);
	pin_write_func(0,pinno);
	cond = pin_read_func(pinno);
	printf("%d is The value of GPIO for %d GPIO PIN\n",cond, pinno);
}
