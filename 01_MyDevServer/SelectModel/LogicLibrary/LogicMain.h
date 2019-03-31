#include <memory>


namespace NetworkLib
{
	struct ServerOption;
	class NetworkFrame;
}


namespace ChatLogic 
{
	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();
		bool Init();

		void Start();
		void Stop();
	private:
		bool isRun;
		
		std::unique_ptr<NetworkLib::NetworkFrame> m_pNetwork;

	};
}
