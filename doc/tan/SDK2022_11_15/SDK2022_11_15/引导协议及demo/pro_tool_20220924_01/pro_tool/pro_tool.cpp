// pro_tool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include <sstream>
#include <limits>
#include <thread>
#include <string>
#include <WS2tcpip.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <spdlog\logger.hpp>
#include "CameraUavProtocal.h"
#include <codehelper/include/buffer.h>

void Output(const char*);
void OutputError(const char*);
void PrintHex(BYTE* lpBuff, int len);
void SetColor(unsigned short forecolor = 4, unsigned short backgroudcolor = 0)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, forecolor | backgroudcolor);
}

std::atomic_flag g_flag_init_net = ATOMIC_FLAG_INIT;
void _InitSocket() {
	if (!g_flag_init_net.test_and_set()) {
		WSADATA wsa;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
		if (ret != 0) {
			g_flag_init_net.clear();
		}
	}
}

SOCKET OpenSocket(unsigned short port);
int Write(const char* lpBuff, int iBuffLen);

std::string gip;
int port = 0;
SOCKET skt = INVALID_SOCKET;
SOCKET gSendSkt = INVALID_SOCKET;
bool quit_ = false;
bool enable_recv_ = false;
std::thread gRecvThread;
int gOutputStateFlag = 0;

void cmd_1_wrap();
void cmd_2_wrap();
void cmd_3_wrap();
void cmd_4_wrap();
void cmd_6_wrap();
void cmd_5_wrap();
void cmd_7_wrap();
void cmd_8_wrap();
void cmd_8_1_wrap();
void cmd_8_2_wrap();
void cmd_9_wrap();
void cmd_9_1_wrap();
void cmd_9_2_wrap();
void cmd_10_wrap();
void cmd_11_wrap();
void cmd_12_wrap();
void cmd_13_wrap();
void cmd_14_wrap();
void cmd_15_wrap();
void cmd_16_wrap();
void cmd_17_wrap();
void cmd_18_wrap();
void recv_data(const char*, int len);

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "codehelper/codehelperd.lib")
#else
#pragma comment(lib, "codehelper/codehelperd_x32.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib, "codehelper/codehelper.lib")
#else
#pragma comment(lib, "codehelper/codehelper_x32.lib")
#endif
#endif

int main()
{
	SPDLOG_HELPER_NS::spdlog_load_default("pro_tool", SPDLOG_HELPER_NS::SPDLOG_SINK_STDOUT, "", true, "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");

#if 0
	{
		gOutputStateFlag = 1;
		BYTE pro[] = { 0x88,0x89,0x80,0x8a,0x2a,0x23,0x00,0x00,0x24,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x5d,0x93,0x94,0x7a,0x80,0x01,0x00,0x00,0x5f,0x5f,0xb2,0xa0,0xb2,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x00,0x24,0xf5,0xf5,0x50,0x19,0x18,0x00,0x1b,0x08,0x00,0x00,0x89,0x80,0x8a,0x8b };
		recv_data((const char*)pro, sizeof(pro));
	}
#endif

#if 0
	{
		gOutputStateFlag = 1;
		BYTE pro[] = { 0x88,0x89,0x80,0x8A,0x2A,0x23,0x00,0x00,0x5C,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0xF0,0x44,0x53,0x2B,0x98,0x60,0x40,0xFE,0xB1,0x93,0xB9,0xC6,0x6A,0x10,0xC0,0xDA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x89,0x80,0x8A,0x8B };
		recv_data((const char*)pro,sizeof(pro));
	}

	{
		HELPER::CFixBuffer buf;

		UAV_PRO::CTargetDirectionProtocal major;

		major.setLng(118.554066);
		major.setLat(24.799684);
		major.setHeight(100);
		//major.setHeight(5);

		UAV_PRO::CCameraUavProtocal pro(major);
		int len = pro.construct(buf);
		if (len > 0)
		{
			PrintHex((BYTE*)buf.peek(), buf.readableBytes());
		}
	}
#endif
	_InitSocket();
	std::cout << std::endl;
	Output("输入光电ip地址:");
	std::cin >> gip;
	Output("输入光电端口(9966):");
	std::cin >> port;

	std::cout << "光电信息: [" << gip.data() << ":" << port << "]" << std::endl;

	int flag = 0, iCmdLen = 0;
	std::cout << std::endl;
	Output("命令 \r\n\
	1:引导 \r\n\
	2:搜索并自动跟踪 \r\n\
	3:释放目标 \r\n\
	4:跟踪指定目标 \r\n\
	5.仅搜索 \r\n\
	6:接收光电状态 \r\n\
	7:光电镜头控制 \r\n\
	8:干扰器 \r\n\
	9:光电搜索扩展包 \r\n\
	10:引导(经纬度) \r\n\
	11:转台方位控制 \r\n\
	12:切换视频跟踪源 \r\n\
	13:手动锁定目标 \r\n\
	14:外围设备控制 \r\n\
	15:AI 参数控制 \r\n\
	16:聚焦模式 \r\n\
	17:跟踪参数 \r\n\
	18:引导(扩展) \r\n\
	0:退出");
	std::cin >> flag;

	while (flag != 0)
	{
		if (flag == 1)
		{
			cmd_1_wrap();
		}
		else if (flag == 2)
		{
			cmd_2_wrap();
		}
		else if (flag == 3)
		{
			cmd_3_wrap();
		}
		else if (flag == 4)
		{
			cmd_4_wrap();
		}
		else if (flag == 5)
		{
			cmd_5_wrap();
		}
		else if (flag == 6)
		{
			//Output("在 dbgview.exe 中查看信息..");
			int state;
			Output("命令 \r\n\
	1:输出【所有】 \r\n\
	2:输出【状态信息包】\r\n\
	3:输出【方位、俯仰信息包】 \r\n\
	4:输出【状态扩展信息包】 \r\n\
	5:输出【干扰状态】\r\n\
	6:输出【光电目标上报信息包 0x0B】\r\n\
	7:输出【光电镜头状态扩展信息包 0x0C】\r\n\
	8:输出【脱靶量上报信息包 0x0F】\r\n\
	9:输出【系统状态扩展信息包 0x15");
			std::cin >> gOutputStateFlag;

			if (!enable_recv_)
			{
				enable_recv_ = true;
				cmd_6_wrap();
			}
		}
		else if (flag == 7)
		{
			cmd_7_wrap();
		}
		else if (flag == 8)
		{
			cmd_8_wrap();
		}
		else if (flag == 9)
		{
			cmd_9_wrap();
		}
		else if (flag == 10)
		{
			cmd_10_wrap();
		}
		else if (flag == 11)
		{
			cmd_11_wrap();
		}
		else if (flag == 12)
		{
			cmd_12_wrap();
		}
		else if (flag == 13)
		{
			cmd_13_wrap();
		}
		else if (flag == 14)
		{
			cmd_14_wrap();
		}
		else if (flag == 15)
		{
			cmd_15_wrap();
		}
		else if (flag == 16)
		{
			cmd_16_wrap();
		}
		else if (flag == 17)
		{
			cmd_17_wrap();
		}
		else if (flag == 18)
		{
			cmd_18_wrap();
		}

		std::cout << std::endl;
		Output("命令 \r\n\
	1:引导 \r\n\
	2:搜索并自动跟踪 \r\n\
	3:释放目标 \r\n\
	4:跟踪指定目标 \r\n\
	5.仅搜索 \r\n\
	6:接收光电状态 \r\n\
	7:光电镜头控制 \r\n\
	8:干扰器 \r\n\
	9:光电搜索扩展包 \r\n\
	10:引导(经纬度) \r\n\
	11:转台方位控制 \r\n\
	12:切换视频跟踪源 \r\n\
	13:手动锁定目标 \r\n\
	14:外围设备控制 \r\n\
	15:AI 参数控制 \r\n\
	16:聚焦模式 \r\n\
	17:跟踪参数 \r\n\
	18:引导(扩展) \r\n\
	0:退出");
		std::cin >> flag;
	}
	quit_ = true;

	if (gRecvThread.joinable())
	{
		gRecvThread.join();
	}
	return 0;
}

