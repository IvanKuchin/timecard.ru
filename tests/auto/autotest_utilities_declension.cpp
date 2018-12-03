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

	if((GetMinutesDeclension(0) == "минут") && (GetMinutesDeclension(10) == "минут") && (GetMinutesDeclension(11) == "минут") && (GetMinutesDeclension(19) == "минут") && (GetMinutesDeclension(100) == "минут"))
	{
		if((GetMinutesDeclension(1) == "минуту") && (GetMinutesDeclension(21) == "минуту") && (GetMinutesDeclension(121) == "минуту"))
		{
			if((GetMinutesDeclension(2) == "минуты") && (GetMinutesDeclension(22) == "минуты") && (GetMinutesDeclension(122) == "минуты") && (GetMinutesDeclension(1122) == "минуты"))
			{
				if((GetMinutesDeclension(3) == "минуты") && (GetMinutesDeclension(23) == "минуты") && (GetMinutesDeclension(123) == "минуты") && (GetMinutesDeclension(1123) == "минуты"))
				{
					if((GetMinutesDeclension(-3) == "") && (GetMinutesDeclension(-23) == "") && (GetMinutesDeclension(-123) == "") && (GetMinutesDeclension(-1123) == ""))
					{
						if((GetMinutesDeclension(1.9) == "минуту") && (GetMinutesDeclension(21.5) == "минуту") && (GetMinutesDeclension(122.5) == "минуты") && (GetMinutesDeclension(1123.1) == "минуты"))
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

	if((GetHoursDeclension(0) == "часов") && (GetHoursDeclension(10) == "часов") && (GetHoursDeclension(11) == "часов") && (GetHoursDeclension(19) == "часов") && (GetHoursDeclension(100) == "часов"))
	{
		if((GetHoursDeclension(1) == "час") && (GetHoursDeclension(21) == "час") && (GetHoursDeclension(121) == "час"))
		{
			if((GetHoursDeclension(2) == "часа") && (GetHoursDeclension(22) == "часа") && (GetHoursDeclension(122) == "часа") && (GetHoursDeclension(1122) == "часа"))
			{
				if((GetHoursDeclension(3) == "часа") && (GetHoursDeclension(23) == "часа") && (GetHoursDeclension(123) == "часа") && (GetHoursDeclension(1123) == "часа"))
				{
					if((GetHoursDeclension(-3) == "") && (GetHoursDeclension(-23) == "") && (GetHoursDeclension(-123) == "") && (GetHoursDeclension(-1123) == ""))
					{
						if((GetHoursDeclension(1.9) == "час") && (GetHoursDeclension(21.5) == "час") && (GetHoursDeclension(122.5) == "часа") && (GetHoursDeclension(1123.1) == "часа"))
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

	if((GetDaysDeclension(0) == "дней") && (GetDaysDeclension(10) == "дней") && (GetDaysDeclension(11) == "дней") && (GetDaysDeclension(19) == "дней") && (GetDaysDeclension(100) == "дней"))
	{
		if((GetDaysDeclension(1) == "день") && (GetDaysDeclension(21) == "день") && (GetDaysDeclension(121) == "день"))
		{
			if((GetDaysDeclension(2) == "дн€") && (GetDaysDeclension(22) == "дн€") && (GetDaysDeclension(122) == "дн€") && (GetDaysDeclension(1122) == "дн€"))
			{
				if((GetDaysDeclension(3) == "дн€") && (GetDaysDeclension(23) == "дн€") && (GetDaysDeclension(123) == "дн€") && (GetDaysDeclension(1123) == "дн€"))
				{
					if((GetDaysDeclension(-3) == "") && (GetDaysDeclension(-23) == "") && (GetDaysDeclension(-123) == "") && (GetDaysDeclension(-1123) == ""))
					{
						if((GetDaysDeclension(1.9) == "день") && (GetDaysDeclension(21.5) == "день") && (GetDaysDeclension(122.5) == "дн€") && (GetDaysDeclension(1123.1) == "дн€"))
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

	if((GetMonthsDeclension(0) == "мес€цев") && (GetMonthsDeclension(10) == "мес€цев") && (GetMonthsDeclension(11) == "мес€цев") && (GetMonthsDeclension(19) == "мес€цев") && (GetMonthsDeclension(100) == "мес€цев"))
	{
		if((GetMonthsDeclension(1) == "мес€ц") && (GetMonthsDeclension(21) == "мес€ц") && (GetMonthsDeclension(121) == "мес€ц"))
		{
			if((GetMonthsDeclension(2) == "мес€ца") && (GetMonthsDeclension(22) == "мес€ца") && (GetMonthsDeclension(122) == "мес€ца") && (GetMonthsDeclension(1122) == "мес€ца"))
			{
				if((GetMonthsDeclension(3) == "мес€ца") && (GetMonthsDeclension(23) == "мес€ца") && (GetMonthsDeclension(123) == "мес€ца") && (GetMonthsDeclension(1123) == "мес€ца"))
				{
					if((GetMonthsDeclension(-3) == "") && (GetMonthsDeclension(-23) == "") && (GetMonthsDeclension(-123) == "") && (GetMonthsDeclension(-1123) == ""))
					{
						if((GetMonthsDeclension(1.9) == "мес€ц") && (GetMonthsDeclension(21.5) == "мес€ц") && (GetMonthsDeclension(122.5) == "мес€ца") && (GetMonthsDeclension(1123.1) == "мес€ца"))
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

	if((GetYearsDeclension(0) == "лет") && (GetYearsDeclension(10) == "лет") && (GetYearsDeclension(11) == "лет") && (GetYearsDeclension(19) == "лет") && (GetYearsDeclension(100) == "лет"))
	{
		if((GetYearsDeclension(1) == "год") && (GetYearsDeclension(21) == "год") && (GetYearsDeclension(121) == "год"))
		{
			if((GetYearsDeclension(2) == "года") && (GetYearsDeclension(22) == "года") && (GetYearsDeclension(122) == "года") && (GetYearsDeclension(1122) == "года"))
			{
				if((GetYearsDeclension(3) == "года") && (GetYearsDeclension(23) == "года") && (GetYearsDeclension(123) == "года") && (GetYearsDeclension(1123) == "года"))
				{
					if((GetYearsDeclension(-3) == "") && (GetYearsDeclension(-23) == "") && (GetYearsDeclension(-123) == "") && (GetYearsDeclension(-1123) == ""))
					{
						if((GetYearsDeclension(1.9) == "год") && (GetYearsDeclension(21.5) == "год") && (GetYearsDeclension(122.5) == "года") && (GetYearsDeclension(1123.1) == "года"))
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

