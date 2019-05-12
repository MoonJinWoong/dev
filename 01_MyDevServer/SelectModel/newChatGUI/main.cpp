#include "NetDefine.h"
#include <windows.h>
#include "resource.h" 
DWORD WINAPI ClientMain(LPVOID arg);
void err_quit(const char* msg);
void err_display(const char* msg);
void DisplayText(const char* fmt, ...);
void PacketProcess(char* recvbuf);


SOCKET sock;
HWND hLoginInput; // ���� ��Ʈ��
HBRUSH g_hbrBackground = NULL;
HWND hPrint;		// ��� â
HWND hLobbyListBox;  // �κ� ����Ʈ â
HWND hLobbyEnter;

LRESULT CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int msgboxID;

	switch (Message)
	{
	case WM_INITDIALOG:
		//g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));
		//SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
		//	MAKEINTRESOURCE(IDI_APPLICATION)));
		//SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
		//	MAKEINTRESOURCE(IDI_APPLICATION)));
		hLoginInput = GetDlgItem(hwnd, INPUT_LOGIN);  // id �Է¶�
		hPrint = GetDlgItem(hwnd, IDC_EDIT3);  //  ���â
		hLobbyListBox = GetDlgItem(hwnd, LISTBOX_LOBBY2);
		hLobbyEnter = GetDlgItem(hwnd, LOBBYENTER);

		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_CTLCOLORDLG:
		return (LONG)g_hbrBackground;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LONG)g_hbrBackground;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case LOGIN:
		{
		    char input[input_size] = { 0, };
			GetDlgItemText(hwnd, INPUT_LOGIN,(LPSTR)input, input_size + 1);
			
			// ���� �ʱ�ȭ
			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// ��Ŷ �ٵ� ���� �� id ���� 
			PacketLayer::CS_Login_Pkt packet;
			strcpy(packet.szID, input);

			// ��� ���� �� ���ۿ� ���� 
			int curPos = 0;
			PacketLayer::PktHeader header{ sizeof(packet) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::CS_LOGIN };
			memcpy(&sendbuf[curPos], (char*)& header, sizeof(PacketLayer::PktHeader));

			// �ٵ� ���ۿ� ����
			curPos += sizeof(PacketLayer::PktHeader);
			if (sizeof(PacketLayer::CS_Login_Pkt) > 0)
				memcpy(&sendbuf[curPos], (char*)& packet, sizeof(PacketLayer::CS_Login_Pkt));

			
			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::CS_Login_Pkt) + sizeof(PacketLayer::PktHeader), 0);

			if (ret == SOCKET_ERROR) 
			{
				err_display("send()");
				break;
			}

			DisplayText("[client] login try.... %s \r\n", &input);
			DisplayText("[client] login %d byte    \r\n", ret);
			return true;
		}
		case LOBBYLIST:
		{
			// ���� �ʱ�ȭ
			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// ��Ŷ �ٵ� ���� �� id ���� 
			PacketLayer::CS_LobbyList_Pkt packet;

			// ��� ���� �� ���ۿ� ���� 
			int curPos = 0;
			PacketLayer::PktHeader header{ sizeof(PacketLayer::CS_LobbyList_Pkt) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::CS_LOBBY_LIST };
			memcpy(&sendbuf[curPos], (char*)& header, sizeof(PacketLayer::PktHeader));

			//// �ٵ� ���ۿ� ����
			//curPos += sizeof(PacketLayer::PktHeader);
			//if (sizeof(PacketLayer::CS_Login_Pkt) > 0)
			//	memcpy(&sendbuf[curPos], (char*)& packet, sizeof(PacketLayer::CS_Login_Pkt));


			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::CS_LobbyList_Pkt) + sizeof(PacketLayer::PktHeader), 0);

			if (ret == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
			DisplayText("[client] Lobby List Call %d byte    \r\n", ret);
			return true;
		}
		case LOBBYENTER:
		{
			short i = SendMessage(hLobbyListBox, LB_GETCURSEL, 0, 0);

			char sendbuf[MAX_PACKET_SIZE] = { 0, };



			// ��Ŷ �ٵ� 
			PacketLayer::CS_LobbyEnter_Pkt packet;
			packet.lobbyIdBySelected = i;



			// ��� ���� �� ���ۿ� ���� 
			PacketLayer::PktHeader header{ sizeof(PacketLayer::CS_LobbyEnter_Pkt) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::CS_LOBBY_ENTER};
			memcpy(&sendbuf[0], (char*)& header, sizeof(PacketLayer::PktHeader));



			// �ٵ� ��� �ڿ��ٰ� ����
			if (sizeof(PacketLayer::CS_LobbyEnter_Pkt) > 0)
				memcpy(&sendbuf[sizeof(PacketLayer::PktHeader)], (char*)& packet, sizeof(PacketLayer::CS_LobbyEnter_Pkt));

			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::CS_LobbyEnter_Pkt) + sizeof(PacketLayer::PktHeader), 0);




			if (ret == SOCKET_ERROR) {
				err_display("send()");
				break;
			}

			DisplayText("[Client] [%d]Lobby Enter Request.....\r\n", i);
			return true;
		}
		case LOBBY_CHAT:
		{
			char input[input_size] = { 0, };
			GetDlgItemText(hwnd, LOBBY_CHAT_INPUT, (LPSTR)input, input_size);

			// ���� �ʱ�ȭ
			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// ��Ŷ �ٵ� ���� �� id ���� 
			PacketLayer::CS_Lobby_Chat_Pkt packet;
			strcpy(packet.msg, input);

			// ��� ���� �� ���ۿ� ���� 
			int curPos = 0;
			PacketLayer::PktHeader header{ sizeof(PacketLayer::CS_Lobby_Chat_Pkt) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::CS_LOBBY_CHAT };
			memcpy(&sendbuf[curPos], (char*)& header, sizeof(PacketLayer::PktHeader));

			// �ٵ� ���ۿ� ����
			curPos += sizeof(PacketLayer::PktHeader);
			if (sizeof(PacketLayer::CS_Lobby_Chat_Pkt) > 0)
				memcpy(&sendbuf[curPos], (char*)& packet, sizeof(PacketLayer::CS_Lobby_Chat_Pkt));


			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::CS_Lobby_Chat_Pkt) + sizeof(PacketLayer::PktHeader), 0);

			if (ret == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			// ���⼭ ���� 190512 
			DisplayText("[Client] chat send In lobby .....\r\n");
			return true;
		}
			
		}
		break;

	case WM_DESTROY:
		DeleteObject(g_hbrBackground);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	
	// ���� ��� ������ ����
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}


DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = WSAConnect(sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);

	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ������ ���
	while (1) {

		// ������ ������
		int retval = 0;


		char recvBuf[MAX_PACKET_SIZE] = { 0, };
		//ZeroMemory(recvBuf, sizeof(recvBuf));


		// ������ �ޱ�
		retval = recv(sock, recvBuf, 1024, 0);

		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;



		// ������ ó�� 
		auto readPos = 0;
		PacketLayer::PktHeader * pPktHeader;
		pPktHeader = (PacketLayer::PktHeader*) & recvBuf[readPos];

		readPos += sizeof(PacketLayer::PktHeader);


		switch (pPktHeader->Id)
		{
		case (int)PacketLayer::PACKET_ID::SC_LOGIN:
		{
			DisplayText("[server] login ....");
			break;
		}
		case (int)PacketLayer::PACKET_ID::SC_LOBBY_LIST:
		{
			PacketLayer::SC_LobbyList_Pkt packet;

			memcpy(&packet, &recvBuf[readPos], sizeof(PacketLayer::SC_LobbyList_Pkt));
			
			DisplayText("[server] lobby total count -> %hd \r\n",packet.LobbyCount);

			for (int i = 0; i < packet.LobbyCount; i++)
			{
				char str[10] = { 0, };
				//sprintf(str, "%d", packet.LobbyList[i].LobbyID);
				sprintf(str, "%d", i);
				SendMessage(hLobbyListBox, LB_ADDSTRING, NULL, (LPARAM)str);
			}
			break;
		}
		case (int)PacketLayer::PACKET_ID::SC_LOBBY_ENTER:
		{
			PacketLayer::SC_LobbyEnter_Pkt packet;

			memcpy(&packet, &recvBuf[readPos], sizeof(PacketLayer::SC_LobbyEnter_Pkt));

			DisplayText("[server] lobby  -> %s \r\n", packet.msg);
			break;
		}
		case (int)PacketLayer::PACKET_ID::SC_LOBBY_CHAT:
		{



			DisplayText("[server] chat!!!    \r\n");
			break;
		}

		}



		//  SetEvent(hReadEvent); // �б� �Ϸ� �˸���
	}

	return 0;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


void DisplayText(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hPrint);
	SendMessage(hPrint, EM_SETSEL, nLength, nLength);
	SendMessage(hPrint, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

void PacketProcess(char* recvbuf)
{

}