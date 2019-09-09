#pragma once
#include "preCompile.h"


class Session;

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	void PrepareClientSessions();
	bool AcceptClientSessions();
	int getMaxSessionCount() const { return m_MaxSessionCount; }

private:
	typedef std::list<Session*> ClientList;
	ClientList	mFreeSessionList;

	FastSpinLock mSpinLock;

	uint64_t mCurrentIssueCount;
	uint64_t mCurrentReturnCount;


	int m_MaxSessionCount = 0;

	std::vector<Session*> m_SessionList;
};

extern SessionManager* gSessionMgr;
