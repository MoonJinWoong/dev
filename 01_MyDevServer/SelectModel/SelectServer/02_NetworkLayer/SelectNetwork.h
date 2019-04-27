#pragma once
#define FD_SETSIZE 5096
#include <vector>
#include <deque>


#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "../../PacketDefine/Packet.h"   // 패킷 정보들 모음 
//#include "PacketInfo.h"
#include "Define.h"     // 클라 정보 구조체 담겨 있음 


namespace NetworkLayer
{
	class SelectNetwork
	{
		using RecvPacket	= PacketLayer::RecvPacketInfo;
		using PacketHeader  = PacketLayer::PktHeader;
		using PacketId		= PacketLayer::PACKET_ID;

	public:
		SelectNetwork();
		~SelectNetwork();
		void	InitNetwork();
		bool	BindAndListen();
		void	CreateClientPool(const int &maxClient);
		void	Run();
		bool	AcceptNewClient();
		int		AllocClientIndex();
		void	RejectClient(SOCKET socket  , const int sessionIndex);
		void	ConnectedClient(const int client_index
			    , const SOCKET whoSocket, const char* pIP);
		void    CheckClients(fd_set& read_set, fd_set& write_set);
		bool    ProcessRecv(const int sessionIndex,
			    const SOCKET fd, fd_set& read_set);
		void    ProcessSend(const int sessionIndex, const SOCKET fd,
			    fd_set& write_set);
		bool    RecvData(const int sessionIndex);
		bool    RecvBufferProcess(const int sessionIndex);

		bool    SendData(const SOCKET fd, const char* pMsg, const int size);
		bool    SettingSendBuff(const int sessionIndex);


	

		void    AddPacketQueue(const int sessionIndex
			    ,const short pktId, const short bodySize, char* pDataPos);

		RecvPacket GetPacketInfo();

	private:

		SOCKET m_ServerSocket;
		int64_t m_ConnectSeq = 0;
		SOCKADDR_IN clientaddr;
		fd_set m_ReadSet;
		size_t m_ConnectedSessionCount = 0;
		std::vector<ClientsInfo> m_ClientsPool;
		std::deque<int> m_ClientPoolIndex;
		std::deque<RecvPacket> m_PacketQueue;
	};
}