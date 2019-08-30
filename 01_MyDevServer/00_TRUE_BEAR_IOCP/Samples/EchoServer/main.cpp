#include "..//IocpNetLib/precompile.h"



#define	MAX_BUFFER		1024
void WorkerThread(void* iocp);


struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};



int main()
{

	SYSTEM_INFO sys;
	GetSystemInfo(&sys);
	auto threadCnt = sys.dwNumberOfProcessors * 2; // 추후 조정해서 테스트

	WSADATA   wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret != NO_ERROR) 
	{
		cout << "[ERR]WSAStartup Error - " << ret << endl;
	}

	// create iocp 
	IocpService iocp(threadCnt);
	
	// 소켓 생성 , bind , listen
	NetworkService networkObj;
	networkObj.bindAndListen( 8000);

	// iocp 등록
	iocp.resister(networkObj);


	// 비동기 accept를 하겠다.
	networkObj.initAccpetEx();


	printf_s("[SERVER][%s] Start.......\n", __FUNCTION__);

	// overlapped accept할 것들을 전부 걸어놓고 풀을 만들어둔다.


	// GClientSessionManager
	while (GClientSessionManager->AcceptClientSessions())
	{
		Sleep(32);
	}

	// AcceptClientSessions
	while (mCurrentIssueCount - mCurrentReturnCount < m_MaxSessionCount)
	{
		ClientSession* newClient = mFreeSessionList.back();
		mFreeSessionList.pop_back();

		++mCurrentIssueCount;

		if (false == newClient->PostAccept()) {
			return false;
		}
	}


	// PostAccept
	OverlappedAcceptContext* acceptContext = new OverlappedAcceptContext(this);
	DWORD bytes = 0;
	DWORD flags = 0;
	acceptContext->mWsaBuf.len = 0;
	acceptContext->mWsaBuf.buf = nullptr;

	if (FALSE == AcceptEx(*GIocpManager->GetListenSocket(), mSocket, GIocpManager->mAcceptBuf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPOVERLAPPED)acceptContext))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			DeleteIoContext(acceptContext);
			printf_s("AcceptEx Error : %d\n", GetLastError());

			return false;
		}
	}


	//
	// 테스트 코드 따로 클래스 만들어서 뺄예정
	// thread 
	// WorkerThread 핸들 전달부터 다시...... 
	// GQCS가 동작을 안함 - > accept를 안했는데 될리가? 

	printf_s("[INFO] Worker Thread 시작...\n");
	vector<thread> workers;
	for (int i = 0; i < threadCnt; i++) 
		workers.push_back(thread(WorkerThread,iocp.mIocpHanle));
	
	for (int i = 0; i < threadCnt; i++)
		workers[i].join();
	

	
	return 0;
}

void WorkerThread(void* iocp)
{
printf_s("ssssss\n");

	// 함수 호출 성공 여부
	BOOL	bResult;
	int		nResult;
	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	recvBytes;
	DWORD	sendBytes;
	// Completion Key를 받을 포인터 변수
	stSOCKETINFO* pCompletionKey;
	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	stSOCKETINFO* pSocketInfo;
	// 
	DWORD	dwFlags = 0;

	while (true)
	{

		bResult = GetQueuedCompletionStatus(iocp,
			&recvBytes,				// 실제로 전송된 바이트
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,			// overlapped I/O 객체
			INFINITE				// 대기할 시간
		);
		printf_s("ssssss\n");


		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김\n", pSocketInfo->socket);
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}
		else
		{
			printf_s("[INFO] 메시지 수신- Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// 클라이언트의 응답을 그대로 송신			
			nResult = WSASend(
				pSocketInfo->socket,
				&(pSocketInfo->dataBuf),
				1,
				&sendBytes,
				dwFlags,
				NULL,
				NULL
			);

			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				printf_s("[ERROR] WSASend 실패 : ", WSAGetLastError());
			}

			printf_s("[INFO] 메시지 송신 - Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// stSOCKETINFO 데이터 초기화
			ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
			pSocketInfo->dataBuf.len = MAX_BUFFER;
			pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
			ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
			pSocketInfo->recvBytes = 0;
			pSocketInfo->sendBytes = 0;

			dwFlags = 0;

			// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
			nResult = WSARecv(
				pSocketInfo->socket,
				&(pSocketInfo->dataBuf),
				1,
				&recvBytes,
				&dwFlags,
				(LPWSAOVERLAPPED) & (pSocketInfo->overlapped),
				NULL
			);

			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				printf_s("[ERROR] WSARecv 실패 : ", WSAGetLastError());
			}
		}
	}
}

