/*
* skyfend 
*/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>

/* Standard includes. */
#include "srv/alink/alink.h"
#include "app/c2_network/c2_network.h"
#include "app/ptz/ptz_app.h"
#include "app/radars_network/radar_network.h"
#include "srv/eth_link/eth_link.h"
#include "common/common.h"
#include "app/alg_app/alg_app.h"
#include "srv/log/skyfend_log.h"
#include "common/tools/skyfend_tools.h"

static bool exit_app = false;

void sig_handler(int signum)
{

    if(SIGINT == signum || SIGQUIT == signum || SIGKILL == signum) {
        skyfend_log_info("Received signal [%s]",SIGINT==signum?"SIGINT":"Other");
		exit_app = true;
    }
	skyfend_log_info("releasing resources, wait app exit");

    return;
}

int main(int argc, char **argv)
{
	int ret = -1;

	signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);

	common_init();
	ret = skyfend_log_init(SKYFEND_LOG_INIT_FILE, SKYFEND_LOG_NAME);
	if(0 != ret) {
		skyfend_log_warn("warmming!!!, log init fail %d", ret);
	}

	ret = alg_init();
	if(0 != ret) {
		skyfend_log_fatal("alg initialized error, ret = %d", ret);
		return ret;
	}

	skyfend_log_debug("%s:%d loaded > eth_link_init", __FUNCTION__, __LINE__);
	eth_link_init();
	
	ret = load_ptz();
	if(0 != ret) {
		skyfend_log_fatal("%s:%d loaded > load_ptz", __FUNCTION__, __LINE__);
		return ret;
	}

	skyfend_log_debug("%s:%d loaded > alink_init", __FUNCTION__, __LINE__);	
	alink_init();
	skyfend_log_debug("%s:%d loaded > radar_network_init", __FUNCTION__, __LINE__);		
	c2_network_init();
	radar_network_init();

	skyfend_log_debug("%s:%d loaded > eth_link_server_init", __FUNCTION__, __LINE__);
	eth_link_server_init();

	skyfend_log_debug("loaded > main thread sleep....");

	while(!exit_app) {
		sleep(1);
	}

	skyfend_log_info("%s:%d relase_ptz_resource", __FUNCTION__, __LINE__);
	relase_ptz_resource();
	skyfend_log_info("%s:%d alg_uninit", __FUNCTION__, __LINE__);
	alg_uninit();
	skyfend_log_info("%s:%d radar_network_release", __FUNCTION__, __LINE__);
	radar_network_release();
	skyfend_log_info("%s:%d common_uninit", __FUNCTION__, __LINE__);	
	common_uninit();
	skyfend_log_deinit();

	skyfend_log_info("malloc count = %d", get_skyfend_malloc_free_cnt());

	return 0;
}
