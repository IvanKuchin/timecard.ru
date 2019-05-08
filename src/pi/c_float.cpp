#include "c_float.h"

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
/*
	double	result;
	long	multiplier;

	multiplier = pow(10, precision);
	result = round(fma(num, multiplier, 0)) / multiplier;

	// MESSAGE_DEBUG("c_float", "", to_string(num) + " -> " + to_string(result));
	return	result;
*/
	return	num;
}

long c_float::GetWhole() const
{
	double	intpart;
	long	result;

	modf (val , &intpart);

	result = lround(intpart);

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

long c_float::GetFraction() const
{
	double	fractpart, intpart;
	long	result;

	fractpart = modf(val , &intpart);

	result = lround(fma(fractpart, pow(10, precision), 0));

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;

}

string c_float::GetStringValue() const
{
	long			multiplier = pow(10, precision);
	ostringstream	ost;

	ost.str("");
	ost.precision(precision);
	ost << fixed << (round(val * multiplier)) / multiplier; // --- this trick will print 0.01 from 0.005

	return ost.str();	
}

string c_float::GetFormattedOutput() const
{
	return CutTrailingZeroes(GetStringValue());
}

string c_float::PrintPriceTag() const
{
	auto	result = GetStringValue();


	// --- having spaces between thouthands will trigger problems with invoice/act/vat price representation in Excel
	// --- for example: 12 000.55 will be presented as 12 in Excel
	{
		auto	frac_separator = result.find_first_not_of("0123456789");

		if(frac_separator == string::npos)
		{
			MESSAGE_ERROR("", "", "fail to find fraction seprator in price_tag(" + result + ")");
		}
		else
		{
			// --- insert _spaces_ between thousands
			while(frac_separator > 3)
			{
				frac_separator -= 3;
				result.insert(frac_separator, " ");
			}
		}
	}	

	return result;
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

