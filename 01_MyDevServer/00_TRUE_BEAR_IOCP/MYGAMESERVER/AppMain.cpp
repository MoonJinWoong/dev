#include "..//NetLib/NetTool.h"
#include "../NetLib/IocpServer.h"


class Session
{
public:
	shared_ptr<thread> thread; // 클라이언트 처리를 하는 스레드 1개
	NetTool acceptedSession;		// accept한 TCP 연결
	Session() {}
	Session(SocketType socketType) :acceptedSession(socketType) {}
};

int main()
{
	SYSTEM_INFO system;
	GetSystemInfo(&system);
	auto threadCnt = system.dwNumberOfProcessors * 2;  // ms 권장사항 추후에 바꿔가면서 해볼 것

	// iocp 클래스 생성
	IocpServer Iocp(threadCnt);
		

	// 네트워크 기능을 iocp와 분리했다.epll에도 써먹을 것
	NetTool netTool(SocketType::TCP);
	netTool.bindAndListen("127.0.0.1", 8000);



	// IOCP 등록
	Iocp.Resister(netTool, nullptr);
	


	// 비동기 accpet
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
			if (readEvent.lpCompletionKey == 0) // 리슨소켓이면
			{
				cout << "asdfasdfasfd" << endl;
				netTool.misReadOverlapped = false;
				// 이미 accept은 완료되었다. 귀찮지만, Win32 AcceptEx 사용법에 따르는 마무리 작업을 하자. 
				if (candidateSession->acceptedSession.UpdateAcceptContext(listenSocket) != 0)
				{
					//리슨소켓을 닫았던지 하면 여기서 에러날거다. 그러면 리슨소켓 불능상태로 만들자.
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



