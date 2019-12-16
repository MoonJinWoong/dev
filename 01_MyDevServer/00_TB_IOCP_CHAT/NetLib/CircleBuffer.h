#pragma once
#include <mutex>

class CircleBuffer
{
public:
	CircleBuffer(int size)
	{
		mTotalSize = size;
		mBuffer = new char[size];

		mReadPos = 0;
		mWritePos = 0;
	}
	~CircleBuffer()
	{
		delete[] mBuffer;
		mBuffer = nullptr;
	}

	void Init()
	{
		// 소멸자가 불린게 아님 memset만 해주자
		std::lock_guard<std::mutex> guard(mLock);
		memset(mBuffer, 0, mTotalSize);
		mReadPos = 0;
		mWritePos = 0;
	}

	char* GetWriteBuffer()
	{
		return &mBuffer[mWritePos];
	}

	int	SetWriteBuffer(char* pData, int size)
	{
		if (size > GetRemainSize())
		{
			size = GetRemainSize();
		}

		std::lock_guard<std::mutex> guard(mLock);
		for (int index = 0; index < size; ++index)
		{
			mBuffer[mWritePos] = pData[index];
			mWritePos = (mWritePos + 1) % mTotalSize;
		}

		return size;
	}

	// 연속으로 쓸 수 있는 사이즈
	int GetWriteAbleSize()
	{
		if (mReadPos > mWritePos)
		{
			return mReadPos - mWritePos;
		}
		else
		{
			return mTotalSize - mWritePos;
		}
	}

	// 연속으로 읽을 수 있는 사이즈
	int	GetReadAbleSize(void)
	{
		if (mReadPos <= mWritePos)
		{
			return mWritePos - mReadPos;
		}

		else
			return (mTotalSize - mReadPos) + mWritePos;
	}

	int	GetHeaderSize(char* pData, int size)
	{
		if (size > GetReadAbleSize())
		{
			size = GetReadAbleSize();
		}

		std::lock_guard<std::mutex> guard(mLock);
		for (int iCnt = 0; iCnt < size; iCnt++)
		{
			pData[iCnt] = mBuffer[(mReadPos + iCnt) % mTotalSize];
		}
		return size;
	}

	
	void MoveReadPos(int size)
	{
		if (GetRemainSize() < size)
		{
			size = GetRemainSize();
		}

		std::lock_guard<std::mutex> guard(mLock);
		mReadPos = (mReadPos + size) % mTotalSize;
	}

	int	MoveWritePos(int size)
	{
		if (GetRemainSize() < size)
		{
			size = GetRemainSize();
		}
		std::lock_guard<std::mutex> guard(mLock);
		mWritePos = (mWritePos + size) % mTotalSize;
		return size;
	}

	int	GetRemainSize()
	{
		if (mReadPos > mWritePos)
			return mReadPos - mWritePos;
		else
			return (mTotalSize - mWritePos) + mReadPos;
	}


	int GetWritePos() { return mWritePos; }
	char* GetBufferPtr(void) { return mBuffer; }
	char* GetReadBufferPtr(void) { return &mBuffer[mReadPos]; }
	char* GetWriteBufferPtr(void) { return &mBuffer[mWritePos]; }

private:
	char* mBuffer;
	int mTotalSize = 0;

	int mReadPos = 0;
	int mWritePos = 0;
	std::mutex	mLock;

};