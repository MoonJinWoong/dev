#include <algorithm>

#include "../02_NetworkLayer/SelectNetwork.h"
#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "Client.h"


#include "Room.h"


namespace LogicLayer
{
	Room::Room() {}
	Room::~Room() {}
	void Room::Init(const short index, const short maxClient)
	{
		m_Index = index;
		m_MaxClient = maxClient;
	}
	void Room::SetSelectNetwork(SelNet* netObj)
	{
		m_SelectNet = netObj;
	}

	bool Room::IsUsed() 
	{ 
		return m_IsUsed; 
	}
	bool Room::EnterClient(Client* client)
	{

		if (m_ClientList.size() == m_MaxClient)
			return false;
		
		m_ClientList.push_back(client);
		return true;
	}
	short Room::getIdx()
	{ 
		return m_Index;
	}
	void Room::BroadCastRoomEnter(const int ClientIndex, const char* sendID)
	{
		PacketLayer::SC_Room_Enter sendpacket;
		strcpy(sendpacket.ID, sendID);

		auto packetId = (short)PacketLayer::PACKET_ID::SC_ROOM_ENTER;
		auto bodySize = sizeof(PacketLayer::SC_Room_Enter);

		for (auto& pClient : m_ClientList)
		{
			m_SelectNet->SendInLogic(pClient->GetClientIndex(),
				packetId, bodySize, (char*)& sendpacket);
		}
	}



}