#include "cron_daily.h"
#include "cactivator.h"

bool CleanupActivators(CMysql *db)
{
	bool		result = true;

	MESSAGE_DEBUG("", "", "start");

	db->Query("DELETE FROM `activators` WHERE  `date`<=(now() - INTERVAL " + to_string(ACTIVATOR_SESSION_LEN) + " MINUTE);");
	
	MESSAGE_DEBUG("", "", "finish");

	return result;
}

bool ParseXMLAndUpdateDB(const string &xml_block, CMysql *db)
{
	bool	result = false;
	regex	date_regex("\\bDate\\=\\\"([[:digit:]]+)\\.([[:digit:]]+)\\.([[:digit:]]+)\\\"");
	smatch	sm;
	string	date = "";

	MESSAGE_DEBUG("", "", "start");

	if(regex_search(xml_block, sm, date_regex))
	{
		regex		numcode_regex	("\\<NumCode\\>(.*)<\\/NumCode\\>");
		regex		charcode_regex	("\\<CharCode\\>(.*)<\\/CharCode\\>");
		regex		nominal_regex	("\\<Nominal\\>(.*)<\\/Nominal\\>");
		regex		name_regex		("\\<Name\\>(.*)<\\/Name\\>");
		regex		value_regex		("\\<Value\\>(.*)<\\/Value\\>");

		string		query = "";

		date = string(sm[3]) + "-" + string(sm[2]) + "-" + string(sm[1]);
		MESSAGE_DEBUG("", "", "Downloaded rate exchange date is " + date + "");

		if(db->Query("SELECT `id` FROM `currency_rate` WHERE `date`=CURDATE();"))
		{
			MESSAGE_ERROR("", "", "Today exchange rate already in DB");
			// --- OR !!!!
			// MESSAGE_DEBUG("cron_daily", "", "exchange rate for " + date + " already in DB");
			// result = true;
		}
		else
		{
			size_t		valute_start, valute_end;

			valute_start = xml_block.find("<Valute");
			valute_end = xml_block.find("</Valute>");

			while((valute_start != string::npos) && (valute_end != string::npos))
			{
				if(valute_start < valute_end)
				{
					string		valute_block = xml_block.substr(valute_start, valute_end - valute_start);

					if(valute_block.length())
					{
						string	numcode, charcode, nominal, name, value;

						if(regex_search(valute_block, sm, numcode_regex)) numcode = sm[1];
						if(regex_search(valute_block, sm, charcode_regex)) charcode = sm[1];
						if(regex_search(valute_block, sm, nominal_regex)) nominal = sm[1];
						if(regex_search(valute_block, sm, name_regex)) name = sm[1];
						if(regex_search(valute_block, sm, value_regex)) value = sm[1];

						if(numcode.length() && charcode.length() && nominal.length() && name.length() && value.length())
						{
							// MESSAGE_DEBUG("", "", "numcode=" + numcode + ", charcode=" + charcode + ", nominal=" + nominal + ", name=" + name + ", value=" + value + "");
							if(query.length()) query += ",";
							value = SymbolReplace(value, ",", ".");
							query += "(CURDATE(), \"" + numcode + "\", \"" + charcode + "\", \"" + nominal + "\", \"" + name + "\", \"" + value + "\")";
						}
						else
						{
							MESSAGE_ERROR("", "", "some parameters are empty (numcode=" + numcode + ", charcode=" + charcode + ", nominal=" + nominal + ", name=" + name + ", value=" + value + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "valute_block(start pos: " + to_string(valute_start) + ") is empty");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "valute_start(" + to_string(valute_start) + ") > valute_end(" + to_string(valute_end) + ")");
				}

				valute_start = xml_block.find("<Valute", valute_end + 1);
				valute_end = xml_block.find("</Valute>", valute_end + 1);
			}

			if(query.length())
			{
				db->Query("INSERT INTO `currency_rate` (`date`, `num_code`, `char_code`, `nominal`, `name`, `value`) VALUES " + query);
				result = true;

			}
			else
			{
				MESSAGE_ERROR("", "", "empty SQL query");
			}
		}

/*
		while(rit != rend)
		{
			for(auto match_substr: sm)
			{
				MESSAGE_DEBUG("", "", "submatch: " + string(match_substr));
			}
			MESSAGE_DEBUG("", "", "-----------" + rit->str());
			++rit;
		}
*/

	}
	else
	{
		MESSAGE_ERROR("", "", "date regex not matched");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string	GetXMLLocaly()
{
	return "<?xml version=\"1.0\" encoding=\"windows-1251\"?><ValCurs Date=\"08.08.2018\" name=\"Foreign Currency Market\"><Valute ID=\"R01010\"><NumCode>036</NumCode><CharCode>AUD</CharCode><Nominal>1</Nominal><Name>Австралийский доллар</Name><Value>47,2184</Value></Valute><Valute ID=\"R01020A\"><NumCode>944</NumCode><CharCode>AZN</CharCode><Nominal>1</Nominal><Name>Азербайджанский манат</Name><Value>37,3450</Value></Valute><Valute ID=\"R01035\"><NumCode>826</NumCode><CharCode>GBP</CharCode><Nominal>1</Nominal><Name>Фунт стерлингов Соединенного королевства</Name><Value>82,3638</Value></Valute><Valute ID=\"R01060\"><NumCode>051</NumCode><CharCode>AMD</CharCode><Nominal>100</Nominal><Name>Армянских драмов</Name><Value>13,1968</Value></Valute><Valute ID=\"R01090B\"><NumCode>933</NumCode><CharCode>BYN</CharCode><Nominal>1</Nominal><Name>Белорусский рубль</Name><Value>31,6132</Value></Valute><Valute ID=\"R01100\"><NumCode>975</NumCode><CharCode>BGN</CharCode><Nominal>1</Nominal><Name>Болгарский лев</Name><Value>37,6236</Value></Valute><Valute ID=\"R01115\"><NumCode>986</NumCode><CharCode>BRL</CharCode><Nominal>1</Nominal><Name>Бразильский реал</Name><Value>17,0082</Value></Valute><Valute ID=\"R01135\"><NumCode>348</NumCode><CharCode>HUF</CharCode><Nominal>100</Nominal><Name>Венгерских форинтов</Name><Value>23,0373</Value></Valute><Valute ID=\"R01200\"><NumCode>344</NumCode><CharCode>HKD</CharCode><Nominal>10</Nominal><Name>Гонконгских долларов</Name><Value>80,9510</Value></Valute><Valute ID=\"R01215\"><NumCode>208</NumCode><CharCode>DKK</CharCode><Nominal>10</Nominal><Name>Датских крон</Name><Value>98,7436</Value></Valute><Valute ID=\"R01235\"><NumCode>840</NumCode><CharCode>USD</CharCode><Nominal>1</Nominal><Name>Доллар США</Name><Value>63,5425</Value></Valute><Valute ID=\"R01239\"><NumCode>978</NumCode><CharCode>EUR</CharCode><Nominal>1</Nominal><Name>Евро</Name><Value>73,5632</Value></Valute><Valute ID=\"R01270\"><NumCode>356</NumCode><CharCode>INR</CharCode><Nominal>100</Nominal><Name>Индийских рупий</Name><Value>92,4355</Value></Valute><Valute ID=\"R01335\"><NumCode>398</NumCode><CharCode>KZT</CharCode><Nominal>100</Nominal><Name>Казахстанских тенге</Name><Value>18,2013</Value></Valute><Valute ID=\"R01350\"><NumCode>124</NumCode><CharCode>CAD</CharCode><Nominal>1</Nominal><Name>Канадский доллар</Name><Value>48,9995</Value></Valute><Valute ID=\"R01370\"><NumCode>417</NumCode><CharCode>KGS</CharCode><Nominal>100</Nominal><Name>Киргизских сомов</Name><Value>93,2392</Value></Valute><Valute ID=\"R01375\"><NumCode>156</NumCode><CharCode>CNY</CharCode><Nominal>10</Nominal><Name>Китайских юаней</Name><Value>92,9718</Value></Valute><Valute ID=\"R01500\"><NumCode>498</NumCode><CharCode>MDL</CharCode><Nominal>10</Nominal><Name>Молдавских леев</Name><Value>38,5106</Value></Valute><Valute ID=\"R01535\"><NumCode>578</NumCode><CharCode>NOK</CharCode><Nominal>10</Nominal><Name>Норвежских крон</Name><Value>77,2732</Value></Valute><Valute ID=\"R01565\"><NumCode>985</NumCode><CharCode>PLN</CharCode><Nominal>1</Nominal><Name>Польский злотый</Name><Value>17,2909</Value></Valute><Valute ID=\"R01585F\"><NumCode>946</NumCode><CharCode>RON</CharCode><Nominal>1</Nominal><Name>Румынский лей</Name><Value>15,8361</Value></Valute><Valute ID=\"R01589\"><NumCode>960</NumCode><CharCode>XDR</CharCode><Nominal>1</Nominal><Name>СДР (специальные права заимствования)</Name><Value>88,6380</Value></Valute><Valute ID=\"R01625\"><NumCode>702</NumCode><CharCode>SGD</CharCode><Nominal>1</Nominal><Name>Сингапурский доллар</Name><Value>46,5172</Value></Valute><Valute ID=\"R01670\"><NumCode>972</NumCode><CharCode>TJS</CharCode><Nominal>10</Nominal><Name>Таджикских сомони</Name><Value>67,4907</Value></Valute><Valute ID=\"R01700J\"><NumCode>949</NumCode><CharCode>TRY</CharCode><Nominal>1</Nominal><Name>Турецкая лира</Name><Value>12,1193</Value></Valute><Valute ID=\"R01710A\"><NumCode>934</NumCode><CharCode>TMT</CharCode><Nominal>1</Nominal><Name>Новый туркменский манат</Name><Value>18,1810</Value></Valute><Valute ID=\"R01717\"><NumCode>860</NumCode><CharCode>UZS</CharCode><Nominal>10000</Nominal><Name>Узбекских сумов</Name><Value>81,6422</Value></Valute><Valute ID=\"R01720\"><NumCode>980</NumCode><CharCode>UAH</CharCode><Nominal>10</Nominal><Name>Украинских гривен</Name><Value>23,5212</Value></Valute><Valute ID=\"R01760\"><NumCode>203</NumCode><CharCode>CZK</CharCode><Nominal>10</Nominal><Name>Чешских крон</Name><Value>28,7003</Value></Valute><Valute ID=\"R01770\"><NumCode>752</NumCode><CharCode>SEK</CharCode><Nominal>10</Nominal><Name>Шведских крон</Name><Value>71,2695</Value></Valute><Valute ID=\"R01775\"><NumCode>756</NumCode><CharCode>CHF</CharCode><Nominal>1</Nominal><Name>Швейцарский франк</Name><Value>63,8233</Value></Valute><Valute ID=\"R01810\"><NumCode>710</NumCode><CharCode>ZAR</CharCode><Nominal>10</Nominal><Name>Южноафриканских рэндов</Name><Value>47,6867</Value></Valute><Valute ID=\"R01815\"><NumCode>410</NumCode><CharCode>KRW</CharCode><Nominal>1000</Nominal><Name>Вон Республики Корея</Name><Value>56,5891</Value></Valute><Valute ID=\"R01820\"><NumCode>392</NumCode><CharCode>JPY</CharCode><Nominal>100</Nominal><Name>Японских иен</Name><Value>57,0758</Value></Valute></ValCurs>";
}

string	GetXMLFromRemoteServer()
{
	string		result = "";

	// --- !!! IMPORTANT !!! Create and destroy CHTML just once in the app
	CHTML	downloader;

	MESSAGE_DEBUG("", "", "start");

	downloader.SetParsingDisable();
	if(downloader.PerformRequest("http://www.cbr.ru/scripts/XML_daily_eng.asp"))
	{
		result = downloader.GetContent();

		MESSAGE_DEBUG("", "", "xml length = " + to_string(result.length()));
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to download currency rate exchange data");
	}

	return result;
}

bool UpdateCurrencyRateExchange(CMysql *db)
{
	bool	result = false;
	// string	xml_block = GetXMLLocaly();
	string	xml_block = GetXMLFromRemoteServer();

	if(xml_block.length())
	{
		if(ParseXMLAndUpdateDB(xml_block, db))
		{

		}
		else
		{
			MESSAGE_ERROR("", "", "fail tp parse XML or fail to update DB");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "xml block is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return	result;
}

bool RemoveOldCaptcha()
{
	DIR 		*dir;
	struct 		dirent *ent;
	bool		result = true;
	string		dirName = IMAGE_CAPTCHA_DIRECTORY;
	time_t		now;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	time(&now);

	if ((dir = opendir( dirName.c_str() )) != NULL)
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL)
		{
			struct	stat	sb;
			string	fileName = dirName + ent->d_name;

			if(stat(fileName.c_str(), &sb) == 0)
			{
				double		secondsBetween = difftime(now, sb.st_mtime);


				if(secondsBetween > 2600 * 24)
				{
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: remove file [" + ent->d_name + "] created " + to_string(secondsBetween) + " secs ago");
					}
					unlink(fileName.c_str());
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: file stat [" + ent->d_name + "]");
				}
			}
		}
		closedir (dir);
	}
	else
	{
		/* could not open directory */
		result = false;
	}



	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish");
	}

	return result;
}

bool RemoveTempMedia(CMysql *db)
{
	bool	result = true;
	int		affected;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("SELECT * FROM `temp_media` WHERE `mediaType`=\"image\" AND `eventTimestamp`<DATE_SUB(CURDATE(), INTERVAL 2 DAY);");
	for(int i = 0; i < affected; ++i)
	{
		string	filename = IMAGE_TEMP_DIRECTORY + "/" + db->Get(i, "folder") + "/" + db->Get(i, "filename");

		if(isFileExists(filename))
		{
			{
				CLog	log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: deleting file [filename=" + filename + "]");
			}
			unlink(filename.c_str());
		}
		else
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) +  "]:ERROR: file doesn't exists  [filename=" + filename + "]");
		}
	}

	db->Query("DELETE FROM `temp_media` WHERE `mediaType`=\"image\" AND `eventTimestamp`<DATE_SUB(CURDATE(), INTERVAL 2 DAY);");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result = " + (result ? "true" : "false") + ")");
	}

	return result;
}

