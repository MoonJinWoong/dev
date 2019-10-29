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

	// �Ҵ�� ũ�� ��ȯ
	int GetBufferSize() const { return mBufferSize; }
	int GetUsedBufferLen() { return mUedBufferSize; }		  // ���� ��
	int GetTotalBufferLen() { return mTotalUsedBufferSize; }  // ���� ���� ��뷮


	// ���� ���� ������ ��ȯ - ������ ,����, ����
	char* GetBeginPos() const { return mBeginPos; }
	char* GetCurrPos() const { return mCurrPos; }
	char* GetEndPos() const { return mEndPos; }



	// send �� ����(���̸� �� �� ����)
	char* CommitForSend(int len)
	{
		char* preCurrPos = NULL;
		SpinLockGuard Lock(&m_CS);
		{
			if ((mUedBufferSize + len) > mBufferSize) return NULL;

			if ((mEndPos - mCurrPos) >= len)
			{
				// ����
				preCurrPos = mCurrPos;
				mCurrPos += len;
			}
			else   // ��ȯ�ؾ� �Ǵ� ���
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


	// recv�� ���� (���� ��)
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
			else // ��ȯ�ؾ� �ϴ� ��� 
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
	// ���� ���� ���� ����
	void Release(int len)
	{
		SpinLockGuard Lock(&m_CS);
		{
			mUedBufferSize -= len;
		}
	}



	// ���� ���� �о ��ȯ
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

	char* mData = nullptr;    // ������ �����͸� �����ϴ� ����
	char* mBeginPos = nullptr;  // ���� ���� ����
	char* mEndPos = nullptr;		// ���� �� ����
	char* mCurrPos = nullptr;    // ���� ���� ���Ǵ� �κ� ����Ű�� �ִ� ����
	char* mReadPos = nullptr;    // ���� ����
	char* mLastSavePos = nullptr;    // ��ȯ�ϱ��� ������ ������

	int mBufferSize =0; // ���� ���� ũ��
	int mUedBufferSize =0; // ���� ũ�� 
	int mTotalUsedBufferSize =0;  // ��ó���� ������

};