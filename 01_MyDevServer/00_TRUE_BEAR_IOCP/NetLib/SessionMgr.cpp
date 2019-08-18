#include "PreComplie.h"
#include "SessionMgr.h"
#include "IocpServer.h"

SessionMgr::SessionMgr()
{
	mSessionID = 1;
	mMaxSession = MAX_SESSION;

}

SessionMgr::~SessionMgr()
{
	vector<Session*> removeSessionVec;
	removeSessionVec.resize(mSessionList.size());
	copy(mSessionList.begin(), mSessionList.end(), removeSessionVec.begin());

	for (auto session : removeSessionVec)
	{
		session->onClose();
	}

	mSessionList.clear();
}

int SessionMgr::createSessionID()
{
	return mSessionID++;
}

bool SessionMgr::addSession(Session* session)
{
	mSessionlock.lock();
	auto findSession = find(mSessionList.begin(), mSessionList.end(), session);
	if (findSession != mSessionList.end())
	{
		cout << " alread exist session.." << endl;
		mSessionlock.unlock();
		return false;
	}
	if (mSessionCnt > mMaxSession)
	{
		cout << "max session ...!!" << endl;
		mSessionlock.unlock();
		return false;
	}

	session->setID(this->createSessionID());
	mSessionList.push_back(session);
	++mSessionCnt;
	mSessionlock.unlock();

	return true;
}

list<Session*> &SessionMgr::getSessionList()
{
	return mSessionList;
}

bool SessionMgr::closeSession(Session* session)
{
	mSessionlock.lock();

	if (session == nullptr)
	{
		mSessionlock.unlock();
		return false;
	}


	 auto findSession = find(mSessionList.begin(), mSessionList.end(), session);
	if (findSession != mSessionList.end())
	{
		Session *delSession = *findSession;
		::closesocket(delSession->mSockData.sSocket);

		mSessionList.remove(delSession);
		--mSessionCnt;

		delete(delSession);
		mSessionlock.unlock();
		return true;
	}

	mSessionlock.unlock();
	return false;
}

void SessionMgr::kickSession(Session* session)
{
	mSessionlock.lock();
	if (!session)
	{
		mSessionlock.unlock();
		return;
	}


	LINGER linger;
	linger.l_onoff = 1; // 사용하겠다
	linger.l_linger = 0;   // 대기시간 옵션 . 완료 안되도 바로 종료하겠다.

	::setsockopt(session->mSockData.sSocket, SOL_SOCKET ,SO_LINGER
		, (char *)&linger, sizeof(linger));

	this->closeSession(session);
	mSessionlock.unlock();

}

Session* SessionMgr::getSession(int id)
{
	mSessionlock.lock();
	Session* findSession = nullptr;

	for (auto session : mSessionList)
	{
		if (session->m_ID == id)
		{
			findSession = session;
			break;
		}
	}



	mSessionlock.unlock();
	return findSession;
}

