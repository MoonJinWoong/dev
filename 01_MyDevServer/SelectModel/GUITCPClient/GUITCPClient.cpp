#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

// 받은 패킷 처리하는 함수 
void PacketProcess(char* recvbuf);


SOCKET sock; // 소켓
char buf[BUFSIZE + 1]; // 데이터 송수신 버퍼
char recvbuf[BUFSIZE + 1]; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hLoginButton; // 로그인 버튼
HWND hEdit1, hEdit2; // 편집 컨트롤
HWND hEdit3, hEdit4;  // 로그인 편집 컨트롤
HWND hLobbyPrint;
HWND hLobbyEnter;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // 이벤트 생성
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;

    // 소켓 통신 스레드 생성
    CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

    // 대화상자 생성
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

    // 이벤트 제거
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG:
        hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);  // 채팅 메세지 입력란
        hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);  //  출력창

		hEdit3 = GetDlgItem(hDlg, INPUT_LOGIN);  // id 입력란

        hSendButton = GetDlgItem(hDlg, IDOK);   // 채팅 보내기 버튼
		hLoginButton = GetDlgItem(hDlg, IDLOGIN);  // 로그인 버튼


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
			//// 패킷 바디 세팅
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
			//DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다.\r\n", ret);
			////SetEvent(hWriteEvent); // 쓰기 완료 알리기
			//SetFocus(hEdit1);
			//SendMessage(hEdit1, EM_SETSEL, 0, -1);


			return TRUE;
		}
		case IDLOGIN:
		{
			// 로그인 버튼을 눌렀을 때 string을 가져옴
			GetDlgItemText(hDlg,INPUT_LOGIN , buf, BUFSIZE + 1);

			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// 패킷 바디 선언 및 id 세팅 
			PacketLayer::PktLogInReq packet;
			strcpy(packet.szID, buf);



			// 헤더 세팅 후 버퍼에 세팅 
			PacketLayer::PktHeader header{ sizeof(packet) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::LOGIN_IN_REQ };

			memcpy(&sendbuf[0], (char*)& header, sizeof(PacketLayer::PktHeader));



			// 바디를 버퍼에 세팅
			if (sizeof(PacketLayer::PktLogInReq) > 0)
				memcpy(&sendbuf[sizeof(PacketLayer::PktHeader)], (char*)& packet, sizeof(PacketLayer::PktLogInReq));
			

			// send
			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::PktLogInReq) + sizeof(PacketLayer::PktHeader), 0);


			if (ret == SOCKET_ERROR) {
				err_display("send()");
				break;
			}
			DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다.\r\n", ret);
			//SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);


		//	char buf[BUFSIZE + 1] = { 0, };
			return true;
		}
		case LOBBYLIST:
		{
			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// 패킷 바디 세팅 
			PacketLayer::PktLobbyListReq packet;


			// 헤더 세팅
			PacketLayer::PktHeader header{ sizeof(packet) + sizeof(PacketLayer::PktHeader)
				, (int)PacketLayer::PACKET_ID::LOBBY_LIST_REQ };
			memcpy(&sendbuf[0], (char*)& header, sizeof(PacketLayer::PktHeader));



			auto ret = send(sock, (char*)& sendbuf,
				sizeof(PacketLayer::PktLogInReq) + sizeof(PacketLayer::PktHeader), 0);



			DisplayText("[TCP 클라이언트] 로비 리스트 요청.\r\n");
			break;
		}
		case LOBBY_ENTER:
		{
			// 현재 선택된 텍스트를 가져옴
			int i = SendMessage(hLobbyPrint, LB_GETCURSEL, 0, 0);

			char sendbuf[MAX_PACKET_SIZE] = { 0, };

			// 패킷 바디 세팅 
			PacketLayer::PktLobbyEnterReq packet;
			packet.selectedLobbyID = i;

			// 헤더 세팅
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

// 편집 컨트롤 출력 함수
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

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// 사용자 정의 데이터 수신 함수
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

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;

    // 윈속 초기화
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
	

    // 서버와 데이터 통신
    while (1) {
 
		// 데이터 보내기
		int retval = 0;


		char recvBuf[MAX_PACKET_SIZE] = { 0, };
		//ZeroMemory(recvBuf, sizeof(recvBuf));


        // 데이터 받기
        retval = recv(sock, recvBuf, 1024, 0);
		
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;



		// 받은거 처리 
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

			// 받은 데이터 출력
			recvBuf[retval] = '\0';
			DisplayText("[TCP 클라이언트] %d바이트를 받았습니다.\r\n", retval);
			DisplayText("[받은 데이터] %s\r\n", bodyData.msg);

			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			break;
		}
		case (int)PacketLayer::PACKET_ID::LOBBY_LIST_RES:
		{
			readPos += sizeof(PacketLayer::PktHeader);
			auto bodySize = pPktHeader->TotalSize - sizeof(PacketLayer::PktHeader);
			
			PacketLayer::PktLobbyListRes bodyData;

			memcpy(&bodyData.LobbyCount, (short*)& recvBuf[readPos], 2);

			readPos += 2;


			// max만큼 돌면 다뿌린다. 그래서 받은거만 뿌려주자
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



      //  SetEvent(hReadEvent); // 읽기 완료 알리기
    }

    return 0;
}

void PacketProcess(char* recvbuf)
{
	// 패킷 바디 선언 및 id 세팅 
	PacketLayer::PktLogInRes packet;
	strcpy(packet.msg, recvbuf);


}
