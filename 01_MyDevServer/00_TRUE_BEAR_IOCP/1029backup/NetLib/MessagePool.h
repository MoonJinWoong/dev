#pragma once
#include <list>
#include <concurrent_queue.h>
#include <iostream>
	//TODO ũ�⺰ Ǯ�� ������
class MessagePool
{
public:
	MessagePool() {}
	explicit MessagePool(const int maxMsgPoolCount, const int extraMsgPoolCount)
	{
		m_MaxMessagePoolCount = maxMsgPoolCount;
		m_ExtraMessagePoolCount = extraMsgPoolCount;

		CreateMessagePool();
	}

	~MessagePool()
	{
		DestroyMessagePool();
	}

public:
	void CheckCreate()
	{
		if (m_MaxMessagePoolCount == -1)
		{
			std::cout << "Messge Pool err in CheckCreate 1" << std::endl;
		}
		if (m_ExtraMessagePoolCount == -1)
		{
			std::cout << "Messge Pool err in CheckCreate 2" << std::endl;
		}
	}

	bool PushMsg(Message* pMsg)
	{
		if (pMsg == nullptr)
		{
			return false;
		}

		pMsg->Clear();

		m_MessagePool.push(pMsg);
		return true;
	}

	Message* PopMsg()
	{
		Message* pMsg = nullptr;
		if (!m_MessagePool.try_pop(pMsg))
		{
			return nullptr;
		}

		return pMsg;
	}



private:
	bool CreateMessagePool()
	{
		Message* pMsg = nullptr;
		for (int i = 0; i < m_MaxMessagePoolCount; ++i)
		{
			pMsg = new Message;
			pMsg->Clear();

			m_MessagePool.push(pMsg);
		}

		for (int i = 0; i < m_ExtraMessagePoolCount; ++i)
		{
			pMsg = new Message;
			pMsg->Clear();

			m_MessagePool.push(pMsg);
		}

		return true;
	}

	void DestroyMessagePool()
	{
		while (auto pData = PopMsg())
		{
			if (pData == nullptr)
			{
				break;
			}
			delete pData;
			pData = nullptr;
		}
	}

private:
	concurrency::concurrent_queue<Message*> m_MessagePool;
	int m_MaxMessagePoolCount = 0;
	int m_ExtraMessagePoolCount = 0;
};
