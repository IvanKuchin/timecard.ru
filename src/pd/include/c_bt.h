#ifndef __C_BT__H__
#define __C_BT__H__

#include <string>
#include <cstdlib>

#include "cmysql.h"
#include "utilities_timecard.h" 
#include "c_float.h"

using namespace std;

class C_ExpenseLineTemplate
{
	public:
		string					id = "";
		string					bt_expense_template_id = "";
		string					dom_type = "";
		string					title = "";
		string					description = "";
		string					tooltip = "";
		string					payment = "";
		bool					required = false;
		bool					matched = false;

								C_ExpenseLineTemplate();

		void					SetID(const string &param)					{ id = param; }
		void					SetID(string &&param) noexcept				{ id = move(param); }
		string					GetID() const								{ return id; }
		void					SetTitle(const string &param)				{ title = param; }
		void					SetTitle(string &&param) noexcept			{ title = move(param); }
		string					GetTitle() const							{ return title; }
		void					SetBTExpenseTemplateID(const string &param) { bt_expense_template_id = param; }
		void					SetBTExpenseTemplateID(string &&param) noexcept { bt_expense_template_id = move(param); }
		string					GetBTExpenseTemplateID() const 				{ return bt_expense_template_id; }

		string					CheckValidity(CMysql *) const;
		string					SetPaymentMethod(const bool is_cash, const bool is_card);
		string					SaveToDB(CMysql *db);

								~C_ExpenseLineTemplate();
};

class C_ExpenseTemplate
{
		string							id = "";
		string							title = "";
		string							company_id = "";
		string							description = "";
		vector<C_ExpenseLineTemplate>	expense_template_lines;

		string					CheckDuplicateLineTitles();
	public:
								C_ExpenseTemplate();
								
		void					SetID(const string &param) 				{ id = param; }
		void					SetID(string &&param) noexcept			{ id = move(param); }
		string					GetID() const							{ return id; }
		void					SetTitle(const string &param)			{ title = param; }
		void					SetTitle(string &&param) noexcept		{ title = move(param); }
		string					GetTitle() const						{ return title; }
		void					SetDescription(const string &param) 	{ description = param; }
		void					SetDescription(string &&param) noexcept { description = move(param); }
		string					GetDescription() const					{ return description; }
		void					SetCompanyID(const string &param)		{ company_id = param; }
		void					SetCompanyID(string &&param) noexcept	{ company_id = move(param); }
		string					GetCompanyID() const 					{ return company_id; }
		string					CheckValidity(CMysql *) const;
		string					CheckDuplicateLineTitles() const;
		string					SaveToDB(CMysql *db);
		string					AddLine(C_ExpenseLineTemplate );

								~C_ExpenseTemplate();
};

class C_ExpenseLine
{
	private:
		string					id = "";
		string					expense_item_doc_id = "";
		string					value = "";
		string					comment = "";
		string					random = "";
		// --- parent_random used on cooking stage only.
		// --- we need to define actual parent C_Expense
		string					parent_random = "";

		string					RemoveOldImage(CMysql *);
	public:
								C_ExpenseLine();

		void					SetID(const string &param)							{ id = param; }
		void					SetID(string &&param) noexcept						{ id = move(param); }
		string					GetID() const										{ return id; }
		void					SetExpenseLineTemplateID(const string &param) 		{ expense_item_doc_id = param; }
		void					SetExpenseLineTemplateID(string &&param) noexcept	{ expense_item_doc_id = move(param); }
		string					GetExpenseLineTemplateID() const					{ return expense_item_doc_id; }
		void					SetValue(const string &param)						{ value = param; }
		void					SetValue(string &&param) noexcept					{ value = move(param); }
		string					GetValue() const 									{ return value; }
		void					SetComment(const string &param)						{ comment = param; }
		void					SetComment(string &&param) noexcept					{ comment = move(param); }
		string					GetComment() const									{ return comment; }
		void					SetRandom(const string &param)						{ random = param; }
		void					SetRandom(string &&param) noexcept					{ random = move(param); }
		string					GetRandom() const									{ return random; }
		void					SetParentRandom(const string &param)				{ parent_random = param; }
		void					SetParentRandom(string &&param) noexcept			{ parent_random = move(param); }
		string					GetParentRandom() const								{ return parent_random; }

