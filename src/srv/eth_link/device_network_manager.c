#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>  
#include <asm/types.h>  
#include <linux/netlink.h>  
#include <linux/rtnetlink.h>  
#include <sys/ioctl.h>  
#include <linux/if.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/route.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "device_network_manager.h"
#include "../log/skyfend_log.h"


static int net_eth_get_ipv4_addr(char *eth_name, unsigned char *ip)
{
	int ret = -1;
	int fd = -1;
	struct ifreq ifreq = {0};
	char *cip = NULL;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	memset(&ifreq, 0x00, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, eth_name);
	ret = ioctl(fd, SIOCGIFADDR, &ifreq);
	if (ret < 0) {
		perror("SIOCGIFADDR: ");
		goto ERROR;
	}
	if (ifreq.ifr_addr.sa_family != AF_INET) {
		ret = -1;
		goto ERROR;
	}
	struct sockaddr_in *addr = (struct sockaddr_in *)&(ifreq.ifr_addr);
	cip = inet_ntoa(addr->sin_addr);
	if(cip) {
		memcpy(ip, cip, strlen(cip));
	} else {
		skyfend_log_error("convert ip fail\n");
	}

	ret = 0;

ERROR:
	close(fd);

	return ret;
}

int agx_get_eht0_ip(unsigned char *ip)
{
    return net_eth_get_ipv4_addr(NETWORK_NETCARD_NAME, ip);
}
