#include "ClientManager.h"

void ClientManager::Init(unsigned int maxCnt)
{
	mMaxCnt = maxCnt;
	mClientPool = std::vector<Client*>(mMaxCnt);

	for (auto i = 0; i < mMaxCnt; i++)
	{
		mClientPool[i] = new Client();
		mClientPool[i]->Init(i);
	}
}

void ClientManager::Add(std::string nickName, int unique_id)
{
	//TODO 예외 처리, 파라미터 복사 연산
	mDicClientId.insert(std::pair<std::string,int>(nickName,unique_id));
	mClientPool[unique_id]->SetLogin(nickName , unique_id);
}


void ClientManager::Delete(Client* client)
{
	//TODO 예외 처리
	mDicClientId.erase(client->GetUserId());
	client->Reset();
}

int ClientManager::FindID(std::string nickName)
{
	if (mDicClientId.find(nickName) != mDicClientId.end())
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
