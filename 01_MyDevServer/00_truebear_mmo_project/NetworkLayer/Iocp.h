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
		SOCKET			m_ListenSocket;	// ���� ���� ����
		HANDLE			m_IocpHandle;	// IOCP ��ü �ڵ�
	};
}