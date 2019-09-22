#pragma once

// ���� ���� ,bind , listen ,  �ɼ� ������� ���⼭ ������.
class CustomSocket
{
public:
	CustomSocket() { m_sock = INVALID_SOCKET; }
	~CustomSocket() {}

	void CreateSocket()
	{
		m_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	void CreateSocket(SOCKET sock)
	{
		m_sock = sock;
	}

	// TODO : ����ó�� �ϳ��� ���� ��.
	void BindAndListenSocket()
	{
		SOCKADDR_IN	addr;
		ZeroMemory(&addr, sizeof(SOCKADDR_IN));

		addr.sin_family = AF_INET;
		addr.sin_port = htons(8000);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (::bind(m_sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR)
		{
			std::cout<<"fail bind "<<std::endl;
			//return false;
		}

		if (::listen(m_sock, SOMAXCONN) == SOCKET_ERROR)
		{
			std::cout << "fail listen " << std::endl;
			//return false;
		}
	}

public:
	SOCKET m_sock;

};