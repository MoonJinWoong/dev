#pragma once

#include "Client.h"
#include <vector>
#include <unordered_map>
#include <iostream>

class ClientManager
{
public:
	ClientManager() = default;
	~ClientManager() = default;

	void Init(unsigned int maxCnt);

	void Add(std::string nickName, int unique_id);
	int FindID(std::string nickName);
	void Delete(Client* client);


	void IncreaseCurClientCnt() { ++mCurrCnt; }
	void DecreaseCurClientCnt() { --mCurrCnt; if (mCurrCnt < 0)  mCurrCnt = 0; }

	int GetCurClientCnt() { return mCurrCnt; }
	int GetMaxClientCnt() { return mMaxCnt; }
	Client* GetClient(unsigned int unique_id) { return mClientPool[unique_id]; }

private:
	std::vector<Client*> mClientPool;
	std::unordered_map<std::string, int> mDicClientId;

	int mMaxCnt = 0;
	int mCurrCnt = 0;

};