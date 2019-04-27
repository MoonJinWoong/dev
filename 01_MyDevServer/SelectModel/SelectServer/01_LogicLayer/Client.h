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



	protected:

		short m_Index = -1;
		int m_SessionIndex = -1;
		std::string m_ID;
		bool m_IsConfirm = false;
		CLIENT_STATE m_CurState = CLIENT_STATE::NONE;

	};
}