#include "CameraUavProtocal.h"
#include <Windows.h>
#include <spdlog\logger.hpp>

UAV_PRO_NAMESPACE_BEGIN

CCameraUavProtocal::CCameraUavProtocal(CMajorProtocal& major)
	:major_(major)
{

}

bool CCameraUavProtocal::parse(COnlyReadBuffer& buf, std::function<void(CCameraUavProtocal&)> cb)
{
	if (!cb) {
		return false;
	}
	if (buf.readableBytes() <= 36)
	{
		SPDLOG_DEBUG("数据大小错误");
		return false;
	}
	uint32_t start_bit = buf.readUInt32();
	uint32_t pro_number = buf.readUInt32();
	uint32_t packet_len = buf.readUInt32();
	uint32_t cmd = buf.readUInt32();
	uint64_t timestamp = buf.readUInt64();

	CMajorProtocal* pMajor = NULL;
	
	if (UAV_CTRL_PRO_CMD_STATUS == cmd)
	{
		CStateProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_PRO_CMD_POSITION == cmd) {
		CPositionProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_PRO_CMD_TARGET_ADDR == cmd) {
		CTargetDirectionProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_PRO_CMD_TRACK == cmd) {
		CCameraCtrlProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if ((UAV_CTRL_PRO_CMD_INTERFERE_QUERY == cmd) || (UAV_CTRL_PRO_CMD_INTERFERE_CTRL == cmd) || (UAV_CTRL_PRO_CMD_INTERFERE_STATUS == cmd)) {
		return CDisturberProtocal::parse(buf, [&](CDisturberProtocal& pro) {
			uint32_t message_index = buf.readUInt32();
			uint32_t verify = buf.readUInt32();
			uint32_t stop_bit = buf.readUInt32();

			CCameraUavProtocal mainPro(pro);
			mainPro.start_bit_ = start_bit;
			mainPro.pro_number_ = pro_number;
			mainPro.packet_len_ = packet_len;
			mainPro.cmd_ = cmd;
			mainPro.timestamp_ = timestamp;
			mainPro.message_index_ = message_index;
			mainPro.verify_ = verify;
			mainPro.stop_bit_ = stop_bit;

			if (!mainPro.valid())
			{
				SPDLOG_DEBUG("数据类型:DISTURBER,解析失败");
				return;
			}
			cb(mainPro);
			});
	}
	else if (UAV_CTRL_PRO_CMD_STATUS_EXTEND == cmd) {
		return CCameraStateExtendInfoProtocal::parse(buf, [&](CCameraStateExtendInfoProtocal& pro) {
			uint32_t message_index = buf.readUInt32();
			uint32_t verify = buf.readUInt32();
			uint32_t stop_bit = buf.readUInt32();

			CCameraUavProtocal mainPro(pro);
			mainPro.start_bit_ = start_bit;
			mainPro.pro_number_ = pro_number;
			mainPro.packet_len_ = packet_len;
			mainPro.cmd_ = cmd;
			mainPro.timestamp_ = timestamp;
			mainPro.message_index_ = message_index;
			mainPro.verify_ = verify;
			mainPro.stop_bit_ = stop_bit;

			if (!mainPro.valid())
			{
				SPDLOG_DEBUG("数据类型:UAV_CTRL_PRO_CMD_STATUS_EXTEND,解析失败");
				return;
			}
			cb(mainPro);
			});
	}
	else if (UAV_CTRL_PRO_CMD_CAMERA_LENS == cmd) {
		CCameraLensInfoProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_SEARCH_EXTEND == cmd) {
		CSearchExtendProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_NOTIFY_TARGET == cmd) {
		CNotifyTarget major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_LENS_STATE == cmd) {
		CLensState major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_PRO_DEVIATION_INFO == cmd) {
		CDeviationInfoProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else if (UAV_CTRL_STATE_EXTEND == cmd) {
		CStateExtendProtocal major;
		if (major.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pMajor = &major;
	}
	else {
		SPDLOG_DEBUG("参数错误,cmd:{}", cmd);
		return false;
	}

	if (NULL == pMajor) {
		SPDLOG_DEBUG("解析失败");
		return false;
	}
	uint32_t message_index = buf.readUInt32();
	uint32_t verify = buf.readUInt32();
	uint32_t stop_bit = buf.readUInt32();

	CCameraUavProtocal pro(*pMajor);
	pro.start_bit_ = start_bit;
	pro.pro_number_ = pro_number;
	pro.packet_len_ = packet_len;
	pro.cmd_ = cmd;
	pro.timestamp_ = timestamp;
	pro.message_index_ = message_index;
	pro.verify_ = verify;
	pro.stop_bit_ = stop_bit;

	if (!pro.valid())
	{
		SPDLOG_DEBUG("数据类型:{},解析失败", cmd);
		return false;
	}
	cb(pro);

	return true;
}

int CCameraUavProtocal::construct(CFixBuffer& buf)
{
	BYTE* lpBit = (BYTE*)&start_bit_;
	lpBit[0] = 0x88;
	lpBit[1] = 0x89;
	lpBit[2] = 0x80;
	lpBit[3] = 0x8A;

	lpBit = (BYTE*)&stop_bit_;
	lpBit[0] = 0x89;
	lpBit[1] = 0x80;
	lpBit[2] = 0x8A;
	lpBit[3] = 0x8B;
	pro_number_ = 9002;

	CFixBuffer major_buf;
	int iDataLen = major_.construct(major_buf);
	if ((iDataLen <= 0) || (iDataLen != major_buf.readableBytes()))
	{
		return -1;
	}
	packet_len_ = iDataLen + 20;

	if (buf.writableBytes() < packet_len_ + 16)
	{
		return -2;
	}
	cmd_ = major_.getMajorCmd();

	buf.appendUInt32(start_bit_);
	buf.appendUInt32(pro_number_);
	buf.appendUInt32(packet_len_);
	buf.appendUInt32(cmd_);
	buf.appendUInt64(timestamp_);
	buf.append(major_buf.peek(), major_buf.readableBytes());
	buf.appendUInt32(message_index_);
	/// 错误校验 
	/**
	* 错误校验码采用校验和方法。
	* 协议体中从“包长度”到“信息序列号” （包括“包长度”、 “信息序列号”）这部分数据。
	*/
	
	const char* pos = buf.buf();
	int verify = 0;
	int size = buf.readableBytes();
	for (int i = 8; i < size; i++)
	{
		verify += pos[i];
	}
	verify_ = verify;
	buf.appendUInt32(verify_);
	buf.appendUInt32(stop_bit_);
	return buf.readableBytes();
}

bool CCameraUavProtocal::valid()
{
	unsigned int start_bit;
	BYTE* lpBit = (BYTE*)&start_bit;
	lpBit[0] = 0x88;
	lpBit[1] = 0x89;
	lpBit[2] = 0x80;
	lpBit[3] = 0x8A;
	if (start_bit != start_bit_)
	{
		SPDLOG_DEBUG("开始位错误");
		return false;
	}

	/// 停止位
	unsigned int stop_bit;
	lpBit = (BYTE*)&stop_bit;
	lpBit[0] = 0x89;
	lpBit[1] = 0x80;
	lpBit[2] = 0x8A;
	lpBit[3] = 0x8B;
	if (stop_bit != stop_bit_)
	{
		SPDLOG_DEBUG("结束位错误");
		return false;
	}
	return true;
}

int CTarget::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		//SPDLOG_DEBUG("数据错误, 可读:{}, 需要:{}", buf.readableBytes(), protocal_head_len);
		return -1;
	}
	number_ = buf.readUInt32();
	type_ = buf.readUInt32();
	same_ = buf.readUInt32();
	width_ = buf.readUInt32();
	height_ = buf.readUInt32();
	angle_ = buf.readDouble();
	elevation_ = buf.readDouble();
	return beginReadableBytes - buf.readableBytes();
}

int CTarget::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(number_);
	buf.appendUInt32(type_);
	buf.appendUInt32(same_);
	buf.appendUInt32(width_);
	buf.appendUInt32(height_);
	buf.appendDouble(angle_);
	buf.appendDouble(elevation_);
	return beginWritableBytes - buf.writableBytes();
}

int CStateProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	state_ = buf.readUInt32();
	mode_ = buf.readUInt32();
	buf.retrieve(sizeof(code_));
	//buf.retrieve(sizeof(reserved_));
	track_code_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CStateProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(state_);
	buf.appendUInt32(mode_);
	ZeroMemory(code_, sizeof(code_));
	buf.append(code_, sizeof(code_));
	//ZeroMemory(reserved_, sizeof(reserved_));
	//buf.append(reserved_, sizeof(reserved_));
	buf.appendUInt32(track_code_);
	return beginWritableBytes - buf.writableBytes();
}

bool CStateProtocal::valid()
{
	return true;
}

int CPositionProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	hor_angle_ = buf.readDouble();
	ver_angle_ = buf.readDouble();
	distance_ = buf.readDouble();
	hor_speed_ = buf.readDouble();
	ver_speed_ = buf.readDouble();
	height_ = buf.readInt16();
	lens_zoom_ = buf.readInt8();
	buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CPositionProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendDouble(hor_angle_);
	buf.appendDouble(ver_angle_);
	buf.appendDouble(distance_);
	buf.appendDouble(hor_speed_);
	buf.appendDouble(ver_speed_);
	buf.appendInt16(height_);
	buf.appendInt8(lens_zoom_);
	ZeroMemory(reserved_, sizeof(reserved_));
	buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

