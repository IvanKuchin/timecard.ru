#include "c_smartway.h"

string C_Smartway::GetAuthJSON()
{
	return "\"authorization\": {"
			      "\"username\": \"" + SMARTWAY_LOGIN + "\","
			      "\"password\": \"" + SMARTWAY_PASSWORD + "\""
			    "}"s;
}

string	C_Smartway::GetPostJSON()
{
	auto	result = ""s;

	if(GetMethod().length())
	{
		result = 
		"{"
		  "\"jsonrpc\": \"2.0\","
		  "\"id\": 1,"
		  "\"method\": \"" + GetMethod() + "\","
		  "\"params\": "
		  	"{" + GetAuthJSON() + ","
		    "\"query\": {" + GetQuery() + "}"
		  "}"
		"}";
	}
	else
	{
		MESSAGE_ERROR("", "", "method is not defined");
	}

	return result;
}

string	C_Smartway::SendRequestToServer()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	post_json = GetPostJSON();

	if(post_json.length())
	{
		chtml.SetParsingDisable();
		chtml.SetPostJSON(post_json);
		chtml.PerformRequest(SMARTWAY_URL);

		MESSAGE_DEBUG("", "", "response (" + chtml.GetContent() + ")")
	}
	else
	{
		MESSAGE_ERROR("", "", "there are no data to post to remote server");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ping()
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	SetMethod("ping");
	SendRequestToServer();

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}