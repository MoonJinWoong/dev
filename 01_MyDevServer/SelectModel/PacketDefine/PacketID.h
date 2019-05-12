#pragma once

namespace PacketLayer
{
	// CS -> 클라에서 서버로 
	// SC -> 서버에서 클라로
	enum class PACKET_ID : short
	{
		SC_CONNECTION = 1,
		SC_SHUT_DOWN_CLIENT = 2,


		CS_LOGIN = 10,
		SC_LOGIN = 11,

		CS_LOBBY_LIST = 20,
		SC_LOBBY_LIST = 21,

		CS_LOBBY_ENTER = 22,
		SC_LOBBY_ENTER = 23,

		CS_LOBBY_CHAT = 24,
		SC_LOBBY_CHAT = 25,

		PACKET_TOTAL_COUNT = 50
	};

}