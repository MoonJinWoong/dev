#pragma once

#include "../NetLib/NetService.h"
#include "LogicMain.h"
#include "../NetLib/Packet.h"


class ChatServer : public NetService
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	virtual void ThrowLogicConnection(unsigned int unique_id, PACKET_TYPE type) override;
	virtual void ThrowLogicRecv(CustomOverEx *pOver, unsigned int size_) override;

	void Run();
	void Stop();
private:
	std::unique_ptr<LogicMain> mLogicProc;
};