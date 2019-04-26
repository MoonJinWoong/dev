#pragma once




namespace LogicLayer
{
	class LogicProcess
	{
	private:
	public:
		LogicProcess();
		~LogicProcess();
		void InitLogicProcess();
		void Process();

		void StateCheck();
	};
}