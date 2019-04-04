#ifndef __C_PRINT_INVOICE_SERVICE__H__
#define __C_PRINT_INVOICE_SERVICE__H__

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

#include "utilities_timecard.h" 
#include "c_timecard_to_print.h"
#include "c_invoicing_vars.h"
#include "c_float.h"
#include "clog.h"

using namespace std;

class C_Print_Invoice_Service
{
	private:
		CMysql							*db = NULL;
		C_Invoicing_Vars				*vars;

		string							filename = "";
		string							cost_center_id = "";

		HPDF_Doc						__pdf;
		HPDF_Font						__pdf_font;
		string							__pdf_font_name = "Helvetica";
		int								__pdf_line = -1;

	public:
					C_Print_Invoice_Service();

		auto		Restart() -> void;
		auto		SetDB(CMysql *param)							{ db = param; };
		auto		SetFilename(const string &param1)				{ filename = param1; };
		auto		SetFilename(string &&param1) 					{ filename = move(param1); };
		auto		SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto		SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };
		auto		SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto		GetFilename()									{ return filename; }

		auto		PrintInvoiceAsXLS() -> string;

					~C_Print_Invoice_Service()						{ if(__pdf) { HPDF_Free(__pdf); }; };
};

ostream&	operator<<(ostream& os, const C_Print_Invoice_Service &);

#endif
