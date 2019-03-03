#include "ChatProtocol.h"






int main()
{

	// 檬扁拳 
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//扩加 檬扁拳


	// socket 积己
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//家南 积己
	if (sock == -1)
	{
		return -1;
	}


	// 林家 技泼 棺 楷搬 夸没
	SOCKADDR_IN servaddr = { 0 };//家南 林家
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(32452);
	int re = 0;
	re = connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));//楷搬 夸没
	if (re == -1)
	{
		return -1;
	}


}