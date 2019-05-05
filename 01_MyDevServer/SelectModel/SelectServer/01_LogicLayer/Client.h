#pragma once
#include <string>

namespace LogicLayer
{
	// Client Manager에서만 쓴다. 상속용 클래스
	class Client
	{
	public:

		// 이 클래스에서만 쓰니까 클래스 안에서만 정의
		enum class CLIENT_STATE 
		{
			NONE = 0,
			LOGIN = 1,
			LOBBY = 2,
			ROOM = 3,
		};

		Client() {}
		~Client() {}
		void Init(const short idx)
		{
			m_Index = idx;
		}

		void Set(const int sessionIndex, const char* ID)
		{
			m_IsConfirm = true;
			m_CurState = CLIENT_STATE::LOGIN;
			m_SessionIndex = sessionIndex;
			m_ID = ID;

		}

		void Clear()
		{
			m_Index = 0;
			m_ID = "";
			m_IsConfirm = false;
			m_CurState = CLIENT_STATE::NONE;
			//m_LobbyIndex = -1;
			//m_RoomIndex = -1;
		}

		short GetIndex() 
		{ 
			return m_Index; 
		}

		std::string& GetID() 
		{ 
			return m_ID; 
		}

		bool IsConfirm() { return m_IsConfirm; }

		bool IsCurDomainInLogIn() {
			return m_CurState == CLIENT_STATE::LOGIN ? true : false;
		}
		void EnterLobby(const short lobbyIndex)
		{
			m_LobbyIndex = lobbyIndex;
			m_CurState = CLIENT_STATE::LOBBY;
		}

	protected:

		short m_Index = -1;
		int m_SessionIndex = -1;
		std::string m_ID;
		bool m_IsConfirm = false;
		short m_LobbyIndex = -1;
		short m_RoomIndex = -1;
		CLIENT_STATE m_CurState = CLIENT_STATE::NONE;

	};
}