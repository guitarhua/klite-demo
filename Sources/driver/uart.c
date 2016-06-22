/******************************************************************************
* STM32F4xx串口驱动
* USART1,2,3,4,5,6,7,8
* 蒋晓岗<kerndev@foxmail.com>
* 2016.6.12
******************************************************************************/

/*
*	stm32f10x串口驱动
* 	只实现串口1，2，3的非复用管脚
*   liudijiang<474147597@qq.com>
*   2016-06-22
*/
#include "kernel.h"
#include "stm32f10x.h"
#include "gpio.h"
#include "nvic.h"
#include "uart.h"

struct uart_contex
{
	USART_TypeDef* uart;
	char* buff;
	int   in;
	int   out;
	int   size;
};

#define MAX_UART_NUM	4

static struct uart_contex uart_contex[MAX_UART_NUM];

#define GET_UART_CTX(id)	(&uart_contex[id])

static __inline void uart_init_contex(int id, USART_TypeDef* uart, int buff_size)
{
	struct uart_contex* ctx;
	ctx = GET_UART_CTX(id);
	ctx->uart = uart;
	ctx->buff = malloc(buff_size);
	ctx->size = buff_size;
	ctx->in   = 0;
	ctx->out  = 0;
}

static __inline void uart_recv_data(int id)
{
	char data;
	struct uart_contex* ctx;
	ctx = GET_UART_CTX(id);
	if(USART_GetITStatus(ctx->uart, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(ctx->uart, USART_IT_RXNE);
		data = USART_ReceiveData(ctx->uart);
		ctx->buff[ctx->in++] = data;
		if(ctx->in == ctx->size)
		{
			ctx->in = 0;
		}
	}
	else
	{
		USART_ReceiveData(ctx->uart);
		USART_ClearFlag(ctx->uart, USART_FLAG_ORE);
	}
}

void uart_init(int id, int fifo_size)
{
	switch(id)
	{
	case 1:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
		gpio_open(PA,9,GPIO_Mode_AF_PP,0);
		gpio_open(PA,10,GPIO_Mode_IN_FLOATING,0);
		uart_init_contex(id,USART1,fifo_size);
		nvic_open(USART1_IRQn,0);
		break;
	case 2:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
		gpio_open(PA,2,GPIO_Mode_AF_PP,0);
		gpio_open(PA,3,GPIO_Mode_IN_FLOATING,0);
		uart_init_contex(id,USART2,fifo_size);
		nvic_open(USART2_IRQn,0);
		break;
	case 3:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
		gpio_open(PB,10,GPIO_Mode_AF_PP,0);
		gpio_open(PB,11,GPIO_Mode_IN_FLOATING,0);
		uart_init_contex(id,USART3,fifo_size);
		nvic_open(USART3_IRQn,0);
		break;
	default:
		break;
	}
}

void uart_open(int id, int baudrate, int parity)
{
	USART_InitTypeDef init;
	struct uart_contex* ctx;
	
	switch(parity)
	{
	case ODDPARITY:
		init.USART_Parity = USART_Parity_Odd;
		init.USART_WordLength = USART_WordLength_9b;
		break;
	case EVENPARITY:
		init.USART_Parity = USART_Parity_Even;
		init.USART_WordLength = USART_WordLength_9b;
		break;
	case NOPARITY:
	default:
		init.USART_Parity = USART_Parity_No;
		init.USART_WordLength = USART_WordLength_8b;
		break;
	}
	init.USART_BaudRate = baudrate;
	init.USART_StopBits = USART_StopBits_1;
	init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	ctx = GET_UART_CTX(id);
	USART_Init(ctx->uart, &init);
	USART_ITConfig(ctx->uart, USART_IT_RXNE, ENABLE);
	USART_Cmd(ctx->uart, ENABLE);
}

void uart_close(int id)
{
	struct uart_contex* ctx;
	ctx = GET_UART_CTX(id);
	USART_ITConfig(ctx->uart, USART_IT_RXNE, DISABLE);
	USART_Cmd(ctx->uart, DISABLE);
}

void uart_write(int id, void* buf, int len)
{
	int i;
	char* pdata;
	struct uart_contex* ctx;
	pdata = buf;
	ctx = GET_UART_CTX(id);
	for(i=0;i<len;i++)
	{
		while(!USART_GetFlagStatus(ctx->uart, USART_FLAG_TXE));
		USART_SendData(ctx->uart, *pdata++);
	}
}

int uart_read(int id, void* buf, int len)
{
	int i;
	int ret;
	char* pdata;
	struct uart_contex* ctx;
	pdata = buf;
	ctx = GET_UART_CTX(id);
	if(ctx->in >= ctx->out)
	{
		ret = ctx->in - ctx->out;
	}
	else
	{
		ret = ctx->size + ctx->in - ctx->out;
	}
	ret = (len<ret)?len:ret;
	for(i=0; i<ret; i++)
	{
		*pdata++ = ctx->buff[ctx->out++];
		if(ctx->out == ctx->size)
		{
			ctx->out = 0;
		}
	}
	return ret;
}


void uart_purge(int id)
{
	struct uart_contex* ctx;
	ctx = GET_UART_CTX(id);
	ctx->in  = 0;
	ctx->out = 0;
}


//UART接收中断处理
void USART1_IRQHandler(void)
{
	uart_recv_data(1);
}


void USART2_IRQHandler(void)
{
	uart_recv_data(2);
}


void USART3_IRQHandler(void)
{
	uart_recv_data(3);
}


void UART4_IRQHandler(void)
{
	uart_recv_data(4);
}


void UART5_IRQHandler(void)
{
	uart_recv_data(5);
}

