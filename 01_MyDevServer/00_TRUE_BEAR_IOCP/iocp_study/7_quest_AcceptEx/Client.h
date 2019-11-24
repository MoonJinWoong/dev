#pragma once
#include <string>
#include <iostream>
#include "TypeDefine.h"
#include "Packet.h"

enum class USER_STATE
{
	NONE = 0,
	LOGIN = 1
};

class Client
{
	const unsigned int PACKET_DATA_BUFFER_SIZE = 8096;
public:
	Client() = default;
	~Client() = default;

	void Init(c_u_Int index);
	void Reset();


	int SetLogin(std::string& userID_);
	void SetDomainState(USER_STATE state) { mState = state; }
	void SetRoomIndex(const int roomIdx) { mRoomIdx = roomIdx; }

	s_Int GetRemoteIdx() const { return mRemoteIdx; }
	std::string GetUserId() const { return mNickName; }
	USER_STATE GetUserState() const {return mState;}

	// packet assemble
	void SetPacketProc(c_u_Int input_size, char* input_data);
	PacketFrame GetPacketProc();

private:
	u_int mRemoteIdx = -1;
	std::string mNickName;
	USER_STATE mState = USER_STATE::NONE;
	int mRoomIdx = -1;

	// buffer ฐüทร
	char* pBuffer = nullptr;
	u_int mReadPos = 0;
	u_int mWritePos = 0;
};