#pragma once
#define FD_SETSIZE 5096
#include <vector>
#include <deque>

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "../../PacketDefine/Packet.h"   // ��Ŷ ������ ���� 
//#include "PacketInfo.h"
#include "Define.h"     // Ŭ�� ���� ����ü ��� ���� 
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
		size_t m_IsConnectedCnt = 0;   // ���� �Ǿ��ִ� Ŭ�� �� 


		std::vector<ClientsInfo> m_ClientsPool;  // Ŭ���̾�Ʈ�� �����ϴ� Ǯ
		std::deque<int> m_ClientPoolIndex;       // Ŭ�� �ĺ� ��ȣ
		std::deque<RecvPacket> m_PacketQueue;    // ��Ŷó���� �����̳�
	};
}