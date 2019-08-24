#include "..//NetLib/NetTool.h"
#include "../NetLib/IocpServer.h"



int main()
{
	SYSTEM_INFO system;
	GetSystemInfo(&system);
	auto threadCnt = system.dwNumberOfProcessors * 2;  // ms 권장사항 추후에 바꿔가면서 해볼 것

	// iocp 클래스 생성
	IocpServer Iocp(threadCnt);
		
	// 네트워크 기능을 iocp와 분리했다.epll에도 써먹을 것
	NetTool netTool(SocketType::TCP);
	netTool.bindAndListen("127.0.0.1", 9000);

	// IOCP 등록
	Iocp.Resister(netTool, nullptr);
		


	return 0;
}

