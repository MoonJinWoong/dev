#pragma once



// define 대신에 const로 사용(이유에 대해 다시 공부 할 것)
const int MAX_WORKER_THREAD = 8;
const int MAX_PROCESS_TRHEAD = 1;



typedef struct INITCONFIG
{
	//  member 변수 네이밍은 구조체 앞글자 따서 s로 

	INITCONFIG()
	{
		ZeroMemory(this, sizeof(INITCONFIG));
	}

	DWORD				sIndex;				//Connection index
	SOCKET			sListenSocket;		//Listen Socket
	//recv Ringbuffer size  = sRecvBufCnt * sRecvBufSize
	DWORD				sRecvBufCnt;
	//send Ringbuffer size  = sSendBufCnt * sSendBufSize
	DWORD				sSendBufCnt;
	DWORD				sRecvBufSize;
	DWORD				sSendBufSize;
	DWORD				sProcessPacketCnt;	//최대 처리 패킷의 개수
	DWORD				sServerPort;
	DWORD				sWorkerThreadCnt;	//i/o 처리를 위한 thread 개수
	DWORD				sProcessThreadCnt;	//패킷처리를 위한 thread 개수

}INITCONFIG;
