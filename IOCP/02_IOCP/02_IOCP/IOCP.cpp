// �ܼ� ���� ���α׷��� ���� �������� ����


#include "stdafx.h"
#include "IOCompletionPort.h"

int main()
{
	IOCompletionPort ioCompletionPort;

	// ������ �ʱ�ȭ
	ioCompletionPort.InitSocket();

	// ���ϰ� �����ּҸ� �����ϰ� ��Ͻ�Ų��. 
	ioCompletionPort.BindandListen(32452);

	ioCompletionPort.StartServer();

	printf("�ƹ�Ű�� ���� ������ ���.....\n");
	getchar();
	ioCompletionPort.DestroyThread();
	return 0;
}