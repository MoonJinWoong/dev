#pragma once

template < class TYPE >
class Queue : public Monitor
{
public:
	Queue(int nMaxSize = MAX_QUEUESIZE);
	virtual ~Queue(void);

	bool	PushQueue(TYPE typeQueueItem);		//queue�� ����Ÿ�� �ִ´�.
	void	PopQueue();								//queue���� ����Ÿ�� �����.

	bool	IsEmptyQueue();							//queue�� ������� �˷��ش�.
	TYPE	GetFrontQueue();						//queue���� ����Ÿ�� ���´�.
	int		GetQueueSize();							//queue�� ���� ����ִ� item�� ������ �˷��ش�.
	int		GetQueueMaxSize() { return m_nQueueMaxSize; }  //�ִ� queue����� �˷��ش�
	void	SetQueueMaxSize(int nMaxSize) { m_nQueueMaxSize = nMaxSize; }   //�ִ� queue size�� ���� �ش�
	void	ClearQueue();						//queue�� �ִ� ��� item�� �����Ѵ�.

private:

	TYPE*			m_arrQueue;
	int				m_nQueueMaxSize;				//ť������ ������ ����
	Monitor		m_csQueue;

	int				m_nCurSize;
	int				m_nEndMark;
	int				m_nBeginMark;

};
template <class TYPE>
Queue< TYPE >::Queue(int nMaxSize)
{
	m_arrQueue = new TYPE[nMaxSize];
	m_nQueueMaxSize = nMaxSize;
	ClearQueue();

}

template <class TYPE>
Queue< TYPE >::~Queue< TYPE >(void)
{
	delete[] m_arrQueue;
}

template <class TYPE>
bool Queue< TYPE >::PushQueue(TYPE typeQueueItem)
{
	Monitor::Owner lock(m_csQueue);
	{

		//���س��� ����� �Ѿ��ٸ� ���̻� ť�� ���� �ʴ´�.
		if (m_nCurSize >= m_nQueueMaxSize)
			return false;

		m_nCurSize++;
		if (m_nEndMark == m_nQueueMaxSize)
			m_nEndMark = 0;
		m_arrQueue[m_nEndMark++] = typeQueueItem;

	}
	return true;


}

template <class TYPE>
TYPE Queue< TYPE >::GetFrontQueue()
{
	TYPE typeQueueItem;
	Monitor::Owner lock(m_csQueue);
	{
		if (m_nCurSize <= 0)
			return NULL;
		if (m_nBeginMark == m_nQueueMaxSize)
			m_nBeginMark = 0;
		typeQueueItem = m_arrQueue[m_nBeginMark++];

	}
	return typeQueueItem;

}

template <class TYPE>
void Queue< TYPE >::PopQueue()
{
	Monitor::Owner lock(m_csQueue);
	{
		m_nCurSize--;
	}
}

template <class TYPE>
bool Queue< TYPE >::IsEmptyQueue()
{
	bool bFlag = false;
	Monitor::Owner lock(m_csQueue);
	{
		bFlag = (m_nCurSize > 0) ? true : false;
	}

	return flag;
}

template <class TYPE>
int Queue< TYPE >::GetQueueSize()
{
	int nSize;
	Monitor::Owner lock(m_csQueue);
	{
		nSize = m_nCurSize;
	}

	return nSize;
}

template <class TYPE>
void Queue< TYPE >::ClearQueue()
{
	Monitor::Owner lock(m_csQueue);
	{
		m_nCurSize = 0;
		m_nEndMark = 0;
		m_nBeginMark = 0;
	}
}
