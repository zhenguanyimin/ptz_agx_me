#include <sys/time.h>
#include "alg_app.h"
#include "../../srv/algs/alg_interface.h"
#include "../../srv/log/skyfend_log.h"
#include "../../common/tools/skyfend_tools.h"
#include "../../inc/skyfend_error.h"

#include "../ptz/ptz_app.h"

#include "jetson-utils/videoSource.h"
#include "jetson-utils/logging.h"
#include "jetson-utils/commandLine.h"
#include <opencv2/opencv.hpp>

#include "ai_main.h"

using cv::Mat;
using std::array;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;
using std::vector;
using namespace cv;

#define TARGET_NODE_MAX_NUMBER  3

typedef struct target_pack_info {
    int data_dev_type;
    void* target_pack;
    int pack_len;
}target_pack_info_t;

typedef struct targets {
    target_pack_info_t target;
    struct targets *next;
}targets_t;

typedef struct alg_app_res {
    thread_task_t   *task_list;
    int             task_list_num;
    targets_t *head;//targets from devices
    targets_t *user_target; //targets from users.
    int         targets_num;
    pthread_mutex_t 	mutex;

    bool release;
}alg_app_res_t;
static alg_app_res_t alg_t = {0};

/*insert to the last of link*/
static int insert_target_to_head(alg_app_res_t *aar_t, target_pack_info_t *pack)
{
    targets_t *node = NULL;
    targets_t *tmp = NULL;
    int ret = -1;

    ret = pthread_mutex_lock(&(aar_t->mutex));
    if(0 != ret) {
        skyfend_log_error("%s:%d> get lock fail = %d", __FUNCTION__, __LINE__, ret);
        return RET_VALUE_FUNCTION_CALL_FAIL;
    }

    if(!pack || pack->pack_len <= 0) {
        skyfend_log_error("%s:%d>invalid parameter", __FUNCTION__, __LINE__);
        ret = RET_VALUE_INVALID_PARM;
        goto exit;
    }

    node = (targets_t*)skyfend_malloc(sizeof(targets_t));
    if(NULL == node) {
        skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
        ret = RET_VALUE_PARAMETER_NULL;
        goto exit;
    }
    node->target.data_dev_type = pack->data_dev_type;
    node->target.pack_len = pack->pack_len;
    node->target.target_pack = skyfend_malloc(node->target.pack_len);
    if(NULL == node->target.target_pack) {
        skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
        ret = RET_VALUE_PARAMETER_NULL;
        skyfend_free(node);
        goto exit;
    } else {
        memcpy(node->target.target_pack, pack->target_pack, node->target.pack_len);
    }
    node->next = NULL;

    /*user targets*/
    if(pack->data_dev_type == TRACK_INFO_FROM_USER) {
        if(aar_t->user_target) {
            skyfend_free(aar_t->user_target);
            aar_t->user_target = NULL;
        }
        aar_t->user_target = node;
        ret = 0;
        skyfend_log_info("insert node user target sucessfully");
        goto exit;
    }
    node->next = NULL;

    /*devices targets*/
    if(NULL == aar_t->head) {
        aar_t->head = node;
        ret = 0;
    } else {
        if(aar_t->targets_num > TARGET_NODE_MAX_NUMBER) {
            tmp = aar_t->head;
            aar_t->head = aar_t->head->next;
            if(tmp->target.target_pack) {
                skyfend_free(tmp->target.target_pack);
                tmp->target.target_pack = NULL;
            }
            skyfend_free(tmp);
            tmp = NULL;
            aar_t->targets_num --;
        }
        tmp = aar_t->head;
        while(tmp->next) {
            tmp=tmp->next;
        }
        tmp->next = node;
        ret = 0;
    }
    aar_t->targets_num ++;

exit:
    pthread_mutex_unlock(&(aar_t->mutex));
    //skyfend_log_debug("insert node> link target number = %d", aar_t->targets_num);

    return ret;
}

/*get the oldest onde head very time*/
static targets_t *get_and_rm_one_target(alg_app_res_t *aar_t)
{
    targets_t *node = NULL;
    int ret = 0;
    
    ret = pthread_mutex_lock(&(aar_t->mutex));
    if(0 != ret) {
        skyfend_log_error("%s:%d> get lock fail = %d", __FUNCTION__, __LINE__, ret);
        return NULL;
    }
    /*the poriority of user targets is highest, will check user target first*/
    if(aar_t->user_target) {
        node = aar_t->user_target;
        aar_t->user_target = NULL;
        goto exit;
    }
    if(!aar_t->head) {
        // skyfend_log_error("head is null");
        goto exit;
    }
    /*get device targets*/
    node = aar_t->head;
    aar_t->head = aar_t->head->next;
    node->next = NULL;
    aar_t->targets_num --;

exit:
    pthread_mutex_unlock(&(aar_t->mutex));
    //skyfend_log_debug("get node> link target number = %d", aar_t->targets_num);

    return node;
}

