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
	std::list<Session*> mFreeSessionList;
	std::vector<Session*> m_SessionList;

	FastSpinLock mSpinLock;

	uint64_t mCurrentIssueCount;
	uint64_t mCurrentReturnCount;


	int m_MaxSessionCount = 0;

};

extern SessionManager* gSessionMgr;
