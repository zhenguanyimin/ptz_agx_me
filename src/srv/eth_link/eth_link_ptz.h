#ifndef __ETH_LINK_PTZ_H__
#define __ETH_LINK_PTZ_H__

#include "ptz_protocol/ptz_control_protocol.h"
#include "ptz_protocol/ptz_control.h"

typedef int32_t (*ptz_comand_handle)(ptz_protocol_msg_t *msg);

typedef struct msg_handle_info
{
    uint32_t msg_id;
    ptz_comand_handle cbk;
} msg_handle_info_t;

int eth_ptz_command_register(uint32_t msgid, ptz_comand_handle cb);
int eth_link_ptz_init(void);
int eth_link_ptz_uninit(void);
int send_ptz_package(ptz_protocol_msg_t *msginfo);

#endif
