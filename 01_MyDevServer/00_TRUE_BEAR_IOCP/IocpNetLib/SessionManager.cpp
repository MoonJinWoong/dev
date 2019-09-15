#include "Session.h"
#include "SessionManager.h"
#include "IocpService.h"

SessionManager* gSessionMgr = nullptr;

SessionManager::SessionManager() :mCurrentIssueCount(0), mCurrentReturnCount(0)
{
}
SessionManager::~SessionManager()
{
}

void SessionManager::PrepareClientSessions()
{
	// 이건 테스트 하면서 조절해볼 것 
	m_MaxSessionCount = 1000;

	// 세션 풀 만들기
	for (int i = 0; i < m_MaxSessionCount; ++i)
	{
		Session* client = new Session();
		mFreeSessionList.push_back(client);
		//m_SessionList.emplace_back(client);
	}

	printf_s("[DEBUG][%s] m_MaxSessionCount: %d\n", __FUNCTION__, m_MaxSessionCount);
}



bool SessionManager::AcceptClientSessions()
{
	FastSpinlockGuard guard(mSpinLock);
	int i = 0;
	while (mCurrentIssueCount - mCurrentReturnCount < m_MaxSessionCount)
	{
		Session* newClient = mFreeSessionList.back();
		mFreeSessionList.pop_back();

		++mCurrentIssueCount;

		if (false == newClient->PostAccept()) 
		{
			return false;
		}

	}

	//std::cout << "current new session count : " << mCurrentIssueCount << std::endl;

	return true;
}



