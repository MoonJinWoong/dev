// 01_OverlappedCallback.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의
//


#include "stdafx.h"

#include "OverlappedCallback.h"

const int PORT = 32452;


int main()
{
	OverlappedCallback overlapped;
	overlapped.InitSocket();

	// socket과 server 주소를 연결하고 등록시킨다.
	overlapped.BindandListen(PORT);
	overlapped.StartServer();

	getchar();
	overlapped.DestroyThread();
	return 0;
}
