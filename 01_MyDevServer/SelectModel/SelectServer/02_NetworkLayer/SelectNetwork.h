#pragma once
#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <deque>
#include <unordered_map>
#include <iostream>


#include "Defines.h"
#include "NetErrSet.h"


namespace NetworkLib
{
	//struct SocketInfo
	//{
	//	SOCKET socket;
	//	bool isConnected= false;
	//	char buf[1024];
	//	int recvByte;
	//	int sendByte;
	//};

	class SelectNetwork
	{
	public:
		SelectNetwork();
		~SelectNetwork();
		NET_ERR_SET Init();
		NET_ERR_SET InitSocket();
		NET_ERR_SET BindListen(short port, int backlogCount);
		void Run();
		NET_ERR_SET JoinClient();
		
		int CreateSessionPool(int maxClientCount);
		int AllocClientSessionIndex();
		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);
		void ReleaseSessionIndex(const int index);
		void AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);
		void SetSockOption(const SOCKET fd);
		void ConnectedSession(const int sessionIndex, const SOCKET fd, const char* pIP);
		void RunCheckSelectClients(fd_set& read_set, fd_set& write_set);
		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);
		void RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set);

		NET_ERR_SET RecvData(const int sessionIndex);
		NET_ERR_SET RecvBufferProcess(const int sessionIndex);
		NetError SendData(const SOCKET fd, const char* pMsg, const int size);
		NetError BroadcastingBuffer(const int sessionIndex);

	private:
		SOCKET m_ServerSockfd;

		FD_SET m_readSet, m_writeSet;
		SOCKET m_ClientSocketfd;

		int m_nSocketIdx;   // 소켓 개수도 가지고 있자

	protected:
		int64_t m_ConnectSeq = 0;
		ServerConfig *m_Config = new ServerConfig;
		size_t m_ConnectedSessionCnt = 0;
		std::vector<ClientSession> m_ClientSessionPool;
		std::deque<int> m_ClientSessionPoolIndex;
		std::deque<RecvPacketInfo> m_PacketQueue;


	};
}