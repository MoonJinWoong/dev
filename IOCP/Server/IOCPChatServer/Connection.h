#pragma once



class Connection
{
private:
	int cConnCnt;
	bool cisConnected;
public:
	Connection();
	~Connection();
	void InitConnection();
	int getConnectionCount();
	bool getIsConnected();
	void setIsConnected(bool conn);

public:
	//Overlapped I/O ��û�� ���� ����
	EX_OVERLAPPED		cOverlappedEx;
	SOCKET				cSocket;
};