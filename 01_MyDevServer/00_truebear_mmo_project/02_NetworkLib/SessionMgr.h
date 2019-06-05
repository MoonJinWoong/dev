#pragma once

#include "SessionMgr.h"

namespace NetworkLayer
{
	class Session;

	class SessionMgr
	{
	public:
		SessionMgr();
		~SessionMgr();
		bool Init();
		bool addSession(Session* client);


	private:
	};
}