#pragma once

#include "NetService.h"
#include "LogicMain.h"
#include "Packet.h"


class ChatServer : public NetService
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	virtual void OnAccept(unsigned int unique_id) override;
	virtual void OnClose(unsigned int unique_id) override;
	virtual void OnRecv(CustomOverEx *pOver, unsigned int size_) override;

	void Run(unsigned int maxClient);
	void Stop();
private:
	std::unique_ptr<LogicMain> mLogicProc;
};