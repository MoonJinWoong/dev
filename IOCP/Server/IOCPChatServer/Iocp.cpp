#include "preCompile.h"

Connection Clients[MAX_USER];
EX_OVERLAPPED sClient[MAX_USER];


Iocp::Iocp() {}
Iocp::~Iocp() {}

BOOL Iocp::StartServer()
{
	if (BindAndListen() == false) return false;

	CreateThreads();
}
BOOL Iocp::InitIOCP(const INITCONFIG & initconfig)
{
	for (int i = 0; i < MAX_USER; i++)
		Clients[i].setIsConnected(false);


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
		unsigned long long  client_id = -1;	

	while (1) {
		// 비동기 입출력 완료 기다리기
		DWORD cbTransferred;
		//SOCKET client_sock;
		EX_OVERLAPPED *ptr;

		
		retval = GetQueuedCompletionStatus(cIocpHandle, &cbTransferred,
			&client_id, (LPOVERLAPPED *)&ptr, INFINITE);

		cout << "client ID - " << client_id << endl;
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
			cout << "[TCP 서버] 클라이언트 종료:  IP 주소=" << inet_ntoa(cClientaddr.sin_addr)
				<< "포트 번호=" << ntohs(cClientaddr.sin_port) << endl;
			
			delete ptr;
			continue;
		}

		// 데이터 전송량 갱신
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// 받은 데이터 출력
			ptr->buf[ptr->recvbytes] = '\0';
			cout << "[TCP/" << inet_ntoa(cClientaddr.sin_addr) 
				<< ":" << ntohs(cClientaddr.sin_port)<<","<<ptr->buf << "]" << endl;				
		}
		else {
			ptr->sendbytes += cbTransferred;
		}

		if (ptr->recvbytes > ptr->sendbytes) {
			// 데이터 보내기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;
			
			sClient[client_id].wsabuf.buf = ptr->wsabuf.buf;
			sClient[client_id].wsabuf.len = ptr->wsabuf.len;
			DWORD sendbytes;

		
					retval = WSASend(sClient[client_id].sock, &ptr->wsabuf, 1,
						&sendbytes, 0, &sClient[client_id].overlapped, NULL);

			if (retval == SOCKET_ERROR) 
			{
				if (WSAGetLastError() != WSA_IO_PENDING) 
				{
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

			sClient[client_id].wsabuf.buf = ptr->wsabuf.buf;
			sClient[client_id].wsabuf.len = ptr->wsabuf.len;

			retval = WSARecv(sClient[client_id].sock, &sClient[client_id].wsabuf, 1,
				&recvbytes, &flags, &sClient[client_id].overlapped, NULL);
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
			cServerSock = accept(cListenSock, (SOCKADDR *)&cClientaddr, &addrlen);
			if (cServerSock == INVALID_SOCKET)
				cout << "Accept error -" << WSAGetLastError() << endl;


			cout << "TCP 서버] 클라이언트 접속: IP 주소=" << inet_ntoa(cClientaddr.sin_addr)
				<< "  포트 번호=" << ntohs(cClientaddr.sin_port) << endl;

			// 클라 고유 번호 부여
			int new_client = -1;
			for (int i = 0; i < MAX_USER; i++)
			{
				if (Clients[i].getIsConnected() == false)
				{
					Clients[i].setIsConnected(true);
					new_client = i;
					break;
				}
			}

			
			// 소켓과 입출력 완료 포트 연결
			CreateIoCompletionPort((HANDLE)cServerSock, cIocpHandle, new_client, 0);
			
			// 소켓 정보 구조체 할당
			//EX_OVERLAPPED *ptr = new EX_OVERLAPPED;
			ZeroMemory(reinterpret_cast<char *>(&(sClient[new_client].overlapped))
				, sizeof(sClient[new_client].overlapped));

			sClient[new_client].sock = cServerSock;
			sClient[new_client].recvbytes = sClient[new_client].sendbytes = 0;
			sClient[new_client].wsabuf.buf = sClient[new_client].buf;
			sClient[new_client].wsabuf.len = BUFSIZE;

			// 비동기 입출력 시작
			flags = 0;
			int retval = WSARecv(cServerSock, &sClient[new_client].wsabuf, 1, &recvbytes,
				&flags, &sClient[new_client].overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					cout << "recv error -" << WSAGetLastError() << endl;
				}
			}
		}
}

void Iocp::CloseServer()
{
	for (int i = 0; i < MAX_USER; i++)
		closesocket(cClientSock[i]);

	closesocket(cServerSock);
	closesocket(cListenSock);

	WSACleanup();
}