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

string Test1()
{
	auto error_message = ""s;
	C_ExpenseTemplate	expense_template;

	expense_template.SetID			("");
	expense_template.SetTitle		("expense title");
	expense_template.SetDescription	("expense description");

	error_message = expense_template.CheckValidity(NULL);

	return error_message;
}

string Test2()
{
	auto error_message = ""s;
	C_ExpenseTemplate	expense_template;

	expense_template.SetID			("0");
	expense_template.SetTitle		("expense title");
	expense_template.SetDescription	("expense description");

	error_message = expense_template.CheckValidity(NULL);

	return error_message;
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

/*
	if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
	{
		MESSAGE_ERROR("", "", "Can not connect to mysql database");
		throw CExceptionHTML("MySql connection");
	}

#ifndef MYSQL_3
	db.Query("set names cp1251");
#endif

	indexPage.SetDB(&db);
*/

	if(Test1().length())
	{
		testing_success = false;
		cout << "Test1 failed" << endl;
	}
	if(Test2().length())
	{
		testing_success = false;
		cout << "Test2 failed" << endl;
	}

	if(testing_success)
		cout << "Test passed" << endl;

	return(0);
}

