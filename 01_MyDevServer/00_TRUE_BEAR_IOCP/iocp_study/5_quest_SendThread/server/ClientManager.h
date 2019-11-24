#pragma once
#include "TypeDefine.h"
#include "Client.h"
#include <vector>
#include <unordered_map>
#include <iostream>

class ClientManager
{
public:
	ClientManager() = default;
	~ClientManager() = default;

	void Init(u_Int maxCnt);

	void Add(std::string nickName, int unique_id);

	unsigned int FindUniqueId(std::string nickName);

	void Delete(Client* client);

	Client* GetClient(u_Int unique_id) { return mClientPool[unique_id]; }

private:
	std::vector<Client*> mClientPool;
	std::unordered_map<std::string, int> mDicClientId;
	u_Int maxClientCnt = 0;

};