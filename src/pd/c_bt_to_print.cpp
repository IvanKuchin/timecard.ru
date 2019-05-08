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
	// else if(GetMarkupType().empty()) 		{ MESSAGE_ERROR("", "", "bt markup type is empty"); 			error_message = gettext("bt date start is empty"); }
	else if(GetDateStart().empty()) 		{ MESSAGE_ERROR("", "", "bt date start is empty"); 			error_message = gettext("bt date start is empty"); }
	else if(GetDateFinish().empty()) 		{ MESSAGE_ERROR("", "", "bt date finish is empty"); 		error_message = gettext("bt date finish is empty"); }
	else if(GetAgreementNumber().empty()) 	{ MESSAGE_ERROR("", "", "agreement number is empty"); 		error_message = gettext("agreement number is empty"); }
	else if(GetDateSign().empty())			{ MESSAGE_ERROR("", "", "sign date is empty"); 				error_message = gettext("sign date is empty"); }
	else if(GetSignatureTitle1().empty())	{ MESSAGE_ERROR("", "", "signature title 1 is empty"); 		error_message = gettext("signature title 1 is empty"); }
	else if(GetSignatureTitle2().empty())	{ MESSAGE_ERROR("", "", "signature title 2 is empty"); 		error_message = gettext("signature title 2 is empty"); }

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_BT_To_Print::GetMarkupValue()	-> c_float
{
	c_float		result(0);

	MESSAGE_DEBUG("", "", "start");

	if(GetMarkupType() == "fix")
	{
		result = GetMarkupDB();
	}
	else if(GetMarkupType() == "percent")
	{
		result = (sum_taxable + tax + sum_non_taxable) * GetMarkupDB() / c_float(100);
	}
	else
	{
		MESSAGE_ERROR("", "", "unknown markup type(" + GetMarkupType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result.PrintPriceTag() + ")");

	return result;
}


auto	C_BT_To_Print::CalculateFinals()	-> void
{
	MESSAGE_DEBUG("", "", "start");

	sum_taxable = 0;
	sum_non_taxable = 0;

	for(auto &expense_line: expense_lines)
	{
		c_float		temp(expense_line.price_domestic);

		if(expense_line.taxable == "Y")
			sum_taxable = sum_taxable + temp;
		else
			sum_non_taxable = sum_non_taxable + temp;
	}

	tax = sum_taxable * c_float(BUSINESS_TRIP_TAX_PERCENTAGE) / c_float(100);
	markup = GetMarkupValue();
	total_payment = sum_taxable + tax + sum_non_taxable + markup;

	MESSAGE_DEBUG("", "", "finish");
}

auto	C_BT_To_Print::GetSumTaxable()		-> c_float
{
	c_float		result(0);

	MESSAGE_DEBUG("", "", "start");

	if(sum_taxable.Get()) {} else { CalculateFinals(); }

	result = sum_taxable;

	MESSAGE_DEBUG("", "", "finish (result is " + result.PrintPriceTag() + ")");

	return result;
}

auto	C_BT_To_Print::GetSumNonTaxable()	-> c_float
{
	c_float		result(0);

	MESSAGE_DEBUG("", "", "start");

	if(sum_non_taxable.Get()) {} else { CalculateFinals(); }

	result = sum_non_taxable;

	MESSAGE_DEBUG("", "", "finish (result is " + result.PrintPriceTag() + ")");

	return result;
}

auto	C_BT_To_Print::GetSumTax()			-> c_float
{
	c_float		result(0);

	MESSAGE_DEBUG("", "", "start");

	if(tax.Get()) {} else { CalculateFinals(); }

	result = tax;

	MESSAGE_DEBUG("", "", "finish (result is " + result.PrintPriceTag() + ")");

	return result;
}

auto	C_BT_To_Print::GetMarkup()			-> c_float
{
	c_float		result(0);

	MESSAGE_DEBUG("", "", "start");

	if(markup.Get()) {} else { CalculateFinals(); }

	result = markup;

	MESSAGE_DEBUG("", "", "finish (result is " + result.PrintPriceTag() + ")");

	return result;
}

auto	C_BT_To_Print::GetTotalPayment()	-> c_float
{
	c_float		result(0);

	MESSAGE_DEBUG("", "", "start");

	if(total_payment.Get()) {} else { CalculateFinals(); }

	result = total_payment;

	MESSAGE_DEBUG("", "", "finish (result is " + result.PrintPriceTag() + ")");

	return result;
}


ostream& operator<<(ostream& os, const C_BT_To_Print &var)
{
	os << "object C_BT_To_Print [empty for now]";

	return os;
}

