#pragma once

// 소켓 생성 ,bind , listen ,  옵션 변경등을 여기서 해주자.
class CustomSocket
{
public:
	CustomSocket() { m_sock = INVALID_SOCKET; }
	~CustomSocket() {}

	void CreateSocket()
	{
		// TODO : UDP 용 소켓도 만들어보자.
		m_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	void CreateSocket(SOCKET sock)
	{
		m_sock = sock;
	}

	// TODO : 에러처리 하나로 묶을 것.
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
			return;
		}

		if (::listen(m_sock, SOMAXCONN) == SOCKET_ERROR)
		{
			std::cout << "fail listen " << std::endl;
			return;
		}
	}

public:
	SOCKET m_sock;

};