bool CPositionProtocal::valid()
{
	return true;
}

int CTargetDirectionProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	sys_number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	lng_ = buf.readDouble();
	lat_ = buf.readDouble();
	height_ = buf.readDouble();
	distance_ = buf.readDouble();
	hor_angle_ = buf.readDouble();
	ver_angle_ = buf.readDouble();
	user_data_ = buf.readUInt16();
	guide_type_ = buf.readUInt8();
	target_direction_ = buf.readUInt8();
	//buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CTargetDirectionProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(number_);
	buf.appendUInt32(sys_number_);
	buf.appendUInt64(timestamp_);
	buf.appendDouble(lng_);
	buf.appendDouble(lat_);
	buf.appendDouble(height_);
	buf.appendDouble(distance_);
	buf.appendDouble(hor_angle_);
	buf.appendDouble(ver_angle_);
	buf.appendUInt16(user_data_);
	buf.appendUInt8(guide_type_);
	buf.appendUInt8(target_direction_);
	//ZeroMemory(reserved_, sizeof(reserved_));
	//buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

bool CTargetDirectionProtocal::valid()
{
	return true;
}

int CCameraCtrlProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	cmd_ = buf.readUInt32();
	hor_start_angle_ = buf.readInt32();
	hor_end_angle_ = buf.readInt32();
	ver_start_angle_ = buf.readInt32();
	ver_end_angle_ = buf.readInt32();
	memcpy(reserved_, buf.peek(), sizeof(reserved_));
	buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CCameraCtrlProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(cmd_);
	buf.appendInt32(hor_start_angle_);
	buf.appendInt32(hor_end_angle_);
	buf.appendInt32(ver_start_angle_);
	buf.appendInt32(ver_end_angle_);
	buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

void CCameraCtrlProtocal::setTargetNumber(int v)
{
	*(unsigned int*)(reserved_) = v;
}

void CCameraCtrlProtocal::setSearchMode(int v)
{
	*(unsigned int*)(reserved_) = v;
}

bool CCameraCtrlProtocal::valid()
{
	return true;
}

CDisturberProtocal::CDisturberProtocal(CDisturberImpl& impl)
	:impl_(impl)
{

}

bool CDisturberProtocal::parse(COnlyReadBuffer& buf, std::function<void(CDisturberProtocal&)> cb)
{
	if (!cb) {
		OutputDebugString("CDisturberProtocal 1");
		return false;
	}
	if (buf.readableBytes() <= protocal_head_len)
	{
		OutputDebugString("CDisturberProtocal 11");
		return false;
	}
	uint16_t start = buf.readUInt16();
	uint8_t cmd = buf.readUInt8();
	uint8_t dev = buf.readUInt8();
	uint8_t channel = buf.readUInt8();

	CDisturberImpl* pImpl = NULL;
	if (ORDER_CTRL == cmd)
	{
		CQueryDisturberProtocal impl;
		if (impl.parse(buf) <= 0)
		{
			OutputDebugString("CQueryDisturberProtocal x");
			return false;
		}
		pImpl = &impl;
	}
	else if (ORDER_QUERY == cmd) {
		CCtrlDisturberProtocal impl;
		if (impl.parse(buf) <= 0)
		{
			OutputDebugString("CCtrlDisturberProtocal x");
			return false;
		}
		pImpl = &impl;
	}
	else if (ORDER_STATE == cmd) {
		CStateDisturberProtocal impl;
		if (impl.parse(buf) <= 0)
		{
			OutputDebugString("CStateDisturberProtocal x");
			return false;
		}
		pImpl = &impl;
	}
	else {
		OutputDebugString("1111");
		return false;
	}

	uint8_t reserved = buf.readUInt8();
	uint8_t check = buf.readUInt8();
	uint16_t end = buf.readUInt16();

	CDisturberProtocal pro(*pImpl);
	pro.start_ = start;
	pro.cmd_ = cmd;
	pro.dev_ = dev;
	pro.channel_ = channel;
	pro.reserved_ = reserved;
	pro.check_ = check;
	pro.end_ = end;

	if (!pro.valid())
	{
		return false;
	}
	cb(pro);

	return true;
}

