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


// 스레드 풀
struct __ThreadPool
{
	// Work를 등록하기 위한 배열
	WORK workdList[WORK_MAX];

	// thread 정보와 각 thread별 Event오브젝트 아래 둘 배열 idx는 쌍을 이룬다.
	WorkerThread workerThreadList[THREAD_MAX];
	HANDLE workerEventList[THREAD_MAX];

	//work에 대한 정보 
	DWORD idxOfCurrentWork;			// 대기 1순위 work idx 
	DWORD idxOfLastAddedWork;		// 마지막으로 추가한 work idx+1 새로 들어온 work가 등록되면 이 위치에 넣으면 됨

	// number of thread;
	DWORD threadIdx;		// pool 에 존재하는 thread 개수 

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


// thread pool에 work를 등록하기 위한 함수 
DWORD AddWorkToPool(WORK work)
{
	AcquireMutex();
	if (gThreadPool.idxOfLastAddedWork >= WORK_MAX)
	{
		_tprintf(_T("AddWorkToPool fail !!! \n"));
		return NULL;
	}

	// WORK 등록 
	gThreadPool.workdList[gThreadPool.idxOfLastAddedWork++] = work;


	// work 등록 후 , 대기중인 thread들을 모두 깨워 일을 시킨다. 
	// 모두 깨울 필요가 없으므로 정교함이 떨어짐 
	for (DWORD i = 0; i < gThreadPool.threadIdx; i++)
		SetEvent(gThreadPool.workerEventList[i]);

	ReleaseMutex();

	return 1;
}



// thread에서 work를 가져올때 호출 되는 함수 
// gthread pool에 대한 접근을 보호하기 위해 정의
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


// thread pool 생성
// 전달되는 인자에 따라 다르게 

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



// 전달되어야 할 대상
void WorkerThreadFunction(LPVOID pParam)
{
	WORK workFunction;
	HANDLE event = gThreadPool.workerEventList[(DWORD)pParam];

	while (1)
	{
		workFunction = GetWorkFromPool();
		if (workFunction == nullptr)
		{
			// work 가 할당될 때까지 blocked 상태 
			WaitForSingleObject(event, INFINITE);
			continue;
		}
		workFunction();
	}
}

void TestFunction()
{
	// i 는 static이므로 둘 이상의 스레드에 의해 동시 접근
	static int i = 0;
	i++;
	_tprintf(_T("Good test !!!!!!! --%d : Processing thread : %d  \n\n"), i, GetCurrentThreadId());
}


int _tmain(int argc, TCHAR* argv[])
{
	MakeThreadToPool(3);

	for (int i = 0; i < 100; i++)
	{
		AddWorkToPool(TestFunction);
	}

	Sleep(50000); 
	return 0;
}