		string					CheckValidity(C_ExpenseLineTemplate, CMysql *) const;
		bool					isSavedInDB() const									{ if(GetID().length() && (GetID() != "0")) return true; else return false; };
		bool					RemoveUnsavedLinesImages(CMysql *) const;

		string					SaveToDB(string expense_id, CMysql*);

								~C_ExpenseLine();
};

class C_Expense
{
	private:
		string					id= "";
		string					date = "";
		string					expense_item_id = "";
		string					payment_type = "";
		c_float					price_domestic = 0, price_foreign = 0;
		long int				currency_nominal = 0;
		c_float					currency_value;
		string					currency_name = "";
		string					comment = "";
		string					random = "";
		vector<C_ExpenseLine>	expense_lines;

		string					CheckExpenseLine(C_ExpenseLineTemplate, CMysql *db) const;
		string					CheckAppropriateExpenseLines(CMysql *db) const;
	public:
								C_Expense() : currency_value(0, 4) {};

		void					SetID(const string &param)						{ id = param; }
		void					SetID(string &&param) noexcept					{ id = move(param); }
		string					GetID() const									{ return id; }
		void					SetExpenseTemplateID(const string &param)		{ expense_item_id = param; }
		void					SetExpenseTemplateID(string &&param) noexcept	{ expense_item_id = move(param); }
		string					GetExpenseTemplateID() const					{ return expense_item_id; }
		void					SetPaymentType(const string &param)				{ payment_type = param; }
		void					SetPaymentType(string &&param) noexcept			{ payment_type = move(param); }
		string					GetPaymentType() const							{ return payment_type; }
		void					SetDate(const string &param)					{ date = param; }
		void					SetDate(string &&param) noexcept				{ date = move(param); }
		string					GetDate() const 								{ return date; }
		void					SetPriceDomestic(const c_float &param)			{ price_domestic = param; }
		c_float					GetPriceDomestic() const						{ return price_domestic; }
		void					SetPriceDomestic(const string &param)			{ price_domestic.Set(param); }
		void					SetPriceForeign(const c_float &param)			{ price_foreign = param; }
		void					SetPriceForeign(const string &param)			{ price_foreign.Set(param); }
		c_float					GetPriceForeign() const							{ return price_foreign; }
		void					SetCurrencyNominal(const long int param)		{ currency_nominal = param; }
		void					SetCurrencyNominal(const string &param)			{ if(param.length()) try {currency_nominal = stol(param);} catch(...) { currency_nominal = 0; MESSAGE_ERROR("C_Expense", "", "can't convert(" + param + ") to long")} }
		long int				GetCurrencyNominal() const						{ return currency_nominal; }
		void					SetCurrencyValue(const c_float &param)			{ currency_value = param; }
		void					SetCurrencyValue(const string &param)			{ currency_value.Set(param); }
		c_float					GetCurrencyValue() const						{ return currency_value; }
		void					SetCurrencyName(const string &param)			{ currency_name = param; }
		void					SetCurrencyName(string &&param) noexcept		{ currency_name = move(param); }
		string					GetCurrencyName() const							{ return currency_name; }
		void					SetComment(const string &param)					{ comment = param; }
		void					SetComment(string &&param) noexcept				{ comment = move(param); }
		string					GetComment() const								{ return comment; }
		void					SetRandom(const string &param)					{ random = param; }
		void					SetRandom(string &&param) noexcept				{ random = move(param); }
		string					GetRandom() const								{ return random; }
		void					AddExpenseLine(C_ExpenseLine param)				{ expense_lines.push_back(param); }
		vector<C_ExpenseLine>	GetExpenseLines() const							{ return expense_lines; }

