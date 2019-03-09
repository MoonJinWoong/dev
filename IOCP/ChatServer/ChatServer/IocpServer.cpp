
#include "preCompile.h"

// connectionManager 초기화 및 생성(들어오는 클라이언트 관리용)
// iocp에서 사용하기 때문에 전역으로 선언해줌. 
// 다른 방법이 없을까 고민 중.....

ConnectionManager* client = new ConnectionManager[100];

IocpServer::IocpServer()
{
	cClientInfo = new sClientInfo[MAX_USER];
	cClientCnt = 0;
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
	// WSAStartup -> iocp 객체 생성 -->CreateIoCompletionPort --> Socket 생성
	// --> bind and listen

	//CompletionPort객체 생성 요청을 한다.
	cIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE
		, NULL, NULL, 0);


	cWorkerThreadCount = initconfig.sWorkerThreadCnt;
	for (DWORD i = 0; i < cWorkerThreadCount; i++)
		cWorkerThread[i] = INVALID_HANDLE_VALUE;

	cWorkerIOCP = INVALID_HANDLE_VALUE;

	// 네트워크 초기화 
	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (nRet)
	{
		cout << "failed.. WSAStartup...!!" << endl;
		return false;
	}

	//iocp 커널객체 생성
	cWorkerIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	// 오버랩드 IO 생성
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

		 
		 // 들어온 client와 IOCP와 bind 해준다. 
		 if (BindAndIOPort(getClientInfo) == false) return;

		 // recv overlapped io 요청
		 if (BindRecv(getClientInfo) == false) return;

		 cClientCnt++;



		 cout << "new client recv...." << endl;
		 cout << "IP Addr : " << inet_ntoa(ClientAddr.sin_addr) << "   port : " << ntohs(ClientAddr.sin_port) << endl;
		 cout << "Accept Client Count --> " << cClientCnt << endl;
	}
}

void IocpServer::WorkerThread()
{
	
		//cout << "워커 스레드 동작중" << endl;
		sClientInfo* getWorkerClient = nullptr;
		//함수 호출 성공 여부
		bool isSuccess = true;
		//Overlapped I/O작업에서 전송된 데이터 크기
		DWORD dwIoSize = 0;
		//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
		LPOVERLAPPED lpOverlapped = nullptr;

	while (true)
	{
		cout << "워커 대기중..." << endl;
		//스레드 풀 생성. 큐에서 작업 완료된 IO가 있으면 읽어 들인다. 그전까지는 대기
		isSuccess = GetQueuedCompletionStatus(cWorkerIOCP, &dwIoSize,(PULONG_PTR)&cClientInfo,
			reinterpret_cast<LPWSAOVERLAPPED *>(&lpOverlapped), INFINITE);
		cout << "여기 오니...." << endl;
		
		if (isSuccess == false) cout << "GQCS failed...!" << endl;

		WSAOVERLAPPED_EX* recvOverlappedEx = (WSAOVERLAPPED_EX*)lpOverlapped;
		if (recvOverlappedEx->event_type == E_RECV)
		{
			recvOverlappedEx->IOCP_buf[dwIoSize] = NULL;
			cout<<"send bytes :"<<dwIoSize<<
				  "   msg :"<< recvOverlappedEx->IOCP_buf<<endl;


			//클라이언트에 메세지를 에코한다.
			SendMsg(getWorkerClient, recvOverlappedEx->IOCP_buf, dwIoSize);
			BindRecv(getWorkerClient);
			
			
			//int to_process = io_size;
			//
			//char *buf_ptr = client[cl].recv_over.IOCP_buf;
			//unsigned char packet_buf[MAX_PACKET_SIZE];
			//int psize = client[cl].curr_packet_size;
			//int pr_size = client[cl].prev_recv_size;
			//while (to_process != 0)
			//{
			//	if (psize == 0) psize = buf_ptr[0];
			//	if (psize <= to_process + pr_size)
			//	{
			//		memcpy(packet_buf, client[cl].pakcet_buf, pr_size);
			//		memcpy(packet_buf + pr_size, buf_ptr, psize - pr_size);
			//		//cout << "프로세스 처리 전" << mClients[cl].x << "   " << mClients[cl].z << endl;
			//		//mClients[cl].ProcessPakcet(cl,packet_buf,mClients[cl].exp,mClients[cl].npcLevel,mClients[cl].hp);
			//		//cout << "프로세스 처리 후" << mClients[cl].x << "   " << mClients[cl].z << endl;
			//		ProcessPacket(static_cast<int>(cl), packet_buf);
			//		
			//		to_process -= psize - pr_size; buf_ptr += psize - pr_size;
			//		psize = 0; pr_size = 0;
			//	}
			//	else
			//	{
			//		memcpy(client[cl].pakcet_buf + pr_size, buf_ptr, to_process);
			//		pr_size += to_process;
			//		buf_ptr += to_process;
			//		to_process = 0;
			//	}
			//}

			//client[cl].curr_packet_size = psize;
			//client[cl].prev_recv_size = pr_size;
			//DWORD recvFlag = 0;

			//int ret = WSARecv(client[cl].s, &client[cl].recv_over.wsabuf,
			//	1, NULL, &recvFlag, &client[cl].recv_over.over, NULL);

			//if (ret != 0)
			//	cout << "WSARecv error....!!!!!" << endl;
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
		//	Heart_Beat(cl);  // 부하를 분산 시킴 , player 간에 비교를 해야되서 iocp 클래스에서 처리해야함
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

	//socket과 pClientInfo를 CompletionPort객체와 연결시킨다.
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


	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
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

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		cout << " BindRecv WSARecv() call failed...!  error:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
bool IocpServer::SendMsg(sClientInfo* sendClient, char* msg, int len)
{
	DWORD dwRecvNumBytes = 0;

	//전송될 메세지를 복사
	memcpy(sendClient->sSendOverlappedEx.IOCP_buf, msg, len);



	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	sendClient->sSendOverlappedEx.wsabuf.len = len;
	sendClient->sSendOverlappedEx.wsabuf.buf =
		sendClient->sSendOverlappedEx.IOCP_buf;
	sendClient->sSendOverlappedEx.event_type = E_SEND;

	int nRet = WSASend(sendClient->sSocketClient,
		&(sendClient->sSendOverlappedEx.wsabuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED)&(sendClient->sSendOverlappedEx),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		cout<<"SendMsg call is failed...! error:"<< WSAGetLastError()<<endl;
		return false;
	}
	return true;
}