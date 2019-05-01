#include <algorithm>
#include "../02_NetworkLayer/Define.h"
#include "LogicErrorSet.h"
#include "Client.h"
#include "ClientManager.h"



namespace LogicLayer
{

	ClientManager::ClientManager() {};
	ClientManager::~ClientManager() {};
	void ClientManager::Init()
	{
		for (int i = 0; i < NetworkLayer::MAX_CLIENTS; ++i)
		{
			Client client;
			client.Init((short)i);

			m_ClientObjPool.push_back(client);
			m_ClientObjPoolIndex.push_back(i);
		}
	}

	std::tuple<LOGIC_ERROR_SET, Client*> ClientManager::GetClient(const int sessionIndex)
	{
		auto pClient = Find(sessionIndex);

		if (pClient == nullptr)
			return { LOGIC_ERROR_SET::CLIENT_MGR_INVALID_INDEX, nullptr };
		
		if (pClient->IsConfirm() == false) 
			return{ LOGIC_ERROR_SET::CLIENT_MGR_NOT_CONFIRM_USER, nullptr };
		

		return{ LOGIC_ERROR_SET::NONE, pClient };
	}
	bool ClientManager::Add(const int sessionIndex, const char* ID)
	{
		if (Find(ID) != nullptr)
		{
			std::cout << "this Client ID duplicate..!" << std::endl;
			return false;
		}

		auto pClient = AllocUserObjPoolIndex();
		if (pClient == nullptr) 
		{
			std::cout << "client manager is full...!" << std::endl;
			return false;
		}

		pClient->Set(sessionIndex, ID);


		// 맵에 넣어준다.
		m_integerSearch.insert({ sessionIndex, pClient });
		m_charSearch.insert({ ID, pClient });

		return true;
	}

	bool ClientManager::Remove(const int sessionIndex)
	{
		auto pClient = Find(sessionIndex);

		if (pClient == nullptr)
			return false;
		

		auto index = pClient->GetIndex();
		auto pszID = pClient->GetID();

		m_integerSearch.erase(sessionIndex);
		m_charSearch.erase(pszID.c_str());
		ReleaseClientPoolIndex(index);

		return true;
	}

	Client* ClientManager::Find(const char* pszID)
	{
		auto Iter = m_charSearch.find(pszID);

		if (Iter == m_charSearch.end())
			return nullptr;
		
		return (Client*)Iter->second;
	}

	Client* ClientManager::Find(const int index)
	{
		auto Iter = m_integerSearch.find(index);

		if (Iter == m_integerSearch.end())
			return nullptr;

		return (Client*)Iter->second;
	}

	Client* ClientManager::AllocUserObjPoolIndex()
	{
		if (m_ClientObjPoolIndex.empty()) 
			return nullptr;
		
		int index = m_ClientObjPoolIndex.front();
		m_ClientObjPoolIndex.pop_front();

		return &m_ClientObjPool[index];
	}

	void ClientManager::ReleaseClientPoolIndex(const int index)
	{
		m_ClientObjPoolIndex.push_back(index);
		m_ClientObjPool[index].Clear();
	}
}