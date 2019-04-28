#include "SelectNetwork.h"

namespace NetworkLayer
{
	SelectNetwork::SelectNetwork() {}
	SelectNetwork::~SelectNetwork() 
	{
		for (auto& client : m_ClientsPool)
		{
			if (client.pRecvBuffer) {
				delete[] client.pRecvBuffer;
			}

			if (client.pSendBuffer) {
				delete[] client.pSendBuffer;
			}
		}
	}
	void SelectNetwork::InitNetwork()
	{

		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSocket < 0)
			std::cout << "socket open is failed..." << std::endl;
		

		auto n = 1;
		if (setsockopt(m_ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)& n, sizeof(n)) < 0)
			std::cout << "socket opt re use addr is failed..!" << std::endl;

		auto bindListenRet = BindAndListen();
		if(bindListenRet == false)
			std::cout << "bind and listen is failed.." << std::endl;


		FD_ZERO(&m_ReadSet);
		FD_SET(m_ServerSocket, &m_ReadSet);

		CreateClientPool(MAX_CLIENTS+64);
	}

	bool SelectNetwork::BindAndListen()
	{
		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(9000);

		if (bind(m_ServerSocket, (SOCKADDR*)& server_addr, sizeof(server_addr)) < 0)
			return false;

		// 비동기로 전환
		unsigned long mode = 1;
		if (ioctlsocket(m_ServerSocket, FIONBIO, &mode) == SOCKET_ERROR)
			return false;

		if (listen(m_ServerSocket, 32) == SOCKET_ERROR)
			return false;

		return true;
	}
	PacketLayer::RecvPacketInfo SelectNetwork::GetPacketInfo()
	{
		RecvPacket packetInfo;

		if (m_PacketQueue.empty() == false)
		{
			packetInfo = m_PacketQueue.front();
			m_PacketQueue.pop_front();
		}
		return packetInfo;
	}
	void SelectNetwork::CreateClientPool(const int &maxClient)
	{
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			ClientsInfo client;
			ZeroMemory(&client, sizeof(client));
			client.Index = i;
			client.pRecvBuffer = new char[MaxClientRecvBufferSize];
			client.pSendBuffer = new char[MaxClientSendBufferSize];

			m_ClientsPool.push_back(client);
			m_ClientPoolIndex.push_back(client.Index);
		}
	}

	void SelectNetwork::Run()
	{
		auto r_set = m_ReadSet;
		//연결된 모든 세션을 write 이벤트를 조사하고 있는데 사실 다 할 필요는 없다. 이전에 send 버퍼가 다 찼던 세션만 조사해도 된다.
		auto w_set = m_ReadSet;

		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec
		auto selectResult = select(0, &r_set, &w_set, 0, &timeout);
		if (selectResult == SOCKET_ERROR) std::cout << "select error..!" << std::endl;


		// Accept
		if (FD_ISSET(m_ServerSocket, &r_set))
		{
			auto accepRet = AcceptNewClient();
		}

		// 
		CheckClients(r_set, w_set);

	}
	bool SelectNetwork::AcceptNewClient()
	{
		auto tryCount = 0; // 너무 많이 accept를 시도하지 않도록 한다.

		do
		{
			++tryCount;

			SOCKADDR_IN client_addr;
			auto client_len = static_cast<int>(sizeof(client_addr));
			auto client_sockfd = accept(m_ServerSocket, (SOCKADDR*)& client_addr, &client_len);
		
			if (client_sockfd == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					return true;
				}

				return false;
			}

			auto newSessionIndex = AllocClientIndex();
			if (newSessionIndex < 0)
			{
				return false;
			}


			char clientIP[MAX_IP_LEN] = { 0, };
			inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, MAX_IP_LEN - 1);



			linger ling;
			ling.l_onoff = 0;
			ling.l_linger = 0;
			setsockopt(client_sockfd, SOL_SOCKET, SO_LINGER, (char*)& ling, sizeof(ling));

			int size1 = MaxClientSockOptRecvBufferSize;
			int size2 = MaxClientSockOptSendBufferSize;

			setsockopt(client_sockfd, SOL_SOCKET, SO_RCVBUF, (char*)& size1, sizeof(size1));
			setsockopt(client_sockfd, SOL_SOCKET, SO_SNDBUF, (char*)& size2, sizeof(size2));



			FD_SET(client_sockfd, &m_ReadSet);

			ConnectedClient(newSessionIndex, client_sockfd, clientIP);

		} while (tryCount < FD_SETSIZE);

		return true;

	}
	int SelectNetwork::AllocClientIndex()
	{
		if (m_ClientPoolIndex.empty()) {
			return -1;
		}

		int index = m_ClientPoolIndex.front();
		m_ClientPoolIndex.pop_front();
		return index;
	}
	void SelectNetwork::RejectClient(SOCKET socket, const int sessionIndex)
	{

		if (m_ClientsPool[sessionIndex].IsConnected() == false) {
			return;
		}

		closesocket(socket);
		FD_CLR(socket, &m_ReadSet);

		m_ClientsPool[sessionIndex].Clear();
		--m_ConnectedSessionCount;
		
		m_ClientPoolIndex.push_back(sessionIndex);
		m_ClientsPool[sessionIndex].Clear();

		AddPacketQueue(sessionIndex, (short)PacketId::CLIENT_CLOSE, 0, nullptr);
	}
	void SelectNetwork::ConnectedClient(const int client_index
		, const SOCKET whoSocket, const char* pIP)
	{
		++m_ConnectSeq;

		auto& session = m_ClientsPool[client_index];
		session.unique_id = m_ConnectSeq;
		session.SocketFD = whoSocket;
		memcpy(session.IP, pIP, MAX_IP_LEN - 1);

		++m_ConnectedSessionCount;

		std::cout << "client unique id = " << client_index << std::endl;

		AddPacketQueue(client_index, (short)PacketLayer::PACKET_ID::CONNECTED, 0, nullptr);
	}

	void SelectNetwork::CheckClients(fd_set& read_set, fd_set& write_set)
	{
		for (int i = 0; i < m_ClientsPool.size(); ++i)
		{
			auto& session = m_ClientsPool[i];

			if (session.IsConnected() == false) {
				continue;
			}

			SOCKET fd = session.SocketFD;
			auto sessionIndex = session.Index;

			// check read
			auto retReceive = ProcessRecv(sessionIndex, fd, read_set);
			if (retReceive == false) {
				continue;
			}

			// check write
			ProcessSend(sessionIndex, fd, write_set);
		}

	}

	bool SelectNetwork::ProcessRecv(const int clientIndex, const SOCKET fd, fd_set& read_set)
	{
		if (!FD_ISSET(fd, &read_set))
		{
			return true;
		}

		auto ret = RecvData(clientIndex);
		if (ret != true)
		{
			//CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_ERROR, fd, sessionIndex);
			return false;
		}

		ret = RecvBufferProcess(clientIndex);
		if (ret != true)
		{
			//CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_BUFFER_PROCESS_ERROR, fd, sessionIndex);
			return false;
		}

		return true;
	}
	bool SelectNetwork::RecvData(const int clientIndex)
	{
		auto& session = m_ClientsPool[clientIndex];
		auto fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
		{
			std::cout << "this client is not connected" << std::endl;
			return false;
		}

		int recvPos = 0;

		if (session.RemainingDataSize > 0)
		{
			memcpy(session.pRecvBuffer, &session.pRecvBuffer[session.PrevReadPosInRecvBuffer], session.RemainingDataSize);
			recvPos += session.RemainingDataSize;
		}

		// 왜 이걸 여러번 보내나? 
		auto recvSize = recv(fd, &session.pRecvBuffer[recvPos], (MAX_PACKET_BODY_SIZE * 2), 0);
		std::cout << "recv size - " << recvSize << std::endl;


		if (recvSize == 0)
		{
			return false;
		}

		if (recvSize < 0)
		{
			auto error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		session.RemainingDataSize += recvSize;
		return true;
	}
	bool SelectNetwork::RecvBufferProcess(const int sessionIndex)
	{
		auto& session = m_ClientsPool[sessionIndex];

		auto readPos = 0;
		const auto remainDataSize = session.RemainingDataSize;
		PacketHeader* pPktHeader;

		while ((remainDataSize - readPos) >= sizeof(PacketHeader))
		{
			pPktHeader = (PacketHeader*)& session.pRecvBuffer[readPos];
			readPos += sizeof(PacketHeader);
			auto bodySize = (INT16)(pPktHeader->TotalSize - sizeof(PacketHeader));

			if (bodySize > 0)
			{
				if (bodySize > (remainDataSize - readPos))
				{
					readPos -= sizeof(PacketHeader);
					break;
				}

				if (bodySize > MAX_PACKET_BODY_SIZE)
				{
					//클라이언트 보고 나가라고 하던가 직접 짤라야 한다.
					return false;
				}
			}

			AddPacketQueue(sessionIndex, pPktHeader->Id, bodySize, &session.pRecvBuffer[readPos]);
			readPos += bodySize;
		}

		session.RemainingDataSize -= readPos;
		session.PrevReadPosInRecvBuffer = readPos;

		return true;
	}
	
	
	void SelectNetwork::ProcessSend(const int sessionIndex, const SOCKET fd, fd_set& write_set)
	{
		if (!FD_ISSET(fd, &write_set))
			return;

		// 여기 리턴부터 다시 
		// 여기를 공부해야함 
		auto retsend = SettingSendBuff(sessionIndex);
		if (retsend != true)
		{
			std::cout << "FlushSendBuff error..!" << std::endl;
			RejectClient(fd, sessionIndex);
		}
	}
	bool SelectNetwork::SettingSendBuff(const int sessionIndex)
	{
		auto& session = m_ClientsPool[sessionIndex];
		auto fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
			return false;
		

		auto result = SendData(fd, session.pSendBuffer, session.SendSize);

		if (result != 0) return false;



		auto sendSize = result;


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
	int SelectNetwork::SendData(const SOCKET fd, const char* pMsg, const int size)
	{

		//auto rfds = m_ReadSet;

		// 접속 되어 있는지 또는 보낼 데이터가 있는지
		if (size == 0) return 0;
		else if(size < 0) return -1;


		auto retSend = 0;
		retSend = send(fd, pMsg, size, 0);
		std::cout << "send size ->" << retSend << std::endl;

		 if (retSend <= 0)
			 return 0;

		 return retSend;
	}


	bool SelectNetwork::LogicSendBufferSet(const int sessionIndex, const short packetID
		, const short bodysize, const char* msg)
	{
		auto& session = m_ClientsPool[sessionIndex];

		auto pos = session.SendSize;
		auto totalSize = (bodysize + sizeof(PacketLayer::PktHeader));

		if ((pos + totalSize) > MaxClientSendBufferSize) 
		{
			std::cout << "client SendBuffer is Full ..." << std::endl;
			return false;
		}

		PacketHeader pktHeader{ totalSize, packetID };
		memcpy(&session.pSendBuffer[pos], (char*)& pktHeader, sizeof(PacketLayer::PktHeader));

		if (bodysize > 0)
			memcpy(&session.pSendBuffer[pos + sizeof(PacketLayer::PktHeader)], msg, bodysize);
		
		session.SendSize += totalSize;

		return true;
	}
	

	void SelectNetwork::AddPacketQueue(const int Index, const short pktId, const short bodySize, char* pDataPos)
	{
		RecvPacket packetInfo;
		packetInfo.SessionIndex = Index;
		packetInfo.PacketId = pktId;
		packetInfo.PacketBodySize = bodySize;
		packetInfo.pRefData = pDataPos;

		m_PacketQueue.push_back(packetInfo);
	}
}