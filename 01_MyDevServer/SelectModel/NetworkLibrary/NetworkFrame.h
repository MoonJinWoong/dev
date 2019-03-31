#pragma once

#include "Define.h"

namespace NetworkLib
{
	class NetworkFrame
	{
		NetworkFrame() {}
		virtual ~NetworkFrame() {}

		virtual bool Init() { return true; }
		virtual void Start() {}
		virtual void Stop() {}

		virtual bool SendData() {}
		virtual NetworkLib::RecvPckInfo getPckInfo() const
		{
			return NetworkLib::RecvPckInfo();
		}

		//void release() override;
	};
}