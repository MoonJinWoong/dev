#pragma once

namespace PacketLayer
{
	// CS -> 클라에서 서버로 
	// SC -> 서버에서 클라로
	enum class PACKET_ID : int
	{
		SC_CONNECTION = 1,
		SC_SHUT_DOWN_CLIENT = 2,

		CS_LOGIN = 100,
		SC_LOGIN = 101,

		CS_ROOM_LIST = 200,
		SC_ROOM_LIST = 201,
		CS_ROOM_ENTER = 202,
		SC_ROOM_ENTER = 203,
		CS_ROOM_LEAVE = 204,
		SC_ROOM_LEAVE = 205,
		CS_ROOM_CHAT = 206,
		SC_ROOM_CHAT = 207,

		SC_ROOM_CHAT_NOTICE = 222,
	};

}