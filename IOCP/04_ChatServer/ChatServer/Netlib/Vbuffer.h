#pragma once


class VBuffer : public Singleton
{
	DECLEAR_SINGLETON(VBuffer);
public:
	VBuffer(int nMaxBufSize = 1024 * 50);
	virtual ~VBuffer(void);

	void getChar(char& cCh);
	void getShort(short& sNum);
	void getInteger(int& nNum);
	void getString(char* pszBuffer);
	void getStream(char* pszBuffer, short sLen);
	void setInteger(int nl);
	void setShort(short sShort);
	void setChar(char cCh);
	void setString(char* pszBuffer);
	void setStream(char* pszBuffer, short sLen);

	void setBuffer(char* pVBuffer);

	inline int getMaxBufSize() { return m_nMaxBufSize; }
	inline int getCurBufSize() { return m_nCurBufSize; }
	inline char* getCurMark() { return m_pCurMark; }
	inline char* getBeginMark() { return m_pszVBuffer; }

	bool CopyBuffer(char* pDestBuffer);
	void Init();


private:
	char* m_pszVBuffer; // ���� ���� 
	char* m_pCurMark;		// ���� ���� ��ġ
	
	int m_nMaxBufSize;		// �ִ� ���� ������ 
	int m_nCurBufSize;		// ���� ���� ���� ������

	VBuffer(const VBuffer &rhs);
	VBuffer &operator=(const VBuffer &rhs);
};

CREATE_FUNCTION(VBuffer, g_GetVBuffer);