static int remove_all_targets(alg_app_res_t *aar_t)
{
    targets_t *node = NULL;
    targets_t *tmp = NULL;

    pthread_mutex_lock(&(aar_t->mutex));    
    if(!aar_t->head) {
        goto exit;
    }
    for(node=aar_t->head; node; ) {
        tmp = node;
        node = node->next;
        if(tmp->target.target_pack) {
            skyfend_free(tmp->target.target_pack);
            tmp->target.target_pack = NULL;
        }
        skyfend_free(tmp);
    }
    aar_t->targets_num  = 0;
    aar_t->head = NULL;

exit:
    pthread_mutex_unlock(&(aar_t->mutex));

    return 0;
}

static int target_node_destory(alg_app_res_t *aar_t, targets_t *node)
{
    if(node) {
        if(node->target.target_pack) {
            skyfend_free(node->target.target_pack);
            node->target.target_pack = NULL;
        }
        skyfend_free(node);
    }

    return 0;
}

static int algs_handle_multiple_targets(void *pack, void* arg)
{
    int ret = 0;
    int pack_size = *((int*)arg);
    target_pack_info_t target = {0};
    int data_dev_type = 0;

    if(pack_size <= 0) {
        skyfend_log_warn("alg> target package size invalid, %d", pack_size);
        return -1;
    }
    data_dev_type = ((int*)pack)[0];
    target.pack_len = pack_size;
    target.data_dev_type = data_dev_type;
    target.target_pack = pack;
    skyfend_log_debug("%s:%d > data_dev_type = %d", __FUNCTION__, __LINE__, data_dev_type);
    
#if 0
    skyfend_log_debug("heading = %d, pitching = %d", pack->track_attitude.heading \
            ,  pack->track_attitude.pitching);
    skyfend_log_debug("trackObjNum = %d",  pack->track_tracked.trackObjNum);
    skyfend_log_debug("trackTasNum = %d",  pack->track_tracked.trackTasNum);
#endif

    ret = insert_target_to_head(&alg_t, &target);
    if(0 != ret) {
        skyfend_log_warn("warming!!! insert target node to link fail, ret = %d", ret);
    }

    return ret;
}

extern int capture_ptz_vstream_pic(videostreaminfo_t *vsinfo, cv::Mat &frame);
static void* fusion_ai_alg_thread0(void *arg)
{
    alg_app_res_t *alg_obj = (alg_app_res_t *)arg;
    int ret = -1;
    videostreaminfo_t vsinfo = {0};
    targets_t *target_node = NULL;
    cv::Mat frame;
    int flag  = 0;
	std::string rtsp = DEFAULT_RTSP_PTZ;

    ret = 0;

    while(!alg_obj->release) {

        // test_upload_target_info();
        ret = capture_ptz_vstream_pic(&vsinfo, frame);
        if(0 != ret) {
            skyfend_msleep(100);
            skyfend_log_info("get stream pic fail, ret = %d", ret);
            continue;
        }

        target_node = get_and_rm_one_target(alg_obj);
        if(target_node) {
            skyfend_log_info("get target successfully");
            // skyfend_msleep(100);
            // continue;
        }
        
        // alg here.
        
        if(target_node) {
            // ret = ai_alg_run(yolov8, tracker, img, target_node->target.target_pack);
            ret = ai_alg_run7(frame, target_node->target.target_pack);
        } else {
            ret = ai_alg_run7(frame, NULL);
            // ret = ai_alg_run(yolov8, tracker, img, NULL);
        }
        
        // ret = ai_alg_run2(img_count, no_obj_count, img, target_node->target.target_pack);
        
        if(0 != ret) {
            skyfend_log_info("ai alg return err %d");
        }

        target_node_destory(alg_obj, target_node);
        target_node = NULL;
        //send alg result to c2.
    }

    return NULL;
}

