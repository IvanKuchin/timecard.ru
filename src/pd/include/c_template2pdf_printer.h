#ifndef __C_TEMPLATE2PDF_PRINTER__H__
#define __C_TEMPLATE2PDF_PRINTER__H__

#include <string>
#include <vector>
#include <locale>

#include <wkhtmltox/pdf.h>

#include "utilities_timecard.h" 
#include "c_agreements_sow_object.h"
#include "c_invoicing_vars.h"
#include "ccgi.h"
#include "clog.h"

using namespace std;

class C_Template2PDF_Printer
{
	protected:
		CMysql							*db = NULL;
		C_Invoicing_Vars				*vars;

		string							template_filename = "";
		string							filename = "";
		string							subcontractor_id = "";

		int								total_table_items = 0;

		string							content = "";

		auto			SaveTemporaryFile() -> string;
	public:
		auto			SetDB(CMysql *param)							{ db = param; };
		auto			SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			SetTemplate(const string &param1)				{ template_filename = param1; };
		auto			SetTemplate(string &&param1) 					{ template_filename = move(param1); };

		auto			GetFilename()									{ return filename; }
		auto			GetTemplate()									{ return template_filename; }


		auto			RenderTemplate() -> string;
};

ostream&	operator<<(ostream& os, const C_Template2PDF_Printer &);

#endif
