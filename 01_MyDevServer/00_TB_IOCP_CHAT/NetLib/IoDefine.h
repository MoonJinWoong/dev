#pragma once
#include <atomic>

enum class IO_TYPE
{
	RECV,
	SEND,
	ACCEPT
};

//EXPLAIN  ������ ����� �� ���� Io ó���� ���� ����
//�� Ŭ������ ���Ƴֱ� �Ⱦ �и� ��Ŵ
class IoReference
{
	void IncRecvCount() { mRecvIoCount.fetch_add(1); }
	void IncSendCount() { mSendIoCount.fetch_add(1); }
	void IncAcptCount() { mAcptIoCount.fetch_add(1); }

	void DecRecvCount() { mRecvIoCount.fetch_sub(1); }
	void DecSendCount() { mSendIoCount.fetch_sub(1); }
	void DecAcptCount() { mAcptIoCount.fetch_sub(1); }

	int GetRecvIoCount() const { return mRecvIoCount.load(); }
	int GetSendIoCount() const { return mSendIoCount.load(); }
	int GetAcptIoCount() const { return mAcptIoCount.load(); }

private:
	//NOTE std::atomic -> 8����Ʈ ������ ��ü���� ����. �Ѿ�� ���� �Ǵ�. 
	std::atomic<int>  mRecvIoCount = 0;
	std::atomic<int>  mSendIoCount = 0;
	std::atomic<int>  mAcptIoCount = 0;
};