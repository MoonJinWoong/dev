#include "..//NetLib/NetTool.h"
#include "../NetLib/IocpServer.h"


class Session
{
public:
	shared_ptr<thread> thread; // Ŭ���̾�Ʈ ó���� �ϴ� ������ 1��
	NetTool acceptedSession;		// accept�� TCP ����
	Session() {}
	Session(SocketType socketType) :acceptedSession(socketType) {}
};

int main()
{
	SYSTEM_INFO system;
	GetSystemInfo(&system);
	auto threadCnt = system.dwNumberOfProcessors * 2;  // ms ������� ���Ŀ� �ٲ㰡�鼭 �غ� ��

	// iocp Ŭ���� ����
	IocpServer Iocp(threadCnt);
		

	// ��Ʈ��ũ ����� iocp�� �и��ߴ�.epll���� ����� ��
	NetTool netTool(SocketType::TCP);
	netTool.bindAndListen("127.0.0.1", 8000);



	// IOCP ���
	Iocp.Resister(netTool, nullptr);
	


	// �񵿱� accpet
	auto overSession = make_shared<Session>(SocketType::TCP);
	if (!netTool.AsyncAccept(overSession->acceptedSession)
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		cout << "overlapped AcceptEX fail" << endl;
	}
	netTool.misReadOverlapped = true;

	EXOverlapped over;

	while (1)
	{

		if (Iocp.GQCS(over, 100))
		{
			
		}
	}
	

	//// worker thread
	//Thread thread(threadCnt,Iocp.mhIocp);
	//thread.CreateWorkerThread();



	return 0;
}



