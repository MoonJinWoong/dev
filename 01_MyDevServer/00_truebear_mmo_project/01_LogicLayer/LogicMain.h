#pragma once

namespace NetworkLayer
{
	class IOCP;
}
namespace LogicLayer
{
	using NetIOCP = NetworkLayer::IOCP;


	class LogicMain
	{
	public:
		LogicMain();
		virtual ~LogicMain();
		void Init();
		void Run();

	private:
		std::unique_ptr<NetIOCP> m_iocpNet;

	};
}