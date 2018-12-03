#include <string>
#include <iostream>
#include "ccgi.h"
#include "cmysql.h"
#include "cuser.h"
#include "cstatistics.h"
#include "utilities.h"
#include "utilities_common.h"
#include "localy.h"

using namespace std;

bool Test1()
{
	auto result = true;
	auto error_message = ""s;

	if((GetMinutesDeclension(0) == "�����") && (GetMinutesDeclension(10) == "�����") && (GetMinutesDeclension(11) == "�����") && (GetMinutesDeclension(19) == "�����") && (GetMinutesDeclension(100) == "�����"))
	{
		if((GetMinutesDeclension(1) == "������") && (GetMinutesDeclension(21) == "������") && (GetMinutesDeclension(121) == "������"))
		{
			if((GetMinutesDeclension(2) == "������") && (GetMinutesDeclension(22) == "������") && (GetMinutesDeclension(122) == "������") && (GetMinutesDeclension(1122) == "������"))
			{
				if((GetMinutesDeclension(3) == "������") && (GetMinutesDeclension(23) == "������") && (GetMinutesDeclension(123) == "������") && (GetMinutesDeclension(1123) == "������"))
				{
					if((GetMinutesDeclension(-3) == "") && (GetMinutesDeclension(-23) == "") && (GetMinutesDeclension(-123) == "") && (GetMinutesDeclension(-1123) == ""))
					{
						if((GetMinutesDeclension(1.9) == "������") && (GetMinutesDeclension(21.5) == "������") && (GetMinutesDeclension(122.5) == "������") && (GetMinutesDeclension(1123.1) == "������"))
						{
						}
						else
						{
							result = false;
							cout << "failed on -22.5" << endl;
						}
					}
					else
					{
						result = false;
						cout << "failed on -3" << endl;
					}
				}
				else
				{
					result = false;
					cout << "failed on 3" << endl;
				}
			}
			else
			{
				result = false;
				cout << "failed on 2" << endl;
			}
		}
		else
		{
			result = false;
			cout << "failed on 1" << endl;
		}
	}
	else
	{
		result = false;
		cout << "failed on 0" << endl;
	}

	return result;
}

bool Test2()
{
	auto result = true;
	auto error_message = ""s;

	if((GetHoursDeclension(0) == "�����") && (GetHoursDeclension(10) == "�����") && (GetHoursDeclension(11) == "�����") && (GetHoursDeclension(19) == "�����") && (GetHoursDeclension(100) == "�����"))
	{
		if((GetHoursDeclension(1) == "���") && (GetHoursDeclension(21) == "���") && (GetHoursDeclension(121) == "���"))
		{
			if((GetHoursDeclension(2) == "����") && (GetHoursDeclension(22) == "����") && (GetHoursDeclension(122) == "����") && (GetHoursDeclension(1122) == "����"))
			{
				if((GetHoursDeclension(3) == "����") && (GetHoursDeclension(23) == "����") && (GetHoursDeclension(123) == "����") && (GetHoursDeclension(1123) == "����"))
				{
					if((GetHoursDeclension(-3) == "") && (GetHoursDeclension(-23) == "") && (GetHoursDeclension(-123) == "") && (GetHoursDeclension(-1123) == ""))
					{
						if((GetHoursDeclension(1.9) == "���") && (GetHoursDeclension(21.5) == "���") && (GetHoursDeclension(122.5) == "����") && (GetHoursDeclension(1123.1) == "����"))
						{
						}
						else
						{
							result = false;
							cout << "failed on -22.5" << endl;
						}
					}
					else
					{
						result = false;
						cout << "failed on -3" << endl;
					}
				}
				else
				{
					result = false;
					cout << "failed on 3" << endl;
				}
			}
			else
			{
				result = false;
				cout << "failed on 2" << endl;
			}
		}
		else
		{
			result = false;
			cout << "failed on 1" << endl;
		}
	}
	else
	{
		result = false;
		cout << "failed on 0" << endl;
	}

	return result;
}

