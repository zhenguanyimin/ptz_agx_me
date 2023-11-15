
#ifndef __SYSPORT_TIME_H__
#define __SYSPORT_TIME_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

int sysport_time_init( void );

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

#endif /* __SYSPORT_TIME_H__ */

