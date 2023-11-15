
#include "alink_target.h"
#include "../track/alink_track.h"
#include "../../../algs/alg_interface.h"

static uint16_t track_pkg_target_info( uint8_t *pbyBuffer, uint16_t wSize, void *pvObjTarget )
{
	uint32_t i = 0;
	alink_taget_info_t *psTarget;
    alink_taget_info_t *psObjTarget;

	psTarget = (alink_taget_info_t*)pbyBuffer;
    psObjTarget = (alink_taget_info_t*)pvObjTarget;
	
    psTarget->timestamp = psObjTarget->timestamp;
    psTarget->objNum = psObjTarget->objNum;

	for( ; psObjTarget->objNum > i; ++i )
	{
        psTarget->targets[i].id = psObjTarget->targets[i].id;
        psTarget->targets[i].classification = psObjTarget->targets[i].classification;
        psTarget->targets[i].rectX = psObjTarget->targets[i].rectX;
        psTarget->targets[i].rectY = psObjTarget->targets[i].rectY;
        psTarget->targets[i].rectW = psObjTarget->targets[i].rectW;
        psTarget->targets[i].rectH = psObjTarget->targets[i].rectH;
        psTarget->targets[i].rectXV = psObjTarget->targets[i].rectXV;
        psTarget->targets[i].rectYV = psObjTarget->targets[i].rectYV;
        psTarget->targets[i].classfyProb = psObjTarget->targets[i].classfyProb;
        // psTarget->targets[i].reserve
	}

	return 5+i*24;

}

static void user_custom_track_target( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	user_track_target_packages_t utt_pack = {0};
	user_track_target_t *utt= (user_track_target_t *)psPayload->pbyData;

#if 1
	char *ptmp = (char *)psPayload->pbyData;
	printf("%s:%d>user target, payload len = %d\n", __FUNCTION__, __LINE__, psPayload->wLength);
	for(int i=0; i<psPayload->wLength; i++) {
		printf("%02x ", ptmp[i]);
	}
	printf("\n");
#endif

	utt_pack.track_pack_type = TRACK_INFO_FROM_USER;
	memcpy(&utt_pack.utt, utt, psPayload->wLength);
	msg_send_to_alg(&utt_pack, sizeof(user_track_target_packages_t));
}

static alink_package_list_t sAlinkPackage_TargetInfo = { 0 };
static alink_cmd_list_t sAlinkPackage_user_target = { 0 };

void alink_target_init( void )
{
    sAlinkPackage_user_target.cmd_type = ALINK_USER_TYPE_C2;

	alink_register_package(&sAlinkPackage_TargetInfo, 0x80,	0, track_pkg_target_info);
    alink_register_cmd(&sAlinkPackage_user_target, 0x81, sizeof(int), user_custom_track_target);
}

int32_t alink_upload_target_info( void *psObjTargetInfo )
{
	return alink_upload_package( &sAlinkPackage_TargetInfo, psObjTargetInfo );
}
