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
* @brief ���ú���
*/

HELPER_BEGIN_NAMESPACE(HELPER)
/// <summary>
/// �������ܵ���
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
/// �ڴ��ʼ��
/// </summary>
/// <param name="p"></param>
/// <param name="n"></param>
inline void memZero(void* p, size_t n){
  memset(p, 0, n);
}

/// <summary>
/// ��ֵ
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
/// ��ӡ��־
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
/// �����
/// default_random_engine
/// </summary>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="seed"></param>
/// <returns></returns>
int Random(int start, int end, unsigned int seed = time(NULL));

/// <summary>
/// ·������
/// ��ȡ����ִ��·��
/// </summary>
char* GetCurrentPath(HINSTANCE hInstance = NULL);
/// <summary>
/// ·���Ƿ����
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
bool IsPathExists(const char* path);

/// <summary>
/// ȡ�Ӵ�
/// </summary>
/// <param name="str"></param>
/// <param name="s">������</param>
/// <param name="e"></param>
/// <returns></returns>
std::string subString(const std::string& str, char s, char e);
/// <summary>
/// ȥ��ǰ��ո�
/// </summary>
/// <param name="str"></param>
std::string trimString(const std::string& str);

/// <summary>
/// �ַ����ַ��滻
/// </summary>
std::string replace(std::string& str, char s, char d);
/// <summary>
/// toСд
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
int StringtoLower(char* s);
/// <summary>
/// to��д
/// </summary>
/// <param name="s"></param>
/// <returns></returns>
int StringtoUpper(char* s);

/// <summary>
/// �ж��ַ����Ƿ�Ϊ���ֻ�����ĸ
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
bool isalnum_wrap(const char* str);
/// <summary>
/// �ж��ַ����Ƿ�Ϊ��ĸ
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
bool isalpha_wrap(const char* str);
/// <summary>
/// ���ݷָ��������Ӵ�
/// ע�� ,�����,�Ӵ��п����ǿ��ַ���, ���� x...x ,,���� �ָ���λ 'x'���Ӵ�
/// </summary>
/// <param name="src">Դ�ַ���</param>
/// <param name="delims"></param>
/// <param name="index">�ڼ����ָ���.��0��ʼ.</param>
/// <param name="buff"></param>
/// <param name="bufflen"></param>
/// <param name="bMutil">��ָ���</param>
void StrTok(const char* src, const char* delims, int index, char* buff, int bufflen, bool bMutil = true);
void StrTok(const char* src, const char* delims, std::vector<std::string>& vStr, bool bMutil = true);

/// <summary>
/// �ַ���ip��ַת����ֵ
/// ipv4
/// </summary>
/// <param name="ip"></param>
/// <returns></returns>
uint32_t iptolong(const char* ip);
/// <summary>
/// ����ֵIP��ַ��ʽת�ַ���
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
std::string longtoiip(uint32_t v);

/// <summary>
/// ��������λ����
/// </summary>
/// <param name="in"></param>
/// <param name="accuracy"></param>
/// <returns></returns>
double ConvertAccuracy(double in, unsigned int accuracy);

/// <summary>
/// �ַ���ת��
/// codecvt
/// </summary>
int ChsToWideChar(const std::string& str, std::wstring& wstr);
int ChsToMultiByte(const std::wstring& wstr, std::string& str);
int Utf8ToWideChar(const std::string& src, std::wstring& des);
int WideCharToUtf8(const std::wstring& src, std::string& des);
int ChsToUtf8(const std::string& src, std::string& des);
int Utf8ToChs(const std::string& src, std::string& des);


/// <summary>
/// ʱ���
/// </summary>

/// <summary>
/// ��ʱ��ת��Ϊ��1970��1��1����������ʱ�������
/// </summary>
/// <param name="y"></param>
/// <param name="m"></param>
/// <param name="d"></param>
/// <param name="h"></param>
/// <param name="mm"></param>
/// <param name="s"></param>
/// <returns>��������ʱ����-1</returns>
uint64_t GetMKTime(int y, int m, int d, int h, int mm, int s);
/// <summary>
/// ��ȡ���ص�ǰʱ����1970��1��1����������ʱ�������
/// </summary>
/// <returns></returns>
uint64_t GetCurrentMKTime();
/// <summary>
/// ��ʱ��ת��Ϊ��1970��1��1����������ʱ�������
/// UTC �����׼ʱ��
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
/// ��ȡ��ǰʱ����1970��1��1����������ʱ�������
/// UTC �����׼ʱ��
/// </summary>
/// <returns></returns>
uint64_t GetCurrentUTCMKTime();

/// <summary>
/// ת��ʱ��
/// </summary>
/// <param name="v">��1970��1��1����������ʱ�������</param>
/// <param name="y"></param>
/// <param name="m"></param>
/// <param name="d"></param>
/// <param name="h"></param>
/// <param name="mm"></param>
/// <param name="s"></param>
/// <returns></returns>
int ConvertTime(time_t v, int& y, int& m, int& d, int& h, int& mm, int& s);
/// <summary>
/// ת��ʱ��
/// </summary>
/// <param name="v">��1970��1��1����������ʱ��ĺ���<</param>
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
/// ����
/// </summary>
/// 
typedef struct _PROCESS_PROPERTY {
	unsigned int uiProcessID;
	HANDLE hProcessHandle;
}PROCESS_PROPERTY, * PPROCESS_PROPERTY;
int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty = NULL, unsigned int uiTimeOut = 0, BOOL bShow = FALSE);

HELPER_END_NAMESPACE(HELPER)
#endif