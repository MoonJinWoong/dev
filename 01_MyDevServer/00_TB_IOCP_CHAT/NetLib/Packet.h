#pragma once
#include <Windows.h>

/* SC -> server to client */
/* CS -> client to server */
enum class  PACKET_TYPE : int
{
	NONE = 0,
	CONNECTION = 1,
	DISCONNECTION = 2,

	CS_LOGIN = 100,
	SC_LOGIN = 101,

    CS_ROOM_LIST = 200,
	SC_ROOM_LIST = 201,
	CS_ROOM_ENTER = 202,
	SC_ROOM_ENTER = 203,
	CS_ROOM_LEAVE = 204,
	SC_ROOM_LEAVE = 205,
	CS_ROOM_CHAT  = 206,
	SC_ROOM_CHAT  = 207,
	
	SC_ROOM_CHAT_NOTICE = 222,
};

/* Client class가 사용 */
#pragma pack(push,1)

// recv ->  parsing 
struct PacketFrame
{
	short packet_type = -1; // NONE
	short size = 0;
	int unique_id = 0;
	char* pData = nullptr;
};



struct PKT_HEADER
{
	unsigned short packet_len;
	unsigned short packet_type;
};

// 로그인 관련 구조체
const int MAX_USER_ID_SIZE = 16;

struct CS_LOGIN_PKT : public PKT_HEADER
{
	char szID[MAX_USER_ID_SIZE + 1] = { 0, };
};
struct SC_LOGIN_PKT : public PKT_HEADER
{
	char msg[50] = { 0, };
};



// 룸 관련 
const int MAX_ROOM_MSG = 100;
const int MAX_CHAT_MSG_SIZE = 256;
struct SC_NOTICE_IN_ROOM : public PKT_HEADER
{
	char Id[MAX_USER_ID_SIZE + 1] = { 0, };
	char Msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};

struct ROOM_INFO
{
	short mRoomId;
	short mInRoomUserCnt;   // 현재 룸안에 있는 유저
	short mRoomMaxCnt;      // 룸 허용 인원 최대치
};

struct CS_ROOM_LIST_PKT : public PKT_HEADER
{
};

struct SC_ROOM_LIST_PKT : public PKT_HEADER
{
	short mRoomCount = 10;
	char mMsg[MAX_ROOM_MSG] = { 0, };
};

struct CS_ROOM_ENTER : public PKT_HEADER
{
	short RoomId = -1;
};

struct SC_ROOM_ENTER : public PKT_HEADER
{
	char mMsg[MAX_ROOM_MSG] = { 0, };
};

struct CS_ROOM_CHAT : public PKT_HEADER
{
	char msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};
struct SC_ROOM_CHAT : public PKT_HEADER
{
	char sendID[MAX_USER_ID_SIZE + 1] = { 0, };
	char msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};

#pragma pack(pop)