bool UpdateGiftsToGive(CMysql *db)
{
	bool	result = true;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	db->Query("DELETE FROM `gifts_to_give` WHERE `eventTimestamp`<UNIX_TIMESTAMP()-(`reserve_period`*60*60*24);");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result = " + (result ? "true" : "false") + ")");
	}

	return result;
}

int main()
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CMysql			user;
	CMysql			db;
	struct timeval	tv;

	{
		CLog	log;
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + "]: " + __FILE__);
	}

	signal(SIGSEGV, crash_handler);

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	try
	{

		if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: Can not connect to mysql database");
			throw CExceptionHTML("MySql connection");
		}

		//--- start of daily cron main functionality
		CleanupActivators(&db);

		//--- Remove temporarily media files
		RemoveTempMedia(&db);

		//--- Remove gifts reservation to give
		// UpdateGiftsToGive(&db);

		//--- Remove old captcha
		RemoveOldCaptcha();

		//--- update currency rate exchange
		UpdateCurrencyRateExchange(&db);

		//--- end of daily cron main functionality
	}
	catch(CExceptionHTML &c)
	{
		CLog	log;

		c.SetLanguage("ru");
		c.SetDB(&db);

		log.Write(DEBUG, string(__func__) + ": catch CExceptionHTML: DEBUG exception reason: [", c.GetReason(), "]");

		return(0);
	}
	catch(CException &c)
	{
		CLog 	log;

		log.Write(ERROR, string(__func__) + ": catch CException: exception: ERROR  ", c.GetReason());

		return(-1);
	}
	catch(exception& e)
	{
		CLog 	log;
		log.Write(ERROR, string(__func__) + ": catch(exception& e): catch standard exception: ERROR  ", e.what());

		return(-1);
	}

	return(0);
}

