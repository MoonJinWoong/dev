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
	// �� �ɾ����


	// lib���� ������ connectionManager ��ü ���� �Ҵ�
	cConnection = new ConnectionManager[dwMaxConnection];

	for (int i = 0; i < (int)dwMaxConnection; i++)
	{
		InitConfig.sIndex = i;
		if (cConnection[i].CreateConnection(InitConfig) == false)
			return false;

	}
	// �� ���� �ؾ���



	return true;
}

bool Connection::AddConnection(ConnectionManager* pConnection)
{
	// �� �ɾ����

	auto iter = cConnStructure.find(pConnection);

	//�̹� ���ӵǾ� �ִ� �����̶��
	if (iter != cConnStructure.end())
	{
		cout << "already connected .... !!! " << endl;
		return false;
	}
	cConnStructure.insert(make_pair(pConnection,GetTickCount()));


	// ��� �������
	return true;
}

bool Connection::RemoveConnection(ConnectionManager* pConnection)
{
	// �� �������


	auto iter = cConnStructure.find(pConnection);

	//���ӵǾ� �ִ� ������ ���°��
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
	// ���ɾ�����


	//iterator�� ���鼭 ���ӵǾ��ִ� ��� �ϴ� �޼��� �����־����.
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

	//������
}

