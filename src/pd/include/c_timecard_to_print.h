#ifndef __C_TIMECARD_TO_PRINT__H__
#define __C_TIMECARD_TO_PRINT__H__

#include <string>
#include <vector>

// #include "cuser.h"
// #include "mysql.h"
#include "c_float.h"
#include "clog.h"

using namespace std;

class C_Timecard_To_Print
{

	struct	Timecard_Line
	{
		string					customer = "";
		string					project = "";
		string					task = "";
		string					hours = "";
	};

	private:
		// CMysql					*db = NULL;
		// CUser					*user = NULL;

		string					id = "";
		string					date_start_str = "";
		string					date_finish_str = "";
		string					agreement_number = "";
		string					date_sign_str = "";
		string					supplier_vat = "";
		string					signature_title_1 = "";
		string					signature_title_2 = "";
		string					initials_1 = "";
		string					initials_2 = "";
		string					position_1 = "";
		string					position_2 = "";
		string					project_number = "";
		string					purchase_order = "";
		string					cost_center_id = "";
		string					sow_id = "";
		c_float					dayrate;
		c_float					total_payment;
		string					approvers = ""s;
		string					sign_role_1 = ""s;
		string					sign_role_2 = ""s;

		vector<Timecard_Line>	timecard_lines = {};
	public:
					C_Timecard_To_Print()						{};
					// C_Timecard_To_Print(CMysql *, CUser *) : db(param1), user(param2) {};

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
		auto		SetSignRole1(const string &param1)			{ sign_role_1 = param1; };
		auto		SetSignRole1(string &&param1) 				{ sign_role_1 = move(param1); };
		auto		SetSignRole2(const string &param1)			{ sign_role_2 = param1; };
		auto		SetSignRole2(string &&param1) 				{ sign_role_2 = move(param1); };
		auto		SetDateSign(const string &param1)			{ date_sign_str = param1; };
		auto		SetDateSign(string &&param1) 				{ date_sign_str = move(param1); };
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
		auto		SetApprovers(const string &param1)			{ approvers = param1; };
		auto		SetApprovers(string &&param1) 				{ approvers = move(param1); };
		auto		SetProjectNumber(const string &param1)		{ project_number = param1; };
		auto		SetProjectNumber(string &&param1) 			{ project_number = move(param1); };
		auto		SetPurchaseOrder(const string &param1)		{ purchase_order = param1; };
		auto		SetPurchaseOrder(string &&param1) 			{ purchase_order = move(param1); };
		auto		SetDayrate(string param1) 					{ dayrate = param1; };
		auto		SetTotalPayment(string param1) 				{ total_payment = param1; };

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
		auto		GetPurchaseOrder()							{ return purchase_order; };	
		auto		GetCostCenterID()							{ return cost_center_id; };	
		auto		GetApprovers()								{ return approvers; };	
		auto		GetDayrate()								{ return dayrate; };
		auto		GetTotalPayment()							{ return total_payment; };
		auto		GetTimecardLines()							{ return timecard_lines; }

		auto		GetTotalHours() -> c_float;

		auto		AddTimecardLine(string customer, string project, string task, string hours) -> string;

		auto		isValid() -> string;
};

ostream&	operator<<(ostream& os, const C_Timecard_To_Print &);

#endif
