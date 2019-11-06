#pragma once
#include "Packet.h"


class PacketMgr
{
public:
	PacketMgr() = default;
	~PacketMgr() = default;

	void Init(const unsigned int maxClient);

	bool Start();

	void Stop();

	void PktProcThread();
private:
	bool isRun = false;

};