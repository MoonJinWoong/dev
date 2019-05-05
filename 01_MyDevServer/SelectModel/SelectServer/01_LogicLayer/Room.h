#pragma once

#include <vector>
#include <string>
#include <memory>


namespace NetworkLayer 
{ 
	class SelectNetwork; 
}

namespace LogicLayer
{
	using SelectNet = NetworkLayer::SelectNetwork;

	class InGame;

	class Room
	{
	public:
		Room();
		~Room();
		void Init(const short index, const short maxUserCount);
		void SetNetworkRoom(SelectNet* pNetwork);


	private:
		SelectNet* m_pRefNetwork;
		short m_RoomIndex; 
		short m_MaxClientCnt;

		InGame* m_pGame = nullptr;
	};
}