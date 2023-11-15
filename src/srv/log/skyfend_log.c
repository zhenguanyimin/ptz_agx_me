#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "skyfend_log.h"

#define debug_print(fmt, args...)       printf("(%d)->%s() : " fmt, __LINE__, __FUNCTION__, ##args)

static zlog_category_t  *cur_category = NULL;
static int              init_flg = 0;


/*	改变当前log的分类   */
int change_cur_log_category(const char *cname)
{
    zlog_category_t *cat = NULL;

    if(NULL == cname)
    {
        debug_print("invalid parameter\n");
        return -1;
    }

    cat = zlog_get_category(cname);
    if(NULL == cat)
    {
        debug_print("get zlog %s category fail!\n", cname);
        return -1;
    }
    cur_category = cat;

    return 0;
}

/*	初始化log   */
int skyfend_log_init(const char *conf_path, const char *cname)
{
    int ret = -1;

    if((NULL == conf_path) || (NULL == cname))
    {
        debug_print("invalid parameter\n");
        return -1;
    }

    debug_print("conf_path = %s,   cname = %s\n", conf_path, cname);

    ret = zlog_init(conf_path);
    if(0 != ret)
    {
        debug_print("init zlog fail!\n");
        return ret;
    }

    ret = change_cur_log_category(cname);
    if(0 != ret)
    {
        debug_print("get zlog %s category fail!\n", cname);
        return ret;
    }

    init_flg = 1;

    return ret;
}

/*	反初始化log */
void skyfend_log_deinit()
{
    zlog_fini();
    init_flg = 0;
}

/*	log原型 */
void skyfend_log(const char *file, size_t file_len, const char *func, size_t func_len, long line,
                int level, const char *format, ...)
{
    va_list args = {0};
    char    buf[MAX_LEN_PRINT_BUF+1] = {0};

    if(NULL == format)
    {
        debug_print("invalid parameter\n");
        return ;
    }

    va_start(args, format);
    if(0 != init_flg)
    {
        vzlog(cur_category, file, file_len, func, func_len, line, level, format, args);
    }
    else
    {
        vsnprintf(buf, sizeof(buf), format, args);
        if(ZLOG_LEVEL_DEBUG < level)
        {
            printf("(%ld)->%s() : %s\n", line, func, buf);
        }
        else
        {
            printf("(%ld)->%s() : %s", line, func, buf);
        }
    }
    va_end(args);
}