static void* fusion_ai_alg_thread(void *arg)
{
    alg_app_res_t *alg_obj = (alg_app_res_t *)arg;
    int ret = -1;
    videostreaminfo_t vsinfo = {0};
    targets_t *target_node = NULL;
    cv::Mat frame;
    int flag  = 0;
	std::string rtsp = DEFAULT_RTSP_PTZ;
	struct timeval tv;
    
    ret = 0;

    while(!alg_obj->release) {
#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t capture_begin_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds        
#endif
        // test_upload_target_info();
        ret = capture_ptz_vstream_pic(&vsinfo, frame);
#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t capture_end_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds
        skyfend_log_debug("capture_duration = %lu ms", capture_end_time - capture_begin_time);         
#endif        
        if(0 != ret) {
            skyfend_msleep(100);
            skyfend_log_info("get stream pic fail, ret = %d", ret);
            continue;
        }
        // else{
        //     skyfend_log_info("-----get stream pic successfully, ret = %d", ret);
        // }

#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t target_begin_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds        
#endif
        target_node = get_and_rm_one_target(alg_obj);
#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t target_end_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds
        skyfend_log_debug("handle_targer_duration = %lu ms", target_end_time - target_begin_time);         
#endif           
        if(target_node) {
            skyfend_log_info("--------------------------get target successfully");
            // skyfend_msleep(100);
            // continue;
        }
    
        
        // alg here.
#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t run_begin_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds        
#endif        
        if(target_node) {
            // ret = ai_alg_run(yolov8, tracker, img, target_node->target.target_pack);
            ret = ai_alg_run7(frame, target_node->target.target_pack);
        } else {
            ret = ai_alg_run7(frame, NULL);
            // ret = ai_alg_run(yolov8, tracker, img, NULL);
        }
#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t run_end_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds
        skyfend_log_debug("alg_run_duration = %lu ms", run_end_time - run_begin_time);         
#endif          
        // ret = ai_alg_run3_nj(frame, NULL);
        
        // ret = ai_alg_run2(img_count, no_obj_count, img, target_node->target.target_pack);
        
        if(0 != ret) {
            skyfend_log_info("ai alg return err %d");
        }

        target_node_destory(alg_obj, target_node);
        target_node = NULL;
        //send alg result to c2.
#ifdef DEBUG_ALG_TRACK_INFO
		gettimeofday(&tv, NULL);
        uint64_t end_time = tv.tv_sec * 1000 + tv.tv_usec / 1000; //millseconds
        skyfend_log_debug("all_duration = %lu ms", end_time - capture_begin_time);         
#endif         
    }

    return NULL;
}


#ifdef DEBUG_NODE_LINKER
static void* test_insert_node(void *arg)
{
    int ret = 0;
    target_pack_info_t target = {0};
    int data_dev_type = 0;
    char pack[255] = {'a'};
    alg_app_res_t *alg_obj = (alg_app_res_t *)arg;

    do {
        data_dev_type = 0;
        target.pack_len = 255;
        target.data_dev_type = data_dev_type;
        target.target_pack = pack;

        ret = insert_target_to_head(alg_obj, &target);
        if(0 != ret) {
            skyfend_log_warn("warming!!! insert target node to link fail, ret = %d", ret);
        }
        // skyfend_msleep(10);
    } while(!alg_obj->release);

    return NULL;
}
#endif

int alg_init(void)
{
    int ret = -1;
#ifdef DEBUG_NODE_LINKER    
    // thread_task_t   task_list[] = { {0, fusion_ai_alg_thread, "fa_alg", &alg_t}, {0, test_insert_node, "testi", &alg_t}};
#else    
    thread_task_t   task_list[] = { {0, fusion_ai_alg_thread, "fa_alg", &alg_t}};
#endif    
    pthread_mutexattr_t attr;

    ret = load_multiple_targets_algs(algs_handle_multiple_targets);
    if(0 != ret) {
        skyfend_log_fatal("load multiple targets alg fail, ret = %d", ret);
        return ret;
    }

    if((ret = pthread_mutexattr_init(&attr)) != 0) {
		fprintf(stderr,"create mutex attribute error.msg:%s",strerror(ret));
	}
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    ret = pthread_mutex_init(&(alg_t.mutex), &attr);
    if(0 != ret){
        skyfend_log_error("init alg mutex fail, ret = %d", ret);
        return ret;
	}

    alg_t.release = false;
    alg_t.targets_num = 0;
    alg_t.head = NULL;
    alg_t.user_target = NULL;
    alg_t.task_list = (thread_task_t*)skyfend_malloc(sizeof(task_list));
    if(!alg_t.task_list) {
        skyfend_log_error("%s:%d> memory error...\n", __FUNCTION__, __LINE__);
        return RET_VALUE_MALLOC_FAIL;
    }
    memcpy(alg_t.task_list, task_list, sizeof(task_list));
    alg_t.task_list_num = sizeof(task_list)/sizeof(task_list[0]);
    ret = opt_thread_task(task_list, sizeof(task_list)/sizeof(task_list[0]), TASK_OPT_CREATE);
    if(0 != ret) {
        skyfend_log_fatal("start thread task fail! ret = %d", ret);
        goto error_exit;
    }

    return 0;
    
error_exit:
    unload_multiple_targets_algs();
    skyfend_log_info("load alg thread fail");
    
    return ret;
}

int alg_uninit(void)
{
    int ret = -1;

    alg_t.release = true;

    if(alg_t.task_list) {
        opt_thread_task(alg_t.task_list, alg_t.task_list_num, TASK_OPT_WAIT);
        skyfend_free(alg_t.task_list);
        alg_t.task_list = NULL;        
    }
    ret = unload_multiple_targets_algs();

    pthread_mutex_destroy(&(alg_t.mutex));

    return ret;
}
