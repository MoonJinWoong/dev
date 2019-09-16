#pragma once
#include "preCompile.h"


class Session;

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	void CreateSessionPool();
	bool AcceptClientSessions();
	int getMaxSessionCount() const { return m_MaxSessionCount; }
	void ReturnSession(Session* client);
		

private:
	std::list<Session*> mFreeSessionList;
	std::list<Session*> m_SessionList;

	FastSpinLock mSpinLock;

	uint64_t mCurrentIssueCount;
	uint64_t mCurrentReturnCount;


	int m_MaxSessionCount = 0;

};

extern SessionManager* gSessionMgr;
