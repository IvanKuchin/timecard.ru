#ifndef __C_BT__H__
#define __C_BT__H__

#include <string>
#include <cstdlib>

#include "cmysql.h"
#include "utilities.h"
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

		void					SetID(string param) { id = param; }
		string					GetID() const { return id; }
		void					SetTitle(string param) { title = param; }
		string					GetTitle() const { return title; }
		void					SetBTExpenseTemplateID(string param) { bt_expense_template_id = param; }
		string					GetBTExpenseTemplateID() const { return bt_expense_template_id; }

		string					CheckValidity(CMysql *) const;
		string					SetPaymentMethod(bool is_cash, bool is_card);
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
								
		void					SetID(string param) { id = param; }
		string					GetID() const { return id; }
		void					SetTitle(string param) { title = param; }
		string					GetTitle() const { return title; }
		void					SetDescription(string param) { description = param; }
		string					GetDescription() const { return description; }
		void					SetCompanyID(string param) { company_id = param; }
		string					GetCompanyID() const { return company_id; }
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

		void					SetID(string param) { id = param; }
		string					GetID() const { return id; }
		void					SetExpenseLineTemplateID(string param) { expense_item_doc_id = param; }
		string					GetExpenseLineTemplateID() const { return expense_item_doc_id; }
		void					SetValue(string param) { value = param; }
		string					GetValue() const { return value; }
		void					SetComment(string param) { comment = param; }
		string					GetComment() const { return comment; }
		void					SetRandom(string param) { random = param; }
		string					GetRandom() const { return random; }
		void					SetParentRandom(string param) { parent_random = param; }
		string					GetParentRandom() const { return parent_random; }

		string					CheckValidity(C_ExpenseLineTemplate, CMysql *) const;
		bool					isSavedInDB() const	{ if(GetID().length() && (GetID() != "0")) return true; else return false; };
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

		void					SetID(string param) { id = param; }
		string					GetID() const { return id; }
		void					SetExpenseTemplateID(string param) { expense_item_id = param; }
		string					GetExpenseTemplateID() const { return expense_item_id; }
		void					SetPaymentType(string param) { payment_type = param; }
		string					GetPaymentType() const { return payment_type; }
		void					SetDate(string param) { date = param; }
		string					GetDate() const { return date; }
		void					SetPriceDomestic(c_float param) { price_domestic = param; }
		c_float					GetPriceDomestic() const { return price_domestic; }
		void					SetPriceDomestic(string param) { if(param.length()) try {price_domestic.Set(stod(param));} catch(...) { price_domestic.Set(0); MESSAGE_ERROR("C_Expense", "", "can't convert(" + param + ") to c_float")} }
		void					SetPriceForeign(c_float param) { price_foreign = param; }
		void					SetPriceForeign(string param) { if(param.length()) try {price_foreign.Set(stod(param));} catch(...) { price_foreign.Set(0); MESSAGE_ERROR("C_Expense", "", "can't convert(" + param + ") to c_float")} }
		c_float					GetPriceForeign() const { return price_foreign; }
		void					SetCurrencyNominal(long int param) { currency_nominal = param; }
		void					SetCurrencyNominal(string param) { if(param.length()) try {currency_nominal = stol(param);} catch(...) { currency_nominal = 0; MESSAGE_ERROR("C_Expense", "", "can't convert(" + param + ") to long")} }
		long int				GetCurrencyNominal() const { return currency_nominal; }
		void					SetCurrencyValue(c_float param) { currency_value = param; }
		void					SetCurrencyValue(string param) { if(param.length()) try { currency_value.Set(stod(param));} catch(...) { currency_value.Set(0); MESSAGE_ERROR("C_Expense", "", "can't convert(" + param + ") to c_float")} }
		c_float					GetCurrencyValue() const { return currency_value; }
		void					SetCurrencyName(string param) { currency_name = param; }
		string					GetCurrencyName() const { return currency_name; }
		void					SetComment(string param) { comment = param; }
		string					GetComment() const { return comment; }
		void					SetRandom(string param) { random = param; }
		string					GetRandom() const { return random; }
		void					AddExpenseLine(C_ExpenseLine param) { expense_lines.push_back(param); }
		vector<C_ExpenseLine>	GetExpenseLines() const { return expense_lines; }

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
		string					destination = "";
		string					start_date = "", end_date = "";
		string					purpose = "";
		string					status = "";
		vector<C_Expense>		expenses;
		string					user_id = "";
		string					action = "saved";
		CMysql					*db = NULL;
		CUser					*user = NULL;

		bool					isSubcontractorAssignedToSow() const;

	public:
								C_BT();

		void					SetDB(CMysql *param) 			{ db = param; }
		void					SetID(string param) 			{ id = param; }
		string					GetID() const 					{ return id; }
		void					SetSowID(string param) 			{ sow_id = param; }
		string					GetSowID() const 				{ return sow_id; }
		void					SetUserID(string param) 		{ user_id = param; }
		string					GetUserID() const 				{ return user_id; }
		void					SetUser(CUser *param)	 		{ if(param) { user_id = param->GetID(); user = param; } else { MESSAGE_DEBUG("C_BT", "", "user * is NULL"); } }
		CUser *					GetUser() const 				{ return user; }
		void					SetDestination(string param)	{ destination = param; }
		string					GetDestination() const 			{ return destination; }
		void					SetStartDate(string param)		{ start_date = param; }
		string					GetStartDate() const			{ return start_date; }
		void					SetEndDate(string param)		{ end_date = param; }
		string					GetEndDate() const 				{ return end_date; }
		void					SetStatus(string param)			{ status = param; };
		string					GetStatus();
		void					SetPurpose(string param) 		{ purpose = param; }
		string					GetPurpose() const 				{ return purpose; }
		void					SetAction(string param) 		{ if(param.length()) action = param; else { MESSAGE_DEBUG("C_BT", "", "action is empty (using default)"); } }
		string					GetAction() const 				{ return action; }
		void					AddExpense(C_Expense param) 	{ expenses.push_back(param); }
		vector<C_Expense>		GetExpenses() const				{ return expenses; }

		string					isValidToUpdate();
		string					CheckValidity() const;
		string					SaveToDB();
		bool					RemoveUnsavedLinesImages() const;

		bool					AssignExpenseLineByParentRandom(C_ExpenseLine);

								~C_BT();
};

ostream&	operator<<(ostream& os, const C_ExpenseLine &expense);
ostream&	operator<<(ostream& os, const C_Expense &expense);
ostream&	operator<<(ostream& os, const C_BT &expense);

#endif
