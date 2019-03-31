

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

	};
}
