#pragma once
#include <functional>
#define ENABLE_BIG_ENDIAN_BYTE_ORDER
#include <codehelper/include/buffer.h>
HELPER_USING_NAMESPACE(HELPER);
#define UAV_PRO_NAMESPACE_BEGIN namespace uav_pro {
#define UAV_PRO_NAMESPACE_END }
#define UAV_PRO uav_pro

UAV_PRO_NAMESPACE_BEGIN
/***
* @brief 协议版本 v2.0 build 20220823
*/

enum UAV_CTRL_PRO_CMD
{
	UAV_CTRL_PRO_CMD_NONE = 0,
	/// 光电设备状态信息包
	UAV_CTRL_PRO_CMD_STATUS = 0x01,
	/// 光电设备方位、俯仰信息包
	UAV_CTRL_PRO_CMD_POSITION = 0x02,
	/// 设置光电目址信息包
	UAV_CTRL_PRO_CMD_TARGET_ADDR = 0x03,
	/// 设置光电搜索跟踪信息包
	UAV_CTRL_PRO_CMD_TRACK = 0x04,
	/// 光电转发干扰查询
	UAV_CTRL_PRO_CMD_INTERFERE_QUERY = 0x05,
	/// 光电转发干扰控制
	UAV_CTRL_PRO_CMD_INTERFERE_CTRL = 0x06,
	/// 光电转发干扰状态
	UAV_CTRL_PRO_CMD_INTERFERE_STATUS = 0x07,
	/// 光电设备状态扩展信息包
	UAV_CTRL_PRO_CMD_STATUS_EXTEND = 0x08,
	/// 光电镜头控制信息包
	UAV_CTRL_PRO_CMD_CAMERA_LENS = 0x09,
	/// 光电扫描扩展信息包
	UAV_CTRL_SEARCH_EXTEND = 0x0A,
	/// 光电目标上报信息包
	UAV_CTRL_NOTIFY_TARGET = 0x0B,
	/// 光电镜头状态扩展信息包
	UAV_CTRL_LENS_STATE = 0x0C,
	/// 转台方位控制信息包
	UAV_CTRL_PTZ = 0x0D,
	/// 切换视频跟踪源
	UAV_CTRL_TRACK_SRC = 0x0E,
	/// 脱靶量上报信息包
	UAV_PRO_DEVIATION_INFO = 0x0F,
	/// 手动锁定目标
	UAV_PRO_MANUAL_TRACK = 0x10,
	/// 光电外围设备控制
	UAV_PRO_FDO_CTRL = 0x11,
	/// AI 参数控制
	UAV_CTRL_AI_PARAMS = 0x12,
	/// 聚焦模式切换
	UAV_CTRL_FOCUS_MODE = 0x13,
	/// 配置光电根据目标大小自动变倍参数
	UAV_CTRL_TRACK_LENSE_CFG = 0x14,
	/// 光电系统状态扩展信息包
	UAV_CTRL_STATE_EXTEND = 0x15,
	/// 设置光电目址信息包
	UAV_CTRL_PRO_CMD_TARGET_ADDR_EXTEND = 0x16,
};

enum TRACKS_DIRECTION
{
	TRACKS_DIRECTION_CLOSEIN = 0, /// 抵近
	TRACKS_DIRECTION_FAR = 1, /// 远离
};

enum GUIDE_TYPE
{
	GUIDE_TYPE_AZIMUTH = 0, /// 方位角
	GUIDE_TYPE_LNGLAT = 1, /// 经纬度
};

enum STATE_TYPE
{
	CLOSE_STATE = 0, /// 关闭
	OPEN_STATE = 1, /// 开启
};

class CMajorProtocal;
class CCameraUavProtocal
{
public:
	CCameraUavProtocal(CMajorProtocal& major);

	static bool parse(COnlyReadBuffer& buf, std::function<void(CCameraUavProtocal&)>);
	/// <summary>
	/// 构造协议数据到缓冲区
	/// </summary>
	/// <param name="buf">缓冲区</param>
	/// <returns>缓存缓冲区数据 >0成功, 否则失败</returns>
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getCmd() { return cmd_; }
	unsigned long long getTimestamp() { return timestamp_; }

	CMajorProtocal& getMajor() { return major_; }
private:
	/**
* @brief 协议头
*/
/// 起始位 1
	unsigned int start_bit_ = 0;
	/// 协议号 2
	unsigned int pro_number_ = 0;
	/// 包长度 3
	/// =命令字+时间戳+信息内容+信息序列号+错误校验
	unsigned int packet_len_ = 0;
	/// 命令字 4
	unsigned int cmd_ = 0;
	/// 时间戳 5
	unsigned long long timestamp_ = 0;
	/// 信息内容
	CMajorProtocal& major_;
	/// 信息序列号 7
	/// 第一条 1,每次累加
	unsigned int message_index_ = 0;
	/// 错误校验 8
	unsigned int verify_ = 0;
	/// 停止位
	unsigned int stop_bit_ = 0;

	static const size_t protocal_len = 36;
};
 
class CMajorProtocal
{
public:
	CMajorProtocal() =default;
	/// <summary>
	/// 解析缓冲区协议数据
	/// </summary>
	/// <param name="buf">缓冲区</param>
	/// <returns>已解析数据长度 >0成功, 否则失败</returns>
	virtual int parse(COnlyReadBuffer& buf) = 0;
	/// <summary>
	/// 构造协议数据到缓冲区
	/// </summary>
	/// <param name="buf">缓冲区</param>
	/// <returns>缓存缓冲区数据 >0成功, 否则失败</returns>
	virtual int construct(CFixBuffer& buf) = 0;
	virtual bool valid() =0;
	virtual unsigned int getMajorCmd() =0;
};

class CDisturberImpl
{
public:
	CDisturberImpl() = default;
	virtual int parse(COnlyReadBuffer& buf) = 0;
	virtual int construct(CFixBuffer& buf) = 0;
	virtual bool valid() = 0;
	virtual unsigned int getDisturberCmd() = 0;
};

class CCameraStateExtendImpl
{
public:
	CCameraStateExtendImpl() = default;
	virtual int parse(COnlyReadBuffer& buf) = 0;
	virtual int construct(CFixBuffer& buf) = 0;
	virtual bool valid() = 0;
	virtual unsigned int getStateCmd() = 0;
};

class CAIParamImpl
{
public:
	CAIParamImpl() = default;
	virtual int parse(COnlyReadBuffer& buf) = 0;
	virtual int construct(CFixBuffer& buf) = 0;
	virtual bool valid() = 0;
	virtual unsigned int getCmd() = 0;
};


class CTarget
{
public:
	CTarget() = default;
	virtual ~CTarget() = default;

