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
			//TODO 두번째 param 은 맥스 유저 카운트이나 일단 같게 한다. 추후 수정
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
		//TODO 방 떠나기 구현
	}

	Room* GetRoomByNum(signed int  roomNum)
	{
		//TODO 예외 처리
		return mRoomList[roomNum];
	}

private:
	std::vector<Room*> mRoomList;
	INT32 mMaxRoomCount = 0;
};