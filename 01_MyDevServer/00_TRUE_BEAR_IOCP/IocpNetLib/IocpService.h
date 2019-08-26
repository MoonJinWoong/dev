#pragma once

class NetworkService;

class IocpService
{
public:
	IocpService(int threadCnt);
	~IocpService();

	void resister(NetworkService& sock);

	HANDLE mIocpHanle;

	int mThreadCnt;
};


