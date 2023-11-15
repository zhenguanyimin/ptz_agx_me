#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fcntl.h>
#include <sys/time.h>

#include "ptz_control_protocol.h"
#include "../../../common/tools/skyfend_tools.h"

static int ptz_protocol_crc_cal(uint8_t* pac, uint32_t len)
{
	const uint8_t* pos = pac;
	int verify = 0;

	for (int i = (PTZ_PACKAGE_PROTOCOL_LEN + PTZ_PACKAGE_HEAD_LEN); i < len; i++)
	{
		verify += pos[i];
	}

    return verify;
}

int ptz_protocol_package(uint32_t msgid, void* msg, uint32_t len, uint8_t* pac)
{
	ptz_protocol_pac_t *psHead = (ptz_protocol_pac_t*)(pac);
    uint8_t *data = pac;
    struct timeval tv;
    static uint32_t seq_no = 0;
    int offset = 0;
    int total_len = 0;

    gettimeofday(&tv, NULL);
    psHead->head = PTZ_PACKAGE_HEAD;
    psHead->protocol_VER = PTZ_PACKAGE_PROTOCOL_VERSION;
    psHead->len = len + (PTZ_PACKAGE_COMMAND_LEN + PTZ_PACKAGE_TIMESMAP_LEN + PTZ_PACKAGE_SEQ_MSG_LEN + PTZ_PACKAGE_CRC_LEN);
    psHead->command = msgid;
    psHead->timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    data = data + (PTZ_PACKAGE_HEAD_LEN+PTZ_PACKAGE_PROTOCOL_LEN+PTZ_PACKAGE_LENGTH_LEN \
                    +PTZ_PACKAGE_COMMAND_LEN + PTZ_PACKAGE_TIMESMAP_LEN);
    memcpy(data, msg, len);
    data = data + len;
    total_len = PTZ_PACKAGE_HEAD_LEN+PTZ_PACKAGE_PROTOCOL_LEN+PTZ_PACKAGE_LENGTH_LEN \
                    + PTZ_PACKAGE_COMMAND_LEN + PTZ_PACKAGE_TIMESMAP_LEN + len;
    ((uint32_t *)data)[offset++] = seq_no;
    total_len += PTZ_PACKAGE_SEQ_MSG_LEN;
    ((uint32_t *)data)[offset++] = ptz_protocol_crc_cal(pac, total_len);
    total_len += PTZ_PACKAGE_CRC_LEN;
    ((uint32_t *)data)[offset++] = PTZ_PACKAGE_STOPBIT;
    total_len += PTZ_PACKAGE_STOPBIT_LEN;

#if 1
    printf("\n----------------send pack(%d)\n", total_len);
    for(int i =0; i<total_len; i++) {
        printf("%02x ", pac[i]);
    }
    printf("\n\n");
#endif

    //the whole pacakge len
	return total_len;
}

int ptz_protocol_network_data_analysis(uint8_t *pac, uint32_t len, ptz_protocol_msg_t *lmsg)
{
	uint8_t* pData = pac;
	ptz_protocol_pac_t *psHead = (ptz_protocol_pac_t*)(pac);
    uint32_t offset = 0;
    int verify = 0, cal_verify = 0;
    int ret = -1;

	do{
        if(len < PTZ_PACKAGE_MINI_LEN) {
            skyfend_log_error("pacakge len not valid, %d, mini len %d\n", len, PTZ_PACKAGE_MINI_LEN);
        }
		if(PTZ_PACKAGE_HEAD != psHead->head) {
            skyfend_log_error("head not the same, %0x, wanted %x\n", psHead->head, PTZ_PACKAGE_HEAD);
			break;
        }
        lmsg->msglen = psHead->len - \
                        (PTZ_PACKAGE_TIMESMAP_LEN + PTZ_PACKAGE_COMMAND_LEN + \
                        PTZ_PACKAGE_SEQ_MSG_LEN + PTZ_PACKAGE_CRC_LEN);
        lmsg->command = psHead->command;

        offset = (PTZ_PACKAGE_HEAD_LEN + PTZ_PACKAGE_PROTOCOL_LEN + PTZ_PACKAGE_LENGTH_LEN \
                        + PTZ_PACKAGE_COMMAND_LEN + PTZ_PACKAGE_TIMESMAP_LEN);
        if(lmsg->msglen > 0) {
            lmsg->msg = skyfend_malloc(lmsg->msglen);
            if(!lmsg->msg) {
                skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
                return -1;
            }
            memcpy(lmsg->msg, pac + offset, lmsg->msglen);
        }
        cal_verify = ptz_protocol_crc_cal(pac, offset + lmsg->msglen + PTZ_PACKAGE_SEQ_MSG_LEN);
        pData = pac + offset + lmsg->msglen + PTZ_PACKAGE_SEQ_MSG_LEN;
        verify = ((uint32_t*)pData)[0];
        if(verify != cal_verify) {
            skyfend_log_warn("verify not the same, pac is %0x, cal is %0x\n", verify, cal_verify);
            ret = -1;
            goto exit;
        }
        verify = ((uint32_t*)pData)[1];
        if(PTZ_PACKAGE_STOPBIT != verify) {
            skyfend_log_warn("warming!!!stopbit not the same, pac is %0x, wanted %0x\n", verify, PTZ_PACKAGE_STOPBIT);
        }
	}while(0);

    return 0;
exit:
    if(lmsg->msg) {
        skyfend_free(lmsg->msg);
        lmsg->msg = NULL;
    }
	return ret;
}

int ptz_protocol_msg_destory(ptz_protocol_msg_t *lmsg)
{
    if(lmsg && lmsg->msg) {
        skyfend_free(lmsg->msg);
        lmsg->msg = NULL;
    }

    return 0;
}

