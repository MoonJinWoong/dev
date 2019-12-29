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
		// �Ҹ��ڰ� �Ҹ��� �ƴ� �׳� ����ϰ� memset�� ������
		std::lock_guard<std::mutex> guard(mLock);
		memset(mBuffer, 0, mTotalSize);
		mReadPos = 0;
		mWritePos = 0;
	}

	int SetWriteBuffer(char* pData, int size)
	{
		std::lock_guard<std::mutex> guard(mLock);
		
		// ������ �׳� ó������ ����
		if (size > GetWriteAbleSize())
		{
			memcpy_s(&mBuffer[0], size, pData, size);
			mReadPos = 0;
			mWritePos = size;
		}
		else
		{
			memcpy_s(&mBuffer[mWritePos],size, pData, size);
			mWritePos += size;
		}
		return size;
	}

	// �������� �� �� �ִ� ������
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

	// �������� ���� �� �ִ� ������
	int	GetReadAbleSize(void)
	{
		if (mReadPos <= mWritePos)
		{
			return mWritePos - mReadPos;
		}

		else
			return (mTotalSize - mReadPos) + mWritePos;
	}

	// ��Ŷ ��Ż ���� �̾ƿ�
	int	GetHeaderSize(char* pData, int size)
	{
		std::lock_guard<std::mutex> guard(mLock);
		
		// �̰� �߸��� �Ŵ�
		if (mTotalSize - mReadPos < size)
		{
			return -1;
		}

		memcpy_s(pData, size, &mBuffer[mReadPos], size);
		return size;
	}

	// Send finish �Ҷ� , Recv finish �Ҷ� 
	void MoveReadPos(int size)
	{
		std::lock_guard<std::mutex> guard(mLock);
		mReadPos = (mReadPos + size) % mTotalSize;
	}

	// OnRecv���� ��Ŷ �����ϱ� ���� ȣ��
	bool MoveWritePos(int size)
	{
		std::lock_guard<std::mutex> guard(mLock);
		
		// ������ �߸� �Ȱ� 
		if (size > mTotalSize)
		{
			return false;
		}

		mWritePos = (mWritePos + size) % mTotalSize;
		return true;
	}

	int	GetRemainSize()
	{
		if (mReadPos > mWritePos)
			return mReadPos - mWritePos;
		else
			return (mTotalSize - mWritePos) + mReadPos;
	}

	char* GetWriteBufferPtr(){return &mBuffer[mWritePos];}
	int   GetWritePos() { return mWritePos; }
	char* GetBufferPtr(void) { return mBuffer; }
	char* GetReadBufferPtr(void) { return &mBuffer[mReadPos]; }
	void  CheckWrite(int remainSize)
	{
		std::lock_guard<std::mutex> guard(mLock);
		
		// ��ȯ
		if (mWritePos + remainSize > mTotalSize)
		{
			memcpy_s(&mBuffer[0],mWritePos - mReadPos,
						&mBuffer[mReadPos], mWritePos - mReadPos);

			mReadPos = 0;
			mWritePos = mWritePos - mReadPos;
			std::cout << "Because Packet split and buffer over ..." << std::endl;
		}
	}

private:
	char* mBuffer;
	int mTotalSize = 0;
	int mReadPos = 0;
	int mWritePos = 0;
	std::mutex	mLock;
};