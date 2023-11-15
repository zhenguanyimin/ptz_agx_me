#ifndef __PTZ_CONTROL_PROTOCOL_H__
#define __PTZ_CONTROL_PROTOCOL_H__

#include "../../log/skyfend_log.h"

#define PTZ_PACKAGE_HEAD    0x8A808988
#define PTZ_PACKAGE_PROTOCOL_VERSION  9002
#define PTZ_PACKAGE_STOPBIT 0x8B8A8089

#define PTZ_PACKAGE_HEAD_LEN    4
#define PTZ_PACKAGE_PROTOCOL_LEN    4
#define PTZ_PACKAGE_LENGTH_LEN    4
#define PTZ_PACKAGE_COMMAND_LEN     4
#define PTZ_PACKAGE_TIMESMAP_LEN    8
#define PTZ_PACKAGE_SEQ_MSG_LEN     4
#define PTZ_PACKAGE_CRC_LEN     4
#define PTZ_PACKAGE_STOPBIT_LEN     4
#define PTZ_PACKAGE_MINI_LEN (PTZ_PACKAGE_HEAD_LEN+PTZ_PACKAGE_PROTOCOL_LEN+PTZ_PACKAGE_LENGTH_LEN \
                              + PTZ_PACKAGE_COMMAND_LEN + PTZ_PACKAGE_TIMESMAP_LEN + PTZ_PACKAGE_SEQ_MSG_LEN \
                              + PTZ_PACKAGE_SEQ_MSG_LEN + PTZ_PACKAGE_STOPBIT_LEN)

#define PTZ_COMMAND_DEV_STATUS          0x01
#define PTZ_COMMAND_ATT_MSG             0x02
#define PTZ_COMMAND_ADDR                0x03
#define PTZ_COMMAND_SERACH_TRACK        0x04
#define PTZ_COMMAND_DISTEND_LOOKUP      0x05
#define PTZ_COMMAND_DISTEND_CONTROL     0x06
#define PTZ_COMMAND_DISTEND_STATUS      0x07
#define PTZ_COMMAND_DEV_EXT_INFO        0x08
#define PTZ_COMMAND_CONTROL_PACKAGE     0x09
#define PTZ_COMMAND_SCAN_EXT            0x0A
#define PTZ_COMMAND_TARGET_INFO         0x0B
#define PTZ_COMMAND_LENSE_EXT_INFO      0x0C
#define PTZ_COMMAND_DIRECTION_CONTROL   0x0D
#define PTZ_COMMAND_LOCK_TARGET_MANUAL  0x10
#define PTZ_COMMAND_PERIPHERAL_CTRL     0x11
#define PTZ_COMMAND_AI_CTRL             0x12
#define PTZ_COMMAND_CONFIG_FOCAL_MODE   0x13
#define PTZ_COMMAND_CONFIG_TRACK_PARAM  0x14
#define PTZ_COMMAND_SYS_EXT_INFO        0x15
#define PTZ_COMMAND_ADDR_EXT            0x16

#define PTZ_PACKAGE_MAX_LEN 2048

#pragma pack(1)
	typedef struct ptz_protocol_pac
	{
		uint32_t head;
		uint32_t protocol_VER;
        uint32_t len;
		uint32_t command;
		uint64_t timesmap;
        uint8_t *msg;
        uint64_t msg_seq_no;
        uint32_t crc;
        uint32_t stopbit;  
	}ptz_protocol_pac_t;
#pragma pack()

#pragma pack(1)
	typedef struct ptz_protocol_msg
	{
        uint32_t command;
        int msglen;
        uint8_t *msg;//msg max len
	}ptz_protocol_msg_t;
#pragma pack()

int ptz_protocol_package(uint32_t msgid, void* msg, uint32_t len, uint8_t* pac);
int ptz_protocol_network_data_analysis(uint8_t *pac, uint32_t len, ptz_protocol_msg_t *lmsg);
int ptz_protocol_msg_destory(ptz_protocol_msg_t *lmsg);

#endif
