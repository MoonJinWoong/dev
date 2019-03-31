

class IocpServer
{
public:
	IocpServer();
	~IocpServer();

	void StartServer();
	bool InitServer();
	bool CreateThreads();

	void WorkerThread();
	void AcceptThread();


	void CloseServer();
	void DisConnect(int id);

	mutex lockObj;
	CLIENT		  * SocketInfo;		// 소켓 정보
	SOCKET			cListenSocket;	// 서버 리슨 소켓
	HANDLE			cIocpHandle;			// IOCP 객체 핸들
	bool			cAccept;		// 요청 동작 플래그
	bool			cWorkerThread;	// 작업 스레드 동작 플래그
	HANDLE *		cWorkerHandle;	// 작업 스레드 핸들		
	int				cThreadCnt;
};
