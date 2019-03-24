#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <time.h>
#include <common.h>

int8_t celcius=0,kelvin=0,fahrenheit=0;
	
int16_t get_temperature()
{
	srand(time);
	return ((rand()%128)-100);
}

void main(void)
{	
	celcius = get_temperature();
	fahrernheit=celcius*1.8+32; // celcuis to Fahrenheit
	kelvin=celcius+273.15; // celcius to kelvin
}

