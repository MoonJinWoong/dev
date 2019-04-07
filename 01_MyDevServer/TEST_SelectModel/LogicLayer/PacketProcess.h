#pragma once

#include <memory>
#include "Packet.h"
#include "ErrorSet.h"
#include "../NetworkLayer/Defines.h"

using ERROR_SET = LogicCommon::ERROR_SET;

namespace NetworkLib
{
	class NetworkFrame;
}

namespace LogicLib
{
	class ConnectedPlayerManager;
	class PlayerManager;
	class LobbyManager;

	using ServerConfig = NetworkLib::ServerConfig;


#define CHECK_START  ERROR_SET __result=ERROR_SET::NONE;
#define CHECK_ERROR(f) __result=f; goto CHECK_ERR;


	class PacketProcess
	{
		using PacketInfo = NetworkLib::RecvPacketInfo;
		typedef ERROR_SET(PacketProcess::*PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)LogicCommon::PACKET_ID::MAX];

		using TcpNet = NetworkLib::NetworkFrame;


	public:

		PacketProcess();
		~PacketProcess();
		void Init(TcpNet* pNetwork, PlayerManager* pUserMgr
			, LobbyManager* pLobbyMgr, ServerConfig* pConfig);
	private:

		TcpNet* m_pRefNetwork;
	    PlayerManager* m_pRefUserMgr;
		LobbyManager* m_pRefLobbyMgr;

		std::unique_ptr<ConnectedPlayerManager> m_pConnectedPlayerManager;


	private:
		ERROR_SET NtfSysConnctSession(PacketInfo packetInfo);
		ERROR_SET NtfSysCloseSession(PacketInfo packetInfo);

		ERROR_SET Login(PacketInfo packetInfo);

		ERROR_SET LobbyList(PacketInfo packetInfo);

		ERROR_SET LobbyEnter(PacketInfo packetInfo);

		ERROR_SET LobbyLeave(PacketInfo packetInfo);

		ERROR_SET RoomEnter(PacketInfo packetInfo);

		ERROR_SET RoomLeave(PacketInfo packetInfo);

		ERROR_SET RoomChat(PacketInfo packetInfo);

		ERROR_SET RoomMasterGameStart(PacketInfo packetInfo);

		ERROR_SET RoomGameStart(PacketInfo packetInfo);

		ERROR_SET DevEcho(PacketInfo packetInfo);
	protected:

	};
}