#include "preCompile.h"

Connection::Connection() {}
Connection::~Connection() {}

void Connection::InitConnection()
{
	cConnCnt = 0;


}

int Connection::getConnectionCount() {
	
	return cConnCnt;
}