#ifndef __C_BT_TO_PRINT__H__
#define __C_BT_TO_PRINT__H__

#include <string>
#include <vector>

// #include "cuser.h"
// #include "mysql.h"
#include "c_float.h"
#include "clog.h"

using namespace std;

class C_BT_To_Print
{

	struct	Expense_Line
	{
		string					date = "";
		string					description = "";
		string					price_domestic = "";
		string					price_foreign = "";
		string					currency_nominal = "";
		string					currency_value = "";
		string					currency_name = "";
		string					taxable = "";
	};

	private:
		// CMysql					*db = NULL;
		// CUser					*user = NULL;

		string					id = "";
		string					sow_id = "";
		string					date_start_str = "";
		string					date_finish_str = "";
		string					agreement_number = "";
		string					date_sign_str = "";
		string					supplier_vat = "";
		string					sign_role_1 = ""s;
		string					sign_role_2 = ""s;
		string					signature_title_1 = "";
		string					signature_title_2 = "";
		string					initials_1 = "";
		string					initials_2 = "";
		string					position_1 = "";
		string					position_2 = "";
		string					project_number = "";
		string					cost_center_id = "";
		string					location = "";
		string					markup_type = "";
		string					approvers = ""s;
		c_float					markup_from_db;
		c_float					sum_taxable;
		c_float					sum_non_taxable;
		c_float					tax;
		c_float					markup;
		c_float					total_payment;

		// c_float					total_payment;

		vector<Expense_Line>	expense_lines = {};

		auto		CalculateFinals()	-> void;
		auto		GetMarkupValue()	-> c_float;

	public:
					C_BT_To_Print()						{};
					// C_BT_To_Print(CMysql *, CUser *) : db(param1), user(param2) {};

		auto		SetID(const string &param1)					{ id = param1; };
		auto		SetID(string &&param1) 						{ id = move(param1); };
		auto		SetSoWID(const string &param1)				{ sow_id = param1; };
		auto		SetSoWID(string &&param1) 					{ sow_id = move(param1); };
		auto		SetDateStart(const string &param1)			{ date_start_str = param1; };
		auto		SetDateStart(string &&param1) 				{ date_start_str = move(param1); };
		auto		SetDateFinish(const string &param1)			{ date_finish_str = param1; };
		auto		SetDateFinish(string &&param1) 				{ date_finish_str = move(param1); };
		auto		SetSupplierVAT(const string &param1)		{ supplier_vat = param1; };
		auto		SetSupplierVAT(string &&param1) 			{ supplier_vat = move(param1); };
		auto		SetAgreementNumber(const string &param1)	{ agreement_number = param1; };
		auto		SetAgreementNumber(string &&param1) 		{ agreement_number = move(param1); };
		auto		SetDateSign(const string &param1)			{ date_sign_str = param1; };
		auto		SetDateSign(string &&param1) 				{ date_sign_str = move(param1); };
		auto		SetSignRole1(const string &param1)			{ sign_role_1 = param1; };
		auto		SetSignRole1(string &&param1) 				{ sign_role_1 = move(param1); };
		auto		SetSignRole2(const string &param1)			{ sign_role_2 = param1; };
		auto		SetSignRole2(string &&param1) 				{ sign_role_2 = move(param1); };
		auto		SetSignatureTitle1(const string &param1)	{ signature_title_1 = param1; };
		auto		SetSignatureTitle1(string &&param1) 		{ signature_title_1 = move(param1); };
		auto		SetSignatureTitle2(const string &param1)	{ signature_title_2 = param1; };
		auto		SetSignatureTitle2(string &&param1) 		{ signature_title_2 = move(param1); };
		auto		SetInitials1(const string &param1)			{ initials_1 = param1; };
		auto		SetInitials1(string &&param1) 				{ initials_1 = move(param1); };
		auto		SetInitials2(const string &param1)			{ initials_2 = param1; };
		auto		SetInitials2(string &&param1) 				{ initials_2 = move(param1); };
		auto		SetPosition1(const string &param1)			{ position_1 = param1; };
		auto		SetPosition1(string &&param1) 				{ position_1 = move(param1); };
		auto		SetPosition2(const string &param1)			{ position_2 = param1; };
		auto		SetPosition2(string &&param1) 				{ position_2 = move(param1); };
		auto		SetCostCenterID(const string &param1)		{ cost_center_id = param1; };
		auto		SetCostCenterID(string &&param1) 			{ cost_center_id = move(param1); };
		auto		SetProjectNumber(const string &param1)		{ project_number = param1; };
		auto		SetProjectNumber(string &&param1) 			{ project_number = move(param1); };
		auto		SetMarkupType(const string &param1)			{ markup_type = param1; };
		auto		SetMarkupType(string &&param1) 				{ markup_type = move(param1); };
		auto		SetLocation(const string &param1)			{ location = param1; };
		auto		SetLocation(string &&param1) 				{ location = move(param1); };
		auto		SetApprovers(const string &param1)			{ approvers = param1; };
		auto		SetApprovers(string &&param1) 				{ approvers = move(param1); };
		auto		SetMarkupDB(c_float param1)					{ markup_from_db = param1; };
		// auto		SetTotalPayment(string param1) 				{ total_payment = param1; };

		auto		GetID()										{ return id; };
		auto		GetSoWID()									{ return sow_id; };
		auto		GetDateStart()								{ return date_start_str; };
		auto		GetDateFinish()								{ return date_finish_str; };
		auto		GetSupplierVAT()							{ return supplier_vat; };
		auto		GetAgreementNumber()						{ return agreement_number; };		
		auto		GetDateSign()								{ return date_sign_str; };
		auto		GetSignRole1()								{ return sign_role_1; };
		auto		GetSignRole2()								{ return sign_role_2; };
		auto		GetSignatureTitle1()						{ return signature_title_1; };		
		auto		GetSignatureTitle2()						{ return signature_title_2; };		
		auto		GetInitials1()								{ return initials_1; };		
		auto		GetInitials2()								{ return initials_2; };		
		auto		GetPosition1()								{ return position_1; };		
		auto		GetPosition2()								{ return position_2; };		
		auto		GetProjectNumber()							{ return project_number; };	
		auto		GetCostCenterID()							{ return cost_center_id; };	
		auto		GetMarkupDB()								{ return markup_from_db; };
		auto		GetLocation()								{ return location; };
		auto		GetMarkupType()								{ return markup_type; };
		auto		GetExpenseLines()							{ return expense_lines; }
		auto		GetApprovers()								{ return approvers; };	

		// --- sum calculations
		auto		GetSumTaxable()		-> c_float;
		auto		GetSumNonTaxable()	-> c_float;
		auto		GetSumTax()			-> c_float;
		auto		GetMarkup()			-> c_float;
		auto		GetTotalPayment()	-> c_float;

		// auto		GetTotalHours() -> c_float;

		auto		AddExpenseLine(string _date, string _description, string _rur_amount, string _currency_amount, string _currency_nominal, string _currency_value, string _currency_name, string _taxable) -> string;
		auto		isValid() -> string;
};

ostream&	operator<<(ostream& os, const C_BT_To_Print &);

#endif
