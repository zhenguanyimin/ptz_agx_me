
#ifndef __STORAGE_FILE_H__
#define __STORAGE_FILE_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

int storage_file_init( void );

int storage_file_write( const char* strName, void* pvData, uint32_t uLen );
int storage_file_read( const char* strName, void* pvBuffer, uint32_t uSize );
int storage_file_del( const char* strName );

#endif /* __STORAGE_FILE_H__ */

