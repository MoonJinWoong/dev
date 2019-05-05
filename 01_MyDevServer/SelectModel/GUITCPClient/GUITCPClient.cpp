#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"
#include "../PacketDefine/Packet.h"
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512
#define	WM_SOCKET				WM_USER + 1

const int MAX_PACKET_SIZE = 1024;
const int MAX_SOCK_RECV_BUFFER = 8016;

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);

// ���� ��Ŷ ó���ϴ� �Լ� 
void PacketProcess(char* recvbuf);


SOCKET sock; // ����
char buf[BUFSIZE + 1]; // ������ �ۼ��� ����
char recvbuf[BUFSIZE + 1]; // ������ �ۼ��� ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ
HWND hSendButton; // ������ ��ư
HWND hLoginButton; // �α��� ��ư
HWND hEdit1, hEdit2; // ���� ��Ʈ��
HWND hEdit3, hEdit4;  // �α��� ���� ��Ʈ��
HWND hLobbyPrint;
HWND hLobbyEnter;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // �̺�Ʈ ����
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;

    // ���� ��� ������ ����
    CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

    // ��ȭ���� ����
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

    // �̺�Ʈ ����
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // ���� ����
    WSACleanup();
    return 0;
}

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG:
        hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);  // ä�� �޼��� �Է¶�
        hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);  //  ���â

		hEdit3 = GetDlgItem(hDlg, INPUT_LOGIN);  // id �Է¶�

        hSendButton = GetDlgItem(hDlg, IDOK);   // ä�� ������ ��ư
		hLoginButton = GetDlgItem(hDlg, IDLOGIN);  // �α��� ��ư


		hLobbyPrint = GetDlgItem(hDlg, LOBBY_PRINT);
		hLobbyEnter = GetDlgItem(hDlg, LOBBY_ENTER);

		SendMessage(hEdit3, EM_SETLIMITTEXT, BUFSIZE, 0);
        SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		
        return TRUE;


    case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		{
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);

			//char sendbuf[MAX_PACKET_SIZE] = { 0, };
			//
			//// ��Ŷ �ٵ� ����
			//Packet_Login_CtoS packet;
			////ZeroMemory(packet, sizeof(Packet_Login_CtoS));
			//strcpy(packet.szID, buf);
			//strcpy(packet.szPW, buf);

			//PacketHead header{sizeof(packet)+ PACKET_HEADER_SIZE, 11 };
			//memcpy(&sendbuf[0], (char*)&header, PACKET_HEADER_SIZE);

			//if (sizeof(Packet_Login_CtoS) > 0)
			//{
			//	memcpy(&sendbuf[PACKET_HEADER_SIZE], (char*)&packet, sizeof(Packet_Login_CtoS));
			//}

			//auto ret = send(sock,(char*)& sendbuf,
			//	sizeof(Packet_Login_CtoS) + PACKET_HEADER_SIZE, 0);


			//if (ret == SOCKET_ERROR) {
			//	err_display("send()");
			//	break;
			//}
			//DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\r\n", ret);
			////SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			//SetFocus(hEdit1);
			//SendMessage(hEdit1, EM_SETSEL, 0, -1);


			return TRUE;
		}
		case IDLOGIN:
		{
			// �α��� ��ư�� ������ �� string�� ������
			GetDlgItemText(hDlg,INPUT_LOGIN , buf, BUFSIZE + 1);

			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// ��Ŷ �ٵ� ���� �� id ���� 
			PacketLayer::PktLogInReq packet;
			strcpy(packet.szID, buf);



			// ��� ���� �� ���ۿ� ���� 
			PacketLayer::PktHeader header{ sizeof(packet) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::LOGIN_IN_REQ };

			memcpy(&sendbuf[0], (char*)& header, sizeof(PacketLayer::PktHeader));



			// �ٵ� ���ۿ� ����
			if (sizeof(PacketLayer::PktLogInReq) > 0)
				memcpy(&sendbuf[sizeof(PacketLayer::PktHeader)], (char*)& packet, sizeof(PacketLayer::PktLogInReq));
			

			// send
			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::PktLogInReq) + sizeof(PacketLayer::PktHeader), 0);


			if (ret == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\r\n", ret);
			//SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);


		//	char buf[BUFSIZE + 1] = { 0, };
			return true;
		}
		case LOBBYLIST:
		{
			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// ��Ŷ �ٵ� ���� 
			PacketLayer::PktLobbyListReq packet;


			// ��� ����
			PacketLayer::PktHeader header{ sizeof(packet) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::LOBBY_LIST_REQ };
			memcpy(&sendbuf[0], (char*)& header, sizeof(PacketLayer::PktHeader));



			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::PktLogInReq) + sizeof(PacketLayer::PktHeader), 0);



			DisplayText("[TCP Ŭ���̾�Ʈ] �κ� ����Ʈ ��û.\r\n");
			break;
		}
		case LOBBY_ENTER:
		{
			// ���� ���õ� �ؽ�Ʈ�� ������
			int i = SendMessage(hLobbyPrint, LB_GETCURSEL, 0, 0);

			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// ��Ŷ �ٵ� ���� 
			PacketLayer::PktLobbyEnterReq packet;
			packet.selectedLobbyID = i;

			// ��� ����
			PacketLayer::PktHeader header{ sizeof(packet) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::LOBBY_ENTER_REQ };
			memcpy(&sendbuf[0], (char*)& header, sizeof(PacketLayer::PktHeader));



			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::PktLobbyEnterReq) + sizeof(PacketLayer::PktHeader), 0);
				
			DisplayText("[Client ->Server] [%d]Lobby Enter Request.....\r\n", i);
			break;
			
				
		}
		case IDCANCEL:
		{
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		}
        return FALSE;

	case WM_SOCKET:
		printf("socket event start1111");
		return TRUE;
    }
    return FALSE;
}

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(hEdit2);
    SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
    SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
    int received;
    char *ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

