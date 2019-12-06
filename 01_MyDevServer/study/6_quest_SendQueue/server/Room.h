#pragma once

#include "ClientManager.h"

class Room
{
public:
	Room() = default;
	~Room() = default;

	void Init(const int roomIndex, const int maxClientCnt);
	bool Enter(Client* client);
	bool Leave(Client* client);
	void BroadCastInRoom(int clientIdx, const char* ID, const char* msg);

public:
	int mRoomIdx = -1;
	int mMaxClientCnt = -1;
	int mCurrClientCnt = 0;

	std::list<Client*> mClientList;
};