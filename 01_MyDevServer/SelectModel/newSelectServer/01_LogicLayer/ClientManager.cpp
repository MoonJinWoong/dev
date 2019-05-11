#include <algorithm>
#include "../02_NetworkLayer/Define.h"
#include "LogicErrorSet.h"
#include "Client.h"
#include "ClientManager.h"



namespace LogicLayer
{
	ClientManager::ClientManager() {}
	ClientManager::~ClientManager() {}
	void ClientManager::Init(const int maxClientCnt)
	{
		for (int i = 0; i < NetworkLayer::MAX_CLIENTS ; ++i)
		{
			Client client;
			client.Init((short)i);

			m_ClientPool.push_back(client);
			m_ClientPoolIndex.push_back(i);
		}
	}

	int ClientManager::AddClient(const int sessionIndex, const char* pszID)
	{
		// TODO 이함수 리턴값때문에 
		// 다시 정의 해주어야 함 일단 int로....

		if (FindByID(pszID) != nullptr)
			return -1;
		

		auto pClient = AllocClientObjPoolIndex();
		if (pClient == nullptr) 
			return -1;
		

		pClient->Set(sessionIndex, pszID);

		m_SearchIdx.insert({ sessionIndex, pClient });
		m_SearchID.insert({ pszID, pClient });

		return 0;
	}

	Client* ClientManager::FindByIdx(const int sessionIndex)
	{
		auto findIter = m_SearchIdx.find(sessionIndex);

		if (findIter == m_SearchIdx.end())
			return nullptr;
		
		return (Client*)findIter->second;
	}

	Client* ClientManager::FindByID(const char* pszID)
	{
		auto findIter = m_SearchID.find(pszID);

		if (findIter == m_SearchID.end())
			return nullptr;
		

		return (Client*)findIter->second;
	}


	Client* ClientManager::AllocClientObjPoolIndex()
	{
		if (m_ClientPoolIndex.empty())
			return nullptr;
		
		int index = m_ClientPoolIndex.front();
		m_ClientPoolIndex.pop_front();


		return &m_ClientPool[index];
	}

	void ClientManager::ReleaseClientObjPoolIndex(const int index)
	{
		m_ClientPoolIndex.push_back(index);
		m_ClientPool[index].Clear();
	}
}