#include "Packet.h"
#include "ErrorSet.h"
#include "../NetworkLayer/SelectNetwork.h"
#include "ConnectedPlayerManager.h"
#include "Player.h"
#include "PlayerManager.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using PACKET_ID = LogicCommon::PACKET_ID;

namespace LogicLib
{
	ERROR_SET PacketProcess::Login(PacketInfo packetInfo)
	{
		CHECK_START
			//TODO: 받은 데이터가 PktLogInReq 크기만큼인지 조사해야 한다.
			// 패스워드는 무조건 pass 해준다.
			// ID 중복이라면 에러 처리한다.

			LogicCommon::PktLogInRes resPkt;
		auto reqPkt = (LogicCommon::PktLogInReq*)packetInfo.pRefData;

		auto addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);

		if (addRet != ERROR_SET::NONE) {
			CHECK_ERROR(addRet);
		}

		m_pConnectedPlayerManager->SetLogin(packetInfo.SessionIndex);

		resPkt.ErrorCode = (short)addRet;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(LogicCommon::PktLogInRes), (char*)&resPkt);

		return ERROR_SET::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(LogicCommon::PktLogInRes), (char*)&resPkt);
		return (ERROR_SET)__result;
	}

	ERROR_SET PacketProcess::LobbyList(PacketInfo packetInfo)
	{
		CHECK_START
			// 인증 받은 유저인가?
			// 아직 로비에 들어가지 않은 유저인가?

			auto pUserRet = m_pRefUserMgr->GetPlayer(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_SET::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainInLogIn() == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LIST_INVALID_DOMAIN);
		}

		m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);

		return ERROR_SET::NONE;

	CHECK_ERR:
		LogicCommon::PktLobbyListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_SET)__result;
	}
}