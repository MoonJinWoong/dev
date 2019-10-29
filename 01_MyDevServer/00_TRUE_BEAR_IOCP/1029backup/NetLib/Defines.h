#pragma once


enum class MsgType : INT8
{
	None = 0,
	Session,
	Close,
	OnRecv
};


enum class OPType : INT8
{
	None = 0,
	Send,
	Recv,
	Accept,
};


struct CustomOverlapped
{
	OVERLAPPED Overlapped;
	WSABUF OverlappedExWsaBuf;

	//TODO 파스칼 형식으로 변수명 통일
	OPType type;

	// 아래도 일관성있게 통일
	int	totalByte;

	// 이거는 packet을 다 못만들었을때  사용하는건데 SESSION에 있는게 낫다. 
	// 링버퍼를 사용하는데 이게 의미가 있나. 다시 생각해보고 지워주자.
	unsigned long OverlappedExRemainByte;
	
	//TODO 활용 잘하고 해제 잘하는지 다시 한번 확인
	char* pOverlappedExSocketMessage;

	INT32 SessionIdx = 0;

	CustomOverlapped(INT32 connectionIndex)
	{
		memset(&Overlapped, 0, sizeof(OVERLAPPED));
		memset(&OverlappedExWsaBuf, 0, sizeof(WSABUF));
		type = OPType::None;
		totalByte = 0;
		OverlappedExRemainByte = 0;
		memset(&pOverlappedExSocketMessage, 0, sizeof(char*));

		SessionIdx = connectionIndex;
	}
};




struct Message
{
	MsgType Type = MsgType::None;
	char* pContents = nullptr;

	void Clear()
	{
		Type = MsgType::None;
		pContents = nullptr;
	}

	void SetMessage(MsgType msgType, char* pSetContents)
	{
		Type = msgType;
		pContents = pSetContents;
	}
};




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

	CS_LOBBY_LEAVE = 26,
	SC_LOBBY_LEAVE = 27,

	SC_ROOM_ENTER = 28,
	CS_ROOM_ENTER = 29,

	SC_ERROR_MSG = 49,
	PACKET_TOTAL_COUNT = 50
};



//TODO 이거 CONFIG로 옮기기
const int MAX_IP_LENGTH = 20;
const int MAX_ADDR_LENGTH = 64;

const int SESSION_MAX_SEND_BUFF_SIZE = 30720;
const int SESSION_MAX_RECV_BUFF_SIZE = 30720;
const int MAX_RECV_OVERLAPPED_BUFF_SIZE = 512;
const int MAX_SEND_OVERLAPPED_BUFF_SIZE = 512;

const int MAX_SESSION_COUNT = 1000;
const int MAX_MESSAGE_POOL_COUNT = 1000;

// CPU에 따라 조절해야함. 나중에 테스트해볼 때 건드리자
const int WORKER_THREAD_COUNT = 4;

// GQCSEx에서 한번에 몇개 가져올거냐
static const int MAX_EVENT_COUNT = 1000;  

// TODO : 숫자 조절해보기
const int MAX_MSG_POOL_COUNT = 100;
const int EXTRA_MSG_POOL_COUNT = 100;


const int PACKET_HEADER_LENGTH = 5;
const int PACKET_SIZE_LENGTH = 2;
const int PACKET_TYPE_LENGTH = 2;