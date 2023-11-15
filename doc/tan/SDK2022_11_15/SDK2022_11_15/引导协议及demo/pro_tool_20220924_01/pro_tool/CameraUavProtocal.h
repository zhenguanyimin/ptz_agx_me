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
* @brief Э��汾 v2.0 build 20220823
*/

enum UAV_CTRL_PRO_CMD
{
	UAV_CTRL_PRO_CMD_NONE = 0,
	/// ����豸״̬��Ϣ��
	UAV_CTRL_PRO_CMD_STATUS = 0x01,
	/// ����豸��λ��������Ϣ��
	UAV_CTRL_PRO_CMD_POSITION = 0x02,
	/// ���ù��Ŀַ��Ϣ��
	UAV_CTRL_PRO_CMD_TARGET_ADDR = 0x03,
	/// ���ù������������Ϣ��
	UAV_CTRL_PRO_CMD_TRACK = 0x04,
	/// ���ת�����Ų�ѯ
	UAV_CTRL_PRO_CMD_INTERFERE_QUERY = 0x05,
	/// ���ת�����ſ���
	UAV_CTRL_PRO_CMD_INTERFERE_CTRL = 0x06,
	/// ���ת������״̬
	UAV_CTRL_PRO_CMD_INTERFERE_STATUS = 0x07,
	/// ����豸״̬��չ��Ϣ��
	UAV_CTRL_PRO_CMD_STATUS_EXTEND = 0x08,
	/// ��羵ͷ������Ϣ��
	UAV_CTRL_PRO_CMD_CAMERA_LENS = 0x09,
	/// ���ɨ����չ��Ϣ��
	UAV_CTRL_SEARCH_EXTEND = 0x0A,
	/// ���Ŀ���ϱ���Ϣ��
	UAV_CTRL_NOTIFY_TARGET = 0x0B,
	/// ��羵ͷ״̬��չ��Ϣ��
	UAV_CTRL_LENS_STATE = 0x0C,
	/// ת̨��λ������Ϣ��
	UAV_CTRL_PTZ = 0x0D,
	/// �л���Ƶ����Դ
	UAV_CTRL_TRACK_SRC = 0x0E,
	/// �Ѱ����ϱ���Ϣ��
	UAV_PRO_DEVIATION_INFO = 0x0F,
	/// �ֶ�����Ŀ��
	UAV_PRO_MANUAL_TRACK = 0x10,
	/// �����Χ�豸����
	UAV_PRO_FDO_CTRL = 0x11,
	/// AI ��������
	UAV_CTRL_AI_PARAMS = 0x12,
	/// �۽�ģʽ�л�
	UAV_CTRL_FOCUS_MODE = 0x13,
	/// ���ù�����Ŀ���С�Զ��䱶����
	UAV_CTRL_TRACK_LENSE_CFG = 0x14,
	/// ���ϵͳ״̬��չ��Ϣ��
	UAV_CTRL_STATE_EXTEND = 0x15,
	/// ���ù��Ŀַ��Ϣ��
	UAV_CTRL_PRO_CMD_TARGET_ADDR_EXTEND = 0x16,
};

enum TRACKS_DIRECTION
{
	TRACKS_DIRECTION_CLOSEIN = 0, /// �ֽ�
	TRACKS_DIRECTION_FAR = 1, /// Զ��
};

enum GUIDE_TYPE
{
	GUIDE_TYPE_AZIMUTH = 0, /// ��λ��
	GUIDE_TYPE_LNGLAT = 1, /// ��γ��
};

enum STATE_TYPE
{
	CLOSE_STATE = 0, /// �ر�
	OPEN_STATE = 1, /// ����
};

class CMajorProtocal;
class CCameraUavProtocal
{
public:
	CCameraUavProtocal(CMajorProtocal& major);

	static bool parse(COnlyReadBuffer& buf, std::function<void(CCameraUavProtocal&)>);
	/// <summary>
	/// ����Э�����ݵ�������
	/// </summary>
	/// <param name="buf">������</param>
	/// <returns>���滺�������� >0�ɹ�, ����ʧ��</returns>
	virtual int construct(CFixBuffer& buf);
	virtual bool valid();

	unsigned int getCmd() { return cmd_; }
	unsigned long long getTimestamp() { return timestamp_; }

