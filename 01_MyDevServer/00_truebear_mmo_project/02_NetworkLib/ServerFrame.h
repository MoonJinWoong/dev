#pragma once


namespace Core
{
	typedef enum SERVER_STATUS
	{
		NONE, START,STOP
	};

	class ServerFrame
	{
	public:
		ServerFrame(Content* content);
		virtual ~ServerFrame();

		virtual void Init();
		virtual bool Start() = 0;		// 순수 가상 함수
		SERVER_STATUS& getStatus();
		
		void PutPacket(PacketBundle* package);

	protected:
		char m_ip[16];
		int  m_port;
		int  m_worker_cnt;

		SERVER_STATUS m_status;
		Content* m_content;
	};
}
 