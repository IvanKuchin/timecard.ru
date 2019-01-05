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
	auto			result = true;
	auto			rus_alphabet = "Àà Áá Ââ Ãã Ää Åå ¨¸ Ææ Çç Èè Éé Êê Ëë Ìì Íí Îî Ïï Ğğ Ññ Òò Óó Ôô Õõ Öö ×÷ Øø Ùù Úú Ûû Üü İı Şş ßÿ"s;
	auto			rus_alphabet_upper = "ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞß"s;
	auto			rus_alphabet_lower = "àáâãäå¸æçèéêëìíîïğñòóôõö÷øùúûüışÿ"s;

	setlocale(LC_ALL, "en_US.utf8");

	if(toLower(rus_alphabet_upper) == rus_alphabet_lower) {}
	else
	{
		result = false;
		cout << "failed on toLower us locale" << endl;
	}

	return result;
}

bool Test2()
{
	auto			result = true;
	auto			rus_alphabet = "Àà Áá Ââ Ãã Ää Åå ¨¸ Ææ Çç Èè Éé Êê Ëë Ìì Íí Îî Ïï Ğğ Ññ Òò Óó Ôô Õõ Öö ×÷ Øø Ùù Úú Ûû Üü İı Şş ßÿ"s;
	auto			rus_alphabet_upper = "ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖ×ØÙÚÛÜİŞß"s;
	auto			rus_alphabet_lower = "àáâãäå¸æçèéêëìíîïğñòóôõö÷øùúûüışÿ"s;

	setlocale(LC_ALL, "ru_RU.utf8");

	if(toLower(rus_alphabet_upper) == rus_alphabet_lower) {}
	else
	{
		result = false;
		cout << "failed on toLower ru locale" << endl;
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

	if(testing_success)
		cout << "Test passed" << endl;

	return(0);
}

