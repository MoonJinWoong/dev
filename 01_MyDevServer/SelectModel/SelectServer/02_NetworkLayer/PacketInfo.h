//#pragma once
//
//
//#pragma push(1)
//struct Packet
//{
//	int UserID = 0;
//	short Operation = 0;
//	short bodySize = 0;
//	char data[512];
//};
//#pragma pop
//
//struct RecvPacketInfo
//{
//	int SessionIndex = 0;
//	short PacketId = 0;
//	short PacketBodySize = 0;
//	char* pRefData = 0;
//};
//
//
//namespace NetworkLayer
//{
//	// 21 이상부터 시작해야 한다!!!
//	enum class PACKET_ID : short
//	{
//		LOGIN_IN_REQ = 11,
//		LOGIN_IN_RES = 22
//
//		//MAX = 256
//	};
//
//}
//
