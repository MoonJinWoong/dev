#pragma once
#include "NetPreCompile.h"

class CircleBuffer
{
public:

	CircleBuffer(size_t capacity) : mBRegionPointer(nullptr), mARegionSize(0), mBRegionSize(0), mCapacity(capacity)
	{
		mBuffer = new char[mCapacity];
		mBufferEnd = mBuffer + mCapacity;
		mARegionPointer = mBuffer;
	}

	~CircleBuffer()
	{
		delete[] mBuffer;
	}

	void BufferReset()
	{
		mBRegionPointer = nullptr;
		mARegionSize = 0;
		mBRegionSize = 0;

		memset(mBuffer, 0, mCapacity);

		mBufferEnd = mBuffer + mCapacity;
		mARegionPointer = mBuffer;
	}

	/// ������ ù�κ� len��ŭ ������
	void Remove(size_t len)
	{
		size_t cnt = len;

		/// Read�� ���������� A�� �ִٸ� A�������� ���� ����

		if (mARegionSize > 0)
		{
			size_t aRemove = (cnt > mARegionSize) ? mARegionSize : cnt;
			mARegionSize -= aRemove;
			mARegionPointer += aRemove;
			cnt -= aRemove;
		}

		// ������ �뷮�� �� ������� B���� ���� 
		if (cnt > 0 && mBRegionSize > 0)
		{
			size_t bRemove = (cnt > mBRegionSize) ? mBRegionSize : cnt;
			mBRegionSize -= bRemove;
			mBRegionPointer += bRemove;
			cnt -= bRemove;
		}

		/// A������ ������� B�� A�� ����ġ 
		if (mARegionSize == 0)
		{
			if (mBRegionSize > 0)
			{
				/// ������ ��� ���̱�
				if (mBRegionPointer != mBuffer)
					memmove(mBuffer, mBRegionPointer, mBRegionSize);

				mARegionPointer = mBuffer;
				mARegionSize = mBRegionSize;
				mBRegionPointer = nullptr;
				mBRegionSize = 0;
			}
			else
			{
				mBRegionPointer = nullptr;
				mBRegionSize = 0;
				mARegionPointer = mBuffer;
				mARegionSize = 0;
			}
		}
	}

	size_t GetFreeSpaceSize()
	{
		if (mBRegionPointer != nullptr)
			return GetBFreeSpace();
		else
		{
			/// A ���ۺ��� �� ���� �����ϸ�, B ���۷� ����ġ
			if (GetAFreeSpace() < GetSpaceBeforeA())
			{
				AllocateB();
				return GetBFreeSpace();
			}
			else
				return GetAFreeSpace();
		}
	}

	size_t GetStoredSize() const
	{
		return mARegionSize + mBRegionSize;
	}

	size_t GetContiguiousBytes() const
	{
		if (mARegionSize > 0)
			return mARegionSize;
		else
			return mBRegionSize;
	}

	/// ���Ⱑ ������ ��ġ (������ ���κ�) ��ȯ
	char* GetBuffer() const
	{
		if (mBRegionPointer != nullptr)
			return mBRegionPointer + mBRegionSize;
		else
			return mARegionPointer + mARegionSize;
	}




	/// Ŀ��(aka. IncrementWritten)
	void Commit(size_t len)
	{
		if (mBRegionPointer != nullptr)
			mBRegionSize += len;
		else
			mARegionSize += len;
	}

	/// ������ ù�κ� ����
	char* GetBufferStart() const
	{
		if (mARegionSize > 0)
			return mARegionPointer;
		else
			return mBRegionPointer;
	}


private:

	void AllocateB()
	{
		mBRegionPointer = mBuffer;
	}

	size_t GetAFreeSpace() const
	{
		return (mBufferEnd - mARegionPointer - mARegionSize);
	}

	size_t GetSpaceBeforeA() const
	{
		return (mARegionPointer - mBuffer);
	}


	size_t GetBFreeSpace() const
	{
		if (mBRegionPointer == nullptr)
			return 0;

		return (mARegionPointer - mBRegionPointer - mBRegionSize);
	}

private:

	char* mBuffer;
	char* mBufferEnd;

	char* mARegionPointer;
	size_t	mARegionSize;

	char* mBRegionPointer;
	size_t	mBRegionSize;

	size_t	mCapacity;
};