	int parse(COnlyReadBuffer& buf);
	int construct(CFixBuffer& buf);

	const uint32_t protocal_head_len = 36;

	unsigned int getNumber() { return number_; }
	unsigned int getType() { return type_; }
	unsigned int getSame() { return same_; }
	unsigned int getWidth() { return width_; }
	unsigned int getHeight() { return height_; }
	double getAngle() { return angle_; }
	double getElevation() { return elevation_; }

	CTarget& operator=(const CTarget& t)
	{
		number_ = t.number_;
		type_ = t.type_;
		same_ = t.same_;
		width_ = t.width_;
		height_ = t.height_;
		angle_ = t.angle_;
		elevation_ = t.elevation_;
		return *this;
	}

	bool operator == (const CTarget& rhs)
	{
		int iCount = 0;
		(number_ == rhs.number_ ? iCount++ : 0);
		(type_ == rhs.type_ ? iCount++ : 0);
		(same_ == rhs.same_ ? iCount++ : 0);
		(width_ == rhs.width_ ? iCount++ : 0);
		(height_ == rhs.height_ ? iCount++ : 0);
		(angle_ == rhs.angle_ ? iCount++ : 0);
		(elevation_ == rhs.elevation_ ? iCount++ : 0);
		return 7 == iCount;
	}
private:
	unsigned int number_ = 0; ///编号
	unsigned int type_ = 0; /// 类型
	unsigned int same_ = 0; ///相识度 百分比乘以100
	unsigned int width_ = 0; ///宽度 像素
	unsigned int height_ = 0; ///高度 像素
	double angle_ = 0;///方位角
	double elevation_ = 0; ///俯仰角
};

