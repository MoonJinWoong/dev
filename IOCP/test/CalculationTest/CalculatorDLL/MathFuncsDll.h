

#ifndef MATHFUNCSDLL_EXPORTS
#define MATHFUNCSDLL_API __declspec(dllexport)
#else
#define MATHFUNCSDLL_API __declspec(dllimport)
#endif // ! MATHFUNCSDLL_EXPORTS




namespace MathFuncs
{

	class MyMathFuncs
	{
	public :
		static MATHFUNCSDLL_API double Add(double a, double b);
		static MATHFUNCSDLL_API double Sub(double a, double b);
		static MATHFUNCSDLL_API double Mul(double a, double b);
		static MATHFUNCSDLL_API double Div(double a, double b);
	};
}