	CMajorProtocal& getMajor() { return major_; }
private:
	/**
* @brief Э��ͷ
*/
/// ��ʼλ 1
	unsigned int start_bit_ = 0;
	/// Э��� 2
	unsigned int pro_number_ = 0;
	/// ������ 3
	/// =������+ʱ���+��Ϣ����+��Ϣ���к�+����У��
	unsigned int packet_len_ = 0;
	/// ������ 4
	unsigned int cmd_ = 0;
	/// ʱ��� 5
	unsigned long long timestamp_ = 0;
	/// ��Ϣ����
	CMajorProtocal& major_;
	/// ��Ϣ���к� 7
	/// ��һ�� 1,ÿ���ۼ�
	unsigned int message_index_ = 0;
	/// ����У�� 8
	unsigned int verify_ = 0;
	/// ֹͣλ
	unsigned int stop_bit_ = 0;

	static const size_t protocal_len = 36;
};
 
class CMajorProtocal
{
public:
	CMajorProtocal() =default;
	/// <summary>
	/// ����������Э������
	/// </summary>
	/// <param name="buf">������</param>
	/// <returns>�ѽ������ݳ��� >0�ɹ�, ����ʧ��</returns>
	virtual int parse(COnlyReadBuffer& buf) = 0;
	/// <summary>
	/// ����Э�����ݵ�������
	/// </summary>
	/// <param name="buf">������</param>
	/// <returns>���滺�������� >0�ɹ�, ����ʧ��</returns>
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
	unsigned int number_ = 0; ///���
	unsigned int type_ = 0; /// ����
	unsigned int same_ = 0; ///��ʶ�� �ٷֱȳ���100
	unsigned int width_ = 0; ///��� ����
	unsigned int height_ = 0; ///�߶� ����
	double angle_ = 0;///��λ��
	double elevation_ = 0; ///������
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
	/// �����
	unsigned int number_ = 0;
	/// ��統ǰ״̬ʱ���
	unsigned long long timestamp_ = 0;
	/// ����״̬ 0-�쳣 1-����
	unsigned int state_ = 0;
	/// ����ģʽ 0-���� 1-���� 2-����
	unsigned int mode_ = 0;
	/// ���ϱ���
	uint8_t code_[8] = {0};
	/// Ԥ�� ����Դ 0-�ɼ��� 1-����
	uint32_t track_code_ =0;
	const uint32_t protocal_head_len = 32;
};

/// <summary>
/// ����豸��λ��������Ϣ��
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
	/// �����
	unsigned int number_ = 0;
	/// ��統ǰ״̬ʱ���
	unsigned long long timestamp_ = 0;
	/// ˮƽ�Ƕ�
	double hor_angle_ = 0;
	/// �����Ƕ�
	double ver_angle_ = 0;
	/// ����
	double distance_ = 0;
	/// ˮƽ���ٶ�
	double hor_speed_ = 0;
	/// �������ٶ�
	double ver_speed_ = 0;
	/// �߶�
	short height_ = 0;
	/// ��ͷ����
	uint8_t lens_zoom_ = 0;
	/// Ԥ��
	uint8_t reserved_[1] = {0};
	const uint32_t protocal_head_len = 32;
};

/// <summary>
/// ���ù��Ŀַ��Ϣ��
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
	/// ����� 1
	unsigned int number_ = 0;
	/// ϵͳ��� 2
	unsigned int sys_number_ = 0;
	/// ϵͳ�·� ʱ��� 3
	unsigned long long timestamp_ = 0;
	/// Ŀ�꾭�� 4
	double lng_ = 0;
	/// Ŀ��γ�� 5
	double lat_ = 0;
	/// Ŀ��߶� 6
	double height_ = 0;
	/// Ŀ����� 7
	double distance_ = 0;
	/// ˮƽ�Ƕ� 7
	double hor_angle_ = 0;
	/// �����Ƕ� 8
	double ver_angle_ = 0;
	/// �û�����
	unsigned short user_data_ = 0;
	/// Ԥ��
	//uint8_t reserved_[2] = {0};
	/// 0-	��λ������������ 1 - ���ȡ�γ�ȡ��߶�
	unsigned char guide_type_ = 0;
	/// 0 --�ֽ� 1--Զ��
	unsigned char target_direction_ = 0;
	const uint32_t protocal_head_len = 68;
};

