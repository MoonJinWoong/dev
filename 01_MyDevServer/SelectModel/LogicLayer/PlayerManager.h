#pragma once
#include <unordered_map>
#include <deque>
#include <string>

namespace LogicCommon
{
	enum class ERROR_CODE :short;
}
using ERROR_CODE = LogicCommon::ERROR_CODE;


namespace LogicLib
{
	class Player;

	class PlayerManager
	{
	public:
		PlayerManager();
		virtual ~PlayerManager();

		void Init(const int maxUserCount);

	//	ERROR_CODE AddUser(const int sessionIndex, const char* pszID);
	//	ERROR_CODE RemoveUser(const int sessionIndex);

	//	std::tuple<ERROR_CODE, Player*> GetUser(const int sessionIndex);


	private:
		//Player* AllocUserObjPoolIndex();
		//void ReleaseUserObjPoolIndex(const int index);

		//Player* FindUser(const int sessionIndex);
		//Player* FindUser(const char* pszID);

	private:
		std::vector<Player> m_UserObjPool;
		std::deque<int> m_UserObjPoolIndex;

		std::unordered_map<int, Player*> m_UserSessionDic;
		std::unordered_map<const char*, Player*> m_UserIDDic; //char*는 key로 사용못함

	};
}