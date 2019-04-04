#include <algorithm>

#include "../NetworkLayer/SelectNetwork.h"
#include "Packet.h"
#include "ErrorSet.h"


#include "Player.h"
#include "Game.h"
#include "Room.h"

using PACKET_ID = NetworkCommon::PACKET_ID;

namespace LogicLib
{
	Room::Room() {}

	Room::~Room()
	{
		if (m_pGame != nullptr) {
			delete m_pGame;
		}
	}

	void Room::Init(const short index, const short maxUserCount)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;

		m_pGame = new Game;
	}

	void Room::SetNetwork(TcpNet* pNetwork)
	{
		//m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;
	}

	void Room::Clear()
	{
		m_IsUsed = false;
		m_Title = L"";
		m_UserList.clear();
	}

	Game* Room::GetGameObj()
	{
		return m_pGame;
	}

	ERROR_SET Room::CreateRoom(const wchar_t* pRoomTitle)
	{
		if (m_IsUsed) {
			return ERROR_SET::ROOM_ENTER_CREATE_FAIL;
		}

		m_IsUsed = true;
		m_Title = pRoomTitle;

		return ERROR_SET::NONE;
	}

	ERROR_SET Room::EnterUser(Player* pPlayer)
	{
		if (m_IsUsed == false) {
			return ERROR_SET::ROOM_ENTER_NOT_CREATED;
		}

		if (m_UserList.size() == m_MaxUserCount) {
			return ERROR_SET::ROOM_ENTER_MEMBER_FULL;
		}

		m_UserList.push_back(pPlayer);
		return ERROR_SET::NONE;
	}

	bool Room::IsMaster(const short userIndex)
	{
		return m_UserList[0]->GetIndex() == userIndex ? true : false;
	}

	ERROR_SET Room::LeaveUser(const short userIndex)
	{
		if (m_IsUsed == false) {
			return ERROR_SET::ROOM_ENTER_NOT_CREATED;
		}

		auto iter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [userIndex](auto pUser) { return pUser->GetIndex() == userIndex; });
		if (iter == std::end(m_UserList)) {
			return ERROR_SET::ROOM_LEAVE_NOT_MEMBER;
		}

		m_UserList.erase(iter);

		if (m_UserList.empty())
		{
			Clear();
		}

		return ERROR_SET::NONE;
	}

	void Room::SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex)
	{
		for (auto pUser : m_UserList)
		{
			if (pUser->GetIndex() == passUserindex) {
				continue;
			}

			m_pRefNetwork->SendData(pUser->GetSessioIndex(), packetId, dataSize, pData);
		}
	}

	void Room::NotifyEnterUserInfo(const int userIndex, const char* pszUserID)
	{
		NetworkCommon::PktRoomEnterUserInfoNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, NetworkCommon::MAX_USER_ID_SIZE);

		SendToAllUser((short)PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(pkt), (char*)&pkt, userIndex);
	}

	void Room::NotifyLeaveUserInfo(const char* pszUserID)
	{
		if (m_IsUsed == false) {
			return;
		}

		NetworkCommon::PktRoomLeaveUserInfoNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, NetworkCommon::MAX_USER_ID_SIZE);

		SendToAllUser((short)PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt);
	}

	void Room::NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg)
	{
		NetworkCommon::PktRoomChatNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, NetworkCommon::MAX_USER_ID_SIZE);
		wcsncpy_s(pkt.Msg, NetworkCommon::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, NetworkCommon::MAX_ROOM_CHAT_MSG_SIZE);

		SendToAllUser((short)PACKET_ID::ROOM_CHAT_NTF, sizeof(pkt), (char*)&pkt, sessionIndex);
	}

	void Room::Update()
	{
		if (m_pGame->GetState() == GameState::ING)
		{
			if (m_pGame->CheckSelectTime())
			{
				//선택 안하는 사람이 지도록 한
			}
		}
	}
}