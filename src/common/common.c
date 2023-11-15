
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "sysport/sysport.h"
#include "storage/storage.h"

void common_init( void )
{
	sysport_init();

	storage_init();
}

void common_uninit(void)
{
	return;
}