int CDisturberProtocal::parse(COnlyReadBuffer& buf)
{
	return 0;
}

int CDisturberProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	dev_ = 0xA0; /// 干扰机
	start_ = 0x5F5F;
	end_ = 0xF5F5;
	cmd_ = getImpl().getDisturberCmd();

	buf.appendUInt16(start_);
	BYTE* pos = (BYTE*)buf.beginWrite();
	buf.appendUInt8(cmd_);
	buf.appendUInt8(dev_);
	buf.appendUInt8(channel_);
	impl_.construct(buf);
	buf.appendUInt8(reserved_);
	int len = buf.beginWrite() - (char*)pos;
	int tmp = 0;
	for (int i = 0; i < len; i++)
	{
		tmp += static_cast<int>(pos[i]);
	}
	tmp = tmp & 0XFF;
	check_ = static_cast<unsigned char>(tmp);

	buf.appendUInt8(check_);
	buf.appendUInt16(end_);
	return beginWritableBytes - buf.writableBytes();
}

bool CDisturberProtocal::valid()
{
	return true;
}

int CQueryDisturberProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	start_fq_ = buf.readInt16Ex();
	end_fq_ = buf.readInt16Ex();
	state_ = buf.readUInt8();
	temp_alarm_ = buf.readUInt8();
	alarm_ = buf.readUInt8();
	return beginReadableBytes - buf.readableBytes();
}

int CQueryDisturberProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt16Ex(start_fq_);
	buf.appendUInt16Ex(end_fq_);
	buf.appendUInt8(state_);
	buf.appendUInt8(temp_alarm_);
	buf.appendUInt8(alarm_);
	return beginWritableBytes - buf.writableBytes();
}

bool CQueryDisturberProtocal::valid()
{
	return true;
}

int CCtrlDisturberProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	start_fq_ = buf.readInt16Ex();
	end_fq_ = buf.readInt16Ex();
	state_ = buf.readUInt8();
	temp_alarm_ = buf.readUInt8();
	alarm_ = buf.readUInt8();
	return beginReadableBytes - buf.readableBytes();
}

int CCtrlDisturberProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt16Ex(start_fq_);
	buf.appendUInt16Ex(end_fq_);
	buf.appendUInt8(state_);
	buf.appendUInt8(temp_alarm_);
	buf.appendUInt8(alarm_);
	return beginWritableBytes - buf.writableBytes();
}

bool CCtrlDisturberProtocal::valid()
{
	return true;
}

int CStateDisturberProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	start_fq_ = buf.readInt16Ex();
	end_fq_ = buf.readInt16Ex();
	state_ = buf.readUInt8();
	temp_alarm_ = buf.readUInt8();
	alarm_ = buf.readUInt8();
	return beginReadableBytes - buf.readableBytes();
}

int CStateDisturberProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt16Ex(start_fq_);
	buf.appendUInt16Ex(end_fq_);
	buf.appendUInt8(state_);
	buf.appendUInt8(temp_alarm_);
	buf.appendUInt8(alarm_);
	return beginWritableBytes - buf.writableBytes();
}

bool CStateDisturberProtocal::valid()
{
	return true;
}

CCameraStateExtendInfoProtocal::CCameraStateExtendInfoProtocal(CCameraStateExtendImpl& impl)
	:impl_(impl)
{

}

