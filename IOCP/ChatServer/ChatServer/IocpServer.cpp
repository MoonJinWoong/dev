
#include "preCompile.h"

// connectionManager �ʱ�ȭ �� ����(������ Ŭ���̾�Ʈ ������)
// iocp���� ����ϱ� ������ �������� ��������. 
// �ٸ� ����� ������ ��� ��.....

ConnectionManager* client = new ConnectionManager[100];

IocpServer::IocpServer()
{
	cClientInfo = new sClientInfo[MAX_USER];
	cClientCnt = 0;
	cIOCP = INVALID_HANDLE_VALUE;
	//cWorkerIOCP = INVALID_HANDLE_VALUE;
}
IocpServer::~IocpServer()
{

}



bool IocpServer::start(INITCONFIG initconfig)
{



	if (InitializeSocket(initconfig) == false)
		return false;
	

	//if (!CreateWorkerIOCP())
	//	return false;
	//if (!CreateProcessIOCP())
	//	return false;
	//if (!CreateWorkerThreads())
	//	return false;
	//if (!CreateProcessThreads())
	//	return false;
	//if (!CreateListenSock())
	//	return false;
}

bool IocpServer::InitializeSocket(INITCONFIG initconfig)
{
	// WSAStartup -> iocp ��ü ���� -->CreateIoCompletionPort --> Socket ����
	// --> bind and listen

	//CompletionPort��ü ���� ��û�� �Ѵ�.
	cIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE
		, NULL, NULL, 0);


	cWorkerThreadCount = initconfig.sWorkerThreadCnt;
	for (DWORD i = 0; i < cWorkerThreadCount; i++)
		cWorkerThread[i] = INVALID_HANDLE_VALUE;

	

	// ��Ʈ��ũ �ʱ�ȭ 
	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (nRet)
	{
		cout << "failed.. WSAStartup...!!" << endl;
		return false;
	}

	//iocp ��Ŀ ����
	//cWorkerIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	// �������� IO ����
	cSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;


	// bind and listen
	::bind(cSocket, reinterpret_cast<sockaddr *>(&ServerAddr), sizeof(ServerAddr));
	int res = listen(cSocket, SOMAXCONN);
	if (res != 0) {
		cout << "liten call failed...!!!" << endl;
		return false;
	}


	return true;
}


void IocpServer::makeThreads()
{
	// accept thread
	thread Accept_Thread{ &IocpServer::AcceptThread,this };

	//   worker thread
	vector<thread *> Worker_Thread;
	Worker_Thread.reserve(4);
	for (int i = 0; i < 4; ++i) 
		Worker_Thread.push_back(new thread{ &IocpServer::WorkerThread,this }); 
	

	// join
	for (auto wor : Worker_Thread) { wor->join();  delete wor; }
	Worker_Thread.clear();
	Accept_Thread.join();
}

void IocpServer::AcceptThread()
{
	SOCKADDR_IN		ClientAddr;
	int addr_len = sizeof(SOCKADDR_IN);

	while (1) 
	{
		sClientInfo* getClientInfo = GetEmptyClientInfo();
		if (getClientInfo == nullptr) cout << "client is full....." << endl;

		getClientInfo->sSocketClient = WSAAccept(cSocket, reinterpret_cast<sockaddr *>(&ClientAddr), &addr_len, NULL, NULL);

		if (INVALID_SOCKET == getClientInfo->sSocketClient)
			continue;

		 
		 // ���� client�� IOCP�� bind ���ش�. 
		 if (BindAndIOPort(getClientInfo) == false) return;

		 // recv overlapped io ��û
		 if (BindRecv(getClientInfo) == false) return;

		 cClientCnt++;



		 cout << "new client recv...." << endl;
		 cout << "IP Addr : " << inet_ntoa(ClientAddr.sin_addr) << "   port : " << ntohs(ClientAddr.sin_port) << endl;
		 cout << "Accept Client Count --> " << cClientCnt << endl;
	}
}

