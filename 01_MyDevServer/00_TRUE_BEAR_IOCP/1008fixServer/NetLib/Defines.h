#pragma once







const int MAX_IP_LENGTH = 20;
const int MAX_ADDR_LENGTH = 64;

const int SESSION_MAX_SEND_BUFF = 65536;
const int SESSION_MAX_RECV_BUFF = 65536;
const int MAX_RECV_OVERLAPPED_BUFF_SIZE = 512;
const int MAX_SEND_OVERLAPPED_BUFF_SIZE = 512;

const int MAX_SESSION_COUNT = 1000;
const int MAX_MESSAGE_POOL_COUNT = 1000;

// CPU�� ���� �����ؾ���. ���߿� �׽�Ʈ�غ� �� �ǵ帮��
const int WORKER_THREAD_COUNT = 4;

// GQCSEx���� �ѹ��� � �����ðų�
static const int MAX_EVENT_COUNT = 1000;  

// TODO : ���� �����غ���
const int MAX_MSG_POOL_COUNT = 100;
const int EXTRA_MSG_POOL_COUNT = 100;


const int PACKET_HEADER_LENGTH = 5;
const int PACKET_SIZE_LENGTH = 2;
const int PACKET_TYPE_LENGTH = 2;