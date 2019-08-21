#include "PreComplie.h"
#include "SessionMgr.h"
#include "IocpServer.h"

SessionMgr::SessionMgr() :mCurSessionCnt(0),mRetSessionCnt(0)
{
	// �񵿱� accept�� ���� session�� �̸� �����д�.
	createSessionPool();
}

SessionMgr::~SessionMgr()
{
	for (auto session : mRetSessionList)
	{
		delete session;
	}
}

void SessionMgr::createSessionPool()
{
	mMaxSession = MAX_SESSION;

	for (int i = 0; i < mMaxSession; ++i)
	{
		Session* session = new Session();
		
		mRetSessionList.push_back(session);
		mSessionList.push_back(session);
	}

	cout << "create session pool : " << mMaxSession <<  endl;
}

void SessionMgr::retSession(Session* session)
{
	// Ŭ�� session �������� ��ȯ �� �� �̰� ȣ���ؼ� �ٽ� ���ڸ��� 


	mSessionlock.lock();
	//CRASH_ASSERT(client->mConnected == 0 && client->mRefCount == 0);
	
	session->resetSession();
	mRetSessionList.push_back(session);
	++mRetSessionCnt;

	mSessionlock.unlock();
}

Session* SessionMgr::getSession(int id)
{
	if (id < 0 || id >= mMaxSession)
	{
		return nullptr;
	}

	return mSessionList[id];
}

bool SessionMgr::acceptSession()
{
	mSessionlock.lock();

	while (mCurSessionCnt - mRetSessionCnt < mMaxSession)
	{
		Session* newSession = mRetSessionList.back();
		mRetSessionList.pop_back();

		++mCurSessionCnt;

		if (false == newSession->postAccept()) 
		{
			mSessionlock.unlock();

			return false;
		}
	}

	mSessionlock.unlock();
}













//
//int SessionMgr::createSessionID()
//{
//	return mSessionID++;
//}

//bool SessionMgr::addSession(Session* session)
//{
//	mSessionlock.lock();
//	auto findSession = find(mSessionList.begin(), mSessionList.end(), session);
//	if (findSession != mSessionList.end())
//	{
//		cout << " alread exist session.." << endl;
//		mSessionlock.unlock();
//		return false;
//	}
//	if (mSessionCnt > mMaxSession)
//	{
//		cout << "max session ...!!" << endl;
//		mSessionlock.unlock();
//		return false;
//	}
//
//	session->setID(this->createSessionID());
//	mSessionList.push_back(session);
//	++mSessionCnt;
//	mSessionlock.unlock();
//
//
//	cout << "Session ID : " << mSessionID << endl;
//	return true;
//}


//bool SessionMgr::closeSession(Session* session)
//{
//	mSessionlock.lock();
//
//	if (session == nullptr)
//	{
//		mSessionlock.unlock();
//		return false;
//	}
//
//
//	 auto findSession = find(mSessionList.begin(), mSessionList.end(), session);
//	if (findSession != mSessionList.end())
//	{
//		Session *delSession = *findSession;
//		::closesocket(delSession->mSockData.sSocket);
//
//		mFreeSessionList.remove(delSession);
//		--mSessionCnt;
//
//		delete(delSession);
//		mSessionlock.unlock();
//		return true;
//	}
//
//	mSessionlock.unlock();
//	return false;
//}

//void SessionMgr::kickSession(Session* session)
//{
//	mSessionlock.lock();
//	if (!session)
//	{
//		mSessionlock.unlock();
//		return;
//	}
//
//	LINGER linger;
//	linger.l_onoff = 1; // ����ϰڴ�
//	linger.l_linger = 0;   // ���ð� �ɼ� . �Ϸ� �ȵǵ� �ٷ� �����ϰڴ�.
//
//	::setsockopt(session->mSockData.sSocket, SOL_SOCKET ,SO_LINGER
//		, (char *)&linger, sizeof(linger));
//
//	this->closeSession(session);
//	mSessionlock.unlock();
//}



