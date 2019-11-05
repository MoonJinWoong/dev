#pragma once


#include "NetService.h"

class ChatServer : public NetService
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;


	virtual void OnConnect(const unsigned int index) override;
	virtual void OnClose(const unsigned int index) override;
	virtual void OnRecv(const unsigned int index, const unsigned int size_, char* pData_) override;

	void Run(unsigned int maxClient);


private:
	//std::unique_ptr<PacketManager> m_pPacketManager;
};