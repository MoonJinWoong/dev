#pragma once
const int MAX_SOCKBUF = 1024;	//��Ŷ ũ��
const int MAX_WORKERTHREAD = 4;  //������ Ǯ�� ���� ������ ��

enum class IOOperation
{
	RECV,
	SEND
};

//WSAOVERLAPPED����ü�� Ȯ�� ���Ѽ� �ʿ��� ������ �� �־���.
struct CustomOverEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O����ü
	SOCKET		m_socketClient;			//Ŭ���̾�Ʈ ����
	WSABUF		m_wsaBuf;				//Overlapped I/O�۾� ����

	char		m_RecvBuf[MAX_SOCKBUF]; //������ ����
	char		m_SendBuf[MAX_SOCKBUF]; //������ ����
	IOOperation m_eOperation;			//�۾� ���� ����
};

//Ŭ���̾�Ʈ ������ ������� ����ü
class RemoteClient
{
public:
	SOCKET			stRemoteSock;			//Cliet�� ����Ǵ� ����
	CustomOverEx	stRecvOver;	//RECV Overlapped I/O�۾��� ���� ����
	CustomOverEx	stSendOver;	//SEND Overlapped I/O�۾��� ���� ����

	RemoteClient()
	{
		ZeroMemory(&stRecvOver, sizeof(CustomOverEx));
		ZeroMemory(&stSendOver, sizeof(CustomOverEx));
		stRemoteSock = INVALID_SOCKET;
	}
};