bool CCameraStateExtendInfoProtocal::parse(COnlyReadBuffer& buf, std::function<void(CCameraStateExtendInfoProtocal&)> cb)
{
	if (!cb) {
		return false;
	}
	if (buf.readableBytes() <= CCameraStateExtendInfoProtocal::protocal_head_len)
	{
		SPDLOG_DEBUG("解析失败");
		return false;
	}
	uint32_t number = buf.readUInt32();
	uint64_t timestamp = buf.readUInt64();
	uint32_t state = buf.readUInt32();
	uint32_t data_len = buf.readUInt32();

	CCameraStateExtendImpl* pImpl = NULL;
	CCameraIdleStateProtocal implIdle;
	CCameraSearchStateProtocal implSearch;
	CCameraTrackStateProtocal implTrack;
	if (CAMERA_STATE_IDLE == state)
	{
		if (implIdle.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pImpl = &implIdle;
	}
	else if (CAMERA_STATE_SEARCH == state) {

		if (implSearch.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pImpl = &implSearch;
	}
	else if (CAMERA_STATE_TRACK == state) {

		if (implTrack.parse(buf) <= 0)
		{
			//SPDLOG_DEBUG("解析失败");
			return false;
		}
		pImpl = &implTrack;
	}
	else {
		SPDLOG_DEBUG("解析失败");
		return false;
	}

	CCameraStateExtendInfoProtocal pro(*pImpl);
	pro.number_ = number;
	pro.timestamp_ = timestamp;
	pro.state_ = state;
	pro.data_len_ = data_len;
	if (!pro.valid())
	{
		return false;
	}
	cb(pro);

	return true;
}

int CCameraStateExtendInfoProtocal::parse(COnlyReadBuffer& buf)
{
	return 0;
}

int CCameraStateExtendInfoProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(state_);
	const char* posDataLen = buf.beginWrite();
	buf.appendUInt32(data_len_);
	int len = impl_.construct(buf);
	if (len <= 0) {
		return -2;
	}
	*static_cast<uint32_t*>((void*)posDataLen) = (uint32_t)len;
	return beginWritableBytes - buf.writableBytes();
}

bool CCameraStateExtendInfoProtocal::valid()
{
	return true;
}


int CCameraIdleStateProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	timestamp_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CCameraIdleStateProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(timestamp_);
	return beginWritableBytes - buf.writableBytes();
}

bool CCameraIdleStateProtocal::valid()
{
	return true;
}

int CCameraSearchStateProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	timestamp_ = buf.readUInt32();
	target_count_ = buf.readUInt32();
	data_len_ = buf.readUInt32();
	if (data_len_ <= 0)
	{
		return beginReadableBytes - buf.readableBytes();
	}
	if (target_count_ < 0 || target_count_ > targets_.max_size())
	{
		return - 2;
	}
	for (uint32_t i = 0; i < target_count_; i++)
	{
		targets_.at(i).parse(buf);
	}
	return beginReadableBytes - buf.readableBytes();
}

int CCameraSearchStateProtocal::construct(CFixBuffer& buf)
{
	return 0;
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(timestamp_);
	buf.appendUInt32(target_count_);
	buf.appendUInt32(data_len_);
	/// add....
	//buf.append((const char*)data_.buf.data(), data_.data_len);
	return beginWritableBytes - buf.writableBytes();
}

bool CCameraSearchStateProtocal::valid()
{
	return true;
}

void CCameraSearchStateProtocal::getTargets(std::function<void(CTarget)> cb)
{
	if (!cb) { return; }
	if (target_count_ <= 0 || target_count_ > targets_.max_size())
	{
		return;
	}
	for (uint32_t i = 0; i < target_count_; i++)
	{
		cb(targets_.at(i));
	}
}

int CCameraTrackStateProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	timestamp_ = buf.readUInt32();
	user_data_ = buf.readUInt16();
	state_ = buf.readUInt16();
	if (target_.parse(buf) <= 0) {
		return -2;
	}
	return beginReadableBytes - buf.readableBytes();
}

int CCameraTrackStateProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(timestamp_);
	buf.appendUInt16(user_data_);
	buf.appendUInt16(state_);
	if (target_.construct(buf) <= 0) {
		return -2;
	}
	return beginWritableBytes - buf.writableBytes();
}

bool CCameraTrackStateProtocal::valid()
{
	return true;
}

int CCameraLensInfoProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	sys_number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	cmd_ = buf.readUInt32();
	zoom_ = buf.readUInt32();
	zoom_step_ = buf.readUInt32();
	focus_step_ = buf.readUInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CCameraLensInfoProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt32(sys_number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(cmd_);
	buf.appendUInt32(zoom_);
	buf.appendUInt32(zoom_step_);
	buf.appendUInt32(focus_step_);
	buf.appendUInt32(reserved_);
	return beginWritableBytes - buf.writableBytes();
}

