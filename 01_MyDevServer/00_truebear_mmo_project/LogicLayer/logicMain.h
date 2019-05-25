#pragma once
#include <memory>


namespace NetworkLayer
{
	class Iocp;
}


namespace LogicLayer
{

	class logicMain
	{
	public :
		logicMain();
		~logicMain();
		bool Init();
		void RunLogicThread();
		void StopLogicThread();

	private:
		std::unique_ptr<NetworkLayer::Iocp> m_IocpNet;

	};
}