


#include "Client.h"
#include "ClientManager.h"
#include "../02_NetworkLayer/Define.h"


namespace LogicLayer
{

	ClientManager::ClientManager() {};
	ClientManager::~ClientManager() {};
	void ClientManager::Init()
	{
		// Ãµ¹ø µ·´Ù
		for (int i = 0; i < NetworkLayer::MAX_CLIENTS; ++i)
		{
			Client client;
			client.Init((short)i);

			m_ClientObjPool.push_back(client);
			m_ClientObjPoolIndex.push_back(i);
		}
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

		m_integerSearch.insert({ sessionIndex, pClient });
		m_charSearch.insert({ ID, pClient });

		return true;
	}

	Client* ClientManager::Find(const char* pszID)
	{
		auto Iter = m_charSearch.find(pszID);

		if (Iter == m_charSearch.end())
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
	


	
}