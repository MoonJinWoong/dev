
#include "preCompile.h"

const int BUFSIZE = 512;


SOCKET g_mysocket;
WSABUF	send_wsabuf;
char 	send_buffer[BUFSIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[BUFSIZE];
char	packet_buffer[BUFSIZE];
DWORD		in_packet_size = 0;
int		saved_packet_size = 0;
int		g_myid;

HANDLE mainhandle;

const char *escape = "/quit";	// 종료 명령
char userID[10];		// 유저ID
char line[30], chatdata[31];
struct sockaddr_in server_addr;

// 함수 몸체 정의
void ProcessPacket(char *ptr);
DWORD WINAPI ProcessInputSend(LPVOID arg);

void Init()
{
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

}

void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}
void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_MSG:
	{
		//	printf("SC_PUT_PLAYER 호출되니......\n");
		sc_packet_msg *my_packet = reinterpret_cast<sc_packet_msg *>(ptr);
		cout << "recv msg : " << my_packet << endl;
		break;
	}


	
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}


int main()
{
	
	Init();

	
	//  send를 위한스레드 생성
	HANDLE hThread;
	DWORD ThreadId;


/*	hThread = CreateThread(NULL, 0, ProcessInputSend, 0, 0, &ThreadId);
	if (hThread == NULL) {
		printf("fail make thread\n");
	*/

	//while (1) {

	//	ReadPacket(g_mysocket);
	//}

	while (1)
	{

		cout << "보낼 데이터 - ";
		if (fgets(chatdata, BUFSIZE+1, stdin) == NULL)
			break;
			
		int len = strlen(chatdata);
		if (chatdata[len - 1] == '\n') chatdata[len - 1] = '\0';
		if (strlen(chatdata) == 0)
			break;

		// 여기서부터 다시 
		//ret = send(g_mysocket,chatdata
			

	}


}
DWORD WINAPI ProcessInputSend(LPVOID arg)
{
	while (TRUE) {
		if (fgets(chatdata, 30, stdin)) {		// 스트림 read
			chatdata[strlen(chatdata) - 1] = '\0';
			sprintf(line, "\n[%s] : %s", userID, chatdata);		// 버퍼의 첫 내용을 출력
			if (send(g_mysocket, line, strlen(line), 0) < 0) {
				printf("Write fail\n");
			}

			if (strstr(line, escape) != 0) {		// 종료명령어
				printf("Bye\n");
				closesocket(g_mysocket);
				exit(0);
			}
		}
	}
	return 0;
}
