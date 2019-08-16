#pragma once 
#include "PreComplie.h"

// ���� �������̽� �ۼ�
// 0. �ɼǵ� ���� 
// 1. Ŭ�� ���� �޾Ƽ� ���� ����, ����
// 2. �������� ���� ��Ŷ �޾Ƽ� ó���ϰ� �ٽ� ���� 

typedef enum SERVER_STATUS
{
	STOP,INIT,READY
};


class NetBase
{
public:
	// ��� ��������� �����ض� 
	virtual bool Run() = 0;

	// con , dest
	NetBase();
	virtual ~NetBase();

	virtual void InitBase();
	void PutPackage();
	SERVER_STATUS& GetServerStatus();

protected:
	char				mIP[16];
	int					mPort;
	int					mWorkerThreadCnt;
	SERVER_STATUS		mServerStatus;
	// ContentsProc      *mContent;
};
