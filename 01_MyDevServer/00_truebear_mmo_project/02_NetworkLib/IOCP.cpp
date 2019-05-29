
#include "IOCP.h"
#include "Config.h"

IOCP::IOCP()
{
}

IOCP::~IOCP()
{
	::closesocket(m_ListenSocket);
}

bool IOCP::InitIOCP()
{
	WSADATA   wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;

	::bind(m_ListenSocket, reinterpret_cast<sockaddr*>(&ServerAddr), sizeof(ServerAddr));
	listen(m_ListenSocket, SOMAXCONN);


	cout << "Server Init Complete...." << endl;
	return true;
}

bool IOCP::CreateThreads()
{

		//   worker thread
		vector<thread*> Worker_Threads;
		Worker_Threads.reserve(4);
		for (int i = 0; i < 4; ++i) 
		{ 
			Worker_Threads.push_back(new thread{ &IOCP::workerThread,this }); 
		}

		//  accept thread
		thread Accept_Thread{ &IOCP::AcceptThread,this };

		//  join 
		for (auto wor : Worker_Threads) { wor->join();  delete wor; }
		Worker_Threads.clear();
		Accept_Thread.join();

		CloseServer();

		cout << "Create thread Call...Complete...!!" << endl;
		return true;
}

void IOCP::AcceptThread()
{
	while (true)
	{

		SOCKADDR_IN ClientAddr;
		ZeroMemory(&ClientAddr, sizeof(SOCKADDR_IN));
		ClientAddr.sin_family = AF_INET;
		ClientAddr.sin_port = htons(SERVER_PORT);
		ClientAddr.sin_addr.s_addr = INADDR_ANY;
		int addr_size = sizeof(ClientAddr);


		SOCKET new_client = WSAAccept(m_ListenSocket,
			reinterpret_cast<sockaddr*>(&ClientAddr), &addr_size, NULL, NULL);
		if (new_client == INVALID_SOCKET)
			cout << "WSAAccept error....!" << endl;





		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client), m_IocpHandle, new_id, 0);

		cout << "Client Connected   id - " << new_id << endl;

		DWORD recv_flag = 0;
		int ret = WSARecv(new_client, &user[new_id].recvOver.wsabuf, 1,
			NULL, &recv_flag, &user[new_id].recvOver.over, NULL);
		if (ret == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
				cout << "WSARecv Error in AcceptThread...! ";
		}


	}
}

void IOCP::WorkerThread()
{
	while (true)
	{
		DWORD io_size = 0;
		unsigned long long  ci;
		OverlappedEx* over = nullptr;

		BOOL ret = GetQueuedCompletionStatus(cIocpHandle, &io_size, (PULONG_PTR)& ci,
			reinterpret_cast<LPWSAOVERLAPPED*>(&over), INFINITE);
		//std::cout << "GQCS :" << (int)ci << endl;
		int clientNum = (int)ci;

		if (false == ret) {
			int err_no = WSAGetLastError();
			if (64 == err_no)
				DisConnect(clientNum); // 클라이언트의 접속이 종료됨을 확인
			cout << "GQCS error :" << WSAGetLastError() << endl; // 클로즈소켓을 안해주고 끄면 여기서 에러가 난다
		}
		if (0 == io_size) {
			DisConnect(clientNum); // 함수로 새로 만들어줌
			continue;
		}

		std::cout << "RECV from Client :" << clientNum;
		std::cout << "  IO_SIZE : " << io_size << std::endl;
		std::cout << "msg - " << over->wsabuf.buf << endl;

		over->IOCP_buf[MAX_BUFFER] = {};




		// 메세지 온거 받아주고 
		DWORD recv_flag = 0;
		WSARecv(user[clientNum].socket,
			&user[clientNum].recvOver.wsabuf, 1,
			NULL, &recv_flag, &user[clientNum].recvOver.over, NULL);


		DWORD send_flag = 0;
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (user[i].isConnected == true)
			{
				WSASend(user[i].socket, &over->wsabuf, 1,
					NULL, 0, NULL, NULL);
				cout << "Server -> Client Send ...!" << endl;
			}
		}
	}
}

