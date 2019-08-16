
#include "../NetLib/IocpServer.h"



int main()
{
	IocpServer iocp;
	
	if (!iocp.Run())
	{
		std::cout << "Run end " << std::endl;
	}
	return 0;
}

