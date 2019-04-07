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
		virtual int ClientSessionPoolSize() { return 0; }
		virtual NET_ERROR_CODE SendData(const int sessionIndex, const short packetId,
			const short size, const char* pMsg) {
			return NET_ERROR_CODE::NONE;
		}
	};
}