#include "Connection.h"
#include "PacketProtocol.h"


Connection::Connection()
{
	cConnection = nullptr;
}
Connection::~Connection()
{
	if (cConnection != nullptr)
		delete[] cConnection;
}

bool Connection::CreateConnection(INITCONFIG &InitConfig, DWORD dwMaxConnection)
{
	// 락 걸어야함


	// lib에서 가져온 connectionManager 객체 동적 할당
	cConnection = new ConnectionManager[dwMaxConnection];

	for (int i = 0; i < (int)dwMaxConnection; i++)
	{
		InitConfig.sIndex = i;
		if (cConnection[i].CreateConnection(InitConfig) == false)
			return false;

	}
	// 락 해제 해야함



	return true;
}

bool Connection::AddConnection(ConnectionManager* pConnection)
{
	// 락 걸어야함

	auto iter = cConnStructure.find(pConnection);

	//이미 접속되어 있는 연결이라면
	if (iter != cConnStructure.end())
	{
		cout << "already connected .... !!! " << endl;
		return false;
	}
	cConnStructure.insert(make_pair(pConnection,GetTickCount()));


	// 언락 해줘야함
	return true;
}

bool Connection::RemoveConnection(ConnectionManager* pConnection)
{
	// 락 해줘야함


	auto iter = cConnStructure.find(pConnection);

	//접속되어 있는 연결이 없는경우
	if (iter == cConnStructure.end())
	{
		cout << "connected device not existing.... !!! " << endl;
		return false;
	}
	cConnStructure.erase(pConnection);
	return true;
}

void Connection::BroadCast_Chat(char* IP, char* ChatMsg)
{
	// 락걸어주자


	//iterator로 돌면서 접속되어있는 놈들 싹다 메세지 날려주어야함.
	for (auto iter = cConnStructure.begin(); iter != cConnStructure.end(); iter++)
	{

		auto pConnection = (ConnectionManager*)iter->first;
		auto pChat = (PacketChat*)pConnection->PrepareSendPacket(sizeof(PacketChat));

		if (NULL == pChat)
			continue;
		pChat->sType = PACKET_CHAT;
		strncpy(pChat->sChatMsg, ChatMsg, MAX_CHATMSG);
		strncpy(pChat->sIP, IP, MAX_IP);
		pConnection->SendPost(pChat->sLength);
	}

	//락해제
}

