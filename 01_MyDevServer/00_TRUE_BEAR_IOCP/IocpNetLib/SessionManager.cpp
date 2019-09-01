
#include "SessionManager.h"


SessionManager* gSessionMgr = nullptr;

SessionManager::~SessionManager()
{

}

void SessionManager::PrepareClientSessions()
{
	m_MaxSessionCount = MAX_CONNECTION;

	for (int i = 0; i < m_MaxSessionCount; ++i)
	{
		ClientSession* client = new ClientSession();

		mFreeSessionList.push_back(client);
		m_SessionList.push_back(client);
	}

	printf_s("[DEBUG][%s] m_MaxSessionCount: %d\n", __FUNCTION__, m_MaxSessionCount);
}



bool SessionManager::AcceptClientSessions()
{



	return true;
}

