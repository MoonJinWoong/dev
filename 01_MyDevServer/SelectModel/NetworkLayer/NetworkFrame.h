#pragma once

#include "Defines.h"


namespace NetworkLib
{
	class NetworkFrame
	{
	public:
		NetworkFrame() {}
		virtual ~NetworkFrame() {}
		virtual bool Init() { return 0; }
	};
}