/// <summary>
/// ���ù������������Ϣ��
/// </summary>
class CCameraCtrlProtocal : public CMajorProtocal
{
public:
	enum
	{
		SEARCH = 1,//�������Զ�����
		TRACK = 2,//����
		FREE = 3,//�ͷ�
		ONLY_SEARCH = 4,//������
		HOR_SEARCH_TRACK=5,//��ָ����С�ӳ������������Զ�����
		VER_SEARCH_TRACK = 6,//��ָ����С�ӳ������������Զ�����
	};

	enum
	{
		SEARCH_TYPE_NO_MOVE = 0,//�ڵ�ǰ�ӳ�������
		SEARCH_TYPE_TOP_MOVE = 1,//�����ƶ���ǰ�ӳ�������
		SEARCH_TYPE_BUTTOM_MOVE = 2,//�����ƶ���ǰ�ӳ�������
		SEARCH_TYPE_LEFT_MOVE = 3,//�����ƶ���ǰ�ӳ�������
		SEARCH_TYPE_RIGHT_MOVE = 4,//�����ƶ���ǰ�ӳ�������
		SEARCH_TYPE_ZOOMNEAR = 5,//��ͷ�����һ��������
		SEARCH_TYPE_ZOOMFAR = 6,//��ͷ��ǰ��һ��������
		SEARCH_TYPE_LEFT_CLOCKWISE = 7,//����˳ʱ������
		SEARCH_TYPE_TOP_CLOCKWISE = 8,//����˳ʱ������
		SEARCH_TYPE_RIGHT_CLOCKWISE = 9,//����˳ʱ������
		SEARCH_TYPE_BOTTOM_CLOCKWISE = 10,//����˳ʱ������
		SEARCH_TYPE_ANGLE_RANGE = 11,//��ָ��������Χ����
		SEARCH_TYPE_ANGLE_RANGE_EX = 12,//��ָ����Χָ���ٶ�����
		SEARCH_TYPE_LEFTRIGHT = 13,//��������һ��
		SEARCH_TYPE_TOPBUTTON = 14,//��������һ��
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
	* 0--�ڵ�ǰ�ӳ�������
	* 1--�����ƶ���ǰ�ӳ�������
	* 2--�����ƶ���ǰ�ӳ�������
	* 3--�����ƶ���ǰ�ӳ�������
	* 4--�����ƶ���ǰ�ӳ�������
	* 5--��ͷ�����һ��������
	* 6--��ͷ��ǰ��һ��������
	* 7-����˳ʱ������
	* 8-����˳ʱ������
	* 9-����˳ʱ������
	* 10-����˳ʱ������
	* 11-��ָ��������Χ����
	* 12-��ָ����Χָ���ٶ�����
	* 13-��������һ��
	* 14-��������һ��
	*/
	void setSearchMode(int v);
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_PRO_CMD_TRACK; }
private:
	/// ����� 1
	unsigned int number_ = 0;
	/// ϵͳ�·� ʱ��� 2
	unsigned long long timestamp_ = 0;
	/// ָ��ָ�� 1:�������Զ����٣�2�����٣�3���ͷ�; 4:������
	unsigned int cmd_ = 0;
	/// ˮƽ��ʼ�Ƕ�(�ӳ���) ����(HOR_SEARCH_TRACK)�ǽǶ�֧��float (*100)
	int hor_start_angle_ = 0;
	/// ˮƽ�����Ƕ�
	int hor_end_angle_ = 0;
	/// ������ʼ�Ƕ�(�ӳ���) ����(VER_SEARCH_TRACK)�ǽǶ�֧��float (*100)
	int ver_start_angle_ = 0;
	/// ���������Ƕ�
	int ver_end_angle_ = 0;
	/// Ԥ�� Ŀ���ţ�����ָ��Ŀ��ʹ�ã�
	uint8_t reserved_[4] = {0};
	const uint32_t protocal_head_len = 36;
};


