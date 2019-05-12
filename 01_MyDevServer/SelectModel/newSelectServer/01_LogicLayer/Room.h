//#pragma once
//
//#include <vector>
//#include <string>
//#include <memory>
//#include "Client.h"
//
//
//namespace NetworkLayer { class SelectNetwork; }
//
//
//enum class LOGIC_ERROR_SET :short;
//using LOGIC_ERROR_SET = LOGIC_ERROR_SET;
//
//namespace NLogicLib
//{
//	using SelectNet = NetworkLayer::SelectNetwork;
//
//	//class Game;
//	class Room
//	{
//	public:
//		Room();
//		virtual ~Room();
//
//		void Init(const short index, const short maxUserCount);
//
//		void SetNetwork(SelectNet* pNetwork);
//
//		void Clear();
//
//		short GetIndex() { return m_Index; }
//
//		bool IsUsed() { return m_IsUsed; }
//
//		const wchar_t* GetTitle() { return m_Title.c_str(); }
//
//		short MaxUserCount() { return m_MaxUserCount; }
//
//		short GetUserCount() { return (short)m_UserList.size(); }
//
//		LOGIC_ERROR_SET CreateRoom(const wchar_t* pRoomTitle);
//
//		LOGIC_ERROR_SET Enter(Client* pUser);
//
//		LOGIC_ERROR_SET Leave(const short userIndex);
//
//		bool IsMaster(const short userIndex);
//
//		//Game* GetGameObj();
//
//		void Update();
//
//		void BroadCastClient(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);
//
//		void NotifyEnterUserInfo(const int userIndex, const char* pszUserID);
//
//		void NotifyLeaveUserInfo(const char* pszUserID);
//
//		void NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg);
//
//	private:
//		
//		SelectNet* m_pRefNetwork;
//
//		short m_Index = -1;
//		short m_MaxUserCount;
//
//		bool m_IsUsed = false;
//		std::wstring m_Title;
//		std::vector<User*> m_UserList;
//
//		Game* m_pGame = nullptr;
//	};
//}