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
	short Lobby::GetIndex() { return m_LobbyIndex; }
	void Lobby::Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount)
	{
		// 로비에 있는 클라들 초기화 
		// 로비 하나당 룸을 여러개 만들고 초기화 

		m_LobbyIndex = lobbyIndex;
		m_maxClientCnt = maxLobbyUserCount;

		for (int i = 0; i < m_maxClientCnt; ++i)
		{
			ClientInLobby lobbyClient;
			lobbyClient.Index = (short)i;
			lobbyClient.pClient = nullptr;

			m_ClientListInLobby.push_back(lobbyClient);
		}

		for (int i = 0; i < maxRoomCountByLobby; ++i)
		{
			m_RoomList.emplace_back(new Room());
			m_RoomList[i]->Init((short)i, maxRoomUserCount);
		}
	}
	void Lobby::SetNetworkLobby(SelectNet* pNetwork)
	{
		m_pSelectNet = pNetwork;

		for (auto pRoom : m_RoomList)
		{
			pRoom->SetNetworkRoom(pNetwork);
		}
	}

	short Lobby::GeClientCnt()
	{
		return static_cast<short>(m_ClientIndexSearch.size());
	}
	short Lobby::MaxClientCnt()
	{
		return (short)m_maxClientCnt;
	}

	short Lobby::MaxRoomCount()
	{ 
		return (short)m_RoomList.size();
	}


	LOGIC_ERROR_SET Lobby::Enter(Client* pClient)
	{
		if (m_ClientIndexSearch.size() >= m_maxClientCnt) 
			return LOGIC_ERROR_SET::LOBBY_ENTER_MAX_USER_COUNT;
		

		if (Find(pClient->GetIndex()) != nullptr) 
			return LOGIC_ERROR_SET::LOBBY_ENTER_USER_DUPLICATION;
		

		auto addRet = Add(pClient);
		if (addRet != LOGIC_ERROR_SET::NONE) 
			return addRet;
		
		pClient->EnterLobby(m_LobbyIndex);

		m_ClientIndexSearch.insert({ pClient->GetIndex(), pClient });
		m_ClientIDSearch.insert({ pClient->GetID().c_str(), pClient });

		return LOGIC_ERROR_SET::NONE;
	}

	Client* Lobby::Find(const int userIndex)
	{
		auto findIter = m_ClientIndexSearch.find(userIndex);

		if (findIter == m_ClientIndexSearch.end())
			return nullptr;
		

		return (Client*)findIter->second;
	}
	LOGIC_ERROR_SET Lobby::Add(Client* pClient)
	{
		auto findIter = std::find_if(std::begin(m_ClientListInLobby),
			std::end(m_ClientListInLobby), [](auto & ClientInLobby)
			{ return ClientInLobby.pClient == nullptr; });

		if (findIter == std::end(m_ClientListInLobby)) {
			return LOGIC_ERROR_SET::LOBBY_ENTER_EMPTY_USER_LIST;
		}

		findIter->pClient = pClient;
		return LOGIC_ERROR_SET::NONE;
	}




}