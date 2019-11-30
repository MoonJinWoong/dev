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

bool Client::SetPacketProc(u_Int input_size, char* input_data)
{
	// recv 받을때마다 버퍼에 Write
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



	u_int remainByte = mWritePos - mReadPos;
	if (remainByte < sizeof(PacketFrame))
	{
		return false;  // 아직 충분히 다 못받았을 때
	}

	return true;
}

PacketFrame Client::GetPacketProc()
{
	// 스레드에서 계속 이함수를 호출하면서 
	// 충분히 Write가 되었을때 패킷을 조립해서 리턴한다. 
	// Read는 증가시키고 기억한다.
	u_int remainByte = mWritePos - mReadPos;
	if (remainByte < sizeof(PacketFrame))
	{
		return PacketFrame();
	}

	// 패킷 앞만 따서 토탈 길이랑 타입만 파싱한다.
	auto pHeader = (PKT_HEADER*)&pBuffer[mReadPos];
	if (pHeader->packet_len > remainByte) //packet_len 은 해당 패킷의 토탈 길이다.
	{
		return PacketFrame();
	}


	// 조립
	PacketFrame packet;
	packet.packet_type = pHeader->packet_type;
	packet.size = pHeader->packet_len;
	packet.pData = &pBuffer[mReadPos];
	mReadPos += pHeader->packet_len;

	return packet;
}
