
#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define X25_INIT_CRC 0xffff
#define X25_VALIDATE_CRC 0xf0b8

void alink_crc_init(uint16_t *crcAccum);
void crc_accumulate(uint8_t data, uint16_t *crcAccum);
uint16_t crc_calculate(const uint8_t *pBuffer, uint16_t length);
void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* _CHECKSUM_H_ */
