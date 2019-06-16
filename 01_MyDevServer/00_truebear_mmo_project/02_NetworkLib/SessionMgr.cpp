

#include "Session.h"
#include "SessionMgr.h"


namespace NetworkLayer
{
	SessionMgr::SessionMgr() 
	{

	}
	SessionMgr::~SessionMgr() {}
	bool SessionMgr::Init()
	{

		// session √ ±‚»≠
		for (int i = 0; i < MAX_CLIENTS; ++i)
		{
			Session session;
			session.Init();
			m_SessionPool.push_back(session);
			m_SessionIndex.push_back(i);
		}

		


		std::cout << "Session Manager Init Complete" << std::endl;
		return true;
	}
	Session* SessionMgr::GetSession(const long long Index)
	{
		auto session = FindBySeq(Index);

		if (session == nullptr)
		{
			std::cout << "GetClient in ClientManager ..Error" << std::endl;
			return nullptr;
		}
		return session;
	}

	bool SessionMgr::addSession(long long seq)
	{
		auto newSession = AllocSessionPoolIndex();
		if (newSession == nullptr)
			return false;

		newSession->setUniqueId(seq);

		m_SearchByLL.insert({ seq, newSession });

		std::cout << "client - " << newSession->getUniqueId() << std::endl;
		return true;
	}

	Session* SessionMgr::AllocSessionPoolIndex()
	{
		if (m_SessionIndex.empty())
			return nullptr;

		int index = m_SessionIndex.front();
		m_SessionIndex.pop_front();


		return &m_SessionPool[index];
	}

	Session* SessionMgr::FindBySeq(const long long sessionIndex)
	{
		auto findIter = m_SearchByLL.find(sessionIndex);

		if (findIter == m_SearchByLL.end())
			return nullptr;

		return (Session*)findIter->second;
	}
}