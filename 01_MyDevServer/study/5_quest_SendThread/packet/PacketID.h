#pragma once

namespace PacketLayer
{
	// CS -> Ŭ�󿡼� ������ 
	// SC -> �������� Ŭ���
	enum class PACKET_ID : short
	{
		SC_CONNECTION = 1,
		SC_SHUT_DOWN_CLIENT = 2,

		// ���⼭���� �ϳ��� ��������
		CS_LOGIN = 100,
		SC_LOGIN = 101,

		CS_LOBBY_LIST = 20,
		SC_LOBBY_LIST = 21,

		CS_LOBBY_ENTER = 22,
		SC_LOBBY_ENTER = 23,

		CS_LOBBY_CHAT = 24,
		SC_LOBBY_CHAT = 25,

		CS_LOBBY_LEAVE = 26,
		SC_LOBBY_LEAVE = 27,

		SC_ROOM_ENTER = 28,
		CS_ROOM_ENTER = 29,




		SC_ERROR_MSG = 49,
		PACKET_TOTAL_COUNT = 50
	};

}