#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <time.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../srv/log/skyfend_log.h"
#include "skyfend_tools.h"

#define MAX_LEN_STR                     (1024*4)
#define MAX_LEN_PATH                    512
#define MAX_LEN_ICONV_BUF               (1024*4)
#define BASE_TO_HEX                     16

static volatile int malloc_free_cnt = 0;

/*	malloc  */
void* skyfend_malloc(size_t size)
{
    void *ptr = NULL;

    if(0 == size)
    {
        return NULL;
    }

    ptr = malloc(size);
    if(NULL != ptr)
    {
        memset(ptr, 0, size);
        ++malloc_free_cnt;
    }

    return ptr;
}

/*	安全版的free(不要直接调用该函数，应使用宏 skyfend_free)    */
void skyfend_safefree(void **ptr)
{
    if((NULL != ptr) && (NULL != *ptr))
    {
        free(*ptr);
        *ptr = NULL;

        --malloc_free_cnt;
    }
}

/*	获取当前malloc - free的计数值   */
int get_skyfend_malloc_free_cnt()
{
    return malloc_free_cnt;
}

/*	获取目录下的所有文件(不包含隐藏文件)    */
static int get_all_file(const char *src_dir, int *num, char **files)
{
    char            path[MAX_LEN_PATH+1] = {'\0'};
    DIR             *dir = NULL;
    struct dirent   *ptr = NULL;

    if((NULL == src_dir) || (NULL == num))
    {
        skyfend_log_error("invalid parameter!\n");
        return -1;
    }

    dir = opendir(src_dir);
    if(NULL == dir)
    {
        skyfend_log_error("opendir %s fail!\n", src_dir);
        return -1;
    }

    if(NULL == files)
    {
        while((ptr = readdir(dir)) != NULL)
        {
            if(ptr->d_name[0] == '.')
            {
                continue;
            }

            if(DT_DIR == ptr->d_type)
            {
                memset(path, '\0', sizeof(path));
                snprintf(path, sizeof(path), "%s/%s", src_dir, ptr->d_name);
                get_all_file(path, num, NULL);
            }
            else
            {
                (*num)++;
            }
        }
    }
    else
    {
        while((ptr = readdir(dir)) != NULL)
        {
            if(ptr->d_name[0] == '.')
            {
                continue;
            }

            if(4 == ptr->d_type)
            {
                memset(path, '\0', sizeof(path));
                snprintf(path, sizeof(path), "%s/%s", src_dir, ptr->d_name);
                get_all_file(path, num, files);
            }
            else
            {
                files[*num] = (char*)skyfend_malloc((strlen(src_dir) + strlen(ptr->d_name) + 2) * sizeof(char));
                if(NULL == files[*num])
                {
                    skyfend_log_error("malloc fail!\n");
                    skyfend_free(files[*num]);
                    closedir(dir);
                    return -1;
                }
                snprintf(files[(*num)++], strlen(src_dir) + strlen(ptr->d_name) + 2, "%s/%s", src_dir, ptr->d_name);
            }
        }
    }
    closedir(dir);

    return 0;
}

/*	获取目录下的所有文件列表(不包含隐藏文件)    */
int get_all_filename_from_dir(const char *src_dir, int *num, char ***files)
{
    int     ret = -1;
    char    **fp = NULL;
    int     i = 0;

    if((NULL == src_dir) || (NULL == num) || (NULL == files))
    {
        skyfend_log_error("invalid parameter!\n");
        return ret;
    }

    ret = get_all_file(src_dir, num, NULL);
    if(0 != ret)
    {
        skyfend_log_error("get num fail!\n");
        return ret;
    }

    fp = (char**)skyfend_malloc((*num) * sizeof(char*));
    printf("------------malloc fp = %p-------\n", fp);
    if(NULL == fp)
    {
        skyfend_log_error("malloc fail!\n");
        return ret;
    }

    *num = 0;
    ret = get_all_file(src_dir, num, fp);
    for(i=0; i <*num; ++i)
    {
        skyfend_log_debug("fp[%d] = %s\n", i, fp[i]);
    }
    *files = fp;

    return ret;
}