class CDisturberProtocal : public CMajorProtocal
{
public:
	enum
	{
		STATE_CLOSE = 0x10, /// �ر�&����
		STATE_OPEN = 0x11,/// ����&�澯
	};
	enum
	{
		CHANNEL_VAR_1 = 0x01, /// ģ��1(840-2600MHZ)
		CHANNEL_VAR_2 = 0x02, /// ģ��2(2600-6000MHZ)
		CHANNEL_VAR_3 = 0x03, /// ģ��3(1570-1620MHZ)
		CHANNEL_FIX_1 = 0x04, /// GPS/BD
		CHANNEL_FIX_2 = 0x10, /// 350M
		CHANNEL_FIX_3 = 0x11, /// 430M
		CHANNEL_FIX_4 = 0x12, /// 850M
		CHANNEL_FIX_5 = 0x21, /// 1200M
		CHANNEL_FIX_6 = 0x31, /// 2400M
		CHANNEL_FIX_7 = 0x32, /// 5800M
		CHANNEL_FIX_8 = 0xA1, /// ȫ������ͨ��
		CHANNEL_FIX_9 = 0xB1, /// GPS+ȫ������ͨ��
		CHANNEL_FIX_10 = 0xB2, /// GPS+ȫ������ͨ�� ��ѯ
	};

	enum
	{
		ORDER_CTRL = 0xA1, /// ����
		ORDER_QUERY = 0xB1,/// ��ѯ
		ORDER_STATE = 0xB2,/// ״̬
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
	/// ��ʼλ
	unsigned short start_ = 0;
	/// ָ���ʶ
	unsigned char cmd_ = 0;
	/// Ŀ���豸
	unsigned char dev_ = 0;
	/// ͨ����ʶ
	unsigned char channel_ = 0;
	/// ����
	CDisturberImpl& impl_; /// 7 byte
	/// Ԥ��
	unsigned char reserved_ = 0;
	/// У��
	unsigned char check_ = 0;
	/// ����λ
	unsigned short end_ = 0;
	static const uint32_t protocal_head_len = 9;
};

/// <summary>
/// ���ת�����Ų�ѯ
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
	/// ��ʼƵ��
	unsigned short start_fq_ = 0;
	/// ����Ƶ��
	unsigned short end_fq_ = 0;
	/// ����״̬
	unsigned char state_ = 0;
	/// �¶ȸ澯
	unsigned char temp_alarm_ = 0;
	/// פ���澯
	unsigned char alarm_ = 0;
	static const uint32_t protocal_head_len = 7;
};

/// <summary>
/// ���ת�����ſ���
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
	/// ��ʼƵ�� ����Ϊ1MHZ(840-6000MHz)
	unsigned short start_fq_ = 0;
	/// ����Ƶ��
	unsigned short end_fq_ = 0;
	/// ����״̬
	unsigned char state_ = 0;
	/// �¶ȸ澯
	unsigned char temp_alarm_ = 0;
	/// פ���澯
	unsigned char alarm_ = 0;
	static const uint32_t protocal_head_len = 7;
};

/// <summary>
/// ���ת������״̬
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
	/// ��ʼƵ�� ����Ϊ1MHZ(840-6000MHz)
	unsigned short start_fq_ = 0;
	/// ����Ƶ��
	unsigned short end_fq_ = 0;
	/// <summary>
	/// ����״̬
	/// bit 0:дƵ 1:GPS/DB 2:Ԥ�� 3:430M 4:850M 5:Ԥ�� 6:2400M 7:5800M
	/// 1���� 0�ر�,,�з������
	/// </summary>
	unsigned char state_ = 0;
	/// �¶ȸ澯
	unsigned char temp_alarm_ = 0;
	/// פ���澯
	unsigned char alarm_ = 0;
	static const uint32_t protocal_head_len = 7;
};

/// <summary>
/// ����豸״̬��չ��Ϣ��
/// </summary>
class CCameraStateExtendInfoProtocal : public CMajorProtocal
{
public:
	enum CAMERA_STATE
	{
		CAMERA_STATE_IDLE = 0, //����
		CAMERA_STATE_SEARCH = 1, //����
		CAMERA_STATE_TRACK = 2, //����
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
	/// �����
	unsigned int number_ = 0;
	/// ��統ǰ״̬ʱ���
	unsigned long long timestamp_ = 0;
	/// ����״̬
	unsigned int state_ = 0;
	/// ���ݳ���
	unsigned int data_len_ = 0;
	/// �������
	CCameraStateExtendImpl& impl_;
	static const uint32_t protocal_head_len = 20;
};

/// <summary>
/// ����豸״̬��չ��Ϣ��
/// ����豸״̬(����)��չ��Ϣ��
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
	unsigned int timestamp_ = 0; /**����ʱ��(��)*/
	const uint32_t protocal_head_len = 4;
};

