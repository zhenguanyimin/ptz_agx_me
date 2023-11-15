
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "task/sysport_task.h"
#include "timer/sysport_timer.h"
#include "time/sysport_time.h"

int sysport_init( void )
{
	sysport_task_init();
	sysport_timer_init();
	sysport_time_init();

	return 0;
}
