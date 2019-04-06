#pragma once
#include <unordered_map>
#include <deque>
#include <string>


namespace LocgicCommon
{
	enum class ERROR_SET :short;
}
using ERROR_SET = LocgicCommon::ERROR_SET;

namespace LogicLib
{
	class Player;


	class PlayerManager
	{
	public:
		PlayerManager();
		~PlayerManager();
		void Init(const int maxUserCount);

	private:
		std::vector<Player> m_PlayerObjPool;
		std::deque<int> m_PlayerObjPoolIdx;


	protected:

	};
}