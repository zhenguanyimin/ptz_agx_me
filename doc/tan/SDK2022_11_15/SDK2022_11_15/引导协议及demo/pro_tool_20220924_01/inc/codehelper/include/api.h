#ifndef __HELPER_API_H__
#define __HELPER_API_H__

#if _HAS_CXX17
#pragma warning(disable:4996)
#endif // _HAS_CXX17
#include <codehelper/include/general.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <time.h>

/**
* @brief 常用函数
*/

HELPER_BEGIN_NAMESPACE(HELPER)
/// <summary>
/// 函数性能调试
/// </summary>
class performance {
public:
	explicit performance(std::function<void(int ms)> cb)
		: cb_(cb), ts_(GetTickCount64()) {}
	~performance(){
		if (cb_) {
			cb_(static_cast<int>(GetTickCount64() - ts_));
		}
	}
private:
	std::function<void(int ms)> cb_;
	uint64_t ts_ = 0;
};
/// <summary>
/// 内存初始化
/// </summary>
/// <param name="p"></param>
/// <param name="n"></param>
inline void memZero(void* p, size_t n){
  memset(p, 0, n);
}

/// <summary>
/// 赋值
/// </summary>
/// <typeparam name="O"></typeparam>
/// <typeparam name="V"></typeparam>
/// <param name="o"></param>
/// <param name="v"></param>
/// <returns></returns>
template<class O, class V>
uint32_t Value(O& o, V v)
{
	o = v;
	return sizeof(O);
}

/// <summary>
/// 打印日志
/// </summary>
/// <param name="flag"></param>
/// <param name="file"></param>
/// <param name="func"></param>
/// <param name="line"></param>
/// <param name="fmt"></param>
/// <param name=""></param>
void dbgview_info(const char* flag, const char* file, const char* func, int line, const char* fmt, ...);
void dbgview_hexinfo(const char* buf, int length);

/// <summary>
/// 随机数
/// default_random_engine
/// </summary>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="seed"></param>
/// <returns></returns>
int Random(int start, int end, unsigned int seed = time(NULL));

/// <summary>
/// 路径操作
/// 获取程序执行路径
/// </summary>
char* GetCurrentPath(HINSTANCE hInstance = NULL);
/// <summary>
/// 路径是否存在
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
bool IsPathExists(const char* path);

/// <summary>
/// 取子串
/// </summary>
/// <param name="str"></param>
/// <param name="s">不包含</param>
/// <param name="e"></param>
/// <returns></returns>
std::string subString(const std::string& str, char s, char e);
/// <summary>
/// 去除前后空格
/// </summary>
/// <param name="str"></param>
std::string trimString(const std::string& str);

/// <summary>
/// 字符串字符替换
/// </summary>
std::string replace(std::string& str, char s, char d);
/// <summary>
/// to小写
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
int StringtoLower(char* s);
/// <summary>
/// to大写
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
int StringtoUpper(char* s);

/// <summary>
/// 判断字符串是否为数字或者字母
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
bool isalnum_wrap(const char* str);
/// <summary>
/// 判断字符串是否为字母
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
bool isalpha_wrap(const char* str);
/// <summary>
/// 根据分隔符查找子串
/// 注意 ,输出的,子串有可以是空字符串, 比如 x...x ,,查找 分隔符位 'x'的子串
/// </summary>
/// <param name="src">源字符串</param>
/// <param name="delims"></param>
/// <param name="index">第几个分隔符.从0开始.</param>
/// <param name="buff"></param>
/// <param name="bufflen"></param>
/// <param name="bMutil">多分隔符</param>
void StrTok(const char* src, const char* delims, int index, char* buff, int bufflen, bool bMutil = true);
void StrTok(const char* src, const char* delims, std::vector<std::string>& vStr, bool bMutil = true);

/// <summary>
/// 字符串ip地址转整型值
/// ipv4
/// </summary>
/// <param name="ip"></param>
/// <returns></returns>
uint32_t iptolong(const char* ip);
/// <summary>
/// 整型值IP地址形式转字符串
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
std::string longtoiip(uint32_t v);

/// <summary>
/// 保留多少位精度
/// </summary>
/// <param name="in"></param>
/// <param name="accuracy"></param>
/// <returns></returns>
double ConvertAccuracy(double in, unsigned int accuracy);

/// <summary>
/// 字符集转换
/// codecvt
/// </summary>
int ChsToWideChar(const std::string& str, std::wstring& wstr);
int ChsToMultiByte(const std::wstring& wstr, std::string& str);
int Utf8ToWideChar(const std::string& src, std::wstring& des);
int WideCharToUtf8(const std::wstring& src, std::string& des);
int ChsToUtf8(const std::string& src, std::string& des);
int Utf8ToChs(const std::string& src, std::string& des);


/// <summary>
/// 时间戳
/// </summary>

/// <summary>
/// 将时间转换为自1970年1月1日以来持续时间的秒数
/// </summary>
/// <param name="y"></param>
/// <param name="m"></param>
/// <param name="d"></param>
/// <param name="h"></param>
/// <param name="mm"></param>
/// <param name="s"></param>
/// <returns>发生错误时返回-1</returns>
uint64_t GetMKTime(int y, int m, int d, int h, int mm, int s);
/// <summary>
/// 获取本地当前时间自1970年1月1日以来持续时间的秒数
/// </summary>
/// <returns></returns>
uint64_t GetCurrentMKTime();
/// <summary>
/// 将时间转换为自1970年1月1日以来持续时间的秒数
/// UTC 世界标准时间
/// </summary>
/// <param name="y"></param>
/// <param name="m"></param>
/// <param name="d"></param>
/// <param name="h"></param>
/// <param name="mm"></param>
/// <param name="s"></param>
/// <returns></returns>
uint64_t GetUTCMKTime(int y, int m, int d, int h, int mm, int s);
/// <summary>
/// 获取当前时间自1970年1月1日以来持续时间的秒数
/// UTC 世界标准时间
/// </summary>
/// <returns></returns>
uint64_t GetCurrentUTCMKTime();

/// <summary>
/// 转换时间
/// </summary>
/// <param name="v">自1970年1月1日以来持续时间的秒数</param>
/// <param name="y"></param>
/// <param name="m"></param>
/// <param name="d"></param>
/// <param name="h"></param>
/// <param name="mm"></param>
/// <param name="s"></param>
/// <returns></returns>
int ConvertTime(time_t v, int& y, int& m, int& d, int& h, int& mm, int& s);
/// <summary>
/// 转换时间
/// </summary>
/// <param name="v">自1970年1月1日以来持续时间的毫秒<</param>
/// <param name="y"></param>
/// <param name="m"></param>
/// <param name="d"></param>
/// <param name="h"></param>
/// <param name="mm"></param>
/// <param name="s"></param>
/// <param name="ms"></param>
/// <returns></returns>
int ConvertMsTime(time_t v, int& y, int& m, int& d, int& h, int& mm, int& s, int& ms);

uint64_t GetCurrentMKTimeMS();
uint64_t GetCurrentUTCMKTimeMS();

/// <summary>
/// 进程
/// </summary>
/// 
typedef struct _PROCESS_PROPERTY {
	unsigned int uiProcessID;
	HANDLE hProcessHandle;
}PROCESS_PROPERTY, * PPROCESS_PROPERTY;
int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty = NULL, unsigned int uiTimeOut = 0, BOOL bShow = FALSE);

HELPER_END_NAMESPACE(HELPER)
#endif