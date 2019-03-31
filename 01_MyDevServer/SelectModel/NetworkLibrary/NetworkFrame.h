#pragma once

#include "Define.h"

namespace NetworkLib
{
	class NetworkFrame
	{
	public:
		NetworkFrame() {}
		virtual ~NetworkFrame() {}

		virtual bool Init(NetworkLib::ServerOption obt) { return true; }
		virtual void Start() {}
		virtual void Stop() {}

		//virtual bool SendData() {}
		/*virtual NetworkLib::RecvPckInfo getPckInfo() const
		{
			return NetworkLib::RecvPckInfo();
		}*/

		//void release() override;
	};
}