/*	删除目录下的所有文件    */
static int rm_all_file(const char *src_dir)
{
    char            path[MAX_LEN_PATH+1] = {'\0'};
    DIR             *dir = NULL;
    int             ret = 0;
    struct dirent   *ptr = NULL;

    dir = opendir(src_dir);
    if(NULL == dir)
    {
        skyfend_log_error("opendir %s fail!\n", src_dir);
        return ret;
    }
    skyfend_log_debug("opendir %s success!\n", src_dir);

    while((ptr = readdir(dir)) != NULL)
    {
        if(ptr->d_name[0] == '.')
        {
            continue;
        }

        if(DT_DIR == ptr->d_type)
        {
            memset(path, '\0', sizeof(path));
            snprintf(path, sizeof(path), "%s/%s", src_dir, ptr->d_name);
            ret = rm_all_file(path);
            if(0 == ret)
            {
                if(rmdir(path) == 0)
                {
                    skyfend_log_info("rmdir(\"%s\")--success!\n", path);
                }
                else
                {
                    skyfend_log_error("rmdir(\"%s\")--fail!--errno = %d\n", path, errno);
                    ret = -1;
                    break;
                }
            }
        }
        else
        {
            snprintf(path, sizeof(path), "%s/%s", src_dir, ptr->d_name);
            if(unlink(path) == 0)
            {
                skyfend_log_info("unlink(\"%s\")--success!\n", path);
            }
            else
            {
                skyfend_log_error("unlink(\"%s\")--fail!--errno = %d\n", path, errno);
                ret = -1;
                break;
            }
        }
    }
    closedir(dir);

    return ret;
}

/*	删除目录及目录下的所有文件  */
int rm_all_file_in_dir(const char *dir, bool flg_rmdir)
{
    int ret = -1;

    ret = rm_all_file(dir);
    if(0 != ret)
    {
        skyfend_log_error("rm all file fail!   ret = %d\n", ret);
        return ret;
    }

    if(0 != flg_rmdir)
    {
        ret = rmdir(dir);
    }

    return ret;
}

/*	毫秒延时    */
int skyfend_msleep(unsigned long ms)
{
    struct timeval tv = {0};

    tv.tv_sec = ms/1000;
    tv.tv_usec = (ms%1000)*1000;

    if(select(0, NULL, NULL, NULL, &tv) != 0)
    {
        return -1;
    }

    return 0;
}

/*	获取字符串的长度    */
size_t skyfend_strlen(const char *str)
{
    int     num = 0;
    int     i = 0;
    size_t  len = 0;
    char    *p = NULL;

    p = (char*)str;
    len = strlen(p);
    if(0 < len && 2 != FILE_CODE_CHINESE_CHAR_OCCUPY_CHAR)
    {
        for(i=0; i<len; ++i)
        {
            if(p[i] < 0)
            {
                ++num;
            }
        }
        len -= num / FILE_CODE_CHINESE_CHAR_OCCUPY_CHAR;
        printf("len = %ld, num = %d\n", len, num);
    }
    printf("%s, str = \"%s\", len = %ld\n", __FUNCTION__, str, len);

    return len;
}

