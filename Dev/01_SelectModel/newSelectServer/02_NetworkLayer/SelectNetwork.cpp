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
			std::cout << "socket opt reuse addr is failed..!" << std::endl;

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
	void SelectNetwork::AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos)
	{
		RecvPacket packetInfo;
		packetInfo.clientIdx = sessionIndex;
		packetInfo.PacketId = pktId;
		packetInfo.PacketBodySize = bodySize;
		packetInfo.pRefData = pDataPos;

		m_PacketQueue.push_back(packetInfo);
	}

	void SelectNetwork::CreateClientPool(const int &maxClient)
	{
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			ClientsInfo client;
			ZeroMemory(&client, sizeof(client));
			client.Index = i;
			client.pRecvBuffer = new char[MaxClientRecvBufSize];
			client.pSendBuffer = new char[MaxClientSendBufSize];

			m_ClientsPool.push_back(client);
			m_ClientPoolIndex.push_back(client.Index);
		}
	}

	NET_ERROR_SET SelectNetwork::AcceptNewClient()
	{
		auto tryCount = 0; // 너무 많이 accept를 시도하지 않도록 한다.

		do
		{
			++tryCount;

			SOCKADDR_IN client_addr;
			auto client_len = static_cast<int>(sizeof(client_addr));
			auto accept_socket = accept(m_ServerSocket, (SOCKADDR*)& client_addr, &client_len);

			if (accept_socket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					return NET_ERROR_SET::ACCEPT_API_WSAEWOULDBLOCK;

				return NET_ERROR_SET::ACCEPT_ERROR;
			}

			auto newSessionIndex = AllocClientIndex();
			if (newSessionIndex < 0)
			{
				ShutdownClient(SOCKET_CLOSE_CASE::SESSION_POOL_MAX, accept_socket, -1);
				return NET_ERROR_SET::ACCEPT_MAX_COUNT;
			}


			char clientIP[MAX_IP_LEN] = { 0, };
			inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, MAX_IP_LEN - 1);

			SetSocketOpt(accept_socket);

			FD_SET(accept_socket, &m_ReadSet);
			ConnectedClient(newSessionIndex, accept_socket, clientIP);

		} while (tryCount < FD_SETSIZE);

		return NET_ERROR_SET::NONE;
	}
	void SelectNetwork::ConnectedClient(const int index, const SOCKET socket, const char* IP)
	{
		++m_ConnectSeq;

		auto& session = m_ClientsPool[index];
		session.unique_id = m_ConnectSeq;
		session.SocketFD = socket;
		memcpy(session.IP, IP, MAX_IP_LEN - 1);

		++m_IsConnectedCnt;

		AddPacketQueue(index, (short)PacketId::SC_CONNECTION, 0, nullptr);

		std::cout << std::endl << "client connected index -> " << index << std::endl;
	}
	int SelectNetwork::AllocClientIndex()
	{
		// --> 더이상 뺄게 없으면 죄다 할당한 상태
		if (m_ClientPoolIndex.empty())
			return -1;

		int index = m_ClientPoolIndex.front();
		m_ClientPoolIndex.pop_front();
		return index;
	}


	void SelectNetwork::Run()
	{
		auto r_set = m_ReadSet;
		auto w_set = m_ReadSet;

		// Select 
		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec
		auto ret = select(0, &r_set, &w_set, 0, &timeout);
		if (ret == 0 || ret == -1) // -1은 에러이다
			return;
		
		// Accept
		if (FD_ISSET(m_ServerSocket, &r_set))
			auto accepRet = AcceptNewClient();
		
		// read write set 검사. 
		checkClient(r_set, w_set);
	}


	void SelectNetwork::checkClient(fd_set& r_set, fd_set& w_set)
	{
		for (int i = 0; i < m_ClientsPool.size(); ++i)
		{
			auto& client = m_ClientsPool[i];

			if (client.IsConnected() == false)
				continue;

			SOCKET fd = client.SocketFD;
			auto Index = client.Index;

			// check read
			auto retReceive = checkRead(Index, fd, r_set);
			if (retReceive == false) continue;
			

			checkWrite(Index, fd, w_set);
			
		}
	}

	bool SelectNetwork::checkRead(const int index, const SOCKET fd, fd_set& r_set)
	{
		if (!FD_ISSET(fd, &r_set))
			return true;
		
		// recv 받고 예외처리하고 버퍼 분해한다. 

		auto ret = RecvData(index);
		if (ret != NET_ERROR_SET::NONE)
		{
			ShutdownClient(SOCKET_CLOSE_CASE::RECV_ERROR, fd, index);
			return false;
		}

		ret = RecvBufferProc(index);
		if (ret != NET_ERROR_SET::NONE)
		{
			ShutdownClient(SOCKET_CLOSE_CASE::RECV_BUFFER_PROCESS_ERROR, fd, index);
			return false;
		}

		return true;
	}
	NET_ERROR_SET SelectNetwork::RecvData(const int index)
	{
		auto& session = m_ClientsPool[index];
		auto fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
		{
			return NET_ERROR_SET::RECV_PROCESS_NOT_CONNECTED;
		}

		int recvPos = 0;

		if (session.RemainingDataSize > 0)
		{
			memcpy(session.pRecvBuffer, &session.pRecvBuffer[session.PrevReadPosInRecvBuffer], session.RemainingDataSize);
			recvPos += session.RemainingDataSize;
		}

		auto recvSize = recv(fd, &session.pRecvBuffer[recvPos], (MAX_PACKET_BODY_SIZE * 2), 0);

		if (recvSize == 0)
			return NET_ERROR_SET::RECV_REMOTE_CLIENT_CLOSE;
		

		if (recvSize < 0)
		{
			auto error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
				return NET_ERROR_SET::RECV_API_ERROR;
			else
				return NET_ERROR_SET::NONE;
			
		}

		session.RemainingDataSize += recvSize;
		return NET_ERROR_SET::NONE;
	}
	NET_ERROR_SET SelectNetwork::RecvBufferProc(const int sessionIndex)
	{
		auto& session = m_ClientsPool[sessionIndex];

		auto readPos = 0;
		const auto dataSize = session.RemainingDataSize;
		PacketHeader* pPktHeader;

		while ((dataSize - readPos) >= sizeof(PacketHeader))
		{
			pPktHeader = (PacketHeader*)& session.pRecvBuffer[readPos];
			readPos += sizeof(PacketHeader);
			auto bodySize = pPktHeader->TotalSize - sizeof(PacketHeader);

			if (bodySize > 0)
			{
				if (bodySize > (dataSize - readPos))
				{
					readPos -= sizeof(PacketHeader);
					break;
				}

				if (bodySize > MAX_PACKET_BODY_SIZE)		
					return NET_ERROR_SET::RECV_CLIENT_MAX_PACKET;
				
			}

			AddPacketQueue(sessionIndex, pPktHeader->Id, bodySize, &session.pRecvBuffer[readPos]);
			readPos += bodySize;
		}

		session.RemainingDataSize -= readPos;
		session.PrevReadPosInRecvBuffer = readPos;

		return NET_ERROR_SET::NONE;
	}


	void SelectNetwork::checkWrite(const int index, const SOCKET fd, fd_set& w_set)
	{
		if (!FD_ISSET(fd, &w_set))
			return;
		// read와는 반대로 버퍼를 세팅하고 보내주어야 함

		auto retsend = SendBufferSet(index);

		if (retsend != NET_ERROR_SET::NONE)
			ShutdownClient(SOCKET_CLOSE_CASE::SEND_ERROR, fd, index);
		
	}
	NET_ERROR_SET SelectNetwork::SendBufferSet(const int index)
	{
		auto& client = m_ClientsPool[index];
		auto fd = static_cast<SOCKET>(client.SocketFD);

		if (client.IsConnected() == false)
			return NET_ERROR_SET::SEND_BUFF_REMOTE_CLOSE;
		

		// tuple<전송한 바이트,에러 enum>
		auto result = SendData(fd, client.pSendBuffer, client.SendSize);

		if (std::get<1>(result) != NET_ERROR_SET::NONE)
			return  std::get<1>(result);
		

		auto sendSize = std::get<0>(result);

		if (sendSize < client.SendSize)
		{
			memmove(&client.pSendBuffer[0],
				&client.pSendBuffer[sendSize],
				client.SendSize - sendSize);

			client.SendSize -= sendSize;
		}
		else
		{
			client.SendSize = 0;
		}
		return NET_ERROR_SET::NONE;
	}
	std::tuple<int,NET_ERROR_SET>  SelectNetwork::SendData(const SOCKET fd, const char* pMsg, const int size)
	{
		if(size <=0)
			return std::make_tuple(0, NET_ERROR_SET::NONE);


		auto ret = send(fd, pMsg, size, 0);

		if (ret <= 0)
			return std::make_tuple(ret, NET_ERROR_SET::SEND_SIZE_ZERO);
		

		return std::make_tuple(ret, NET_ERROR_SET::NONE);
	}


	void SelectNetwork::ReleaseClientIndex(const int index)
	{
		m_ClientPoolIndex.push_back(index);
		m_ClientsPool[index].Clear();
	}
	void SelectNetwork::ShutdownClient(const SOCKET_CLOSE_CASE closeCase, const SOCKET socket, const int clientIndex)
	{
		if (closeCase == SOCKET_CLOSE_CASE::SESSION_POOL_MAX)
		{
			closesocket(socket);
			FD_CLR(socket, &m_ReadSet);
			return;
		}

		if (m_ClientsPool[clientIndex].IsConnected() == false)
			return;
		
		closesocket(socket);
		FD_CLR(socket, &m_ReadSet);

		m_ClientsPool[clientIndex].Clear();
		--m_IsConnectedCnt;
		ReleaseClientIndex(clientIndex);

		AddPacketQueue(clientIndex, (short)PacketId::SC_SHUT_DOWN_CLIENT, 0, nullptr);
		std::cout << "shut down - ["<< (int)closeCase <<" ]this client get out ->" << clientIndex << std::endl;
	}
	void SelectNetwork::SetSocketOpt(const SOCKET socket)
	{
		// 공부 해야함
		linger ling;
		ling.l_onoff = 0;
		ling.l_linger = 0;
		setsockopt(socket, SOL_SOCKET, SO_LINGER, (char*)& ling, sizeof(ling));


		// 소켓 버퍼 크기 늘림
		int size1 = MaxClientSockOptRecvBufSize;
		int size2 = MaxClientSockOptSendBufSize;
		setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)& size1, sizeof(size1));
		setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)& size2, sizeof(size2));
	}


	NET_ERROR_SET SelectNetwork::SendInLogic(const int idx, const short packetId, const short bodySize, const char* pMsg)
	{
		auto& session = m_ClientsPool[idx];

		auto pos = session.SendSize;
		auto totalSize = bodySize + sizeof(PacketHeader);

		if ((pos + totalSize) > MaxClientSendBufSize) 
			return NET_ERROR_SET::CLIENT_SEND_BUFFER_FULL;
		

		PacketHeader pktHeader{ totalSize, packetId};
		memcpy(&session.pSendBuffer[pos], (char*)& pktHeader, sizeof(PacketHeader));

		if (bodySize > 0)
			memcpy(&session.pSendBuffer[pos + sizeof(PacketHeader)], pMsg, bodySize);
		

		session.SendSize += totalSize;

		return NET_ERROR_SET::NONE;
	}


}