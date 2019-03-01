#pragma once


class NETLIB CircleBuffer
{
public:
	CircleBuffer(void);
	virtual ~CircleBuffer(void);

	//������ �޸� �Ҵ�
	bool			CreateCircleBuffer(int nBufferSize = MAX_CIRCLE_BUFFER_SIZE);
	
	//�ʱ�ȭ
	void			InitializeCircleBuffer();
	//�Ҵ�� ���� ũ�⸦ ��ȯ�Ѵ�.
	//inline int		GetBufferSize() { return m_nBufferSize; }

	////�ش��ϴ� ������ �����͸� ��ȯ
	//inline char*	GetBeginMark() { return m_pBeginMark; }
	//inline char*	GetCurrentMark() { return m_pCurrentMark; }
	//inline char*	GetEndMark() { return m_pEndMark; }

	//char*			ForwardMark(int nForwardLength);
	//char*			ForwardMark(int nForwardLength, int nNextLength, DWORD dwRemainLength);
	//void			BackwardMark(int nBackwardLength);

	////���� ���� ����
	//void			ReleaseBuffer(int nReleaseSize);
	////���� ���� ũ�� ��ȯ
	//int				GetUsedBufferSize() { return cAllUserBufSize; }
	////���� ���۾� ����(�̰��� �ϴ� ������ SendPost()�Լ��� ��Ƽ �����忡�� ���ư��⶧����
	////PrepareSendPacket()����(ForwardMark()����) ���� ���� �÷������� PrepareSendPacket�Ѵ����� �����͸�
	////ä�� �ֱ����� �ٷ� �ٸ� �����忡�� SendPost()�� �Ҹ��ٸ� ������ ������ �����Ͱ� �� �� �ִ�.
	////�װ� �����ϱ� ���� �����͸� �� ä�� ���¿����� ���� ���� ����� ������ �� �־���Ѵ�.
	////���Լ��� sendpost�Լ����� �Ҹ��� �ȴ�.
	//void			SetUsedBufferSize(int nUsedBufferSize);

	////���� ���� ���� ��ȯ
	//int				GetAllUsedBufferSize() { return cAllUserBufSize; }


	////���� ������ �о ��ȯ
	//char*			GetBuffer(int nReadSize, int* pReadSize);

	// mutex cLockObject;
private:

	char*			cRingBuffer;		   //���� �����͸� �����ϴ� ���� ������
					
	char*			cBeginMark;			//������ ó���κ��� ����Ű�� �ִ� ������
	char*			cEndMark;				//������ �������κ��� ����Ű�� �ִ� ������
	char*			cCurrentMark;			//������ ���� �κ��� ����Ű�� �ִ� ������
	char*			cGettedBufferMark;	//������� �����͸� ���� ������ ������
	char*			cLastMoveMark;		//ForwardMark�Ǳ� ���� ������ ������

	int				cBufferSize;		//������ �� ũ��
	int 			cUsedBufferSize;	//���� ���� ������ ũ��
	unsigned int	cAllUserBufSize; //�� ó���� �����ͷ�
	//lock_guard<mutex>  cLockObject;		//����ȭ ��ü

	
private:
	//// No copies do not implement
	//CircleBuffer(const RingBuffer &rhs);
	//CircleBuffer &operator=(const RingBuffer &rhs);
};

