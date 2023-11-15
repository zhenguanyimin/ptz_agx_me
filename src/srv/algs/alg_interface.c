#include "alg_interface.h"
#include "../log/skyfend_log.h"
#include "../../common/sysport/sysport.h"

typedef struct alg_ctr_t {
    multiple_targets_algs_handle mtargets_handle;

    void*	pvalgtask;
}alg_ctrl_t;

static alg_ctrl_t alg_obj = {0};

static int load_alg_task(multiple_targets_algs_handle cbk)
{
    alg_obj.mtargets_handle = cbk;
    alg_obj.pvalgtask = sysport_task_create("/alg_task", &alg_obj);	
    if(NULL == alg_obj.pvalgtask) {
        skyfend_log_error("load alg msg thread fail");
        return -1;
    }

    return 0;
}

int load_multiple_targets_algs(multiple_targets_algs_handle cbk)
{
    int ret = -1;

    ret = load_alg_task(cbk);

    return ret;
}

int unload_multiple_targets_algs(void)
{
    if(alg_obj.pvalgtask) {
        sysport_task_delete(alg_obj.pvalgtask);
    }

    return 0;
}

static void alg_thread_handler(void* pvArg, void* pvInfo, uint32_t uSize)
{
    // alg_ctrl_t* palg = (alg_ctrl_t*)pvArg;
    uint32_t pack_size = uSize;

    if(alg_obj.mtargets_handle) {
        alg_obj.mtargets_handle(pvInfo, (void*)(&pack_size));
    }
}

int msg_send_to_alg(void *pack, int pack_len)
{
    alg_ctrl_t* palg = &alg_obj;
    void* pvInfo =  NULL;
    int ret = -1;
    
    pvInfo = sysport_task_malloc(palg->pvalgtask, pack_len);
    if(pvInfo) {
        memcpy(pvInfo, pack, pack_len);
        sysport_task_func(palg->pvalgtask, alg_thread_handler, pvInfo);
        ret = 0;
    } else {
        skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
        ret = -1;
    }

    return ret;
}
