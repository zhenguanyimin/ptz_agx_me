
#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <syslog.h>

int storage_init( void );

/* log */
#define LOG_LEVEL_EMERG		(LOG_EMERG)		//紧急情况
#define LOG_LEVEL_ALERT		(LOG_ALERT)		//高优先级故障，例如数据库崩溃
#define LOG_LEVEL_CRIT		(LOG_CRIT)		//严重错误，例如硬件故障
#define LOG_LEVEL_ERR		(LOG_ERR)		//错误
#define LOG_LEVEL_WARNING	(LOG_WARNING)	//警告
#define LOG_LEVEL_NOTICE	(LOG_NOTICE)	//需要注意的特殊情况
#define LOG_LEVEL_INFO		(LOG_INFO)		//一般信息
#define LOG_LEVEL_DEBUG		(LOG_DEBUG)		//调试信息

void storage_log( const char* strName, int eLevel, const char* strFormat, ... );

/* file */
int storage_file_write( const char* strName, void* pvData, uint32_t uLen );
int storage_file_read( const char* strName, void* pvBuffer, uint32_t uSize );
int storage_file_del( const char* strName );

/* record */

int storage_record_printf( const char* strName, const char* strFormat, ... );
int storage_record( const char* strName, void* pvRecord, uint32_t uLen );

#endif /* __STORAGE_H__ */

