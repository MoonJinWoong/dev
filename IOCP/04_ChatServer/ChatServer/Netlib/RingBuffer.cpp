#include "PreComplie.h"
#include "RingBuffer.h"


RingBuffer::RingBuffer(void)
{
	m_pRingBuffer			= nullptr;
	m_pBeginMark			= nullptr;
	m_pEndMark				= nullptr;
	m_pCurrentMark			= nullptr;
	m_pGettedBufferMark		= nullptr;
	m_pLastMoveMark			= nullptr;
	m_nUsedBufferSize		= 0;
	m_uiAllUserBufSize		= 0;
}

RingBuffer::~RingBuffer(void)
{
	if (m_pBeginMark != nullptr)
		delete [] m_pBeginMark;
}

bool RingBuffer::Initialize()
{
	Monitor::Owner lock(m_csRingBuffer);
	{
		m_nUsedBufferSize = 0;
		m_pCurrentMark = m_pBeginMark;
		m_pGettedBufferMark = m_pBeginMark;
		m_pLastMoveMark = m_pEndMark;
		m_uiAllUserBufSize = 0;
	}
	return true;
}


bool RingBuffer::Create(int nBufferSize)
{
	if (m_pRingBuffer != nullptr)
		delete [] m_pRingBuffer;

	m_pBeginMark = new char[nBufferSize];

	if (m_pBeginMark == nullptr)
		return false;

	m_pEndMark = m_pBeginMark + nBufferSize - 1;
	m_nBufferSize = nBufferSize;

	Initialize();
	return true;
}

char* RingBuffer::ForwardMark(int nForwardLength)
{
	char* pPreCurrentMark = nullptr;
	Monitor::Owner lock(m_csRingBuffer);
	{
		// 오버플로우 체크 
		if (m_nUsedBufferSize + nForwardLength > m_nBufferSize)
			return nullptr;
		if ((m_pEndMark - m_pCurrentMark) >= nForwardLength)
		{
			pPreCurrentMark = m_pCurrentMark;
			m_pCurrentMark += nForwardLength;
		}
		else
		{ 
			// 순환되기 전 마지막 좌표 저장 
			m_pLastMoveMark = m_pCurrentMark;
			m_pCurrentMark = m_pBeginMark + nForwardLength;
			pPreCurrentMark = m_pBeginMark;
		}
	}

	return pPreCurrentMark;
}

char* RingBuffer::ForwardMark(int nForwardLength
	, int nNextLength
	, DWORD dwRemainLength)
{
	Monitor::Owner lock(m_csRingBuffer);
	{
		//링버퍼 오버플로 체크
		if (m_nUsedBufferSize + nForwardLength + nNextLength > m_nBufferSize)
			return NULL;

		if ((m_pEndMark - m_pCurrentMark) > (nNextLength + nForwardLength))
			m_pCurrentMark += nForwardLength;

		//남은 버퍼의 길이보다 앞으로 받을 메세지양 크면 현재 메세지를 처음으로 
		//복사한다음 순환 된다.
		else
		{
			//순환 되기 전 마지막 좌표를 저장
			m_pLastMoveMark = m_pCurrentMark;
			CopyMemory(m_pBeginMark,
				m_pCurrentMark - (dwRemainLength - nForwardLength),
				dwRemainLength);
			m_pCurrentMark = m_pBeginMark + dwRemainLength;
		}
	}
	return m_pCurrentMark;
}

void RingBuffer::BackwardMark(int nBackwardLength)
{
	Monitor::Owner lock(m_csRingBuffer);
	{
		//nBackwardLength양만큼 현재 버퍼포인터를 뒤로 보낸다.
		m_nUsedBufferSize -= nBackwardLength;
		m_pCurrentMark -= nBackwardLength;
	}
}

void RingBuffer::ReleaseBuffer(int nReleaseSize)
{
	Monitor::Owner lock(m_csRingBuffer);
	{
		m_nUsedBufferSize -= nReleaseSize;
	}
}


char* RingBuffer::GetBuffer(int nReadSize, int* pReadSize)
{
	char* pRet = nullptr;
	Monitor::Owner lock(m_csRingBuffer);
	{
		if (m_pLastMoveMark == m_pGettedBufferMark)
		{
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark = m_pEndMark;
		}

		// 현재 버퍼에 있는 size가 읽어들일 size보다 크다면 
		if (m_nUsedBufferSize > nReadSize)
		{
			// 링버퍼의 끝인지 판단
			if ((m_pLastMoveMark - m_pGettedBufferMark) >= nReadSize)
			{
				*pReadSize = nReadSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += nReadSize;
			}
			else
			{
				*pReadSize = (int)(m_pLastMoveMark - m_pGettedBufferMark);
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
		else if (m_nUsedBufferSize > 0)
		{
			//링버퍼의 끝인지 판단.
			if ((m_pLastMoveMark - m_pGettedBufferMark) >= m_nUsedBufferSize)
			{
				*pReadSize = m_nUsedBufferSize;
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += m_nUsedBufferSize;
			}
			else
			{
				*pReadSize = (int)(m_pLastMoveMark - m_pGettedBufferMark);
				pRet = m_pGettedBufferMark;
				m_pGettedBufferMark += *pReadSize;
			}
		}
	}

	return pRet;
}


