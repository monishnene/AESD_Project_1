#ifndef BBGLED_H
#define BBGLED_H 
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/gpio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

void led_toggle(uint8_t led);
void led_off(uint8_t led);
void led_on(uint8_t led);

#endif