/// <summary>
/// ����豸״̬��չ��Ϣ��
/// ����豸״̬(����)��չ��Ϣ��
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
	unsigned int timestamp_ = 0; /**����ʱ��(��)*/
	unsigned int target_count_ = 0; /**����������Ŀ������*/
	unsigned int data_len_ = 0;
	/// <summary>
	/// Ŀ��
	/// </summary>
	std::array<CTarget, 128> targets_;
	const uint32_t protocal_head_len = 12;
};

/// <summary>
/// ����豸״̬��չ��Ϣ��
/// ����豸״̬(����)��չ��Ϣ��
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
	unsigned int timestamp_ = 0; /**����ʱ��(��)*/
	//unsigned int state_ = 0; /**1-���� 0-��ʧ*/
	unsigned short user_data_ = 0; /**�Զ�������*/
	unsigned short state_ = 0; /**1-���� 0-��ʧ*/
	CTarget target_; /// Ŀ��
	const uint32_t protocal_head_len = 8;
};


/// <summary>
/// ��羵ͷ������Ϣ��
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
	/// �����
	unsigned int number_ = 0;
	/// ϵͳ���
	unsigned int sys_number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// ��������
	/**
		0.��ͷֹͣ�˶�(0x00)
		1. ��ת��ָ��������
		2. ��ת��ָ������
		3. ��ת��ָ��zoom��focusλ��
		4. ��ͷ������Զ(0x04)
		5. ��ͷ��������(0x05)
		6. ��ͷ�㶯��Զ(0x06)
		7. ��ͷ�㶯����(0x07)
		8. ���������Զ(0x08)
		9. �����������(0x09)
		10. ����㶯��Զ(0x0A)
		11. ����㶯����(0x0B)
	*/
	unsigned int cmd_ = 0;
	/** @brief ������/���� ..
		����ͷ��������Ϊ 0x01ʱ��ʾ��ͷ��������, ��λmm ��150mm��
		����������Ϊ0x02ʱ��ʾ��ͷ��������42����
		����ͷ��������Ϊ0x03ʱ���ֶ���Ϊ������ʹ��
	*/
	unsigned int zoom_ = 0;
	/// zoomλ��. ����ͷ��������Ϊ0x03ʱ��Ч����ʾzoom�Ĳ���
	unsigned int zoom_step_ = 0;
	/// Focus λ�� .. ����ͷ��������Ϊ0x03ʱ��Ч����ʾfocus�Ĳ���
	unsigned int focus_step_ = 0;
	/// Ԥ��
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 36;
};


/// <summary>
/// ���ɨ����չ��Ϣ��
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
	/// �����
	uint32_t number_ = 0;
	/// ʱ���
	uint64_t timestamp_ = 0;
	/// �������� 1: ����ɨ�� 0��ֹͣɨ��
	uint32_t cmd_ = 0;
	/// ˮƽ������ʼ�Ƕ�
	uint32_t hor_start_angle_ = 0;
	/// ˮƽ���������Ƕ�
	uint32_t hor_stop_angle_ = 0;
	/// ����������ʼ�Ƕ�  ���������ʼ�ͽ����Ƕ���ȣ���罫��ָ���ĸ���λ���ϳ���ɨ�裬�������ȣ���������ʽ����ɨ��
	uint32_t ver_start_angle_ = 0;
	/// �������������Ƕ�
	uint32_t ver_stop_angle_ = 0;
	/// ɨ���ٶ� ��/��
	uint32_t speed_ = 0;
	/// ����
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
	uint16_t getReserved3() { return reserved3_; } // ����
	uint8_t getReserved() { return reserved_; } // ��ǰAIģ������

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
	// ����
	void setReserved3(uint32_t v) {
		reserved3_ = v;
	}
	// ��ǰAIģ������ 1 -����3���� 2 -�Ե� 80 ����
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
	uint32_t number_ = 0; ///���
	uint32_t type_ = 0; /// ����
	uint32_t same_ = 0; ///��ʶ�� �ٷֱȳ���100
	uint32_t width_ = 0; ///��� ����
	uint32_t height_ = 0; ///�߶� ����
	uint32_t real_width_ = 0;/// ʵ�ʿ�� * 100,��λm,���磺200��ʾ2m, 10��ʾ0.1 m
	uint32_t real_height_ = 0;/// ʵ�ʸ߶� * 100,��λm,���磺200��ʾ2m, 10��ʾ0.1 m
	uint8_t direction_ = 0; /// �˶�����0: ��ֹ;  1: ��;   2: ��;3: ��;    4: ��;   5������;6������;  7: ����; 8: ����;
	uint8_t reserved_ = 0; //�����ֶ�
	uint16_t reserved2_ = 0;  //�����ֶ�
	uint16_t hor_speed_ = 0;  //ˮƽ�ٶ� ���� / ��
	uint16_t ver_speed_ = 0;  //��ֱ�ٶ� ���� / ��
	uint16_t real_hor_speed_ = 0;  // ˮƽ�����ٶ� �ٶ� * 100������253��ʾ2.53 m/s
	uint16_t real_ver_speed_ = 0;  // ��ֱ�����ٶ� �ٶ� * 100������253��ʾ2.53 m/s
	double azimuth_ = 0; // ��λ�� ����Ŀ������Ƶ�е�λ��ƫ���Լ���ǰ���ķ�λ�Ƕȼ������
	double pitch_angle_ = 0; // ������ ����Ŀ������Ƶ�е�λ��ƫ���Լ���ǰ���ĸ����Ƕȼ������
	uint32_t reserved3_ = 0; //�����ֶ� ����
};

