#ifndef __NETCON_PROTOCOL_H__
#define __NETCON_PROTOCOL_H__

#define NETPROTOCAL_MSG_ID_LOOKUP           0xBA
#define NETPROTOCAL_MSG_ID_LOOKUP_RESP      0xBB
#define NETPROTOCAL_MSG_ID_REQUEST_CONNECT  0xBC
#define NETPROTOCAL_MSG_ID_CONFIG_RESP      0xBD
#define RADAR_DEVICE_SN_LEN 32
#define NETPOTOCAL_INITILA_SN	"sn_init"
#pragma pack(1)
	typedef struct eth_protocol_head
	{
		uint8_t magic;
		uint8_t len_lo;
		uint8_t len_hi;
		uint8_t seq;
		uint8_t destid;
		uint8_t sourceid;
		uint8_t msgid;
		uint8_t ans;
		uint8_t checksum;
	}eth_protocol_head_t;
#pragma pack()

#pragma pack(1)
	typedef struct eth_protocol_devinfo
	{
		eth_protocol_head_t head;
			
		uint32_t	protocol;
		uint8_t 	sn[64];
		uint16_t	type;
		uint16_t	status;
		char		version[64];

		uint16_t	crc;
	}eth_protocol_devinfo_t;
#pragma pack()

#pragma pack(1)
	typedef struct eth_protocol_devinfo_body
	{
		uint32_t	protocol;
		uint8_t 	sn[32];
		uint16_t	type;
		uint16_t	status;
		char		version[64];
	}eth_protocol_devinfo_body_t;
#pragma pack()

#pragma pack(1)
	typedef struct eth_protocol_search
	{
		uint32_t	protocol;
		uint8_t		sn[32];
	}eth_protocol_search_t;
#pragma pack()

#pragma pack(1)
	typedef struct eth_protocol_cntinfo
	{
		uint8_t		sn[32];
		uint32_t	ip;
		uint16_t	port;
		uint16_t	type;
	}eth_protocol_cntinfo_t;
#pragma pack()

#pragma pack(1)
typedef struct eth_protocol_lookup {
    eth_protocol_head_t head;
    uint32_t	protocol;
    uint8_t 	sn[32];

    uint16_t	crc;
}eth_protocol_lookup_t;
#pragma pack()

#pragma pack(1)
typedef struct eth_protocol_request {
    eth_protocol_head_t head;
    eth_protocol_cntinfo_t cntinfo;
    uint16_t	crc;
}eth_protocol_request_t;
#pragma pack()

uint32_t eth_protocol_package(uint8_t msgid, void* msg, uint32_t len);
uint8_t* eth_protocol_analysis(void *msg, uint32_t len);
char* eth_protocol_get_agx_sn(void);
uint32_t eth_protocol_get_request_package(void* msg, eth_protocol_cntinfo_t *ep_ctlinfo);
uint32_t eth_protocol_get_detect_package(void* msg, uint32_t len);

#endif
