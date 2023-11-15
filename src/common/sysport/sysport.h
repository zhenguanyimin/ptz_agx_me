
#ifndef __SYSPORT_H__
#define __SYSPORT_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* sysport */
int sysport_init( void );

/* task */
void* sysport_task_create( char *strName, void* pvArg );
int sysport_task_delete( void* pvModule );
void* sysport_task_malloc( void* pvModule, uint32_t uSize );
int sysport_task_func( void* pvModule, void (*pv_func)(void* pvArg, void* pvInfo, uint32_t uSize), void* pvData );

/* timer */
void* sysport_timer_create( char *strName, uint32_t uTimeMs, uint32_t uRunTime, void* pvArg, void (*pv_func)(void* pvArg) );
int sysport_timer_delete( void* pvModule );

/* time */
typedef struct sysport_timespec
{
    long long tv_sec;	// 秒
    long long tv_nsec;	// 纳秒
}sysport_timespec_t;

typedef struct sysport_time
{
    int tm_sec;     // 秒，取值范围为0-59
    int tm_min;     // 分钟，取值范围为0-59
    int tm_hour;    // 小时，取值范围为0-23
    int tm_mday;    // 一个月中的日期，取值范围为1-31
    int tm_mon;     // 月份，取值范围为0-11，表示实际月份减1
    int tm_year;    // 年份，表示实际年份减1900
    int tm_wday;    // 一周中的星期，取值范围为0-6，0表示星期日
    int tm_yday;    // 一年中的天数，取值范围为0-365，0表示1月1日
    int tm_isdst;   // 夏令时标志，正数表示夏令时，0表示不是夏令时，负数表示夏令时信息不可用
}sysport_time_t;
uint64_t sysport_time_count( struct sysport_timespec* psTimespec );
uint64_t sysport_time_get( struct sysport_time* psTime );

/* info version */
const char *sysport_version( void );
const char *sysport_version_ps( void );
const char *sysport_version_pl( void );
const char *sysport_version_alg_detect( void );
const char *sysport_version_alg_track( void );
const char *sysport_version_hw_code( void );
const char *sysport_version_hw_source( void );

/* info sn */
const char *sysport_sn( void );
const char *sysport_sn_hardware( void );
const char *sysport_sn_source( void );

#endif /* __SYSPORT_H__ */

