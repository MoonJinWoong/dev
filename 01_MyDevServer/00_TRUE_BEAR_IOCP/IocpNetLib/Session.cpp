#include "Session.h"

const int CLIENT_BUFSIZE = 65536;


Session::Session() : Session(CLIENT_BUFSIZE, CLIENT_BUFSIZE)
{
	memset(&mClientAddr, 0, sizeof(SOCKADDR_IN));
}

Session::~Session()
{
}

void Session::SessionReset()
{

}

bool Session::PostAccept()
{


	return true;
}

bool Session::AcceptCompletion()
{
	
	return true;
}

void Session::OnDisconnect()
{
	
}

void Session::OnRelease()
{
}