/// <summary>
/// ���Ŀ���ϱ���Ϣ��
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
	/// ����ʱ�䣨�룩
	uint32_t timestamp_ = 0;
	/// ��ǰ�ӳ��м�������Ŀ������
	uint32_t target_count_ = 0;
	/// �������ݳ���
	uint32_t data_len_ = 0;
	/// <summary>
	/// Ŀ��
	/// </summary>
	std::array<CTargetEx, 128> targets_;
	const uint32_t protocal_head_len = 12;
};

/// <summary>
/// ��羵ͷ״̬��չ��Ϣ��
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
	uint32_t number_ = 0; ///���
	uint64_t timestamp_ = 0; /// ʱ���
	uint32_t light_hor_view_angle_ = 0; /// �ɼ��⾵ͷ��ǰˮƽ�ӳ��� ˮƽ�ӳ��� * 100 ��2532��ʾ25.32��
	uint32_t light_ver_view_angle_ = 0; /// �ɼ��⾵ͷ��ǰ��ֱ�ӳ��� ��ֱ�ӳ��� * 100 ��1532��ʾ15.32��
	uint32_t thermography_hor_view_angle_ = 0; /// ����ͷ��ǰˮƽ�ӳ��� ˮƽ�ӳ��� * 100 ��2532��ʾ25.32��
	uint32_t thermography_ver_view_angle_ = 0; /// ����ͷ��ǰ��ֱ�ӳ��� ��ֱ�ӳ��� * 100 ��1532��ʾ15.32��
	uint32_t light_zoom_ = 0; /// �ɼ��⾵ͷ��ǰ���� 
	uint32_t thermography_zoom_ = 0; /// ����ͷ��ǰ���� 
	uint32_t focus_ = 0; // ��ͷ��ǰ������ ��ͷ��ǰ������ * 100 ��1500��ʾ��ǰ������Ϊ15mm
	uint32_t reserved_ = 0; // ����
	const uint32_t protocal_head_len = 44;
};

/// <summary>
/// ת̨��λ������Ϣ��
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
	uint32_t number_ = 0; ///���
	uint64_t timestamp_ = 0; /// ʱ���
	uint32_t cmd_ = 0; /// 
	uint32_t speed_ = 0; /// ��Чֵ 0-254
	uint32_t mode_ = 0; /// 0--�����˶���ֱ���յ�ָֹͣ�1--�����˶�(ָ��ִ��һ���Զ�ֹͣ)
	uint32_t reserved_ = 0; // ����
	const uint32_t protocal_head_len = 28;
};

/// <summary>
/// �л�������ƵԴ��Ϣ��
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
	uint32_t number_ = 0; ///���
	uint64_t timestamp_ = 0; /// ʱ���
	uint32_t source_ = 0; /// ������ƵԴͨ 0-�ɼ��� 1-����
	uint32_t reserved_ = 0; // ����
	const uint32_t protocal_head_len = 20;
};

