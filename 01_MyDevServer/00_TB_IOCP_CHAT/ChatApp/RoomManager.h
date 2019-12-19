#pragma once
#include "Room.h"

class RoomManager
{
public:
	RoomManager() = default;
	~RoomManager() = default;

	void Init(int maxRoomcnt,int maxClientInRoom)
	{
		mMaxRoomCount = maxRoomcnt;
		mRoomList = std::vector<Room*>(mMaxRoomCount);

		for (auto i = 0; i < mMaxRoomCount; i++)
		{
			mRoomList[i] = new Room();
			//TODO �ι�° param �� �ƽ� ���� ī��Ʈ�̳� �ϴ� ���� �Ѵ�. ���� ����
			mRoomList[i]->Init(i, maxClientInRoom);
		}
	}

	UINT GetMaxRoomCount() { return mMaxRoomCount; }

	bool EnterRoom(INT32 roomNum, Client* client)
	{
		auto pRoom = GetRoomByNum(roomNum);
		if (pRoom == nullptr)
		{
			return false;
		}

		if (!pRoom->Enter(client))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool LeaveRoom(INT32 roomNum, Client* client)
	{
		//TODO �� ������ ����
	}

	Room* GetRoomByNum(signed int  roomNum)
	{
		//TODO ���� ó��
		return mRoomList[roomNum];
	}

private:
	std::vector<Room*> mRoomList;
	INT32 mMaxRoomCount = 0;
};