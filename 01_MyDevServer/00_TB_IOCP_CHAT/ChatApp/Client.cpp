#include "Client.h"


void Client::Init(unsigned int index)
{
	mRemoteIdx = index;
}

void Client::Reset()
{
	mRoomIdx = -1;
	mNickName = "";
	mState = USER_STATE::NONE;
}

int Client::SetLogin(std::string& nickname , int unique_id)
{
	mState = USER_STATE::LOGIN;
	mNickName = nickname;
	mRemoteIdx = unique_id;
	return 0;
}


