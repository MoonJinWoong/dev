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
		// ���� ��ü ����� �����ϱ� ���� idx�� �ο���
		for (int i = 0; i < maxUserCount; ++i)
		{
			Player player;
			player.Init((short)i);

			m_PlayerObjPool.push_back(player);
			m_PlayerObjPoolIdx.push_back(i);
		}

	}
}