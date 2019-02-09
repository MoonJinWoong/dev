#pragma once


class RingBuffer : public Monitor
{
public:
	RingBuffer(void);
	virtual ~RingBuffer(void);

	// 링버퍼 메모리 할당 
	bool	Create(int nBufferSize = MAX_RINGBUFSIZE);
	// init
	bool	Initialize();
	// 할당된 버퍼 크기 반환 
	inline int GetBufferSize() { return m_nBufferSize; }

	// 해당하는 버퍼의 포인터를 반환
	inline char* GetBeginMark() { return m_pBeginMark; }
	inline char* GetCurrentMark() { return m_pCurrentMark; }
	inline char* GetEndMark() { return m_pEndMark; }

	char*	ForwardMark(int nForwardLength);
	char*   ForwardMark(int nForwardLength, int nNextLength, DWORD dwRemainLength);
	void    BackwardMark(int nBackwardLength);

	// 사용된 버퍼 해제 
	void ReleaseBuffer(int nReleaseSize);
	// 사용된 버퍼 크기 반환
	int  GetUsedBufferSize() { return m_nUsedBufferSize; }
	
	// 사용된 버퍼양 설정
	void SetUsedBufferSize(int nUsedBufferSize);

	// 누적 버퍼 사용량 반환
	int GetAllUsedBufferSize() { return m_uiAllUserBufSize; }

	// 버터 데이터 읽어서 반환 
	char* GetBuffer(int nReadSize, int* pReadSize);

private:

	char*         m_pRingBuffer;		// 실제 데이터를 저장하는 버퍼 포인터 

	char*         m_pBeginMark;			// 버퍼의 처음부분을 가리키는 포인터 
	char*         m_pEndMark;			// 버퍼의 마지막부분을 가리키고 있는 포인터 
	char*         m_pCurrentMark;       // 버퍼의 현재 부분을 가리키고 있는 포인터 
	char*         m_pGettedBufferMark;  // 현재까지 데이터를 읽은 버퍼의 포인터 
	char*         m_pLastMoveMark;      // ForwardMark되기전 마지막 포인터 

	int           m_nBufferSize;        // 버퍼의 총 크기
	int			  m_nUsedBufferSize;   
	unsigned int  m_uiAllUserBufSize; // 총 처리된 데이터량 
	Monitor       m_csRingBuffer;      // 동기화객체 
};