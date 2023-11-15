
#ifndef _ALINK_PACKAGE_H_
#define _ALINK_PACKAGE_H_

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

int32_t alink_package_init( alink_package_list_t *psPackage, uint8_t byMsgId, uint8_t byAns, 
							uint16_t (*pw_package_task)(uint8_t* pbyBuffer, uint16_t wSize, void *psImport) );

int32_t alink_package_connect_send( alink_package_list_t *psPackage, alink_send_list_t *psSendList );

int32_t alink_package_disconnect_send( alink_package_list_t *psPackage, alink_send_list_t *psSendList );

#ifdef __cplusplus
}
#endif

#endif
