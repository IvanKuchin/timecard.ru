#include "c_timecard_to_print.h"

string C_Timecard_To_Print::AddTimecardLine(string customer, string project, string task, string hours)
{
	string				error_message = "";
	Timecard_Line		temp_line;

	MESSAGE_DEBUG("", "", "start");

	temp_line.customer = customer;
	temp_line.project = project;
	temp_line.task = task;
	temp_line.hours = hours;

	timecard_lines.push_back(temp_line);

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Timecard_To_Print::isValid() -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(
		GetID().length() &&
		GetDateStart().length() &&
		GetDateFinish().length() &&
		GetAgreementNumber().length() &&
		GetDateSign().length() &&
		GetSignatureTitle1().length() &&
		GetSignatureTitle2().length() &&
		string(GetDayrate()).length()
	)
	{
		result = true;
	}
	else
	{
		MESSAGE_ERROR("", "", "object is invalid");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

ostream& operator<<(ostream& os, const C_Timecard_To_Print &var)
{
	os << "object C_Timecard_To_Print [empty for now]";

	return os;
}