void cmd_1_wrap()
{
	int dev_id = 0;
	double fHor = 0, fVer = 0;
	int iDis = 0;
	std::cout << std::endl;
	Output("输入水平角度:");
	std::cin >> fHor;
	Output("输入垂直角度:");
	std::cin >> fVer;
	Output("输入距离:");
	std::cin >> iDis;

	HELPER::CFixBuffer buf;

	UAV_PRO::CTargetDirectionProtocal major;

	major.setDistance(iDis * 1.0);
	major.setVerAngle(fVer);
	major.setHorAngle(fHor);
	//major.setHeight(5);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "引导命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "引导命令发送失败";
	}
}

void cmd_2_wrap()
{
	int number = 0, cmd = UAV_PRO::CCameraCtrlProtocal::SEARCH;
	float angle = 0.0;
	std::cout << std::endl;
	Output("命令:\r\n\
1.当前视场\r\n\
2.给定最小视场,水平搜索跟踪\r\n\
3.给定最大视场,垂直搜搜跟踪");
	std::cin >> cmd;

	if ((cmd == 2) || (cmd == 3)) {
		if (cmd == 2) {
			cmd = UAV_PRO::CCameraCtrlProtocal::HOR_SEARCH_TRACK;
		}
		else {
			cmd = UAV_PRO::CCameraCtrlProtocal::VER_SEARCH_TRACK;
		}
		Output("最小视场角度:");
		std::cin >> angle;
	}
	else {
		Output("搜索方式:");
		std::cin >> number;
	}

	HELPER::CFixBuffer buf;
	UAV_PRO::CCameraCtrlProtocal major;
	major.setCmd(cmd);
	major.setSearchMode(number);

	if (cmd == UAV_PRO::CCameraCtrlProtocal::HOR_SEARCH_TRACK) {
		major.setHorStartAngle(static_cast<int>(angle * 100));
	}
	else if (cmd == UAV_PRO::CCameraCtrlProtocal::VER_SEARCH_TRACK) {
		major.setVerStartAngle(static_cast<int>(angle * 100));
	}

	if (cmd == UAV_PRO::CCameraCtrlProtocal::SEARCH) {
		if ((number == 11) || (number == 12)) {

			/// 水平开始角度
			double hor_start_angle = 0;
			/// 水平结束角度
			double hor_end_angle = 0;
			/// 俯仰开始角度
			double ver_start_angle = 0;
			/// 俯仰结束角度
			double ver_end_angle = 0;

			std::cout << std::endl;
			Output("输入水平开始角度:");
			std::cin >> hor_start_angle;
			std::cout << std::endl;
			Output("输入水平结束角度:");
			std::cin >> hor_end_angle;
			std::cout << std::endl;
			Output("输入俯仰开始角度:");
			std::cin >> ver_start_angle;
			std::cout << std::endl;
			Output("输入俯仰结束角度:");
			std::cin >> ver_end_angle;

			if (12 == number) {

			}

			major.setHorStartAngle(hor_start_angle);
			major.setHorEndAngle(hor_end_angle);
			major.setVerStartAngle(ver_start_angle);
			major.setVerEndAngle(ver_end_angle);
		}
	}


	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "搜索目标命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "搜索目标命令发送失败";
	}
}

void cmd_3_wrap()
{
	HELPER::CFixBuffer buf;
	UAV_PRO::CCameraCtrlProtocal major;
	major.setCmd(UAV_PRO::CCameraCtrlProtocal::FREE);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "释放目标命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "释放目标命令发送失败";
	}
}

void cmd_4_wrap()
{
	int number = 0;
	std::cout << std::endl;
	Output("输入目标编号:");
	std::cin >> number;

	HELPER::CFixBuffer buf;
	UAV_PRO::CCameraCtrlProtocal major;
	major.setCmd(UAV_PRO::CCameraCtrlProtocal::TRACK);
	major.setTargetNumber(number);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "跟踪命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "跟踪命令发送失败";
	}
}

