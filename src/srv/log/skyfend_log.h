#ifndef __COMMON_SKYFEND_LOG_H__
#define __COMMON_SKYFEND_LOG_H__

#define SKYFEND_LOG_INIT_FILE   "/home/skyfend/config/zlog.conf"
#define SKYFEND_LOG_NAME         "agx_ptz"

#ifdef __cplusplus
extern "C" {
#endif


#include "zlog.h"


#define MAX_LEN_PRINT_BUF               4096


/*	log API */
#define skyfend_log_fatal(format, args...) \
        skyfend_log(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	                ZLOG_LEVEL_FATAL, format, ##args)

#define skyfend_log_error(format, args...) \
        skyfend_log(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	                ZLOG_LEVEL_ERROR, format, ##args)

#define skyfend_log_warn(format, args...) \
        skyfend_log(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	                ZLOG_LEVEL_WARN, format, ##args)

#define skyfend_log_notice(format, args...) \
        skyfend_log(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	                ZLOG_LEVEL_NOTICE, format, ##args)

#define skyfend_log_info(format, args...) \
        skyfend_log(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	                ZLOG_LEVEL_INFO, format, ##args)

#define skyfend_log_debug(format, args...) \
        skyfend_log(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	                ZLOG_LEVEL_DEBUG, format, ##args)


/*	改变当前log的分类   */
int     change_cur_log_category(const char *cname);
/*	初始化log           */
int     skyfend_log_init(const char *conf_path, const char *log_name);
/*	反初始化log         */
void    skyfend_log_deinit();
/*	log原型             */
void    skyfend_log(const char *file, size_t file_len, const char *func, size_t func_len, long line,
                    int level, const char *format, ...);


// #define NETWORK_DATA_DEBUG
#ifdef NETWORK_DATA_DEBUG
	#define NETWORK_DATA_DEBUG_PRINTF(format, args...)	 printf(format, ##args)
#else
	#define NETWORK_DATA_DEBUG_PRINTF(format, args...)		((void)0)
#endif

#ifdef __cplusplus
}
#endif


#endif

