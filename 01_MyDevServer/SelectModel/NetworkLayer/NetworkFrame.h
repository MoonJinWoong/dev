#pragma once

#include "Defines.h"


namespace NetworkLayer
{
	class NetworkFrame
	{
	public:
		NetworkFrame() {}
		virtual ~NetworkFrame() {}
		bool Init(const ServerConfig& config) { return 0; }
	};
}