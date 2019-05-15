#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Client.h"

namespace NetworkLayer 
{ 
	class SelectNetwork; 
}

namespace LogicLayer
{
	using SelNet = NetworkLayer::SelectNetwork;

	class Room
	{
	public:
		Room();
		~Room();
		void Init(const short index, const short maxClient);

	private:
		short m_Index;
		short m_MaxClient;
	};
}