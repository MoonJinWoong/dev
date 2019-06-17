#pragma once


#include "TypeDefine.h"
#include <queue>
#include <stdexcept>

namespace Core
{
	enum { READ, WRITE, MAX };

	// 이 클래스의 목적은 여러 thread에서 패킷 처리를 할 때
	// 도착한 순서대로 처리를 한곳에서 일관적으로 하기 위함

	template <typename T>
	class ThreadQueue
	{
	public:
		ThreadQueue(WCHAR* name) : m_lock(name)
		{
			m_input_queue = &m_queue[WRITE];
			m_output_queue = &m_queue[READ];
		}
		~ThreadQueue()
		{
			m_input_queue->empty();
			m_output_queue->empty();
		}
		
		void push(const T& value)
		{
			std::lock_guard<std::mutex> guard(m_lock);
			m_input_queue->push(value);
		}
		bool pop(T& value)
		{
			std::lock_guard<std::mutex> guard(m_lock);
			size_t size = this->getQueueSize();
		
			if (size == 0) return false;

			if (m_output_queue->empty())
				this->changeQueue();

			value = m_output_queue->front();
			m_output_queue->pop();
			
			return true;
		}

		void changeQueue()
		{
			std::lock_guard<std::mutex> guard(m_lock);
			if (m_input_queue == &m_queue[WRITE])
			{
				m_input_queue = &m_queue[READ];
				m_output_queue = &m_queue[WRITE];
			}
			else
			{
				m_input_queue = &m_queue[WRITE];
				m_output_queue = &m_queue[READ];
			}
		}

		bool isEmpty() const
		{
			return m_output_queue->empty();
		}

		size_t getQueueSize() const
		{
			std::lock_guard<std::mutex> guard(m_lock);
			return (size_t)(m_input_queue[WRITE].size() + m_output_queue[READ].size());
		}


	private:
		std::queue<T>	m_queue[MAX_QUEUE];
		std::queue<T>	*m_input_queue;		// input 받는 컨테이너
		std::queue<T>   *m_output_queue;	    // output 하는 곳.

		__lock			m_lock;				
	};
}