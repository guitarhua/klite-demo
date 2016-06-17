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
#include "debug.h"


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
	kthread_create(demo_main,0,0);//故意创建两个同样的线程
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
