#pragma once
#include "Defines.h"
#include "SpinLock.h"


class CustomBuffer
{
public:
	CustomBuffer(int size) :mBufferSize(size){}
	~CustomBuffer() { if (mBeginPos != nullptr) delete[] mBeginPos; }

	bool Init()
	{
		SpinLockGuard Lock(&m_CS);
		
		{
			mBufferSize = 0; 
			mUedBufferSize = 0; 
			mCurrPos = mBeginPos;
			mReadPos = mBeginPos;
			mLastSavePos = mEndPos;
		}

		return true;
	}
	bool Create()
	{
		if (mBeginPos != nullptr)	delete[] mBeginPos;
		
		// begin setting
		mBeginPos = new char[mBufferSize];
		if (mBeginPos == nullptr)
		{
			std::cout << "mBegin alloc fail" << std::endl;
			return false;
		}

		// end setting
		mEndPos = mBeginPos + mBufferSize - 1;
		

		Init();
		return true;
	}

	// 할당된 크기 반환
	int GetBufferSize() const { return mBufferSize; }
	int GetUsedBufferLen() { return mUedBufferSize; }		  // 사용된 양
	int GetTotalBufferLen() { return mTotalUsedBufferSize; }  // 누적 버퍼 사용량


	// 내부 버퍼 포인터 반환 - 시작점 ,현재, 끝점
	char* GetBeginPos() const { return mBeginPos; }
	char* GetCurrPos() const { return mCurrPos; }
	char* GetEndPos() const { return mEndPos; }



	// send 용 전진(길이를 알 수 있음)
	char* CommitForSend(int len)
	{
		char* preCurrPos = NULL;
		SpinLockGuard Lock(&m_CS);
		{
			if ((mUedBufferSize + len) > mBufferSize) return NULL;

			if ((mEndPos - mCurrPos) >= len)
			{
				// 전진
				preCurrPos = mCurrPos;
				mCurrPos += len;
			}
			else   // 순환해야 되는 경우
			{
				mLastSavePos = mCurrPos;
				mCurrPos = mBeginPos + len;
				preCurrPos = mBeginPos;
			}
			mUedBufferSize += len;
			mTotalUsedBufferSize += len;
		}
		return preCurrPos;
	}


	// recv용 전진 (길이 모름)
	char* CommitForRecv(int len,int nextLen,unsigned long remainLen)
	{
		char* preCurrPos = NULL;
		SpinLockGuard Lock(&m_CS);
		{
			if ((mUedBufferSize + len + nextLen) > mBufferSize) return NULL;

			if ((mEndPos - mCurrPos) > (nextLen + len))
			{
				mCurrPos += len;
			}
			else // 순환해야 하는 경우 
			{
				mLastSavePos = mCurrPos;
				CopyMemory(mBeginPos, mCurrPos - (remainLen - len), remainLen);
				mCurrPos = mBeginPos + remainLen;
			}
			mUedBufferSize += len;
			mTotalUsedBufferSize += len;

		}
		return mCurrPos;
	}
	// 사용된 내부 버퍼 해제
	void Release(int len)
	{
		SpinLockGuard Lock(&m_CS);
		{
			mUedBufferSize -= len;
		}
	}



	// 내부 버퍼 읽어서 반환
	char* GetBuffer(int readLen, int& readPtr)
	{
		char* pResult = nullptr;
		if (mLastSavePos == mReadPos)
		{
			mReadPos = mBeginPos;
			mLastSavePos = mEndPos;
		}

		if (mUedBufferSize > readLen)
		{
			if ((mLastSavePos - mReadPos) >= readLen)
			{
				readPtr = readLen;
			}
			else
			{
				readPtr = static_cast<int>(mLastSavePos - mReadPos);
			}

			pResult = mReadPos;
			mReadPos += readPtr;
		}
		else if (mReadPos > 0)
		{
			if ((mLastSavePos - mReadPos) >= mUedBufferSize)
			{
				readPtr = mUedBufferSize;
				pResult = mReadPos;
				mReadPos += mUedBufferSize;
			}
			else
			{
				readPtr = static_cast<int>(mLastSavePos - mReadPos);
				pResult = mReadPos;
				mReadPos += readPtr;
			}
		}

		return pResult;

	}

private:
	CustomSpinLockCriticalSection m_CS;

	char* mData = nullptr;    // 실제로 데이터를 저장하는 버퍼
	char* mBeginPos = nullptr;  // 버퍼 시작 지점
	char* mEndPos = nullptr;		// 버퍼 끝 지점
	char* mCurrPos = nullptr;    // 버퍼 현재 사용되는 부분 가리키고 있는 지점
	char* mReadPos = nullptr;    // 읽은 버퍼
	char* mLastSavePos = nullptr;    // 순환하기전 마지막 포인터

	int mBufferSize =0; // 내부 버퍼 크기
	int mUedBufferSize =0; // 사용된 크기 
	int mTotalUsedBufferSize =0;  // 총처리된 데이터

};