#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define MAX_THREADS		(1024*10)

DWORD WINAPI ThreadFlow(LPVOID param)
{
	DWORD threadNum = (DWORD)param;

	while (1)
	{
		_tprintf(_T("thread num : %d  \n"), threadNum);
		Sleep(5000);
	}
	return 0;
}

DWORD cntOfThread = 0;


int _tmain(int argcm, TCHAR* argv[])
{
	DWORD	dwThreadID[MAX_THREADS];
	HANDLE	hThread[MAX_THREADS];

	// 생성 가능한 최대 개수의 스레드 
	while (1)
	{
		hThread[cntOfThread] = CreateThread(
			NULL,							// 보안 속성
			0,								// 디폴트 스택 사이즈 
			ThreadFlow,						// thread 함수 이름
			(LPVOID)cntOfThread,			// thread 함수 전달 인자
			0,								// 디포르 생성 flag
			&dwThreadID[cntOfThread]		// thread id 반환
		);
		if (hThread[cntOfThread] == NULL)
		{
			_tprintf(_T("Maximum thread number !!!! : %d \n"), cntOfThread);
			break;
		}
		cntOfThread++;
	}


	for (DWORD i = 0; i < cntOfThread; i++)
	{
		CloseHandle(hThread[i]);
	}

	return 0;
}