		string					CheckValidity(CMysql *) const;
		bool					RemoveUnsavedLinesImages(CMysql *) const;

		string					SaveToDB(string bt_id, CMysql*);

								~C_Expense();
};

class C_BT
{
	private:
		string					id = "";
		string					sow_id = "";
		string					customer_id = "";
		string					destination = "";
		string					start_date = "", end_date = "";
		string					purpose = "";
		string					status = "";
		vector<C_Expense>		expenses;
		string					user_id = "";
		string					action = "saved";
		CMysql					*db = NULL;
		CUser					*user = NULL;

		auto					isSubcontractorAssignedToSow() const -> bool;
		auto					isSubcontractorAssignedToCustomerID() const -> bool;
		auto					isCustomerAssignmentInsideBTPeriod() const -> bool;

	public:
								C_BT();

		void					SetDB(CMysql *param)	 				{ db = param; }
		void					SetID(const string &param) 				{ id = param; }
		void					SetID(string &&param) noexcept			{ id = move(param); }
		string					GetID() const 							{ return id; }
		void					SetSowID(const string &param) 			{ sow_id = param; }
		void					SetSowID(string &&param) noexcept		{ sow_id = move(param); }
		string					GetSowID() const 						{ return sow_id; }
		void					SetCustomerID(const string &param) 		{ customer_id = param; }
		void					SetCustomerID(string &&param) noexcept	{ customer_id = move(param); }
		string					GetCustomerID() const 					{ return customer_id; }
		void					SetUserID(const string &param) 			{ user_id = param; }
		void					SetUserID(string &&param) noexcept		{ user_id = move(param); }
		string					GetUserID() const 						{ return user_id; }
		void					SetUser(CUser *param)	 				{ if(param) { user_id = param->GetID(); user = param; } else { MESSAGE_DEBUG("C_BT", "", "CUser * is NULL"); } }
		CUser *					GetUser() const 						{ return user; }
		void					SetDestination(const string &param)		{ destination = param; }
		void					SetDestination(string &&param) noexcept	{ destination = move(param); }
		string					GetDestination() const 					{ return destination; }
		void					SetStartDate(const string &param)		{ start_date = param; }
		void					SetStartDate(string &&param) noexcept	{ start_date = move(param); }
		string					GetStartDate() const					{ return start_date; }
		void					SetEndDate(const string &param)			{ end_date = param; }
		void					SetEndDate(string &&param) noexcept		{ end_date = move(param); }
		string					GetEndDate() const 						{ return end_date; }
		void					SetStatus(const string &param)			{ status = param; };
		void					SetStatus(string &&param) noexcept		{ status = move(param); };
		string					GetStatus();
		void					SetPurpose(const string &param) 		{ purpose = param; }
		void					SetPurpose(string &&param) noexcept		{ purpose = move(param); }
		string					GetPurpose() const 						{ return purpose; }
		void					SetAction(const string &param) 			{ if(param.length()) action = param; else { MESSAGE_DEBUG("C_BT", "", "action is empty (using default)"); } }
		string					GetAction() const 						{ return action; }
		void					AddExpense(C_Expense param) 			{ expenses.push_back(param); }
		vector<C_Expense>		GetExpenses() const						{ return expenses; }

		auto					isValidToUpdate() -> string;
		auto					CheckValidity() const -> string;
		auto					SaveToDB() -> string;
		auto					RemoveUnsavedLinesImages() const -> bool;

		auto					AssignExpenseLineByParentRandom(C_ExpenseLine) -> bool;

								~C_BT();
};

ostream&	operator<<(ostream& os, const C_ExpenseLine &expense);
ostream&	operator<<(ostream& os, const C_Expense &expense);
ostream&	operator<<(ostream& os, const C_BT &expense);

#endif
