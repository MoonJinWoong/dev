#pragma once


namespace LogicLayer
{
	class Room
	{
	public:
		Room();
		~Room();
		void Init(const short index, const short maxUserCount);
	};
}