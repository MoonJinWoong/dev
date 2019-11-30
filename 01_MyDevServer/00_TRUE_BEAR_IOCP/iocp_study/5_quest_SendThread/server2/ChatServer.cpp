#include "ChatServer.h"


void ChatServer::OnAccept(u_Int unique_id)
{
	// �����Ҷ����� ť�� �о�ִ´�.
	auto id = unique_id;
	std::cout << "[OnAccept] UniqueID : " << id << std::endl;
	PacketFrame packet{ id ,(int)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnClose(u_Int unique_id)
{
	// ������ ���Ӳ��������� ť�� �о�ִ´�.
	PacketFrame packet{ unique_id ,(int)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnRecv(CustomOverEx *pOver, u_Int ioSize)
{
	RemoteSession* session = GetSessionByIdx(pOver->mUid);
	if (session == nullptr)
	{
		return;
	}

	session->mOverEx.mCurrRecvPos += ioSize;
	if (session->mOverEx.mCurrRecvPos > MAX_SOCKBUF)
	{
		session->mOverEx.mCurrRecvPos = 0;
	}

	pOver->m_wsaBuf.buf = session->mOverEx.m_RecvBuf.data();
	pOver->mRemainByte += ioSize;
	
	auto& remain = pOver->mRemainByte;
	auto pBuf = pOver->m_wsaBuf.buf;

	// ��Ŷ ��� ����, ��Ŷ ��ü ����
	const int HEAD_LEN = sizeof(PKT_HEADER);
	const int PKT_TOTAL_LEN = 2;
	const int PKT_TYPE_LEN = 2;

	short packetSize = 0;

	while (true)
	{
		if (remain < HEAD_LEN)
		{
			break;
		}

		// ��Ŷ ������� ��Ż ���� ����
		CopyMemory(&packetSize, pBuf, PKT_TOTAL_LEN);
		auto currentSize = packetSize;

		if (packetSize <= 0 || packetSize > MAX_SOCKBUF)
		{
			return;
		}

		// �������� ������.
		if (remain >= (DWORD)currentSize)
		{
			mLogicProc->RecvPktData(session->GetUniqueId(), pBuf , currentSize);
			remain -= currentSize;
			pBuf += currentSize;
		}
		else
		{
			break;
		}
	}

	// recv IO
	// 1130 ���⼭���� �ٽ�
	session->RecvFinish();
}



void ChatServer::Run(u_Int maxClient)
{
	// logic���� Send�� �θ��� ����
	auto SendFunc = [&](u_Int index, int size, char* pPacket)
	{
		SendMsg(index, size, pPacket);
	};

	mLogicProc = std::make_unique<LogicMain>();
	mLogicProc->SendPacketFunc = SendFunc;
	mLogicProc->Init(maxClient);
	mLogicProc->Start();
	StartNetService(maxClient);
}

void ChatServer::Stop()
{
	mLogicProc->Stop();
	DestroyThread();
}

