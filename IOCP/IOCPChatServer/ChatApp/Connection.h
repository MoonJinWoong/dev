#pragma once
#include "../IOCPChatServer/preCompile.h"
#pragma comment(lib,"NetLibrary.lib")

class Connection 
{

public:
	Connection(void);
	~Connection(void);

	map< ConnectionManager*, DWORD >   cConnStructure;	

	bool CreateConnection(INITCONFIG &InitConfig, DWORD dwMaxConnection);
	bool AddConnection(ConnectionManager* pConnection);
	bool RemoveConnection(ConnectionManager* pConnection);

	void BroadCast_Chat(char* IP, char* szChatMsg);

protected:
	ConnectionManager*	cConnection;
};