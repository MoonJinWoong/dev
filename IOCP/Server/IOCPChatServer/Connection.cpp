#include "preCompile.h"

Connection::Connection() {}
Connection::~Connection() {}

void Connection::InitConnection()
{
	cConnCnt = 0;
	cisConnected = false;

}

int Connection::getConnectionCount() {
	
	return cConnCnt;
}

bool Connection::getIsConnected()
{
	return cisConnected;
}

void Connection::setIsConnected(bool conn)
{
	this->cisConnected = conn;
}