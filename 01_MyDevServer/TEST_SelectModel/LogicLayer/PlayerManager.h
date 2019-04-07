#pragma once
#include <unordered_map>
#include <deque>
#include <string>


namespace LogicCommon
{
	enum class ERROR_SET :short;
}


using ERROR_SET = LogicCommon::ERROR_SET;

namespace LogicLib
{
	class Player;


	class PlayerManager
	{
	public:
		PlayerManager();
		~PlayerManager();
		void Init(const int maxUserCount);
		ERROR_SET AddUser(const int sessionIndex, const char* pszID);
		std::tuple<ERROR_SET, Player*> GetPlayer(const int sessionIndex);
	private:
		std::vector<Player> m_PlayerObjPool;
		std::deque<int> m_PlayerObjPoolIdx;


		Player* FindPlayer(const int sessionIndex);
		Player* FindPlayer(const char* pszID);
		Player* AllocPlayerObjPoolIndex();

		std::unordered_map<int, Player*> m_PlayerSessionDic;
		std::unordered_map<const char*, Player*> m_PlayerIDDic; //char*는 key로 사용못함
	protected:

	};
}