bool CCameraLensInfoProtocal::valid()
{
	return true;
}

int CSearchExtendProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	cmd_ = buf.readUInt32();
	hor_start_angle_ = buf.readUInt32();
	hor_stop_angle_ = buf.readUInt32();
	ver_start_angle_ = buf.readUInt32();
	ver_stop_angle_ = buf.readUInt32();
	speed_ = buf.readUInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CSearchExtendProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(cmd_);
	buf.appendUInt32(hor_start_angle_);
	buf.appendUInt32(hor_stop_angle_);
	buf.appendUInt32(ver_start_angle_);
	buf.appendUInt32(ver_stop_angle_);
	buf.appendUInt32(speed_);
	buf.appendUInt32(reserved_);
	return beginWritableBytes - buf.writableBytes();
}

bool CSearchExtendProtocal::valid()
{
	return true;
}

int CTargetEx::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	type_ = buf.readUInt32();
	same_ = buf.readUInt32();
	width_ = buf.readUInt32();
	height_ = buf.readUInt32();
	real_width_ = buf.readUInt32();
	real_height_ = buf.readUInt32();
	direction_ = buf.readUInt8();
	reserved_ = buf.readUInt8();
	reserved2_ = buf.readUInt16();
	hor_speed_ = buf.readUInt16();
	ver_speed_ = buf.readUInt16();
	real_hor_speed_ = buf.readUInt16();
	real_ver_speed_ = buf.readUInt16();
	azimuth_ = buf.readDouble();
	pitch_angle_ = buf.readDouble();
	reserved3_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CTargetEx::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt32(type_);
	buf.appendUInt32(same_);
	buf.appendUInt32(width_);
	buf.appendUInt32(height_);
	buf.appendUInt32(real_width_);
	buf.appendUInt32(real_height_);
	buf.appendUInt8(direction_);
	buf.appendUInt8(reserved_);
	buf.appendUInt16(reserved2_);
	buf.appendUInt16(hor_speed_);
	buf.appendUInt16(ver_speed_);
	buf.appendUInt16(real_hor_speed_);
	buf.appendUInt16(real_ver_speed_);
	buf.appendDouble(azimuth_);
	buf.appendDouble(pitch_angle_);
	buf.appendUInt32(reserved3_);
	return beginWritableBytes - buf.writableBytes();
}

int CNotifyTarget::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		SPDLOG_DEBUG("解析失败");
		return -1;
	}
	timestamp_ = buf.readUInt32();
	target_count_ = buf.readUInt32();
	data_len_ = buf.readUInt32();
	if (data_len_ <= 0)
	{
		return beginReadableBytes - buf.readableBytes();
	}
	if (target_count_ < 0 || target_count_ > targets_.max_size())
	{
		SPDLOG_DEBUG("解析失败");
		return -2;
	}
	
	for (uint32_t i = 0; i < target_count_; i++)
	{
		targets_.at(i).parse(buf);
	}
	return beginReadableBytes - buf.readableBytes();
}

int CNotifyTarget::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(timestamp_);
	buf.appendUInt32(target_count_);

	const char* posDataLen = buf.beginWrite();
	buf.appendUInt32(data_len_);

	int length = 0, lengthCount=0;
	for (int i = 0; i < target_count_; i++)
	{
		length = targets_[i].construct(buf);
		if (length <=0)
		{
			break;
		}
		lengthCount += length;
	}
	*static_cast<uint32_t*>((void*)posDataLen) = (uint32_t)lengthCount;
	return beginWritableBytes - buf.writableBytes();
}

bool CNotifyTarget::valid()
{
	return true;
}

void CNotifyTarget::getTargets(std::function<void(CTargetEx)> cb)
{
	if (!cb) { return; }
	if (target_count_ <= 0 || target_count_ > targets_.max_size())
	{
		return;
	}
	for (uint32_t i = 0; i < target_count_; i++)
	{
		cb(targets_.at(i));
	}
}

void CNotifyTarget::setTargets(std::array<CTargetEx, 16>& t, int count)
{
	if (count < 0 || count > t.max_size()) {
		target_count_ = 0;
		return;
	}
	target_count_ = count;
	for (uint32_t i = 0; i < count; i++)
	{
		targets_[i] = t[i];
	}
}

