#pragma once

#include "Defines.h"
#include "NetworkError.h"

namespace NetworkLib
{
	class NetworkFrame
	{
	public:
		NetworkFrame() {}
		virtual ~NetworkFrame() {}
		virtual NET_ERROR_CODE Init() { return NET_ERROR_CODE::NONE; }
	};
}