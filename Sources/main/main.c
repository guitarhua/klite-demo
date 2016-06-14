/*
* KLite示例工程
* 基于STM32F429,XTAL=16MHz
*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "board.h"
#include "nvic.h"
#include "uart.h"

#define MAX_DEBUG_LEN	1024
static kmutex_t debug_mutex;
static char*    debug_buff;

void debug(const char* fmt, ...)
{
	va_list va;
	kmutex_lock(debug_mutex);
	va_start(va,fmt);
	vsnprintf(debug_buff,MAX_DEBUG_LEN,fmt,va);
	va_end(va);
	uart_write(1,debug_buff,strlen(debug_buff));
	kmutex_unlock(debug_mutex);
}

void debug_init(void)
{
	debug_buff = malloc(MAX_DEBUG_LEN);
	debug_mutex = kmutex_create();
	uart_init(1,1024);
	uart_open(1,115200,0);
}

void demo_main(void* arg)
{
	debug("demo thread start.\r\n");
	debug("thread pid=%p, arg=%p\r\n",kthread_self(),arg);
	while(1)
	{
		sleep(2000);
		debug("thread<%p> running.kernel time=%d\r\n",kthread_self(),kernel_time());
	}
}

void demo_init(void)
{
	uint32_t ver;
	uint32_t total;
	uint32_t used;
	ver = kernel_version();
	debug("\r\nklite version:%d.%d.%d\r\n",(ver>>24)&0xFF,(ver>>16)&0xFF,ver&0xFFFF);
	kthread_create(demo_main,0,0);
	kthread_create(demo_main,0,0);
	kmem_info(&total,&used);
	debug("memory usage: %d/%d byte\r\n",used,total);
}

void init(void* arg)
{
	board_init();
	nvic_init();
	debug_init();
	demo_init();
	debug("init thread exit!\r\n");
}

int main(void)
{
	//使用CCM内存
	kernel_init(0x10000000,0x10000);
	kthread_create(init,0,0);
	kernel_start();
}
