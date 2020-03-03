#include <algorithm>
#include "../02_NetworkLayer/SelectNetwork.h"
#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "Client.h"
#include "Room.h"
#include "Lobby.h"


namespace LogicLayer
{
	Lobby::Lobby() {}

	Lobby::~Lobby() {}

	void Lobby::Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount)
	{
		m_LobbyIndex = lobbyIndex;
		m_MaxUserCount = (short)maxLobbyUserCount;

		for (int i = 0; i < maxLobbyUserCount; ++i)
		{
			ClientInLobby client;
			client.Index = (short)i;
			client.pUser = nullptr;

			m_ClientList.push_back(client);
		}

		for (int i = 0; i < maxRoomCountByLobby; ++i)
		{
			m_RoomList.emplace_back(new Room());
			m_RoomList[i]->Init((short)i, maxRoomUserCount);
		}
	}

	void Lobby::Release()
	{
		for (int i = 0; i < (int)m_RoomList.size(); ++i)
		{
			delete m_RoomList[i];
		}

		m_RoomList.clear();
	}

	void Lobby::SetNetwork(SelectNet* pNetwork)
	{
	
		m_SelNetwork = pNetwork;

		for (auto pRoom : m_RoomList)
		{
			pRoom->SetSelectNetwork(pNetwork);
		}
	}

	LOGIC_ERROR_SET Lobby::Enter(Client* pUser)
	{
		if (m_SeachByIndex.size() >= m_MaxUserCount) 
			return LOGIC_ERROR_SET::LOBBY_ENTER_MAX_USER_COUNT;
		

		if (Find(pUser->GetIndex()) != nullptr) {
			return LOGIC_ERROR_SET::LOBBY_ENTER_USER_DUPLICATION;
		}

		auto addRet = Add(pUser);
		if (addRet != LOGIC_ERROR_SET::NONE) {
			return LOGIC_ERROR_SET::LOBBY_ADD_FAIL;
		}

		pUser->EnterLobby(m_LobbyIndex);

		m_SeachByIndex.insert({ pUser->GetIndex(), pUser });
		m_SearchByStr.insert({ pUser->GetID().c_str(), pUser });

		return LOGIC_ERROR_SET::NONE;
	}

	LOGIC_ERROR_SET Lobby::LeaveUser(const int Index)
	{
		Remove(Index);

		auto pUser = Find(Index);

		if (pUser == nullptr) 
			return LOGIC_ERROR_SET::LOBBY_LEAVE_USER_INVALID_INDEX;
		

		pUser->LeaveLobby();

		m_SeachByIndex.erase(pUser->GetIndex());
		m_SearchByStr.erase(pUser->GetID().c_str());

		return LOGIC_ERROR_SET::NONE;
	}

	Client* Lobby::Find(const int Index)
	{
		auto findIter = m_SeachByIndex.find(Index);

		if (findIter == m_SeachByIndex.end())
			return nullptr;
		

		return (Client*)findIter->second;
	}

	LOGIC_ERROR_SET Lobby::Add(Client* pUser)
	{
		auto findIter = std::find_if(std::begin(m_ClientList), std::end(m_ClientList), [](auto & lobbyUser) { return lobbyUser.pUser == nullptr; });

		if (findIter == std::end(m_ClientList))
			return LOGIC_ERROR_SET::LOBBY_ENTER_EMPTY_USER_LIST;
		
		findIter->pUser = pUser;
		return LOGIC_ERROR_SET::NONE;
	}

	void Lobby::Remove(const int Index)
	{
		auto findIter = std::find_if(std::begin(m_ClientList), std::end(m_ClientList)
			, [Index](auto & clientInLobby) 
			{ 
				return clientInLobby.pUser != nullptr && clientInLobby.pUser->GetIndex() == Index; 
			});

		if (findIter == std::end(m_ClientList))
			return;
		

		findIter->pUser = nullptr;
	}

	short Lobby::GetClientCnt()
	{
		return static_cast<short>(m_SeachByIndex.size());
	}

	void Lobby::BroadCastClient(const int clientIdx, const char*  sendID,const char* msg)
	{
		PacketLayer::SC_Lobby_Chat_Pkt packet;

		// TODO : strcpy_s 를 써야함 이거 찾아서 전부 고쳐주기
		strcpy(packet.sendID, sendID);
		strcpy(packet.msg, msg);

		auto packetId = (short)PacketLayer::PACKET_ID::SC_LOBBY_CHAT;
		auto bodySize = sizeof(PacketLayer::SC_Lobby_Chat_Pkt);

		for (auto& pClient : m_SeachByIndex)
		{
			if (pClient.second->IsCurStateInLobby() == false) 
				continue;

			m_SelNetwork->SendInLogic(pClient.second->GetClientIndex(),
				packetId, bodySize, (char*)&packet);
		}
	}

	Room* Lobby::CreateRoom()
	{
		for (int i = 0; i < (int)m_RoomList.size(); ++i)
		{
			if (m_RoomList[i]->IsUsed() == false) {
				return m_RoomList[i];
			}
		}
		return nullptr;
	}

	Room* Lobby::GetRoom(const short roomIndex)
	{
		if (roomIndex < 0 || roomIndex >= m_RoomList.size())
			return nullptr;
		
		return m_RoomList[roomIndex];
	}


}
