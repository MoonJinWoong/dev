#pragma once
#include "PreCompile.h"

namespace NetworkLayer
{
	class Iocp
	{
	public:
		Iocp();
		~Iocp();
		bool InitNetwork();

	public:
		SOCKET			m_ListenSocket;	// 辑滚 府郊 家南
		HANDLE			m_IocpHandle;	// IOCP 按眉 勤甸
	};
}