// TCP Ŭ���̾�Ʈ ���� �κ�
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
    retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr),NULL,NULL,NULL,NULL);
	
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
		PacketLayer::PktHeader* pPktHeader;
		pPktHeader = (PacketLayer::PktHeader*)& recvBuf[readPos];

		switch (pPktHeader->Id)
		{
		case (int)PacketLayer::PACKET_ID::LOGIN_IN_RES:
		{
			readPos += sizeof(PacketLayer::PktHeader);
			auto bodySize = pPktHeader->TotalSize - sizeof(PacketLayer::PktHeader);
			PacketLayer::PktLogInRes bodyData;
			memcpy(&bodyData.msg[0], (char*)& recvBuf[readPos], bodySize);

			// ���� ������ ���
			recvBuf[retval] = '\0';
			DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\r\n", retval);
			DisplayText("[���� ������] %s\r\n", bodyData.msg);

			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			break;
		}
		case (int)PacketLayer::PACKET_ID::LOBBY_LIST_RES:
		{
			readPos += sizeof(PacketLayer::PktHeader);
			auto bodySize = pPktHeader->TotalSize - sizeof(PacketLayer::PktHeader);
			
			PacketLayer::PktLobbyListRes bodyData;

			memcpy(&bodyData.LobbyCount, (short*)& recvBuf[readPos], 2);

			readPos += 2;


			// max��ŭ ���� �ٻѸ���. �׷��� �����Ÿ� �ѷ�����
			for (int i = 0; i < bodyData.LobbyCount; i++)
			{
				memcpy(&bodyData.LobbyList[i].LobbyId, (short*)& recvBuf[readPos], 2);
				readPos += 2;
				memcpy(&bodyData.LobbyList[i].LobbyMaxUserCount, (short*)& recvBuf[readPos], 2);
				readPos += 2;
				memcpy(&bodyData.LobbyList[i].LobbyUserCount, (short*)& recvBuf[readPos], 2);
				readPos += 2;

				char str[10];
				sprintf(str, "%d", i);
				SendMessage(hLobbyPrint, LB_ADDSTRING, NULL, (LPARAM)str);
			}


			break;
		}

		}



      //  SetEvent(hReadEvent); // �б� �Ϸ� �˸���
    }

    return 0;
}

void PacketProcess(char* recvbuf)
{
	// ��Ŷ �ٵ� ���� �� id ���� 
	PacketLayer::PktLogInRes packet;
	strcpy(packet.msg, recvbuf);


}
