#pragma once

#include "preCompile.h"

namespace NetworkLayer
{
	class Session;

	class SessionMgr
	{
	public:
		SessionMgr();
		~SessionMgr();
		bool Init();
		bool addSession(long long seq);
		Session* GetSession(const long long Index);

		Session* AllocSessionPoolIndex();
		Session* FindBySeq(const long long sessionIndex);


	private:
		//std::unique_ptr<Session> m_Session;

		std::vector<Session> m_SessionPool;
		std::list<int> m_SessionIndex;   // 


		std::unordered_map<long long , Session*> m_SearchByLL;
		//std::unordered_map<const char*, Session*> m_SearchID; //char*는 key로 사용못함

	};
}