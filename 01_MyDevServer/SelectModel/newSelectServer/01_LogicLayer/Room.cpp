#include <algorithm>

#include "../02_NetworkLayer/SelectNetwork.h"
#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"

#include "Client.h"
//#include "Game.h"
#include "Room.h"


namespace LogicLayer
{
	Room::Room() {}
	Room::~Room() {}
	void Room::Init(const short index, const short maxClient)
	{
		m_Index = index;
		m_MaxClient = maxClient;
	}
}