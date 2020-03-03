#pragma once
#define FD_SETSIZE 5096
#include <vector>
#include <deque>
#include <tuple>

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "../../PacketDefine/Packet.h"		 // ��Ŷ ������ ���� 
#include "Define.h"							// Ŭ�� ���� ����ü ��� ���� 
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
		size_t m_IsConnectedCnt = 0;   // ���� �Ǿ��ִ� Ŭ�� �� 


		std::vector<ClientsInfo> m_ClientsPool;  // Ŭ���̾�Ʈ�� �����ϴ� Ǯ
		std::deque<int> m_ClientPoolIndex;       // Ŭ�� �ĺ� ��ȣ
		std::deque<RecvPacket> m_PacketQueue;    // ��Ŷó���� �����̳�
	};
}