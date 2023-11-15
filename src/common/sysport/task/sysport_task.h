
#ifndef __SYSPORT_TASK_H__
#define __SYSPORT_TASK_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

int sysport_task_init( void );

void* sysport_task_create( char *strName, void* pvArg );
int sysport_task_delete( void* pvModule );
void* sysport_task_malloc( void* pvModule, uint32_t uSize );
int sysport_task_func( void* pvModule, void (*pv_func)(void* pvArg, void* pvInfo, uint32_t uSize), void* pvData );

#endif /* __SYSPORT_TASK_H__ */

