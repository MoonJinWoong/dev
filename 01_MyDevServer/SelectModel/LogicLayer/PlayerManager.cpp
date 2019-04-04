#include <algorithm>
#include "ErrorSet.h"
#include "Player.h"
#include "PlayerManager.h"


namespace LogicLib
{
	PlayerManager::PlayerManager(){}
	PlayerManager::~PlayerManager(){}

	void PlayerManager::Init(const int maxUserCount)
	{
		// 플레이어 객체를 만들어준다. 인덱스를 담고 vector로 관리함
		for (int i = 0; i < maxUserCount; ++i)
		{
			Player player;
			player.Init((short)i);

			m_UserObjPool.push_back(player);
			m_UserObjPoolIndex.push_back(i);
		}
	}
}