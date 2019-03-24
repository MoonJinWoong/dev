
#define	MAX_BUFFER		1024
#define SERVER_PORT		9000
#define MAX_CLIENTS		100




// IOCP 소켓 구조체
enum OPTYPE { OP_SEND, OP_RECV };

struct OverlappedEx {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFFER];
	OPTYPE event_type;
};

struct CLIENT {

	bool isConnected;

	SOCKET socket;
	OverlappedEx recvOver;
	OverlappedEx sendOver;
	unsigned char packet_buf[MAX_BUFFER];
	int prev_packet_data;
	int curr_packet_size;
};