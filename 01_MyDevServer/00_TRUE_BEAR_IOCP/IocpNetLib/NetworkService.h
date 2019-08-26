#pragma once



class NetworkService 
{
public:
	NetworkService();
	~NetworkService();

	void bindAndListen( int port);

	void initAccpetEx();

public:
	LPFN_ACCEPTEX mFnAcceptEx = nullptr;

	SOCKET mSock;
	WSAOVERLAPPED m_readOverlappedStruct;

private:
};