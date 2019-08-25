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
	auto candidateSession = make_shared<Session>(SocketType::TCP);
	if (!netTool.AsyncAccept(candidateSession->acceptedSession)
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		cout << "overlapped AcceptEX fail" << endl;
	}
	netTool.misReadOverlapped = true;

	



	while (1)
	{
		IocpEvents readEvents;
		Iocp.RunGQCS(readEvents, 100);

		for (int i = 0; i < readEvents.mEventCount; i++)
		{
			auto& readEvent = readEvents.mEvents[i];
			if (readEvent.lpCompletionKey == 0) // ���������̸�
			{
				cout << "asdfasdfasfd" << endl;
				netTool.misReadOverlapped = false;
				// �̹� accept�� �Ϸ�Ǿ���. ��������, Win32 AcceptEx ������ ������ ������ �۾��� ����. 
				if (candidateSession->acceptedSession.UpdateAcceptContext(listenSocket) != 0)
				{
					//���������� �ݾҴ��� �ϸ� ���⼭ �������Ŵ�. �׷��� �������� �Ҵɻ��·� ������.
					listenSocket.Close();
				}
			}
	


		}

	}
	

	//// worker thread
	//Thread thread(threadCnt,Iocp.mhIocp);
	//thread.CreateWorkerThread();



	return 0;
}



