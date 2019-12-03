#ifndef __C_PRINT_1C_COSTCENTER__H__
#define __C_PRINT_1C_COSTCENTER__H__

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

class C_Print_1C_CostCenter_Base
{
	protected:
		CMysql							*db = NULL;
		C_Invoicing_Vars				*vars;

		string							filename = "";
		string							cost_center_id = "";

		int								total_table_items = 0;

		string							content = "";

		virtual auto	GetTemplateFullPath()					-> string	const	= 0;
		virtual auto	GetTemplateTableRowFullPath()			-> string	const	= 0;
		virtual auto	SetTableVariable(const string &param)	-> void	= 0;
		virtual	auto	SetCommentVariable()					-> void = 0;
	public:
		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };

		auto			SetDB(CMysql *param)							{ db = param; };
		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto			SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };
		auto			SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto			GetFilename()									{ return filename; }
		auto			SaveFile() -> string;

		auto			RenderTemplate() -> string;

		auto			Print() -> string;
};

ostream&	operator<<(ostream& os, const C_Print_1C_CostCenter_Base &);

class C_Print_1C_CostCenter_Service : public C_Print_1C_CostCenter_Base
{
	protected:
		auto			GetTemplateFullPath()					-> string	const	{ return vars->Get("1C_template_invoice_to_cc_service_full_path"); };
		auto			GetTemplateTableRowFullPath()			-> string	const	{ return vars->Get("1C_template_invoice_to_cc_service_table_row_full_path"); };
		auto			SetTableVariable(const string &param)	-> void				{ vars->AssignVariableValue("Table_Service_Invoice_To_CostCenter_1C", param, true); };
		auto			SetCommentVariable()					-> void				{ vars->AssignVariableValue("comment_1C", gettext("Local service delivery") + " "s + vars->Get("comment_1C_suffix"), true); };
};

class C_Print_1C_CostCenter_BT : public C_Print_1C_CostCenter_Base
{
	protected:
		auto			GetTemplateFullPath()					-> string	const	{ return vars->Get("1C_template_invoice_to_cc_bt_full_path"); };
		auto			GetTemplateTableRowFullPath()			-> string	const	{ return vars->Get("1C_template_invoice_to_cc_bt_table_row_full_path"); };
		auto			SetTableVariable(const string &param)	-> void				{ vars->AssignVariableValue("Table_BT_Invoice_To_CostCenter_1C", param, true); };
		auto			SetCommentVariable()					-> void				{ vars->AssignVariableValue("comment_1C", gettext("Remote service delivery") + " "s + vars->Get("comment_1C_suffix"), true); };
/*
		auto			GetTemplateFullPath()					-> string	const	{ return vars->Get("1C_template_invoice_to_cc_service_full_path"); };
		auto			GetTemplateTableRowFullPath()			-> string	const	{ return vars->Get("1C_template_invoice_to_cc_service_table_row_full_path"); };
		auto			SetTableVariable(const string &param)	-> void				{ vars->AssignVariableValue("Table_Service_Invoice_To_CostCenter_1C", param, true); };
		auto			SetCommentVariable()					-> void				{ vars->AssignVariableValue("comment_1C", gettext("Remote service delivery") + " "s + vars->Get("comment_1C_suffix"), true); };
*/
};

#endif