void IocpServer::WorkerThread()
{
	
		//cout << "��Ŀ ������ ������" << endl;
		sClientInfo* getWorkerClient = nullptr;
		//�Լ� ȣ�� ���� ����
		bool isSuccess = true;
		//Overlapped I/O�۾����� ���۵� ������ ũ��
		DWORD dwIoSize = 0;
		//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
		LPOVERLAPPED lpOverlapped = nullptr;

	while (true)
	{
		//������ Ǯ ����. ť���� �۾� �Ϸ�� IO�� ������ �о� ���δ�. ���������� ���
		isSuccess = GetQueuedCompletionStatus(cIOCP, &dwIoSize,(PULONG_PTR)&cClientInfo,
			reinterpret_cast<LPWSAOVERLAPPED *>(&lpOverlapped), INFINITE);
		
		//cout << (PULONG_PTR)&cClientInfo << endl;
		if (isSuccess == false) cout << "GQCS failed...!" << endl;

		
		WSAOVERLAPPED_EX* recvOverlappedEx = (WSAOVERLAPPED_EX*)lpOverlapped;
		
		if (recvOverlappedEx->event_type == E_RECV)
		{
			recvOverlappedEx->IOCP_buf[dwIoSize] = NULL;
			cout<<"recv bytes :"<<dwIoSize<<
				  "   msg :"<< recvOverlappedEx->IOCP_buf<<endl;
			

			//Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
			int ret = SendMsg(cClientInfo, recvOverlappedEx->IOCP_buf, dwIoSize);
			//cout << "eeeee    " << ret << endl;

			if (ret == false) cout << "Send Msg failed...!" << endl;

		}

		//else if (pOver->event_type == E_SEND)
		//{
		//	if (io_size != pOver->IOCP_buf[0])
		//	{
		//		cout << "incomplete packet send error" << endl;
		//		exit(-1);
		//	}
		//	delete pOver;
		//}

		//else if (pOver->event_type == E_DOAI)
		//{
		//	Heart_Beat(cl);  // ���ϸ� �л� ��Ŵ , player ���� �񱳸� �ؾߵǼ� iocp Ŭ�������� ó���ؾ���
		//	delete pOver;
		//}

		//else if (pOver->event_type == E_NPC_ATTACK)
		//{
		//	NPC_ATTACK(cl);
		//	delete pOver;
		//}
		//else
		//{
		//	cout << "Unknown GQCS Event type! " << endl;
		//	exit(-1);
		//}
	}
}

void IocpServer::ProcessPacket(int clientNum, unsigned char *packet)
{
	switch (packet[1])
	{
	case CS_MSG:
		break;
	}
}

sClientInfo* IocpServer::GetEmptyClientInfo()
{
	for (int i = 0; i < MAX_USER; i++)
	{
		if (INVALID_SOCKET == cClientInfo[i].sSocketClient)
			return &cClientInfo[i];
	}
	return nullptr;
}

bool IocpServer::BindAndIOPort(sClientInfo * getClientInfo)
{

	HANDLE handleIOCP;

	//socket�� pClientInfo�� CompletionPort��ü�� �����Ų��.
	handleIOCP = CreateIoCompletionPort((HANDLE)getClientInfo->sSocketClient
		, cIOCP
		, (ULONG_PTR)(getClientInfo), 0);
	

	if (NULL == handleIOCP || cIOCP != handleIOCP)
	{
		cout << " CreateIoCompletionPort() call failed...!!  error:" << GetLastError() << endl;
		return false;
	}
	return true;

}
bool IocpServer::BindRecv(sClientInfo* recvClientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;


	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	recvClientInfo->sRecvOverlappedEx.wsabuf.len = MAX_BUFF_SIZE;
	recvClientInfo->sRecvOverlappedEx.wsabuf.buf =
		recvClientInfo->sRecvOverlappedEx.IOCP_buf;
	recvClientInfo->sRecvOverlappedEx.event_type = E_RECV;


	int nRet = WSARecv(recvClientInfo->sSocketClient,
		&(recvClientInfo->sRecvOverlappedEx.wsabuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED)&(recvClientInfo->sRecvOverlappedEx),
		NULL);
	cout << "WSARecv -" << nRet << endl;
	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		cout << " BindRecv WSARecv() call failed...!  error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
bool IocpServer::SendMsg(sClientInfo* client, char* msg, int len)
{

	
	WSAOVERLAPPED_EX *send_over = new WSAOVERLAPPED_EX;
	ZeroMemory(send_over, sizeof(*send_over));
	send_over->event_type = E_SEND;
	memcpy(send_over->IOCP_buf, msg, len);
	send_over->wsabuf.buf = reinterpret_cast<CHAR *>(send_over->IOCP_buf);
	send_over->wsabuf.len = len;
	DWORD send_flag = 0;
	int ret = WSASend(client->sSocketClient, &send_over->wsabuf, 1, NULL, send_flag, &send_over->over, NULL);
	
	if (ret != 0)
		cout << "WSASend Failed...   err-" << GetLastError() << endl;



	return true;
}