/// <summary>
/// �Ѱ����ϱ���Ϣ��
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
	/// �����
	unsigned int number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// ������ƵԴͨ��  0 -- �ɼ��� 1 --����
	unsigned int channel_ = 0;
	/// ˮƽ�Ѱ���ƫ�� (����) ������
	int hor_offset_ = 0;
	/// �����Ѱ���ƫ�� (����) �¸�����
	int ver_offset_ = 0;
	/// Ԥ��
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 28;
};


/// <summary>
/// �ֶ�����Ŀ��
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
	/// �����
	unsigned int number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// ������ƵԴͨ��  0 -- �ɼ��� 1 --����
	unsigned int channel_ = 0;
	/// Ŀ��λ������ (����) ��Ƶ���������Ͷ�䵽 704 X 576 ����
	int x_ = 0;
	int y_ = 0;
	/// Ԥ��
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 28;
};

/// <summary>
/// �����Χ�豸����
/// </summary>
class CFDOCtrl : public CMajorProtocal
{
public:
	enum fdo_cmd
	{
		fdo_cmd_laser_ranging =1, // ���Ƽ�������
		fdo_cmd_laser_position = 2, //  ���Ƽ�����ջ�
		fdo_cmd_laser_light = 3, // ���Ƽ��ⲹ��
		fdo_cmd_heating = 4, // ���Ƽ�����
		fdo_cmd_thermal_compressor= 5, // �����ȳ���ѹ����
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
	/// �����
	unsigned int number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// �������� �μ� fdo_cmd
	unsigned int cmd_ = 0;
	/// ���Ʋ���  1������ 0���ر�
	unsigned int params_ = 0;
	/// Ԥ��
	unsigned int reserved_ = 0;
	static const uint32_t protocal_head_len = 24;
};

/// <summary>
/// AI ��������
/// </summary>
class CAIModelCtrl : public CMajorProtocal
{
public:
	enum params_cmd
	{
		params_cmd_mode_cfg = 1, // AI ģ���л�
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
	/// �����
	unsigned int number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// ������������ �μ� params_cmd  1. AI ģ���л� 2. ����
	unsigned int cmd_ = 0;
	/// �������� �����������ݵĳ���. ���ݡ�������������������峤��.�� �������������Ϊ1 ʱ : ����Ϊ4
	unsigned int params_length_ = 0;
	CAIParamImpl& impl_;
	static const uint32_t protocal_head_len = 20;
};

/// <summary>
/// AI ��������
/// AI ģ���л� ����
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
	unsigned int template_ = 0; /// 1: ѡ��AIģ��1(���� 3 ����)  2: ѡ�� AIģ��2(�Ե�80����)
	const uint32_t protocal_head_len = 4;
};

/// <summary>
/// ���þ۽�ģʽ
/// </summary>
class CFocusMode : public CMajorProtocal
{
public:
	enum focus_mode
	{
		focus_mode_auto =0, // �Զ�(AF)
		focus_mode_semiauto = 1, // ���Զ�(ZF)
		focus_mode_manual = 2, // �ֶ���MF��
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
	/// �����
	unsigned int number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// �۽�ģʽ �μ� focus_mode 
	unsigned int mode_ = 0;
	/// ͨ�� 0-�ɽ��⾵ͷ 1-����ͷ
	uint8_t channel_ = 0;
	/// Ԥ��
	uint8_t reserved_[3];
	static const uint32_t protocal_head_len = 20;
};

/// <summary>
/// ���ù����ٲ���
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
	/// 0 -������ 1 -����
	/// </summary>
	/// <param name="v"></param>
	void setAutoZoom(unsigned int v) { auto_zoom_enable_ = v; }
	/// <summary>
	/// �Զ��䱶�Ŵ���ֵ 
	/// ���ٵ�Ŀ���С(��*��) ���С�ڸ���ֵ����ͷ���Զ��Ŵ�һ�Ρ��Ƽ�ֵΪ40�� ��λ������
	/// </summary>
	/// <param name="v"></param>
	void setZoominThreshold(uint32_t v) { zoomin_threshold_ = v; }
	/// <summary>
	/// �Զ��䱶��С��ֵ 
	/// ���ٵ�Ŀ���С(��*��) ������ڸ���ֵ����ͷ���Զ���Сһ�Ρ��Ƽ�ֵΪ100�� ��λ������
	/// </summary>
	/// <param name="v"></param>
	void setZoomoutThreshold(uint32_t v) { zoomout_threshold_ = v; }
protected:
	unsigned int getMajorCmd() { return UAV_CTRL_TRACK_LENSE_CFG; }
private:
	/// �����
	unsigned int number_ = 0;
	/// ʱ���
	unsigned long long timestamp_ = 0;
	/// �Ƿ������Զ��䱶���� 0 -������ 1 -����
	unsigned int auto_zoom_enable_ = 0;
	/// �Զ��䱶�Ŵ���ֵ ���ٵ�Ŀ���С(��*��) ���С�ڸ���ֵ����ͷ���Զ��Ŵ�һ�Ρ��Ƽ�ֵΪ40�� ��λ������
	unsigned int zoomin_threshold_ = 40;
	/// �Զ��䱶��С��ֵ ���ٵ�Ŀ���С(��*��) ������ڸ���ֵ����ͷ���Զ���Сһ�Ρ��Ƽ�ֵΪ100�� ��λ������
	unsigned int zoomout_threshold_ = 100;
	/// Ԥ��
	uint8_t reserved_[32];
	static const uint32_t protocal_head_len = 56;
};

class CStateExtendProtocal : public CMajorProtocal
{
public:
	enum focus_mode
	{
		focus_mode_auto = 0, // �Զ�(AF)
		focus_mode_semiauto = 1, // ���Զ�(ZF)
		focus_mode_manual = 2, // �ֶ���MF��
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
	/// �����
	unsigned int number_ = 0;
	/// ��統ǰ״̬ʱ���
	unsigned long long timestamp_ = 0;
	/// ��������״̬ 0 -�ر� 1 -����
	unsigned int laser_distance_ = 0;
	/// ������ջ�״̬ 0 -�ر� 1 -����
	unsigned int laser_hit_ = 0;
	/// ���ⲹ��״̬ 0 -�ر� 1 -����
	unsigned int laser_light_ = 0;
	/// ������״̬ 0 -�ر� 1 -����
	unsigned int heating_ = 0;
	/// �ȳ���ѹ����״̬ 0 -�ر� 1 -����
	unsigned int thermal_compressor_ = 0;
	/// �ɼ���۽�ģʽ 0 -AF(�Զ�) 1 -ZF(���Զ�) 2 -MF(�ֶ�)
	unsigned int visiable_focus_ = 0;
	/// �ȳ���۽�ģʽ 0 -AF(�Զ�) 1 -ZF(���Զ�) 2 -MF(�ֶ�)
	unsigned int thermal_focus_ = 0;
	uint8_t reserved_[32];
	const uint32_t protocal_head_len = 72;
};

/// <summary>
/// ���ù��Ŀַ��չ��Ϣ��
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
	/// ����� 1
	unsigned int number_ = 0;
	/// ϵͳ��� 2
	unsigned int sys_number_ = 0;
	/// ϵͳ�·� ʱ��� 3
	unsigned long long timestamp_ = 0;
	/// Ŀ�꾭�� 4
	double lng_ = 0;
	/// Ŀ��γ�� 5
	double lat_ = 0;
	/// Ŀ��߶� 6
	double height_ = 0;
	/// Ŀ����� 7
	double distance_ = 0;
	/// ˮƽ�Ƕ� 7
	double hor_angle_ = 0;
	/// �����Ƕ� 8
	double ver_angle_ = 0;
	/// �û�����
	unsigned short user_data_ = 0;
	/// Ԥ��
	//uint8_t reserved_[2] = {0};
	/// 0-	��λ������������ 1 - ���ȡ�γ�ȡ��߶�
	unsigned char guide_type_ = 0;
	/// 0 --�ֽ� 1--Զ��
	unsigned char target_direction_ = 0;
	/// �Ƿ�������
	uint32_t search_ = 0;
	/// ʵ��ֵ * 100���� 15 ��ʾ������ˮƽ�ӳ���Ϊ0.15��
	uint32_t hor_viewing_angle_ = 0;
	/// ʵ��ֵ * 100���� 15 ��ʾ�����Ĵ�ֱ�ӳ���Ϊ0.15��
	uint32_t ver_viewing_angle_ = 0;
	uint8_t reserved_[4];
	const uint32_t protocal_head_len = 84;
};

UAV_PRO_NAMESPACE_END