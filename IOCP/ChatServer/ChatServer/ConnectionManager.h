#pragma once


class ConnectionManager
{
public:
	ConnectionManager();
	~ConnectionManager();

	void SendPacket(SOCKET &s, unsigned char *packet);

	//Overlapped I/O ��û�� ���� ����
	//LPOVERLAPPED_EX		m_lpRecvOverlappedEx;
	//LPOVERLAPPED_EX		m_lpSendOverlappedEx;
	//Ŭ���̾�Ʈ�� ������ �ۼ����� ���� �� ����
	//cRingBuffer		m_ringRecvBuffer;
	//cRingBuffer		m_ringSendBuffer;
	//Ŭ���̾�Ʈ �ּҸ� �ޱ����� ����
	char			cAddressBuf[1024];
	//Ŭ���̾�Ʈ�� ���� ���ᰡ �Ǿ����� ����
	BOOL			cIsClosed;
	//Ŭ���̾�Ʈ�� ������ �Ǿ��ִ��� ����
	BOOL		    cIsConnect;
	//���� Overlapped I/O ���� �۾��� �ϰ� �ִ��� ����
	BOOL			cIsSend;


public:
	int ConnNum;

	SOCKET s;
	WSAOVERLAPPED_EX recv_over;

	int prev_recv_size;
	int curr_packet_size;
	unsigned char pakcet_buf[MAX_PACKET_SIZE];


};