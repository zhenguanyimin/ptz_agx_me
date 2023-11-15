#ifndef __COMMON_TOOLS_H__
#define __COMMON_TOOLS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#define SYSLOG_LEVEL_RELATION_EQUAL         0
#define SYSLOG_LEVEL_RELATION_GREAT         1
#define SYSLOG_LEVEL_RELATION_LESS          2

#define FILE_CODE_CHINESE_CHAR_OCCUPY_CHAR  3


#define TASK_OPT_CREATE                     1
#define TASK_OPT_CHECK_ALIVE                2
#define TASK_OPT_WAIT                       3

#define MAX_LEN_TASK_NAME                   64


typedef struct
{
    pthread_t   task_id;
    void*       (*task_app)(void*);
    char        task_name[MAX_LEN_TASK_NAME+1];
    void*       arg;
}thread_task_t;


/*	安全版的free（释放后会将指针指向NULL）  */
#define skyfend_free(p) skyfend_safefree((void**)&(p))


/*	自动在末尾添加‘\0’的strncpya            */
#define skyfend_strncpy(dest, src, len)            \
do                                              \
{                                               \
    char        *__p_dest = (dest);             \
    const char  *__p_src = (src);               \
    size_t      __src_len = (len);              \
                                                \
    memset(__p_dest, '\0', __src_len);          \
    strncpy(__p_dest, __p_src, __src_len);      \
    __p_dest[__src_len] = '\0';                 \
}while(0)

/*	malloc  */
void*   skyfend_malloc(size_t size);
/*	free（不要直接调用该函数，应使用宏 skyfend_free）  */
void    skyfend_safefree(void **ptr);
/*	获取当前malloc - free的计数值 （值为0则malloc与free成对使用）   */
int     get_skyfend_malloc_free_cnt();

/*	获取目录下的所有文件列表(不包含隐藏文件);   */
int     get_all_filename_from_dir(const char *src_dir, int *num, char ***files);

/*	删除目录及目录下的所有文件  */
int     rm_all_file_in_dir(const char *dir, bool flg_rmdir);

/*	毫秒延时    */
int     skyfend_msleep(unsigned long ms);

/*	获取字符串的长度    */
size_t  skyfend_strlen(const char *str);

/*	读取文件全部数据并返回 （需要手动释放内存） */
char*   read_file(const char *filename);

/*  非阻塞的getchar */
int     skyfend_getchar();

/*  16进制字串转16进制整数    */
long    str_to_hex(const char *str);

/*  检查cpu是否是小端模式   */
bool check_cpu_little_endian(void);

/*  32位的字节序翻转    */
uint32_t u32_reverse_byte_order(uint32_t num);

/*  16位的字节序翻转    */
uint16_t u16_reverse_byte_order(uint16_t num);

/*  BCD转十进制 */
uint8_t bcd_to_decimal(uint8_t data);

/*  十进制转BCD */
uint8_t decimal_to_bcd(uint8_t data);

/*  pthread线程操作 创建线程、检查线程存活、等待线程退出    */
int opt_thread_task(thread_task_t *task_list, unsigned int len, int opt);

/*  将字符串中首次出现的数字转换成整数  */
int skyfend_atoi(const char *str);

/*  创建多级目录 （mkdir -p ）（目录权限775）  */
int mkdir_p(const char *path);

/*  字符串转小写    */
int skyfend_strnlwr(char *dest, const char *src, size_t len);


#ifdef __cplusplus
}
#endif

#endif

