

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
	CLIENT		  * SocketInfo;		// ���� ����
	SOCKET			cListenSocket;	// ���� ���� ����
	HANDLE			cIocpHandle;			// IOCP ��ü �ڵ�
	bool			cAccept;		// ��û ���� �÷���
	bool			cWorkerThread;	// �۾� ������ ���� �÷���
	HANDLE *		cWorkerHandle;	// �۾� ������ �ڵ�		
	int				cThreadCnt;
};
