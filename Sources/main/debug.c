#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "kernel.h"
#include "uart.h"
#include "debug.h"


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

