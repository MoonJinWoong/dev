#pragma once
#include "PreComplie.h"


const int MAX_SESSION = 1000;

// 전방 선언 해주어야 함..
class Session;

class SessionMgr
{
public:
	SessionMgr();
	~SessionMgr();


	void				createSessionPool();
	Session*			getSession(int id);
	bool				acceptSession();
	void				retSession(Session* session);

private:
	 list< Session* >		mRetSessionList;
	 vector<Session*>		mSessionList;

	int						 mSessionCnt;
	int						 mMaxSession;
	mutex					 mSessionlock;

	int					     mSessionID;

	UINT64					mCurSessionCnt;
	UINT64					mRetSessionCnt;
};