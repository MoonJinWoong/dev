#pragma once
#include "precompile.h"



class Session;

class SessionMgr
{
public:
	SessionMgr() : mCurrentIssueCount(0), mCurrentReturnCount(0)
	{}

	~SessionMgr();

	void PrepareClientSessions();
	bool AcceptClientSessions();

	void ReturnClientSession(ClientSession* client);


	int MaxClientSessionCount() { return m_MaxSessionCount; }

	ClientSession* GetClientSession(const int index);


private:
	typedef std::list<ClientSession*> ClientList;
	ClientList	mFreeSessionList;

	FastSpinlock mLock;

	uint64_t mCurrentIssueCount;
	uint64_t mCurrentReturnCount;


	int m_MaxSessionCount = 0;

	std::vector<Session*> m_SessionList;
};

//TODO: 가능하면 전역으로 사용하지 않기
extern ClientSessionManager* GClientSessionManager;
