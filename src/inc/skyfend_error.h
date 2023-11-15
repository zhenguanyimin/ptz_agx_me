
#ifndef __SKYFEND_ERROR_H__
#define __SKYFEND_ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum skyfend_ret_no_e
{
    /*	通用返回和错误码    */
    SKYFEND_SUCCESSFUL                 = 0,            /*	成功时返回                              */
    SKYFEND_ERROR_LOGIN                = 10001,        /*	用户未登录                              */
    SKYFEND_ERROR_LOGIN_FAIL           = 10002,        /*	用户登录失败                            */
    SKYFEND_ERROR_OUT_OF_MEMORY        = 10003,        /*	内存越界                                */
    SKYFEND_ERROR_ASR_OUT_OF_MEMORY    = 10004,        /*	内存耗尽                                */
    SKYFEND_ERROR_INVALID_PARA_VALUE   = 10005,        /*	无效的参数值                            */
    SKYFEND_ERROR_PERMISSION_DENIED    = 10006,        /*	无效授权                                */
    SKYFEND_ERROR_NO_LICENSE           = 10007,        /*	没有授权许可                            */
    SKYFEND_ERROR_FILE_ACCESS          = 10008,        /*	打开文件失败                            */
    SKYFEND_ERROR_FILE_CLOSE           = 10009,        /*	关闭文件失败                            */
    SKYFEND_ERROR_FILE_EXIST           = 10010,        /*	文件已经存在                            */
    SKYFEND_ERROR_FILE_WRITE           = 10011,        /*	文件写入失败                            */
    SKYFEND_ERROR_FILE_READ            = 10012,        /*	文件读出失败                            */
    SKYFEND_ERROR_DEVICE_OFFLINE       = 10013,        /*	设置不在线                              */
    SKYFEND_ERROR_UNRECOGNIZED         = 10014,        /*	函数调用失败或不认识的错误              */
    SKYFEND_ERROR_APP_BUSY             = 10015,        /*	应用程序忙                              */

    /*	网络错误            */
    SKYFEND_NET_ERROR_GENERAL          = 20001,        /*	网络连接超时或服务不可访问或服务不存在  */
    SKYFEND_NET_ERROR_OPENSOCK         = 20002,        /*	打开socket失败                          */
    SKYFEND_NET_ERROR_CONNECTSOCK      = 20003,        /*	建立socket的连接失败                    */
    SKYFEND_NET_ERROR_ACCEPTSOCK       = 20004,        /*	接受socket连接失败                      */
    SKYFEND_NET_ERROR_SENDSOCK         = 20005,        /*	socket发送数据错误                      */
    SKYFEND_NET_ERROR_RECVSOCK         = 20006,        /*	socket接收数据错误                      */
    SKYFEND_NET_ERROR_INVALIDSOCK      = 20007,        /*	无效的socket                            */
    SKYFEND_NET_ERROR_BADADDRESS       = 20008,        /*	无效的地址                              */
    SKYFEND_NET_ERROR_NOTOPENSOCK      = 20009,        /*	没有打开socket                          */
    SKYFEND_NET_ERROR_NOTBIND          = 20010,        /*	没有绑定socket                          */
    SKYFEND_NET_ERROR_NOTLISTEN        = 20011,        /*	没有监听socket                          */
    SKYFEND_NET_ERROR_CONNECTCLOSE     = 20012,        /*	连接被关闭                              */
    SKYFEND_NET_ERROR_NOTDGRAMSOCK     = 20013,        /*	非数据报socket                          */
    SKYFEND_NET_ERROR_DNS              = 20014,        /*	DNS解析错误                             */

    /*	HTTP/HTTPS错误      */
    SKYFEND_HTTP_ERROR_INVALID         = 20301,        /*	无效的资源或服务不存在                  */
    SKYFEND_HTTP_SUCCESS_200           = 20302,        /*	200（成功）                             */
    SKYFEND_HTTP_SUCCESS_201           = 20303,        /*	201（已创建）                           */
    SKYFEND_HTTP_SUCCESS_202           = 20304,        /*	202（已接受）                           */
    SKYFEND_HTTP_SUCCESS_203           = 20305,        /*	203（无授权信息）                       */
    SKYFEND_HTTP_SUCCESS_204           = 20306,        /*	204（无内容）                           */
    SKYFEND_HTTP_SUCCESS_205           = 20307,        /*	205（重置内容）                         */
    SKYFEND_HTTP_SUCCESS_206           = 20308,        /*	206（部分内容）                         */
    SKYFEND_HTTP_REDIRECT_300          = 20309,        /*	300（多种选择）                         */
    SKYFEND_HTTP_REDIRECT_301          = 20300,        /*	301（永久移动）                         */
    SKYFEND_HTTP_REDIRECT_302          = 20311,        /*	302（临时移动）                         */
    SKYFEND_HTTP_REDIRECT_303          = 20312,        /*	303（查看其他位置）                     */
    SKYFEND_HTTP_REDIRECT_304          = 20313,        /*	304（未修改）                           */
    SKYFEND_HTTP_REDIRECT_305          = 20314,        /*	305（使用代理）                         */
    SKYFEND_HTTP_REDIRECT_307          = 20315,        /*	307（临时重定向）                       */
    SKYFEND_HTTP_ERROR_400             = 20316,        /*	400（错误请求）                         */
    SKYFEND_HTTP_ERROR_401             = 20317,        /*	401（未授权）                           */
    SKYFEND_HTTP_ERROR_403             = 20318,        /*	403（禁止）                             */
    SKYFEND_HTTP_ERROR_404             = 20319,        /*	404（未找到）                           */
    SKYFEND_HTTP_ERROR_405             = 20320,        /*	405（方法禁用）                         */
    SKYFEND_HTTP_ERROR_406             = 20321,        /*	406（不接受）                           */
    SKYFEND_HTTP_ERROR_407             = 20322,        /*	407（需要代理授权）                     */
    SKYFEND_HTTP_ERROR_408             = 20323,        /*	408（请求超时）                         */
    SKYFEND_HTTP_ERROR_409             = 20324,        /*	409（冲突）                             */
    SKYFEND_HTTP_ERROR_410             = 20325,        /*	410（已删除）                           */
    SKYFEND_HTTP_ERROR_411             = 20326,        /*	411（需要有效长度）                     */
    SKYFEND_HTTP_ERROR_412             = 20327,        /*	412（未满足前提条件）                   */
    SKYFEND_HTTP_ERROR_413             = 20328,        /*	413（请求实体过大）                     */
    SKYFEND_HTTP_ERROR_414             = 20329,        /*	414（请求的 URI 过长）                  */
    SKYFEND_HTTP_ERROR_415             = 20330,        /*	415（不支持的媒体类型）                 */
    SKYFEND_HTTP_ERROR_416             = 20331,        /*	416（请求范围不符合要求                 */
    SKYFEND_HTTP_ERROR_417             = 20332,        /*	417（未满足期望值）                     */
    SKYFEND_HTTP_ERROR_SERVER_500      = 20333,        /*	500（服务器内部错误）                   */
    SKYFEND_HTTP_ERROR_SERVER_501      = 20334,        /*	501（尚未实施）                         */
    SKYFEND_HTTP_ERROR_SERVER_502      = 20335,        /*	502（错误网关）                         */
    SKYFEND_HTTP_ERROR_SERVER_503      = 20336,        /*	503（服务不可用）                       */
    SKYFEND_HTTP_ERROR_SERVER_504      = 20337,        /*	504（网关超时）                         */
    SKYFEND_HTTP_ERROR_SERVER_505      = 20338,        /*	505（HTTP 版本不受支持                  */

    /*	MQTT错误            */
    SKYFEND_MQTT_ERROR_CONNECT_FAIL    = 20401,        /*	MQTT连接失败                            */
    SKYFEND_MQTT_ERROR_PUB_FAIL        = 20402,        /*	MQTT发布失败                            */
    SKYFEND_MQTT_ERROR_SUB_FAIL        = 20403         /*	MQTT订阅失败                            */
}skyfend_ret_no_t;

typedef enum local_retno_e
{
    RET_VALUE_STOP_RUN              = -7,
    RET_VALUE_JSON_ERROR            = -6,
    RET_VALUE_INVALID_PARM          = -4,
    RET_VALUE_MALLOC_FAIL           = -3,
    RET_VALUE_FUNCTION_CALL_FAIL    = -2,
    RET_VALUE_PARAMETER_NULL        = -1,
    RET_VALUE_FUNCTION_CALL_SUCCESS = 0
}local_retno_t;

#ifdef __cplusplus
}
#endif

#endif /* RADAR_ERROR_H */
