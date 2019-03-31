#include "preCompile.h"


void SpreadsheetCell::setValue(double inValue)
{
	mValue = inValue;

	//std::cout << *this << std::endl;
}

double SpreadsheetCell::getValue() const
{
	return mValue;
}

void SpreadsheetCell::setString(const std::string& inString)
{
	mString = inString;
	mValue = stringToDouble(mString);
}

const std::string& SpreadsheetCell::getString() const
{
	return mString;
}

std::string SpreadsheetCell::doubleToString(double inValue) const
{
	std::ostringstream ostr;
	ostr << inValue;
	return ostr.str();
}

double SpreadsheetCell::stringToDouble(const std::string& inString) const
{
	double tmp;
	std::istringstream istr;
	istr >> tmp;
	if (istr.fail() || !istr.eof())
		return 0;

	return tmp;
}