#include "ServerOpt.h"
#include "preCompile.h"
#include "Session.h"


namespace NetworkLayer
{
	Session::Session() {}
	Session::~Session() {}
	bool Session::ProcAccept(SOCKET sock, SOCKADDR_IN addrLen)
	{
		return true;
	}
}