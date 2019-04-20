#ifndef __C_PRINT_1C_SUBCONTRACTOR__H__
#define __C_PRINT_1C_SUBCONTRACTOR__H__

#include <string>
#include <vector>
#include <locale>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "codecvt/mbwcvt.hpp"
#pragma GCC diagnostic pop 

#include "utilities_timecard.h" 
#include "c_timecard_to_print.h"
#include "c_invoicing_vars.h"
#include "c_float.h"
#include "ccgi.h"
#include "clog.h"

using namespace std;

class C_Print_1C_Subcontractor
{
	protected:
		CMysql							*db = NULL;
		C_Invoicing_Vars				*vars;

		string							filename = "";
		string							subcontractor_id = "";

		int								total_table_items = 0;

		string							content = "";
	public:
		auto			isTableRowExists(int i) -> bool					{ return vars->Get("timecard_index_" + to_string(i)).length(); };

		auto			SetDB(CMysql *param)							{ db = param; };
		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			SetSubcontractorID(const string &param1)		{ subcontractor_id = param1; };
		auto			SetSubcontractorID(string &&param1) 			{ subcontractor_id = move(param1); };
		auto			SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto			GetFilename()									{ return filename; }
		auto			SaveFile() -> string;

		auto			RenderTemplate() -> string;

		virtual	auto	Print() -> string								= 0;
		virtual auto	GetTemplate_Filename() -> string				= 0;
		virtual auto	GetTemplateBody_Filename() -> string			= 0;
};

ostream&	operator<<(ostream& os, const C_Print_1C_Subcontractor &);

class C_Print_1C_Subcontractor_Payment : public C_Print_1C_Subcontractor
{
	public:
		auto			Print() -> string;
		auto			GetTemplate_Filename() -> string				{ return vars->Get("1C_template_payment_to_subcontractor_full_path"); };
		auto			GetTemplateBody_Filename() -> string			{ return vars->Get("1C_template_payment_to_subcontractor_body_full_path"); };
};

class C_Print_1C_Subcontractor_Payment_Order : public C_Print_1C_Subcontractor
{
	public:
		auto			Print() -> string;
		auto			GetTemplate_Filename() -> string				{ return vars->Get("1C_template_payment_order_to_subcontractor_full_path"); };
		auto			GetTemplateBody_Filename() -> string			{ return vars->Get("1C_template_payment_order_to_subcontractor_body_full_path"); };
};

#endif
