#include "c_float.h"

c_float::c_float() : c_float(0, 2) {}

c_float::c_float(double param) : c_float(param, 2) {}

c_float::c_float(double param,  int prec_param) : precision(prec_param) 
{
	val = RoundWithPrecision(param, precision);
}

c_float::c_float(string param) : c_float(param, 2) {}

c_float::c_float(string param, int prec_param) : precision(prec_param)
{
// TODO: remove after building c_float tests
/*
	if(param.empty()) val = 0;
	else
	{
		try
		{
			val = stod(FixRussianLocale(param));
		}
		catch(...)
		{
			MESSAGE_ERROR("c_float", "", "can't convert " + param + " to double");
			val = 0;
		}

	}
*/
	Set(param);
}

void c_float::Set(string param)
{
	if(param.empty()) Set(0);
	else
	{
		Set(stod_noexcept(param));
	}
}

auto c_float::FixRussianLocale(string param) -> string
{
	locale	loc;

	MESSAGE_DEBUG("c_float", "", "start (" + param + ")")

	if(loc.name().find("ru"))
		if(param.find(".") != string::npos)
			param.replace(param.find("."), 1, ",");

	MESSAGE_DEBUG("c_float", "", "finish (" + param + ")")

	return param;
}

double c_float::RoundWithPrecision(double num, int precision)
{
	double	result;
	long	multiplier;

	multiplier = pow(10, precision);
	result = round(fma(num, multiplier, 0)) / multiplier;

	// MESSAGE_DEBUG("c_float", "", to_string(num) + " -> " + to_string(result));
	return	result;
}

double c_float::RoundWithPrecision(double num)
{
	return	RoundWithPrecision(num, precision);
}

long c_float::GetWhole()
{
    double	intpart;
    long	result;

    modf (val , &intpart);

    result = lround(intpart);

    MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

    return result;

}

long c_float::GetFraction()
{
    double	fractpart, intpart;
    long	result;

    fractpart = modf (val , &intpart);

    result = lround(fma(fractpart, 100, 0));

    MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

    return result;

}

string c_float::GetFormattedOutput() const
{
/*
	long			multiplier = pow(10, precision);
	ostringstream	ost;

	ost.str("");
	ost.precision(precision);
	ost << fixed << (round(val * multiplier)) / multiplier;

	return CutTrailingZeroes(ost.str());
*/
	return CutTrailingZeroes(GetPriceTag());
}

string c_float::GetPriceTag() const
{
	long			multiplier = pow(10, precision);
	ostringstream	ost;

	ost.str("");
	ost.precision(precision);
	ost << fixed << (round(val * multiplier)) / multiplier;

	return ost.str();
}

c_float::operator string() const
{
	return GetFormattedOutput();
}

c_float	c_float::operator+(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(fma(1, term2.Get(), Get()));

	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " + " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator-(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(fma(-1, term2.Get(), Get()));

	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " - " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator*(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(fma(Get(), term2.Get(), 0));

	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " * " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator/(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(fma(Get(), 1 / term2.Get(), 0));

	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " / " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator/(const long int &term2)
{
	c_float	result;

	result = RoundWithPrecision(Get() / term2);
	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " / " + to_string(term2) + " = " + to_string(result.Get()));

	return result;
}

ostream& operator<<(ostream& os, const c_float &var)
{
	os << string(var);
	return os;
}

