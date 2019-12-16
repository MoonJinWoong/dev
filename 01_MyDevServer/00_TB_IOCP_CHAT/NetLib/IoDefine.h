#pragma once
#include <atomic>

enum class IO_TYPE
{
	RECV,
	SEND,
	ACCEPT
};

//EXPLAIN  세션이 종료될 때 남은 Io 처리를 위해 만듦
//한 클래스에 몰아넣기 싫어서 분리 시킴
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
	//NOTE std::atomic -> 8바이트 이하의 객체여야 빠름. 넘어가면 락을 건다. 
	std::atomic<int>  mRecvIoCount = 0;
	std::atomic<int>  mSendIoCount = 0;
	std::atomic<int>  mAcptIoCount = 0;
};