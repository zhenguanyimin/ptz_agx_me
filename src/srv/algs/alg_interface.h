#ifndef __ALG_INTERFACE_H__
#define __ALG_INTERFACE_H__

#include "../alink/command/track/alink_track.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef int (*multiple_targets_algs_handle)(void *pack, void* arg);

int msg_send_to_alg(void *pack, int pack_len);
int load_multiple_targets_algs(multiple_targets_algs_handle cbk);
int unload_multiple_targets_algs(void);

#ifdef __cplusplus
}
#endif

#endif
