#include "preCompile.h"


mutex LockObject;

CircleBuffer::CircleBuffer(void)
{
	cRingBuffer = nullptr;
	cBeginMark = nullptr;
	cEndMark = nullptr;
	cCurrentMark = nullptr;
	cGettedBufferMark = nullptr;
	cLastMoveMark = nullptr;
	cUsedBufferSize = 0;
	cAllUserBufSize = 0;

}

CircleBuffer::~CircleBuffer(void)
{
	if (cRingBuffer != nullptr)
		delete[] cRingBuffer;
} 

bool CircleBuffer::CreateCircleBuffer(int nBufferSize)
{
	if (cRingBuffer != NULL)
		delete[] cRingBuffer;

	cBeginMark = new char[nBufferSize];		// 시작 지점 마킹

	if (cBeginMark == nullptr)
	{
		cout << "CircleBuffer Create failed....(cBeginMark is null )" << endl;
		return false;
	}

	cEndMark = cBeginMark + nBufferSize - 1;
	cBufferSize = nBufferSize;

	InitializeCircleBuffer();


}

void CircleBuffer::InitializeCircleBuffer()
{
	// 원형 버퍼 할당할때는 락걸어주자 중간에 끼어들면 안되니까 
	LockObject.lock();
	cUsedBufferSize = 0;
	cCurrentMark = cBeginMark;
	cGettedBufferMark = cBeginMark;
	cLastMoveMark = cEndMark;
	cAllUserBufSize = 0;

	LockObject.unlock();

	cout << "CircleBuffer Init Clear....!!" << endl;
}

char* CircleBuffer::ForwardMark(int Length)
{
	char* PreCurrentMark = NULL;
	
	// 락 걸어야함
	LockObject.lock();

		//링버퍼 예외 처리(overflow)
		if (cUsedBufferSize + Length > cBufferSize)
			return NULL;


		// 공간 있어서  current mark 전진
		if ((cEndMark - cCurrentMark) >= Length)
		{
			PreCurrentMark = cCurrentMark;
			cCurrentMark += Length;
		}

		// 공간이 모자라서 순환하는 경우
		else
		{
			//순환 되기 전 마지막 좌표를 저장
			cLastMoveMark = cCurrentMark;
			cCurrentMark = cBeginMark + Length;
			PreCurrentMark = cBeginMark;
		}

		//락 풀어야함
		LockObject.unlock();
		return PreCurrentMark;
}

//사용된 버퍼양 설정(이것을 하는 이유는 SendPost()함수가 멀티 쓰레드에서 돌아가기때문에
//PrepareSendPacket()에서(ForwardMark()에서) 사용된 양을 늘려버리면 PrepareSendPacket한다음에 데이터를
//채워 넣기전에 바로 다른 쓰레드에서 SendPost()가 불린다면 엉뚱한 쓰레기 데이터가 갈 수 있다.
//그걸 방지하기 위해 데이터를 다 채운 상태에서만 사용된 버퍼 사이즈를 설정할 수 있어야한다.
//이함수는 sendpost함수에서 불리게 된다.


void CircleBuffer::SetUsedBufferSize(int UsedBufferSize)
{
	//락 걸어야함
	LockObject.lock();

	cUsedBufferSize += UsedBufferSize;
	cAllUserBufSize += UsedBufferSize;
	
	
	//락 풀어야함
	LockObject.unlock();
}

char* CircleBuffer::GetBuffer(int nReadSize, int* pReadSize)
{
	char* pRet = NULL;
	//락 걸어야함
	LockObject.lock();

		//마지막까지 다 읽었다면 그 읽어들일 버퍼의 포인터는 맨앞으로 옮긴다.
		if (cLastMoveMark == cGettedBufferMark)
		{
			cGettedBufferMark = cBeginMark;
			cLastMoveMark = cEndMark;
		}

		//현재 버퍼에 있는 size가 읽어들일 size보다 크다면
		if (cUsedBufferSize > nReadSize)
		{
			//링버퍼의 끝인지 판단.
			if ((cLastMoveMark - cGettedBufferMark) >= nReadSize)
			{
				*pReadSize = nReadSize;
				pRet = cGettedBufferMark;
				cGettedBufferMark += nReadSize;   // 전진
			}
			else
			{
				*pReadSize = (int)(cLastMoveMark - cGettedBufferMark);
				pRet = cGettedBufferMark;
				cGettedBufferMark += *pReadSize;
			}
		}
		else if (cUsedBufferSize > 0)
		{
			//링버퍼의 끝인지 판단.
			if ((cLastMoveMark - cGettedBufferMark) >= cUsedBufferSize)
			{
				*pReadSize = cUsedBufferSize;
				pRet = cGettedBufferMark;
				cGettedBufferMark += cUsedBufferSize;
			}
			else
			{
				*pReadSize = (int)(cLastMoveMark - cGettedBufferMark);
				pRet = cGettedBufferMark;
				cGettedBufferMark += *pReadSize;
			}
		}
	

// 락풀어야함
		LockObject.unlock();
	return pRet;
}

	
