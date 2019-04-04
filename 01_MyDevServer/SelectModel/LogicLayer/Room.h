#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Player.h"


namespace NetworkLib { class NetworkFrame; }
//namespace NServerNetLib { class ILog; }
namespace NetworkCommon { enum class ERROR_SET :short; }

using ERROR_SET = LogicCommon::ERROR_SET;

namespace LogicLib
{
	using TcpNet = NetworkLib::NetworkFrame;
	//using ILog = NetworkLib::ILog;

	class Game;

	class Room
	{
	public:
		Room();
		virtual ~Room();

		void Init(const short index, const short maxUserCount);

		void SetNetwork(TcpNet* pNetwork);

		void Clear();

		short GetIndex() { return m_Index; }

		bool IsUsed() { return m_IsUsed; }

		const wchar_t* GetTitle() { return m_Title.c_str(); }

		short MaxUserCount() { return m_MaxUserCount; }

		short GetUserCount() { return (short)m_UserList.size(); }

		ERROR_SET CreateRoom(const wchar_t* pRoomTitle);

		ERROR_SET EnterUser(Player* Player);

		ERROR_SET LeaveUser(const short userIndex);

		bool IsMaster(const short userIndex);

		Game* GetGameObj();

		void Update();

		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);

		void NotifyEnterUserInfo(const int userIndex, const char* pszUserID);

		void NotifyLeaveUserInfo(const char* pszUserID);

		void NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg);

	private:
		//ILog* m_pRefLogger;
		TcpNet* m_pRefNetwork;

		short m_Index = -1;
		short m_MaxUserCount;

		bool m_IsUsed = false;
		std::wstring m_Title;
		std::vector<Player*> m_UserList;

		Game* m_pGame = nullptr;
	};
}