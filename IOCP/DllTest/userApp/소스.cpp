
#include "../DllTest/calculatoe.h"
#pragma comment(lib,"Dlltest.lib")

int main()
{
	Calculator * cal = new Calculator;

	cal->Sum(10, 3);

	cal->printSum();


	cal->Sum(132414, 134134134);

	cal->printSum();

}
