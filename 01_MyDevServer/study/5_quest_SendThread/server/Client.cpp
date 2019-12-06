#include "Client.h"


void Client::Init(u_Int index)
{
	mRemoteIdx = index;
	pBuffer = new char[PACKET_DATA_BUFFER_SIZE];
}

void Client::Reset()
{
	//mRoomIndex = -1;
	mNickName = "";
	//mIsConfirm = false;
	mState = USER_STATE::NONE;

	mReadPos = 0;
	mWritePos = 0;
}

int Client::SetLogin(std::string& nickname)
{
	mState = USER_STATE::LOGIN;
	mNickName = nickname;
	return 0;
}

void Client::SetPacketProc(u_Int input_size, char* input_data)
{
	// ������� ����� ������ ���� �������� ���� ���ۻ���� ũ�ٸ�
	// ó������ �ǵ��ư���.
	if ((mReadPos + input_size) >= PACKET_DATA_BUFFER_SIZE)
	{
		auto remainDataSize = mWritePos - mReadPos;
		
		if (remainDataSize > 0)
		{
			CopyMemory(&pBuffer[0], &pBuffer[mReadPos], remainDataSize);
			mWritePos = remainDataSize;
		}
		else
		{
			mWritePos = 0;
		}
		mReadPos = 0;
	}
	CopyMemory(&pBuffer[mWritePos], input_data, input_size);
	mWritePos += input_size;
}

PacketFrame Client::GetPacketProc()
{
	u_int remainByte = mWritePos - mReadPos;

	// ©���� ���°� ����غ��� �Ѵ�.
	if (remainByte < sizeof(PKT_HEADER))
	{
		return PacketFrame();
	}

	auto pHeader = (PKT_HEADER*)&pBuffer[mReadPos];

	if (pHeader->packet_len > remainByte) //packet_len �� �ش� ��Ŷ�� ��Ż ���̴�.
	{
		return PacketFrame();
	}


	// ���� �غ� �Ϸ�
	PacketFrame packet;
	packet.packet_type = pHeader->packet_type;
	packet.size = pHeader->packet_len;
	packet.pData = &pBuffer[mReadPos];

	mReadPos += pHeader->packet_len;


	return packet;
}