int CLensState::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		SPDLOG_DEBUG("解析失败");
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	light_hor_view_angle_ = buf.readUInt32();
	light_ver_view_angle_ = buf.readUInt32();
	thermography_hor_view_angle_ = buf.readUInt32();
	thermography_ver_view_angle_ = buf.readUInt32();
	light_zoom_ = buf.readUInt32();
	thermography_zoom_ = buf.readUInt32();
	focus_ = buf.readUInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CLensState::construct(CFixBuffer& buf)
{
	return -1;
}

bool CLensState::valid()
{
	return true;
}

int CCtrlPtz::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	cmd_ = buf.readUInt32();
	speed_ = buf.readUInt32();
	mode_ = buf.readUInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CCtrlPtz::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(cmd_);
	buf.appendUInt32(speed_);
	buf.appendUInt32(mode_);
	buf.appendUInt32(reserved_);
	return beginWritableBytes - buf.writableBytes();
}

bool CCtrlPtz::valid()
{
	return true;
}

int CTrackSource::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	source_ = buf.readUInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CTrackSource::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(source_);
	buf.appendUInt32(reserved_);
	return beginWritableBytes - buf.writableBytes();
}

bool CTrackSource::valid()
{
	return true;
}


int CDeviationInfoProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		SPDLOG_DEBUG("解析失败");
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	channel_ = buf.readUInt32();
	hor_offset_ = buf.readInt32();
	ver_offset_ = buf.readInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CDeviationInfoProtocal::construct(CFixBuffer& buf)
{
	return -1;
}

bool CDeviationInfoProtocal::valid()
{
	return true;
}


int CManualTrackProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	channel_ = buf.readUInt32();
	x_ = buf.readInt32();
	y_ = buf.readInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CManualTrackProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(channel_);
	buf.appendInt32(x_);
	buf.appendInt32(y_);
	buf.appendUInt32(reserved_);
	return beginWritableBytes - buf.writableBytes();
}

bool CManualTrackProtocal::valid()
{
	return true;
}

int CFDOCtrl::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	cmd_ = buf.readUInt32();
	params_ = buf.readUInt32();
	reserved_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CFDOCtrl::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(cmd_);
	buf.appendUInt32(params_);
	buf.appendUInt32(reserved_);
	return beginWritableBytes - buf.writableBytes();
}

bool CFDOCtrl::valid()
{
	return true;
}

CAIModelCtrl::CAIModelCtrl(CAIParamImpl& impl)
	:impl_(impl)
{

}

bool CAIModelCtrl::parse(COnlyReadBuffer& buf, std::function<void(CAIModelCtrl&)> cb)
{
	if (!cb) {
		return false;
	}
	if (buf.readableBytes() <= CAIModelCtrl::protocal_head_len)
	{
		SPDLOG_DEBUG("解析失败");
		return false;
	}
	uint32_t number = buf.readUInt32();
	uint64_t timestamp = buf.readUInt64();
	uint32_t cmd = buf.readUInt32();
	uint32_t params_length = buf.readUInt32();

	CAIParamImpl* pImpl = NULL;
	CAIModeCfgParam implModeCfg;
	if (params_cmd_mode_cfg == cmd)
	{
		if (implModeCfg.parse(buf) <= 0)
		{
			SPDLOG_DEBUG("解析失败");
			return false;
		}
		pImpl = &implModeCfg;
	}
	else {
		SPDLOG_DEBUG("解析失败");
		return false;
	}

	CAIModelCtrl pro(*pImpl);
	pro.number_ = number;
	pro.timestamp_ = timestamp;
	pro.cmd_ = cmd;
	pro.params_length_ = params_length;
	if (!pro.valid())
	{
		return false;
	}
	cb(pro);

	return true;
}

int CAIModelCtrl::parse(COnlyReadBuffer& buf)
{
	return 0;
}

int CAIModelCtrl::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(cmd_);
	const char* posDataLen = buf.beginWrite();
	buf.appendUInt32(params_length_);
	int len = impl_.construct(buf);
	if (len <= 0) {
		return -2;
	}
	*static_cast<uint32_t*>((void*)posDataLen) = (uint32_t)len;
	return beginWritableBytes - buf.writableBytes();
}

bool CAIModelCtrl::valid()
{
	return true;
}


int CAIModeCfgParam::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	template_ = buf.readUInt32();
	return beginReadableBytes - buf.readableBytes();
}

