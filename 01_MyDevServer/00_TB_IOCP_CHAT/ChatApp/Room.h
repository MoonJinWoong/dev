#pragma once

#include "ClientManager.h"
#include "../NetLib/Packet.h"

class Room
{
public:
	Room() = default;
	~Room() = default;

	void Init(const int roomIndex, const int maxClientCnt)
	{	
		mRoomIdx = roomIndex;
		mMaxClientCnt = maxClientCnt;
	}
	bool Enter(Client* client)
	{
		auto count = mCurrClientCnt + 1;
		if (mMaxClientCnt <= count)
		{
			std::cout << "Full User in Room ....!" << std::endl;
			return false;
		}

		mClientList.push_back(client);
		++mCurrClientCnt;

		client->SetRoomIndex(mRoomIdx);
		return true;
	}
	bool Leave(Client* client)
	{
		mClientList.remove_if
		(
				[ID = client->GetUserId()](Client* client2)
			{
				if (ID == client2->GetUserId())
					return true;
				else
					false;
			}
		);
		return false;
	}
	void BroadCastInRoom(int clientIdx, const char* ID, const char* msg)
	{
		SC_NOTICE_IN_ROOM packet;
		packet.packet_type = (unsigned short)PACKET_TYPE::SC_ROOM_CHAT_NOTICE;
		packet.packet_len = sizeof(SC_NOTICE_IN_ROOM);

		CopyMemory(packet.Id, ID, sizeof(packet.Id));
		CopyMemory(packet.Msg, msg, sizeof(packet.Msg));
	}

public:
	int mRoomIdx = -1;
	int mMaxClientCnt = -1;
	int mCurrClientCnt = 0;
	std::list<Client*> mClientList;
};