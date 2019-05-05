#ifndef __WKHTMLTOX_WRAPPER__H__
#define __WKHTMLTOX_WRAPPER__H__

#include <string>

#include <wkhtmltox/pdf.h>

#include "utilities.h"
#include "clog.h"

using namespace std;

class wkhtmltox_wrapper
{
	protected:
		string						src = ""s;
		string						dst = ""s;

	public:
									wkhtmltox_wrapper()			{ wkhtmltopdf_init(false); }; // Init wkhtmltopdf in graphics less mode

		auto						SetSrc(const string &param)	{ src = param; };
		auto						SetSrc(string &&param)		{ src = move(param); };
		auto						SetDst(const string &param)	{ dst = param; };
		auto						SetDst(string &&param)		{ dst = move(param); };

		auto						GetSrc()					{ return src; };
		auto						GetDst()					{ return dst; };

		auto						Convert() -> string;

									~wkhtmltox_wrapper()		{ wkhtmltopdf_deinit(); }; // We will no longer be needing wkhtmltopdf funcionality
};

ostream&	operator<<(ostream& os, const wkhtmltox_wrapper &);

#endif
