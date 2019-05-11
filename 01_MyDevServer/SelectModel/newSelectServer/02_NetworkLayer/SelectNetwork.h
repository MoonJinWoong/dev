#pragma once
#define FD_SETSIZE 5096
#include <vector>
#include <deque>
#include <tuple>

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "../../PacketDefine/Packet.h"		 // 패킷 정보들 모음 
#include "Define.h"							// 클라 정보 구조체 담겨 있음 
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

		void    AddPacketQueue(const int sessionIndex
			    ,const short pktId, const short bodySize, char* pDataPos);

		RecvPacket GetPacketInfo();


		void checkClient(fd_set& read_set, fd_set& write_set);
	

		// read recv 
		bool checkRead(const int index, const SOCKET fd, fd_set& r_set);
		NET_ERROR_SET RecvData(const int index);
		NET_ERROR_SET RecvBufferProc(const int sessionIndex);

		// write
		void checkWrite(const int index, const SOCKET fd, fd_set& w_set);
		NET_ERROR_SET SendBufferSet(const int index);
		std::tuple<int, NET_ERROR_SET> SendData(const SOCKET fd, const char* pMsg, const int size);


		// logic send 
		NET_ERROR_SET SendInLogic(const int idx, const short packetId, const short bodySize, const char* pMsg);

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