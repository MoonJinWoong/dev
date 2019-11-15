#include "Client.h"


void Client::Init(c_u_Int index)
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

void Client::SetPacketProc(c_u_Int input_size, char* input_data)
{
	// 현재까지 사용한 버퍼의 남은 공간보다 들어온 버퍼사이즈가 크다면
	// 처음으로 되돌아간다.
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

	// while 루프 
	if (remainByte < PKT_HEADER_LENGTH)
	{
		return PacketFrame();
	}

	auto pHeader = (PKT_HEADER*)&pBuffer[mReadPos];

	if (pHeader->packet_len > remainByte)
	{
		return PacketFrame();
	}

	PacketFrame packet;
	packet.packet_type = pHeader->packet_type;
	packet.size = pHeader->packet_len;
	packet.pData = &pBuffer[mReadPos];

	mReadPos += pHeader->packet_len;

	return packet;
}
