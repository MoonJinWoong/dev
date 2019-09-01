#pragma once
#include "preCompile.h"


class SessionManager
{
public:
	SessionManager() : mCurrentIssueCount(0), mCurrentReturnCount(0)
	{}

	~SessionManager();

	void PrepareClientSessions();
	bool AcceptClientSessions();






private:
	uint64_t mCurrentIssueCount;
	uint64_t mCurrentReturnCount;
};

//TODO: 가능하면 전역으로 사용하지 않기
extern SessionManager* gSessionMgr;
