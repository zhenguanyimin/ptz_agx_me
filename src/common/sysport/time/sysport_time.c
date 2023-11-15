
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <sys/time.h>
#include <time.h>

#include "../sysport.h"

int sysport_time_init( void )
{
	return 0;
}

uint64_t sysport_time_count( sysport_timespec_t *psTimespec )
{
	struct timespec time_count;

	clock_gettime(CLOCK_MONOTONIC, &time_count);

	psTimespec->tv_sec = time_count.tv_sec;
	psTimespec->tv_nsec = time_count.tv_nsec;

	return (1000*time_count.tv_sec) + (time_count.tv_nsec/1000000);
}

uint64_t sysport_time_get( sysport_time_t *psTime )
{
	time_t currentTime;
	struct tm *localTime;

    currentTime = time( NULL );

    localTime = localtime(&currentTime);

	psTime->tm_year = localTime->tm_year;
	psTime->tm_mon = localTime->tm_mon;
	psTime->tm_mday = localTime->tm_mday;
	psTime->tm_hour = localTime->tm_hour;
	psTime->tm_min = localTime->tm_min;
	psTime->tm_sec = localTime->tm_sec;

	return currentTime;
}


