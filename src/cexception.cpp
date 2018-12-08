#include <sstream>
#include "localy.h"
#include "cexception.h"

CExceptionHTML::CExceptionHTML() : messageID("unknown error"), lng(DEFAULT_LANGUAGE), db(NULL)
{
}

CExceptionHTML::CExceptionHTML(string m) : messageID(m), lng(DEFAULT_LANGUAGE), db(NULL)
{
}

CExceptionHTML::CExceptionHTML(string m, string n) : messageID(m), lng(DEFAULT_LANGUAGE), param1(n), db(NULL)
{
}

string CExceptionHTML::GetReason()
{
	string	result = "";

	if(db)
	{
		if(db->Query("SELECT `message` FROM `exception` WHERE `id`=\"" + messageID + "\" AND `lng`=\"" + lng + "\";"))
			result = db->Get(0, "message");
		else
			MESSAGE_ERROR("CExceptionHTML", "", "exception.id(" + messageID + ") not found");
	}
	else
	{
		MESSAGE_ERROR("CExceptionHTML", "", "DB connection didn't initialized");
	}

	return result;
}

string CExceptionHTML::GetTemplate()
{
	string	result = "";

	if(db)
	{
		if(db->Query("SELECT `template` FROM `exception` WHERE `id`=\"" + messageID + "\" AND `lng`=\"" + lng + "\";"))
			result = db->Get(0, "template");
		else
			MESSAGE_ERROR("CExceptionHTML", "", "exception.id(" + messageID + ") not found");
	}
	else
	{
		MESSAGE_ERROR("CExceptionHTML", "", "DB connection didn't initialized");
	}

	return result;
}

CExceptionHTML::~CExceptionHTML() {}






