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
	//TODO 예외 처리, 파라미터 복사 연산
	mDicClientId.insert(std::pair<std::string,int>(nickName,unique_id));
	mClientPool[unique_id]->SetLogin(nickName);
}

unsigned int ClientManager::FindUniqueId(std::string userID_)
{
	//TODO 예외 처리, 파라미터 복사 연산
	for (auto iter = mDicClientId.find(userID_); iter != mDicClientId.end(); ++iter)
	{
		return (*iter).second;
	}
	std::cout << "FindUniqueId() fail" << std::endl;
	return -1;
}


void ClientManager::Delete(Client* client)
{
	//TODO 예외 처리
	mDicClientId.erase(client->GetUserId());
	client->Reset();
}
