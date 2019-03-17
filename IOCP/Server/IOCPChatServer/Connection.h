#pragma once



class Connection
{
private:
	int cConnCnt;
public:
	Connection();
	~Connection();
	void InitConnection();
	int getConnectionCount();
};