/*	读取文件全部数据并返回 （需要手动释放内存） */
char* read_file(const char *filename)
{
    FILE    *fp = NULL;
    char    *data = NULL;
    int     len = 0;
    int     ret = 0;

    fp = fopen(filename, "rb");
    if(NULL == fp)
    {
        skyfend_log_error("open \"%s\" file fail!\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if(0 < len)
    {
        data = (char*)skyfend_malloc(len * sizeof(char));
        if(NULL == data)
        {
            skyfend_log_error("malloc fail!\n");
            skyfend_free(data);
        }
        ret = fread(data, sizeof(char), len, fp);
        (void)ret;
    }

    fclose(fp);

    return data;
}

/*  非阻塞的getchar */
int skyfend_getchar()
{
    fd_set rfds;
    struct timeval tv = {0};
    int ch = EOF;

    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    if(0 < select(1, &rfds, NULL, NULL, &tv))
    {
        ch = getchar();
    }

    return ch;
}

/*  16进制字串转16进制整数    */
long str_to_hex(const char *str)
{
    long val = 0;

    val = strtol(str, NULL, BASE_TO_HEX);
    if(((0 != errno) && (0 == val)))
    {
        perror("strtol");
        return -1;
    }

    return val;
}

/*  检查cpu是否是小端模式   */
bool check_cpu_little_endian(void)
{
	union
	{
		uint32_t data;
		uint8_t ch;
	} endian;

	endian.data = 1;

    return (endian.ch == 1);
}

/*  32位的字节序翻转    */
uint32_t u32_reverse_byte_order(uint32_t num)
{
	uint32_t tmp = num;

	num = ((tmp & 0xFF000000) << 0) >> 24;
	num += ((tmp & 0xFF0000) << 8) >> 16;
	num += ((tmp & 0xFF00) << 16) >> 8;
	num += ((tmp & 0xFF) << 24) >> 0;

	return num;
}

/*  16位的字节序翻转    */
uint16_t u16_reverse_byte_order(uint16_t num)
{
	uint16_t tmp = num;

	num = (tmp & 0xFF00) >> 8;
	num += (tmp & 0xFF) << 8;

	return num;
}

/*  BCD转十进制 */
uint8_t bcd_to_decimal(uint8_t data)
{
	uint8_t temp = 0;

	temp =  (data >> 4) * 10;
	temp += (data & 0x0F);

    return temp;
}

/*  十进制转BCD */
uint8_t decimal_to_bcd(uint8_t data)
{
	uint8_t temp = 0;

	temp =  ((data / 10) << 4);
	temp += (data % 10);

    return temp;
}

/*  pthread线程操作 创建线程、检查线程存活、等待线程退出    */
int opt_thread_task(thread_task_t *task_list, unsigned int len, int opt)
{
    int ret = 0;
    int i = 0;

    if(TASK_OPT_CHECK_ALIVE == opt)
    {
        for(i=0; i<len; ++i)
        {
            ret = pthread_kill(task_list[i].task_id, 0);
            if(ESRCH == ret)
            {
                skyfend_log_error("%s, id %lu, is exit", task_list[i].task_name, task_list[i].task_id);
                ret = -1;
            }
            else if(EINVAL == ret)
            {
                skyfend_log_info("signal is invalid.\n");
                ret = 0;
            }
            else
            {
                skyfend_log_debug("%s, id %lu, is alive\n", task_list[i].task_name, task_list[i].task_id);
                ret = 0;
            }
        }
    }
    else if(TASK_OPT_CREATE == opt)
    {
        for(i=0; i<len; ++i)
        {
            pthread_create(&(task_list[i].task_id), NULL, task_list[i].task_app, task_list[i].arg);
            if(0 != ret)
            {
                skyfend_log_debug("pthread_create %s fail----ret = %d\n", task_list[i].task_name, ret);
                return -1;
            }
            skyfend_log_info("create %s thread, id %lu.", task_list[i].task_name, task_list[i].task_id);
        }
    }
    else if(TASK_OPT_WAIT == opt)
    {
        for(i=len-1; 0<=i; --i)
        {
/*          pthread_detach(task_list[i].task_id);   */
            skyfend_log_info("wait %lu.", task_list[i].task_id);
            pthread_join(task_list[i].task_id, NULL);
            skyfend_log_info("%lu join end.", task_list[i].task_id);
        }
    }

    return ret;
}

/*  将字符串中首次出现的数字转换成整数    */
int skyfend_atoi(const char *str)
{
    const char  *pos = NULL;
    int         numb = 0;
    int         sign = 1;

    if(NULL == str)
    {
        skyfend_log_warn("invalid parameter!\n");
        return numb;
    }

    pos = str;

    while('\0' != *pos)
    {
        if((('-' != *pos) && ('+' != *pos)) && (('9' < *pos) || (*pos < '0')))
        {
            ++pos;
            continue;
        }
        break;
    }

    if('\0' == *pos)
    {
        return numb;
    }

    if('-' == *pos)
    {
        sign = -1;
        ++pos;
    }
    else if('+' == *pos)
    {
        sign = 1;
        ++pos;
    }

    while(('0' <= *pos) && (*pos <= '9'))
    {
        numb = numb * 10 + (*pos - '0');
        ++pos;
    }

    return numb * sign;
}

/*  创建多级目录 （mkdir -p ）（目录权限775）  */
int mkdir_p(const char *path)
{
    char            tmp[MAX_LEN_PATH+1] = {0};
    unsigned int    len = 0;
    int             i = 0;

    if(NULL == path)
    {
        skyfend_log_warn("invalid parameter!\n");
        return -1;
    }

    if(access(path, F_OK) == 0)
    {
        skyfend_log_warn("%s already exist!\n", path);
        return 0;
    }

    len = strlen(path);
    if(MAX_LEN_PATH <= len)
    {
        skyfend_log_warn("%s, lenght is tool long! over the max(%u)\n\n", path, MAX_LEN_PATH);
        return -1;
    }

    memcpy(tmp, path, len>(MAX_LEN_PATH+1)?(MAX_LEN_PATH+1):len);
    if('/' != tmp[len - 1])
    {
        tmp[len] = '/';
        tmp[len + 1] = '\0';
        ++len;
    }

    for(i=0; i<len; ++i)
    {
        if('/' == tmp[i])
        {
            tmp[i] = '\0';
            if(access(tmp, F_OK) != 0)
            {
                skyfend_log_debug("%s not exist!   make it!\n", tmp);
                if(mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
                {
                    skyfend_log_error("mkdir %s fail!\n", tmp);
                    return -1;
                }
            }
            tmp[i] = '/';
        }
    }

    return 0;
}

/*  字符串转小写    */
int skyfend_strnlwr(char *dest, const char *src, size_t len)
{
    int i = 0;

    if((NULL == src) || (NULL == dest))
    {
        skyfend_log_warn("invalid parameter!\n");
        return -1;
    }

    for(i=0; i<len; ++i)
    {
        dest[i] = tolower(src[i]);
    }

    return 0;
}

