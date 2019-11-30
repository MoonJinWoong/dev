#pragma once
#include "TypeDefine.h"
#include "NetService.h"
#include "LogicMain.h"
#include "Packet.h"


class ChatServer : public NetService
{
public:
	ChatServer() = default;
	virtual ~ChatServer() = default;

	virtual void OnAccept(u_Int unique_id) override;
	virtual void OnClose(u_Int unique_id) override;
	virtual void OnRecv(CustomOverEx *pOver, u_Int size_) override;

	void Run(u_Int maxClient);
	void Stop();
private:
	std::unique_ptr<LogicMain> mLogicProc;
};