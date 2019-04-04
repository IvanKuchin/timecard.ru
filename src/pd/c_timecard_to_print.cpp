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

auto C_Timecard_To_Print::isValid() -> string
{
	auto error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(GetID().empty())						{ MESSAGE_ERROR("", "", "timecard.id is empty"); 			error_message = gettext("timecard id is empty"); }
	else if(GetDateStart().empty()) 		{ MESSAGE_ERROR("", "", "timecard date start is empty"); 	error_message = gettext("timecard date start is empty"); }
	else if(GetDateFinish().empty()) 		{ MESSAGE_ERROR("", "", "timecard date finish is empty"); 	error_message = gettext("timecard date finish is empty"); }
	else if(GetAgreementNumber().empty()) 	{ MESSAGE_ERROR("", "", "agreement number is empty"); 		error_message = gettext("agreement number is empty"); }
	else if(GetDateSign().empty())			{ MESSAGE_ERROR("", "", "sign date is empty"); 				error_message = gettext("sign date is empty"); }
	else if(GetSignatureTitle1().empty())	{ MESSAGE_ERROR("", "", "signature title 1 is empty"); 		error_message = gettext("signature title 1 is empty"); }
	else if(GetSignatureTitle2().empty())	{ MESSAGE_ERROR("", "", "signature title 2 is empty"); 		error_message = gettext("signature title 2 is empty"); }
	else if(string(GetDayrate()).empty())	{ MESSAGE_ERROR("", "", "dayrate not set");					error_message = gettext("dayrate not set"); }

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Timecard_To_Print::GetTotalHours() -> c_float
{
	c_float	result = 0;

	MESSAGE_DEBUG("", "", "start");

	for(auto &timecard_line: timecard_lines)
	{
		auto hours = split(timecard_line.hours, ',');

		for(auto &hour: hours)
		{	
			result = result + c_float(hour);
		}
	}

	MESSAGE_DEBUG("", "", "end(result = " + string(result) + ")");

	return result;
}


ostream& operator<<(ostream& os, const C_Timecard_To_Print &var)
{
	os << "object C_Timecard_To_Print [empty for now]";

	return os;
}