void cmd_5_wrap()
{
	int number = 0;
	std::cout << std::endl;
	Output("搜索方式:");
	std::cin >> number;

	HELPER::CFixBuffer buf;
	UAV_PRO::CCameraCtrlProtocal major;
	major.setCmd(UAV_PRO::CCameraCtrlProtocal::ONLY_SEARCH);
	major.setSearchMode(number);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_7_wrap()
{
	int cmd = 0, zoom = 0, zoom_step = 0, focus_step = 0, reserved =0;
	std::cout << std::endl;
	Output("命令: 0.镜头停止运动(0x00) \r\n\
1. 跳转到指定物理焦距 \r\n\
2. 跳转到指定倍数 \r\n\
3. 跳转到指定zoom和focus位置 \r\n\
4. 镜头持续推远(0x04) \r\n\
5. 镜头持续拉近(0x05) \r\n\
6. 镜头点动推远(0x06) \r\n\
7. 镜头点动拉近(0x07) \r\n\
8. 焦点持续推远(0x08) \r\n\
9. 焦点持续拉近(0x09) \r\n\
10. 焦点点动推远(0x0A) \r\n\
11. 焦点点动拉近(0x0B)");
	std::cin >> cmd;
	if ((1 == cmd) || (2 == cmd))
	{
		Output("物理焦距/倍数: (命令为 1 时表示镜头的物理焦距, 单位mm 如150mm；命令为 2 时表示镜头倍数，如42倍)");
		std::cin >> zoom;
	}
	else if (3 == cmd)
	{
		Output("zoom 步数: ");
		std::cin >> zoom_step;
		Output("Focus 步数: ");
		std::cin >> focus_step;
	}
	else if (cmd >= 4 && cmd <= 11) {
		Output("镜头控制速度: ");
		std::cin >> zoom;
	}
	Output("控制通道号: 0-可见光 1-热像 ");
	std::cin >> reserved;

	HELPER::CFixBuffer buf;
	UAV_PRO::CCameraLensInfoProtocal major;
	major.setCmd(cmd);
	major.setZoom(zoom);
	major.setZoomStep(zoom_step);
	major.setFocusStep(focus_step);
	major.setReserved(reserved);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_6_wrap()
{
	gRecvThread = std::thread([&]() {
		std::array<BYTE, 4096> buf;
		skt = OpenSocket(port);
		if (skt == INVALID_SOCKET)
		{
			std::cout << "监听端口失败" << std::endl;
			return;
		}
		struct sockaddr_in from;
		int ret = 0, add_len = sizeof(struct sockaddr_in);
		int proc_len = 0;
		int has = 0;
		char ip[32] = {0};
		while (!quit_)
		{
			if (enable_recv_)
			{
				has = 0;
				ret = ::recvfrom(skt, (char*)buf.data(), buf.max_size(), 0, (struct sockaddr*)&from, &add_len);
				if (ret > 0)
				{
					has = 1;
					::inet_ntop(AF_INET, (void*)&from.sin_addr, ip, 32);
					if (!strcmp(gip.data(), ip)) 
					{
						recv_data((const char*)buf.data(), ret);
					}
				}
				if (has == 0) {
					Sleep(20);
				}
			}
		}
		});
}

void cmd_8_wrap()
{
	int flag = 0;
	std::cout << std::endl;
	Output("命令 \r\n\
	1:控制 \r\n\
	2:查询 \r\n\
	0:退出");
	std::cin >> flag;

	while (flag != 0)
	{
		if (flag == 1)
		{
			cmd_8_1_wrap();
		}
		else if (flag == 2)
		{
			cmd_8_2_wrap();
		}

		std::cout << std::endl;
		Output("命令 \r\n\
	1:控制 \r\n\
	2:查询 \r\n\
	0:退出");
		std::cin >> flag;
	}
	return;
}


int CtrlFrep(int start, int stop, int channel, bool open)
{
	HELPER::CFixBuffer buf;
	UAV_PRO::CCtrlDisturberProtocal disturber;
	disturber.setState(open ? static_cast<int>(UAV_PRO::CDisturberProtocal::STATE_OPEN) : static_cast<int>(UAV_PRO::CDisturberProtocal::STATE_CLOSE));
	disturber.setStartFrequency(start);
	disturber.setEndFrequency(stop);

	UAV_PRO::CDisturberProtocal major(disturber);
	major.setChannel(channel);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		SPDLOG_DEBUG("协议命令构造失败");
		return -1;
	}
	int ret = Write((const char*)buf.peek(), len);
	PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	return ret == 0 ? 0 : -3;
}

void cmd_8_1_wrap()
{
	int order = 0;
	std::cout << std::endl;
	Output("1:全开(全部控制通道) 2:全关(全部控制通道) 3:指定频段 4.模式(驱离&迫降) 5.写频");
	std::cin >> order;

	int channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_9;
	int state = 0;
	if (3 == order)
	{
		int fp = 0;
		std::cout << std::endl;
		Output("频段: 1:350M 2:430M 3:850M 4:1200M 5:2400M 6:5800M 7:GPS");
		std::cin >> fp;
		std::cout << std::endl;
		Output("1:打开 2:关闭");
		std::cin >> state;
		if (1 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_2;
		}
		else if (2 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_3;
		}
		else if (3 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_4;
		}
		else if (4 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_5;
		}
		else if (5 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_6;
		}
		else if (6 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_7;
		}
		else if (7 == fp)
		{
			channel = UAV_PRO::CDisturberProtocal::CHANNEL_FIX_1;
		}
		else {

		}
		if (state == 1)
		{
			state = UAV_PRO::CDisturberProtocal::STATE_OPEN;
		}
		else {
			state = UAV_PRO::CDisturberProtocal::STATE_CLOSE;
		}
	}
	else if (4 == order)
	{
		int mode = 0;
		std::cout << std::endl;
		Output("模式: 1:驱离 2:迫降");
		std::cin >> mode;

		std::cout << std::endl;
		Output("1:打开 2:关闭");
		std::cin >> state;

		if (state == 1){
			state = UAV_PRO::CDisturberProtocal::STATE_OPEN;
		}
		else {
			state = UAV_PRO::CDisturberProtocal::STATE_CLOSE;
		}

		if (mode == 1)
		{
			{
				HELPER::CFixBuffer buf;
				UAV_PRO::CCtrlDisturberProtocal disturber;
				disturber.setState(state);

				UAV_PRO::CDisturberProtocal major(disturber);
				major.setChannel(UAV_PRO::CDisturberProtocal::CHANNEL_FIX_6); /// 2400M


				UAV_PRO::CCameraUavProtocal pro(major);
				int len = pro.construct(buf);
				if (len <= 0)
				{
					OutputError("错误");
					return;
				}
				Write((const char*)buf.peek(), buf.readableBytes());
				PrintHex((BYTE*)buf.peek(), buf.readableBytes());
			}
			std::cout << std::endl;

			{
				HELPER::CFixBuffer buf;
				UAV_PRO::CCtrlDisturberProtocal disturber;
				disturber.setState(state);

				UAV_PRO::CDisturberProtocal major(disturber);
				major.setChannel(UAV_PRO::CDisturberProtocal::CHANNEL_FIX_7); /// 5800M

				UAV_PRO::CCameraUavProtocal pro(major);
				int len = pro.construct(buf);
				if (len <= 0)
				{
					OutputError("错误");
					return;
				}
				Write((const char*)buf.peek(), buf.readableBytes());
				PrintHex((BYTE*)buf.peek(), buf.readableBytes());
			}
		}
		else if (mode == 2)
		{
			{
				HELPER::CFixBuffer buf;
				UAV_PRO::CCtrlDisturberProtocal disturber;
				disturber.setState(state);
				UAV_PRO::CDisturberProtocal major(disturber);
				major.setChannel(UAV_PRO::CDisturberProtocal::CHANNEL_FIX_1); /// GPS/BD
				UAV_PRO::CCameraUavProtocal pro(major);
				int len = pro.construct(buf);
				if (len <= 0)
				{
					OutputError("错误");
					return;
				}
				Write((const char*)buf.peek(), buf.readableBytes());
				PrintHex((BYTE*)buf.peek(), buf.readableBytes());
			}
			std::cout << std::endl;

			{
				HELPER::CFixBuffer buf;
				UAV_PRO::CCtrlDisturberProtocal disturber;
				disturber.setState(state);
				UAV_PRO::CDisturberProtocal major(disturber);
				major.setChannel(UAV_PRO::CDisturberProtocal::CHANNEL_FIX_6); /// 2400M
				UAV_PRO::CCameraUavProtocal pro(major);
				int len = pro.construct(buf);
				if (len <= 0)
				{
					OutputError("错误");
					return;
				}
				Write((const char*)buf.peek(), buf.readableBytes());
				PrintHex((BYTE*)buf.peek(), buf.readableBytes());
			}
			std::cout << std::endl;
			{
				HELPER::CFixBuffer buf;
				UAV_PRO::CCtrlDisturberProtocal disturber;
				disturber.setState(state);
				UAV_PRO::CDisturberProtocal major(disturber);
				major.setChannel(UAV_PRO::CDisturberProtocal::CHANNEL_FIX_7); /// 5800M
				UAV_PRO::CCameraUavProtocal pro(major);
				int len = pro.construct(buf);
				if (len <= 0)
				{
					OutputError("错误");
					return;
				}
				Write((const char*)buf.peek(), buf.readableBytes());
				PrintHex((BYTE*)buf.peek(), buf.readableBytes());
			}
		}
		return;
	}
	else if (5 == order) {
		int cmd = 0, mode=0;
		std::cout << std::endl;
		Output("模式: 1.模块1(840-2600MHZ) 2.模块2(2600-6000MHZ) 3.模块3(1570-1620MHZ) \
4.4G(4800-4900) 5.5G(5725-5850) 6.2G(2400-2500) 7.GPS(1520-1620) 8.驱离 9.迫降 10.关闭");
		std::cin >> cmd;

		if (cmd == 1) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_1;
		}
		else if(cmd == 2) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2;
		}
		else if (cmd == 3) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_3;
		}
		else if (cmd == 4) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2;
		}
		else if (cmd == 5) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2;
		}
		else if (cmd == 6) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_1;
		}
		else if (cmd == 7) {
			mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_3;
		}
		else if (cmd == 8) {
			CtrlFrep(5725, 5850, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2), true);
			CtrlFrep(2400, 2500, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_1), true);
			return;
		}
		else if (cmd == 9) {
			CtrlFrep(5725, 5850, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2), true);
			CtrlFrep(2400, 2500, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_1), true);
			CtrlFrep(1520, 1620, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_3), true);
			return;
		}
		else if (cmd == 10) {
			CtrlFrep(5725, 5850, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2), false);
			CtrlFrep(2400, 2500, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_1), false);
			CtrlFrep(1520, 1620, static_cast<int>(UAV_PRO::CDisturberProtocal::CHANNEL_VAR_3), false);
			return;
		}
		else {
			OutputError("错误");
			return;
		}

		int start_fp = 0, stop_fp = 0;
		if (cmd == 1 || cmd == 2 || cmd ==3) {
			std::cout << std::endl;
			Output("开始频率（MHZ）: ");
			std::cin >> start_fp;

			std::cout << std::endl;
			Output("结束频率（减去开始频率小于等于125）: ");
			std::cin >> stop_fp;
		}
		else if (cmd == 4) { //4G
			start_fp = 4800;
			stop_fp = 4900;
		}
		else if (cmd == 5) { // 5G
			start_fp = 5725;
			stop_fp = 5850;
		}
		else if (cmd == 6) { // 2G
			start_fp = 2400;
			stop_fp = 2500;
		}
		else if (cmd == 7) { // GPS
			start_fp = 1520;
			stop_fp = 1620;
		}

		std::cout << std::endl;
		Output("1:打开 2:关闭");
		std::cin >> state;

		if (state == 1) {
			state = UAV_PRO::CDisturberProtocal::STATE_OPEN;
		}
		else {
			state = UAV_PRO::CDisturberProtocal::STATE_CLOSE;
		}

		HELPER::CFixBuffer buf;
		UAV_PRO::CCtrlDisturberProtocal disturber;
		disturber.setState(state);
		disturber.setStartFrequency(start_fp);
		disturber.setEndFrequency(stop_fp);

		UAV_PRO::CDisturberProtocal major(disturber);
		major.setChannel(mode); 

		UAV_PRO::CCameraUavProtocal pro(major);
		int len = pro.construct(buf);
		if (len <= 0)
		{
			OutputError("错误");
			return;
		}
		Write((const char*)buf.peek(), buf.readableBytes());
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
		return;
	}
	else {
		if (order == 1)
		{
			state = UAV_PRO::CDisturberProtocal::STATE_OPEN;
		}
		else {
			state = UAV_PRO::CDisturberProtocal::STATE_CLOSE;
		}
	}

	HELPER::CFixBuffer buf;
	UAV_PRO::CCtrlDisturberProtocal disturber;
	disturber.setState(state);
	UAV_PRO::CDisturberProtocal major(disturber);
	major.setChannel(channel);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "干扰器控制命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "干扰器控制命令发送失败";
	}
}

