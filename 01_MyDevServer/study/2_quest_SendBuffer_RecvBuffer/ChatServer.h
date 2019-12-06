#pragma once
#include "NetService.h"
#include "PacketMgr.h"
#include "Packet.h"


class ChatServer : public NetService
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	virtual void OnAccept(const unsigned int unique_id) override;
	virtual void OnClose(const unsigned int unique_id) override;
	virtual void OnRecv(const unsigned int unique_id, const unsigned int size_, char* pData_) override;

	void Run(unsigned int maxClient);

private:
	std::unique_ptr<PacketMgr> mPktMgr;
};