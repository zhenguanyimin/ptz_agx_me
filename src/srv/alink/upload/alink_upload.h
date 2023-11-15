
#ifndef ALINK_UPLOAD_H
#define ALINK_UPLOAD_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../alink_defines.h"

int32_t alink_upload_init( alink_upload_t *psUpload, uint8_t byDevId, uint8_t *pbyBuffer, uint16_t wBufferSize );

int32_t alink_upload_send_package( alink_upload_t *psUpload, alink_package_list_t *psPackage, void *psImport );

int32_t alink_upload_register_package(         alink_upload_t *psUpload, alink_package_list_t *psPackageList );

int32_t alink_upload_connect_send( alink_upload_t *psUpload, uint8_t byMsgId, alink_send_list_t *psSendList );

int32_t alink_upload_disconnect_send( alink_upload_t *psUpload,         uint8_t byMsgId, alink_send_list_t *psSendList );

#endif
