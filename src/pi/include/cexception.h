#ifndef __CEXCEPTION__H__
#define __CEXCEPTION__H__

#include <string>
#include "cmysql.h"

using namespace std;

class CException
{
	string	message;
public:

			CException() : message("unknown error")	{};
			CException(string m) : message(m)		{};
	string	GetReason() const 						{ return message; };
			~CException()							{};
};

class CExceptionHTML
{
	string	messageID, lng, param1;
	CMysql	*db;
public:

			CExceptionHTML();
			CExceptionHTML(string m);
			CExceptionHTML(string m, string n);
	void	SetLanguage(string lang)				{ lng = lang; }
	void	SetDB(CMysql *mysql)					{ db = mysql; }
	void	SetParam1(const string &p) 				{ param1 = p; };
	void	SetParam1(string &&p) noexcept			{ param1 = move(p); };
	string	GetID() const							{ return messageID; }
	string	GetReason();
	string	GetTemplate();
	string	GetParam1() { return param1; };
			~CExceptionHTML();
};

class CEmptyException
{
	string	message;
public:
    
			CEmptyException() : message("empty error")	{};
			CEmptyException(string m) : message(m) 		{};
	string	GetReason() const							{ return message; };
			~CEmptyException() {};
};

#endif
