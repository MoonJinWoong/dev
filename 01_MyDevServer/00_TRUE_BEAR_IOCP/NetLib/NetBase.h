#pragma once 
#include "PreComplie.h"

// 서버 인터페이스 작성
// 0. 옵션들 설정 
// 1. 클라 접속 받아서 세션 생성, 관리
// 2. 세션으로 오는 패킷 받아서 처리하고 다시 전송 

typedef enum SERVER_STATUS
{
	STOP,INIT,READY
};


class NetBase
{
public:
	// 상속 받은놈들이 구현해라 
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
