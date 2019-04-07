#include <algorithm>
#include "ErrorSet.h"
#include "Player.h"
#include "PlayerManager.h"

namespace LogicLib
{
	PlayerManager::PlayerManager() {}
	PlayerManager::~PlayerManager() {}
	void PlayerManager::Init(const int maxUserCount)
	{
		// 유저 객체 만들고 관리하기 위해 idx도 부여함
		for (int i = 0; i < maxUserCount; ++i)
		{
			Player player;
			player.Init((short)i);

			m_PlayerObjPool.push_back(player);
			m_PlayerObjPoolIdx.push_back(i);
		}
	}
	ERROR_SET PlayerManager::AddUser(const int sessionIndex, const char* pszID)
	{
		if (FindPlayer(pszID) != nullptr) {
			return ERROR_SET::USER_MGR_ID_DUPLICATION;
		}

		auto pUser = AllocPlayerObjPoolIndex();
		if (pUser == nullptr) {
			return ERROR_SET::USER_MGR_MAX_USER_COUNT;
		}

		pUser->Set(sessionIndex, pszID);

		m_PlayerSessionDic.insert({ sessionIndex, pUser });
		m_PlayerIDDic.insert({ pszID, pUser });

		return ERROR_SET::NONE;
	}
	Player* PlayerManager::FindPlayer(const int sessionIndex)
	{
		auto findIter = m_PlayerSessionDic.find(sessionIndex);

		if (findIter == m_PlayerSessionDic.end()) {
			return nullptr;
		}

		//auto pUser = (User*)&findIter->second;
		return (Player*)findIter->second;
	}

	Player* PlayerManager::FindPlayer(const char* pszID)
	{
		auto findIter = m_PlayerIDDic.find(pszID);

		if (findIter == m_PlayerIDDic.end()) {
			return nullptr;
		}

		return (Player*)findIter->second;
	}

	Player* PlayerManager::AllocPlayerObjPoolIndex()
	{
		if (m_PlayerObjPoolIdx.empty())
			return nullptr;
		
		int index = m_PlayerObjPoolIdx.front();
		m_PlayerObjPoolIdx.pop_front();
		return &m_PlayerObjPool[index];
	}
	
	std::tuple<ERROR_SET, Player*> 
		PlayerManager::GetPlayer(const int sessionIndex)
	{
		auto pUser = FindPlayer(sessionIndex);

		if (pUser == nullptr) {
			return { ERROR_SET::USER_MGR_INVALID_SESSION_INDEX, nullptr };
		}

		if (pUser->IsConfirm() == false) {
			return{ ERROR_SET::USER_MGR_NOT_CONFIRM_USER, nullptr };
		}

		return{ ERROR_SET::NONE, pUser };
	}
}