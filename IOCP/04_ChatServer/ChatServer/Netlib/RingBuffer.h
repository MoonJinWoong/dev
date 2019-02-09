#pragma once


class RingBuffer : public Monitor
{
public:
	RingBuffer(void);
	virtual ~RingBuffer(void);

	// ������ �޸� �Ҵ� 
	bool	Create(int nBufferSize = MAX_RINGBUFSIZE);
	// init
	bool	Initialize();
	// �Ҵ�� ���� ũ�� ��ȯ 
	inline int GetBufferSize() { return m_nBufferSize; }

	// �ش��ϴ� ������ �����͸� ��ȯ
	inline char* GetBeginMark() { return m_pBeginMark; }
	inline char* GetCurrentMark() { return m_pCurrentMark; }
	inline char* GetEndMark() { return m_pEndMark; }

	char*	ForwardMark(int nForwardLength);
	char*   ForwardMark(int nForwardLength, int nNextLength, DWORD dwRemainLength);
	void    BackwardMark(int nBackwardLength);

	// ���� ���� ���� 
	void ReleaseBuffer(int nReleaseSize);
	// ���� ���� ũ�� ��ȯ
	int  GetUsedBufferSize() { return m_nUsedBufferSize; }
	
	// ���� ���۾� ����
	void SetUsedBufferSize(int nUsedBufferSize);

	// ���� ���� ��뷮 ��ȯ
	int GetAllUsedBufferSize() { return m_uiAllUserBufSize; }

	// ���� ������ �о ��ȯ 
	char* GetBuffer(int nReadSize, int* pReadSize);

private:

	char*         m_pRingBuffer;		// ���� �����͸� �����ϴ� ���� ������ 

	char*         m_pBeginMark;			// ������ ó���κ��� ����Ű�� ������ 
	char*         m_pEndMark;			// ������ �������κ��� ����Ű�� �ִ� ������ 
	char*         m_pCurrentMark;       // ������ ���� �κ��� ����Ű�� �ִ� ������ 
	char*         m_pGettedBufferMark;  // ������� �����͸� ���� ������ ������ 
	char*         m_pLastMoveMark;      // ForwardMark�Ǳ��� ������ ������ 

	int           m_nBufferSize;        // ������ �� ũ��
	int			  m_nUsedBufferSize;   
	unsigned int  m_uiAllUserBufSize; // �� ó���� �����ͷ� 
	Monitor       m_csRingBuffer;      // ����ȭ��ü 
};