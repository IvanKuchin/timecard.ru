#include <string>
#include <iostream>
#include "ccgi.h"
#include "cmysql.h"
#include "cuser.h"
#include "cstatistics.h"
#include "utilities.h"
#include "utilities_common.h"
#include "localy.h"
#include "c_bt.h"

using namespace std;

bool Test1()
{
	auto result = true;
	auto error_message = ""s;
	C_ExpenseTemplate	expense_template;

	expense_template.SetID			("");
	expense_template.SetCompanyID	("0");
	expense_template.SetTitle		("expense title");
	expense_template.SetDescription	("expense description");

	error_message = expense_template.CheckValidity(NULL);

	// --- error_message expected not empty
	if(error_message.length())
	{
		if(error_message == "Ошибка БД") result = false;
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

bool Test2()
{
	auto result = true;
	auto error_message = ""s;
	C_ExpenseTemplate	expense_template;

	expense_template.SetID			("0");
	expense_template.SetTitle		("expense title");
	expense_template.SetDescription	("expense description");

	error_message = expense_template.CheckValidity(NULL);

	// --- error_message expected not empty
	if(error_message.length())
	{
		if(error_message == "Ошибка БД") result = false;
	}

	MESSAGE_DEBUG("", "", "finish");
	
	return result;
}

bool Test3()
{
	auto result = true;
	auto error_message = ""s;
	C_ExpenseTemplate	expense_template;

	expense_template.SetID			("123");
	expense_template.SetCompanyID	("123");
	expense_template.SetTitle		("");
	expense_template.SetDescription	("expense description");

	error_message = expense_template.CheckValidity(NULL);

	// --- error_message expected not empty
	if(error_message.length())
	{
		if(error_message == "Ошибка БД") result = false;
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

bool Test4()
{
	auto result = true;
	auto error_message = ""s;
	C_ExpenseTemplate	expense_template;

	expense_template.SetID			("123");
	expense_template.SetCompanyID	("123");
	expense_template.SetTitle		("expense title");
	expense_template.SetDescription	("expense description");

	error_message = expense_template.CheckValidity(NULL);

	// --- error_message expected not empty
	if(error_message.length())
	{
		if(error_message == "Ошибка БД") {}
		else { result = false; }
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}


int main(void)
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	CMysql			db;
	auto			testing_success = true;

	{ MESSAGE_DEBUG("", "", __FILE__); }
	signal(SIGSEGV, crash_handler);

	if(!Test1())
	{
		testing_success = false;
		cout << "Test1 failed" << endl;
	}
	if(!Test2())
	{
		testing_success = false;
		cout << "Test2 failed" << endl;
	}
	if(!Test3())
	{
		testing_success = false;
		cout << "Test3 failed" << endl;
	}
	if(!Test4())
	{
		testing_success = false;
		cout << "Test4 failed" << endl;
	}

	if(testing_success)
		cout << "Test passed" << endl;

	return(0);
}

