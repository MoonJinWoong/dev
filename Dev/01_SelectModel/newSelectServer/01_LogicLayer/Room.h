#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Client.h"

namespace NetworkLayer 
{ 
	class SelectNetwork; 
}
//using LOGIC_ERROR_SET = LOGIC_ERROR_SET;

namespace LogicLayer
{
	using SelNet = NetworkLayer::SelectNetwork;
	


	class Room
	{
	public:
		Room();
		~Room();
		void Init(const short index, const short maxClient);
		void SetSelectNetwork(SelNet* netObj);

		bool IsUsed();
		bool EnterClient(Client* client);
		short getIdx();
		void BroadCastRoomEnter(const int ClientIndex, const char* sendID);

	private:
		SelNet* m_SelectNet;

		short m_Index = -1;
		short m_MaxClient = 30;
		bool m_IsUsed = false;

		std::vector<Client*> m_ClientList;
	};
}