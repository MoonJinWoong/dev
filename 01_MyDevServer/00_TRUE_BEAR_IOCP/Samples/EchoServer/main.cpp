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
	auto threadCnt = sys.dwNumberOfProcessors * 2; // ���� �����ؼ� �׽�Ʈ

	WSADATA   wsadata;
	auto ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret != NO_ERROR) 
	{
		cout << "[ERR]WSAStartup Error - " << ret << endl;
	}

	// create iocp 
	IocpService iocp(threadCnt);
	
	// ���� ���� , bind , listen
	NetworkService networkObj;
	networkObj.bindAndListen( 8000);

	// iocp ���
	iocp.resister(networkObj);


	// �񵿱� accept�� �ϰڴ�.
	networkObj.initAccpetEx();


	printf_s("[SERVER][%s] Start.......\n", __FUNCTION__);

	// overlapped accept�� �͵��� ���� �ɾ���� Ǯ�� �����д�.


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
	// �׽�Ʈ �ڵ� ���� Ŭ���� ���� ������
	// thread 
	// WorkerThread �ڵ� ���޺��� �ٽ�...... 
	// GQCS�� ������ ���� - > accept�� ���ߴµ� �ɸ���? 

	printf_s("[INFO] Worker Thread ����...\n");
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

	// �Լ� ȣ�� ���� ����
	BOOL	bResult;
	int		nResult;
	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	recvBytes;
	DWORD	sendBytes;
	// Completion Key�� ���� ������ ����
	stSOCKETINFO* pCompletionKey;
	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSOCKETINFO* pSocketInfo;
	// 
	DWORD	dwFlags = 0;

	while (true)
	{

		bResult = GetQueuedCompletionStatus(iocp,
			&recvBytes,				// ������ ���۵� ����Ʈ
			(PULONG_PTR)& pCompletionKey,	// completion key
			(LPOVERLAPPED*)& pSocketInfo,			// overlapped I/O ��ü
			INFINITE				// ����� �ð�
		);
		printf_s("ssssss\n");


		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) ���� ����\n", pSocketInfo->socket);
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
			printf_s("[INFO] �޽��� ����- Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// Ŭ���̾�Ʈ�� ������ �״�� �۽�			
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
				printf_s("[ERROR] WSASend ���� : ", WSAGetLastError());
			}

			printf_s("[INFO] �޽��� �۽� - Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// stSOCKETINFO ������ �ʱ�ȭ
			ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
			pSocketInfo->dataBuf.len = MAX_BUFFER;
			pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
			ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
			pSocketInfo->recvBytes = 0;
			pSocketInfo->sendBytes = 0;

			dwFlags = 0;

			// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
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
				printf_s("[ERROR] WSARecv ���� : ", WSAGetLastError());
			}
		}
	}
}

