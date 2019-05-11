#pragma once
#include <unordered_map>
#include <deque>
#include <string>



enum class LOGIC_ERROR_SET :short;

namespace LogicLayer
{

	class Client; // 전방 선언


	class ClientManager
	{
		
	public:
		ClientManager();
		~ClientManager();
		void Init(const int maxClientCnt);
		int AddClient(const int sessionIndex, const char* pszID);

	private:
		Client* AllocClientObjPoolIndex();
		void ReleaseClientObjPoolIndex(const int index);

		Client* FindByIdx(const int sessionIndex);
		Client* FindByID(const char* pszID);

	private:
		std::vector<Client> m_ClientPool;
		std::deque<int> m_ClientPoolIndex;

		std::unordered_map<int, Client*> m_SearchIdx;
		std::unordered_map<const char*, Client*> m_SearchID; //char*는 key로 사용못함
	};
}