// 01_OverlappedCallback.cpp : �ܼ� ���� ���α׷��� ���� �������� ����
//


#include "stdafx.h"

#include "OverlappedCallback.h"

const int PORT = 32452;


int main()
{
	OverlappedCallback overlapped;
	overlapped.InitSocket();

	// socket�� server �ּҸ� �����ϰ� ��Ͻ�Ų��.
	overlapped.BindandListen(PORT);
	overlapped.StartServer();

	getchar();
	overlapped.DestroyThread();
	return 0;
}
