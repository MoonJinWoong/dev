#include "PacketMgr.h"



void PacketMgr::Init(const unsigned int maxClient)
{
}

bool PacketMgr::Start()
{
	isRun = true;

	//TODO PktProcThread ��������
	return true;
}

void PacketMgr::Stop()
{

}

void PacketMgr::PktProcThread()
{
	while (isRun)
	{

	}
}