void cmd_8_2_wrap()
{
	int mode = 0;
	std::cout << std::endl;
	Output("模式: 1.模块1(840-2600MHZ) 2.模块2(2600-6000MHZ) 3.模块3(1570-1620MHZ) 4.所有");
	std::cin >> mode;

	if (mode == 1) {
		mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_1;
	}
	else if (mode == 2) {
		mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_2;
	}
	else if (mode == 3) {
		mode = UAV_PRO::CDisturberProtocal::CHANNEL_VAR_3;
	}
	else {
		mode = 0XB2;
	}

	unsigned short startfp = 0;
	unsigned short stopfp = 0;
	HELPER::CFixBuffer buf;
	UAV_PRO::CQueryDisturberProtocal disturber;
	disturber.setState(UAV_PRO::CDisturberProtocal::STATE_OPEN);
	
	if (1/*(1 == mode) || (2 == mode) || (3 == mode)*/) {
		std::cout << std::endl;
		Output("开始频率（MHZ）: ");
		std::cin >> startfp;

		std::cout << std::endl;
		Output("结束频率（减去开始频率小于等于125）: ");
		std::cin >> stopfp;
	}
	disturber.setStartFrequency(startfp);
	disturber.setEndFrequency(stopfp);

	UAV_PRO::CDisturberProtocal major(disturber);
	major.setChannel(mode);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "干扰器查询命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "干扰器查询命令发送失败";
	}
}

void cmd_9_wrap()
{
	int cmd = 0;
	std::cout << std::endl;
	Output("命令: 1.启动扫描 2.停止扫描");
	std::cin >> cmd;
	if (1 == cmd)
	{
		cmd_9_1_wrap();
	}
	else if (2 == cmd)
	{
		cmd_9_2_wrap();
	}
}

