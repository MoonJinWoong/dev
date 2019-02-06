#pragma once



class Thread
{
public:
	Thread(void);
	virtual ~Thread(void);

	bool CreateThread(DWORD dwWaitTick);
	void DestroyThread();
	void Run();
	void Stop();
	void TickThread();
	virtual void  OnProcess() = 0;

	inline DWORD GetTickCount() { return m_dwTickCount; }
	bool IsRun() { return m_blsRun; }


protected:
	HANDLE m_hThread;
	HANDLE m_hQuitEvent;
	HANDLE m_blsRun;
	DWORD  m_dwWaitTick;
	DWORD  m_dwTickCount;
};