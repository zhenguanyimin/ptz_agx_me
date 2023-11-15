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
#include "netcon_protocol.h"

#define DEV_ID_AGX 0x07
#define PROTOCOL_ID 0x04
#define NETPROTOCAL_CRC_LEN 2

char* eth_protocol_get_agx_sn(void)
{
	return "AGX-12345678";
}

uint32_t eth_protocol_package(uint8_t msgid, void* msg, uint32_t len)
{
	eth_protocol_head_t *psHead = (eth_protocol_head_t*)(msg);
	psHead->magic = 0xFD;
	psHead->len_lo = (0xFF & (len>>0));
	psHead->len_hi = (0xFF & (len>>8));
	psHead->seq = 0;
	psHead->destid = 0xFF;
	psHead->sourceid = DEV_ID_AGX;	//radar id
	psHead->msgid = msgid;
	psHead->ans = 0;

	uint8_t checksum = 0;
	for( int i = 0; 8 > i; ++i )
		checksum += ((uint8_t*)(psHead))[i];
	psHead->checksum = checksum;

	uint16_t crc = 0xFFFF;
	uint32_t count = len + 11 - 3;
	uint8_t *data = (uint8_t*)(psHead);
    while( count-- )
    {
		uint8_t tmp;
		++data;
	    tmp = *data ^ (uint8_t)(crc & 0xFF);
	    tmp ^= (tmp << 4);
	    crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
    }
	data[1] = (0xFF & (crc>>0));
	data[2] = (0xFF & (crc>>8));

	return (len+11);
}

uint8_t* eth_protocol_analysis(void *msg, uint32_t len)
{
	uint8_t* pData = NULL;
	eth_protocol_head_t *psHead = (eth_protocol_head_t*)(msg);

	do{
		if( 0xFD != psHead->magic )
			break;

		uint32_t size;
		size = psHead->len_hi;
		size <<= 8;
		size |= psHead->len_lo;
		if( (size + 11) != (len) )
			break;

		uint8_t checksum = 0;
		for( int i = 0; 8 > i; ++i )
			checksum += ((uint8_t*)(psHead))[i];
		if( psHead->checksum != checksum )
			break;

		uint16_t crc = 0xFFFF;
		uint32_t count = len - 3;
		uint8_t *data = (uint8_t*)(psHead) + 1;
	    while( count-- )
	    {
			uint8_t tmp;
		    tmp = *data ^ (uint8_t)(crc & 0xFF);
		    tmp ^= (tmp << 4);
		    crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
			data++;
	    }
		uint16_t check = (data[1]<<8) | (data[0]<<0);
		if( crc != check )
			break;

		pData = ((uint8_t*)(msg)) + sizeof(eth_protocol_head_t);
	}while(0);

	return pData;
}

uint32_t eth_protocol_get_detect_package(void* msg, uint32_t len)
{
    eth_protocol_lookup_t ep_lookup_package = {0};

    ep_lookup_package.protocol = PROTOCOL_ID;
    //strncpy( ep_lookup_package.sn, eth_protocol_get_agx_sn(), RADAR_DEVICE_SN_LEN);
    memset( ep_lookup_package.sn, 0, RADAR_DEVICE_SN_LEN);

	eth_protocol_package(NETPROTOCAL_MSG_ID_LOOKUP, (uint8_t*)(&ep_lookup_package), sizeof(eth_protocol_lookup_t) \
                        - sizeof(eth_protocol_head_t) -  NETPROTOCAL_CRC_LEN);

    memcpy(msg, &ep_lookup_package, sizeof(eth_protocol_lookup_t));

    return sizeof(eth_protocol_lookup_t);
}

uint32_t eth_protocol_get_request_package(void* msg, eth_protocol_cntinfo_t *ep_ctlinfo)
{
    eth_protocol_request_t ep_req_package = {0};

    memcpy(&ep_req_package.cntinfo, ep_ctlinfo, sizeof(eth_protocol_cntinfo_t));

	eth_protocol_package(NETPROTOCAL_MSG_ID_REQUEST_CONNECT, (uint8_t*)(&ep_req_package), sizeof(eth_protocol_request_t) \
                        - sizeof(eth_protocol_head_t) -  NETPROTOCAL_CRC_LEN);

    memcpy(msg, &ep_req_package, sizeof(eth_protocol_request_t));

    return sizeof(eth_protocol_request_t);
}
