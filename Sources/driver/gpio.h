#ifndef __GPIO_H
#define __GPIO_H

enum GPIO_PORT
{
	PA=1,
	PB,
	PC,
	PD,
	PE,
	PF,
	PG,
};



void gpio_init(int port);
void gpio_open(int port, int pin, GPIOMode_TypeDef mode, int type);
void gpio_close(int port, int pin);
int  gpio_read(int port, int pin);
void gpio_write(int port, int pin, int value);

#endif
