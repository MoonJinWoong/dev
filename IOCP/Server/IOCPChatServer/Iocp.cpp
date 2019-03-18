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
	// ����� �Ϸ� ��Ʈ ����
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
		// �񵿱� ����� �Ϸ� ��ٸ���
		DWORD cbTransferred;
		//SOCKET client_sock;
		EX_OVERLAPPED *ptr;

		
		retval = GetQueuedCompletionStatus(cIocpHandle, &cbTransferred,
			&client_id, (LPOVERLAPPED *)&ptr, INFINITE);

		cout << "client ID - " << client_id << endl;
		// Ŭ���̾�Ʈ ���� ���
		//SOCKADDR_IN clientaddr;
		int addrlen = sizeof(cClientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&cClientaddr, &addrlen);

		// �񵿱� ����� ��� Ȯ��
		if (retval == 0 || cbTransferred == 0) {
			if (retval == 0) {
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
					&temp1, FALSE, &temp2);
				cout << "Overlapped error -" << endl;
			}
			closesocket(ptr->sock);
			cout << "[TCP ����] Ŭ���̾�Ʈ ����:  IP �ּ�=" << inet_ntoa(cClientaddr.sin_addr)
				<< "��Ʈ ��ȣ=" << ntohs(cClientaddr.sin_port) << endl;
			
			delete ptr;
			continue;
		}

		// ������ ���۷� ����
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// ���� ������ ���
			ptr->buf[ptr->recvbytes] = '\0';
			cout << "[TCP/" << inet_ntoa(cClientaddr.sin_addr) 
				<< ":" << ntohs(cClientaddr.sin_port)<<","<<ptr->buf << "]" << endl;				
		}
		else {
			ptr->sendbytes += cbTransferred;
		}

		if (ptr->recvbytes > ptr->sendbytes) {
			// ������ ������
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

			// ������ �ޱ�
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
			// ���� class �ϼ��ϰ� WSAAccept�� test �ؾ���
			cServerSock = accept(cListenSock, (SOCKADDR *)&cClientaddr, &addrlen);
			if (cServerSock == INVALID_SOCKET)
				cout << "Accept error -" << WSAGetLastError() << endl;


			cout << "TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=" << inet_ntoa(cClientaddr.sin_addr)
				<< "  ��Ʈ ��ȣ=" << ntohs(cClientaddr.sin_port) << endl;

			// Ŭ�� ���� ��ȣ �ο�
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

			
			// ���ϰ� ����� �Ϸ� ��Ʈ ����
			CreateIoCompletionPort((HANDLE)cServerSock, cIocpHandle, new_client, 0);
			
			// ���� ���� ����ü �Ҵ�
			//EX_OVERLAPPED *ptr = new EX_OVERLAPPED;
			ZeroMemory(reinterpret_cast<char *>(&(sClient[new_client].overlapped))
				, sizeof(sClient[new_client].overlapped));

			sClient[new_client].sock = cServerSock;
			sClient[new_client].recvbytes = sClient[new_client].sendbytes = 0;
			sClient[new_client].wsabuf.buf = sClient[new_client].buf;
			sClient[new_client].wsabuf.len = BUFSIZE;

			// �񵿱� ����� ����
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