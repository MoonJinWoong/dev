#pragma once
#include <unordered_map>
#include <deque>
#include <iostream>


namespace LogicLayer
{
	// ���漱��
	class Client;

	class ClientManager
	{
	public:
		ClientManager();
		~ClientManager();
		void Init();
		bool Add(const int sessionIndex, const char* ID);
		Client* Find(const char* pszID);
		Client* AllocUserObjPoolIndex();



	private:
		std::vector<Client> m_ClientObjPool;
		std::deque<int> m_ClientObjPoolIndex;


		std::unordered_map<int, Client*> m_integerSearch;
		
		// id �ߺ� üũ��
		std::unordered_map<const char*, Client*> m_charSearch; //char*�� key�� ������

	};

}