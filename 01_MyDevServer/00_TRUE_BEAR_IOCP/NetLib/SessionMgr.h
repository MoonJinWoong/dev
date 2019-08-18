#pragma once
#include "PreComplie.h"


const int MAX_SESSION = 1000;

// ���� ���� ���־�� ��..
class Session;

class SessionMgr
{
public:
	SessionMgr();
	~SessionMgr();
	int					createSessionID();
	bool				addSession(Session *session);
	list<Session*>		&getSessionList();
	bool				closeSession(Session* session);
	void				kickSession(Session* session);

	Session*			getSession(int id);


private:
	typedef list< Session* > SessionList;
	SessionList				 mSessionList;

	int						 mSessionCnt;
	int						 mMaxSession;
	mutex					 mSessionlock;

	int					     mSessionID;




};