#include "preCompile.h"


Iocp::Iocp() {}
Iocp::~Iocp() {}

BOOL Iocp::StartServer()
{
	if (BindAndListen() == false) return false;

	CreateThreads();
}
BOOL Iocp::InitIOCP(const INITCONFIG & initconfig)
{
	
	cPort = initconfig.nServerPort;
	cSendBufSize = initconfig.nSendBufSize;
	cRecvBufSize = initconfig.nRecvBufSize;
	cWorkerThreadCnt = initconfig.nWorkerThreadCnt;


	if (WSAStartup(MAKEWORD(2, 2), &cWsadata) != 0) cout<<"failed.. WSAStartup...!"<<endl;
	// 입출력 완료 포트 생성
	cIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (cIocpHandle == NULL) cout << "iocp handle create failed...!" << endl;

	cout << "asdfasf" << endl;
	
	return true;
}

BOOL Iocp::BindAndListen()
{
	// socket()
	cListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (cListenSock == INVALID_SOCKET)
	{
		cout << "socket error -" << WSAGetLastError() << endl;
		return false;
	}
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(cPort);
	int ret = ::bind(cListenSock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (ret == SOCKET_ERROR)
	{
		cout << "bind error -" << WSAGetLastError() << endl;
		return false;
	}
	// listen()
	ret = listen(cListenSock, SOMAXCONN);
	if (ret == SOCKET_ERROR) 
	{
		cout << "listen error -" << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

void Iocp::CreateThreads()
{

		//   worker thread
		//vector<thread *> workerThreads;

		workerThreads.reserve(cWorkerThreadCnt);
		for (int i = 0; i < cWorkerThreadCnt; ++i)
			workerThreads.push_back(new thread{ &Iocp::WorkerThread,this });
		
		//  accept thread
		thread Accept_Thread{ &Iocp::AcceptThread,this };

		//  join 
		for (auto wor : workerThreads) { wor->join();  delete wor; }
		workerThreads.clear();
		Accept_Thread.join();

		CloseServer();
}

void Iocp::WorkerThread()
{
	int retval;
	

	while (1) {
		// 비동기 입출력 완료 기다리기
		DWORD cbTransferred;
		//SOCKET client_sock;
		EX_OVERLAPPED *ptr;
		
		
		retval = GetQueuedCompletionStatus(cIocpHandle, &cbTransferred,
			(PULONG_PTR)&cClientSock, (LPOVERLAPPED *)&ptr, INFINITE);

		// 클라이언트 정보 얻기
		//SOCKADDR_IN clientaddr;
		int addrlen = sizeof(cClientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&cClientaddr, &addrlen);

		// 비동기 입출력 결과 확인
		if (retval == 0 || cbTransferred == 0) {
			if (retval == 0) {
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
					&temp1, FALSE, &temp2);
				cout << "Overlapped error -" << endl;
			}
			closesocket(ptr->sock);
			printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(cClientaddr.sin_addr), ntohs(cClientaddr.sin_port));
			delete ptr;
			continue;
		}

		// 데이터 전송량 갱신
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// 받은 데이터 출력
			ptr->buf[ptr->recvbytes] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(cClientaddr.sin_addr),
				ntohs(cClientaddr.sin_port), ptr->buf);
		}
		else {
			ptr->sendbytes += cbTransferred;
		}

		if (ptr->recvbytes > ptr->sendbytes) {
			// 데이터 보내기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			DWORD sendbytes;
			retval = WSASend(ptr->sock, &ptr->wsabuf, 1,
				&sendbytes, 0, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					cout << "WSASend error -" << WSAGetLastError() << endl;
				}
			}
		}
		else {
			ptr->recvbytes = 0;

			// 데이터 받기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf;
			ptr->wsabuf.len = BUFSIZE;

			DWORD recvbytes;
			DWORD flags = 0;
			retval = WSARecv(ptr->sock, &ptr->wsabuf, 1,
				&recvbytes, &flags, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					cout << "WSARecv error -" << WSAGetLastError() << endl;
				}
			}
		}
	}
}

void Iocp::AcceptThread()
{

	int addrlen;
	DWORD recvbytes, flags;

		addrlen = sizeof(cClientaddr);
		
		while (1) {
			// 접속 class 완성하고 WSAAccept와 test 해야함
			cClientSock = accept(cListenSock, (SOCKADDR *)&cClientaddr, &addrlen);
			if (cClientSock == INVALID_SOCKET)
				cout << "Accept error -" << WSAGetLastError() << endl;


			cout << "TCP 서버] 클라이언트 접속: IP 주소=" << inet_ntoa(cClientaddr.sin_addr)
				<< "  포트 번호=" << ntohs(cClientaddr.sin_port) << endl;

			
			// 소켓과 입출력 완료 포트 연결
			CreateIoCompletionPort((HANDLE)cClientSock, cIocpHandle, (ULONG_PTR)this, 0);

			// 소켓 정보 구조체 할당
			EX_OVERLAPPED *ptr = new EX_OVERLAPPED;
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->sock = cClientSock;
			ptr->recvbytes = ptr->sendbytes = 0;
			ptr->wsabuf.buf = ptr->buf;
			ptr->wsabuf.len = BUFSIZE;

			// 비동기 입출력 시작
			flags = 0;
			int retval = WSARecv(cClientSock, &ptr->wsabuf, 1, &recvbytes,
				&flags, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					cout << "recv error -" << WSAGetLastError() << endl;
				}
			}
		}
}

void Iocp::CloseServer()
{

	closesocket(cClientSock);
	closesocket(cListenSock);
	WSACleanup();
}