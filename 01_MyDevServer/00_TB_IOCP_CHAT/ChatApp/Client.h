#pragma once
#include <string>
#include <iostream>

#include "../NetLib/Packet.h"

enum class USER_STATE
{
	NONE = 0,
	LOGIN = 1,
	ROOM = 2
};

class Client
{
	const unsigned int PACKET_DATA_BUFFER_SIZE = 8096;
public:
	Client() = default;
	~Client() = default;

	void Init(unsigned int index);
	void Reset();

	int SetLogin(std::string& nickName , int unique_id);
	void SetDomainState(USER_STATE state) { mState = state; }
	void SetRoomIndex(const unsigned short roomIdx) { mRoomIdx = roomIdx; }

	signed int GetRemoteIdx() const { return mRemoteIdx; }
	std::string GetUserId() const { return mNickName; }
	USER_STATE GetUserState() const {return mState;}
	signed int GetRoomIdx() const { return mRoomIdx; }

private:
	unsigned int mRemoteIdx = -1;
	unsigned short mRoomIdx = -1;
	std::string mNickName;
	USER_STATE mState = USER_STATE::NONE;
};