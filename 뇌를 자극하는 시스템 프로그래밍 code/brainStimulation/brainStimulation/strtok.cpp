#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>








int _tmain(int argc, TCHAR* argv[])
{
	TCHAR string[] = _T("Hey! get a life!!!!")
		_T("You dont even have two, pennies to rub together !!!! ");

	TCHAR seps[] = _T(" ,.!");

	// ��ū �и� ���� , ���ڿ� ���� �� ù��° ��ū ��ȯ
	TCHAR * token = _tcstok(string, seps);

	// ����ؼ� ��ū ��ȯ �� ��� 
	while (token != NULL)
	{
		_tprintf(_T(" %s\n"), token);
		token = _tcstok(NULL, seps);
	}

	return 0;
}