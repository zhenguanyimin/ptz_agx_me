
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <dirent.h>
#include <mqueue.h>
#include <unistd.h>
#include "sys/types.h"
#include <sys/stat.h>
#include <pthread.h>

#include "../storage.h"
#include "../../sysport/sysport.h"
#include "../../../srv/log/skyfend_log.h"

#define STORAGE_FILE_NAME_LEN_MAX	(256)

static const char strFileFolder[] = "./log/";

typedef struct storage_file_module
{
	pthread_mutex_t 	mutex;
}storage_file_module_t;

static struct storage_file_module sFile = { 0 };

static void storage_file_calculate_name( char* strFileName, const char* strName )
{
	memcpy( strFileName, strFileFolder, sizeof(strFileFolder) );
	for( ; '/' == *strName; ++strName );
	strcpy( strFileName + sizeof(strFileFolder) - 1, strName );
}

int storage_file_folder_create( void )
{
	int eRet = 0;
	
	if( 0 == access(strFileFolder, F_OK) )
	{
		skyfend_log_info( "folder %s exist", strFileFolder );
	}
	else
	{
		if( 0 == mkdir(strFileFolder, 0777) )
		{
			skyfend_log_info( "create folder %s", strFileFolder );
		}
		else
		{
			skyfend_log_error( "create folder %s err %s", strFileFolder, strerror(errno) );
			eRet = -1;
		}
	}

	return eRet;
}

int storage_file_init( void )
{	
	int eRet;

	eRet = storage_file_folder_create();

	if( 0 <= eRet )
	{
		eRet = pthread_mutex_init( &(sFile.mutex), NULL );
		if( 0 > eRet )
		{
			skyfend_log_error( "create mutex err %s", strerror(errno) );
		}
		else
		{
			skyfend_log_info( "create mutex" );
		}
	}

	return eRet;
}

int storage_file_write( const char* strName, void* pvData, uint32_t uLen )
{
	int ret = 0;

	if( strName && pvData )
	{
		char strFileName[STORAGE_FILE_NAME_LEN_MAX];
		storage_file_calculate_name( strFileName, strName );

		pthread_mutex_lock( &(sFile.mutex) );
		
		int fd = open( strFileName, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
		if(fd < 0)
		{
			skyfend_log_error( "write open file %s err %s", strFileName, strerror(errno) );
			ret = -1;
		}
		else
		{
			if( 0 > write( fd, pvData, uLen ) )
			{
				skyfend_log_error( "write file %s err %s", strFileName, strerror(errno) );
				ret = -1;
			}
			
			close(fd);
		}

		pthread_mutex_unlock( &(sFile.mutex) );
	}
	
	return ret;
}

int storage_file_read( const char* strName, void* pvBuffer, uint32_t uSize )
{
	int len = 0;

	if( strName && pvBuffer )
	{
		char strFileName[STORAGE_FILE_NAME_LEN_MAX];
		storage_file_calculate_name( strFileName, strName );
		
		int fd = open( strFileName, O_RDONLY, 0666 );
		if(fd < 0)
		{
			skyfend_log_error( "read open file %s err %s", strFileName, strerror(errno) );
		}
		else
		{
			len = read( fd, pvBuffer, uSize );
			if( 0 > len )
			{
				skyfend_log_error( "read file %s err %s", strFileName, strerror(errno) );
				len = 0;
			}
			
			close(fd);
		}
	}
	
	return len;
}

int storage_file_del( const char* strName )
{
	int ret = -1;

	if( strName )
	{
		char strFileName[STORAGE_FILE_NAME_LEN_MAX];
		storage_file_calculate_name( strFileName, strName );
		
		ret = unlink( strFileName );
		if( 0 > ret )
		{
			skyfend_log_error( "del file %s err %s", strFileName, strerror(errno) );
			ret = -1;
		}
	}
	
	return ret;
}
