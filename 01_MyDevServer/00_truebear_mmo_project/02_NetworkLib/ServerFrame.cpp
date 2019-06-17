
#include "ServerFrame.h"


namespace Core
{
	ServerFrame::ServerFrame(Content* content) :m_content(content)
	{
		this->Init();
	}

	ServerFrame::~ServerFrame()
	{
		//ShutDown();

		m_status = STOP;
		
		delete(m_content);
	}
	void ServerFrame::Init()
	{
		m_status = NONE;
	}

	SERVER_STATUS& ServerFrame::getStatus()
	{
		return m_status;
	}

	void ServerFrame::PutPacket(PacketBundle* packetbundle)
	{
		m_content->putPacketBundle(packetbundle);
	}


}