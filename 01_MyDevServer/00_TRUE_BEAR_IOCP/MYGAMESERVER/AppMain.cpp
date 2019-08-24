#include "..//NetLib/NetTool.h"
#include "../NetLib/IocpServer.h"



int main()
{
	SYSTEM_INFO system;
	GetSystemInfo(&system);
	auto threadCnt = system.dwNumberOfProcessors * 2;  // ms ������� ���Ŀ� �ٲ㰡�鼭 �غ� ��

	// iocp Ŭ���� ����
	IocpServer Iocp(threadCnt);
		
	// ��Ʈ��ũ ����� iocp�� �и��ߴ�.epll���� ����� ��
	NetTool netTool(SocketType::TCP);
	netTool.bindAndListen("127.0.0.1", 9000);

	// IOCP ���
	Iocp.Resister(netTool, nullptr);
		


	return 0;
}

