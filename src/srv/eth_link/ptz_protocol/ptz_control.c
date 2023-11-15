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
#include "../../log/skyfend_log.h"
#include "../eth_link_ptz.h"
#include "ptz_control.h"
#include "skyfend_tools.h"

//0x03
int send_ptz_control_addr(ptz_msgid_0x03_addr_t *addr_t)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!addr_t) {
		return -1;
	}

	gettimeofday(&tv, NULL);
	addr_t->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	addr_t->user_id = PTZ_USER_ID;
	addr_t->sys_no = PTZ_SYS_NO;
	addr_t->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	memcpy(dat, addr_t, sizeof(ptz_msgid_0x03_addr_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_ADDR;
	msginfo.msglen = sizeof(ptz_msgid_0x03_addr_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x0D
int send_ptz_control_direction(ptz_msg_direction_ctrl_t *dir_ctrl)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};

	if(!dir_ctrl) {
		return -1;
	}
	memcpy(dat, dir_ctrl, sizeof(ptz_msg_direction_ctrl_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_DIRECTION_CONTROL;
	msginfo.msglen = sizeof(ptz_msg_direction_ctrl_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x16
int send_ptz_control_add_ext(ptz_msg_addr_external_t *addr_ext)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};

	if(!addr_ext) {
		return -1;
	}
	memcpy(dat, addr_ext, sizeof(ptz_msg_addr_external_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_ADDR_EXT;
	msginfo.msglen = sizeof(ptz_msg_addr_external_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x09
int send_ptz_control_pack(ptz_msg_ctrl_pack_t *ctrl_p)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!ctrl_p) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	ctrl_p->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	ctrl_p->sys_no = PTZ_SYS_NO;
	ctrl_p->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, ctrl_p, sizeof(ptz_msg_ctrl_pack_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_CONTROL_PACKAGE;
	msginfo.msglen = sizeof(ptz_msg_ctrl_pack_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x04
int send_ptz_control_track(ptz_msg_track_t *track)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!track) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	track->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	track->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, track, sizeof(ptz_msg_track_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_SERACH_TRACK;
	msginfo.msglen = sizeof(ptz_msg_track_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x10
int send_ptz_control_manual_lock_target(ptz_msg_manual_lock_target_t *track)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!track) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	track->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	track->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, track, sizeof(ptz_msg_manual_lock_target_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_LOCK_TARGET_MANUAL;
	msginfo.msglen = sizeof(ptz_msg_manual_lock_target_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x0A
int send_ptz_control_scan_ext(ptz_msg_scan_ext_t *scan)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!scan) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	scan->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	scan->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, scan, sizeof(ptz_msg_scan_ext_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_SCAN_EXT;
	msginfo.msglen = sizeof(ptz_msg_scan_ext_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x11
int send_ptz_peripheral_ctrl(ptz_msg_peripheral_ctrl_t *pctrl)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!pctrl) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	pctrl->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	pctrl->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, pctrl, sizeof(ptz_msg_peripheral_ctrl_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_PERIPHERAL_CTRL;
	msginfo.msglen = sizeof(ptz_msg_peripheral_ctrl_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x12
int send_ptz_ai_ctrl(ptz_msg_ai_ctrl_t *actrl)
{
	int ret = 0;
	unsigned char *dat = NULL;
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!actrl || (actrl->param_len > 0 && !actrl->param)) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	actrl->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	actrl->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	dat = skyfend_malloc(sizeof(ptz_msg_ai_ctrl_t) + actrl->param_len);
	if(!dat) {
		return -1;
	}
	memcpy(dat, actrl, sizeof(ptz_msg_peripheral_ctrl_t));
	memcpy(dat+sizeof(ptz_msg_peripheral_ctrl_t) - sizeof(actrl->param), actrl->param, actrl->param_len);
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_AI_CTRL;
	msginfo.msglen = sizeof(ptz_msg_peripheral_ctrl_t) - sizeof(actrl->param) + actrl->param_len;
	ret = send_ptz_package(&msginfo);
	skyfend_free(dat);

	return ret;
}

//0x13
int send_ptz_config_focal_mode(ptz_msg_config_focal_mode_t *cfocal_mode)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!cfocal_mode) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	cfocal_mode->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	cfocal_mode->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, cfocal_mode, sizeof(ptz_msg_config_focal_mode_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_CONFIG_FOCAL_MODE;
	msginfo.msglen = sizeof(ptz_msg_config_focal_mode_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

//0x14
int send_ptz_config_track_param(ptz_msg_config_track_param_t *ctrack_param)
{
	int ret = 0;
	unsigned char dat[128] = {0};
	ptz_protocol_msg_t msginfo = {0};
	struct timeval tv;

	if(!ctrack_param) {
		return -1;
	}
	gettimeofday(&tv, NULL);
	ctrack_param->photoelectricity_no = PTZ_PHOTOELECTRICITY_NO;
	ctrack_param->sys_timesmap = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	memcpy(dat, ctrack_param, sizeof(ptz_msg_config_track_param_t));
	msginfo.msg = dat;
	msginfo.command = PTZ_COMMAND_CONFIG_TRACK_PARAM;
	msginfo.msglen = sizeof(ptz_msg_config_track_param_t);
	ret = send_ptz_package(&msginfo);

	return ret;
}