void cmd_9_1_wrap()
{
	int hor_start = 0, hor_stop = 0, ver_start = 0, ver_stop = 0, speed = 0;
	std::cout << std::endl;
	Output("请输入水平开始角度:");
	std::cin >> hor_start;
	Output("请输入水平结束角度:");
	std::cin >> hor_stop;
	Output("请输入垂直开始角度:  注意：（如果俯仰开始和结束角度相等，光电将在指定的俯仰位置上持续扫描，如果不相等，则做螺旋式持续扫描）");
	std::cin >> ver_start;
	Output("请输入垂直结束角度:");
	std::cin >> ver_stop;
	Output("请输入扫描速度: (度/秒)");
	std::cin >> speed;

	HELPER::CFixBuffer buf;
	UAV_PRO::CSearchExtendProtocal major;

	major.setCmd(1);
	major.setHorStartAngle(hor_start);
	major.setHorStopAngle(hor_stop);
	major.setVerStartAngle(ver_start);
	major.setVerStopAngle(ver_stop);
	major.setSpeed(speed);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_9_2_wrap()
{
	HELPER::CFixBuffer buf;
	UAV_PRO::CSearchExtendProtocal major;
	major.setCmd(0);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_11_wrap()
{
	int ptz_cmd = 0, ptz_speed=0, ptz_mode=0;
	std::cout << std::endl;
	Output("请输入命令: 0.停止 1.向左 2.向右 3.向上 4.向下 5.左上 6.左下 7.右上 8.右下");
	std::cin >> ptz_cmd;
	Output("请输入速度(0~254):");
	std::cin >> ptz_speed;
	Output("请输入模式: 0.持续运动（直到收到停止指令）1.单次运动(指令执行一次自动停止)");
	std::cin >> ptz_mode;

	HELPER::CFixBuffer buf;
	UAV_PRO::CCtrlPtz major;
	major.setCmd(ptz_cmd);
	major.setSpeed(ptz_speed);
	major.setMode(ptz_mode);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_12_wrap()
{
	int source = 0;
	std::cout << std::endl;
	Output("请输入: 0.可见光 1.热像");
	std::cin >> source;

	HELPER::CFixBuffer buf;
	UAV_PRO::CTrackSource major;
	major.setSource(source);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void toCameraPosition(int x, int y, int winWidth, int winHeight, POINT& pt)
{
	int iNewX = x, iNewY = y;
	iNewX = static_cast<int>((x * 1.0 / (winWidth)) * 704);
	iNewY = static_cast<int>((y * 1.0 / (winHeight)) * 576);
	pt.x = iNewX;
	pt.y = iNewY;
}

void cmd_13_wrap()
{
	int source = 0;
	int w = 0,h=0;
	int x = 0, y = 0;
	std::cout << std::endl;
	Output("请输入: 0.可见光 1.热像");
	std::cin >> source;
	Output("请输入视频窗口宽度: ");
	std::cin >> w;
	Output("请输入视频窗口高度: ");
	std::cin >> h;
	Output("请输入跟踪点的x坐标(相对于窗口): ");
	std::cin >> x;
	Output("请输入跟踪点的y坐标(相对于窗口): ");
	std::cin >> y;

	POINT p;
	toCameraPosition(x,y,w,h, p);

	HELPER::CFixBuffer buf;
	UAV_PRO::CManualTrackProtocal major;
	major.setChannel(source);
	major.setX(p.x);
	major.setY(p.y);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_14_wrap()
{
	int cmd = 0, state=0;
	std::cout << std::endl;
	Output("请输入控制命令: \r\n\
	1:控制激光测距器 \r\n\
	2:控制激光测照机 \r\n\
	3:控制激光补光 \r\n\
	4:控制加热器 \r\n\
	5:控制热成像压缩机");
	std::cin >> cmd;
	Output("控制参数: 1.开启 0.关闭");
	std::cin >> state;

	HELPER::CFixBuffer buf;
	UAV_PRO::CFDOCtrl major;
	major.setCmd(cmd);
	major.setParams(state);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_15_wrap()
{
	int cmd = 0, par = 0;
	std::cout << std::endl;
	Output("请输入控制命令: 1:AI 模板切换 2.退出");
	std::cin >> cmd;

	if (1 != cmd) {
		return;
	}

	Output("参数: 1: 选择AI模板1(对天 3 分类) \r\n\
2:选择 AI模板2(对地80分类)");
	std::cin >> par;

	HELPER::CFixBuffer buf;
	UAV_PRO::CAIModeCfgParam params;
	params.setTemplate(par);
	UAV_PRO::CAIModelCtrl major(params);
	major.setCmd(cmd);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_16_wrap()
{
	int cmd = 0,channel=0;
	std::cout << std::endl;
	Output("聚焦模式: 0-自动(AF)  1-半自动(ZF)  2-手动(MF)");
	std::cin >> cmd;

	Output("通道: 0-可见光  1-热像");
	std::cin >> channel;

	HELPER::CFixBuffer buf;
	UAV_PRO::CFocusMode major;
	major.setMode(cmd);
	major.setChannel(channel);
	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

void cmd_17_wrap()
{
	int cmd = 0, zoomin_threshold = 0, zoomout_threshold_=0;
	std::cout << std::endl;
	Output("自动变倍: 0-关闭  1-启用");
	std::cin >> cmd;

	if (cmd == 1) {
		Output("放大(ZoomIn)阈值(像素,默认40): ");
		std::cin >> zoomin_threshold;

		if (zoomin_threshold <= 0) {
			OutputError("错误");
			return;
		}

		Output("缩小(ZoomOut)阈值(像素,默认100): ");
		std::cin >> zoomout_threshold_;

		if (zoomout_threshold_ <= 0) {
			OutputError("错误");
			return;
		}
	}


	HELPER::CFixBuffer buf;
	UAV_PRO::CTrackCfgProtocal major;
	major.setAutoZoom(cmd);
	if (cmd == 1) {
		major.setZoominThreshold(zoomin_threshold);
		major.setZoomoutThreshold(zoomout_threshold_);
	}

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "命令发送失败";
	}
}

std::string getAIMode(int mode) {
	if (1 == mode) {
		return "对天(3分类)";
	}
	else if (2 == mode) {
		return "对地(80分类)";
	}
	return "";
}
std::string getStateType(int state) {
	if (0== state) {
		return "关闭";
	}
	else if (1 == state) {
		return "开启";
	}
	return "";
}
std::string getFocusMode(int mode) {
	if (0 == mode) {
		return "AF(自动)";
	}
	else if (1 == mode) {
		return "ZF(半自动)";
	}
	else if (2 == mode) {
		return "MF(手动)";
	}
	return "";
}

void recv_data(const char* data, int len)
{
	//char log[128];
	//_snprintf_s(log, _TRUNCATE,"%02x %02x %02x %02x", static_cast<uint8_t>(data[0]), static_cast<uint8_t>(data[1]), static_cast<uint8_t>(data[2]), static_cast<uint8_t>(data[3]));
	//OutputDebugString(log);
	//SPDLOG_DEBUG("数据大小:{} {:02X} {:02X} {:02X} {:02X}", len, static_cast<uint8_t>(data[0]), static_cast<uint8_t>(data[1]), static_cast<uint8_t>(data[2]), static_cast<uint8_t>(data[3]));
	HELPER::COnlyReadBuffer buf((void*)data, len);
	/*if (len > buf.Capacity()) {
		SPDLOG_DEBUG("数据大小错误");
		return;
	}
	buf.append(data, len);*/
	int pro_len = UAV_PRO::CCameraUavProtocal::parse(buf, [data,len](UAV_PRO::CCameraUavProtocal& pro_msg) {
		if (!pro_msg.valid())
		{
			SPDLOG_DEBUG("光电数据解析失败");
			return;
		}
		/// 如有耗时操作异步处理.
		int err = 0, debug_err = 0;
		int cmd_type = pro_msg.getCmd();
		if (UAV_PRO::UAV_CTRL_PRO_CMD_POSITION == cmd_type)
		{/// 光电设备方位、俯仰信息包
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 3))
			{
				UAV_PRO::CPositionProtocal& impl_msg = dynamic_cast<UAV_PRO::CPositionProtocal&>(pro_msg.getMajor());
				std::stringstream msg;
				msg.precision(2);
				msg << std::fixed;
				msg << "[光电设备方位、俯仰信息包]"
					<< " 编号:" << impl_msg.getNumber()
					<< " 距离:" << impl_msg.getDistance()
					<< " 水平角度:" << impl_msg.getHorAngle()
					<< " 垂直角度:" << impl_msg.getVerAngle()
					<< " 镜头倍率:" << static_cast<int>(impl_msg.getLensZoom());
				SPDLOG_DEBUG("{}", msg.str().data());
			}
		}
		else if (UAV_PRO::UAV_CTRL_PRO_CMD_STATUS == cmd_type) {
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 2))
			{
				UAV_PRO::CStateProtocal& impl_msg = dynamic_cast<UAV_PRO::CStateProtocal&>(pro_msg.getMajor());
				std::string work, mode;
				int tmp = impl_msg.getMode();
				if (0 == tmp) {
					mode = "空闲";
				}
				else if (1 == tmp) {
					mode = "搜索";
				}
				else if (2 == tmp) {
					mode = "跟踪";
				}
				else {
					mode = "未知";
				}
				tmp = impl_msg.getState();
				if (0 == tmp) {
					work = "异常";
				}
				else if (1 == tmp) {
					work = "正常";
				}
				else {
					work = "未知";
				}
				SPDLOG_DEBUG("[光电设备状态信息包] 当前工作状态:{} 当前工作模式:{}", work.data(), mode.data());
			}
		}
		else if (UAV_PRO::UAV_CTRL_PRO_CMD_STATUS_EXTEND == cmd_type) {
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 4))
			{
				UAV_PRO::CCameraStateExtendInfoProtocal& impl_msg = dynamic_cast<UAV_PRO::CCameraStateExtendInfoProtocal&>(pro_msg.getMajor());
				if (UAV_PRO::CCameraStateExtendInfoProtocal::CAMERA_STATE_IDLE == impl_msg.getState())
				{
					UAV_PRO::CCameraIdleStateProtocal& impl = dynamic_cast<UAV_PRO::CCameraIdleStateProtocal&>(impl_msg.getStateImpl());
					SPDLOG_DEBUG("[光电设备状态扩展信息包] 当前状态: 空闲, 持续时间:{}.", impl_msg.getTimestamp());
				}
				else if (UAV_PRO::CCameraStateExtendInfoProtocal::CAMERA_STATE_SEARCH == impl_msg.getState()) {
					UAV_PRO::CCameraSearchStateProtocal& impl = dynamic_cast<UAV_PRO::CCameraSearchStateProtocal&>(impl_msg.getStateImpl());
					SPDLOG_DEBUG("[光电设备状态扩展信息包] 当前状态: 搜索, 搜索到的目标数:{}", impl.getTargetCount());

					impl.getTargets([](UAV_PRO::CTarget tar) {
						std::stringstream msg;
						msg.precision(2);
						msg << std::fixed;
						msg << "[光电设备状态扩展信息包] "
							<< " 目标编号:" << tar.getNumber()
							<< " 类型:" << tar.getType()
							<< " 相识度:" << tar.getSame() * 1.0 / 100
							<< " 宽(像素):" << tar.getWidth()
							<< " 高(像素):" << tar.getHeight()
							<< " 水平角度:" << tar.getAngle()
							<< " 俯仰角度:" << tar.getElevation();
						SPDLOG_DEBUG("{}", msg.str().data());
						});
				}
				else if (UAV_PRO::CCameraStateExtendInfoProtocal::CAMERA_STATE_TRACK == impl_msg.getState()) {
					UAV_PRO::CCameraTrackStateProtocal& impl = dynamic_cast<UAV_PRO::CCameraTrackStateProtocal&>(impl_msg.getStateImpl());
					SPDLOG_DEBUG("[光电设备状态扩展信息包] 当前状态: 跟踪");
					UAV_PRO::CTarget& tar = impl.getTarget();

					std::stringstream msg;
					msg.precision(2);
					msg << std::fixed;
					msg << "[光电设备状态扩展信息包] "
						<< " 目标编号:" << tar.getNumber()
						<< " 相识度:" << tar.getSame() * 1.0 / 100
						<< " 宽(像素):" << tar.getWidth()
						<< " 高(像素):" << tar.getHeight()
						<< " 水平角度:" << tar.getAngle()
						<< " 俯仰角度:" << tar.getElevation();
					SPDLOG_DEBUG("{}", msg.str().data());
				}
			}
		}
		else if (UAV_PRO::UAV_CTRL_PRO_CMD_INTERFERE_STATUS == cmd_type)
		{
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 5))
			{
				UAV_PRO::CDisturberProtocal& impl_msg = dynamic_cast<UAV_PRO::CDisturberProtocal&>(pro_msg.getMajor());
				if (UAV_PRO::CDisturberProtocal::ORDER_STATE == impl_msg.getCmd())
				{
					UAV_PRO::CStateDisturberProtocal& impl = dynamic_cast<UAV_PRO::CStateDisturberProtocal&>(impl_msg.getImpl());
					std::stringstream msg;
					msg.precision(2);
					msg << std::fixed;
					msg << "[干扰状态] "
						<< " 开始频率:" << impl.getStartFrequency()
						<< " 结速频率:" << impl.getEndFrequency()
						//<< " 开启状态:" << ((impl.getState() == UAV_PRO::CDisturberProtocal::STATE_OPEN) ? "开启" : "关闭")
						<< " 温度告警:" << ((impl.getTempAlarm() == UAV_PRO::CDisturberProtocal::STATE_OPEN) ? "告警" : "正常")
						<< " 驻波告警:" << ((impl.getAlarm() == UAV_PRO::CDisturberProtocal::STATE_OPEN) ? "告警" : "正常");
					SPDLOG_DEBUG("{}", msg.str().data());
					PrintHex((BYTE*)data,len);

					uint8_t state = impl.getState();
					if ((state & 0))
					{// 定频
						SPDLOG_DEBUG("定频");
					}
					else {
						SPDLOG_DEBUG("非定频");
					}
					if (/*!*/(state & 0x02))
					{// GPS/ 开启
						SPDLOG_DEBUG("GPS:开启");
					}
					else {
						SPDLOG_DEBUG("GPS:关闭");
					}
					if (/*!*/(state & 0x08))
					{// 430M 开启
						SPDLOG_DEBUG("430M:开启");
					}
					else {
						SPDLOG_DEBUG("430M:关闭");
					}
					if (/*!*/(state & 0x10))
					{// 850M 开启
						SPDLOG_DEBUG("850M:开启");
					}
					else {
						SPDLOG_DEBUG("850M:关闭");
					}
					if (/*!*/(state & 0x40))
					{// 2400M 开启
						SPDLOG_DEBUG("2400M:开启");
					}
					else {
						SPDLOG_DEBUG("2400M:关闭");
					}
					if (/*!*/(state & 0x80))
					{// 5800M 开启
						SPDLOG_DEBUG("5800M:开启");
					}
					else {
						SPDLOG_DEBUG("5800M:关闭");
					}
				}
				else {

				}
			}
		}
		else if (UAV_PRO::UAV_CTRL_NOTIFY_TARGET == cmd_type)
		{
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 6))
			{
				UAV_PRO::CNotifyTarget& impl_msg = dynamic_cast<UAV_PRO::CNotifyTarget&>(pro_msg.getMajor());
				std::stringstream msg;
				msg.precision(2);
				msg << std::fixed;
				msg << "[光电目标上报信息包 0x0B] "
					<< " 目标数:" << impl_msg.getCount();
				SPDLOG_DEBUG("{}", msg.str().data());

				impl_msg.getTargets([](UAV_PRO::CTargetEx tar) {
					std::stringstream msg;
					msg.precision(2);
					msg << std::fixed;
					msg << "[光电目标上报信息包 0x0B] "
						<< " 目标编号:" << tar.getNumber()
						<< " 类型:" << tar.getType()
						<< " 相识度:" << tar.getSame() * 1.0 / 100
						<< " 宽(像素):" << tar.getWidth()
						<< " 高(像素):" << tar.getHeight()
						<< " 实际宽度(m):" << tar.getRealWidth()*1.0/100
						<< " 实际高度(m):" << tar.getRealHeight() * 1.0 / 100
						<< " 水平速度(像素/秒):" << tar.getHorSpeed()
						<< " 垂直速度(像素/秒):" << tar.getVerSpeed()
						<< " 水平物理速度(m/s):" << tar.getRealHorSpeed() * 1.0 / 100
						<< " 垂直物理速度(m/s):" << tar.getRealVerSpeed() * 1.0 / 100
						<< " 运动方向:" << static_cast<int>(tar.getDirection())
						<< " 水平角度:" << tar.getAngle()
						<< " 俯仰角度:" << tar.getElevation()
						<< " AI模板:" << getAIMode(tar.getReserved())
						<< " r3:"<<tar.getReserved3();
					SPDLOG_DEBUG("{}", msg.str().data());
					});
			}
		}
		else if (UAV_PRO::UAV_CTRL_LENS_STATE == cmd_type)
		{
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 7))
			{
				UAV_PRO::CLensState& impl_msg = dynamic_cast<UAV_PRO::CLensState&>(pro_msg.getMajor());
				std::stringstream msg;
				msg.precision(2);
				msg << std::fixed;

				msg << "[镜头状态扩展信息包 0x0C] "
					<< " 可见光: 水平视场角:" << impl_msg.getLightHorViewAngle()*1.0/100
					<< " 垂直视场角:" << impl_msg.getLightVerViewAngle() * 1.0 / 100
					<< " , 热像:水平视场角:" << impl_msg.getThermographyHorViewAngle() * 1.0 / 100
					<< " 垂直视场角:" << impl_msg.getThermographyVerViewAngle() * 1.0 / 100
					<< " ,可见光镜头当前倍数:" << impl_msg.getLightZoom()
					<< " 热像镜头当前倍数" << impl_msg.getThermographyZoom()
					<< " ,物理焦距:" << impl_msg.getFocus()*1.0/100 << " mm";
				SPDLOG_DEBUG("{}", msg.str().data());
			}
		}
		else if (UAV_PRO::UAV_CTRL_PRO_CMD_TARGET_ADDR == cmd_type)
		{
			UAV_PRO::CTargetDirectionProtocal& impl_msg = dynamic_cast<UAV_PRO::CTargetDirectionProtocal&>(pro_msg.getMajor());
			std::stringstream msg;
			msg.precision(8);
			msg << std::fixed;

			msg << "[设置光电目址信息包 0x03] "
				<< "经度:" << impl_msg.getLng()
				<< " 纬度:" << impl_msg.getLat()
				<< " 高度:" << impl_msg.getHeight();
			SPDLOG_DEBUG("{}", msg.str().data());
		}
		else if (UAV_PRO::UAV_PRO_DEVIATION_INFO == cmd_type)
		{
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 8))
			{
				UAV_PRO::CDeviationInfoProtocal& impl_msg = dynamic_cast<UAV_PRO::CDeviationInfoProtocal&>(pro_msg.getMajor());
				std::stringstream msg;
				msg.precision(2);
				msg << std::fixed;

				msg << "[脱靶量信息包 0x0F] "
					<< "跟踪源:" << impl_msg.getChannel()
					<< " 水平脱靶量偏移:" << impl_msg.getHorOffset()
					<< " 俯仰脱靶量偏移:" << impl_msg.getVerOffset();
				SPDLOG_DEBUG("{}", msg.str().data());
			}
		}
		else if (UAV_PRO::UAV_CTRL_STATE_EXTEND == cmd_type)
		{
			if (gOutputStateFlag == 1 || (gOutputStateFlag == 9))
			{
				UAV_PRO::CStateExtendProtocal& impl_msg = dynamic_cast<UAV_PRO::CStateExtendProtocal&>(pro_msg.getMajor());
				std::stringstream msg;
				msg.precision(2);
				msg << std::fixed;

				msg << "[0x15] "
					<< "激光测距器:" << getStateType(impl_msg.getLaserDistanceState())
					<< " 激光测照机:" << getStateType(impl_msg.getLaserHitState())
					<< " 激光补光:" << getStateType(impl_msg.getLaserLightState())
					<< " 加热器状态:" << getStateType(impl_msg.getHeatingState())
					<< " 热成像压缩机状态:" << getStateType(impl_msg.getThermalCompressorState())
					<< " 可见光聚焦模式:" << getFocusMode(impl_msg.getVisiableImageFocusMode())
					<< " 热成像聚焦模式:" << getFocusMode(impl_msg.getThermalImageFocusMode());
				SPDLOG_DEBUG("{}", msg.str().data());
			}
		}
		else {
			SPDLOG_DEBUG("未知");
		}
		});
}

