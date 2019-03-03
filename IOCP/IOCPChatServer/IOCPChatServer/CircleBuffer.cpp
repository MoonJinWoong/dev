#include "preCompile.h"


mutex LockObject;

CircleBuffer::CircleBuffer(void)
{
	cRingBuffer = nullptr;
	cBeginMark = nullptr;
	cEndMark = nullptr;
	cCurrentMark = nullptr;
	cGettedBufferMark = nullptr;
	cLastMoveMark = nullptr;
	cUsedBufferSize = 0;
	cAllUserBufSize = 0;

}

CircleBuffer::~CircleBuffer(void)
{
	if (cRingBuffer != nullptr)
		delete[] cRingBuffer;
} 

bool CircleBuffer::CreateCircleBuffer(int nBufferSize)
{
	if (cRingBuffer != NULL)
		delete[] cRingBuffer;

	cBeginMark = new char[nBufferSize];		// ���� ���� ��ŷ

	if (cBeginMark == nullptr)
	{
		cout << "CircleBuffer Create failed....(cBeginMark is null )" << endl;
		return false;
	}

	cEndMark = cBeginMark + nBufferSize - 1;
	cBufferSize = nBufferSize;

	InitializeCircleBuffer();


}

void CircleBuffer::InitializeCircleBuffer()
{
	// ���� ���� �Ҵ��Ҷ��� ���ɾ����� �߰��� ������ �ȵǴϱ� 
	LockObject.lock();
	cUsedBufferSize = 0;
	cCurrentMark = cBeginMark;
	cGettedBufferMark = cBeginMark;
	cLastMoveMark = cEndMark;
	cAllUserBufSize = 0;

	LockObject.unlock();

	cout << "CircleBuffer Init Clear....!!" << endl;
}

char* CircleBuffer::ForwardMark(int Length)
{
	char* PreCurrentMark = NULL;
	
	// �� �ɾ����
	LockObject.lock();

		//������ ���� ó��(overflow)
		if (cUsedBufferSize + Length > cBufferSize)
			return NULL;


		// ���� �־  current mark ����
		if ((cEndMark - cCurrentMark) >= Length)
		{
			PreCurrentMark = cCurrentMark;
			cCurrentMark += Length;
		}

		// ������ ���ڶ� ��ȯ�ϴ� ���
		else
		{
			//��ȯ �Ǳ� �� ������ ��ǥ�� ����
			cLastMoveMark = cCurrentMark;
			cCurrentMark = cBeginMark + Length;
			PreCurrentMark = cBeginMark;
		}

		//�� Ǯ�����
		LockObject.unlock();
		return PreCurrentMark;
}

//���� ���۾� ����(�̰��� �ϴ� ������ SendPost()�Լ��� ��Ƽ �����忡�� ���ư��⶧����
//PrepareSendPacket()����(ForwardMark()����) ���� ���� �÷������� PrepareSendPacket�Ѵ����� �����͸�
//ä�� �ֱ����� �ٷ� �ٸ� �����忡�� SendPost()�� �Ҹ��ٸ� ������ ������ �����Ͱ� �� �� �ִ�.
//�װ� �����ϱ� ���� �����͸� �� ä�� ���¿����� ���� ���� ����� ������ �� �־���Ѵ�.
//���Լ��� sendpost�Լ����� �Ҹ��� �ȴ�.


void CircleBuffer::SetUsedBufferSize(int UsedBufferSize)
{
	//�� �ɾ����
	LockObject.lock();

	cUsedBufferSize += UsedBufferSize;
	cAllUserBufSize += UsedBufferSize;
	
	
	//�� Ǯ�����
	LockObject.unlock();
}

char* CircleBuffer::GetBuffer(int nReadSize, int* pReadSize)
{
	char* pRet = NULL;
	//�� �ɾ����
	LockObject.lock();

		//���������� �� �о��ٸ� �� �о���� ������ �����ʹ� �Ǿ����� �ű��.
		if (cLastMoveMark == cGettedBufferMark)
		{
			cGettedBufferMark = cBeginMark;
			cLastMoveMark = cEndMark;
		}

		//���� ���ۿ� �ִ� size�� �о���� size���� ũ�ٸ�
		if (cUsedBufferSize > nReadSize)
		{
			//�������� ������ �Ǵ�.
			if ((cLastMoveMark - cGettedBufferMark) >= nReadSize)
			{
				*pReadSize = nReadSize;
				pRet = cGettedBufferMark;
				cGettedBufferMark += nReadSize;   // ����
			}
			else
			{
				*pReadSize = (int)(cLastMoveMark - cGettedBufferMark);
				pRet = cGettedBufferMark;
				cGettedBufferMark += *pReadSize;
			}
		}
		else if (cUsedBufferSize > 0)
		{
			//�������� ������ �Ǵ�.
			if ((cLastMoveMark - cGettedBufferMark) >= cUsedBufferSize)
			{
				*pReadSize = cUsedBufferSize;
				pRet = cGettedBufferMark;
				cGettedBufferMark += cUsedBufferSize;
			}
			else
			{
				*pReadSize = (int)(cLastMoveMark - cGettedBufferMark);
				pRet = cGettedBufferMark;
				cGettedBufferMark += *pReadSize;
			}
		}
	

// ��Ǯ�����
		LockObject.unlock();
	return pRet;
}

	
