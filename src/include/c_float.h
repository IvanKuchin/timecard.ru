#ifndef __C_FLOAT__H__
#define __C_FLOAT__H__

#include <string>
#include <cstdlib>
#include <math.h>		// --- pow() in RoundWithPrecision

#include "clog.h"

using namespace std;

class c_float
{
	private:
		double		val;
		int			precision;

		double		RoundWithPrecision(double num, int precision);
		double		RoundWithPrecision(double num);

	public:
					c_float();
					c_float(string param);
					c_float(string param, int prec_param);
					c_float(double param);
					c_float(double param,  int prec_param);

		void		Set(double param) 						{ val = RoundWithPrecision(param, precision); };
		double		Get()	const 							{ return val; };
		double		GetPrecision()	const					{ return precision; };

					operator double() 						{ return Get(); };
					operator string();
		c_float		operator+(const c_float&);
		c_float		operator-(const c_float&);
		c_float		operator*(const c_float&);
		c_float		operator/(const c_float&);
		c_float		operator/(const long int&);
		// c_float&	operator=(const c_float &);

					~c_float()								{};
};

ostream&	operator<<(ostream& os, const c_float &);

#endif
