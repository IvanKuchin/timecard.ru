#include "c_bt_to_print.h"

string C_BT_To_Print::AddExpenseLine(string _date, string _description, string _price_domestic, string _price_foreign, string _currency_nominal, string _currency_value, string _currency_name, string _taxable)
{
	string				error_message = "";
	Expense_Line		temp_line;

	MESSAGE_DEBUG("", "", "start");

	temp_line.date				= _date;
	temp_line.description		= _description;
	temp_line.price_domestic	= _price_domestic;
	temp_line.price_foreign		= _price_foreign;
	temp_line.currency_nominal	= _currency_nominal;
	temp_line.currency_value	= _currency_value;
	temp_line.currency_name		= _currency_name;
	temp_line.taxable			= _taxable;

	expense_lines.push_back(temp_line);

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_BT_To_Print::isValid() -> string
{
	auto error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(GetID().empty())						{ MESSAGE_ERROR("", "", "bt.id is empty"); 					error_message = gettext("bt id is empty"); }
	else if(GetDateStart().empty()) 		{ MESSAGE_ERROR("", "", "bt date start is empty"); 			error_message = gettext("bt date start is empty"); }
	else if(GetDateFinish().empty()) 		{ MESSAGE_ERROR("", "", "bt date finish is empty"); 		error_message = gettext("bt date finish is empty"); }
	else if(GetAgreementNumber().empty()) 	{ MESSAGE_ERROR("", "", "agreement number is empty"); 		error_message = gettext("agreement number is empty"); }
	else if(GetDateSign().empty())			{ MESSAGE_ERROR("", "", "sign date is empty"); 				error_message = gettext("sign date is empty"); }
	else if(GetSignatureTitle1().empty())	{ MESSAGE_ERROR("", "", "signature title 1 is empty"); 		error_message = gettext("signature title 1 is empty"); }
	else if(GetSignatureTitle2().empty())	{ MESSAGE_ERROR("", "", "signature title 2 is empty"); 		error_message = gettext("signature title 2 is empty"); }

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

ostream& operator<<(ostream& os, const C_BT_To_Print &var)
{
	os << "object C_BT_To_Print [empty for now]";

	return os;
}

