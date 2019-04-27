#pragma once
#include <string>

namespace LogicLayer
{
	// Client Manager������ ����. ��ӿ� Ŭ����
	class Client
	{
	public:

		// �� Ŭ���������� ���ϱ� Ŭ���� �ȿ����� ����
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