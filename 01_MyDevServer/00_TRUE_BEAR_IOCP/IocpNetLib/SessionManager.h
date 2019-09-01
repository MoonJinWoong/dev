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

//TODO: �����ϸ� �������� ������� �ʱ�
extern SessionManager* gSessionMgr;
