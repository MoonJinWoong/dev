#pragma once




class DLLTEST_API IocpServer
{
public:
	IocpServer(void);
	virtual ~IocpServer(void);

	static IocpServer* m_pIocpServer;
};