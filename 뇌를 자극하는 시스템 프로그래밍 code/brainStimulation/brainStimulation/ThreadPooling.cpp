#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>


#define WORK_MAX 10000
#define THREAD_MAX 50


typedef void(*WORK)  (void);

DWORD AddWorkToPool(WORK work);
WORK GetWorkFromPool(void);
DWORD MakeThreadToPool(DWORD numOfThread);
void WorkerThreadFunction(LPVOID pParam);


typedef struct __WorkerThread
{
	HANDLE hThread;
	DWORD idThread;

}WorkerThread;


// ������ Ǯ
struct __ThreadPool
{
	// Work�� ����ϱ� ���� �迭
	WORK workdList[WORK_MAX];

	// thread ������ �� thread�� Event������Ʈ 
	WorkerThread workerThreadList[THREAD_MAX];
	HANDLE workerEventList[THREAD_MAX];

	//work�� ���� ���� 
	DWORD idxOfCurrentWork;
	DWORD idxOfLastAddedWork; 

	// number of thread;
	DWORD threadIdx;		// pool �� �����ϴ� thread ���� 

}gThreadPool;





static HANDLE mutex = NULL;

void InitMutex(void)
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void DeInitMutex(void)
{
	BOOL ret = CloseHandle(mutex);
}


void AcquireMutex(void)
{
	DWORD ret = WaitForSingleObject(mutex, INFINITE);

	if (ret = WAIT_FAILED)
		_tprintf(_T("Error !!!! \n"));
}

void ReleaseMutex(void)
{
	BOOL ret = ReleaseMutex(mutex);

	if (ret == 0)
		_tprintf(_T("error release mutex 1!! \n"));
}


// thread pool�� work�� ����ϱ� ���� �Լ� 
DWORD AddWorkToPool(WORK work)
{
	AcquireMutex();
	if (gThreadPool.idxOfLastAddedWork >= WORK_MAX)
	{
		_tprintf(_T("AddWorkToPool fail !!! \n"));
		return NULL;
	}

	// WORK ��� 
	gThreadPool.workdList[gThreadPool.idxOfLastAddedWork++] = work;


	// work ��� �� , ������� thread���� ��� ���� ���� ��Ų��. 
	// ��� ���� �ʿ䰡 �����Ƿ� �������� ������ 
	for (DWORD i = 0; i < gThreadPool.threadIdx; i++)
		SetEvent(gThreadPool.workerEventList[i]);

	ReleaseMutex();

	return 1;
}



// thread���� work�� �����ö� ȣ�� �Ǵ� �Լ� 
// gthread pool�� ���� ������ ��ȣ�ϱ� ���� ����
WORK GetWorkFromPool()
{
	WORK work = NULL;

	AcquireMutex();

	if (!(gThreadPool.idxOfCurrentWork < gThreadPool.idxOfLastAddedWork))
	{
		ReleaseMutex();
		return NULL;
	}

	work = gThreadPool.workdList[gThreadPool.idxOfCurrentWork++];
	ReleaseMutex();

	return work;
}


// thread pool ����
// ���޵Ǵ� ���ڿ� ���� �ٸ��� 

DWORD MakeThreadToPool(DWORD numOfThread)
{
	InitMutex();
	DWORD capacity = WORK_MAX - (gThreadPool.threadIdx);

	if (capacity < numOfThread)
		numOfThread = capacity;

	for (DWORD i = 0; i < numOfThread; i++)
	{
		DWORD idThread;
		HANDLE hThread;

		gThreadPool.workerEventList[gThreadPool.threadIdx] = CreateEvent(NULL, FALSE, FALSE, NULL);

		hThread = CreateThread(
			NULL, 0,
			(LPTHREAD_START_ROUTINE)WorkerThreadFunction,
			(LPVOID)gThreadPool.threadIdx,
			0, &idThread
		);

		gThreadPool.workerThreadList[gThreadPool.threadIdx].hThread = hThread;
		
		gThreadPool.workerThreadList[gThreadPool.threadIdx].idThread = idThread;

		gThreadPool.threadIdx++;
	}

	return numOfThread;
}



// ���޵Ǿ�� �� ���




