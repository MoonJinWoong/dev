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