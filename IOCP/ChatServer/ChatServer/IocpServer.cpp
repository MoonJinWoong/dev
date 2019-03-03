
#include "preCompile.h"

// connectionManager �ʱ�ȭ �� ����(������ Ŭ���̾�Ʈ ������)
// iocp���� ����ϱ� ������ �������� ��������. 
// �ٸ� ����� ������ ��� ��.....

ConnectionManager* client = new ConnectionManager[100];

IocpServer::IocpServer()
{
	
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

	cWorkerThreadCount = initconfig.sWorkerThreadCnt;
	for (DWORD i = 0; i < cWorkerThreadCount; i++)
		cWorkerThread[i] = INVALID_HANDLE_VALUE;

	cWorkerIOCP = INVALID_HANDLE_VALUE;

	// ��Ʈ��ũ �ʱ�ȭ 
	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (nRet)
	{
		cout << "failed.. WSAStartup...!!" << endl;
		return false;
	}

	//iocp Ŀ�ΰ�ü ����
	cWorkerIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

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
	SOCKADDR_IN ClientAddr;
	ZeroMemory(&ClientAddr, sizeof(SOCKADDR_IN));
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_port = htons(SERVER_PORT);
	ClientAddr.sin_addr.s_addr = INADDR_ANY;
	int addr_len = sizeof(ClientAddr);

	while (1) 
	{
		SOCKET sClient = WSAAccept(cSocket, reinterpret_cast<sockaddr *>(&ClientAddr), &addr_len, NULL, NULL);

		if (INVALID_SOCKET == sClient)
			cout << "accept thread error...!" << endl;


		 cout << "new client recv....!! " << endl;;
	}
}

void IocpServer::WorkerThread()
{
	while (true)
	{
		//cout << "��Ŀ ������ ������" << endl;
		DWORD io_size;
		unsigned long long cl;
		WSAOVERLAPPED_EX *pOver;



		//������ Ǯ ����. ť���� �۾� �Ϸ�� IO�� ������ �о� ���δ�. ���������� ���
		BOOL is_ok = GetQueuedCompletionStatus(cWorkerIOCP, &io_size, &cl,
			reinterpret_cast<LPWSAOVERLAPPED *>(&pOver), INFINITE);

		//if (is_ok == false)
		//{
		//	int err_no = WSAGetLastError();
		//	if (err_no == 64) DisconnectClient(cl);
		//	else err_display("GQCS ���� !! : ", WSAGetLastError());
		//}

		//if (io_size == 0)
		//{
		//	DisconnectClient(cl);
		//	continue;
		//}

		if (pOver->event_type == E_RECV)
		{

			int to_process = io_size;
			
			unsigned char *buf_ptr = client[cl].recv_over.IOCP_buf;
			unsigned char packet_buf[MAX_PACKET_SIZE];
			int psize = client[cl].curr_packet_size;
			int pr_size = client[cl].prev_recv_size;
			while (to_process != 0)
			{
				if (psize == 0) psize = buf_ptr[0];
				if (psize <= to_process + pr_size)
				{
					memcpy(packet_buf, client[cl].pakcet_buf, pr_size);
					memcpy(packet_buf + pr_size, buf_ptr, psize - pr_size);
					//cout << "���μ��� ó�� ��" << mClients[cl].x << "   " << mClients[cl].z << endl;
					//mClients[cl].ProcessPakcet(cl,packet_buf,mClients[cl].exp,mClients[cl].npcLevel,mClients[cl].hp);
					//cout << "���μ��� ó�� ��" << mClients[cl].x << "   " << mClients[cl].z << endl;
					ProcessPacket(static_cast<int>(cl), packet_buf);
					
					to_process -= psize - pr_size; buf_ptr += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else
				{
					memcpy(client[cl].pakcet_buf + pr_size, buf_ptr, to_process);
					pr_size += to_process;
					buf_ptr += to_process;
					to_process = 0;
				}
			}

			client[cl].curr_packet_size = psize;
			client[cl].prev_recv_size = pr_size;
			DWORD recvFlag = 0;

			int ret = WSARecv(client[cl].s, &client[cl].recv_over.wsabuf,
				1, NULL, &recvFlag, &client[cl].recv_over.over, NULL);

			if (ret != 0)
				cout << "WSARecv error....!!!!!" << endl;
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