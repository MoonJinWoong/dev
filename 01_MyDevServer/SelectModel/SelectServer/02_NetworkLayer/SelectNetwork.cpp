
#include "SelectNetwork.h"
namespace NetworkLib
{
	SelectNetwork::SelectNetwork() {}
	SelectNetwork::~SelectNetwork() {}
	NET_ERR_SET SelectNetwork::Init()
	{


		m_nSocketIdx = 0;

		// socket()
		auto ret = InitSocket();
		if (ret != NET_ERR_SET::NONE) return ret;

		// bind and listen
		ret = BindListen(m_Config->Port, m_Config->BackLogCount);
		if (ret != NET_ERR_SET::NONE) return ret;
	

		// 읽기 셋 초기화 
		FD_ZERO(&m_readSet);
		FD_SET(m_ServerSockfd, &m_readSet);


		auto sessionPoolSize = CreateSessionPool(m_Config->MaxClientCount + m_Config->ExtraClientCount);

		std::cout << "Select Network Init" << std::endl;
		return NET_ERR_SET::NONE;
	}
	NET_ERR_SET SelectNetwork::InitSocket()
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSockfd < 0)
			return NET_ERR_SET::SERVER_SOCKET_CREATE_FAIL;

		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
			return NET_ERR_SET::SERVER_SOCKET_SO_REUSEADDR_FAIL;

	}
	NET_ERR_SET SelectNetwork::BindListen(short port, int backlogCount)
	{
		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(port);

		if (bind(m_ServerSockfd, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
			return NET_ERR_SET::SERVER_SOCKET_BIND_FAIL;
		

		unsigned long mode = 1;
		if (ioctlsocket(m_ServerSockfd, FIONBIO, &mode) == SOCKET_ERROR)
			return NET_ERR_SET::SERVER_SOCKET_FIONBIO_FAIL;
		

		if (listen(m_ServerSockfd, backlogCount) == SOCKET_ERROR)
			return NET_ERR_SET::SERVER_SOCKET_LISTEN_FAIL;

		
		return NET_ERR_SET::NONE;
	}

	void SelectNetwork::Run()
	{
		auto read_set = m_readSet;
		auto write_set = m_readSet;

		// select 
		int ret = 0;
		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec
		ret = select(0, &read_set, &write_set, 0, &timeout);
		if (ret == SOCKET_ERROR) 
			std::cout << "SELECT() Error...!  in Run()" << std::endl;
	

		// 소켓셋 검사 첫번째 : Accept
		if (FD_ISSET(m_ServerSockfd, &m_readSet))
			JoinClient();

		// 소켓셋 검사 두번째 : 통신 
		RunCheckSelectClients(read_set, write_set);
	}

	void SelectNetwork::RunCheckSelectClients(fd_set& read_set, fd_set& write_set)
	{
		// 1. 일단 접속해있는지 검사.
		// 2. 모든 접속해있는 클라 소켓 받아오고 인덱스 받아오고
		// 3. recv 해준다. write도 검사해준다
		
		for (int i = 0; i < m_ClientSessionPool.size(); ++i)
		{
			auto& session = m_ClientSessionPool[i];

			if (session.IsConnected() == false) {
				continue;
			}

			SOCKET fd = session.SocketFD;
			auto sessionIndex = session.Index;

			// check read
			auto retReceive = RunProcessReceive(sessionIndex, fd, read_set);
			if (retReceive == false) {
				continue;
			}

			// check write
			RunProcessWrite(sessionIndex, fd, write_set);
		}
	}

	bool SelectNetwork::RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set)
	{
		if (!FD_ISSET(fd, &read_set))
			return true;
		
		// 데이터를 받아온다
		auto ret = RecvData(sessionIndex);
		if (ret != NET_ERR_SET::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_ERROR, fd, sessionIndex);
			return false;
		}

		// 받아온 데이터를 만져서 처리해준다.
		ret = RecvBufferProcess(sessionIndex);
		if (ret != NET_ERR_SET::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_BUFFER_PROCESS_ERROR, fd, sessionIndex);
			return false;
		}

		return true;
	}
	void SelectNetwork::RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set)
	{
		if (!FD_ISSET(fd, &write_set))
			return;
		
		auto retsend = BroadcastingBuffer(sessionIndex);
		if (retsend.Error != NET_ERR_SET::NONE)
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_SEND_ERROR, fd, sessionIndex);
	}
	NET_ERR_SET SelectNetwork::RecvBufferProcess(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];

		auto readPos = 0;
		const auto dataSize = session.RemainingDataSize;
		PacketHeader* pPktHeader;

		while ((dataSize - readPos) >= PACKET_HEADER_SIZE)
		{
			pPktHeader = (PacketHeader*)&session.pRecvBuffer[readPos];
			readPos += PACKET_HEADER_SIZE;
			auto bodySize = (INT16)(pPktHeader->TotalSize - PACKET_HEADER_SIZE);

			if (bodySize > 0)
			{
				if (bodySize > (dataSize - readPos))
				{
					readPos -= PACKET_HEADER_SIZE;
					break;
				}

				if (bodySize > MAX_PACKET_BODY_SIZE)
				{
					// 더 이상 이 세션과는 작업을 하지 않을 예정. 클라이언트 보고 나가라고 하던가 직접 짤라야 한다.
					return NET_ERR_SET::RECV_CLIENT_MAX_PACKET;
				}
			}

			AddPacketQueue(sessionIndex, pPktHeader->Id, bodySize, &session.pRecvBuffer[readPos]);
			readPos += bodySize;
		}

		session.RemainingDataSize -= readPos;
		session.PrevReadPosInRecvBuffer = readPos;

		return NET_ERR_SET::NONE;
	}
	NET_ERR_SET SelectNetwork::RecvData(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];
		auto fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
			return NET_ERR_SET::RECV_PROCESS_NOT_CONNECTED;
		

		int recvPos = 0;

		if (session.RemainingDataSize > 0)
		{
			memcpy(session.pRecvBuffer, &session.pRecvBuffer[session.PrevReadPosInRecvBuffer], session.RemainingDataSize);
			recvPos += session.RemainingDataSize;
		}

		auto recvSize = recv(fd, &session.pRecvBuffer[recvPos], (MAX_PACKET_BODY_SIZE * 2), 0);

		if (recvSize == 0)
			return NET_ERR_SET::RECV_REMOTE_CLOSE;

		if (recvSize < 0)
		{
			auto error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
				return NET_ERR_SET::RECV_API_ERROR;
			
			else
				return NET_ERR_SET::NONE;
		}

		session.RemainingDataSize += recvSize;
		return NET_ERR_SET::NONE;
	}
	NetError SelectNetwork::SendData(const SOCKET fd, const char* pMsg, const int size)
	{
		NetError result(NET_ERR_SET::NONE);
		auto rfds = m_readSet;

		// 접속 되어 있는지 또는 보낼 데이터가 있는지
		if (size <= 0)
			return result;
		
		result.Vlaue = send(fd, pMsg, size, 0);

		if (result.Vlaue <= 0)
			result.Error = NET_ERR_SET::SEND_SIZE_ZERO;
		
		return result;
	}
	NetError SelectNetwork::BroadcastingBuffer(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];
		auto fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
			return NetError(NET_ERR_SET::CLIENT_FLUSH_SEND_BUFF_REMOTE_CLOSE);
		

		auto result = SendData(fd, session.pSendBuffer, session.SendSize);

		if (result.Error != NET_ERR_SET::NONE) 
			return result;
		

		auto sendSize = result.Vlaue;
		if (sendSize < session.SendSize)
		{
			memmove(&session.pSendBuffer[0],
				&session.pSendBuffer[sendSize],
				session.SendSize - sendSize);

			session.SendSize -= sendSize;
		}
		else
			session.SendSize = 0;
		
		return result;
	}
	NET_ERR_SET SelectNetwork::JoinClient()
	{

		// setting 해준 셋 사이즈만큼 돌면서 accept 
		// 이게 단점인거 같다. 다돌아야 되니까 

		int tryCnt = 0;
		do
		{
			++tryCnt;
			// accept , client_socketfd로 accept하고 
			// 세션에 연결 시켜준다. 

			SOCKADDR_IN client_addr;
			auto client_len = static_cast<int>(sizeof(client_addr));
			auto client_sockfd = accept(m_ServerSockfd, (SOCKADDR*)&client_addr, &client_len);
			
			if (client_sockfd == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					return NET_ERR_SET::ACCEPT_API_WSAEWOULDBLOCK;
				return NET_ERR_SET::ACCEPT_API_ERROR;
			}

			// session 할당 받기 
			auto newSessionIndex = AllocClientSessionIndex();
			if (newSessionIndex < 0)
			{
				// 더 이상 수용할 수 없으므로 바로 짜른다.
				CloseSession(SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY, client_sockfd, -1);
				return NET_ERR_SET::ACCEPT_MAX_SESSION_COUNT;
			}

			char clientIP[MAX_IP_LEN] = { 0, };
			inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, MAX_IP_LEN - 1);

			SetSockOption(client_sockfd);

			FD_SET(client_sockfd, &m_readSet);
			//m_pRefLogger->Write(LOG_TYPE::L_DEBUG, "%s | client_sockfd(%I64u)", __FUNCTION__, client_sockfd);
			ConnectedSession(newSessionIndex, client_sockfd, clientIP);



		}while (tryCnt < FD_SETSIZE);


		return NET_ERR_SET::NONE;
	}

	int SelectNetwork::CreateSessionPool(int maxClientCount)
	{
		for (int i = 0; i < maxClientCount; ++i)
		{
			ClientSession session;
			ZeroMemory(&session, sizeof(session));
			session.Index = i;
			session.pRecvBuffer = new char[m_Config->MaxClientRecvBufferSize];
			session.pSendBuffer = new char[m_Config->MaxClientSendBufferSize];

			m_ClientSessionPool.push_back(session);
			m_ClientSessionPoolIndex.push_back(session.Index);
		}

		return maxClientCount;
	}

	int SelectNetwork::AllocClientSessionIndex()
	{
		if (m_ClientSessionPoolIndex.empty()) {
			return -1;
		}

		int index = m_ClientSessionPoolIndex.front();
		m_ClientSessionPoolIndex.pop_front();
		return index;
	}

	void SelectNetwork::CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex)
	{
		if (closeCase == SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY)
		{
			closesocket(sockFD);
			FD_CLR(sockFD, &m_readSet);
			return;
		}
		if (m_ClientSessionPool[sessionIndex].IsConnected() == false)
			return;
		
		closesocket(sockFD);
		FD_CLR(sockFD, &m_readSet);

		m_ClientSessionPool[sessionIndex].Clear();
		--m_ConnectedSessionCnt;
		ReleaseSessionIndex(sessionIndex);

		AddPacketQueue(sessionIndex, (short)PACKET_ID::NTF_SYS_CLOSE_SESSION, 0, nullptr);
	}

	void SelectNetwork::ReleaseSessionIndex(const int index)
	{
		m_ClientSessionPoolIndex.push_back(index);
		m_ClientSessionPool[index].Clear();
	}

	void SelectNetwork::AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos)
	{
		RecvPacketInfo packetInfo;
		packetInfo.SessionIndex = sessionIndex;
		packetInfo.PacketId = pktId;
		packetInfo.PacketBodySize = bodySize;
		packetInfo.pRefData = pDataPos;

		m_PacketQueue.push_back(packetInfo);
	}

	void SelectNetwork::SetSockOption(const SOCKET fd)
	{
		linger ling;
		ling.l_onoff = 0;
		ling.l_linger = 0;
		setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));

		int size1 = m_Config->MaxClientSockOptRecvBufferSize;
		int size2 = m_Config->MaxClientSockOptSendBufferSize;
		setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size1, sizeof(size1));
		setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size2, sizeof(size2));
	}

	void SelectNetwork::ConnectedSession(const int sessionIndex, const SOCKET fd, const char* pIP)
	{
		++m_ConnectSeq;

		auto& session = m_ClientSessionPool[sessionIndex];
		session.Seq = m_ConnectSeq;
		session.SocketFD = fd;
		memcpy(session.IP, pIP, MAX_IP_LEN - 1);

		++m_ConnectedSessionCnt;

		AddPacketQueue(sessionIndex, (short)PACKET_ID::NTF_SYS_CONNECT_SESSION, 0, nullptr);
		
		std::cout << "Connected Client IP:[" << session.IP
			<< "]    Session idx[" <<session.Index << "]" << std::endl;

	
	}
}