bool Test3()
{
	auto result = true;
	auto error_message = ""s;

	if((GetDaysDeclension(0) == "����") && (GetDaysDeclension(10) == "����") && (GetDaysDeclension(11) == "����") && (GetDaysDeclension(19) == "����") && (GetDaysDeclension(100) == "����"))
	{
		if((GetDaysDeclension(1) == "����") && (GetDaysDeclension(21) == "����") && (GetDaysDeclension(121) == "����"))
		{
			if((GetDaysDeclension(2) == "���") && (GetDaysDeclension(22) == "���") && (GetDaysDeclension(122) == "���") && (GetDaysDeclension(1122) == "���"))
			{
				if((GetDaysDeclension(3) == "���") && (GetDaysDeclension(23) == "���") && (GetDaysDeclension(123) == "���") && (GetDaysDeclension(1123) == "���"))
				{
					if((GetDaysDeclension(-3) == "") && (GetDaysDeclension(-23) == "") && (GetDaysDeclension(-123) == "") && (GetDaysDeclension(-1123) == ""))
					{
						if((GetDaysDeclension(1.9) == "����") && (GetDaysDeclension(21.5) == "����") && (GetDaysDeclension(122.5) == "���") && (GetDaysDeclension(1123.1) == "���"))
						{
						}
						else
						{
							result = false;
							cout << "failed on -22.5" << endl;
						}
					}
					else
					{
						result = false;
						cout << "failed on -3" << endl;
					}
				}
				else
				{
					result = false;
					cout << "failed on 3" << endl;
				}
			}
			else
			{
				result = false;
				cout << "failed on 2" << endl;
			}
		}
		else
		{
			result = false;
			cout << "failed on 1" << endl;
		}
	}
	else
	{
		result = false;
		cout << "failed on 0" << endl;
	}

	return result;
}

bool Test4()
{
	auto result = true;
	auto error_message = ""s;

	if((GetMonthsDeclension(0) == "�������") && (GetMonthsDeclension(10) == "�������") && (GetMonthsDeclension(11) == "�������") && (GetMonthsDeclension(19) == "�������") && (GetMonthsDeclension(100) == "�������"))
	{
		if((GetMonthsDeclension(1) == "�����") && (GetMonthsDeclension(21) == "�����") && (GetMonthsDeclension(121) == "�����"))
		{
			if((GetMonthsDeclension(2) == "������") && (GetMonthsDeclension(22) == "������") && (GetMonthsDeclension(122) == "������") && (GetMonthsDeclension(1122) == "������"))
			{
				if((GetMonthsDeclension(3) == "������") && (GetMonthsDeclension(23) == "������") && (GetMonthsDeclension(123) == "������") && (GetMonthsDeclension(1123) == "������"))
				{
					if((GetMonthsDeclension(-3) == "") && (GetMonthsDeclension(-23) == "") && (GetMonthsDeclension(-123) == "") && (GetMonthsDeclension(-1123) == ""))
					{
						if((GetMonthsDeclension(1.9) == "�����") && (GetMonthsDeclension(21.5) == "�����") && (GetMonthsDeclension(122.5) == "������") && (GetMonthsDeclension(1123.1) == "������"))
						{
						}
						else
						{
							result = false;
							cout << "failed on -22.5" << endl;
						}
					}
					else
					{
						result = false;
						cout << "failed on -3" << endl;
					}
				}
				else
				{
					result = false;
					cout << "failed on 3" << endl;
				}
			}
			else
			{
				result = false;
				cout << "failed on 2" << endl;
			}
		}
		else
		{
			result = false;
			cout << "failed on 1" << endl;
		}
	}
	else
	{
		result = false;
		cout << "failed on 0" << endl;
	}

	return result;
}

bool Test5()
{
	auto result = true;
	auto error_message = ""s;

	if((GetYearsDeclension(0) == "���") && (GetYearsDeclension(10) == "���") && (GetYearsDeclension(11) == "���") && (GetYearsDeclension(19) == "���") && (GetYearsDeclension(100) == "���"))
	{
		if((GetYearsDeclension(1) == "���") && (GetYearsDeclension(21) == "���") && (GetYearsDeclension(121) == "���"))
		{
			if((GetYearsDeclension(2) == "����") && (GetYearsDeclension(22) == "����") && (GetYearsDeclension(122) == "����") && (GetYearsDeclension(1122) == "����"))
			{
				if((GetYearsDeclension(3) == "����") && (GetYearsDeclension(23) == "����") && (GetYearsDeclension(123) == "����") && (GetYearsDeclension(1123) == "����"))
				{
					if((GetYearsDeclension(-3) == "") && (GetYearsDeclension(-23) == "") && (GetYearsDeclension(-123) == "") && (GetYearsDeclension(-1123) == ""))
					{
						if((GetYearsDeclension(1.9) == "���") && (GetYearsDeclension(21.5) == "���") && (GetYearsDeclension(122.5) == "����") && (GetYearsDeclension(1123.1) == "����"))
						{
						}
						else
						{
							result = false;
							cout << "failed on -22.5" << endl;
						}
					}
					else
					{
						result = false;
						cout << "failed on -3" << endl;
					}
				}
				else
				{
					result = false;
					cout << "failed on 3" << endl;
				}
			}
			else
			{
				result = false;
				cout << "failed on 2" << endl;
			}
		}
		else
		{
			result = false;
			cout << "failed on 1" << endl;
		}
	}
	else
	{
		result = false;
		cout << "failed on 0" << endl;
	}

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
	if(!Test5())
	{
		testing_success = false;
		cout << "Test5 failed" << endl;
	}

	if(testing_success)
		cout << "Test passed" << endl;

	return(0);
}

