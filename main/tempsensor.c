/******************************************
* tempsensor.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/25/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <time.h>

/*******************************************
* Macros
*******************************************/

#define slave_addr     (0x48)
#define tempregaddr    (00)
#define inicond        (0x60A0)
#define configregaddr  (0x01)
#define tlowregaddr    (0x02)
#define thighregaddr   (0x03)
#define highmask       (0x00FF)
#define interr         (0x62a0)
#define shutdown       (0x61a0)

/*******************************************
* Global variables
*******************************************/
int val;
int16_t celcius=0,kelvin=0,fahrenheit=0;


typedef enum  //error or success enum
{
	error=0,
	success=1
}error_check;


/***********************
*temp_file function
************************/

uint8_t temp_file_func()
{
	int output;
	output= open("/dev/i2c-2", O_RDWR);
	if(output<0)
	{	
		perror("file open failed\n");
		return error;
	}
	if(ioctl(output, I2C_SLAVE, slave_addr)<0)
	{
		return error;
	}
	return success;
}

/********************
* Write operation 
***********************/
uint8_t i2c_write(int32_t temp_fd,uint8_t regval)
{
	if(write(temp_fd, &regval, sizeof(regval))!=sizeof(regval))
	{
		return error;
	}
	return success;
}

uint8_t i2c_readword(int32_t temp_fd,uint8_t* buff)
{
	if(read(temp_fd, buff, 2)!=2)
	{
		return error;
	}
	return success;

}

uint8_t i2c_writeword(int32_t temp_fd, uint8_t* buff)
{
	if(write(temp_fd,buff, 3)!=3)
    	{
    		perror("write word failed\n");
        	return error; 
	}
	return SUCCESS;
}

uint16_t pointerreg(int temp_fd, uint16_t regval)
{
	uint8_t* buffer=malloc(sizeof(uint8_t)*2);
	uint8_t buffer1= 0x60;
	uint8_t buffer2 = 0xa0;
	uint8_t addr = configregaddr;
	if(regval==1)
	{
		perror("The tempreg is read only register\n");
	}
	if(regval==2)
	{
		uint8_t addr1= configregaddr;
		register_read(temp_fd,addr1);
	}
	if(regval==3)
	{
		uint8_t addr1= tlowregaddr;
		register_read(temp_fd,addr1);
	}
	if(regval==4)
	{
		uint8_t addr1=thighregaddr;
		//i2c_write(temp_fd,thighregaddr);
	}
	return success;
}	

float resolution_set_func(int temp_fd, int data_in)
{
	float data_resolution=0;
	pointerreg(temp_fd,2);
	uint8_t* buffer=malloc(sizeof(uint8_t)*2);
	if(data_in==1)
	{
		data_resolution = 0.0625;

	}
	if(data_in==2)
	{
		data_resolution=0.5;
	}
	printf("resolution set is %f\n",data_resolution);
	return data_resolution;
}

uint16_t* register_read(int temp_fd, uint8_t regval)
{
	uint8_t* buffer=malloc(sizeof(uint8_t)*2);
	write(temp_fd,&regval,sizeof(regval));
	i2c_readword(temp_fd,buffer);
	printf("1st data is %x\n", buffer[0]);
	printf("2nd data is %x\n", buffer[1]);
	return (buffer);
}

void register_write(int temp_fd, uint8_t regval, uint16_t* ptr)
{
	uint8_t* buffer=malloc(sizeof(uint8_t)*3);
	buffer[0]=regval;
	*((uint16_t*)&buffer[1])=*ptr;
	i2c_writeword(&buffer[0],temp_fd);
	free(buffer);
}
		
/***************************
* configuration reg write
****************************/
uint16_t configreg_test(int temp_fd)
{
	uint8_t addr = configregaddr;
	uint16_t* output;
	uint16_t data_config = inicond;
	register_write(temp_fd,addr,&data_config);
	output=register_read(temp_fd,addr);
	if(*output==60a0)
	{
		return success;
	}
	return error;
}

uint16_t configreg_interruptmode(int temp_fd)
{
	uint8_t addr = configregaddr;
	uint16_t* output;
	uint16_t data_config = inicond|interr;
	register_write(temp_fd,configregaddr,&data_config);
	output=register_read(temp_fd,addr);
	if(*output==62a0)
	{
		return success;
	}
	return error;
}

uint16_t configreg_shutdown(int temp_fd)
{
	uint8_t addr = configregaddr;
	uint16_t* output;
	uint16_t data_config = inicond|shutdown;
	register_write(temp_fd,addr,&data_config);
	output=register_read(temp_fd,addr);
	if(*output==61a0)
	{
		return success;
	}
	return error;
}

uint16_t tlowreg_write(int temp_fd)
{
	uint8_t addr = tlowregaddr;
	uint16_t opt = 72;
	uint16_t* output;
	register_write(temp_fd,addr,&opt);
	output= register_read(temp_fd,tlowregaddr);
	if(*output==72)
	{
		return success;
	}
	return error;
}

uint16_t thighreg_write(int temp_fd)
{
	uint8_t addr = thighregaddr;
	uint16_t opt=88;
	uint16_t* output;
	register_write(temp_fd,addr,&opt);
	output= register_read(temp_fd,addr);
	if(*output==88)
	{
		return success;
	}
	return error;
}

/********************************************************
* Get temperature function
* Reads tempreg
* and obtained temp in celsius is output/16
* return the temperature value
*****************************************************/	
	
int get_temperature(int temp_fd)
{
	int data, h_bit=0;
	uint8_t buffer[2], valmsb, vallsb;
	write_func(temp_fd, tempregaddr);
	val = read(temp_fd, &buffer, sizeof(buffer));
	valmsb = buffer[0];
	vallsb = buffer[1];
	data = ((valmsb << 8) | vallsb) >> 4; //12 bits resolution
	if(h_bit!=0)
	{	
		return data;
	}
	else 
	{
		data = data/16;
		return data;
	}
}
void main()
{
	uint16_t regval, buffer, op;
	uint16_t tester, tester1, tester2, tester3, tester4;
	int temp_fd;
	op = temp_file_func();
	if(op==0)
	{
		perror("init failed\n");
	}
	printf("temp sensor is ready\n");
	tester = configreg_test(temp_fd);
	if(tester==1)
	{
		printf("config reg test successful\n");
	}
	else
	{
		printf("config reg test failed\n");
	}
	tester1= configreg_interruptmode(temp_fd);
	if(tester1==1)
	{
		printf("config reg interrupt test successful\n");
	}
	else
	{
		printf("config reg interrupt test failed\n");
	}
	tester2=configreg_shutdown(temp_fd)
	if(tester2==1)
	{
		printf("config reg shutdown test successful\n");
	}
	else
	{
		printf("config reg shutdown test failed\n");
	}
	tester3=tlowreg_write(int temp_fd);
	if(tester3==1)
	{
		printf("tlow reg test successful\n");
	}
	else
	{
		printf("tlow reg test failed\n");
	}
	tester4=thighreg_write(temp_fd);
	if(tester4==1)
	{
		printf("thigh reg test successful\n");
	}
	else
	{
		printf("thigh reg test failed\n");
	}
	celcius = get_temperature(temp_fd);
	fahrenheit=celcius*1.8+32; // celcius to Fahrenheit
	kelvin=celcius+273.15; // celcius to kelvin
	printf("\ntemperature value in celcius is %d\n", celcius); //temp data in C
	printf("\ntemperature value in fahrenheit is %d\n", fahrenheit); // temp data in F
	printf("\ntemperature value in kelvin is %d\n", kelvin); // temp data in K
}
