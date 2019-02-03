#include <iostream>
#include "MathFuncsDll.h"
#include <stdexcept>




using namespace std;

namespace MathFuncs
{
	double MyMathFuncs::Add(double a, double b) { return a + b; }
	double MyMathFuncs::Sub(double a, double b) { return a - b; }
	double MyMathFuncs::Mul(double a, double b) { return a * b; }
	double MyMathFuncs::Div(double a, double b) { return a / b; }
}