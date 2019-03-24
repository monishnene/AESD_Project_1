#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <time.h>

typedef enum
{
	light,
	temperature,
	remote,
}header_t;

typedef struct
{
	header_t header;
	uint32_t size;
	uint8_t* data;  
}log_t,