class CStateProtocal : public CMajorProtocal
{
public:
	CStateProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getState() { return state_; }
	unsigned int getMode() { return mode_; }
	uint32_t getTrackCode() { return track_code_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setState(unsigned int v) { state_ = v; }
	void setMode(unsigned int v) { mode_ = v; }

protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_STATUS; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 光电当前状态时间戳
	unsigned long long timestamp_ = 0;
	/// 工作状态 0-异常 1-正常
	unsigned int state_ = 0;
	/// 工作模式 0-空闲 1-搜索 2-跟踪
	unsigned int mode_ = 0;
	/// 故障编码
	uint8_t code_[8] = {0};
	/// 预留 跟踪源 0-可见光 1-热像
	uint32_t track_code_ =0;
	const uint32_t protocal_head_len = 32;
};

/// <summary>
/// 光电设备方位、俯仰信息包
/// </summary>
class CPositionProtocal : public CMajorProtocal
{
public:
	CPositionProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	double getHorAngle() { return hor_angle_; }
	double getVerAngle() { return ver_angle_; }
	double getDistance() { return distance_; }
	double getHorSpeed() { return hor_speed_; }
	double getVerSpeed() { return ver_speed_; }
	short getHeight() { return height_; }
	char getLensZoom() { return lens_zoom_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setHorAngle(double v) { hor_angle_ = v; }
	void setVerAngle(double v) { ver_angle_ = v; }
	void setDistance(double v) { distance_ = v; }
	void setHorSpeed(double v) { hor_speed_ = v; }
	void setVerSpeed(double v) { ver_speed_ = v; }
	void setHeight(short v) { height_ = v; }
	void setLensZoom(uint8_t v) { lens_zoom_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_POSITION; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 光电当前状态时间戳
	unsigned long long timestamp_ = 0;
	/// 水平角度
	double hor_angle_ = 0;
	/// 俯仰角度
	double ver_angle_ = 0;
	/// 距离
	double distance_ = 0;
	/// 水平角速度
	double hor_speed_ = 0;
	/// 俯仰角速度
	double ver_speed_ = 0;
	/// 高度
	short height_ = 0;
	/// 镜头倍率
	uint8_t lens_zoom_ = 0;
	/// 预留
	uint8_t reserved_[1] = {0};
	const uint32_t protocal_head_len = 32;
};

/// <summary>
/// 设置光电目址信息包
/// </summary>
class CTargetDirectionProtocal : public CMajorProtocal
{
public:
	CTargetDirectionProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned int getSysNumber() { return sys_number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	double getLng() { return lng_; }
	double getLat() { return lat_; }
	double getHeight() { return height_; }
	double getDistance() { return distance_; }
	double getHorAngle() { return hor_angle_; }
	double getVerAngle() { return ver_angle_; }
	unsigned short getUserData() { return user_data_; }
	unsigned char getGuideType() { return guide_type_; }
	unsigned char getTargetDirection() { return target_direction_; }

	void setNumber(int v) { number_ = v; }
	void setSysNumber(int v) { sys_number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setLng(double v) { lng_ = v; }
	void setLat(double v) { lat_ = v; }
	void setHeight(double v) { height_ = v; }
	void setDistance(double v) { distance_ = v; }
	void setHorAngle(double v) { hor_angle_ = v; }
	void setVerAngle(double v) { ver_angle_ = v; }
	void setUserData(unsigned short v) { user_data_ = v; }
	void setGuideType(unsigned char v) { guide_type_ = v; }
	void setTargetDirection(unsigned char v) { target_direction_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_TARGET_ADDR; }
private:
	/// 光电编号 1
	unsigned int number_ = 0;
	/// 系统编号 2
	unsigned int sys_number_ = 0;
	/// 系统下发 时间戳 3
	unsigned long long timestamp_ = 0;
	/// 目标经度 4
	double lng_ = 0;
	/// 目标纬度 5
	double lat_ = 0;
	/// 目标高度 6
	double height_ = 0;
	/// 目标距离 7
	double distance_ = 0;
	/// 水平角度 7
	double hor_angle_ = 0;
	/// 俯仰角度 8
	double ver_angle_ = 0;
	/// 用户数据
	unsigned short user_data_ = 0;
	/// 预留
	//uint8_t reserved_[2] = {0};
	/// 0-	方位、俯仰、距离 1 - 经度、纬度、高度
	unsigned char guide_type_ = 0;
	/// 0 --抵近 1--远离
	unsigned char target_direction_ = 0;
	const uint32_t protocal_head_len = 68;
};

/// <summary>
/// 设置光电搜索跟踪信息包
/// </summary>
class CCameraCtrlProtocal : public CMajorProtocal
{
public:
	enum
	{
		SEARCH = 1,//搜索并自动跟踪
		TRACK = 2,//跟踪
		FREE = 3,//释放
		ONLY_SEARCH = 4,//仅搜索
		HOR_SEARCH_TRACK=5,//在指定大小视场左右搜索并自动跟踪
		VER_SEARCH_TRACK = 6,//在指定大小视场上下搜索并自动跟踪
	};

	enum
	{
		SEARCH_TYPE_NO_MOVE = 0,//在当前视场里搜索
		SEARCH_TYPE_TOP_MOVE = 1,//向上移动当前视场再搜索
		SEARCH_TYPE_BUTTOM_MOVE = 2,//向下移动当前视场再搜索
		SEARCH_TYPE_LEFT_MOVE = 3,//向左移动当前视场再搜索
		SEARCH_TYPE_RIGHT_MOVE = 4,//向右移动当前视场再搜索
		SEARCH_TYPE_ZOOMNEAR = 5,//镜头向后拉一倍再搜索
		SEARCH_TYPE_ZOOMFAR = 6,//镜头向前推一倍再搜索
		SEARCH_TYPE_LEFT_CLOCKWISE = 7,//从左顺时针搜索
		SEARCH_TYPE_TOP_CLOCKWISE = 8,//从上顺时针搜索
		SEARCH_TYPE_RIGHT_CLOCKWISE = 9,//从右顺时针搜索
		SEARCH_TYPE_BOTTOM_CLOCKWISE = 10,//从下顺时针搜索
		SEARCH_TYPE_ANGLE_RANGE = 11,//按指定搜索范围搜索
		SEARCH_TYPE_ANGLE_RANGE_EX = 12,//按指定范围指定速度搜索
		SEARCH_TYPE_LEFTRIGHT = 13,//左右搜索一次
		SEARCH_TYPE_TOPBUTTON = 14,//上下搜索一次
	};
public:
	CCameraCtrlProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getCmd() { return cmd_; }
	double getHorStartAngle() { return hor_start_angle_; }
	double getHorEndAngle() { return hor_end_angle_; }
	double getVerStartAngle() { return ver_start_angle_; }
	double getVerEndAngle() { return ver_end_angle_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setCmd(int v) { cmd_ = v; }
	void setHorStartAngle(double v) { hor_start_angle_ = static_cast<int>(v); }
	void setHorEndAngle(double v) { hor_end_angle_ = static_cast<int>(v); }
	void setVerStartAngle(double v) { ver_start_angle_ = static_cast<int>(v); }
	void setVerEndAngle(double v) { ver_end_angle_ = static_cast<int>(v); }
	void setTargetNumber(int v);

	/**
	* 0--在当前视场里搜索
	* 1--向上移动当前视场再搜索
	* 2--向下移动当前视场再搜索
	* 3--向左移动当前视场再搜索
	* 4--向右移动当前视场再搜索
	* 5--镜头向后拉一倍再搜索
	* 6--镜头向前推一倍再搜索
	* 7-从左顺时针搜索
	* 8-从上顺时针搜索
	* 9-从右顺时针搜索
	* 10-从下顺时针搜索
	* 11-按指定搜索范围搜索
	* 12-按指定范围指定速度搜索
	* 13-左右搜索一次
	* 14-上下搜索一次
	*/
	void setSearchMode(int v);
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_TRACK; }
private:
	/// 光电编号 1
	unsigned int number_ = 0;
	/// 系统下发 时间戳 2
	unsigned long long timestamp_ = 0;
	/// 指控指令 1:搜索并自动跟踪；2：跟踪；3：释放; 4:仅搜索
	unsigned int cmd_ = 0;
	/// 水平开始角度(视场角) 命令(HOR_SEARCH_TRACK)是角度支持float (*100)
	int hor_start_angle_ = 0;
	/// 水平结束角度
	int hor_end_angle_ = 0;
	/// 俯仰开始角度(视场角) 命令(VER_SEARCH_TRACK)是角度支持float (*100)
	int ver_start_angle_ = 0;
	/// 俯仰结束角度
	int ver_end_angle_ = 0;
	/// 预留 目标编号（跟踪指定目标使用）
	uint8_t reserved_[4] = {0};
	const uint32_t protocal_head_len = 36;
};


class CDisturberProtocal : public CMajorProtocal
{
public:
	enum
	{
		STATE_CLOSE = 0x10, /// 关闭&正常
		STATE_OPEN = 0x11,/// 开启&告警
	};
	enum
	{
		CHANNEL_VAR_1 = 0x01, /// 模块1(840-2600MHZ)
		CHANNEL_VAR_2 = 0x02, /// 模块2(2600-6000MHZ)
		CHANNEL_VAR_3 = 0x03, /// 模块3(1570-1620MHZ)
		CHANNEL_FIX_1 = 0x04, /// GPS/BD
		CHANNEL_FIX_2 = 0x10, /// 350M
		CHANNEL_FIX_3 = 0x11, /// 430M
		CHANNEL_FIX_4 = 0x12, /// 850M
		CHANNEL_FIX_5 = 0x21, /// 1200M
		CHANNEL_FIX_6 = 0x31, /// 2400M
		CHANNEL_FIX_7 = 0x32, /// 5800M
		CHANNEL_FIX_8 = 0xA1, /// 全部控制通道
		CHANNEL_FIX_9 = 0xB1, /// GPS+全部控制通道
		CHANNEL_FIX_10 = 0xB2, /// GPS+全部控制通道 查询
	};

	enum
	{
		ORDER_CTRL = 0xA1, /// 控制
		ORDER_QUERY = 0xB1,/// 查询
		ORDER_STATE = 0xB2,/// 状态
	};
public:
	CDisturberProtocal(CDisturberImpl&);
	virtual ~CDisturberProtocal() = default;

	static bool parse(COnlyReadBuffer& buf, std::function<void(CDisturberProtocal&)>);

	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned short getStart() { return start_; }
	unsigned char getCmd() { return cmd_; }
	unsigned char getDev() { return dev_; }
	unsigned char getChannel() { return channel_; }

	void setCmd(unsigned char v) { cmd_ = v; }
	void setDev(unsigned char v) { dev_ = v; }
	void setChannel(unsigned char v) { channel_ = v; }

	CDisturberImpl& getImpl() { return impl_; }
protected:
	unsigned int getMajorCmd() { 
		unsigned char cmd = getCmd();
		if (ORDER_CTRL == cmd) {
			return UAV_CTRL_PRO_CMD_INTERFERE_CTRL;
		}
		else if (ORDER_QUERY == cmd) {
			return UAV_CTRL_PRO_CMD_INTERFERE_QUERY;
		}
		else if (ORDER_STATE == cmd) {
			return UAV_CTRL_PRO_CMD_INTERFERE_STATUS;
		}
		return 0; 
	}
private:
	/// 起始位
	unsigned short start_ = 0;
	/// 指令标识
	unsigned char cmd_ = 0;
	/// 目的设备
	unsigned char dev_ = 0;
	/// 通道标识
	unsigned char channel_ = 0;
	/// 数据
	CDisturberImpl& impl_; /// 7 byte
	/// 预留
	unsigned char reserved_ = 0;
	/// 校验
	unsigned char check_ = 0;
	/// 结束位
	unsigned short end_ = 0;
	static const uint32_t protocal_head_len = 9;
};

/// <summary>
/// 光电转发干扰查询
/// </summary>
class CQueryDisturberProtocal : public CDisturberImpl
{
public:
	CQueryDisturberProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned short getStartFrequency() { return start_fq_; }
	unsigned short getEndFrequency() { return end_fq_; }
	unsigned char getState() { return state_; }
	unsigned char getTempAlarm() { return temp_alarm_; }
	unsigned char getAlarm() { return alarm_; }

	void setStartFrequency(unsigned short v) { start_fq_ = v; }
	void setEndFrequency(unsigned short v) { end_fq_ = v; }
	void setState(unsigned char v) { state_ = v; }

protected:
	unsigned int getDisturberCmd() { return CDisturberProtocal::ORDER_QUERY; }
private:
	/// 开始频率
	unsigned short start_fq_ = 0;
	/// 结速频率
	unsigned short end_fq_ = 0;
	/// 开启状态
	unsigned char state_ = 0;
	/// 温度告警
	unsigned char temp_alarm_ = 0;
	/// 驻波告警
	unsigned char alarm_ = 0;
	static const uint32_t protocal_head_len = 7;
};

/// <summary>
/// 光电转发干扰控制
/// </summary>
class CCtrlDisturberProtocal : public CDisturberImpl
{
public:
	CCtrlDisturberProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned short getStartFrequency() { return start_fq_; }
	unsigned short getEndFrequency() { return end_fq_; }
	unsigned char getState() { return state_; }
	unsigned char getTempAlarm() { return temp_alarm_; }
	unsigned char getAlarm() { return alarm_; }

	void setStartFrequency(unsigned short v) { start_fq_ = v; }
	void setEndFrequency(unsigned short v) { end_fq_ = v; }
	void setState(unsigned char v) { state_ = v; }
protected:
	unsigned int getDisturberCmd() { return CDisturberProtocal::ORDER_CTRL; }
private:
	/// 开始频率 步进为1MHZ(840-6000MHz)
	unsigned short start_fq_ = 0;
	/// 结速频率
	unsigned short end_fq_ = 0;
	/// 开启状态
	unsigned char state_ = 0;
	/// 温度告警
	unsigned char temp_alarm_ = 0;
	/// 驻波告警
	unsigned char alarm_ = 0;
	static const uint32_t protocal_head_len = 7;
};

/// <summary>
/// 光电转发干扰状态
/// </summary>
class CStateDisturberProtocal : public CDisturberImpl
{
public:
	CStateDisturberProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned short getStartFrequency() { return start_fq_; }
	unsigned short getEndFrequency() { return end_fq_; }
	unsigned char getState() { return state_; }
	unsigned char getTempAlarm() { return temp_alarm_; }
	unsigned char getAlarm() { return alarm_; }

	void setStartFrequency(unsigned short v) { start_fq_ = v; }
	void setEndFrequency(unsigned short v) { end_fq_ = v; }
	void setState(unsigned char v) { state_ = v; }
protected:
	unsigned int getDisturberCmd() { return CDisturberProtocal::ORDER_STATE; }
private:
	/// 开始频率 步进为1MHZ(840-6000MHz)
	unsigned short start_fq_ = 0;
	/// 结速频率
	unsigned short end_fq_ = 0;
	/// <summary>
	/// 开启状态
	/// bit 0:写频 1:GPS/DB 2:预留 3:430M 4:850M 5:预留 6:2400M 7:5800M
	/// 1开启 0关闭,,有反的情况
	/// </summary>
	unsigned char state_ = 0;
	/// 温度告警
	unsigned char temp_alarm_ = 0;
	/// 驻波告警
	unsigned char alarm_ = 0;
	static const uint32_t protocal_head_len = 7;
};

/// <summary>
/// 光电设备状态扩展信息包
/// </summary>
class CCameraStateExtendInfoProtocal : public CMajorProtocal
{
public:
	enum CAMERA_STATE
	{
		CAMERA_STATE_IDLE = 0, //空闲
		CAMERA_STATE_SEARCH = 1, //搜索
		CAMERA_STATE_TRACK = 2, //跟踪
	};
public:
	CCameraStateExtendInfoProtocal(CCameraStateExtendImpl&);
	static bool parse(COnlyReadBuffer& buf, std::function<void(CCameraStateExtendInfoProtocal&)>);
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getState() { return state_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setState(unsigned int v) { state_ = v; }
	CCameraStateExtendImpl& getStateImpl() { return impl_; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_STATUS_EXTEND; }
public:
	/// 光电编号
	unsigned int number_ = 0;
	/// 光电当前状态时间戳
	unsigned long long timestamp_ = 0;
	/// 工作状态
	unsigned int state_ = 0;
	/// 数据长度
	unsigned int data_len_ = 0;
	/// 外带数据
	CCameraStateExtendImpl& impl_;
	static const uint32_t protocal_head_len = 20;
};

/// <summary>
/// 光电设备状态扩展信息包
/// 光电设备状态(空闲)扩展信息包
/// </summary>
class CCameraIdleStateProtocal : public CCameraStateExtendImpl
{
public:
	CCameraIdleStateProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getTimestamp() { return timestamp_; }
	void setTimestamp(unsigned int v) { timestamp_ = v; }
protected:
	unsigned int getStateCmd() { return CCameraStateExtendInfoProtocal::CAMERA_STATE_IDLE; }
private:
	unsigned int timestamp_ = 0; /**持续时间(秒)*/
	const uint32_t protocal_head_len = 4;
};

/// <summary>
/// 光电设备状态扩展信息包
/// 光电设备状态(搜索)扩展信息包
/// </summary>
class CCameraSearchStateProtocal : public CCameraStateExtendImpl
{
public:
	CCameraSearchStateProtocal() =default;

	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getTimestamp() { return timestamp_; }
	unsigned int getTargetCount() { return target_count_; }

	void setTimestamp(unsigned int v) { timestamp_ = v; }
	void setTargetCount(unsigned int v) { target_count_ = v; }
	void getTargets(std::function<void(CTarget)> cb);
protected:
	unsigned int getStateCmd() { return CCameraStateExtendInfoProtocal::CAMERA_STATE_SEARCH; }
private:
	unsigned int timestamp_ = 0; /**持续时间(秒)*/
	unsigned int target_count_ = 0; /**已搜索到的目标数量*/
	unsigned int data_len_ = 0;
	/// <summary>
	/// 目标
	/// </summary>
	std::array<CTarget, 128> targets_;
	const uint32_t protocal_head_len = 12;
};

/// <summary>
/// 光电设备状态扩展信息包
/// 光电设备状态(跟踪)扩展信息包
/// </summary>
class CCameraTrackStateProtocal : public CCameraStateExtendImpl
{
public:
	CCameraTrackStateProtocal() = default;

	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getTimestamp() { return timestamp_; }
	void setTimestamp(unsigned int v) { timestamp_ = v; }

	CTarget& getTarget() { return target_; }
	void setTarget(CTarget& v) { target_ = v; }

	unsigned short getState() { return state_; }
	void setState(unsigned short v) { state_ = v; }

	unsigned short getUserData() { return user_data_; }
	void setUserData(unsigned short v) { user_data_ = v; }
protected:
	unsigned int getStateCmd() { return CCameraStateExtendInfoProtocal::CAMERA_STATE_TRACK; }
private:
	unsigned int timestamp_ = 0; /**持续时间(秒)*/
	//unsigned int state_ = 0; /**1-锁定 0-丢失*/
	unsigned short user_data_ = 0; /**自定义数据*/
	unsigned short state_ = 0; /**1-锁定 0-丢失*/
	CTarget target_; /// 目标
	const uint32_t protocal_head_len = 8;
};


/// <summary>
/// 光电镜头控制信息包
/// </summary>
class CCameraLensInfoProtocal : public CMajorProtocal
{
public:
	CCameraLensInfoProtocal()= default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned int getSysNumber() { return sys_number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getCmd() { return cmd_; }
	unsigned int getZoom() { return zoom_; }
	unsigned int getZoomStep() { return zoom_step_; }
	unsigned int getFocusStep() { return focus_step_; }
	uint32_t getReserved() { return reserved_; }

	void setNumber(int v) { number_ = v; }
	void setSysNumber(int v) { sys_number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setCmd(unsigned int v) { cmd_ = v; }
	void setZoom(unsigned int v) { zoom_ = v; }
	void setZoomStep(unsigned int v) { zoom_step_ = v; }
	void setFocusStep(unsigned int v) { focus_step_ = v; }
	void setReserved(uint32_t v) { reserved_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_CAMERA_LENS; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 系统编号
	unsigned int sys_number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 控制命令
	/**
		0.镜头停止运动(0x00)
		1. 跳转到指定物理焦距
		2. 跳转到指定倍数
		3. 跳转到指定zoom和focus位置
		4. 镜头持续推远(0x04)
		5. 镜头持续拉近(0x05)
		6. 镜头点动推远(0x06)
		7. 镜头点动拉近(0x07)
		8. 焦点持续推远(0x08)
		9. 焦点持续拉近(0x09)
		10. 焦点点动推远(0x0A)
		11. 焦点点动拉近(0x0B)
	*/
	unsigned int cmd_ = 0;
	/** @brief 物理焦距/倍数 ..
		当镜头控制命令为 0x01时表示镜头的物理焦距, 单位mm 如150mm；
		当控制命令为0x02时表示镜头倍数，如42倍。
		当镜头控制命令为0x03时此字段作为保留不使用
	*/
	unsigned int zoom_ = 0;
	/// zoom位置. 当镜头控制命令为0x03时有效，表示zoom的步数
	unsigned int zoom_step_ = 0;
	/// Focus 位置 .. 当镜头控制命令为0x03时有效，表示focus的步数
	unsigned int focus_step_ = 0;
	/// 预留
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 36;
};


/// <summary>
/// 光电扫描扩展信息包
/// </summary>
class CSearchExtendProtocal : public CMajorProtocal
{
public:
	CSearchExtendProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	uint32_t getNumber() { return number_; }
	uint64_t getTimestamp() { return timestamp_; }
	uint32_t getCmd() { return cmd_; }
	uint32_t getHorStartAngle() { return hor_start_angle_; }
	uint32_t getHorStopAngle() { return hor_stop_angle_; }
	uint32_t getVerStartAngle() { return ver_start_angle_; }
	uint32_t getVerStopAngle() { return ver_stop_angle_; }
	uint32_t getSpeed() { return speed_; }

	void setNumber(uint32_t v) { number_ = v; }
	void setTimestamp(uint64_t v) { timestamp_ = v; }
	void setCmd(uint32_t v) { cmd_ = v; }
	void setHorStartAngle(uint32_t v) { hor_start_angle_ = v; }
	void setHorStopAngle(uint32_t v) { hor_stop_angle_ = v; }
	void setVerStartAngle(uint32_t v) { ver_start_angle_ = v; }
	void setVerStopAngle(uint32_t v) { ver_stop_angle_ = v; }
	void setSpeed(uint32_t v) { speed_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_SEARCH_EXTEND; }
private:
	/// 光电编号
	uint32_t number_ = 0;
	/// 时间戳
	uint64_t timestamp_ = 0;
	/// 控制命令 1: 启动扫描 0：停止扫描
	uint32_t cmd_ = 0;
	/// 水平搜索开始角度
	uint32_t hor_start_angle_ = 0;
	/// 水平搜索结束角度
	uint32_t hor_stop_angle_ = 0;
	/// 俯仰搜索开始角度  如果俯仰开始和结束角度相等，光电将在指定的俯仰位置上持续扫描，如果不相等，则做螺旋式持续扫描
	uint32_t ver_start_angle_ = 0;
	/// 俯仰搜索结束角度
	uint32_t ver_stop_angle_ = 0;
	/// 扫描速度 度/秒
	uint32_t speed_ = 0;
	/// 保留
	uint32_t reserved_ = 0;
	const uint32_t protocal_head_len = 40;
};

class CTargetEx
{
public:
	CTargetEx() = default;
	virtual ~CTargetEx() = default;

	int parse(COnlyReadBuffer& buf);
	int construct(CFixBuffer& buf);

	const uint32_t protocal_head_len = 60;

	uint32_t getNumber() { return number_; }
	uint32_t getType() { return type_; }
	uint32_t getSame() { return same_; }
	uint32_t getWidth() { return width_; }
	uint32_t getHeight() { return height_; }
	uint32_t getRealWidth() { return real_width_; }
	uint32_t getRealHeight() { return real_height_; }
	uint8_t getDirection() { return direction_; }
	uint16_t getHorSpeed() { return hor_speed_; }
	uint16_t getVerSpeed() { return ver_speed_; }
	uint16_t getRealHorSpeed() { return real_hor_speed_; }
	uint16_t getRealVerSpeed() { return real_ver_speed_; }
	uint16_t getReserved3() { return reserved3_; } // 距离
	uint8_t getReserved() { return reserved_; } // 当前AI模板类型

	double getAngle() { return azimuth_; }
	double getElevation() { return pitch_angle_; }

	void setNumber(uint32_t v) {
		number_ = v;
	}
	void setAngle(double v){
		azimuth_ = v;
	}
	void setElevation(double v) {
		pitch_angle_ = v;
	}
	// 距离
	void setReserved3(uint32_t v) {
		reserved3_ = v;
	}
	// 当前AI模板类型 1 -对天3分类 2 -对地 80 分类
	void setReserved(uint8_t v) {
		reserved_ = v;
	}

	CTargetEx& operator=(const CTargetEx& t)
	{
		number_ = t.number_;
		type_ = t.type_;
		same_ = t.same_;
		width_ = t.width_;
		height_ = t.height_;
		real_width_ = t.real_width_;
		real_height_ = t.real_height_;
		direction_ = t.direction_;
		reserved_ = t.reserved_;
		reserved2_ = t.reserved2_;
		hor_speed_ = t.hor_speed_;
		ver_speed_ = t.ver_speed_;
		real_hor_speed_ = t.real_hor_speed_;
		real_ver_speed_ = t.real_ver_speed_;
		azimuth_ = t.azimuth_;
		pitch_angle_ = t.pitch_angle_;
		reserved3_ = t.reserved3_;
		return *this;
	}
private:
	uint32_t number_ = 0; ///编号
	uint32_t type_ = 0; /// 类型
	uint32_t same_ = 0; ///相识度 百分比乘以100
	uint32_t width_ = 0; ///宽度 像素
	uint32_t height_ = 0; ///高度 像素
	uint32_t real_width_ = 0;/// 实际宽度 * 100,单位m,比如：200表示2m, 10表示0.1 m
	uint32_t real_height_ = 0;/// 实际高度 * 100,单位m,比如：200表示2m, 10表示0.1 m
	uint8_t direction_ = 0; /// 运动方向：0: 静止;  1: 左;   2: 右;3: 上;    4: 下;   5：左上;6：右上;  7: 左下; 8: 右下;
	uint8_t reserved_ = 0; //保留字段
	uint16_t reserved2_ = 0;  //保留字段
	uint16_t hor_speed_ = 0;  //水平速度 像素 / 秒
	uint16_t ver_speed_ = 0;  //垂直速度 像素 / 秒
	uint16_t real_hor_speed_ = 0;  // 水平物理速度 速度 * 100，比如253表示2.53 m/s
	uint16_t real_ver_speed_ = 0;  // 垂直物理速度 速度 * 100，比如253表示2.53 m/s
	double azimuth_ = 0; // 方位角 根据目标在视频中的位置偏移以及当前光电的方位角度计算出。
	double pitch_angle_ = 0; // 俯仰角 根据目标在视频中的位置偏移以及当前光电的俯仰角度计算出。
	uint32_t reserved3_ = 0; //保留字段 距离
};

/// <summary>
/// 光电目标上报信息包
/// </summary>
class CNotifyTarget : public CMajorProtocal
{
public:
	CNotifyTarget() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	uint32_t getTimestamp() { return timestamp_; }
	uint32_t getCount() { return target_count_; }
	void getTargets(std::function<void(CTargetEx)> cb);

	void setTargets(std::array<CTargetEx, 16>& t, int count);
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_NOTIFY_TARGET; }
private:
	/// 持续时间（秒）
	uint32_t timestamp_ = 0;
	/// 当前视场中检测出来的目标数量
	uint32_t target_count_ = 0;
	/// 后续数据长度
	uint32_t data_len_ = 0;
	/// <summary>
	/// 目标
	/// </summary>
	std::array<CTargetEx, 128> targets_;
	const uint32_t protocal_head_len = 12;
};

/// <summary>
/// 光电镜头状态扩展信息包
/// </summary>
class CLensState : public CMajorProtocal
{
public:
	CLensState() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	uint32_t getNumber() { return number_; }
	uint64_t getTimestamp() { return timestamp_; }
	uint32_t getLightHorViewAngle() { return light_hor_view_angle_; }
	uint32_t getLightVerViewAngle() { return light_ver_view_angle_; }
	uint32_t getThermographyHorViewAngle() { return thermography_hor_view_angle_; }
	uint32_t getThermographyVerViewAngle() { return thermography_ver_view_angle_; }
	uint32_t getLightZoom() { return light_zoom_; }
	uint32_t getThermographyZoom() { return thermography_zoom_; }
	uint32_t getFocus() { return focus_; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_LENS_STATE; }
private:
	uint32_t number_ = 0; ///编号
	uint64_t timestamp_ = 0; /// 时间戳
	uint32_t light_hor_view_angle_ = 0; /// 可见光镜头当前水平视场角 水平视场角 * 100 如2532表示25.32度
	uint32_t light_ver_view_angle_ = 0; /// 可见光镜头当前垂直视场角 垂直视场角 * 100 如1532表示15.32度
	uint32_t thermography_hor_view_angle_ = 0; /// 热像镜头当前水平视场角 水平视场角 * 100 如2532表示25.32度
	uint32_t thermography_ver_view_angle_ = 0; /// 热像镜头当前垂直视场角 垂直视场角 * 100 如1532表示15.32度
	uint32_t light_zoom_ = 0; /// 可见光镜头当前倍数 
	uint32_t thermography_zoom_ = 0; /// 热像镜头当前倍数 
	uint32_t focus_ = 0; // 镜头当前物理焦距 镜头当前物理焦距 * 100 如1500表示当前物理焦距为15mm
	uint32_t reserved_ = 0; // 保留
	const uint32_t protocal_head_len = 44;
};

/// <summary>
/// 转台方位控制信息包
/// </summary>
class CCtrlPtz : public CMajorProtocal
{
public:
	enum ptz_cmd
	{
		ptz_cmd_stop = 0, 
		ptz_cmd_left = 1,
		ptz_cmd_right = 2,
		ptz_cmd_up = 3,
		ptz_cmd_down = 4,
		ptz_cmd_leftup = 5,
		ptz_cmd_leftdown = 6,
		ptz_cmd_rightup = 7,
		ptz_cmd_rightdown = 8,
	};
public:
	CCtrlPtz() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	void setCmd(int v) {cmd_ = v;}
	void setSpeed(int v) { speed_ = v; }
	void setMode(int v) { mode_ = v; }

	uint32_t getNumber() { return number_; }
	uint64_t getTimestamp() { return timestamp_; }
	uint32_t getCmd() { return cmd_; }
	uint32_t getSpeed() { return speed_; }
	uint32_t getMode() { return mode_; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PTZ; }
private:
	uint32_t number_ = 0; ///编号
	uint64_t timestamp_ = 0; /// 时间戳
	uint32_t cmd_ = 0; /// 
	uint32_t speed_ = 0; /// 有效值 0-254
	uint32_t mode_ = 0; /// 0--持续运动（直到收到停止指令）1--单次运动(指令执行一次自动停止)
	uint32_t reserved_ = 0; // 保留
	const uint32_t protocal_head_len = 28;
};

/// <summary>
/// 切换跟踪视频源信息包
/// </summary>
class CTrackSource : public CMajorProtocal
{
public:
	CTrackSource() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	void setSource(int v) { source_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_TRACK_SRC; }
private:
	uint32_t number_ = 0; ///编号
	uint64_t timestamp_ = 0; /// 时间戳
	uint32_t source_ = 0; /// 跟踪视频源通 0-可见光 1-热线
	uint32_t reserved_ = 0; // 保留
	const uint32_t protocal_head_len = 20;
};

/// <summary>
/// 脱靶量上报信息包
/// </summary>
class CDeviationInfoProtocal : public CMajorProtocal
{
public:
	CDeviationInfoProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getChannel() { return channel_; }
	int getHorOffset() { return hor_offset_; }
	int getVerOffset() { return ver_offset_; }
	uint32_t getReserved() { return reserved_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setChannel(unsigned int v) { channel_ = v; }
	void setHorOffset(int v) { hor_offset_ = v; }
	void setVerOffset(int v) { ver_offset_ = v; }
	void setReserved(uint32_t v) { reserved_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_PRO_DEVIATION_INFO; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 跟踪视频源通道  0 -- 可见光 1 --热像
	unsigned int channel_ = 0;
	/// 水平脱靶量偏移 (像素) 左负右正
	int hor_offset_ = 0;
	/// 俯仰脱靶量偏移 (像素) 下负上正
	int ver_offset_ = 0;
	/// 预留
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 28;
};


/// <summary>
/// 手动锁定目标
/// </summary>
class CManualTrackProtocal : public CMajorProtocal
{
public:
	CManualTrackProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getChannel() { return channel_; }
	int getX() { return x_; }
	int getY() { return y_; }
	uint32_t getReserved() { return reserved_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setChannel(unsigned int v) { channel_ = v; }
	void setX(int v) { x_ = v; }
	void setY(int v) { y_ = v; }
	void setReserved(uint32_t v) { reserved_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_PRO_MANUAL_TRACK; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 跟踪视频源通道  0 -- 可见光 1 --热像
	unsigned int channel_ = 0;
	/// 目标位置坐标 (像素) 视频区域的坐标投射到 704 X 576 区域
	int x_ = 0;
	int y_ = 0;
	/// 预留
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 28;
};

/// <summary>
/// 光电外围设备控制
/// </summary>
class CFDOCtrl : public CMajorProtocal
{
public:
	enum fdo_cmd
	{
		fdo_cmd_laser_ranging =1, // 控制激光测距器
		fdo_cmd_laser_position = 2, //  控制激光测照机
		fdo_cmd_laser_light = 3, // 控制激光补光
		fdo_cmd_heating = 4, // 控制加热器
		fdo_cmd_thermal_compressor= 5, // 控制热成像压缩机
	};
public:
	CFDOCtrl() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getCmd() { return cmd_; }
	int getParams() { return params_; }
	uint32_t getReserved() { return reserved_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setCmd(unsigned int v) { cmd_ = v; }
	void setParams(int v) { params_ = v; }
	void setReserved(uint32_t v) { reserved_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_PRO_FDO_CTRL; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 控制命令 参见 fdo_cmd
	unsigned int cmd_ = 0;
	/// 控制参数  1：开启 0：关闭
	unsigned int params_ = 0;
	/// 预留
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 24;
};

/// <summary>
/// AI 参数控制
/// </summary>
class CAIModelCtrl : public CMajorProtocal
{
public:
	enum params_cmd
	{
		params_cmd_mode_cfg = 1, // AI 模板切换
	};
public:
	CAIModelCtrl(CAIParamImpl&);
	static bool parse(COnlyReadBuffer& buf, std::function<void(CAIModelCtrl&)>);
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getCmd() { return cmd_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setCmd(unsigned int v) { cmd_ = v; }
	CAIParamImpl& getParamImpl() { return impl_; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_AI_PARAMS; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 参数配置命令 参见 params_cmd  1. AI 模板切换 2. 保留
	unsigned int cmd_ = 0;
	/// 参数长度 后续参数内容的长度. 根据”参数配置命令”决定具体长度.当 “参数配置命令”为1 时 : 长度为4
	unsigned int params_length_ = 0;
	CAIParamImpl& impl_;
	static const uint32_t protocal_head_len = 20;
};

/// <summary>
/// AI 参数控制
/// AI 模板切换 参数
/// </summary>
class CAIModeCfgParam : public CAIParamImpl
{
public:
	CAIModeCfgParam() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getTemplate() { return template_; }
	void setTemplate(unsigned int v) { template_ = v; }
protected:
	unsigned int getCmd() { return CAIModelCtrl::params_cmd_mode_cfg; }
private:
	unsigned int template_ = 0; /// 1: 选择AI模板1(对天 3 分类)  2: 选择 AI模板2(对地80分类)
	const uint32_t protocal_head_len = 4;
};

/// <summary>
/// 配置聚焦模式
/// </summary>
class CFocusMode : public CMajorProtocal
{
public:
	enum focus_mode
	{
		focus_mode_auto =0, // 自动(AF)
		focus_mode_semiauto = 1, // 半自动(ZF)
		focus_mode_manual = 2, // 手动（MF）
	};
public:
	CFocusMode() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getMode() { return mode_; }
	uint8_t getChannel() { return channel_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setMode(unsigned int v) { mode_ = v; }
	void setChannel(uint8_t v) {}
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_FOCUS_MODE; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 聚焦模式 参见 focus_mode 
	unsigned int mode_ = 0;
	/// 通道 0-可将光镜头 1-热像镜头
	uint8_t channel_ = 0;
	/// 预留
	uint8_t reserved_[3];
	static const uint32_t protocal_head_len = 20;
};

/// <summary>
/// 配置光电跟踪参数
/// </summary>
class CTrackCfgProtocal : public CMajorProtocal
{
public:
	CTrackCfgProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int isEnableAutoZoom() { return auto_zoom_enable_; }
	unsigned int getAutoZoominThreshold() { return zoomin_threshold_; }
	unsigned int getAutoZoomoutThreshold() { return zoomout_threshold_; }

	void setNumber(int v) { number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	/// <summary>
	/// 0 -不启用 1 -启用
	/// </summary>
	/// <param name="v"></param>
	void setAutoZoom(unsigned int v) { auto_zoom_enable_ = v; }
	/// <summary>
	/// 自动变倍放大阈值 
	/// 跟踪的目标大小(宽*高) 如果小于该阈值，则镜头会自动放大一次。推荐值为40， 单位：像素
	/// </summary>
	/// <param name="v"></param>
	void setZoominThreshold(uint32_t v) { zoomin_threshold_ = v; }
	/// <summary>
	/// 自动变倍缩小阈值 
	/// 跟踪的目标大小(宽*高) 如果大于该阈值，则镜头会自动缩小一次。推荐值为100， 单位：像素
	/// </summary>
	/// <param name="v"></param>
	void setZoomoutThreshold(uint32_t v) { zoomout_threshold_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_TRACK_LENSE_CFG; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 时间戳
	unsigned long long timestamp_ = 0;
	/// 是否启用自动变倍功能 0 -不启用 1 -启用
	unsigned int auto_zoom_enable_ = 0;
	/// 自动变倍放大阈值 跟踪的目标大小(宽*高) 如果小于该阈值，则镜头会自动放大一次。推荐值为40， 单位：像素
	unsigned int zoomin_threshold_ = 40;
	/// 自动变倍缩小阈值 跟踪的目标大小(宽*高) 如果大于该阈值，则镜头会自动缩小一次。推荐值为100， 单位：像素
	unsigned int zoomout_threshold_ = 100;
	/// 预留
	uint8_t reserved_[32];
	static const uint32_t protocal_head_len = 56;
};

class CStateExtendProtocal : public CMajorProtocal
{
public:
	enum focus_mode
	{
		focus_mode_auto = 0, // 自动(AF)
		focus_mode_semiauto = 1, // 半自动(ZF)
		focus_mode_manual = 2, // 手动（MF）
	};
public:
	CStateExtendProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	unsigned int getLaserDistanceState() { return laser_distance_; }
	unsigned int getLaserHitState() { return laser_hit_; }
	unsigned int getLaserLightState() { return laser_light_; }
	unsigned int getHeatingState() { return heating_; }
	unsigned int getThermalCompressorState() { return thermal_compressor_; }
	unsigned int getVisiableImageFocusMode() { return visiable_focus_; }
	unsigned int getThermalImageFocusMode() { return thermal_focus_; }

protected:
	unsigned int getMajorCmd() { return UAV_CTRL_STATE_EXTEND; }
private:
	/// 光电编号
	unsigned int number_ = 0;
	/// 光电当前状态时间戳
	unsigned long long timestamp_ = 0;
	/// 激光测距器状态 0 -关闭 1 -开启
	unsigned int laser_distance_ = 0;
	/// 激光测照机状态 0 -关闭 1 -开启
	unsigned int laser_hit_ = 0;
	/// 激光补光状态 0 -关闭 1 -开启
	unsigned int laser_light_ = 0;
	/// 加热器状态 0 -关闭 1 -开启
	unsigned int heating_ = 0;
	/// 热成像压缩机状态 0 -关闭 1 -开启
	unsigned int thermal_compressor_ = 0;
	/// 可见光聚焦模式 0 -AF(自动) 1 -ZF(半自动) 2 -MF(手动)
	unsigned int visiable_focus_ = 0;
	/// 热成像聚焦模式 0 -AF(自动) 1 -ZF(半自动) 2 -MF(手动)
	unsigned int thermal_focus_ = 0;
	uint8_t reserved_[32];
	const uint32_t protocal_head_len = 72;
};

/// <summary>
/// 设置光电目址扩展信息包
/// </summary>
class CTargetDirectionExtendProtocal : public CMajorProtocal
{
public:
	CTargetDirectionExtendProtocal() = default;
	virtual int parse(COnlyReadBuffer& buf);
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getNumber() { return number_; }
	unsigned int getSysNumber() { return sys_number_; }
	unsigned long long getTimestamp() { return timestamp_; }
	double getLng() { return lng_; }
	double getLat() { return lat_; }
	double getHeight() { return height_; }
	double getDistance() { return distance_; }
	double getHorAngle() { return hor_angle_; }
	double getVerAngle() { return ver_angle_; }
	unsigned short getUserData() { return user_data_; }
	unsigned char getGuideType() { return guide_type_; }
	unsigned char getTargetDirection() { return target_direction_; }
	uint32_t getSearchMethod() { return search_; }
	float getHorViewingAngle() { return hor_viewing_angle_ * 1.0 / 100; }
	float getVerViewingAngle() { return ver_viewing_angle_ * 1.0 / 100; }

	void setNumber(int v) { number_ = v; }
	void setSysNumber(int v) { sys_number_ = v; }
	void setTimestamp(unsigned long long v) { timestamp_ = v; }
	void setLng(double v) { lng_ = v; }
	void setLat(double v) { lat_ = v; }
	void setHeight(double v) { height_ = v; }
	void setDistance(double v) { distance_ = v; }
	void setHorAngle(double v) { hor_angle_ = v; }
	void setVerAngle(double v) { ver_angle_ = v; }
	void setUserData(unsigned short v) { user_data_ = v; }
	void setGuideType(unsigned char v) { guide_type_ = v; }
	void setTargetDirection(unsigned char v) { target_direction_ = v; }
	void setHorViewingAngle(float v) { hor_viewing_angle_ = static_cast<int>(v * 100); }
	void setVerViewingAngle(float v) { ver_viewing_angle_ = static_cast<int>(v * 100); }
	void setSearchMethod(int v) { search_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_TARGET_ADDR_EXTEND; }
private:
	/// 光电编号 1
	unsigned int number_ = 0;
	/// 系统编号 2
	unsigned int sys_number_ = 0;
	/// 系统下发 时间戳 3
	unsigned long long timestamp_ = 0;
	/// 目标经度 4
	double lng_ = 0;
	/// 目标纬度 5
	double lat_ = 0;
	/// 目标高度 6
	double height_ = 0;
	/// 目标距离 7
	double distance_ = 0;
	/// 水平角度 7
	double hor_angle_ = 0;
	/// 俯仰角度 8
	double ver_angle_ = 0;
	/// 用户数据
	unsigned short user_data_ = 0;
	/// 预留
	//uint8_t reserved_[2] = {0};
	/// 0-	方位、俯仰、距离 1 - 经度、纬度、高度
	unsigned char guide_type_ = 0;
	/// 0 --抵近 1--远离
	unsigned char target_direction_ = 0;
	/// 是否开启搜索
	uint32_t search_ = 0;
	/// 实际值 * 100，如 15 表示搜索的水平视场角为0.15度
	uint32_t hor_viewing_angle_ = 0;
	/// 实际值 * 100，如 15 表示搜索的垂直视场角为0.15度
	uint32_t ver_viewing_angle_ = 0;
	uint8_t reserved_[4];
	const uint32_t protocal_head_len = 84;
};

UAV_PRO_NAMESPACE_END