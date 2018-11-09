#include "c_float.h"

static string CutTrailingZeroes(string number)
{
    size_t   dec_point_place = number.find(".");
    
    if(dec_point_place != string::npos)
    {
        bool   domore = true;
        int    symbols_to_cut = 0;

        for(string::reverse_iterator rit = number.rbegin(); domore && (rit != number.rend()); ++rit)
        {
            if(*rit == '0') ++symbols_to_cut;
            else if(*rit == '.') { ++symbols_to_cut; domore = false;}
            else domore = false;
        }

        number.erase(number.length() - symbols_to_cut);
    }
    
    return number;
}

static string GetFormattedOutput(double val, int prec)
{
	long			multiplier = pow(10, prec);
	ostringstream	ost;

	ost.str("");
	ost.precision(prec);
	ost << fixed << (round(val * multiplier)) / multiplier;

	return CutTrailingZeroes(ost.str());
}

/*
static string GetFormattedOutput(double val, int prec)
{
	ostringstream	result;

	result.setf( ios::fixed, ios::floatfield ); // floatfield set to fixed
	result.precision(prec);
	result << val;

	return result.str();
}
*/
c_float::c_float() : c_float(0, 2) {}

c_float::c_float(double param) : c_float(param, 2) {}

c_float::c_float(double param,  int prec_param)
{
	precision = prec_param;
	val = param;
}

c_float::c_float(string param) : c_float(param, 2) {}

c_float::c_float(string param, int prec_param)
{
	precision = prec_param;

	if(param.empty()) val = 0;
	else
	{
		try
		{
			val = stod(param);
		}
		catch(...)
		{
			MESSAGE_ERROR("", "", "can't convert " + param + " to double");
			val = 0;
		}

	}
}

double c_float::RoundWithPrecision(double num, int precision)
{
	double	result;
	long	multiplier;

	multiplier = pow(10, precision);
	result = round(num * multiplier) / multiplier;

	// MESSAGE_DEBUG("c_float", "", to_string(num) + " -> " + to_string(result));
	return	result;
}

double c_float::RoundWithPrecision(double num)
{
	return	RoundWithPrecision(num, precision);
}

c_float::operator string()
{
	return GetFormattedOutput(Get(), GetPrecision());
}

c_float	c_float::operator+(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(Get() + term2.Get());
	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " + " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator-(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(Get() - term2.Get());
	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " - " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator*(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(Get() * term2.Get());
	MESSAGE_DEBUG("c_float", "", to_string(Get()) + " * " + to_string(term2.Get()) + " = " + to_string(result.Get()));

	return result;
}

c_float	c_float::operator/(const c_float &term2)
{
	c_float	result;

	result = RoundWithPrecision(Get() / term2.Get());
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

/*
c_float& c_float::operator=(const c_float &other)
{
	if(this != &other)
	{
		MESSAGE_DEBUG("c_float", "", "copy assignment (this.precision = " + to_string(this->precision) + ") value (other.value = " + to_string(other.val) + ")");
		this->val = other.val;
	}

	return *this;
}
*/
ostream& operator<<(ostream& os, const c_float &var)
{
	os << GetFormattedOutput(var.Get(), var.GetPrecision());;
	return os;
}

