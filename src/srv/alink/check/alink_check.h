
#ifndef _ALINK_CHECK_H_
#define _ALINK_CHECK_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../alink_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ALINK_MSG_MAGIC_VALUE 		(0XFD)
#define ALINK_NO_PAYLOAD_SIZE		(11)
#define ALINK_HEADER_CHECKSUM_LEN 	(8)

// #define ALINK_CHECK_PIL_SPECAIL

uint8_t alink_get_header_size( void );

uint16_t alink_get_payload_size( alink_msg_head_t *psHeader );

uint32_t alink_get_package_size( alink_msg_head_t *psHeader );

uint8_t alink_get_header_checksum( alink_msg_head_t *psHeader );

uint16_t alink_get_package_crc( uint8_t *pbyPkg, uint16_t wPkgSize );

bool alink_check_magic( uint8_t x );

bool alink_check_header_checksum( alink_msg_head_t *psHeader );

bool alink_check_crc( uint8_t *pbyPkg, uint32_t wLen );

bool alink_check_package( uint8_t *pbyPkg, uint16_t wLen );

#ifdef __cplusplus
}
#endif

#endif
