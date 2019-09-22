#pragma once
#include "NetPreCompile.h"

	//TODO 크기별 풀로 나누기
class MessagePool
{
public:
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
	bool CheckCreate()
	{
		if (m_MaxMessagePoolCount == -1)
		{
			std::cout << "Messge Pool err in CheckCreate 1" << std::endl;
			return false;
		}
		if (m_ExtraMessagePoolCount == -1)
		{
			std::cout << "Messge Pool err in CheckCreate 2" << std::endl;
			return false;
		}
		return true;
	}

	bool DeallocMsg(Message* pMsg)
	{
		if (pMsg == nullptr)
		{
			return false;
		}

		pMsg->Clear();

		m_MessagePool.push(pMsg);
		return true;
	}

	Message* AllocMsg()
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
		while (auto pData = AllocMsg())
		{
			if (pData == nullptr)
			{
				break;
			}

			delete pData;
		}
	}

private:
	concurrency::concurrent_queue<Message*> m_MessagePool;

	int m_MaxMessagePoolCount = -1;
	int m_ExtraMessagePoolCount = -1;
};
