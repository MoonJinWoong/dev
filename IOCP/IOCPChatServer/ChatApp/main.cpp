
#include "../IOCPChatServer/preCompile.h"
#pragma comment(lib,"NetLibrary.lib")





int main()
{

	IocpServer* obj = new IocpServer();
	test* t = new test();
	t->run();

	return 0;
}