void Output(const char* msg)
{
	SetColor(10, FOREGROUND_INTENSITY);
	std::cout << msg << std::endl;
	SetColor(7, 0);
}

void OutputError(const char* msg)
{
	SetColor(FOREGROUND_RED, FOREGROUND_INTENSITY);
	std::cout << msg << std::endl;
	SetColor(7, 0);
}

SOCKET OpenSocket(unsigned short port)
{
	int domain, type, protocol;
	domain = AF_INET;
	type = SOCK_DGRAM;
	protocol = IPPROTO_UDP;

	SOCKET sockfd = socket(domain, type, protocol);
	if (sockfd == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons((u_short)port);

	uint32_t argp;
	argp = 1;
	int err = ioctlsocket(sockfd, FIONBIO, (u_long*)&argp);
	err = ::bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr_in));
	if (err != 0)
	{
		std::cout << "绑定端口失败 :" << port << std::endl;
	}
	return sockfd;
}

int Write(const char* lpBuff, int iBuffLen)
{
	struct sockaddr_in to;
	if (NULL == lpBuff || iBuffLen <= 0)
	{
		return -1;
	}
	if (INVALID_SOCKET == gSendSkt)
	{
		gSendSkt = OpenSocket(0);
	}
	if (INVALID_SOCKET == gSendSkt)
	{
		std::cout << "UDP端口错误" << std::endl;
		return -2;
	}
	to.sin_family = AF_INET;
	to.sin_port = htons(port);
	inet_pton(AF_INET, gip.data(), &to.sin_addr);
	int iRet = ::sendto(gSendSkt, lpBuff, iBuffLen, 0, (struct sockaddr*)&to, sizeof(sockaddr));
	return iRet;
}

