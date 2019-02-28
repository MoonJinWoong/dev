#pragma once



// define ��ſ� const�� ���(������ ���� �ٽ� ���� �� ��)
const int MAX_WORKER_THREAD = 8;
const int MAX_PROCESS_TRHEAD = 1;



typedef struct INITCONFIG
{
	//  member ���� ���̹��� ����ü �ձ��� ���� s�� 

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
	DWORD				sProcessPacketCnt;	//�ִ� ó�� ��Ŷ�� ����
	DWORD				sServerPort;
	DWORD				sWorkerThreadCnt;	//i/o ó���� ���� thread ����
	DWORD				sProcessThreadCnt;	//��Ŷó���� ���� thread ����

}INITCONFIG;
