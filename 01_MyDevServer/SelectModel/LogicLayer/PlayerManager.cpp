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
}