std::string Print(int v)
{
	char szTmp[8];
	_snprintf_s(szTmp, _TRUNCATE, "0x%02X", v);
	return szTmp;
}

void PrintHex(BYTE* lpBuff, int len)
{
	std::string hex;
	char tmp[16];
	std::cout << std::endl;
	_snprintf_s(tmp, _TRUNCATE, "字节数:%d,", len);
	hex.append(tmp);
	std::cout << hex.data() << std::endl;
	hex = "";
	for (int i = 0; i < len; i++)
	{
		hex.append(Print((int)lpBuff[i]));
		hex.append(" ");
	}
	std::cout << hex.data() << std::endl;
	//OutputDebugString(hex.data());
}

void cmd_10_wrap()
{
	int dev_id = 0;
	double flng = 0, flat = 0;
	double north = 0.0;
	int iDis = 0,height=0;
	std::cout << std::endl;
	Output("输入经度:");
	std::cin >> flng;
	Output("输入纬度:");
	std::cin >> flat;
	Output("输入距离:");
	std::cin >> iDis;
	Output("输入高度:");
	std::cin >> height;

	HELPER::CFixBuffer buf;
	UAV_PRO::CTargetDirectionProtocal major;

	major.setDistance(iDis * 1.0);
	major.setLng(flng);
	major.setLat(flat);
	major.setHeight(height);

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}

	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "引导命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "引导命令发送失败";
	}

	{
		HELPER::COnlyReadBuffer rbuf(buf.buf(),len);
		UAV_PRO::CCameraUavProtocal::parse(rbuf, [](UAV_PRO::CCameraUavProtocal& pro) {
			if (pro.getMajor().getMajorCmd() == UAV_PRO::UAV_CTRL_PRO_CMD_TARGET_ADDR) {
				UAV_PRO::CTargetDirectionProtocal& impl_msg = dynamic_cast<UAV_PRO::CTargetDirectionProtocal&>(pro.getMajor());
				std::cout << "lng:" << impl_msg.getLng() << "  lat:" << impl_msg.getLat() << "  dis:" << impl_msg.getDistance() << " height:" << impl_msg.getHeight() << std::endl;

			}
			});
	}
}

