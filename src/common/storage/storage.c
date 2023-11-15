
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "file/storage_file.h"
#include "../sysport/sysport.h"
#include "../../srv/log/skyfend_log.h"

static const char strFolder[] = "./storage";

int storage_init( void )
{
	int eRet = 0;

	if( 0 == access(strFolder, F_OK) )
	{
		skyfend_log_info("folder %s exist", strFolder );
	}
	else
	{
		if( 0 == mkdir(strFolder, 0777) )
		{
			skyfend_log_info( "create folder %s", strFolder );
		}
		else
		{
			skyfend_log_error( "create folder %s err %s", strFolder, strerror(errno) );
			eRet = -1;
		}
	}

	if( 0 <= eRet )
	{
		storage_file_init();
	}

	return eRet;
}

