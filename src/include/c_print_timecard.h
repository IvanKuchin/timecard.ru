#ifndef __C_PRINT_TIMECARD__H__
#define __C_PRINT_TIMECARD__H__

#include <string>
#include <vector>
#include <locale>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "codecvt/mbwcvt.hpp"
#pragma GCC diagnostic pop 

#define _UNICODE
#include "libxl.h"
#undef _UNICODE

#include "hpdf.h"

#include "utilities.h" 
#include "c_timecard_to_print.h"
#include "c_float.h"
#include "clog.h"

using namespace std;

class C_Print_Timecard
{

	private:
		// CMysql						*db = NULL;
		// CUser						*user = NULL;

		string							filename = "";

		C_Timecard_To_Print				timecard;

	public:
					C_Print_Timecard()								{};
					// C_Print_Timecard(CMysql *, CUser *) : db(param1), user(param2) {};

		auto		SetTimecard(const C_Timecard_To_Print &param1)	{ timecard = param1; };

		auto		SetFilename(const string &param1)				{ filename = param1; };
		auto		SetFilename(string &&param1) 					{ filename = move(param1); };

		auto		GetFilename()									{ return filename; }

		auto		PrintAsXLS() -> string;
		auto		PrintAsPDF() -> string;
};

ostream&	operator<<(ostream& os, const C_Print_Timecard &);

#endif
