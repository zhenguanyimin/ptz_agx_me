
#ifndef _RADAR_NETWORK_H_
#define _RADAR_NETWORK_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

int32_t radar_network_init(void);
int radar_network_release(void);

#ifdef __cplusplus
}
#endif

#endif
