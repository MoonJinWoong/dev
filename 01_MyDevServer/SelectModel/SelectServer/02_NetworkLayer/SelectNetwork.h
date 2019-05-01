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
#include "ErrorSet.h"



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
		void			InitNetwork();
		bool			BindAndListen();
		void			CreateClientPool(const int &maxClient);
		void			Run();
		NET_ERROR_SET	AcceptNewClient();
		int				AllocClientIndex();
		void			ReleaseClientIndex(const int index);
		void			ShutdownClient(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);
		
		void			SetSocketOpt(const SOCKET socket);

		void			ConnectedClient(const int index, const SOCKET socket
						, const char* IP);

		void    CheckClients(fd_set& read_set, fd_set& write_set);
		bool    checkReadSet(const int sessionIndex,
			    const SOCKET fd, fd_set& read_set);


		void    checkWriteSet(const int sessionIndex, const SOCKET fd,
			    fd_set& write_set);
		NET_ERROR_SET    RecvData(const int sessionIndex);
		NET_ERROR_SET    RecvBufferProcess(const int sessionIndex);

		NET_ERROR_SET    SendData(const int sessionIndex);

		NET_ERROR_SET 	 LogicSendBufferSet(const int sessionIndex, const short packetID
						 ,const short bodysize, const char* msg);
	

		void    AddPacketQueue(const int sessionIndex
			    ,const short pktId, const short bodySize, char* pDataPos);

		RecvPacket GetPacketInfo();

	private:

		SOCKET m_ServerSocket;
		int64_t m_ConnectSeq = 0;
		SOCKADDR_IN clientaddr;
		fd_set m_ReadSet;
		size_t m_IsConnectedCnt = 0;   // 접속 되어있는 클라 수 


		std::vector<ClientsInfo> m_ClientsPool;  // 클라이언트들 관리하는 풀
		std::deque<int> m_ClientPoolIndex;       // 클라 식별 번호
		std::deque<RecvPacket> m_PacketQueue;    // 패킷처리용 컨테이너
	};
}