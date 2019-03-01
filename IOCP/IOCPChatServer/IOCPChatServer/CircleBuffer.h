#pragma once


class NETLIB CircleBuffer
{
public:
	CircleBuffer(void);
	virtual ~CircleBuffer(void);

	//링버퍼 메모리 할당
	bool			CreateCircleBuffer(int nBufferSize = MAX_CIRCLE_BUFFER_SIZE);
	
	//초기화
	void			InitializeCircleBuffer();
	//할당된 버퍼 크기를 반환한다.
	//inline int		GetBufferSize() { return m_nBufferSize; }

	////해당하는 버퍼의 포인터를 반환
	//inline char*	GetBeginMark() { return m_pBeginMark; }
	//inline char*	GetCurrentMark() { return m_pCurrentMark; }
	//inline char*	GetEndMark() { return m_pEndMark; }

	//char*			ForwardMark(int nForwardLength);
	//char*			ForwardMark(int nForwardLength, int nNextLength, DWORD dwRemainLength);
	//void			BackwardMark(int nBackwardLength);

	////사용된 버퍼 해제
	//void			ReleaseBuffer(int nReleaseSize);
	////사용된 버퍼 크기 반환
	//int				GetUsedBufferSize() { return cAllUserBufSize; }
	////사용된 버퍼양 설정(이것을 하는 이유는 SendPost()함수가 멀티 쓰레드에서 돌아가기때문에
	////PrepareSendPacket()에서(ForwardMark()에서) 사용된 양을 늘려버리면 PrepareSendPacket한다음에 데이터를
	////채워 넣기전에 바로 다른 쓰레드에서 SendPost()가 불린다면 엉뚱한 쓰레기 데이터가 갈 수 있다.
	////그걸 방지하기 위해 데이터를 다 채운 상태에서만 사용된 버퍼 사이즈를 설정할 수 있어야한다.
	////이함수는 sendpost함수에서 불리게 된다.
	//void			SetUsedBufferSize(int nUsedBufferSize);

	////누적 버퍼 사용양 반환
	//int				GetAllUsedBufferSize() { return cAllUserBufSize; }


	////버퍼 데이터 읽어서 반환
	//char*			GetBuffer(int nReadSize, int* pReadSize);

	// mutex cLockObject;
private:

	char*			cRingBuffer;		   //실제 데이터를 저장하는 버퍼 포인터
					
	char*			cBeginMark;			//버퍼의 처음부분을 가리키고 있는 포인터
	char*			cEndMark;				//버퍼의 마지막부분을 가리키고 있는 포인터
	char*			cCurrentMark;			//버퍼의 현재 부분을 가리키고 있는 포인터
	char*			cGettedBufferMark;	//현재까지 데이터를 읽은 버퍼의 포인터
	char*			cLastMoveMark;		//ForwardMark되기 전에 마지막 포인터

	int				cBufferSize;		//버퍼의 총 크기
	int 			cUsedBufferSize;	//현재 사용된 버퍼의 크기
	unsigned int	cAllUserBufSize; //총 처리된 데이터량
	//lock_guard<mutex>  cLockObject;		//동기화 객체

	
private:
	//// No copies do not implement
	//CircleBuffer(const RingBuffer &rhs);
	//CircleBuffer &operator=(const RingBuffer &rhs);
};