void cmd_18_wrap()
{
	int dev_id = 0;
	double fHor = 0, fVer = 0,lng=0,lat=0,alt=0;
	int iDis = 0;
	int mode = 0,search = 0;
	float viewing_angle = 0;

	std::cout << std::endl;
	Output("模式: 0-方位角 1-经纬度");
	std::cin >> mode;

	if (0 == mode) {
		std::cout << std::endl;
		Output("输入水平角度:");
		std::cin >> fHor;
		Output("输入垂直角度:");
		std::cin >> fVer;
		Output("输入距离:");
		std::cin >> iDis;
	}
	else if (1 == mode) {
		std::cout << std::endl;
		Output("输入经度:");
		std::cin >> lng;
		Output("输入纬度:");
		std::cin >> lat;
		Output("输入海拔:");
		std::cin >> alt;
	}
	else {
		return;
	}

	Output("搜索: 0-不搜索 1-水平搜索 2-俯仰搜索");
	std::cin >> search;
	if((search == 1) || (search == 2)){
		Output("输入视场角度(光电视场小于等于则启动搜索): (x.x度)");
		std::cin >> viewing_angle;
	}

	HELPER::CFixBuffer buf;

	UAV_PRO::CTargetDirectionExtendProtocal major;

	major.setDistance(iDis * 1.0);
	major.setVerAngle(fVer);
	major.setHorAngle(fHor);
	major.setLng(lng);
	major.setLat(lat);
	major.setHeight(alt);
	major.setGuideType(mode);
	//major.setHeight(5);

	major.setSearchMethod(search);
	if (1 == search) {
		major.setHorViewingAngle(viewing_angle);
	}
	else if (2 == search) {
		major.setVerViewingAngle(viewing_angle);
	}

	UAV_PRO::CCameraUavProtocal pro(major);
	int len = pro.construct(buf);
	if (len <= 0)
	{
		OutputError("错误");
		return;
	}
	int ret = Write((const char*)buf.peek(), buf.readableBytes());
	if (ret == len)
	{
		std::cout << "引导命令发送成功";
		PrintHex((BYTE*)buf.peek(), buf.readableBytes());
	}
	else {
		std::cout << "引导命令发送失败";
	}
}