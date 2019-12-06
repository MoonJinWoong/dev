#pragma once
#include <string>
#include "Packet.h"

//TODO: 한 곳에 몰아넣기
typedef const unsigned int c_u_int;
typedef unsigned int u_int;

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

	void Init(c_u_int index);
	void Reset();

	// set
	int SetLogin(std::string& userID_);
	void SetDomainState(USER_STATE state) { mState = state; }

	// get
	INT32 GetRemoteIdx() const { return mRemoteIdx; }
	std::string GetUserId() const { return mNickName; }
	USER_STATE GetUserState() const {return mState;}

	// packet assemble
	void SetPacketAssemble(c_u_int input_size, char* input_data);
	PacketFrame GetPacketAssemble();

private:
	u_int mRemoteIdx;
	std::string mNickName;
	USER_STATE mState = USER_STATE::NONE;

	// buffer 관련
	char* pBuffer = nullptr;
	u_int mReadPos = 0;
	u_int mWritePos = 0;




};