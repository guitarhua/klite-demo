/*
* STM32F4xx GPIO驱动
* 蒋晓岗<kerndev@foxmail.com>
* 2016.6.12
*/

/*
* stm32f103 GPIO驱动
* liudijiang<474147597@qq.com>
* 2016-06-22
* stm32f1 系列的GPIO 复用功能设置跟f2，f4系列不一样，要指定哪种重映射
*/
#include "stm32f10x.h"
#include "gpio.h"

#define GET_GPIO_CTX(x)	((GPIO_TypeDef*)(APB2PERIPH_BASE + ((x)<<10)))

void gpio_open(int port, int pin, GPIOMode_TypeDef mode, int type)
{
	GPIO_InitTypeDef io;
	
    io.GPIO_Mode  = mode;
	io.GPIO_Speed = GPIO_Speed_50MHz;
	io.GPIO_Pin   = 1<<pin;
	GPIO_Init(GET_GPIO_CTX(port),&io);
	if(mode==GPIO_Mode_AF_OD||mode==GPIO_Mode_AF_PP)
		GPIO_PinRemapConfig(type,ENABLE);
}

void gpio_close(int port, int pin)
{
	GPIO_InitTypeDef io;
	
    io.GPIO_Mode  = GPIO_Mode_IPD;
	io.GPIO_Speed = GPIO_Speed_10MHz;
	io.GPIO_Pin   = 1<<pin;
	GPIO_Init(GET_GPIO_CTX(port),&io);
}

int gpio_read(int port, int pin)
{
	return (((GET_GPIO_CTX(port)->IDR)>>pin)&0x01);
}

void gpio_write(int port, int pin, int value)
{
	if(value)
		GET_GPIO_CTX(port)->BSRR = (1<<pin);
	else
		GET_GPIO_CTX(port)->BSRR = (1<<(pin+15));
}

void gpio_init(int port)
{
	RCC_APB2PeriphClockCmd(1<<port, ENABLE);
}
