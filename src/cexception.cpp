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

void CExceptionHTML::SetLanguage(string lang)
{
	lng = lang;
}

void CExceptionHTML::SetDB(CMysql *mysql)
{
	db = mysql;
}


string CExceptionHTML::GetID()
{
	return messageID;
}

string CExceptionHTML::GetReason()
{
	string	result = "";
	ostringstream	ost;

	if(!db)
	{
		CLog	log;

		log.Write(ERROR,  string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: DB connection doesn't setup");
		return result;
	}

	ost << "SELECT `message` FROM `exception` WHERE `id`=\"" << messageID << "\" AND `lng`=\"" << lng << "\";";
	if(db->Query(ost.str()) == 0) return result;

	result = db->Get(0, "message");
	return result;
}

string CExceptionHTML::GetTemplate()
{
	string	result = "";
	ostringstream	ost;

	if(!db)
	{
		CLog	log;

		log.Write(ERROR,  string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: DB connection doesn't setup");
		return result;
	}

	ost << "SELECT `template` FROM `exception` WHERE `id`=\"" << messageID << "\" AND `lng`=\"" << lng << "\";";
	if(db->Query(ost.str()) == 0) return result;

	result = db->Get(0, "template");
	return result;
}

CExceptionHTML::~CExceptionHTML() {}






