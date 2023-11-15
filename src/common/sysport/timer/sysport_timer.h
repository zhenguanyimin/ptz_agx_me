
#ifndef __SYSPORT_TIMER_H__
#define __SYSPORT_TIMER_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

int sysport_timer_init( void );

void* sysport_timer_create( char *strName, uint32_t uTimeMs, uint32_t uRunTime, void* pvArg, void (*pv_func)(void* pvArg) );
int sysport_timer_delete( void* pvModule );

#endif /* __SYSPORT_TIMER_H__ */

