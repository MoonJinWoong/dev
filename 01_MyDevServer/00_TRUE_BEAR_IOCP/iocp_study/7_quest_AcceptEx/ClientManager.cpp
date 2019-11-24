#include "ClientManager.h"

void ClientManager::Init(c_u_Int maxCnt)
{
	maxClientCnt = maxCnt;
	mClientPool = std::vector<Client*>(maxClientCnt);

	for (auto i = 0; i < maxClientCnt; i++)
	{
		mClientPool[i] = new Client();
		mClientPool[i]->Init(i);
	}
}

void ClientManager::Add(std::string nickName, int unique_id)
{
	//TODO ���� ó��, �Ķ���� ���� ����
	mDicClientId.insert(std::pair<std::string,int>(nickName,unique_id));
	mClientPool[unique_id]->SetLogin(nickName);
}

unsigned int ClientManager::FindUniqueId(std::string userID_)
{
	//TODO ���� ó��, �Ķ���� ���� ����
	for (auto iter = mDicClientId.find(userID_); iter != mDicClientId.end(); ++iter)
	{
		return (*iter).second;
	}
	std::cout << "FindUniqueId() fail" << std::endl;
	return -1;
}


void ClientManager::Delete(Client* client)
{
	//TODO ���� ó��
	mDicClientId.erase(client->GetUserId());
	client->Reset();
}
