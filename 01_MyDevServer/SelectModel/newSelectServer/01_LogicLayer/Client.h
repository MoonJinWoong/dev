#pragma once
#include <string>

namespace LogicLayer
{
	class Client
	{
	public:
		enum class CLIENT_STATE {
			NONE = 0,
			LOGIN = 1,
			LOBBYIN = 2,
			ROOMIN = 3,
		};

	public:
		Client() {}
		virtual ~Client() {}

		void Init(const short index)
		{
			m_Index = index;
		}

		void Clear()
		{
			m_ClientIndex = 0;
			m_ID = "";
			m_IsConfirm = false;
			m_CurDomainState = CLIENT_STATE::NONE;
			m_LobbyIndex = -1;
			m_RoomIndex = -1;
		}

		void Set(const int sessionIndex, const char* pszID)
		{
			m_IsConfirm = true;
			m_CurDomainState = CLIENT_STATE::LOGIN;

			m_ClientIndex = sessionIndex;
			m_ID = pszID;

		}

		short GetIndex() { return m_Index; }

		int GetClientIndex() { return m_ClientIndex; }

		std::string GetID() { return m_ID; }

		bool IsConfirm() { return m_IsConfirm; }

		short GetLobbyIndex() { return m_LobbyIndex; }

		void EnterLobby(const short lobbyIndex)
		{
			m_LobbyIndex = lobbyIndex;
			m_CurDomainState = CLIENT_STATE::LOBBYIN;
		}

		void LeaveLobby()
		{
			m_CurDomainState = CLIENT_STATE::LOGIN;
		}


		short GetRoomIndex() { return m_RoomIndex; }

		void EnterRoom(const short lobbyIndex, const short roomIndex)
		{
			m_LobbyIndex = lobbyIndex;
			m_RoomIndex = roomIndex;
			m_CurDomainState = CLIENT_STATE::ROOMIN;
		}


		// 아래 3개를 템플릿으로 구현 해보자 
		bool IsCurStateInLogIn() {
			return m_CurDomainState == CLIENT_STATE::LOGIN ? true : false;
		}
		bool IsCurStateInLobby() {
			return m_CurDomainState == CLIENT_STATE::LOBBYIN ? true : false;
		}
		bool IsCurStateInRoom() {
			return m_CurDomainState == CLIENT_STATE::ROOMIN ? true : false;
		}


	protected:
		short m_Index = -1;

		int m_ClientIndex = -1;

		std::string m_ID;

		bool m_IsConfirm = false;

		CLIENT_STATE m_CurDomainState = CLIENT_STATE::NONE;

		short m_LobbyIndex = -1;

		short m_RoomIndex = -1;
	};
}