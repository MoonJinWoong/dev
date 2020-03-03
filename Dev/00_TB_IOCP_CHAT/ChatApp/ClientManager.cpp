#include "ClientManager.h"

void ClientManager::Init()
{
	mMaxCnt = Config::LoadNetConfig(CATEGORY_LOGIC, MAX_CLIENT_COUNT);
	mClientPool = std::vector<Client*>(mMaxCnt);

	for (auto i = 0; i < mMaxCnt; i++)
	{
		mClientPool[i] = new Client();
		mClientPool[i]->Init(i);
	}
}

void ClientManager::Add(std::string nickName, int unique_id)
{
	//TODO ���� ó��, �Ķ���� ���� ����
	mDicClientId.insert(std::pair<std::string,int>(nickName,unique_id));
	mClientPool[unique_id]->SetLogin(nickName , unique_id);
}


void ClientManager::Delete(Client* client)
{
	//TODO ���� ó��
	mDicClientId.erase(client->GetUserId());
	client->Reset();
}

bool ClientManager::FindID(std::string nickName)
{
	if (mDicClientId.find(nickName) != mDicClientId.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}