int CAIModeCfgParam::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(template_);
	return beginWritableBytes - buf.writableBytes();
}

bool CAIModeCfgParam::valid()
{
	return true;
}

int CFocusMode::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	mode_ = buf.readUInt32();
	channel_ = buf.readUInt8();
	memcpy(reserved_, buf.peek(), sizeof(reserved_));
	buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CFocusMode::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(mode_);
	buf.appendUInt8(channel_);
	ZeroMemory(reserved_, sizeof(reserved_));
	buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

bool CFocusMode::valid()
{
	return true;
}

int CTrackCfgProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	auto_zoom_enable_ = buf.readUInt32();
	zoomin_threshold_ = buf.readUInt32();
	zoomout_threshold_ = buf.readUInt32();
	memcpy(reserved_, buf.peek(), sizeof(reserved_));
	buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CTrackCfgProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(auto_zoom_enable_);
	buf.appendUInt32(zoomin_threshold_);
	buf.appendUInt32(zoomout_threshold_);
	ZeroMemory(reserved_, sizeof(reserved_));
	buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

bool CTrackCfgProtocal::valid()
{
	return true;
}

int CStateExtendProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	size_t len = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	laser_distance_ = buf.readUInt32();
	laser_hit_ = buf.readUInt32();
	laser_light_ = buf.readUInt32();
	heating_ = buf.readUInt32();
	thermal_compressor_ = buf.readUInt32();
	visiable_focus_ = buf.readUInt32();
	thermal_focus_ = buf.readUInt32();
	memcpy(reserved_, buf.peek(), sizeof(reserved_));
	buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CStateExtendProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}
	buf.appendUInt32(number_);
	buf.appendUInt64(timestamp_);
	buf.appendUInt32(laser_distance_);
	buf.appendUInt32(laser_hit_);
	buf.appendUInt32(laser_light_);
	buf.appendUInt32(heating_);
	buf.appendUInt32(thermal_compressor_);
	buf.appendUInt32(visiable_focus_);
	buf.appendUInt32(thermal_focus_);
	ZeroMemory(reserved_, sizeof(reserved_));
	buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

bool CStateExtendProtocal::valid()
{
	return true;
}

int CTargetDirectionExtendProtocal::parse(COnlyReadBuffer& buf)
{
	size_t beginReadableBytes = buf.readableBytes();
	if (buf.readableBytes() < protocal_head_len)
	{
		return -1;
	}
	number_ = buf.readUInt32();
	sys_number_ = buf.readUInt32();
	timestamp_ = buf.readUInt64();
	lng_ = buf.readDouble();
	lat_ = buf.readDouble();
	height_ = buf.readDouble();
	distance_ = buf.readDouble();
	hor_angle_ = buf.readDouble();
	ver_angle_ = buf.readDouble();
	user_data_ = buf.readUInt16();
	guide_type_ = buf.readUInt8();
	target_direction_ = buf.readUInt8();
	search_ = buf.readUInt32();
	hor_viewing_angle_ = buf.readUInt32();
	ver_viewing_angle_ = buf.readUInt32();
	buf.retrieve(sizeof(reserved_));
	return beginReadableBytes - buf.readableBytes();
}

int CTargetDirectionExtendProtocal::construct(CFixBuffer& buf)
{
	size_t beginWritableBytes = buf.writableBytes();
	if (buf.writableBytes() < protocal_head_len)
	{
		return -1;
	}

	buf.appendUInt32(number_);
	buf.appendUInt32(sys_number_);
	buf.appendUInt64(timestamp_);
	buf.appendDouble(lng_);
	buf.appendDouble(lat_);
	buf.appendDouble(height_);
	buf.appendDouble(distance_);
	buf.appendDouble(hor_angle_);
	buf.appendDouble(ver_angle_);
	buf.appendUInt16(user_data_);
	buf.appendUInt8(guide_type_);
	buf.appendUInt8(target_direction_);
	buf.appendUInt32(search_);
	buf.appendUInt32(hor_viewing_angle_);
	buf.appendUInt32(ver_viewing_angle_);
	ZeroMemory(reserved_, sizeof(reserved_));
	buf.append(reserved_, sizeof(reserved_));
	return beginWritableBytes - buf.writableBytes();
}

bool CTargetDirectionExtendProtocal::valid()
{
	return true;
}

UAV_PRO_NAMESPACE_END