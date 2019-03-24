#pragma once


class ConnectionManager
{
public:
	ConnectionManager();
	~ConnectionManager();

	void SendPacket(SOCKET &s, unsigned char *packet);

	//Overlapped I/O 요청을 위한 변수
	//LPOVERLAPPED_EX		m_lpRecvOverlappedEx;
	//LPOVERLAPPED_EX		m_lpSendOverlappedEx;
	//클라이언트와 데이터 송수신을 위한 링 버퍼
	//cRingBuffer		m_ringRecvBuffer;
	//cRingBuffer		m_ringSendBuffer;
	//클라이언트 주소를 받기위한 버퍼
	char			cAddressBuf[1024];
	//클라이언트와 연결 종료가 되었는지 여부
	BOOL			cIsClosed;
	//클라이언트와 연결이 되어있는지 여부
	BOOL		    cIsConnect;
	//현재 Overlapped I/O 전송 작업을 하고 있는지 여부
	BOOL			cIsSend;


public:
	int ConnNum;

	SOCKET s;
	WSAOVERLAPPED_EX recv_over;

	int prev_recv_size;
	int curr_packet_size;
	unsigned char pakcet_buf[MAX_PACKET_SIZE];


};