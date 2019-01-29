#include "utilities.h"

void crash_handler(int sig)
{
	void	   *array[10];
	size_t		nptrs;
	char	   **strings;

	// get void*'s for all entries on the stack
	nptrs = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, nptrs, STDERR_FILENO);


	// --- try to print out to the CLog
	strings = backtrace_symbols(array, nptrs);
	if(strings)
	{
		for(unsigned int i = 0; i < nptrs; i++)
		{
			MESSAGE_ERROR("", "", string(strings[i]));
		}

		free(strings);
	}

	exit(1);
}

std::string rtrim(std::string& str)
{
	str.erase(str.find_last_not_of(' ')+1);		 //suffixing spaces
	return str;
}

std::string ltrim(std::string& str)
{
	str.erase(0, str.find_first_not_of(' '));	   //prefixing spaces
	return str;
}

std::string trim(std::string& str)
{
	rtrim(str);
	ltrim(str);
	return str;
}

string	quoted(string src)
{
	return '"' + src + '"';
}

string  toLower(string src)
{
	using namespace std::regex_constants;

	string  result = src;
	regex	r1("А");
	regex	r2("Б");
	regex	r3("В");
	regex	r4("Г");
	regex	r5("Д");
	regex	r6("Е");
	regex	r7("Ё");
	regex	r8("Ж");
	regex	r9("З");
	regex	r10("И");
	regex	r11("Й");
	regex	r12("К");
	regex	r13("Л");
	regex	r14("М");
	regex	r15("Н");
	regex	r16("О");
	regex	r17("П");
	regex	r18("Р");
	regex	r19("С");
	regex	r20("Т");
	regex	r21("У");
	regex	r22("Ф");
	regex	r23("Х");
	regex	r24("Ц");
	regex	r25("Ч");
	regex	r26("Ш");
	regex	r27("Щ");
	regex	r28("Ь");
	regex	r29("Ы");
	regex	r30("Ъ");
	regex	r31("Э");
	regex	r32("Ю");
	regex	r33("Я");

	src = regex_replace(src, r1, "а");
	src = regex_replace(src, r2, "б");
	src = regex_replace(src, r3, "в");
	src = regex_replace(src, r4, "г");
	src = regex_replace(src, r5, "д");
	src = regex_replace(src, r6, "е");
	src = regex_replace(src, r7, "ё");
	src = regex_replace(src, r8, "ж");
	src = regex_replace(src, r9, "з");
	src = regex_replace(src, r10, "и");
	src = regex_replace(src, r11, "й");
	src = regex_replace(src, r12, "к");
	src = regex_replace(src, r13, "л");
	src = regex_replace(src, r14, "м");
	src = regex_replace(src, r15, "н");
	src = regex_replace(src, r16, "о");
	src = regex_replace(src, r17, "п");
	src = regex_replace(src, r18, "р");
	src = regex_replace(src, r19, "с");
	src = regex_replace(src, r20, "т");
	src = regex_replace(src, r21, "у");
	src = regex_replace(src, r22, "ф");
	src = regex_replace(src, r23, "х");
	src = regex_replace(src, r24, "ц");
	src = regex_replace(src, r25, "ч");
	src = regex_replace(src, r26, "ш");
	src = regex_replace(src, r27, "щ");
	src = regex_replace(src, r28, "ь");
	src = regex_replace(src, r29, "ы");
	src = regex_replace(src, r30, "ъ");
	src = regex_replace(src, r31, "э");
	src = regex_replace(src, r32, "ю");
	src = regex_replace(src, r33, "я");

	transform(src.begin(), src.end(), result.begin(), (int(*)(int))tolower);

	return result;
}

string	GetPasswordNounsList(CMysql *db)
{
	string 	result = "";
	int		affected;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query("SELECT COUNT(*) as `total` FROM `password_dictionary_nouns`;");
	if(affected)
	{
		int		total_number_of_words = stoi(db->Get(0, "total")) - 1;

		affected = db->Query("SELECT * FROM `password_dictionary_nouns` WHERE `id` in (round(rand()*" + to_string(total_number_of_words) + ") + 1, round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1)");
		for(int i = 0; i < affected; ++i)
		{
			if(result.length()) result += ",";
			result += string("\"") + db->Get(i, "word") + "\"";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "password_dictionary_nouns is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	GetPasswordCharacteristicsList(CMysql *db)
{
	string 	result = "";
	int		affected;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query("SELECT COUNT(*) as `total` FROM `password_dictionary_characteristics`;");
	if(affected)
	{
		int		total_number_of_words = stoi(db->Get(0, "total"));

		affected = db->Query("SELECT * FROM `password_dictionary_characteristics` WHERE `id` in (round(rand()*" + to_string(total_number_of_words) + ") + 1, round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1)");
		for(int i = 0; i < affected; ++i)
		{
			if(result.length()) result += ",";
			result += string("\"") + db->Get(i, "word") + "\"";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "password_dictionary_characteristics is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	GetPasswordAdjectivesList(CMysql *db)
{
	string 	result = "";
	int		affected;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query("SELECT COUNT(*) as `total` FROM `password_dictionary_adjectives`;");
	if(affected)
	{
		int		total_number_of_words = stoi(db->Get(0, "total"));

		affected = db->Query("SELECT * FROM `password_dictionary_adjectives` WHERE `id` in (round(rand()*" + to_string(total_number_of_words) + ") + 1, round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1,round(rand()*" + to_string(total_number_of_words) + ") + 1)");
		for(int i = 0; i < affected; ++i)
		{
			if(result.length()) result += ",";
			result += string("\"") + db->Get(i, "word") + "\"";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "password_dictionary_adjectives is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string GetRandom(int len)
{
	string	result;
	int	i;

	for(i = 0; i < len; i++)
	{
		result += (char)('0' + (int)(rand()/(RAND_MAX + 1.0) * 10));
	}

	return result;
}

string DeleteHTML(string src, bool removeBR /* = true*/)
{
	string			  result;
	string::size_type   firstPos, lastPos;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start (src.len = " + to_string(src.length()) + ")");
	}

	result = src;
	firstPos = result.find("<");
	if(firstPos != string::npos)
	{
		lastPos = result.find(">", firstPos);
		if(lastPos == string::npos) lastPos = result.length();

		while(firstPos != string::npos)
		{
			if(removeBR)
				result.erase(firstPos, lastPos - firstPos + 1);
			else
			{
				string  htmlTag;

				// --- this will run in case "keep BR"
				htmlTag = result.substr(firstPos + 1, lastPos-firstPos-1);
				transform(htmlTag.begin(), htmlTag.end(),htmlTag.begin(), ::tolower);

				if(htmlTag != "br")
				{
					result.erase(firstPos, lastPos - firstPos + 1);
				}
				else
				{
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + "(" + src + ")" + "[" + to_string(__LINE__) + "]: keep <br> at pos: " + to_string(firstPos));
					}
				}
			}

			firstPos = result.find("<", firstPos + 1);
			if(firstPos == string::npos) break;
			lastPos = result.find(">", firstPos);
			if(lastPos == string::npos) lastPos = result.length();
		}
	} // --- if "<" fount in srcStr

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish(result.len = " + to_string(result.length()) + ")");
	}

	return result;
}

/*
	Delete symbol " from string src
*/
string RemoveQuotas(string src)
{
	string		result = src;
	string::size_type	pos = 0;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start (src = " + src + ")");
	}

	while((pos = result.find("\"", pos)) != string::npos)
	{
		result.replace(pos, 1, "\\\"");
		pos += 2;
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result = " + result + ")");
	}

	return result;
}

/*
	Delete special symbols like \t \\ \<
*/
string RemoveSpecialSymbols(string src)
{
	string		result = src;
	string::size_type	pos = 0;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start (src = " + src + ")");
	}

	while ((result.length()) && (result.at(result.length() - 1) == '\\')) result.replace(result.length() - 1, 1, "");

	pos = 0;
	while((pos = result.find("\\", pos)) != string::npos)
	{
		result.replace(pos, 2, "");
		pos += 2;
	}


	pos = 0;
	while((pos = result.find("\t", pos)) != string::npos)
	{
		result.replace(pos, 1, " ");
	}

	pos = 0;
	while((pos = result.find("№", pos)) != string::npos)
	{
		result.replace(pos, 1, "N");
		pos += 1;
	}

	pos = 0;
	while((pos = result.find("—", pos)) != string::npos)
	{
		result.replace(pos, 1, "-");
		pos += 1;
	}


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result = " + result + ")");
	}

	return result;
}

/*
	Delete special symbols like \t \\ \<
*/
string RemoveSpecialHTMLSymbols(string src)
{
	string		result = src;
	string::size_type	pos = 0;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	while ((result.length()) && (result.at(result.length() - 1) == '\\')) result.replace(result.length() - 1, 1, "");

	pos = 0;
	while((pos = result.find("\\", pos)) != string::npos)
	{
		result.replace(pos, 1, "&#92;");
	}


	pos = 0;
	while((pos = result.find("\t", pos)) != string::npos)
	{
		result.replace(pos, 1, " ");
	}

	pos = 0;
	while((pos = result.find("<", pos)) != string::npos)
	{
		result.replace(pos, 1, "&lt;");
	}

	pos = 0;
	while((pos = result.find(">", pos)) != string::npos)
	{
		result.replace(pos, 1, "&gt;");
	}

	pos = 0;
	while((pos = result.find("№", pos)) != string::npos)
	{
		result.replace(pos, 1, "&#35;");
	}

	pos = 0;
	while((pos = result.find("—", pos)) != string::npos)
	{
		result.replace(pos, 1, "-");
	}

	pos = 0;
	while((pos = result.find("\"", pos)) != string::npos)
	{
		result.replace(pos, 1, "&quot;");
	}


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result.length = " + to_string(result.length()) + ")");
	}

	return result;
}

/*
	Change " symbol to " from string src
*/
string ReplaceDoubleQuoteToQuote(string src)
{
	string		result = src;
	string::size_type	pos = 0;

	while((pos = result.find("\"", pos)) != string::npos)
	{
		result.replace(pos, 1, "'");
		// pos += 2;
	}

	return result;
}

/*
	Change CR/CRLF symbol to <BR> from string src
*/
string ReplaceCRtoHTML(string src)
{
	string		result = src;
	string::size_type	pos = 0;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}


	pos = 0;
	while((pos = result.find("\r\n", pos)) != string::npos)
	{
		result.replace(pos, 2, "<br>");
		// pos += 1;
	}

	pos = 0;
	while((pos = result.find("\n", pos)) != string::npos)
	{
		result.replace(pos, 1, "<bR>");
	}

	pos = 0;
	while((pos = result.find("\r", pos)) != string::npos)
	{
		result.replace(pos, 1, "<Br>");
	}


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish");
	}

	return result;
}

string CleanUPText(const string messageBody, bool removeBR/* = true*/)
{
	string	  result = messageBody;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: start"));
	}

	result = DeleteHTML(result, removeBR);
	result = ReplaceDoubleQuoteToQuote(result);
	result = ReplaceCRtoHTML(result);
	result = RemoveSpecialSymbols(result);
	trim(result);

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end"));
	}

	return result;
}

/*
	Delete any special symbols
	Used only for matching duplicates
*/
string RemoveAllNonAlphabetSymbols(const string &src)
{
	string		result = src;
	string::size_type	pos = 0;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: start"));
	}

	while ((result.length()) && (result.at(result.length() - 1) == '\\')) result.replace(result.length() - 1, 1, "");

	pos = 0;
	while((pos = result.find("&lt;", pos)) != string::npos)
	{
		result.replace(pos, 4, "");
	}
	pos = 0;
	while((pos = result.find("&gt;", pos)) != string::npos)
	{
		result.replace(pos, 4, "");
	}
	pos = 0;
	while((pos = result.find("&quot;", pos)) != string::npos)
	{
		result.replace(pos, 6, "");
	}
	pos = 0;
	while((pos = result.find("&#92;", pos)) != string::npos)
	{
		result.replace(pos, 5, "");
	}
	pos = 0;
	while((pos = result.find(" ", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}
	pos = 0;
	while((pos = result.find("\\", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}
	pos = 0;
	while((pos = result.find("/", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}
	pos = 0;
	while((pos = result.find("\t", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("<", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find(">", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("№", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("—", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("\"", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("'", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}
	pos = 0;
	while((pos = result.find(";", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}
	pos = 0;
	while((pos = result.find(":", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}


	pos = 0;
	while((pos = result.find("`", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find(".", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find(",", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("%", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}

	pos = 0;
	while((pos = result.find("-", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}
	pos = 0;
	while((pos = result.find("N", pos)) != string::npos)
	{
		result.replace(pos, 1, "");
	}


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end ( result length = " + to_string(result.length()) + ")"));
	}

	return result;
}


string ConvertTextToHTML(const string &messageBody)
{
	string 		result = messageBody;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: start"));
	}

	result = RemoveSpecialHTMLSymbols(result);
	result = DeleteHTML(result);
	result = ReplaceCRtoHTML(result);
	trim(result);

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end ( result length = " + to_string(result.length()) + ")"));
	}

	return result;
}

string CheckHTTPParam_Text(const string &srcText)
{
	char	convertBuffer[16384];
	string	result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: start param(" + srcText + ")"));
	}

	memset(convertBuffer, 0, sizeof(convertBuffer));
	convert_utf8_to_windows1251(srcText.c_str(), convertBuffer, sizeof(convertBuffer) - 1);
	result = ConvertTextToHTML(convertBuffer);

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end ( result length = " + to_string(result.length()) + ")"));
	}

	return	result;
}

auto CheckHTTPParam_Number(const string &srcText) -> string
{
	auto	result = ""s;

	{
		MESSAGE_DEBUG("", "", "start param(" + srcText + ")");
	}

	if(srcText.length())
	{
		smatch	m;
		regex	r("[^0-9]");

		if(regex_search(srcText, m, r))
		{
			MESSAGE_ERROR("", "", "can't convert(" + srcText + ") to number");
		}
		else
		{
			result = srcText;
		}
	}

	{
		MESSAGE_DEBUG("", "", "end ( result length = " + to_string(result.length()) + ")");
	}

	return	result;
}

string CheckHTTPParam_Float(const string &srcText)
{
	string	result = "";

	{
		MESSAGE_DEBUG("", "", "start param(" + srcText + ")");
	}

	if(srcText.length())
	{
		regex	r("^[0-9]+\\.?[0-9]+$");

		if(regex_match(srcText, r))
		{
			result = srcText;
		}
		else
		{
			MESSAGE_ERROR("", "", "can't convert(" + srcText + ") to float");
		}
	}

	{
		MESSAGE_DEBUG("", "", "end ( result length = " + to_string(result.length()) + ")");
	}

	return	result;
}

string CheckHTTPParam_Date(const string &srcText)
{
	string	result = "";

	{
		MESSAGE_DEBUG("", "", "start param(" + srcText + ")");
	}

	if(srcText.length())
	{
		regex	r("^([[:digit:]]{1,2})\\/([[:digit:]]{1,2})\\/([[:digit:]]{2,4})$");
		smatch	sm;

		if(regex_match(srcText, sm, r))
		{
			auto	date	= stoi(sm[1]);
			auto	month	= stoi(sm[2]);
			auto	year	= stoi(sm[3]);

			if(year < 100) year += 2000;

			if((1 <= date) && (date <= 31))
			{
				if((1 <= month) && (month <= 12))
				{
					if((1900 <= year) && (year <= 2100))
					{
						auto tm_obj = GetTMObject(srcText);

						mktime(&tm_obj);

						if((date == tm_obj.tm_mday) && (month == (tm_obj.tm_mon + 1)) && (year == (tm_obj.tm_year + 1900)))
						{
							result = srcText;
						}
						else
						{
							MESSAGE_ERROR("", "", "wrong date (" + srcText + " -> " + to_string(date) + "/" + to_string(month) + "/" + to_string(year) + " vs " + to_string(tm_obj.tm_mday) + "/" + to_string(tm_obj.tm_mon + 1) + "/" + to_string(tm_obj.tm_year + 1900) + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "year (" + to_string(year) + ") is out of range");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "month (" + to_string(month) + ") is out of range");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "date (" + to_string(date) + ") is out of range");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "(" + srcText + ") doesn't match date regex");
		}
	}

	{
		MESSAGE_DEBUG("", "", "end (result length = " + to_string(result.length()) + ")");
	}

	return	result;
}

string CheckHTTPParam_Email(const string &srcText)
{
	char		convertBuffer[16384];
	string		result = "";

    regex       positionRegex(".*([+-][[:digit:]]+\\.[[:digit:]]+)([+-][[:digit:]]+\\.[[:digit:]]+)([+-][[:digit:]]+\\.[[:digit:]]+).*");
    smatch      matchResult;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: start param(" + srcText + ")"));
	}

	memset(convertBuffer, 0, sizeof(convertBuffer));
	convert_utf8_to_windows1251(srcText.c_str(), convertBuffer, sizeof(convertBuffer) - 1);
	result = ConvertTextToHTML(convertBuffer);

//  if(regex_match(srcText, regex("^[[:alnum:]][._[:alnum:]]+[[:alnum:]]@[[:alnum:]][.-[:alnum:]]+[[:alnum:]].[[:alnum:]]{2,5}$") ))
	if(regex_match(srcText, regex("^[._[:alnum:]]+@[[:alnum:]][\\-.[:alnum:]]*[[:alnum:]]\\.[[:alnum:]]{2,5}$") ))
    {
    	if(result.length() > 128)
    	{
    		result = "";
    		MESSAGE_DEBUG("", "", "e-mail too long >128 symbols");
    	}
    	else
    	{
	    	// --- regex matched
    	}
    }
    else
    {
		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: email doesn't match regex " + result));
		}

    	result = "";
    }

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end ( result length = " + to_string(result.length()) + ")"));
	}

	return	result;
}

string CheckHTTPParam_Timeentry(const string &srcText)
{
	string			result = "";
	string			timeentry_string;
	vector<string>	timeentry_vec;

	MESSAGE_DEBUG("", "", "start param(" + srcText + ")");

	timeentry_string = CheckHTTPParam_Text(srcText);
	timeentry_vec = SplitTimeentry(timeentry_string);
	for(unsigned int i = 0; i < timeentry_vec.size(); ++i)
	{
		if(i) result += ",";
		result += (timeentry_vec.at(i) == "0" ? "" : timeentry_vec.at(i));
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end ( result length = " + result + ")"));
	}

	MESSAGE_DEBUG("", "", "finish param(" + result + ")");

	return	result;
}

/*
string CheckIfFurtherThanNow(string occupationStart_cp1251)
{
	time_t	  now_t, checked_t;
	// char		utc_str[100];
	struct tm   *local_tm, check_tm;
	ostringstream	ost;

	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): start";
		log.Write(DEBUG, ost.str());
	}


	now_t = time(NULL);
	local_tm = localtime(&now_t);
	if(local_tm == NULL)
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "CheckIfFurtherThanNow(now): ERROR in running localtime(&t)";
		log.Write(ERROR, ost.str());
	}

	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "CheckIfFurtherThanNow(now): now_t = " << now_t;
		log.Write(DEBUG, ost.str());
	}


	// now2_t = time(NULL);
	// check_tm = localtime(&now2_t);
	sscanf(occupationStart_cp1251.c_str(), "%4d-%2d-%2d", &check_tm.tm_year, &check_tm.tm_mon, &check_tm.tm_mday);
	check_tm.tm_year -= 1900;
	check_tm.tm_mon -= 1;
	check_tm.tm_hour = 23;
	check_tm.tm_min = 59;
	check_tm.tm_isdst = 0;	// --- Summer time is OFF. Be carefull with it.

	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): checked year = " << check_tm.tm_year << " checked month = " << check_tm.tm_mon << " checked day = " << check_tm.tm_mday << "";
		log.Write(DEBUG, ost.str());
	}

	checked_t = mktime(&check_tm);

	{
		CLog	log;
		ostringstream	ost;
		char	buffer[80];

		ost.str("");
		strftime(buffer,80,"check_tm: date regenerated: %02d-%b-%Y %T %Z  %I:%M%p.", &check_tm);
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): " << buffer << "";
		log.Write(DEBUG, ost.str());

		memset(buffer, 0, 80);
		strftime(buffer,80,"local_tm: date regenerated: %02d-%b-%Y %T %Z  %I:%M%p.", local_tm);
		ost.str("");
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): " << buffer << "";
		log.Write(DEBUG, ost.str());

		ost.str("");
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): difftime( now_t=" << now_t << ", checked_t=" << checked_t << ")";
		log.Write(DEBUG, ost.str());

		ost.str("");
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): difference = " << difftime(now_t, checked_t);
		log.Write(DEBUG, ost.str());
	}

	if(difftime(now_t, checked_t) <= 0)
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "CheckIfFurtherThanNow(" << occupationStart_cp1251 << "): clicked date further in futer than now, therefore considered as a 0000-00-00";
		log.Write(DEBUG, ost.str());

		return "0000-00-00";
	}

	return occupationStart_cp1251;
}
*/

string	GetDefaultActionFromUserType(string role, CMysql *db)
{
	string	result = GUEST_USER_DEFAULT_ACTION;

	MESSAGE_DEBUG("", "", "start");

	if(role == "subcontractor") result = LOGGEDIN_SUBCONTRACTOR_DEFAULT_ACTION;
	if(role == "agency") result = LOGGEDIN_AGENCY_DEFAULT_ACTION;
	if(role == "approver") result = LOGGEDIN_APPROVER_DEFAULT_ACTION;
	if(role == "no role") result = LOGGEDIN_NOROLE_DEFAULT_ACTION;

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

double GetSecondsSinceY2k()
{
	time_t	  timer;
	// struct tm   y2k = {0};
	double		seconds;
	double		secondsY2kUTC = 946684800;

	// y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	// y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	timer = time(NULL);  /* get current time; same as: timer = time(NULL)  */
	// seconds = difftime(timer,mktime(&y2k));
	seconds = timer - secondsY2kUTC;

	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetSecondsSinceY2k(): end (seconds since Y2k" << std::to_string(seconds) << ")";
		log.Write(DEBUG, ost.str());
	}

	return seconds;
}

string GetLocalFormattedTimestamp()
{
	time_t	  now_t;
	struct tm   *local_tm;
	char		buffer[80];
	string		result = "";

	now_t = time(NULL);
	local_tm = localtime(&now_t);
	if(local_tm == NULL)
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetLocalFormatedTimestamp(): ERROR in running localtime(&t)";
		log.Write(ERROR, ost.str());
	}

	memset(buffer, 0, 80);
	strftime(buffer,80,"%Y-%m-%02d %T", local_tm);
	result = buffer;


	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetLocalFormatedTimestamp(): end (" << result << ")";
		log.Write(DEBUG, ost.str());
	}

	return result;

}

// --- input format: "2015-06-10 00:00:00"
// --- return: number of second difference from now
double GetTimeDifferenceFromNow(const string timeAgo)
{
	time_t	  now_t, checked_t;
	// char		utc_str[100];
	struct tm   *local_tm, check_tm;
	ostringstream	ost;
/*
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetTimeDifferenceFromNow(" << timeAgo << "): start";
		log.Write(DEBUG, ost.str());
	}
*/

	now_t = time(NULL);
	local_tm = localtime(&now_t);
	if(local_tm == NULL)
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetTimeDifferenceFromNow(now): ERROR in running localtime(&t)";
		log.Write(ERROR, ost.str());
	}
/*
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetTimeDifferenceFromNow(now): now_t = " << now_t;
		log.Write(DEBUG, ost.str());
	}
*/

	// now2_t = time(NULL);
	// check_tm = localtime(&now2_t);
	sscanf(timeAgo.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &check_tm.tm_year, &check_tm.tm_mon, &check_tm.tm_mday, &check_tm.tm_hour, &check_tm.tm_min, &check_tm.tm_sec);
	check_tm.tm_year -= 1900;
	check_tm.tm_mon -= 1;
	check_tm.tm_isdst = 0;	// --- "Summer time" is OFF. Be carefull with it.
							// --- Russia is not using Daylight saving
							// --- USA is

	// --- For testing purposes try to use the same Daylight saving as in local clock
	// --- Test starts on 9/11. If there is no side effect, comment-out previous line.
	if(local_tm) check_tm.tm_isdst = local_tm->tm_isdst;	// --- "Summer time" is the same as locak clock.

/*
	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << "GetTimeDifferenceFromNow(" << timeAgo << "): checked year = " << check_tm.tm_year << " checked month = " << check_tm.tm_mon << " checked day = " << check_tm.tm_mday << " checked hour = " << check_tm.tm_hour << " checked min = " << check_tm.tm_min << " checked sec = " << check_tm.tm_sec;
		log.Write(DEBUG, ost.str());
	}
*/
	checked_t = mktime(&check_tm);

	{
		CLog	log;
		ostringstream	ost;
		char	buffer[80];

		ost.str("");
		strftime(buffer,80,"check_tm: date regenerated: %02d-%b-%Y %T %Z  %I:%M%p.", &check_tm);
		ost << "GetTimeDifferenceFromNow(" << timeAgo << "): " << buffer << "";
		log.Write(DEBUG, ost.str());

		memset(buffer, 0, 80);
		strftime(buffer,80,"local_tm: date regenerated: %02d-%b-%Y %T %Z  %I:%M%p.", local_tm);
		ost.str("");
		ost << "GetTimeDifferenceFromNow(" << timeAgo << "): " << buffer << "";
		log.Write(DEBUG, ost.str());

		ost.str("");
		ost << "GetTimeDifferenceFromNow(" << timeAgo << "): difftime( now_t=" << now_t << ", checked_t=" << checked_t << ")";
		log.Write(DEBUG, ost.str());

		ost.str("");
		ost << "GetTimeDifferenceFromNow(" << timeAgo << "): end (difference = " << difftime(now_t, checked_t) << ")";
		log.Write(DEBUG, ost.str());
	}

	return difftime(now_t, checked_t);
}

string GetMinutesDeclension(const int value)
{
	map<int, string> 	mapDeclension = {
		{1, "минуту"},
		{2, "минуты"},
		{3, "минут"}
	};

	string				result;

	if(value % 10 == 0) 						{ result = mapDeclension.at(3); };
	if(value % 10 == 1) 						{ result = mapDeclension.at(1); };
	if((value % 10 >= 2) and (value % 10 <= 4))	{ result = mapDeclension.at(2); };
	if((value % 10 >= 5) and (value % 10 <= 9))	{ result = mapDeclension.at(3); };
	if((value >= 5) and (value <= 20))			{ result = mapDeclension.at(3); };
	return result;
}


string GetHoursDeclension(const int value)
{
	map<int, string> 	mapDeclension = {
		{1, "час"},
		{2, "часа"},
		{3, "часов"}
	};
	string				result;

	if(value % 10 == 0) 						{ result = mapDeclension.at(3); };
	if(value % 10 == 1) 						{ result = mapDeclension.at(1); };
	if((value % 10 >= 2) and (value % 10 <= 4))	{ result = mapDeclension.at(2); };
	if((value % 10 >= 5) and (value % 10 <= 9))	{ result = mapDeclension.at(3); };
	if((value >= 5) and (value <= 20))			{ result = mapDeclension.at(3); };
	return result;
}

string GetDaysDeclension(const int value)
{
	map<int, string> 	mapDeclension = {
		{1, "день"},
		{2, "дня"},
		{3, "дней"}
	};
	string				result;

	if(value % 10 == 0) 						{ result = mapDeclension.at(3); };
	if(value % 10 == 1) 						{ result = mapDeclension.at(1); };
	if((value % 10 >= 2) and (value % 10 <= 4))	{ result = mapDeclension.at(2); };
	if((value % 10 >= 5) and (value % 10 <= 9))	{ result = mapDeclension.at(3); };
	if((value >= 5) and (value <= 20))			{ result = mapDeclension.at(3); };
	return result;
}

string GetMonthsDeclension(const int value)
{
	map<int, string> 	mapDeclension = {
		{1, "месяц"},
		{2, "месяца"},
		{3, "месяцев"}
	};
	string				result;

	if(value % 10 == 0) 						{ result = mapDeclension.at(3); };
	if(value % 10 == 1) 						{ result = mapDeclension.at(1); };
	if((value % 10 >= 2) and (value % 10 <= 4))	{ result = mapDeclension.at(2); };
	if((value % 10 >= 5) and (value % 10 <= 9))	{ result = mapDeclension.at(3); };
	if((value >= 5) and (value <= 20))			{ result = mapDeclension.at(3); };
	return result;
}

string GetYearsDeclension(const int value)
{
	map<int, string> 	mapDeclension = {
		{1, "год"},
		{2, "года"},
		{3, "лет"}
	};
	string				result;

	if(value % 10 == 0) 						{ result = mapDeclension.at(3); };
	if(value % 10 == 1) 						{ result = mapDeclension.at(1); };
	if((value % 10 >= 2) and (value % 10 <= 4))	{ result = mapDeclension.at(2); };
	if((value % 10 >= 5) and (value % 10 <= 9))	{ result = mapDeclension.at(3); };
	if((value >= 5) and (value <= 20))			{ result = mapDeclension.at(3); };
	return result;
}

// --- input format: "2015-06-10 00:00:00"
// --- return: human readable format
string GetHumanReadableTimeDifferenceFromNow (const string timeAgo)
{
	double			seconds = GetTimeDifferenceFromNow(timeAgo);
	double			minutes = seconds / 60;
	double			hours = minutes / 60;
	double			days = hours / 24;
	double			months = days / 30;
	double			years = months / 12;
	ostringstream	ost;

	ost.str("");
	if(years >= 1)
	{
		ost << (int)years << " " << GetYearsDeclension(years);
	}
	else if(months >= 1)
	{
		ost << (int)months << " " << GetMonthsDeclension(months);
	}
	else if(days >= 1)
	{
		ost << (int)days << " " << GetDaysDeclension(days);
	}
	else if(hours >= 1)
	{
		ost << (int)hours << " " << GetHoursDeclension(hours);
	}
	else
	{
		ost << (int)minutes << " " << GetMinutesDeclension(minutes);
	}

	ost << " назад.";


	// --- commented to reduce logs flooding
	{
		CLog	log;
		ostringstream	ost1;

		ost1.str("");
		ost1 << "string GetHumanReadableTimeDifferenceFromNow (" << timeAgo << "): sec difference (" << seconds << ") human format (" << ost.str() << ")" ;
		log.Write(DEBUG, ost1.str());
	}


	return ost.str();
}


/*
Copyright (c) <YEAR>, <OWNER>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions a
re met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in th
e documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from t
his software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT L
IMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIG
HT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AN
D ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
int convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n)
{
		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int k = 0;

		typedef struct ConvLetter {
				char	win1251;
				int	 unicode;
		} Letter;

		Letter  g_letters[] = {
		{(char)0x82, 0x201A}, // SINGLE LOW-9 QUOTATION MARK
		{(char)0x83, 0x0453}, // CYRILLIC SMALL LETTER GJE
		{(char)0x84, 0x201E}, // DOUBLE LOW-9 QUOTATION MARK
		{(char)0x85, 0x2026}, // HORIZONTAL ELLIPSIS
		{(char)0x86, 0x2020}, // DAGGER
		{(char)0x87, 0x2021}, // DOUBLE DAGGER
		{(char)0x88, 0x20AC}, // EURO SIGN
		{(char)0x89, 0x2030}, // PER MILLE SIGN
		{(char)0x8A, 0x0409}, // CYRILLIC CAPITAL LETTER LJE
		{(char)0x8B, 0x2039}, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
		{(char)0x8C, 0x040A}, // CYRILLIC CAPITAL LETTER NJE
		{(char)0x8D, 0x040C}, // CYRILLIC CAPITAL LETTER KJE
		{(char)0x8E, 0x040B}, // CYRILLIC CAPITAL LETTER TSHE
		{(char)0x8F, 0x040F}, // CYRILLIC CAPITAL LETTER DZHE
		{(char)0x90, 0x0452}, // CYRILLIC SMALL LETTER DJE
		{(char)0x91, 0x2018}, // LEFT SINGLE QUOTATION MARK
		{(char)0x92, 0x2019}, // RIGHT SINGLE QUOTATION MARK
		{(char)0x93, 0x201C}, // LEFT DOUBLE QUOTATION MARK
		{(char)0x94, 0x201D}, // RIGHT DOUBLE QUOTATION MARK
		{(char)0x95, 0x2022}, // BULLET
		{(char)0x96, 0x2013}, // EN DASH
		{(char)0x97, 0x2014}, // EM DASH
		{(char)0x99, 0x2122}, // TRADE MARK SIGN
		{(char)0x9A, 0x0459}, // CYRILLIC SMALL LETTER LJE
		{(char)0x9B, 0x203A}, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
		{(char)0x9C, 0x045A}, // CYRILLIC SMALL LETTER NJE
		{(char)0x9D, 0x045C}, // CYRILLIC SMALL LETTER KJE
		{(char)0x9E, 0x045B}, // CYRILLIC SMALL LETTER TSHE
		{(char)0x9F, 0x045F}, // CYRILLIC SMALL LETTER DZHE
		{(char)0xA0, 0x00A0}, // NO-BREAK SPACE
		{(char)0xA1, 0x040E}, // CYRILLIC CAPITAL LETTER SHORT U
		{(char)0xA2, 0x045E}, // CYRILLIC SMALL LETTER SHORT U
		{(char)0xA3, 0x0408}, // CYRILLIC CAPITAL LETTER JE
		{(char)0xA4, 0x00A4}, // CURRENCY SIGN
		{(char)0xA5, 0x0490}, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
		{(char)0xA6, 0x00A6}, // BROKEN BAR
		{(char)0xA7, 0x00A7}, // SECTION SIGN
		{(char)0xA8, 0x0401}, // CYRILLIC CAPITAL LETTER IO
		{(char)0xA9, 0x00A9}, // COPYRIGHT SIGN
		{(char)0xAA, 0x0404}, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
		{(char)0xAB, 0x00AB}, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		{(char)0xAC, 0x00AC}, // NOT SIGN
		{(char)0xAD, 0x00AD}, // SOFT HYPHEN
		{(char)0xAE, 0x00AE}, // REGISTERED SIGN
		{(char)0xAF, 0x0407}, // CYRILLIC CAPITAL LETTER YI
		{(char)0xB0, 0x00B0}, // DEGREE SIGN
		{(char)0xB1, 0x00B1}, // PLUS-MINUS SIGN
		{(char)0xB2, 0x0406}, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
		{(char)0xB3, 0x0456}, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
		{(char)0xB4, 0x0491}, // CYRILLIC SMALL LETTER GHE WITH UPTURN
		{(char)0xB5, 0x00B5}, // MICRO SIGN
		{(char)0xB6, 0x00B6}, // PILCROW SIGN
		{(char)0xB7, 0x00B7}, // MIDDLE DOT
		{(char)0xB8, 0x0451}, // CYRILLIC SMALL LETTER IO
		{(char)0xB9, 0x2116}, // NUMERO SIGN
		{(char)0xBA, 0x0454}, // CYRILLIC SMALL LETTER UKRAINIAN IE
		{(char)0xBB, 0x00BB}, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		{(char)0xBC, 0x0458}, // CYRILLIC SMALL LETTER JE
		{(char)0xBD, 0x0405}, // CYRILLIC CAPITAL LETTER DZE
		{(char)0xBE, 0x0455}, // CYRILLIC SMALL LETTER DZE
		{(char)0xBF, 0x0457} // CYRILLIC SMALL LETTER YI
	};

		typedef struct ConvLetter3Bytes {
				char	win1251;
				char	unicode1;
				char	unicode2;
				char	unicode3;
		} Letter3Bytes;
		Letter3Bytes g_lettersGeneralPunctuation[] = {
			{'-', (char)0xe2, (char)0x80, (char)0x94}, // EM DASH
			{'-', (char)0xe2, (char)0x80, (char)0x93}, // EN DASH
			{'-', (char)0xe2, (char)0x80, (char)0x90}, // HYPHEN
			{'-', (char)0xe2, (char)0x80, (char)0x91}, // NON-BREAKING HYPHEN
			{'-', (char)0xe2, (char)0x80, (char)0x92}, // FIGURE DASH
			{'-', (char)0xe2, (char)0x80, (char)0x93}, // EN DASH
			{'-', (char)0xe2, (char)0x80, (char)0x94}, // EM DASH
			{'-', (char)0xe2, (char)0x80, (char)0x95}, // HORIZONTAL BAR
			{'|', (char)0xe2, (char)0x80, (char)0x96}, // DOUBLE VERTICAL LINE
			{'_', (char)0xe2, (char)0x80, (char)0x97}, // DOUBLE LOW LINE
			{',', (char)0xe2, (char)0x80, (char)0x9a}, // SINGLE LOW QUOTATION MARK
			{'.', (char)0xe2, (char)0x80, (char)0xa4}, // ONE DOT LEADER
			{'.', (char)0xe2, (char)0x80, (char)0x5}, // TWO DOT LEADER
			{'.', (char)0xe2, (char)0x80, (char)0x6}, // HORIZONTAL ELLIPSIS
			{'<', (char)0xe2, (char)0x80, (char)0xb9}, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
			{'>', (char)0xe2, (char)0x80, (char)0xba}, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
			{'!', (char)0xe2, (char)0x80, (char)0xbc}, // DOUBLE EXCLAMNATION MARK
			{'-', (char)0xe2, (char)0x81, (char)0x83}, // HYPHEN BULLET
			{'/', (char)0xe2, (char)0x81, (char)0x84}, // FRACTION SLASH
			{'?', (char)0xe2, (char)0x81, (char)0x87}, // DOUBLE QUESTION MARK
			{'*', (char)0xe2, (char)0x81, (char)0x8e}, // LOW ASTERISK
			{'|', (char)0xe2, (char)0x83, (char)0x92}, // COMBINING LONG VERTICAL LINE OVERLAY
			{'\\', (char)0xe2, (char)0x83, (char)0xa5}, //COMBINING REVERSE SOLIDUS OVERLAY
			{'|', (char)0xe2, (char)0x83, (char)0xa6}, // COMBINING DOUBLE VERTICAL STROKE OVERLAY
			{'/', (char)0xe2, (char)0x83, (char)0xab}, // COMBINING LONG DOUBLE SOLIDUS OVERLAY
			{'\'', (char)0xe2, (char)0x80, (char)0x9b}, // SINGLE HIGN-REVERSED-9 QUATATION MARK
			{'\\', (char)0xe2, (char)0x80, (char)0xb2}, // PRIME
			{'\\', (char)0xe2, (char)0x80, (char)0xb5}, // REVERSED PRIME
			{'"', (char)0xe2, (char)0x80, (char)0x9c}, // LEFT DOUBLE QUATATION MARK
			{'"', (char)0xe2, (char)0x80, (char)0xb3}, // DOUBLE PRIME
			{'"', (char)0xe2, (char)0x80, (char)0xb6}, // REVERSED DOUBLE PRIME
			{'"', (char)0xe2, (char)0x80, (char)0x9c}, // RIGHT DOUBLE QUATATION MARK
			{'"', (char)0xe2, (char)0x80, (char)0x9e}, // DOUBLE LOW-9 QUATATION MARK
			{'"', (char)0xe2, (char)0x80, (char)0x9f}, // DOUBLE HIGH-REVERSED-9 QUATATION MARK
			{'*', (char)0xe2, (char)0x80, (char)0xA2}, //  BULLET
			{'%', (char)0xe2, (char)0x80, (char)0xb0}, // PER MILLE SIGN
			{'%', (char)0xe2, (char)0x80, (char)0xb1}, // PER TEN THOUSAND SIGN
			{'*', (char)0xe2, (char)0x80, (char)0xa7}, // HYPHENATION POINT
			{'~', (char)0xe2, (char)0x81, (char)0x93}, // SWUNG DASH
			{'*', (char)0xe2, (char)0x81, (char)0x95}, // FLOWER PUNCTION MARK
			{':', (char)0xe2, (char)0x81, (char)0x9a}, // TWO DOT PUNCTION		};
			{'|', (char)0xe2, (char)0x83, (char)0x92}, // COMBINING LONG VERTICAL LINE OVERLAY
			{'\\', (char)0xe2, (char)0x83, (char)0xa5}, // COMBINING REVERSE SOLIDUS OVERLAY
			{'|', (char)0xe2, (char)0x83, (char)0xa6}, // COMBINING DOUBLE VERTICAL STROKE OVERLAY
			{'/', (char)0xe2, (char)0x83, (char)0xab}, // COMBINING LONG DOUBLE SOLIDUS OVERLAY
			{'*', (char)0xe2, (char)0x83, (char)0xb0}, // COMBINING ASTERISK ABOVE
			{'#', (char)0xe2, (char)0x84, (char)0x96}, // number sign
			{'1', (char)0xe2, (char)0x85, (char)0xa0}, // ROMAN NUMERAL ONE
			{'2', (char)0xe2, (char)0x85, (char)0xa1}, // ROMAN NUMERAL TWO
			{'3', (char)0xe2, (char)0x85, (char)0xa2}, // ROMAN NUMERAL THREE
			{'4', (char)0xe2, (char)0x85, (char)0xa3}, // ROMAN NUMERAL FOUR
			{'5', (char)0xe2, (char)0x85, (char)0xa4}, // ROMAN NUMERAL FIVE
			{'6', (char)0xe2, (char)0x85, (char)0xa5}, // ROMAN NUMERAL SIX
			{'7', (char)0xe2, (char)0x85, (char)0xa6}, // ROMAN NUMERAL SEVEN
			{'8', (char)0xe2, (char)0x85, (char)0xa7}, // ROMAN NUMERAL EIGHT
			{'9', (char)0xe2, (char)0x85, (char)0xa8}, // ROMAN NUMERAL NINE
			{'X', (char)0xe2, (char)0x85, (char)0xa9}, // ROMAN NUMERAL TEN
			{'L', (char)0xe2, (char)0x85, (char)0xac}, // ROMAN NUMERAL FIFTY
			{'C', (char)0xe2, (char)0x85, (char)0xad}, // ROMAN NUMERAL ONE HUNDRED
			{'D', (char)0xe2, (char)0x85, (char)0xae}, // ROMAN NUMERAL FIVE HUNDRED
			{'M', (char)0xe2, (char)0x85, (char)0xaf} // ROMAN NUMERAL ONE THOUSAND
		};
	{
		CLog	log;
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + string("]:") + string(": start"));
	}


		for(; i < (unsigned int)n && utf8[i] != 0; ++i) {
				char prefix = utf8[i];
				char suffix = utf8[i+1];
				if ((prefix & 0x80) == 0)
				{
						windows1251[j] = (char)prefix;
						++j;
				}
				else if ((~prefix) & 0x20)
				{
						int first5bit = prefix & 0x1F;
						first5bit <<= 6;
						int sec6bit = suffix & 0x3F;
						int unicode_char = first5bit + sec6bit;


						if ( unicode_char >= 0x410 && unicode_char <= 0x44F ) {
								windows1251[j] = (char)(unicode_char - 0x350);
						} else if (unicode_char >= 0x80 && unicode_char <= 0xFF) {
								windows1251[j] = (char)(unicode_char);
						} else if (unicode_char >= 0x402 && unicode_char <= 0x403) {
								windows1251[j] = (char)(unicode_char - 0x382);
						} else {
								unsigned int count = sizeof(g_letters) / sizeof(Letter);
								for (k = 0; k < count; ++k) {
										if (unicode_char == g_letters[k].unicode) {
												windows1251[j] = g_letters[k].win1251;
												goto NEXT_LETTER;
										}
								}
								// can't convert this char
								{
									CLog	log;
									ostringstream	ost;

									ost.str("");
									ost << __func__ << "[" << __LINE__ << "]:" << ": symbol at position " << i << " [" << hex << (prefix & 0xFF) << " " << (suffix & 0xFF) << "] doesn't belongs to UTF-8 cyrillic range (U+0400 ... U+04FF)";
									log.Write(DEBUG, ost.str());
								}
								return 0;
						}
NEXT_LETTER:
						++i;
						++j;
				}
				else if((prefix == static_cast<char>(0xf0)) and (suffix == static_cast<char>(0x9f)))
				{
					// --- emoji part (f0 9f 98 80 - f0 9f 9b b3) or (U+1F600 - U+1F6FF)
					// --- 4 bytes long
					if((i + 3) < n)
					{
						{
							CLog	log;
							ostringstream   ost;

							ost.str("");
							ost << __func__ << "[" << __LINE__ << "]:" << ": emojy detected [" << hex << (utf8[i] & 0xFF) << " " << (utf8[i+1] & 0xFF) << " " << (utf8[i+2] & 0xFF) << " " << (utf8[i+3] & 0xFF) << "]";
							log.Write(DEBUG, ost.str());
						}
						i += 3;
					}
					else
					{
						{
							CLog	log;
							ostringstream   ost;

							ost.str("");
							ost << __func__ << "[" << __LINE__ << "]:" << ": ERROR: emojy detected but dst string not lenghty enough, dst buffer size(" << n << ") < current position (" << i << "+3)";
							log.Write(ERROR, ost.str());
						}
						return 0;
					}
				}
				else if((prefix == static_cast<char>(0xe2)) && ((suffix & 0x80) == 0x80))
				{
					// --- general punctuation (e2 80 80 - e2 82 bf) or (U+2000 - U+20FF)
					// --- 3 bytes long
					if((i + 2) < n)
					{
						unsigned int	count = sizeof(g_lettersGeneralPunctuation) / sizeof(Letter3Bytes);
						bool			isFound = false;

						for (k = 0; ((k < count) && !isFound); ++k)
						{
								char currentSymb1 = prefix;
								char currentSymb2 = suffix;
								char currentSymb3 = utf8[i+2];

								if(
									(currentSymb1 == g_lettersGeneralPunctuation[k].unicode1) &&
									(currentSymb2 == g_lettersGeneralPunctuation[k].unicode2) &&
									(currentSymb3 == g_lettersGeneralPunctuation[k].unicode3)
								  )
								{
										windows1251[j] = g_lettersGeneralPunctuation[k].win1251;
										isFound = true;
										i += 2;
										j++;
								}
						}
						if(!isFound)
						{
							// --- unknown symbol
							// --- replace it to space
							windows1251[j] = ' ';
							// --- this is 3 bytes length symbol
							i += 2;
							j++;

							// can't convert this char
							CLog	log;
							ostringstream   ost;

							ost.str("");
							ost << __func__ << "[" << __LINE__ << "]:" << ": symbol at position " << (i - 2) << " [" << hex << (prefix & 0xFF) << " " << (suffix & 0xFF) << " " << (utf8[i+2 - 2] & 0xFF) << "] not found in mapping table Punctuation range (U+0400 ... U+04FF)";
							log.Write(DEBUG, ost.str());
						}
						else
						{
							CLog	log;
							ostringstream   ost;

							ost.str("");
							ost << __func__ << "[" << __LINE__ << "]:" << ": general punctuation detected [" << hex << (prefix & 0xFF) << " " << (suffix & 0xFF) << " " << (utf8[i+2 - 2] & 0xFF) << "] at position " << (i - 2);
							log.Write(DEBUG, ost.str());
						}
					}
					else
					{
						{
							CLog	log;
							ostringstream   ost;

							ost.str("");
							ost << __func__ << "[" << __LINE__ << "]:ERROR: general punctuation detected but dst string not lenghty enough, dst buffer size(" << n << ") < current position (" << i << "+2)";
							log.Write(ERROR, ost.str());
						}
						return 0;
					}

				}
				else
				{
					// can't convert this chars
					{
						CLog	log;
						ostringstream   ost;

						ost.str("");
						ost << __func__ << "[" << __LINE__ << "]:ERROR: can't convert this " << i << "-rd/th char " << hex << (+prefix & 0xFF) << " " << (+suffix & 0xFF);
						log.Write(ERROR, ost.str());
					}
					return 0;
				}
		}
		windows1251[j] = 0;

	{
		CLog	log;
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + string("]:") + string(": end"));
	}

		return 1;
}

// --- wrapper to convert_utf8_to_windows1251
auto	utf8_to_cp1251(const string &src) -> string
{
	char	convertBuffer[1024];
	auto	result = 0;
	
	MESSAGE_DEBUG("", "", "start");

	memset(convertBuffer, 0, sizeof(convertBuffer));

	result = convert_utf8_to_windows1251(gettext(src.c_str()), convertBuffer, sizeof(convertBuffer));

	MESSAGE_DEBUG("", "", "finish (conversation result is " + to_string(result) + ")");

	return convertBuffer;
}


bool convert_cp1251_to_utf8(const char *in, char *out, int size)
{
	const char table[129*3] = {
		"\320\202 \320\203 \342\200\232\321\223 \342\200\236\342\200\246\342\200\240\342\200\241"
		"\342\202\254\342\200\260\320\211 \342\200\271\320\212 \320\214 \320\213 \320\217 "
		"\321\222 \342\200\230\342\200\231\342\200\234\342\200\235\342\200\242\342\200\223\342\200\224"
		"   \342\204\242\321\231 \342\200\272\321\232 \321\234 \321\233 \321\237 "
		"\302\240 \320\216 \321\236 \320\210 \302\244 \322\220 \302\246 \302\247 "
		"\320\201 \302\251 \320\204 \302\253 \302\254 \302\255 \302\256 \320\207 "
		"\302\260 \302\261 \320\206 \321\226 \322\221 \302\265 \302\266 \302\267 "
		"\321\221 \342\204\226\321\224 \302\273 \321\230 \320\205 \321\225 \321\227 "
		"\320\220 \320\221 \320\222 \320\223 \320\224 \320\225 \320\226 \320\227 "
		"\320\230 \320\231 \320\232 \320\233 \320\234 \320\235 \320\236 \320\237 "
		"\320\240 \320\241 \320\242 \320\243 \320\244 \320\245 \320\246 \320\247 "
		"\320\250 \320\251 \320\252 \320\253 \320\254 \320\255 \320\256 \320\257 "
		"\320\260 \320\261 \320\262 \320\263 \320\264 \320\265 \320\266 \320\267 "
		"\320\270 \320\271 \320\272 \320\273 \320\274 \320\275 \320\276 \320\277 "
		"\321\200 \321\201 \321\202 \321\203 \321\204 \321\205 \321\206 \321\207 "
		"\321\210 \321\211 \321\212 \321\213 \321\214 \321\215 \321\216 \321\217 "
	};
	int	counter = 0;
	bool result = true;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	while (*in)
		if (*in & 0x80) {
			const char *p = &table[3 * (0x7f & *in++)];
			if (*p == ' ')
				continue;
			counter += 2;
			if(counter > size)
			{
				result = false;
				break;
			}
			*out++ = *p++;
			*out++ = *p++;

			if (*p == ' ')
				continue;

			counter++;
			if(counter > size)
			{
				result = false;
				break;
			}
			*out++ = *p++;
		}
		else
		{
			counter++;
			if(counter > size)
			{
				result = false;
				break;
			}
			*out++ = *in++;
		}
	*out = 0;

	if(!result)
	{
		CLog	log;
		log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: ERROR: buffer size is not enough");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}
	return result;
}

string	FilterCP1251Symbols(const string &src)
{
	auto	result = ""s;

	for(auto n:src)
	{
		if((unsigned int)(n) > 127) result += "_";
		else result += n;
	}

	return result;
}

bool isFileExists(const std::string& name)
{
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

bool isDirExists(const std::string& name)
{
	struct stat buffer;
	auto		result = false;

	MESSAGE_DEBUG("", "", "start");

	if(stat(name.c_str(), &buffer) != 0)
	{
		MESSAGE_DEBUG("", "", "can't access " + name);
	}
	else if(buffer.st_mode & S_IFDIR)
	{
		result = true;
	}
	else
	{
		MESSAGE_DEBUG("", "", name + " isn't directory");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

bool CreateDir(const string &dir)
{
	auto		result = false;
	struct stat buffer;

	MESSAGE_DEBUG("", "", "start");

	if(stat(dir.c_str(), &buffer) != 0)
	{
		if ((mkdir(dir.c_str(), 0750) != 0) && (errno != EEXIST))
		{
			MESSAGE_ERROR("", "", "can't create directory(" + dir + ")");
		}
		else
			result = true;
	}
	else
	{
		MESSAGE_ERROR("", "", "dir(" + dir + ") already exists")
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

bool RmDirRecursive(const char *dirname)
{
	DIR		*dir;
	struct	dirent *entry;
	char	path[4096];

	MESSAGE_DEBUG("", "", "start (" + dirname + ")");
	
	dir = opendir(dirname);

	if(dir == NULL) 
	{
		MESSAGE_ERROR("", "", "fail to open dir("s + dirname + ")")
		return false;
	}
	
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) 
		{
			snprintf(path, (size_t) 4095, "%s/%s", dirname, entry->d_name);

			if (entry->d_type == DT_DIR)
			{
				RmDirRecursive(path);
			} 
			else
			{
				MESSAGE_DEBUG("", "", "remove file " + path);
				// unlink(path);
			}
		}
	}
	
	closedir(dir);
	
	MESSAGE_DEBUG("", "", "remove dir " + dirname);
	// rmdir(dirname);
	
	MESSAGE_DEBUG("", "", "finish (" + dirname + ")");

	return true;
}

string SymbolReplace(const string where, const string src, const string dst)
{
	auto				  result = ""s;
	string::size_type	  pos;

	result = where;

	pos = result.find(src);
	while(pos != string::npos)
	{
		result.replace(pos, src.length(), dst);
		pos = result.find(src, pos + 1);
	}
	return result;
}

auto SymbolReplace_KeepDigitsOnly(const string &where) -> string
{
	auto				  	result = where;
	unsigned int			i = 0;

	while(i < result.length())
	{
		auto	currSymb = result.at(i);

		if((currSymb >= static_cast<char>('0')) && (currSymb <= static_cast<char>('9')))
			i++;
		else
			result.replace(i, 1, "");
	}

	return result;
}

bool CheckUserEmailExisting(string userNameToCheck, CMysql *db) {
	CUser		user;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	user.SetDB(db);
	user.SetLogin(userNameToCheck);
	user.SetEmail(userNameToCheck);

	if(user.isLoginExist() or user.isEmailDuplicate()) {
		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: login or email already registered");
		}
		return true;
	}
	else {
		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: login or email not yet exists");
		}
		return false;
	}
}

// --- Quote Words: split string into vector<string> words
// --- input: string, reference to vector
// --- output: success status
//				1 - success
//				0 - fail
/*
int	qw(const string src, vector<string> &dst)
{
	std::size_t	prevPointer = 0, nextPointer;
	string		trimmedStr = src;
	int			wordCounter = 0;

	trim(trimmedStr);

	prevPointer = 0, wordCounter = 0;
	do
	{
		nextPointer = trimmedStr.find(" ", prevPointer);
		if(nextPointer == string::npos)
		{
			dst.push_back(trimmedStr.substr(prevPointer));
		}
		else
		{
			dst.push_back(trimmedStr.substr(prevPointer, nextPointer - prevPointer));
		}
		prevPointer = nextPointer + 1;
		wordCounter++;
	} while( (nextPointer != string::npos) );

	return 1;
}
*/
int	qw(const string src, vector<string> &dst)
{
	istringstream iss(src);

    do
    {
        string word;

        iss >> word;
        if(word.length()) dst.push_back(word);
    } while (iss);

	return 1;
}

vector<string> split(const string& s, const char& c)
{
	string buff{""};
	vector<string> v;

	for(auto n:s)
	{
		if(n != c) buff+=n;
		else if(n == c && buff != "")
		{
			v.push_back(buff);
			buff = "";
		}
	}
	if(buff.length()) v.push_back(buff);

	return v;
}

 string join(const vector<string>& vec, string separator)
{
	string result{""};

	if(separator.empty()) separator = ",";

	for(auto &item:vec)
	{
		if(result.length()) result += separator;
		result += item;
	}

	return result;
}

string UniqueUserIDInUserIDLine(string userIDLine) //-> decltype(static_cast<string>("123"))
{
	list<long int>	listUserID;
	string			result {""};
	std::size_t		prevPointer {0}, nextPointer;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start (", userIDLine, ")");
	}

	do
	{
		nextPointer = userIDLine.find(",", prevPointer);
		if(nextPointer == string::npos)
		{
			listUserID.push_back(atol(userIDLine.substr(prevPointer).c_str()));
		}
		else
		{
			listUserID.push_back(atol(userIDLine.substr(prevPointer, nextPointer - prevPointer).c_str()));
		}
		prevPointer = nextPointer + 1;
	} while( (nextPointer != string::npos) );

	listUserID.sort();
	listUserID.unique();
	for(auto it: listUserID)
	{
		result += (result.length() ? "," : "") + to_string(it);
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result ", result, ")");
	}

	return result;
}

string GetChatMessagesInJSONFormat(string dbQuery, CMysql *db)
{
	ostringstream	result, ost;
	int				affected;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}
	result.str("");

	affected = db->Query(dbQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			result << (i ? "," : "") << "{\
				\"id\": \""						<< db->Get(i, "id") << "\", \
				\"message\": \"" 				<< ReplaceDoubleQuoteToQuote(db->Get(i, "message")) << "\", \
				\"fromType\": \"" 				<< db->Get(i, "fromType") << "\",\
				\"fromID\": \""					<< db->Get(i, "fromID") << "\",\
				\"toType\": \""			 		<< db->Get(i, "toType") << "\",\
				\"toID\": \""	 				<< db->Get(i, "toID") << "\",\
				\"messageStatus\": \""		  << db->Get(i, "messageStatus") << "\",\
				\"messageType\": \""			<< db->Get(i, "messageType") << "\",\
				\"eventTimestampDelta\": \""	<< GetHumanReadableTimeDifferenceFromNow(db->Get(i, "eventTimestamp")) << "\",\
				\"secondsSinceY2k\": \""		<< db->Get(i, "secondsSinceY2k") << "\",\
				\"eventTimestamp\": \""			<< db->Get(i, "eventTimestamp") << "\"\
			}";
		}
	}

	{
		CLog	log;
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + string("]: end"));
	}

	return  result.str();
}

bool	isFilenameImage(string filename)
{
	bool	result = false;
	regex   e1("[.](gif|jpg|jpeg|png)$", regex_constants::icase);
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "(" + filename + ")[" + to_string(__LINE__) + string("]: start" ));
	}

	result = regex_search(filename, e1);

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end (result: ") + (result ? "true" : "false") + ")" );
	}
	return  result;
}

bool	isFilenameVideo(string filename)
{
	bool	result = false;
	regex   e1("[.](mov|avi|mp4|webm)$", regex_constants::icase);
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "(" + filename + ")[" + to_string(__LINE__) + string("]: start" ));
	}

	result = regex_search(filename, e1);

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: end (result: ") + (result ? "true" : "false") + ")" );
	}
	return  result;
}

// --- extrasct all @[[:digit:]] patterns form srcMessage
vector<string> GetUserTagsFromText(string srcMessage)
{
	vector<string>  result;
	regex		   exp1("@([[:digit:]]+)");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]: start"));
	}

	regex_token_iterator<string::iterator>   rItr(srcMessage.begin(), srcMessage.end(), exp1, 1);
	regex_token_iterator<string::iterator>   rItrEnd;
	while(rItr != rItrEnd)
	{
		result.push_back(rItr->str());
		++rItr;
	}

	{
		CLog	log;
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + string("]: end (result length: ") + to_string(result.size()) + string(")"));
	}

	return result;
}

// --- Returns user list in JSON format grabbed from DB
// --- Input: dbQuery - SQL format returns users
//			db	  - DB connection
//			user	- current user object
string GetUserListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	ostringstream					 ost, ost1;
	string							 sessid, lookForKey;
	unordered_set<unsigned long>	 setOfUserID;

	struct	ItemClass
	{
		string	userID;
		string	userLogin;
		string	userName;
		string	userNameLast;
		string	userSex;
		string	userBirthday;
		string	userBirthdayAccess;
		string	userAppliedVacanciesRender;
		string	userCurrentCityID;
		string	userLastOnline;
		string	userLastOnlineSecondSinceY2k;
	};
	vector<ItemClass>		itemsList;
	int						itemsCount;


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if((itemsCount = db->Query(dbQuery)) > 0)
	{
		for(int i = 0; i < itemsCount; ++i)
		{
			ItemClass	item;
			item.userID = db->Get(i, "id");
			item.userLogin = db->Get(i, "login");
			item.userName = db->Get(i, "name");
			item.userNameLast = db->Get(i, "nameLast");
			item.userSex = db->Get(i, "sex");
			item.userBirthday = db->Get(i, "birthday");
			item.userBirthdayAccess = db->Get(i, "birthdayAccess");
			item.userAppliedVacanciesRender = db->Get(i, "appliedVacanciesRender");
			item.userCurrentCityID = db->Get(i, "geo_locality_id");
			item.userLastOnline = db->Get(i, "last_online");
			item.userLastOnlineSecondSinceY2k = db->Get(i, "last_onlineSecondsSinceY2k");

			itemsList.push_back(item);
		}


		ost.str("");
		for(int i = 0; i < itemsCount; i++)
		{

			if(setOfUserID.find(stol(itemsList[i].userID)) == setOfUserID.end())
			{
				auto				userID = ""s;
				auto				userLogin = ""s;
				auto				userName = ""s;
				auto				userNameLast = ""s;
				auto				userSex = ""s;
				auto				userBirthday = ""s;
				auto				userBirthdayAccess = ""s;
				auto				userCurrentEmployment = ""s;
				auto				userCurrentCityID = ""s;
				auto				userCurrentCity = ""s;
				auto				avatarPath = ""s;
				auto				userAppliedVacanciesRender = ""s;
				auto				userLastOnline = ""s;
				auto				numberUreadMessages = ""s;
				auto				userLastOnlineSecondSinceY2k = ""s;
				auto				userFriendship = ""s;
				ostringstream		ost1;
				// auto				affected1 = 0;

				userID = itemsList[i].userID;
				userLogin = itemsList[i].userLogin;
				userName = itemsList[i].userName;
				userNameLast = itemsList[i].userNameLast;
				userSex = itemsList[i].userSex;
				userBirthday = itemsList[i].userBirthday;
				userBirthdayAccess = itemsList[i].userBirthdayAccess;
				userAppliedVacanciesRender = itemsList[i].userAppliedVacanciesRender;
				userCurrentCityID = itemsList[i].userCurrentCityID;
				userLastOnline = itemsList[i].userLastOnline;
				userLastOnlineSecondSinceY2k = itemsList[i].userLastOnlineSecondSinceY2k;

				setOfUserID.insert(atol(userID.c_str()));

/*
				// --- Defining title and company of user
				ost1.str("");
				ost1 << "SELECT `company_position`.`title` as `users_company_position_title`,  "
						"`company`.`name` as `company_name`, `company`.`id` as `company_id`  "
						"FROM `users_company` "
						"LEFT JOIN  `company_position` ON `company_position`.`id`=`users_company`.`position_title_id` "
						"LEFT JOIN  `company` ON `company`.`id`=`users_company`.`company_id` "
						"WHERE `users_company`.`user_id`=\"" << userID << "\" and `users_company`.`current_company`='1' "
						"ORDER BY  `users_company`.`occupation_start` DESC ";

				affected1 = db->Query(ost1.str());
				ost1.str("");
				ost1 << "[";
				if(affected1 > 0)
				{
					for(int j = 0; j < affected1; j++)
					{
						ost1 << "{ \
								\"companyID\": \"" << db->Get(j, "company_id") << "\", \
								\"company\": \"" << db->Get(j, "company_name") << "\", \
								\"title\": \"" << db->Get(j, "users_company_position_title") << "\" \
								}";
						if(j < (affected1 - 1)) ost1 << ", ";
					}
				}
				ost1 << "]";
				userCurrentEmployment = ost1.str();

				MESSAGE_DEBUG("", "", "done with building employment list (length is " + to_string(userCurrentEmployment.length()) + ")");
*/

				// --- Get user avatars
				ost1.str("");
				ost1 << "select * from `users_avatars` where `userid`='" << userID << "' and `isActive`='1';";
				avatarPath = "empty";
				if(db->Query(ost1.str()))
				{
					ost1.str("");
					ost1 << "/images/avatars/avatars" << db->Get(0, "folder") << "/" << db->Get(0, "filename");
					avatarPath = ost1.str();
				}

				// --- Get friendship status
				userFriendship = "empty";
				if(user && db->Query("select * from `users_friends` where `userid`='" + user->GetID() + "' and `friendID`='" + userID + "';"))
					userFriendship = db->Get(0, "state");

/*
				if(db->Query("select COUNT(*) as `number_unread_messages` from `chat_messages` where `fromType`='fromUser' and `fromID`='" + userID + "' and (`messageStatus`='unread' or `messageStatus`='sent' or `messageStatus`='delivered');"))
					numberUreadMessages = db->Get(0, "number_unread_messages");
*/
				if(userCurrentCityID.length() && db->Query("SELECT `title` FROM `geo_locality` WHERE `id`=\"" + userCurrentCityID + "\";"))
					userCurrentCity = db->Get(0, "title");

				if(ost.str().length()) ost << ", ";

				if(userBirthdayAccess == "private") userBirthday = "";

				ost << "{ \"id\": \""							<< userID << "\", "
						  "\"name\": \""						<< userName << "\", "
						  "\"nameLast\": \""					<< userNameLast << "\","
						  "\"userSex\": \""						<< userSex << "\","
						  "\"birthday\": \""					<< userBirthday << "\","
						  "\"birthdayAccess\": \""				<< userBirthdayAccess << "\","
						  "\"appliedVacanciesRender\": \""		<< userAppliedVacanciesRender << "\","
						  "\"last_online\": \""					<< userLastOnline << "\","
						  "\"last_online_diff\": \""			<< to_string(GetTimeDifferenceFromNow(userLastOnline)) << "\","
						  "\"last_onlineSecondsSinceY2k\": \""  << userLastOnlineSecondSinceY2k << "\","
						  "\"userFriendship\": \""				<< userFriendship << "\","
						  "\"avatar\": \""						<< avatarPath << "\","
						  // "\"currentEmployment\": "				<< userCurrentEmployment << ", "
						  "\"currentCity\": \""					<< userCurrentCity << "\", "
						  "\"numberUnreadMessages\": \""		<< numberUreadMessages << "\", "
						  // "\"languages\": ["		 			<< GetLanguageListInJSONFormat("SELECT * FROM `language` WHERE `id` in (SELECT `language_id` FROM `users_language` WHERE `user_id`=\"" + userID + "\");", db) << "], "
						  // "\"skills\": ["		 				<< GetSkillListInJSONFormat("SELECT * FROM `skill` WHERE `id` in (SELECT `skill_id` FROM `users_skill` WHERE `user_id`=\"" + userID + "\");", db) << "], "
						  // "\"subscriptions\":[" 				<< (user && (user->GetID() == userID) ? GetUserSubscriptionsInJSONFormat("SELECT * FROM `users_subscriptions` WHERE `user_id`=\"" + userID + "\";", db) : "") << "],"
						  "\"isMe\": \""						<< ((user && (userID == user->GetID())) ? "yes" : "no") << "\" "
						"}";
			} // --- if user is not dupicated
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;

		ost.str("");
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are users returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		ostringstream	ostTemp;

		ostTemp.str("");
		ostTemp << "GetUserListInJSONFormat: end. returning string length " << ost.str().length();
		log.Write(DEBUG, ostTemp.str());
	}

	return ost.str();
}

// --- Returns company list in JSON format grabbed from DB
// --- Input:   dbQuery 		- SQL format returns users
//			  db	  			- DB connection
//			  user				- current user object, used to define company admin
//			  quickSearch		- owners, founders, openVacancies are not included into result
//			  include_employees	- used in admin _ONLY_ to get company list + # of users in each company
string GetCompanyListInJSONFormat(string dbQuery, CMysql *db, CUser *user, bool quickSearch/* = true*/, bool include_employees/*= false*/)
{
	struct CompanyClass {
		string	id;
		string	type;
		string	name;
		string	legal_geo_zip_id;
		string	legal_address;
		string	mailing_geo_zip_id;
		string	mailing_address;
		string	tin;
		string	bank_id;
		string	account;
		string	kpp;
		string	ogrn;
		string	link;
		string	foundationDate;
		string	numberOfEmployee;
		string	admin_userID;
		string	webSite;
		string	description;
		string	logo_folder;
		string	logo_filename;
		string	employedUsersList;
	};

	ostringstream				ost, ostFinal;
	string						sessid, lookForKey;
	int							affected;
	vector<CompanyClass>		companiesList;
	int							companyCounter = 0;

	MESSAGE_DEBUG("", "", "start");

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		companyCounter = affected;
		companiesList.reserve(companyCounter);  // --- reserving allows avoid moving vector in memory
												// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			CompanyClass	company;

			company.id = db->Get(i, "id");
			company.type = db->Get(i, "type");
			company.name = db->Get(i, "name");
			company.legal_geo_zip_id = db->Get(i, "legal_geo_zip_id");
			company.legal_address = db->Get(i, "legal_address");
			company.mailing_geo_zip_id = db->Get(i, "mailing_geo_zip_id");
			company.mailing_address = db->Get(i, "mailing_address");
			company.tin = db->Get(i, "tin");
			company.bank_id = db->Get(i, "bank_id");
			company.account = db->Get(i, "account");
			company.kpp = db->Get(i, "kpp");
			company.ogrn = db->Get(i, "ogrn");
			company.link = db->Get(i, "link");
			company.admin_userID = db->Get(i, "admin_userID");
			company.foundationDate = db->Get(i, "foundationDate");
			company.numberOfEmployee = db->Get(i, "numberOfEmployee");
			company.webSite = db->Get(i, "webSite");
			company.description = db->Get(i, "description");
			company.logo_folder = db->Get(i, "logo_folder");
			company.logo_filename = db->Get(i, "logo_filename");
			company.employedUsersList = "";
			companiesList.push_back(company);
		}

		for(int i = 0; i < companyCounter; i++)
		{
				if(ostFinal.str().length()) ostFinal << ", ";

				ostFinal << "{";
				ostFinal << "\"id\": \""				<< companiesList[i].id << "\",";
				ostFinal << "\"type\": \""				<< companiesList[i].type << "\", ";
				ostFinal << "\"name\": \""				<< companiesList[i].name << "\", ";
				ostFinal << "\"legal_geo_zip_id\": \""	<< companiesList[i].legal_geo_zip_id << "\", ";
				ostFinal << "\"legal_geo_zip\": ["		<< GetZipInJSONFormat(companiesList[i].legal_geo_zip_id, db, user) << "], ";
				ostFinal << "\"legal_address\": \""		<< companiesList[i].legal_address << "\", ";
				ostFinal << "\"mailing_geo_zip_id\": \""<< companiesList[i].mailing_geo_zip_id << "\", ";
				ostFinal << "\"mailing_geo_zip\": ["	<< GetZipInJSONFormat(companiesList[i].mailing_geo_zip_id, db, user) << "], ";
				ostFinal << "\"mailing_address\": \""	<< companiesList[i].mailing_address << "\", ";
				ostFinal << "\"tin\": \""				<< companiesList[i].tin << "\", ";
				ostFinal << "\"bank_id\": \""			<< companiesList[i].bank_id << "\", ";
				ostFinal << "\"banks\": ["				<< GetBankInJSONFormat("SELECT * FROM `banks` WHERE `id`=\"" + companiesList[i].bank_id + "\";", db, user) << "], ";
				ostFinal << "\"account\": \""			<< companiesList[i].account << "\", ";
				ostFinal << "\"kpp\": \""				<< companiesList[i].kpp << "\", ";
				ostFinal << "\"ogrn\": \""				<< companiesList[i].ogrn << "\", ";
				ostFinal << "\"link\": \""				<< companiesList[i].link << "\", ";
				ostFinal << "\"foundationDate\": \""	<< companiesList[i].foundationDate << "\",";
				ostFinal << "\"numberOfEmployee\": \""	<< companiesList[i].numberOfEmployee << "\",";
				ostFinal << "\"webSite\": \""			<< companiesList[i].webSite << "\",";
				ostFinal << "\"description\": \""		<< companiesList[i].description << "\",";
				ostFinal << "\"logo_folder\": \""		<< companiesList[i].logo_folder << "\",";
				ostFinal << "\"logo_filename\": \""	   	<< companiesList[i].logo_filename << "\",";
				ostFinal << "\"owners\": ["			   	<< (include_employees ? GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + companiesList[i].admin_userID + "\";", db, user) : "") << "],";
				ostFinal << "\"employees\": ["			<< (include_employees ? GetAgencyEmployeesInJSONFormat(companiesList[i].id, db, user) : "") << "],";
				ostFinal << "\"isMine\": \""			<< (user ? companiesList[i].admin_userID == user->GetID() : false) << "\"";
				ostFinal << "}";
		} // --- for loop through company list
	} // --- if sql-query on company selection success
	else
	{
		MESSAGE_DEBUG("", "", "there are no companies returned by request [" + dbQuery + "]");
	}

	MESSAGE_DEBUG("", "", "finish (number of companies returned is " + to_string(companyCounter) + ")");

	return ostFinal.str();
}

// input: ....
//		  includeReaders will add readers counter
string GetBookListInJSONFormat(string dbQuery, CMysql *db, bool includeReaders/* = false*/)
{
	struct BookClass {
		string	id, title, authorID, authorName, isbn10, isbn13, photoCoverFolder, PhotoCoverFilename, readersUserID;
	};

	ostringstream				   ostResult;
	int							 booksCount;
	vector<BookClass>			   bookList;


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	booksCount = db->Query(dbQuery);
	if(booksCount)
	{
		for(int i = 0; i < booksCount; i++)
		{
			BookClass   bookListItem;
			bookListItem.id				 = db->Get(i, "id");
			bookListItem.title			  = db->Get(i, "title");
			bookListItem.authorID		   = db->Get(i, "authorID");
			bookListItem.isbn10			 = db->Get(i, "isbn10");
			bookListItem.isbn13			 = db->Get(i, "isbn13");
			bookListItem.photoCoverFolder   = db->Get(i, "coverPhotoFolder");
			bookListItem.PhotoCoverFilename = db->Get(i, "coverPhotoFilename");
			bookListItem.readersUserID	  = "";

			bookList.push_back(bookListItem);
		}

		for(int i = 0; i < booksCount; i++)
		{
				if(db->Query("select * from `book_author` where `id`=\"" + bookList.at(i).authorID + "\";"))
					bookList.at(i).authorName = db->Get(0, "name");

				if(includeReaders)
				{
					string temp = "";

					for(int j = 0; j < db->Query("SELECT `userID` from `users_books` WHERE `bookID`=\"" + bookList.at(i).id + "\";"); ++j)
					{
						if(temp.length()) temp += ",";
						temp += db->Get(j, "userID");
					}

					bookList.at(i).readersUserID = temp;
				}

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"bookID\": \""				 << bookList.at(i).id << "\", "
						  << "\"bookTitle\": \""			  << bookList.at(i).title << "\", "
						  << "\"bookAuthorID\": \""		   << bookList.at(i).authorID << "\","
						  << "\"bookAuthorName\": \""		 << bookList.at(i).authorName << "\","
						  << "\"bookISBN10\": \""			 << bookList.at(i).isbn10 << "\","
						  << "\"bookISBN13\": \""			 << bookList.at(i).isbn13 << "\","
						  << "\"bookPhotoCoverFolder\": \""   << bookList.at(i).photoCoverFolder << "\","
						  << "\"bookPhotoCoverFilename\": \"" << bookList.at(i).PhotoCoverFilename << "\","
						  << "\"bookReadersUserID\": ["	   << bookList.at(i).readersUserID << "]"
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no books returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

// input: ....
//		  includeReaders will add readers counter
string GetComplainListInJSONFormat(string dbQuery, CMysql *db, bool includeReaders/* = false*/)
{
	struct ComplainClass {
		string	  id;
		string	  userID;
		string	  entityID;
		string	  type;
		string	  subtype;
		string	  complainComment;
		string	  resolveComment;
		string	  state;
		string	  openEventTimestamp;
		string	  closeEventTimestamp;
	};

	ostringstream				   ostResult;
	int							 complainsCount;
	vector<ComplainClass>		   complainList;


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	complainsCount = db->Query(dbQuery);
	if(complainsCount)
	{
		for(int i = 0; i < complainsCount; i++)
		{
			ComplainClass   complainListItem;
			complainListItem.id				 = db->Get(i, "id");
			complainListItem.userID			 = db->Get(i, "userID");
			complainListItem.entityID		   = db->Get(i, "entityID");
			complainListItem.type			   = db->Get(i, "type");
			complainListItem.subtype			= db->Get(i, "subtype");
			complainListItem.complainComment	= db->Get(i, "complainComment");
			complainListItem.resolveComment	 = db->Get(i, "resolveComment");
			complainListItem.state			  = db->Get(i, "state");
			complainListItem.openEventTimestamp = db->Get(i, "openEventTimestamp");
			complainListItem.closeEventTimestamp= db->Get(i, "closeEventTimestamp");

			complainList.push_back(complainListItem);
		}

		for(int i = 0; i < complainsCount; i++)
		{
				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"id\": \""			  << complainList.at(i).id << "\", "
						  << "\"userID\": \""		  << complainList.at(i).userID << "\", "
						  << "\"entityID\": \""		<< complainList.at(i).entityID << "\","
						  << "\"type\": \""			<< complainList.at(i).type << "\","
						  << "\"subtype\": \""		 << complainList.at(i).subtype << "\","
						  << "\"complainComment\": \"" << complainList.at(i).complainComment << "\","
						  << "\"resolveComment\": \""  << complainList.at(i).resolveComment << "\","
						  << "\"state\": \""		   << complainList.at(i).state << "\","
						  << "\"openEventTimestamp\": \""  << complainList.at(i).openEventTimestamp << "\","
						  << "\"closeEventTimestamp\": \"" << complainList.at(i).closeEventTimestamp << "\""
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no complains returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

// input: ....
//		  includeDevoted will add student counter
string GetCertificationListInJSONFormat(string dbQuery, CMysql *db, bool includeDevoted/* = false*/)
{
	struct ItemClass {
		string	  id, title, photoFolder, photoFilename, devotedUserList;
		string	  vendorID, vendorName;
		string	  isComplained, complainedUserList;
	};

	ostringstream	   ostResult;
	int				 itemsCount;
	vector<ItemClass>   itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	itemsCount = db->Query(dbQuery);
	if(itemsCount)
	{
		for(int i = 0; i < itemsCount; i++)
		{
			ItemClass   item;

			item.id				 = db->Get(i, "id");
			item.title			  = db->Get(i, "title");
			item.vendorID		   = db->Get(i, "vendor_id");
			item.vendorName		 = "";
			item.photoFolder		= db->Get(i, "logo_folder");
			item.photoFilename	  = db->Get(i, "logo_filename");
			item.devotedUserList	= "";

			itemsList.push_back(item);
		}

		for(int i = 0; i < itemsCount; i++)
		{
				if(db->Query("SELECT `name` FROM `company` WHERE `id`=\"" + itemsList.at(i).vendorID + "\";"))
					itemsList.at(i).vendorName = db->Get(0, "name");

				if(includeDevoted)
				{
					string temp = "";

					for(int j = 0; j < db->Query("SELECT * from `users_certifications` WHERE `track_id`=\"" + itemsList.at(i).id + "\";"); ++j)
					{
						if(temp.length()) temp += ",";
						temp += db->Get(j, "user_id");
					}

					itemsList.at(i).devotedUserList = temp;
				}

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"certificationID\": \""				 << itemsList.at(i).id << "\", "
						  << "\"certificationTitle\": \""			  << itemsList.at(i).title << "\", "
						  << "\"certificationVendorID\": \""		   << itemsList.at(i).vendorID << "\", "
						  << "\"certificationVendorName\": \""		 << itemsList.at(i).vendorName << "\", "
						  << "\"certificationPhotoCoverFolder\": \""   << itemsList.at(i).photoFolder << "\","
						  << "\"certificationPhotoCoverFilename\": \"" << itemsList.at(i).photoFilename << "\","
						  << "\"certificationReadersUserID\": ["	   << itemsList.at(i).devotedUserList << "]"
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no certifications returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

// input: ....
//		  includeStudents will add student counter
string GetCourseListInJSONFormat(string dbQuery, CMysql *db, bool includeStudents/* = false*/)
{
	struct ItemClass {
		string	  id, title, photoFolder, photoFilename, studentUserList;
		string	  vendorID, vendorName;
		string	  isComplained, complainedUserList;
	};

	ostringstream	   ostResult;
	int				 itemsCount;
	vector<ItemClass>   itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	itemsCount = db->Query(dbQuery);
	if(itemsCount)
	{
		for(int i = 0; i < itemsCount; i++)
		{
			ItemClass   item;

			item.id				 = db->Get(i, "id");
			item.title			  = db->Get(i, "title");
			item.vendorID		   = db->Get(i, "vendor_id");
			item.vendorName		 = "";
			item.photoFolder		= db->Get(i, "logo_folder");
			item.photoFilename	  = db->Get(i, "logo_filename");
			item.studentUserList	= "";

			itemsList.push_back(item);
		}

		for(int i = 0; i < itemsCount; i++)
		{
				if(db->Query("SELECT `name` FROM `company` WHERE `id`=\"" + itemsList.at(i).vendorID + "\";"))
					itemsList.at(i).vendorName = db->Get(0, "name");

				if(includeStudents)
				{
					string temp = "";

					for(int j = 0; j < db->Query("SELECT * from `users_courses` WHERE `track_id`=\"" + itemsList.at(i).id + "\";"); ++j)
					{
						if(temp.length()) temp += ",";
						temp += db->Get(j, "user_id");
					}

					itemsList.at(i).studentUserList = temp;
				}

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"courseID\": \""				 << itemsList.at(i).id << "\", "
						  << "\"courseTitle\": \""			  << itemsList.at(i).title << "\", "
						  << "\"courseVendorID\": \""		   << itemsList.at(i).vendorID << "\", "
						  << "\"courseVendorName\": \""		 << itemsList.at(i).vendorName << "\", "
						  << "\"coursePhotoCoverFolder\": \""   << itemsList.at(i).photoFolder << "\","
						  << "\"coursePhotoCoverFilename\": \"" << itemsList.at(i).photoFilename << "\","
						  << "\"courseStudentsUserID\": ["	   << itemsList.at(i).studentUserList << "]"
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no courses returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

// input: ....
//		  includeStudents will add student counter
string GetLanguageListInJSONFormat(string dbQuery, CMysql *db, bool includeStudents/* = false*/)
{
	struct ItemClass {
		string	  id, title, photoFolder, photoFilename, studentUserList;
		string	  isComplained, complainedUserList;
	};

	ostringstream	   ostResult;
	int				 itemsCount;
	vector<ItemClass>   itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	itemsCount = db->Query(dbQuery);
	if(itemsCount)
	{
		for(int i = 0; i < itemsCount; i++)
		{
			ItemClass   item;

			item.id				 = db->Get(i, "id");
			item.title			  = db->Get(i, "title");
			item.photoFolder		= db->Get(i, "logo_folder");
			item.photoFilename	  = db->Get(i, "logo_filename");
			item.studentUserList	= "";

			itemsList.push_back(item);
		}

		for(int i = 0; i < itemsCount; i++)
		{

				if(includeStudents)
				{
					string temp = "";

					for(int j = 0; j < db->Query("SELECT * from `users_language` WHERE `language_id`=\"" + itemsList.at(i).id + "\";"); ++j)
					{
						if(temp.length()) temp += ",";
						temp += db->Get(j, "user_id");
					}

					itemsList.at(i).studentUserList = temp;
				}

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"languageID\": \""				 << itemsList.at(i).id << "\", "
						  << "\"languageTitle\": \""			  << itemsList.at(i).title << "\", "
						  << "\"languagePhotoCoverFolder\": \""   << itemsList.at(i).photoFolder << "\","
						  << "\"languagePhotoCoverFilename\": \"" << itemsList.at(i).photoFilename << "\","
						  << "\"languageStudentsUserID\": ["	   << itemsList.at(i).studentUserList << "]"
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no languages returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

string GetSkillListInJSONFormat(string dbQuery, CMysql *db)
{
	struct ItemClass {
		string	  id, title;
		string	  isComplained, complainedUserList;
	};

	ostringstream	   ostResult;
	int				 itemsCount;
	vector<ItemClass>   itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	itemsCount = db->Query(dbQuery);
	if(itemsCount)
	{
		for(int i = 0; i < itemsCount; i++)
		{
			ItemClass   item;

			item.id				 = db->Get(i, "id");
			item.title			  = db->Get(i, "title");

			itemsList.push_back(item);
		}

		for(int i = 0; i < itemsCount; i++)
		{

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"id\": \""		<< itemsList.at(i).id << "\", "
						  << "\"title\": \""	<< itemsList.at(i).title << "\" "
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no skills returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}


// input: ....
//		  includeStudents will add student counter
string GetUniversityListInJSONFormat(string dbQuery, CMysql *db, bool includeStudents/* = false*/)
{
	struct ItemClass {
		string	  id, title, photoFolder, photoFilename, studentUserList;
		string	  regionID, regionTitle;
		string	  isComplained, complainedUserList;
	};

	ostringstream	   ostResult;
	int				 itemsCount;
	vector<ItemClass>   itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	itemsCount = db->Query(dbQuery);
	if(itemsCount)
	{
		for(int i = 0; i < itemsCount; i++)
		{
			ItemClass   item;

			item.id				 = db->Get(i, "id");
			item.title			  = db->Get(i, "title");
			item.regionID		   = db->Get(i, "geo_region_id");
			item.regionTitle		= "";
			item.photoFolder		= db->Get(i, "logo_folder");
			item.photoFilename	  = db->Get(i, "logo_filename");
			item.studentUserList	= "";

			itemsList.push_back(item);
		}

		for(int i = 0; i < itemsCount; i++)
		{
				if(db->Query("SELECT `title` FROM `geo_region` WHERE `id`=\"" + itemsList.at(i).regionID + "\";"))
					itemsList.at(i).regionTitle = db->Get(0, "title");

				if(includeStudents)
				{
					string temp = "";

					for(int j = 0; j < db->Query("SELECT * from `users_university` WHERE `university_id`=\"" + itemsList.at(i).id + "\";"); ++j)
					{
						if(temp.length()) temp += ",";
						temp += db->Get(j, "user_id");
					}

					itemsList.at(i).studentUserList = temp;
				}

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"universityID\": \""				 << itemsList.at(i).id << "\", "
						  << "\"universityTitle\": \""			  << itemsList.at(i).title << "\", "
						  << "\"universityRegionID\": \""		   << itemsList.at(i).regionID << "\", "
						  << "\"universityRegionName\": \""		 << itemsList.at(i).regionTitle << "\", "
						  << "\"universityPhotoCoverFolder\": \""   << itemsList.at(i).photoFolder << "\","
						  << "\"universityPhotoCoverFilename\": \"" << itemsList.at(i).photoFilename << "\","
						  << "\"universityStudentsUserID\": ["	   << itemsList.at(i).studentUserList << "]"
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no university's returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

// input: ....
//		  includeStudents will add student counter
string GetSchoolListInJSONFormat(string dbQuery, CMysql *db, bool includeStudents/* = false*/)
{
	struct ItemClass {
		string	  id, title, photoFolder, photoFilename, studentUserList;
		string	  regionID, regionTitle;
		string	  isComplained, complainedUserList;
	};

	ostringstream	   ostResult;
	int				 itemsCount;
	vector<ItemClass>   itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");
	itemsCount = db->Query(dbQuery);
	if(itemsCount)
	{
		for(int i = 0; i < itemsCount; i++)
		{
			ItemClass   item;

			item.id				 = db->Get(i, "id");
			item.title			  = db->Get(i, "title");
			item.regionID		   = db->Get(i, "geo_locality_id");
			item.regionTitle		= "";
			item.photoFolder		= db->Get(i, "logo_folder");
			item.photoFilename	  = db->Get(i, "logo_filename");
			item.studentUserList	= "";

			itemsList.push_back(item);
		}

		for(int i = 0; i < itemsCount; i++)
		{
				if(db->Query("SELECT `title` FROM `geo_locality` WHERE `id`=\"" + itemsList.at(i).regionID + "\";"))
					itemsList.at(i).regionTitle = db->Get(0, "title");

				if(includeStudents)
				{
					string temp = "";

					for(int j = 0; j < db->Query("SELECT * from `users_school` WHERE `school_id`=\"" + itemsList.at(i).id + "\";"); ++j)
					{
						if(temp.length()) temp += ",";
						temp += db->Get(j, "user_id");
					}

					itemsList.at(i).studentUserList = temp;
				}

				if(ostResult.str().length()) ostResult << ", ";

				ostResult << "{"
						  << "\"schoolID\": \""				 << itemsList.at(i).id << "\", "
						  << "\"schoolTitle\": \""			  << itemsList.at(i).title << "\", "
						  << "\"schoolLocalityID\": \""		 << itemsList.at(i).regionID << "\", "
						  << "\"schoolLocalityTitle\": \""	   << itemsList.at(i).regionTitle << "\", "
						  << "\"schoolPhotoCoverFolder\": \""   << itemsList.at(i).photoFolder << "\","
						  << "\"schoolPhotoCoverFilename\": \"" << itemsList.at(i).photoFilename << "\","
						  << "\"schoolStudentsUserID\": ["	  << itemsList.at(i).studentUserList << "]"
						  << "}";
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no school's returned by the request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (returning string length = " + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

string	AutodetectSexByName(string name, CMysql *db)
{
	string		result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if(db->Query("SELECT * FROM `name_sex` WHERE `name`=\"" + name + "\";"))
		result = db->Get(0, "sex");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result = " + result + ")");
	}

	return result;
}

string GetNewsFeedInJSONFormat(string whereStatement, int currPage, int newsOnSinglePage, CUser *user, CMysql *db)
{
		struct  ItemClass
		{
			string  action_category_title;
			string  action_category_title_female;
			string  action_category_title_male;
			string  action_types_title;
			string  action_types_title_female;
			string  action_types_title_male;
			string  feed_actionId;
			string  feed_actionTypeId;
			string  feed_eventTimestamp;
			string  feed_id;
			string  src_id;
			string  src_type;
			string  src_name;
			string  src_nameLast;
			string  src_sex;
			string  dst_id;
			string  dst_type;
			string  dst_name;
			string  dst_nameLast;
			string  dst_sex;
		};
		vector<ItemClass>   itemsList;

		ostringstream	  ostResult;
		int			  affected;
		vector<string>	vectorFriendList;

		ostResult.str("");
		ostResult << "SELECT `users_friends`.`friendID` \
				from `users_friends` \
				left join `users` on `users`.`id`=`users_friends`.`friendID` \
				where `users_friends`.`userID`='" << user->GetID() << "' and `users_friends`.`state`='confirmed' and `users`.`isactivated`='Y' and `users`.`isblocked`='N';";

		affected = db->Query(ostResult.str());
		for(int i = 0; i < affected; i++)
		{
			vectorFriendList.push_back(db->Get(i, "friendID"));
		}

		ostResult.str("");
		ostResult << "SELECT "
			<< "`feed`.`id` as `feed_id`, `feed`.`eventTimestamp` as `feed_eventTimestamp`, `feed`.`actionId` as `feed_actionId` , `feed`.`actionTypeId` as `feed_actionTypeId`, `feed`.`srcType` as `feed_srcType`, `feed`.`userId` as `feed_srcID`, `feed`.`dstType` as `feed_dstType`, `feed`.`dstID` as `feed_dstID`, "
			<< "`action_types`.`title` as `action_types_title`, "
			<< "`action_types`.`title_male` as `action_types_title_male`, "
			<< "`action_types`.`title_female` as `action_types_title_female`, "
			<< "`action_category`.`title` as `action_category_title`, "
			<< "`action_category`.`title_male` as `action_category_title_male`, "
			<< "`action_category`.`title_female` as `action_category_title_female` "
			<< "FROM `feed` "
			<< "INNER JOIN  `action_types`  ON `feed`.`actionTypeId`=`action_types`.`id` "
			<< "INNER JOIN  `action_category`   ON `action_types`.`categoryID`=`action_category`.`id` "
			<< "WHERE (" << whereStatement << ") and `action_types`.`isShowFeed`='1' "
			<< "ORDER BY  `feed`.`eventTimestamp` DESC LIMIT " << currPage * newsOnSinglePage << " , " << newsOnSinglePage;

		affected = db->Query(ostResult.str());
		for(int i = 0; i < affected; i++)
		{
			ItemClass   item;

			item.action_category_title = db->Get(i, "action_category_title");
			item.action_category_title_female = db->Get(i, "action_category_title_female");
			item.action_category_title_male = db->Get(i, "action_category_title_male");
			item.action_types_title = db->Get(i, "action_types_title");
			item.action_types_title_female = db->Get(i, "action_types_title_female");
			item.action_types_title_male = db->Get(i, "action_types_title_male");
			item.feed_actionId = db->Get(i, "feed_actionId");
			item.feed_actionTypeId = db->Get(i, "feed_actionTypeId");
			item.feed_eventTimestamp = db->Get(i, "feed_eventTimestamp");
			item.feed_id = db->Get(i, "feed_id");
			item.src_id = db->Get(i, "feed_srcID");
			item.src_type = db->Get(i, "feed_srcType");
			item.dst_id = db->Get(i, "feed_dstID");
			item.dst_type = db->Get(i, "feed_dstType");

			itemsList.push_back(item);
		}

		ostResult.str("");
		for(int i = 0; i < affected; i++)
		{
			ostringstream   ost1;
			string		  srcAvatarPath;
			string		  dstAvatarPath;
			string		  feedID = itemsList[i].feed_id;
			string		  feedActionTypeId = itemsList[i].feed_actionTypeId;
			string		  feedActionId = itemsList[i].feed_actionId;
			string		  feedMessageOwner = itemsList[i].src_id;
			string		  feedMessageTimestamp = itemsList[i].feed_eventTimestamp;
			string		  messageSrcObject = "";
			string		  messageDstObject = "";

			// --- avatar for srcObj
			if(itemsList[i].src_type == "user")
			{
				if(db->Query("SELECT `id`, `name`, `nameLast`, `sex` FROM `users` WHERE `id`=\"" + itemsList[i].src_id + "\";"))
				{
					messageSrcObject = "\"type\":\"" + itemsList[i].src_type + "\",\"id\":\"" + itemsList[i].src_id + "\",\"name\":\"" + db->Get(0, "name") + "\",\"nameLast\":\"" + db->Get(0, "nameLast") + "\",\"sex\":\"" + db->Get(0, "sex") + "\",\"link\":\"\"";

					if(db->Query("SELECT * FROM `users_avatars` WHERE `userid`='" + itemsList[i].src_id + "' and `isActive`='1';"))
						srcAvatarPath = "/images/avatars/avatars" + string(db->Get(0, "folder")) + "/" + string(db->Get(0, "filename"));
					else
						srcAvatarPath = "empty";

					messageSrcObject = messageSrcObject + ",\"avatar\":\"" + srcAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: user.id [" + itemsList[i].src_id + "] not found");
				}
			}
			else if(itemsList[i].src_type == "company")
			{
				if(db->Query("SELECT `id`, `type`, `name`, `link`, `logo_folder`, `logo_filename` FROM `company` WHERE `id`=\"" + itemsList[i].src_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageSrcObject = "\"type\":\"" + itemsList[i].src_type + "\",\"id\":\"" + itemsList[i].src_id + "\",\"companyType\":\"" + string(db->Get(0, "type")) + "\",\"name\":\"" + string(db->Get(0, "name")) + "\",\"nameLast\":\"\",\"sex\":\"F\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						srcAvatarPath = "/images/companies/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						srcAvatarPath = "empty";

					messageSrcObject = messageSrcObject + ",\"avatar\":\"" + srcAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].src_id + "] not found");
				}
			}
			else if(itemsList[i].src_type == "group")
			{
				if(db->Query("SELECT * FROM `groups` WHERE `id`=\"" + itemsList[i].src_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageSrcObject = "\"type\":\"" + itemsList[i].src_type + "\",\"id\":\"" + itemsList[i].src_id + "\",\"name\":\"" + db->Get(0, "title") + "\",\"nameLast\":\"\",\"sex\":\"F\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						srcAvatarPath = "/images/groups/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						srcAvatarPath = "empty";

					messageSrcObject = messageSrcObject + ",\"avatar\":\"" + srcAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].src_id + "] not found");
				}
			}
			else
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: unknown srcType [" + itemsList[i].src_type + "]");
			}

			// --- avatar for dstObj
			if(itemsList[i].dst_type == "group")
			{
				if(db->Query("SELECT * FROM `groups` WHERE `id`=\"" + itemsList[i].dst_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageDstObject = "\"type\":\"" + itemsList[i].dst_type + "\",\"id\":\"" + itemsList[i].dst_id + "\",\"name\":\"" + db->Get(0, "title") + "\",\"nameLast\":\"\",\"sex\":\"F\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						dstAvatarPath = "/images/groups/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						dstAvatarPath = "empty";

					messageDstObject = messageDstObject + ",\"avatar\":\"" + dstAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].dst_id + "] not found");
				}
			}
			// --- avatar for dstObj
			if(itemsList[i].dst_type == "event")
			{
				if(db->Query("SELECT * FROM `events` WHERE `id`=\"" + itemsList[i].dst_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageDstObject = "\"type\":\"" + itemsList[i].dst_type + "\",\"id\":\"" + itemsList[i].dst_id + "\",\"name\":\"" + db->Get(0, "title") + "\",\"nameLast\":\"\",\"sex\":\"\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						dstAvatarPath = "/images/events/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						dstAvatarPath = "empty";

					messageDstObject = messageDstObject + ",\"avatar\":\"" + dstAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].dst_id + "] not found");
				}
			}

			if(feedActionTypeId == "11")
			{
				// --- 11 - message

				if(db->Query("SELECT * FROM `feed_message` WHERE `id`='" + feedActionId + "';"))
				{
					string  messageId = db->Get(0, "id");
					string  messageTitle = db->Get(0, "title");
					string  messageLink = db->Get(0, "link");
					string  messageMessage = db->Get(0, "message");
					string  messageImageSetID = db->Get(0, "imageSetID");
					string  messageAccessRights = db->Get(0, "access");
					string  messageImageList =			GetMessageImageList(messageImageSetID, db);
					string  messageParamLikesUserList =  GetMessageLikesUsersList(messageId, user, db);
					string  messageParamCommentsCount =  GetMessageCommentsCount(messageId, db);
					string  messageParamSpam =			GetMessageSpam(messageId, db);
					string  messageParamSpamMe =			GetMessageSpamUser(messageId, user->GetID(), db);

					if(AllowMessageInNewsFeed(user, feedMessageOwner, messageAccessRights, &vectorFriendList))
					{
						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"dstObj\":{"			 << messageDstObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female		<< "\",";
						ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId				   << "\",";

						ostResult << "\"messageId\":\""		<< messageId							 << "\",";
						ostResult << "\"messageTitle\":\""	  << messageTitle						  << "\",";
						ostResult << "\"messageLink\":\""		<< messageLink						   << "\",";
						ostResult << "\"messageMessage\":\""	  << messageMessage						<< "\",";
						ostResult << "\"messageImageSetID\":\""   << messageImageSetID					  << "\",";
						ostResult << "\"messageImageList\":["	<< messageImageList						 << "],";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList			  << "],";
						ostResult << "\"messageCommentsCount\":\""<< messageParamCommentsCount			  << "\",";
						ostResult << "\"messageSpam\":\""		<< messageParamSpam						 << "\",";
						ostResult << "\"messageSpamMe\":\""	<< messageParamSpamMe					   << "\",";
						ostResult << "\"messageAccessRights\":\"" << messageAccessRights					  << "\",";

						ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp										<< "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";

						ostResult << "}";

						// if(i < (affected - 1)) ostResult << ",";
					} // --- Message Access Rights onot allow to post it into feed
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: can't get message [" + feedActionId + "] FROM feed_message");
				} // --- Message in news feed not found
			}
			else if((feedActionTypeId == "14") || (feedActionTypeId == "15") || (feedActionTypeId == "16"))
			{
				// --- 14 friendship established
				// --- 15 friendship broken
				// --- 16 friendship request sent

				string  friendID = feedActionId;

				ost1.str("");
				ost1 << "SELECT `users`.`name` as `users_name`, `users`.`nameLast` as `users_nameLast` FROM `users` WHERE `id`=\"" << friendID << "\" and `isblocked`='N';";
				if(db->Query(ost1.str()))
				{
					string  friendAvatar = "empty";
					string  friendName;
					string  friendNameLast;
					string  friendCompanyName;
					string  friendCompanyID;
					string  friendUsersCompanyPositionTitle;

					friendName = db->Get(0, "users_name");
					friendNameLast = db->Get(0, "users_nameLast");

					ost1.str("");
					ost1 << "SELECT * FROM `users_avatars` WHERE `userid`='" << friendID << "' and `isActive`='1';";
					if(db->Query(ost1.str()))
					{
						ost1.str("");
						ost1 << "/images/avatars/avatars" << db->Get(0, "folder") << "/" << db->Get(0, "filename");
						friendAvatar = ost1.str();
					}

					ost1.str("");
					ost1 << "SELECT `company_position`.`title` as `users_company_position_title`,  \
							`company`.`id` as `company_id`, `company`.`name` as `company_name` \
							FROM `users_company` \
							LEFT JOIN  `company_position` ON `company_position`.`id`=`users_company`.`position_title_id` \
							LEFT JOIN  `company`				ON `company`.`id`=`users_company`.`company_id` \
							WHERE `users_company`.`user_id`=\"" << friendID << "\" and `users_company`.`current_company`='1' \
							ORDER BY  `users_company`.`occupation_start` DESC ";
					if(db->Query(ost1.str()))
					{
						friendCompanyName = db->Get(0, "company_name");
						friendCompanyID = db->Get(0, "company_id");
						friendUsersCompanyPositionTitle = db->Get(0, "users_company_position_title");
					}
					else
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + string("]:") + ": can't get information [", itemsList[i].feed_actionId, "] about his/her employment");
					} // --- Message in news feed not found

					{
						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";
						ostResult << "\"avatar\":\""			  << srcAvatarPath									<< "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId				   << "\",";

						ostResult << "\"friendAvatar\":\""	  << friendAvatar								  << "\",";
						ostResult << "\"friendID\":\""		  << friendID									  << "\",";
						ostResult << "\"friendName\":\""		  << friendName									<< "\",";
						ostResult << "\"friendNameLast\":\""	  << friendNameLast								<< "\",";
						ostResult << "\"friendCompanyID\":\""	<< friendCompanyID							   << "\",";
						ostResult << "\"friendCompanyName\":\""   << friendCompanyName							  << "\",";
						ostResult << "\"friendUsersCompanyPositionTitle\":\"" << friendUsersCompanyPositionTitle	  << "\",";

						ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp										<< "\",";
						// ostResult << "\"eventTimestampDelta\":\""  << GetHumanReadableTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)   << "\"";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";
						ostResult << "}";

						// if(i < (affected - 1)) ostResult << ",";
					} // --- Message Access Rights onot allow to post it into feed
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: user [", friendID, "] not found or blocked");
				}
			}
			else if((feedActionTypeId == "41"))
			{
				// --- 41 skill added

				string  users_skillID = feedActionId;

				ost1.str("");
				ost1 << "SELECT * FROM `users_skill` WHERE `id`=\"" << users_skillID << "\";";
				if(db->Query(ost1.str()))
				{
					string  skillID = db->Get(0, "skill_id");

					ost1.str("");
					ost1 << "SELECT * FROM `skill` WHERE `id`=\"" << skillID << "\";";
					if(db->Query(ost1.str()))
					{
						string	skillTitle = db->Get(0, "title");

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";
						ostResult << "\"avatar\":\""			  << srcAvatarPath									<< "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId				   << "\",";

						ostResult << "\"skillID\":\""			<< skillID									   << "\",";
						ostResult << "\"skillTitle\":\""		  << skillTitle									<< "\",";

						ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp							<< "\",";
						// ostResult << "\"eventTimestampDelta\":\""  << GetHumanReadableTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)   << "\"";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";
						ostResult << "}";

					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: skill_id [", skillID, "] not found");
					}

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: users_skill_id [", users_skillID, "] not found");
				}
			}
			else if((feedActionTypeId == "54") || (feedActionTypeId == "53"))
			{
				// --- book read
				string  usersBooksID = itemsList[i].feed_actionId;
				string  bookID = "";
				string  readerUserID = "";
				string  bookReadTimestamp = "";

				if(db->Query("SELECT * FROM `users_books` WHERE `id`=\"" + feedActionId + "\";"))
				{
					bookID = db->Get(0, "bookID");
					readerUserID = db->Get(0, "userID");
					bookReadTimestamp = db->Get(0, "bookReadTimestamp");


					if(bookID.length() && db->Query("SELECT * FROM `book` WHERE `id`=\"" + bookID + "\";"))
					{
						string  bookTitle = db->Get(0, "title");
						string  bookAuthorID = db->Get(0, "authorID");
						string  bookISBN10 = db->Get(0, "isbn10");
						string  bookISBN13 = db->Get(0, "isbn13");
						string  bookCoverPhotoFolder = db->Get(0, "coverPhotoFolder");
						string  bookCoverPhotoFilename = db->Get(0, "coverPhotoFilename");

						if(bookAuthorID.length() && db->Query("SELECT * FROM `book_author` WHERE `id`=\"" + bookAuthorID + "\";"))
						{
							string  bookAuthorName = db->Get(0, "name");
							string  bookReadersRatingList =	 GetBookRatingList(bookID, db);
							string  messageParamLikesUserList = GetBookLikesUsersList(usersBooksID, user, db);
							string  messageParamCommentsCount = GetBookCommentsCount(bookID, db);
							string  bookMyRating = "0";

							if(db->Query("SELECT * FROM `users_books` WHERE `userID`=\"" + user->GetID() + "\" AND `bookID`=\"" + bookID + "\";"))
								bookMyRating = db->Get(0, "rating");

							if(ostResult.str().length() > 10) ostResult << ",";

							ostResult << "{";

							ostResult << "\"bookID\":\""			<< bookID << "\",";
							ostResult << "\"bookTitle\":\""	   << bookTitle << "\",";
							ostResult << "\"bookAuthorID\":\""	  << bookAuthorID << "\",";
							ostResult << "\"bookAuthorName\":\""	<< bookAuthorName << "\",";
							ostResult << "\"bookISBN10\":\""		<< bookISBN10 << "\",";
							ostResult << "\"bookISBN13\":\""		<< bookISBN13 << "\",";
							ostResult << "\"bookMyRating\":\""		<< bookMyRating << "\",";
							ostResult << "\"bookCoverPhotoFolder\":\""<< bookCoverPhotoFolder << "\",";
							ostResult << "\"bookCoverPhotoFilename\":\""<< bookCoverPhotoFilename << "\",";
							ostResult << "\"bookReadTimestamp\":\""   << bookReadTimestamp << "\",";
							ostResult << "\"bookReadersRatingList\":["<< bookReadersRatingList << "],";
							ostResult << "\"bookCommentsCount\":\""   << messageParamCommentsCount << "\",";

							ostResult << "\"usersBooksID\":\""	  << usersBooksID << "\",";
							ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

							ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
							ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
							ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
							ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
							ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
							ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
							ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
							ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
							ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
							ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
							ostResult << "\"actionID\":\""		  << itemsList[i].feed_actionId << "\",";
							ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
							ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
							ostResult << "}";
						}
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: books_author_id [" + bookAuthorID + "] not found");
						}

					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: books_id [" + bookID + "] not found");
					}

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_books_id [" + feedActionId + "] not found");
				}
			}
			else if((feedActionTypeId == "64") || (feedActionTypeId == "65"))
			{
				// --- group created / subscribed
				string  groupID = feedActionId;

				if(groupID.length())
				{
					if(db->Query("SELECT * FROM `groups` WHERE `id`=\"" + groupID + "\""))
					{
						string		isBlocked = db->Get(0, "isBlocked");

						if(isBlocked == "N")
						{
							if(ostResult.str().length() > 10) ostResult << ",";

							ostResult << "{";
							ostResult << "\"groups\":["			<< GetGroupListInJSONFormat("SELECT * FROM `groups` WHERE `id`=\"" + groupID + "\" AND `isBlocked`=\"N\";", db, user) << "],";
							ostResult << "\"avatar\":\""		<< srcAvatarPath << "\",";
							ostResult << "\"srcObj\":{"			<< messageSrcObject << "},";
							ostResult << "\"messageOwner\":{"	<< messageSrcObject << "},";
							ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
							ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
							ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
							ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
							ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
							ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
							ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
							ostResult << "\"actionID\":\""		  << itemsList[i].feed_actionId << "\",";
							ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
							ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
							ostResult << "}";
						}
						else
						{
							CLog	log;
							log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: `group`.`id` [" + groupID + "] blocked");
						}
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: `group`.`id` [" + groupID + "] not found");
					}
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: `group`.`id` [" + groupID + "] is empty");
				}
			}
			else if(feedActionTypeId == "63")
			{
				// --- group created / subscribed
				string  companyID = feedActionId;

				if(companyID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + companyID + "\" AND `isBlocked`=\"N\";"))
				{
					if(ostResult.str().length() > 10) ostResult << ",";

					ostResult << "{";
					ostResult << "\"companies\":["		<< GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + companyID + "\" AND `isBlocked`=\"N\";", db, user) << "],";
					ostResult << "\"avatar\":\""		<< srcAvatarPath << "\",";
					ostResult << "\"srcObj\":{"			<< messageSrcObject << "},";
					ostResult << "\"messageOwner\":{"	<< messageSrcObject << "},";
					ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
					ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
					ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
					ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
					ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
					ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
					ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
					ostResult << "\"actionID\":\""		  << itemsList[i].feed_actionId << "\",";
					ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
					ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
					ostResult << "}";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: books_id [" + companyID + "] not found");
				}
			}
			else if(feedActionTypeId == "22")
			{
				// --- cretificate received

				if(db->Query("SELECT * FROM `users_certifications` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  certificationNumber = "";
					string  certificationTrackID = "";
					string  certifiedUserID = "";
					string  usersCertificationID = "";

					usersCertificationID = db->Get(0, "id");
					certificationNumber = db->Get(0, "certification_number");
					certificationTrackID = db->Get(0, "track_id");
					certifiedUserID = db->Get(0, "user_id");

					if(certificationTrackID.length() && db->Query("SELECT * FROM `certification_tracks` WHERE `id`=\"" + certificationTrackID + "\";"))
					{
						string  certificationVendorID = db->Get(0, "vendor_id");
						string  certificationVendorName = "";
						string  certificationTrackTitle = db->Get(0, "title");
						string  certificationVendorLogoFolder = db->Get(0, "logo_folder");
						string  certificationVendorLogoFilename = db->Get(0, "logo_filename");
						string  messageParamLikesUserList = GetCertificationLikesUsersList(usersCertificationID, user, db);
						string  messageParamCommentsCount = GetCertificateCommentsCount(certificationTrackID, db);

						if(certificationVendorID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + certificationVendorID + "\";"))
							certificationVendorName = db->Get(0, "name");
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: certification vendor [" + certificationVendorID + "] not found");
						}

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"certificationID\":\"" << usersCertificationID << "\",";
						ostResult << "\"certificationTrackID\":\"" << certificationTrackID << "\",";
						ostResult << "\"certificationNumber\":\"" << certificationNumber << "\",";
						ostResult << "\"certificationVendorID\":\"" << certificationVendorID << "\",";
						ostResult << "\"certificationVendorName\":\"" << certificationVendorName << "\",";
						ostResult << "\"certificationTrackTitle\":\"" << certificationTrackTitle << "\",";
						ostResult << "\"certificationVendorLogoFolder\":\""<< certificationVendorLogoFolder << "\",";
						ostResult << "\"certificationVendorLogoFilename\":\""<< certificationVendorLogoFilename << "\",";

						ostResult << "\"certificationCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: certification track [" + certificationTrackID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_certification_id [" + feedActionId + "] not found");
				}

			}
			else if(feedActionTypeId == "23")
			{
				// --- course received

				if(db->Query("SELECT * FROM `users_courses` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  courseTrackID = "";
					string  courseUserID = "";
					string  courseMyRating = "0";
					string  usersCourseID = "";
					string  eventTimestamp = "";

					usersCourseID = db->Get(0, "id");
					courseTrackID = db->Get(0, "track_id");
					courseUserID = db->Get(0, "user_id");
					eventTimestamp = db->Get(0, "eventTimestamp");

					if(courseTrackID.length() && db->Query("SELECT * FROM `certification_tracks` WHERE `id`=\"" + courseTrackID + "\";"))
					{
						string  courseVendorID = db->Get(0, "vendor_id");
						string  courseVendorName = "";
						string  courseTrackTitle = db->Get(0, "title");
						string  courseVendorLogoFolder = db->Get(0, "logo_folder");
						string  courseVendorLogoFilename = db->Get(0, "logo_filename");
						string  courseRatingList =			GetCourseRatingList(courseTrackID, db);
						string  messageParamLikesUserList = GetCourseLikesUsersList(usersCourseID, user, db);
						string  messageParamCommentsCount = GetCertificateCommentsCount(courseTrackID, db);

						if(courseVendorID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + courseVendorID + "\";"))
							courseVendorName = db->Get(0, "name");
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: course vendor [" + courseVendorID + "] not found");
						}

						if(db->Query("SELECT * FROM `users_courses` WHERE `user_id`=\"" + user->GetID() + "\" AND `track_id`=\"" + courseTrackID + "\";"))
							courseMyRating = db->Get(0, "rating");

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"courseID\":\""		  << courseTrackID << "\",";
						ostResult << "\"usersCourseID\":\""   << usersCourseID << "\",";
						ostResult << "\"courseTrackID\":\""   << courseTrackID << "\",";
						ostResult << "\"courseVendorID\":\""	<< courseVendorID << "\",";
						ostResult << "\"courseVendorName\":\""  << courseVendorName << "\",";
						ostResult << "\"courseTrackTitle\":\""  << courseTrackTitle << "\",";
						ostResult << "\"courseVendorLogoFolder\":\""<< courseVendorLogoFolder << "\",";
						ostResult << "\"courseVendorLogoFilename\":\""<< courseVendorLogoFilename << "\",";
						ostResult << "\"courseMyRating\":\""	<< courseMyRating << "\",";
						ostResult << "\"courseRatingList\":["  << courseRatingList << "],";
						ostResult << "\"courseEventTimestamp\":\""<< eventTimestamp << "\",";

						ostResult << "\"courseCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: course track [" + courseTrackID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_course_id [" + feedActionId + "] not found");
				}

			}
			else if(feedActionTypeId == "39")
			{
				// --- university degree received

				if(db->Query("SELECT * FROM `users_university` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  scienceDegreeTitle = "";
					string  scienceDegreeUniversityID = "";
					string  scienceDegreeStart = "";
					string  scienceDegreeFinish = "";
					string  degreedUserID = "";
					string  usersUniversityDegreeID = "";

					usersUniversityDegreeID = db->Get(0, "id");
					degreedUserID = db->Get(0, "user_id");
					scienceDegreeUniversityID = db->Get(0, "university_id");
					scienceDegreeTitle = db->Get(0, "degree");
					scienceDegreeStart = db->Get(0, "occupation_start");
					scienceDegreeFinish = db->Get(0, "occupation_finish");

					if(scienceDegreeUniversityID.length() && db->Query("SELECT * FROM `university` WHERE `id`=\"" + scienceDegreeUniversityID + "\";"))
					{
						string  scienceDegreeUniversityID = db->Get(0, "id");
						string  scienceDegreeUniversityTitle = db->Get(0, "title");
						string  scienceDegreeUniversityRegionID = db->Get(0, "geo_region_id");
						string  scienceDegreeUniversityRegionTitle = "";
						string  scienceDegreeUniversityCountryID = "";
						string  scienceDegreeUniversityCountryTitle = "";
						string  scienceDegreeUniversityLogoFolder = db->Get(0, "logo_folder");
						string  scienceDegreeUniversityLogoFilename = db->Get(0, "logo_filename");

						if(scienceDegreeUniversityRegionID.length() && db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + scienceDegreeUniversityRegionID + "\";"))
						{
							scienceDegreeUniversityRegionTitle = db->Get(0, "title");
							scienceDegreeUniversityCountryID = db->Get(0, "geo_country_id");
							if(scienceDegreeUniversityCountryID.length() && db->Query("SELECT * FROM `geo_country` WHERE `id`=\"" + scienceDegreeUniversityCountryID + "\";"))
							{
								scienceDegreeUniversityCountryTitle = db->Get(0, "title");
							}
							else
							{
								CLog	log;
								log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: university [" + scienceDegreeUniversityID + "] geo_region [" + scienceDegreeUniversityRegionID + "] geo_country [" + scienceDegreeUniversityCountryID + "] not found");
							}

						}
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: university [" + scienceDegreeUniversityID + "] geo_region [" + scienceDegreeUniversityRegionID + "] not found");
						}

						{
							string  messageParamLikesUserList = GetUniversityDegreeLikesUsersList(usersUniversityDegreeID, user, db);
							string  messageParamCommentsCount = GetUniversityDegreeCommentsCount(scienceDegreeUniversityID, db);

							if(ostResult.str().length() > 10) ostResult << ",";

							ostResult << "{";

							ostResult << "\"scienceDegreeID\":\"" << usersUniversityDegreeID << "\",";
							ostResult << "\"scienceDegreeTitle\":\"" << scienceDegreeTitle << "\",";
							ostResult << "\"scienceDegreeUniversityTitle\":\"" << scienceDegreeUniversityTitle << "\",";
							ostResult << "\"scienceDegreeUniversityID\":\"" << scienceDegreeUniversityID << "\",";
							ostResult << "\"scienceDegreeUniversityRegionID\":\"" << scienceDegreeUniversityRegionID << "\",";
							ostResult << "\"scienceDegreeUniversityRegionTitle\":\"" << scienceDegreeUniversityRegionTitle << "\",";
							ostResult << "\"scienceDegreeUniversityCountryID\":\"" << scienceDegreeUniversityCountryID << "\",";
							ostResult << "\"scienceDegreeUniversityCountryTitle\":\"" << scienceDegreeUniversityCountryTitle << "\",";
							ostResult << "\"scienceDegreeUniversityLogoFolder\":\""<< scienceDegreeUniversityLogoFolder << "\",";
							ostResult << "\"scienceDegreeUniversityLogoFilename\":\""<< scienceDegreeUniversityLogoFilename << "\",";
							ostResult << "\"scienceDegreeStart\":\""<< scienceDegreeStart << "\",";
							ostResult << "\"scienceDegreeFinish\":\""<< scienceDegreeFinish << "\",";

							ostResult << "\"scienceDegreeCommentsCount\":\"" << messageParamCommentsCount << "\",";
							ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

							ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
							ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
							ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
							ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
							ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
							ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
							ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
							ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
							ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
							ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
							ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
							ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
							ostResult << "}";
						}

					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: university ID [" + scienceDegreeUniversityID + "] not found");
					}

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_university_id [" + feedActionId + "] not found");
				}
			}
			else if(feedActionTypeId == "40")
			{
				// --- language improved

				if(db->Query("SELECT * FROM `users_language` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  usersLanguageID = "";
					string  languageID = "";
					string  languageUserID = "";
					string  languageLevel = "";

					usersLanguageID = db->Get(0, "id");
					languageID = db->Get(0, "language_id");
					languageUserID = db->Get(0, "user_id");
					languageLevel = db->Get(0, "level");

					if(languageID.length() && db->Query("SELECT * FROM `language` WHERE `id`=\"" + languageID + "\";"))
					{
						string  languageTitle = db->Get(0, "title");
						string  languageLogoFolder = db->Get(0, "logo_folder");
						string  languageLogoFilename = db->Get(0, "logo_filename");
						string  messageParamLikesUserList = GetLanguageLikesUsersList(usersLanguageID, user, db);
						string  messageParamCommentsCount = GetLanguageCommentsCount(languageID, db);

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"languageID\":\""		  << languageID << "\",";
						ostResult << "\"usersLanguageID\":\""   << usersLanguageID << "\",";
						ostResult << "\"languageTitle\":\""   << languageTitle << "\",";
						ostResult << "\"languageLogoFolder\":\""<< languageLogoFolder << "\",";
						ostResult << "\"languageLogoFilename\":\""<< languageLogoFilename << "\",";
						ostResult << "\"languageLevel\":\""  << languageLevel << "\",";

						ostResult << "\"languageCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: language[" + languageID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_language_id [" + feedActionId + "] not found");
				}

			}
			else if(feedActionTypeId == "1")
			{
				// --- change employment

				if(db->Query("SELECT * FROM `users_company` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  usersCompanyID = "";
					string  companyID = "";
					string  companyUserID = "";
					string  companyPositionTitleID = "";

					usersCompanyID = db->Get(0, "id");
					companyID = db->Get(0, "company_id");
					companyUserID = db->Get(0, "user_id");
					companyPositionTitleID = db->Get(0, "position_title_id");

					if(companyID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + companyID + "\";"))
					{
						string  companyTitle = db->Get(0, "name");
						string  companyLogoFolder = db->Get(0, "logo_folder");
						string  companyLogoFilename = db->Get(0, "logo_filename");
						string  companyPositionTitle = "";
						string  messageParamLikesUserList = GetCompanyLikesUsersList(usersCompanyID, user, db);
						string  messageParamCommentsCount = GetCompanyCommentsCount(companyID, db);

						if(companyPositionTitleID.length() && db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + companyPositionTitleID + "\";"))
							companyPositionTitle = db->Get(0, "title");

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"companyID\":\""		<< companyID << "\",";
						ostResult << "\"usersCompanyID\":\""	 << usersCompanyID << "\",";
						ostResult << "\"companyTitle\":\""  << companyTitle << "\",";
						ostResult << "\"companyLogoFolder\":\""<< companyLogoFolder << "\",";
						ostResult << "\"companyLogoFilename\":\""<< companyLogoFilename << "\",";
						ostResult << "\"companyPositionTitleID\":\""	<< companyPositionTitleID << "\",";
						ostResult << "\"companyPositionTitle\":\""	<< companyPositionTitle << "\",";

						ostResult << "\"companyCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: company[" + companyID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_company_id [" + feedActionId + "] not found");
				}

			}
			else
			{

				if(ostResult.str().length() > 10) ostResult << ",";

				ostResult << "{";
				ostResult << "\"avatar\":\""			  << srcAvatarPath															<< "\",";
				ostResult << "\"srcObj\":{"	<< messageSrcObject << "},";
				ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
				ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
				ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
				ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
				ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
				ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
				ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId										   << "\",";
				ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp										<< "\",";
				// ostResult << "\"eventTimestampDelta\":\""  << GetHumanReadableTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)   << "\"";
				ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";
				ostResult << "}";

				// if(i < (affected - 1)) ostResult << ",";
			}
		}

		return ostResult.str();
}

string GetUnreadChatMessagesInJSONFormat(CUser *user, CMysql *db)
{
	ostringstream	result, ost;
	int				affected;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	result.str("");

	ost.str("");
	ost << "select * from `chat_messages` where `toID`='" << user->GetID() << "' and (`messageStatus`='unread' or `messageStatus`='delivered' or `messageStatus`='sent');";
	affected = db->Query(ost.str());
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			result << (i ? "," : "") << "{\
				\"id\": \""					<< db->Get(i, "id") << "\", \
				\"message\": \"" 			<< ReplaceDoubleQuoteToQuote(db->Get(i, "message")) << "\", \
				\"fromType\": \"" 			<< db->Get(i, "fromType") << "\",\
				\"fromID\": \""				<< db->Get(i, "fromID") << "\",\
				\"toType\": \""			 	<< db->Get(i, "toType") << "\",\
				\"toID\": \""	 			<< db->Get(i, "toID") << "\",\
				\"messageType\": \""		<< db->Get(i, "messageType") << "\",\
				\"messageStatus\": \""		<< db->Get(i, "messageStatus") << "\",\
				\"eventTimestamp\": \""		<< db->Get(i, "eventTimestamp") << "\"\
			}";
		}
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return	result.str();
}


// --- Function returns list of images belongs to imageSet
// --- input: imageSetID, db
// --- output: list of image objects
string GetMessageImageList(string imageSetID, CMysql *db)
{
	ostringstream	ost;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if(imageSetID != "0")
	{
		int				affected;

		ost.str("");
		ost << "select * from `feed_images` where `setID`='" << imageSetID << "';";
		affected = db->Query(ost.str());
		if(affected > 0)
		{
			ost.str("");
			for(int i = 0; i < affected; i++)
			{
				if(i > 0) ost << "," << std::endl;
				ost << "{";
				ost << "\"id\":\"" << db->Get(i, "id") << "\",";
				ost << "\"folder\":\"" << db->Get(i, "folder") << "\",";
				ost << "\"filename\":\"" << db->Get(i, "filename") << "\",";
				ost << "\"mediaType\":\"" << db->Get(i, "mediaType") << "\",";
				ost << "\"isActive\":\"" << db->Get(i, "isActive") << "\"";
				ost << "}";
			}

			result = ost.str();
		}
	}

	{
		CLog			log;
		ostringstream	ost;

		ost.str();
		ost <<  "GetMessageImageList: end. returning string length " << result.length();
		log.Write(DEBUG, ost.str());
	}

	return result;
}

// --- Function returns list of users "liked" messageID in JSON-format
// --- input: messageID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetMessageLikesUsersList(string messageID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='like' and `messageID`='" + messageID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from users where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersBookID in JSON-format
// --- input: usersBookID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetBookLikesUsersList(string usersBookID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeBook' and `messageID`='" + usersBookID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersLanguageID in JSON-format
// --- input: usersLanguageID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetLanguageLikesUsersList(string usersLanguageID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeLanguage' and `messageID`='" + usersLanguageID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersCompanyID in JSON-format
// --- input: usersCompanyID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetCompanyLikesUsersList(string usersCompanyID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeCompany' and `messageID`='" + usersCompanyID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersCertificationID in JSON-format
// --- input: usersCertificationID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetCertificationLikesUsersList(string usersCertificationID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeCertification' and `messageID`='" + usersCertificationID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersCourseID in JSON-format
// --- input: usersCourseID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetCourseLikesUsersList(string usersCourseID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeCourse' and `messageID`='" + usersCourseID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

string GetCompanyPositionIDByTitle(string positionTitle, CMysql *db)
{
	ostringstream   ost;
	string		  	result = "";
	string			positionID = "";

	MESSAGE_DEBUG("", "", "start");

	if(positionTitle.length())
	{
		if(db->Query("SELECT `id` FROM `company_position` WHERE `title`=\"" + positionTitle + "\";"))
		{
			positionID = db->Get(0, "id");
		}
		else
		{
			long int 	tmp;

			MESSAGE_DEBUG("", "", "company position not found. Creating new one.");

			tmp = db->InsertQuery("INSERT INTO `company_position` SET `area`=\"\", `title`=\"" + positionTitle + "\";");
			if(tmp)
				positionID = to_string(tmp);
			else
			{
				MESSAGE_ERROR("", "", "fail to insert to company_position table");
			}
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "positionTitle is empty");
	}

	result = positionID;

	MESSAGE_DEBUG("", "", "end (returning string length " + to_string(result.length()) + ")");

	return result;
}

string GetLanguageIDByTitle(string languageTitle, CMysql *db)
{
	ostringstream   ost;
	string		  	result = "";
	string			languageID = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if(languageTitle.length())
	{
		if(db->Query("SELECT `id` FROM `language` WHERE `title`=\"" + languageTitle + "\";"))
		{
			languageID = db->Get(0, "id");
		}
		else
		{
			long int 	tmp;
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: languageTitle [" + languageTitle + "] not found. Creating new one.");
			}

			tmp = db->InsertQuery("INSERT INTO `language` SET `title`=\"" + languageTitle + "\";");
			if(tmp)
				languageID = to_string(tmp);
			else
			{
				CLog			log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: insert into language");
			}
		}
	}
	else
	{
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: languageTitle is empty");
		}
	}

	result = languageID;

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}


	return result;
}

string GetSkillIDByTitle(string skillTitle, CMysql *db)
{
	ostringstream   ost;
	string		  	result = "";
	string			languageID = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if(skillTitle.length())
	{
		if(db->Query("SELECT `id` FROM `skill` WHERE `title`=\"" + skillTitle + "\";"))
		{
			languageID = db->Get(0, "id");
		}
		else
		{
			long int 	tmp;
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: skillTitle [" + skillTitle + "] not found. Creating new one.");
			}

			tmp = db->InsertQuery("INSERT INTO `skill` SET `title`=\"" + skillTitle + "\";");
			if(tmp)
				languageID = to_string(tmp);
			else
			{
				CLog			log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: insert into skill");
			}
		}
	}
	else
	{
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: skillTitle is empty");
		}
	}

	result = languageID;

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

string GetGeoLocalityIDByCityAndRegion(string regionName, string cityName, CMysql *db)
{
	ostringstream   ost;
	string		 	result = "";
	string			regionID = "", cityID = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if(regionName.length())
	{
		if(db->Query("SELECT `id` FROM `geo_region` WHERE `title`=\"" + regionName + "\";"))
		{
			regionID = db->Get(0, "id");
		}
		else
		{
			long int 	tmp;
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: region[" + regionName + "] not found. Creating new one.");
			}

			tmp = db->InsertQuery("INSERT INTO `geo_region` SET `geo_country_id`=\"0\", `title`=\"" + regionName + "\";");
			if(tmp)
				regionID = to_string(tmp);
			else
			{
				CLog			log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: insert into geo_region_db");
			}
		}
	}
	else
	{
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: regionName is empty");
		}
	}

	if(cityName.length())
	{
		if(db->Query("SELECT `id` FROM `geo_locality` WHERE `title`=\"" + cityName + "\" " + (regionID.length() ? " AND `geo_region_id`=\"" + regionID + "\" " : "") + ";"))
		{
			cityID = db->Get(0, "id");
		}
		else
		{
			long int 	tmp;
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: region[" + cityName + "] not found. Creating new one.");
			}

			tmp = db->InsertQuery("INSERT INTO `geo_locality` SET " + (regionID.length() ? "`geo_region_id`=\"" + regionID + "\"," : "") + " `title`=\"" + cityName + "\";");
			if(tmp)
				cityID = to_string(tmp);
			else
			{
				CLog			log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: insert into geo_region_db");
			}
		}
		result = cityID;
	}
	else
	{
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: cityName is empty");
		}
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" universityDegreeID in JSON-format
// --- input: universityDegreeID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetUniversityDegreeLikesUsersList(string universityDegreeID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeUniversityDegree' and `messageID`='" + universityDegreeID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" bookID in JSON-format
// --- input: bookID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetBookRatingUsersList(string bookID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `users_books` where `bookID`=\"" + bookID + "\";");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from users where id in (";
		for(int i = 0; i < affected; ++i)
		{
			if(i) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns array of book rating
// --- input: bookID, db
// --- output: book rating array
string GetBookRatingList(string bookID, CMysql *db)
{
	int			 affected;
	string		  result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `users_books` where `bookID`=\"" + bookID + "\";");
	if(affected > 0)
	{
		for(int i = 0; i < affected; ++i)
		{
			if(i) result += ",";
			result += db->Get(i, "rating");
		}
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns array of course rating
// --- input: courseID, db
// --- output: course rating array
string GetCourseRatingList(string courseID, CMysql *db)
{
	int				affected;
	string			result = "";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("select * from `users_courses` where `track_id`=\"" + courseID + "\";");
	if(affected > 0)
	{
		for(int i = 0; i < affected; ++i)
		{
			if(i) result += ",";
			result += db->Get(i, "rating");
		}
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

string GetMessageCommentsCount(string messageID, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_comment` where `type`=\"message\" and `messageID`='" << messageID << "';";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

string GetCompanyCommentsCount(string messageID, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_comment` where `type`=\"company\" and `messageID`='" << messageID << "';";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

string GetLanguageCommentsCount(string messageID, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_comment` where `type`=\"language\" and `messageID`='" << messageID << "';";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

string GetBookCommentsCount(string messageID, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_comment` where `type`=\"book\" and `messageID`='" << messageID << "';";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

string GetCertificateCommentsCount(string messageID, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_comment` where `type` in (\"certification\", \"course\") and `messageID`='" << messageID << "';";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

string GetUniversityDegreeCommentsCount(string messageID, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_comment` where `type`=\"university\" and `messageID`='" << messageID << "';";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

string GetMessageSpam(string messageID, CMysql *db)
{
	ostringstream	ost;
	int				affected;
	string			result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_params` where `parameter`='spam' and messageID='" << messageID << "';";
	affected = db->Query(ost.str());


	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

// --- Function returns true or false depends on userID "spamed" it or not
// --- input: messageID, userID
// --- output: was this message "spamed" by particular user or not
string GetMessageSpamUser(string messageID, string userID, CMysql *db)
{
	ostringstream	ost;
	int				affected;
	string			result = "0";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ost.str("");
	ost << "select count(*) as `counter` from `feed_message_params` where `parameter`='spam' and `messageID`='" << messageID << "' and `userID`='" << userID << "' ;";
	affected = db->Query(ost.str());
	if(affected > 0)
	{
		result = db->Get(0, "counter");
	}


	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}

	return result;
}

bool AllowMessageInNewsFeed(CUser *me, const string messageOwnerID, const string messageAccessRights, vector<string> *messageFriendList)
{

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:parameters (user [" + me->GetID() + "], messageOwnerID [" + messageOwnerID + "], messageAccessRights [" + messageAccessRights + "]): start");
	}

	// --- messages belons to yourself must be shown unconditionally
	// --- must be checked before message access private
	if(me->GetID() == messageOwnerID) return true;

	if(messageAccessRights == "public") return true;
	if(messageAccessRights == "private") return false;

	// --- require to check friendship list;
	if(messageAccessRights == "friends")
	{
		for(auto it = messageFriendList->begin(); it != messageFriendList->end(); ++it)
		{
			if(*it == messageOwnerID) return true;
		}

		return false;
	}

	return true;
}

// --- rate-limit on sessid persistence
// --- input: REMOTE_ADDR
// --- output: true, if rate-limit required
// ---		 false, if rate-limit not required
bool isPersistenceRateLimited(string REMOTE_ADDR, CMysql *db)
{
	int 			maxTime = 60, maxAttempts = 3;
	ostringstream	ost;
	string			rateLimitID = "";
	int				affected, attempts;
	bool			result = false;

	{
		CLog	log;
		ostringstream	ostTemp;

		ostTemp.str("");
		ostTemp << "isPersistenceRateLimited: start. REMOTE_ADDR [" << REMOTE_ADDR << "]";
		log.Write(DEBUG, ostTemp.str());
	}

	// --- cleanup rate-limit table
	ost.str("");
	ost << "delete from `sessions_persistence_ratelimit` where `eventTimestamp` < (NOW() - interval " << maxTime << " second);";
	affected = db->Query(ost.str());

	ost.str("");
	ost << "select `id`, `attempts` from `sessions_persistence_ratelimit` where `ip`='" << REMOTE_ADDR << "';";
	affected = db->Query(ost.str());
	if(affected)
	{
		{
			CLog	log;
			ostringstream	ostTemp;

			ostTemp.str("");
			ostTemp << "isPersistenceRateLimited: REMOTE_ADDR in rate-limit table";
			log.Write(DEBUG, ostTemp.str());
		}
		rateLimitID = db->Get(0, "id");
		attempts = stoi(db->Get(0, "attempts"));
		ost.str("");
		ost << "update `sessions_persistence_ratelimit` set `attempts`=`attempts`+1 where `id`='" << rateLimitID << "';";
		db->Query(ost.str());

		if(attempts > maxAttempts)
		{
			{
				CLog	log;
				ostringstream	ostTemp;

				ostTemp.str("");
				ostTemp << "isPersistenceRateLimited: REMOTE_ADDR has tryed " << attempts << " times during the last " << maxTime << "sec. Needed to be rate-limited.";
				log.Write(DEBUG, ostTemp.str());
			}
			result = true;
		}
		else
		{
			{
				CLog	log;
				ostringstream	ostTemp;

				ostTemp.str("");
				ostTemp << "isPersistenceRateLimited: REMOTE_ADDR has tryed " << attempts << " times during the last " << maxTime << "sec. No need to rate-limit.";
				log.Write(DEBUG, ostTemp.str());
			}
			result = false;
		}
	}
	else
	{
		{
			CLog	log;
			ostringstream	ostTemp;

			ostTemp.str("");
			ostTemp << "isPersistenceRateLimited: REMOTE_ADDR not in rate-limit table";
			log.Write(DEBUG, ostTemp.str());
		}
		ost.str("");
		ost << "insert into `sessions_persistence_ratelimit` set `attempts`='1', `ip`='" << REMOTE_ADDR << "', `eventTimestamp`=NOW();";
		db->Query(ost.str());

		result = false;
	}

	{
		CLog	log;
		ostringstream	ostTemp;

		ostTemp.str("");
		ostTemp << "isPersistenceRateLimited: end. " << (result ? "rate-limit" : "no need rate-limit");
		log.Write(DEBUG, ostTemp.str());
	}

	return result;
}

void CopyFile(const string src, const string dst)
{
	clock_t start, end;

	{
		MESSAGE_DEBUG("", "", "start (copy " + src + " -> " + dst + ")");
	}

	start = clock();

	ifstream source(src.c_str(), ios::binary);
	ofstream dest(dst.c_str(), ios::binary);

	dest << source.rdbuf();

	source.close();
	dest.close();


	end = clock();

	{
		MESSAGE_DEBUG("", "", "finish (copying time is " + to_string((end - start) / CLOCKS_PER_SEC) + ")");
	}
}

// --- admin function
string GetCompanyDuplicates(CMysql *db)
{
	// --- map<companyName, companyID>
	map<string, string>		companyMap;

	// --- map<companyID, companyName>
	map<string, string>		duplicatesMap;
	ostringstream			ost, ostResult;

	{
		CLog	log;
		ostringstream	ostTemp;

		ostTemp.str("");
		ostTemp << "GetCompanyDuplicates: start ";
		log.Write(DEBUG, ostTemp.str());
	}

	ostResult.str("");
	ost.str("");
	ost << "select * from `company`;";
	for(int i = 0; i < db->Query(ost.str()); i++)
	{
		string		companyToTestName, companyToTestID;

		companyToTestID = db->Get(i, "id");
		companyToTestName = RemoveAllNonAlphabetSymbols(db->Get(i, "name"));

		auto it = companyMap.find(companyToTestName);
		if(it != companyMap.end())
		{
			// --- duplicate found
			duplicatesMap[companyToTestID] = companyToTestName;
			duplicatesMap[it->second] = it->first;
		}
		companyMap[companyToTestName] = companyToTestID;
	}

	for (const auto& it : duplicatesMap)
	{
		if(ostResult.str().length()) ostResult << ",";
		ost.str("");
		ost << "select * from `company` where `id`='" << it.first << "';";
		db->Query(ost.str());
		ostResult << "{\"id\":\"" << it.first << "\", \"companyName\":\"" << db->Get(0, "name") << "\"";

		ost.str("");
		ost << "select count(*) as `number_of_users` from `users_company` where `company_id`='" << it.first << "';";
		db->Query(ost.str());
		ostResult << ", \"usersInCompany\":\"" << db->Get(0, "number_of_users") << "\"}";
	}

	{
		CLog	log;
		ostringstream	ostTemp;

		ostTemp.str("");
		ostTemp << "GetCompanyDuplicates: end (number of duplicates: " << duplicatesMap.size() << ")";
		log.Write(DEBUG, ostTemp.str());
	}

	return ostResult.str();
}

// --- admin function
string GetPicturesWithEmptySet(CMysql *db)
{
	ostringstream		   ost, ostResult;
	int					 affected;

	{
		CLog	log;
		ostringstream   ostTemp;

		ostTemp.str("");
		ostTemp << "GetPicturesWithEmptySet: start ";
		log.Write(DEBUG, ostTemp.str());
	}

	ostResult.str("");
	ost.str("");
	ost << "SELECT * FROM `feed_images` where `setID`='0';";
	affected = db->Query(ost.str());
	for(int i = 0; i < affected; i++)
	{
		if(ostResult.str().length()) ostResult << ",";
		ostResult << "{\"id\":\"" << db->Get(i, "id") << "\",\"srcType\":\"" << db->Get(i, "srcType") << "\",\"userID\":\"" << db->Get(i, "userID") << "\",\"folder\":\"" << db->Get(i, "folder") << "\",\"filename\":\"" << db->Get(i, "filename") << "\"}";
	}

	{
		CLog	log;
		ostringstream   ostTemp;

		ostTemp.str("");
		ostTemp << "GetPicturesWithEmptySet: end (# of lost pictures: " << affected << ")";
		log.Write(DEBUG, ostTemp.str());
	}

	return ostResult.str();
}

// --- admin function
string GetPicturesWithUnknownMessage(CMysql *db)
{
	ostringstream					   ost, ostResult;
	int								 affected, lostCount = 0;
	unordered_set<unsigned long>		allImageSets;
	unordered_set<unsigned long>		lostImageSets;

	{
		CLog	log;
		ostringstream   ostTemp;

		ostTemp.str("");
		ostTemp << "GetPicturesWithUnknownMessage: start ";
		log.Write(DEBUG, ostTemp.str());
	}

	ostResult.str("");
	ost.str("");
	ost << "SELECT `setID` FROM `feed_images`;";
	affected = db->Query(ost.str());
	for(int i = 0; i < affected; i++)
	{
		allImageSets.insert(stol(db->Get(i, "setID")));
	}

	for(const unsigned long& id: allImageSets)
	{
		ost.str("");
		ost << "select count(*) as count from `feed_message` where `imageSetID`=\"" << id << "\";";
		db->Query(ost.str());
		if(!stoi(db->Get(0, "count")))
		{
			lostImageSets.insert(id);
		}
	}

	ostResult.str("");
	for(const unsigned long& id: lostImageSets)
	{
		ost.str("");
		ost << "select * from `feed_images` where `setID`=\"" << id << "\";";
		for(int i = 0; i < db->Query(ost.str()); i++, lostCount++)
		{
			if(ostResult.str().length()) ostResult << ",";
			ostResult << "{\"id\":\"" << db->Get(i, "id") << "\",\"srcType\":\"" << db->Get(i, "srcType") << "\",\"userID\":\"" << db->Get(i, "userID") << "\",\"setID\":\"" << db->Get(i, "setID") << "\",\"folder\":\"" << db->Get(i, "folder") << "\",\"filename\":\"" << db->Get(i, "filename") << "\"}";
		}
	}

	{
		CLog	log;
		ostringstream   ostTemp;

		ostTemp.str("");
		ostTemp << "GetPicturesWithUnknownMessage: end (# of lost pictures: " << affected << ")";
		log.Write(DEBUG, ostTemp.str());
	}

	return ostResult.str();
}

// --- admin function
string GetPicturesWithUnknownUser(CMysql *db)
{
	ostringstream					   		ost, ostResult;
	int								 		affected, lostCount = 0;
	unordered_set<string>					allImageOwners;
	unordered_set<unsigned long>			lostImages;

	{
		CLog	log;
		ostringstream   ostTemp;

		ostTemp.str("");
		ostTemp << "GetPicturesWithUnknownUser: start ";
		log.Write(DEBUG, ostTemp.str());
	}

	ostResult.str("");
	ost.str("");
	ost << "SELECT `srcType`,`userID` FROM `feed_images`;";
	affected = db->Query(ost.str());
	for(int i = 0; i < affected; i++)
		allImageOwners.insert(string(db->Get(i, "srcType")) + string(db->Get(i, "userID")));

	for(auto &item: allImageOwners)
	{
		string				tableName = "";
		string				id;
		size_t				pos;

		if((pos = item.find("company")) != string::npos)
		{
			tableName = "company";
			id = item.substr(string("company").length(), item.length() - string("company").length());
		}
		else if((pos = item.find("user")) != string::npos)
		{
			tableName = "users";
			id = item.substr(string("user").length(), item.length() - string("user").length());
		}

		if(tableName.length() && !db->Query("select `id` from `" + tableName + "` where `id`=\"" + id + "\";"))
			lostImages.insert(stol(id));
	}

	ostResult.str("");
	for(const unsigned long& id: lostImages)
	{
		ost.str("");
		ost << "select * from `feed_images` where `userID`=\"" << id << "\";";
		for(int i = 0; i < db->Query(ost.str()); i++, lostCount++)
		{
			if(ostResult.str().length()) ostResult << ",";
			ostResult << "{\"id\":\"" << db->Get(i, "id") << "\",\"srcType\":\"" << db->Get(i, "srcType") << "\",\"userID\":\"" << db->Get(i, "userID") << "\",\"setID\":\"" << db->Get(i, "setID") << "\",\"folder\":\"" << db->Get(i, "folder") << "\",\"filename\":\"" << db->Get(i, "filename") << "\"}";
		}
	}

	{
		CLog	log;
		ostringstream   ostTemp;

		ostTemp.str("");
		ostTemp << "GetPicturesWithUnknownUser: end (# of lost pictures: " << affected << ")";
		log.Write(DEBUG, ostTemp.str());
	}

	return ostResult.str();
}

// --- admin function
string GetRecommendationAdverse(CMysql *db)
{
	ostringstream					   ost, ostResult, dictionaryStatement;
	int								 affected;

	dictionaryStatement.str("");
	ostResult.str("");
	ost.str("");
	ost << "SELECT * FROM `dictionary_adverse`;";
	affected = db->Query(ost.str());
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			if(i) dictionaryStatement << " or ";
			dictionaryStatement << "(`title` like \"%" << db->Get(i, "word") << "%\")";
		}
	}

	ost.str("");
	ost << "select * from `users_recommendation` where `state` = 'unknown' and (" << dictionaryStatement.str() << ");";
	affected = db->Query(ost.str());
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			if(i) ostResult << ",";
			ostResult << "{";
			ostResult << "\"recommendationId\":\"" << db->Get(i, "id") <<"\",";
			ostResult << "\"recommendationRecommended_userID\":\"" << db->Get(i, "recommended_userID") <<"\",";
			ostResult << "\"recommendationRecommending_userID\":\"" << db->Get(i, "recommending_userID") <<"\",";
			ostResult << "\"recommendationTitle\":\"" << db->Get(i, "title") <<"\",";
			ostResult << "\"recommendationEventTimestamp\":\"" << db->Get(i, "eventTimestamp") <<"\",";
			ostResult << "\"recommendationState\":\"" << db->Get(i, "state") <<"\"";
			ostResult << "}";
		}
	}

	return ostResult.str();
}

// --- User notification part
string GetUserAvatarByUserID(string userID, CMysql *db)
{
	ostringstream   ost;
	string		  userAvatar = "";

	ost.str("");
	ost << "select * from `users_avatars` where `userid`='" << userID << "' and `isActive`='1';";
	if(db->Query(ost.str()))
	{
		ost.str("");
		ost << "/images/avatars/avatars" << db->Get(0, "folder") << "/" << db->Get(0, "filename");
		userAvatar = ost.str();
	}

	return userAvatar;
}

string  GetUserNotificationSpecificDataByType(unsigned long typeID, unsigned long actionID, CMysql *db, CUser *user)
{
	ostringstream   ostResult, ost;

	{
		CLog	log;

		log.Write(DEBUG, string(__func__) + "(typeID=" + to_string(typeID) + ", actionID=" + to_string(actionID) + ")[" + to_string(__LINE__) + "]: start");
	}


	ostResult.str("");

	// --- comment provided
	if(typeID == 19)
	{
		unsigned long   comment_id = actionID;

		if(db->Query("select * from  `feed_message_comment` where `id`=\"" + to_string(comment_id) + "\";"))
		{
			string  friend_userID = db->Get(0, "userID");
			string  commentTitle = db->Get(0, "comment");
			string  commentTimestamp = db->Get(0, "eventTimestamp");
			string  messageID = db->Get(0, "messageID");
			string  commentType = db->Get(0, "type");

			if(commentType == "message")
			{
				if(db->Query("select * from  `feed_message` where `id`=\"" + messageID + "\";"))
				{
					string  messageTitle = db->Get(0, "title");
					string  messageBody = db->Get(0, "message");
					string  messageImageSetID = db->Get(0, "imageSetID");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");
						string	  imageSetFolder = "";
						string	  imageSetPic = "";
						string	  messageMediaType = "";

						if(messageImageSetID.length() && (messageImageSetID != "0") && (db->Query("select * from `feed_images` where `setID`=\"" + messageImageSetID + "\";")))
						{
							imageSetFolder = db->Get(0, "folder");
							imageSetPic = db->Get(0, "filename");
							messageMediaType = db->Get(0, "mediaType");
						}
						else
						{
							CLog log;
							log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:typeID=" + to_string(typeID) + ": there is no media assigned to message");
						}

						ostResult << "\"notificationMessageID\":\"" << messageID << "\",";
						ostResult << "\"notificationMessageTitle\":\"" << messageTitle << "\",";
						ostResult << "\"notificationMessageBody\":\"" << messageBody << "\",";
						ostResult << "\"notificationMessageImageFolder\":\"" << imageSetFolder << "\",";
						ostResult << "\"notificationMessageImageName\":\"" << imageSetPic << "\",";
						ostResult << "\"notificationMessageMediaType\":\"" << messageMediaType << "\",";
						ostResult << "\"notificationCommentType\":\"message\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR finding messageID[" + messageID + "] in feed_message");
				}
			} // --- comment type "message"

			if(commentType == "book")
			{
				string	  bookID = messageID;

				if(db->Query("select * from  `book` where `id`=\"" + bookID + "\";"))
				{
					string  bookTitle = db->Get(0, "title");
					string  bookAuthorID = db->Get(0, "authorID");
					string  bookAuthor;
					string  bookISBN10 = db->Get(0, "isbn10");
					string  bookISBN13 = db->Get(0, "isbn13");
					string  coverPhotoFolder = db->Get(0, "coverPhotoFolder");
					string  coverPhotoFilename = db->Get(0, "coverPhotoFilename");

					if(bookAuthorID.length() && db->Query("select * from `book_author` where `id`=\"" + bookAuthorID + "\";"))
						bookAuthor = db->Get(0, "name");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationBookID\":\"" << bookID << "\",";
						ostResult << "\"notificationBookTitle\":\"" << bookTitle << "\",";
						ostResult << "\"notificationBookAuthorID\":\"" << bookAuthorID << "\",";
						ostResult << "\"notificationBookAuthor\":\"" << bookAuthor << "\",";
						ostResult << "\"notificationBookISBN10\":\"" << bookISBN10 << "\",";
						ostResult << "\"notificationBookISBN13\":\"" << bookISBN13 << "\",";
						ostResult << "\"notificationBookImageFolder\":\"" << coverPhotoFolder << "\",";
						ostResult << "\"notificationBookImageName\":\"" << coverPhotoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR finding book in feed_message");
				}
			} // --- comment type "book"

			if(commentType == "certification")
			{
				string	  trackID = messageID;
				string	  usersCertificationsID = "";
				string	  certificationNumber = "";

				if(db->Query("select * from `users_certifications` where `track_id`=\"" + trackID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					string	  usersCertificationsID = db->Get(0, "id");
					string	  certificationNumber = db->Get(0, "certification_number");
				}
				else
				{
					CLog log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:typeID=" + to_string(typeID) + ": finding users_certifications by (track_id[" + trackID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
				{
					string  certificationTitle = db->Get(0, "title");
					string  vendorID = db->Get(0, "vendor_id");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  companyName = "";

					if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
						companyName = db->Get(0, "name");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersCertificationID\":\"" << usersCertificationsID << "\",";
						ostResult << "\"notificationCertificationTitle\":\"" << certificationTitle << "\",";
						ostResult << "\"notificationCertificationNumber\":\"" << certificationNumber << "\",";
						ostResult << "\"notificationCertificationID\":\"" << trackID << "\",";
						ostResult << "\"notificationCertificationCompanyID\":\"" << vendorID << "\",";
						ostResult << "\"notificationCertificationCompanyName\":\"" << companyName << "\",";
						ostResult << "\"notificationCertificationImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationCertificationImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: certification.id[" + trackID + "] not found");
				}
			} // --- if(likeType == "certification")

			if(commentType == "course")
			{
				string	  trackID = messageID;
				string	  usersCoursesID = "";

				if(db->Query("select * from `users_courses` where `track_id`=\"" + trackID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersCoursesID = db->Get(0, "id");
				}
				else
				{
					CLog log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:typeID=" + to_string(typeID) + ": finding users_courses by (track_id[" + trackID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
				{
					string  courseTitle = db->Get(0, "title");
					string  vendorID = db->Get(0, "vendor_id");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  companyName = "";

					if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
						companyName = db->Get(0, "name");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersCourseID\":\"" << usersCoursesID << "\",";
						ostResult << "\"notificationCourseTitle\":\"" << courseTitle << "\",";
						ostResult << "\"notificationCourseID\":\"" << trackID << "\",";
						ostResult << "\"notificationCourseCompanyID\":\"" << vendorID << "\",";
						ostResult << "\"notificationCourseCompanyName\":\"" << companyName << "\",";
						ostResult << "\"notificationCourseImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationCourseImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: course.id[" + trackID + "] not found");
				}
			} // --- if(likeType == "course")


			if(commentType == "university")
			{
				string	  universityID = messageID;
				string	  usersUniversityDegreeID = "";
				string	  degree = "";
				string	  studyStart = "";
				string	  studyFinish = "";

				if(db->Query("select * from `users_university` where `university_id`=\"" + universityID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersUniversityDegreeID = db->Get(0, "id");
					degree = db->Get(0, "degree");
					studyStart = db->Get(0, "occupation_start");
					studyFinish = db->Get(0, "occupation_finish");
				}
				else
				{
					CLog log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: typeID=" + to_string(typeID) + ": finding users_universitys by (university_id[" + universityID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `university` where `id`=\"" + universityID + "\";"))
				{
					string  universityTitle = db->Get(0, "title");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  universityCountryID = "";
					string  universityCountryName = "";
					string  universityRegionID = db->Get(0, "geo_region_id");
					string  universityRegionName = "";

					if(universityRegionID.length() && db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + universityRegionID + "\";"))
					{
						universityRegionName = db->Get(0, "title");
						universityCountryID = db->Get(0, "geo_country_id");

						if(universityCountryID.length() && db->Query("SELECT * FROM `geo_country` WHERE `id`=\"" + universityCountryID + "\";"))
							universityCountryName = db->Get(0, "title");
					}

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersUniversityID\":\"" << usersUniversityDegreeID << "\",";
						ostResult << "\"notificationUniversityID\":\"" << universityID << "\",";
						ostResult << "\"notificationUniversityTitle\":\"" << universityTitle << "\",";
						ostResult << "\"notificationUniversityDegree\":\"" << degree << "\",";
						ostResult << "\"notificationUniversityStart\":\"" << studyStart << "\",";
						ostResult << "\"notificationUniversityFinish\":\"" << studyFinish << "\",";
						ostResult << "\"notificationUniversityRegionID\":\"" << universityRegionID << "\",";
						ostResult << "\"notificationUniversityRegionName\":\"" << universityRegionName << "\",";
						ostResult << "\"notificationUniversityCountryID\":\"" << universityCountryID << "\",";
						ostResult << "\"notificationUniversityCountryName\":\"" << universityCountryName << "\",";
						ostResult << "\"notificationUniversityImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationUniversityImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: university.id[" + universityID + "] not found");
				}
			} // --- if(commentType == "university")
			if(commentType == "company")
			{
				string	  companyID = messageID;

				string	  usersCompanyID = "";
				string	  positionTitleID = "";
				string	  employmentStart = "";
				string	  employmentFinish = "";

				if(db->Query("select * from `users_company` where `company_id`=\"" + companyID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersCompanyID = db->Get(0, "id");
					positionTitleID = db->Get(0, "position_title_id");
					employmentStart = db->Get(0, "occupation_start");
					employmentFinish = db->Get(0, "occupation_finish");
				}
				else
				{
					CLog log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:typeID=" + to_string(typeID) + ": finding users_companys by (company_id[" + companyID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `company` where `id`=\"" + companyID + "\";"))
				{
					string  companyName = db->Get(0, "name");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  companyWebSite = db->Get(0, "webSite");
					string  companyFoundationDate = db->Get(0, "foundationDate");
					string  positionTitle = "";

					if(positionTitleID.length() && db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + positionTitleID + "\";"))
						positionTitle = db->Get(0, "title");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersCompanyID\":\"" << usersCompanyID << "\",";
						ostResult << "\"notificationCompanyID\":\"" << companyID << "\",";
						ostResult << "\"notificationCompanyName\":\"" << companyName << "\",";
						ostResult << "\"notificationCompanyPositionTitleID\":\"" << positionTitleID << "\",";
						ostResult << "\"notificationCompanyPositionTitle\":\"" << positionTitle << "\",";
						ostResult << "\"notificationCompanyStart\":\"" << employmentStart << "\",";
						ostResult << "\"notificationCompanyFinish\":\"" << employmentFinish << "\",";
						ostResult << "\"notificationCompanyFoundationDate\":\"" << companyFoundationDate << "\",";
						ostResult << "\"notificationCompanyWebSite\":\"" << companyWebSite << "\",";
						ostResult << "\"notificationCompanyImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationCompanyImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: company.id[" + companyID + "] not found");
				}
			} // --- if(commentType == "company")
			if(commentType == "language")
			{
				string	  languageID = messageID;
				string	  usersLanguageID = "";
				string	  languageLevel = "";

				if(db->Query("select * from `users_language` where `language_id`=\"" + languageID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersLanguageID = db->Get(0, "id");
					languageLevel = db->Get(0, "level");
				}
				else
				{
					CLog log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:typeID=" + to_string(typeID) + ": finding users_languages by (language_id[" + languageID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `language` where `id`=\"" + languageID + "\";"))
				{
					string  languageTitle = db->Get(0, "title");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersLanguageID\":\"" << usersLanguageID << "\",";
						ostResult << "\"notificationLanguageID\":\"" << languageID << "\",";
						ostResult << "\"notificationLanguageTitle\":\"" << languageTitle << "\",";
						ostResult << "\"notificationLanguageLevel\":\"" << languageLevel << "\",";
						ostResult << "\"notificationLanguageImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationLanguageImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: language.id[" + languageID + "] not found");
				}
			} // --- if(commentType == "language")
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_recommendation");
		}
	}


	// --- skill confirmed
	if(typeID == 43)
	{
		unsigned long   skill_confirmed_id = actionID;

		if(db->Query("select * from `skill_confirmed` where `id`=\"" + to_string(skill_confirmed_id) + "\";"))
		{
			string  users_skill_id = db->Get(0, "users_skill_id");
			string  approver_userID = db->Get(0, "approver_userID");

			ost.str("");
			ost << "select * from `users` where `id`='" << approver_userID << "';";
			if(db->Query(ost.str()))
			{
				string  approver_userName = db->Get(0, "name");
				string  approver_userNameLast = db->Get(0, "nameLast");

				ost.str("");
				ost << "select * from `users_skill` where `id`='" << users_skill_id << "';";
				if(db->Query(ost.str()))
				{
					string  skillID = db->Get(0, "skill_id");

					ost.str("");
					ost << "select * from `skill` where `id`='" << skillID << "';";
					if(db->Query(ost.str()))
					{
						string  skillTitle = db->Get(0, "title");

						ostResult << "\"notificationFriendUserID\":\"" << approver_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << approver_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << approver_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(approver_userID, db) << "\",";
						ostResult << "\"notificationSkillTitle\":\"" << skillTitle << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_skill");
				}

			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill_confirmed");
		}
	}

	// --- skill removed
	if(typeID == 44)
	{
		ost.str("");
		{
			unsigned long   users_skill_id = actionID;

			{
				ost.str("");
				ost << "select * from `users_skill` where `id`='" << users_skill_id << "';";
				if(db->Query(ost.str()))
				{
					string  skillID = db->Get(0, "skill_id");

					ost.str("");
					ost << "select * from `skill` where `id`='" << skillID << "';";
					if(db->Query(ost.str()))
					{
						string  skillTitle = db->Get(0, "title");

						ostResult << "\"notificationSkillTitle\":\"" << skillTitle << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_skill");
				}

			}

		}
	}

	// --- recommendation provided
	if(typeID == 45)
	{
		ost.str("");
		{
			unsigned long   users_recommendation_id = actionID;

			{
				ost.str("");
				ost << "select * from `users_recommendation` where `id`=\"" << users_recommendation_id << "\";";
				if(db->Query(ost.str()))
				{
					string  recommended_userID = db->Get(0, "recommended_userID");
					string  recommending_userID = db->Get(0, "recommending_userID");
					string  title = db->Get(0, "title");
					string  eventTimestamp = db->Get(0, "eventTimestamp");

					ost.str("");
					ost << "select * from `users` where `id`='" << recommending_userID << "';";
					if(db->Query(ost.str()))
					{
						string  recommending_userName = db->Get(0, "name");
						string  recommending_userNameLast = db->Get(0, "nameLast");

						ostResult << "\"notificationFriendUserID\":\"" << recommending_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << recommending_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << recommending_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(recommending_userID, db) << "\",";
						ostResult << "\"notificationRecommendationID\":\"" << users_recommendation_id << "\",";
						ostResult << "\"notificationRecommendationTitle\":\"" << title << "\",";
						ostResult << "\"notificationRecommendationEventTimestamp\":\"" << eventTimestamp << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_recommendation (probably deleted already)");
				}

			}

		}
	}

	// --- 46/47 recommendation deleted by benefit-owner
	if((typeID == 46) || (typeID == 47))
	{
		ost.str("");
		{
			unsigned long   friend_userID = actionID;

			{
				{
					ost.str("");
					ost << "select * from `users` where `id`='" << friend_userID << "';";
					if(db->Query(ost.str()))
					{
						string  friend_userName = db->Get(0, "name");
						string  friend_userNameLast = db->Get(0, "nameLast");

						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(to_string(friend_userID), db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
			}
		}
	}

	// --- recommendation modified
	if(typeID == 48)
	{
		unsigned long   user_recommendation_id = actionID;

		ost.str("");
		ost << "select * from  `users_recommendation` where `id`=\"" << user_recommendation_id << "\";";
		if(db->Query(ost.str()))
		{
			string  recommended_userID = db->Get(0, "recommended_userID");
			string  friend_userID = db->Get(0, "recommending_userID");
			string  recommendationTitle = db->Get(0, "title");
			string  recommendationEventTimestamp = db->Get(0, "eventTimestamp");

			{
				{
					ost.str("");
					ost << "select * from `users` where `id`='" << friend_userID << "';";
					if(db->Query(ost.str()))
					{
						string  friend_userName = db->Get(0, "name");
						string  friend_userNameLast = db->Get(0, "nameLast");

						ostResult << "\"notificationRecommendationID\":\"" << user_recommendation_id << "\",";
						ostResult << "\"notificationRecommendationTitle\":\"" << recommendationTitle << "\",";
						ostResult << "\"notificationRecommendationEventTimestamp\":\"" << recommendationEventTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
			}
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_recommendation");
		}
	}

	// --- message liked
	if(typeID == 49)
	{
		unsigned long   feed_message_params_id = actionID;

		if(db->Query("select * from  `feed_message_params` where `id`=\"" + to_string(feed_message_params_id) + "\";"))
		{
			string  friend_userID = db->Get(0, "userID");
			string  messageID = db->Get(0, "messageID");
			string  commentType = db->Get(0, "parameter");

			if((commentType == "like"))
			{
				if(db->Query("select * from  `feed_message` where `id`=\"" + messageID + "\";"))
				{
					string  messageTitle = db->Get(0, "title");
					string  messageBody = db->Get(0, "message");
					string  messageImageSetID = db->Get(0, "imageSetID");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");
							string	  imageSetFolder = "";
							string	  imageSetPic = "";
							string	  messageMediaType = "";

							if((messageImageSetID != "0") && (db->Query("select * from `feed_images` where `setID`=\"" + messageImageSetID + "\";")))
							{
								imageSetFolder = db->Get(0, "folder");
								imageSetPic = db->Get(0, "filename");
								messageMediaType = db->Get(0, "mediaType");
							}
							else
							{
								CLog log;
								log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: typeID=49: there is no media assigned to message");
							} // --- imageSet is empty

							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationMessageID\":\"" << messageID << "\",";
							ostResult << "\"notificationMessageTitle\":\"" << messageTitle << "\",";
							ostResult << "\"notificationMessageBody\":\"" << messageBody << "\",";
							ostResult << "\"notificationMessageImageFolder\":\"" << imageSetFolder << "\",";
							ostResult << "\"notificationMessageImageName\":\"" << imageSetPic << "\",";
							ostResult << "\"notificationMessageMediaType\":\"" << messageMediaType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";
						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: selecting from users");
						}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding message in feed_message");
				}
			} // --- if(likeType == "message")


			if(commentType == "likeBook")
			{
				string	  usersBooksID = messageID;

				if(db->Query("select * from `users_books` where `id`=\"" + usersBooksID + "\";"))
				{
					string	  bookID = db->Get(0, "bookID");

					if(db->Query("select * from  `book` where `id`=\"" + bookID + "\";"))
					{
						string  bookTitle = db->Get(0, "title");
						string  bookAuthorID = db->Get(0, "authorID");
						string  bookAuthor;
						string  bookISBN10 = db->Get(0, "isbn10");
						string  bookISBN13 = db->Get(0, "isbn13");
						string  coverPhotoFolder = db->Get(0, "coverPhotoFolder");
						string  coverPhotoFilename = db->Get(0, "coverPhotoFilename");

						if(bookAuthorID.length() && db->Query("select * from `book_author` where `id`=\"" + bookAuthorID + "\";"))
							bookAuthor = db->Get(0, "name");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationBookID\":\"" << bookID << "\",";
							ostResult << "\"notificationBookTitle\":\"" << bookTitle << "\",";
							ostResult << "\"notificationBookAuthorID\":\"" << bookAuthorID << "\",";
							ostResult << "\"notificationBookAuthor\":\"" << bookAuthor << "\",";
							ostResult << "\"notificationBookISBN10\":\"" << bookISBN10 << "\",";
							ostResult << "\"notificationBookISBN13\":\"" << bookISBN13 << "\",";
							ostResult << "\"notificationBookImageFolder\":\"" << coverPhotoFolder << "\",";
							ostResult << "\"notificationBookImageName\":\"" << coverPhotoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: book.id[" + bookID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_books.id[" + usersBooksID + "]");
				}
			} // --- if(likeType == "book")

			if(commentType == "likeCertification")
			{
				string	  usersCertificationsID = messageID;

				if(db->Query("select * from `users_certifications` where `id`=\"" + usersCertificationsID + "\";"))
				{
					string	  trackID = db->Get(0, "track_id");
					string	  certificationNumber = db->Get(0, "certification_number");

					if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
					{
						string  certificationTitle = db->Get(0, "title");
						string  vendorID = db->Get(0, "vendor_id");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");
						string  companyName = "";

						if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
							companyName = db->Get(0, "name");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersCertificationID\":\"" << usersCertificationsID << "\",";
							ostResult << "\"notificationCertificationID\":\"" << trackID << "\",";
							ostResult << "\"notificationCertificationTitle\":\"" << certificationTitle << "\",";
							ostResult << "\"notificationCertificationNumber\":\"" << certificationNumber << "\",";
							ostResult << "\"notificationCertificationCompanyID\":\"" << vendorID << "\",";
							ostResult << "\"notificationCertificationCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCertificationImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationCertificationImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: certification.id[" + trackID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_certifications.id[" + usersCertificationsID + "]");
				}
			} // --- if(likeType == "certification")

			if(commentType == "likeCourse")
			{
				string	  usersCoursesID = messageID;

				if(db->Query("select * from `users_courses` where `id`=\"" + usersCoursesID + "\";"))
				{
					string	  trackID = db->Get(0, "track_id");

					if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
					{
						string  courseTitle = db->Get(0, "title");
						string  vendorID = db->Get(0, "vendor_id");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");
						string  companyName = "";

						if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
							companyName = db->Get(0, "name");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersCourseID\":\"" << usersCoursesID << "\",";
							ostResult << "\"notificationCourseID\":\"" << trackID << "\",";
							ostResult << "\"notificationCourseTitle\":\"" << courseTitle << "\",";
							ostResult << "\"notificationCourseCompanyID\":\"" << vendorID << "\",";
							ostResult << "\"notificationCourseCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCourseImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationCourseImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: course.id[" + trackID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_courses.id[" + usersCoursesID + "]");
				}
			} // --- if(likeType == "course")
			if(commentType == "likeLanguage")
			{
				string	  usersLanguagesID = messageID;

				if(db->Query("select * from `users_language` where `id`=\"" + usersLanguagesID + "\";"))
				{
					string	  languageID = db->Get(0, "language_id");
					string	  languageLevel = db->Get(0, "level");

					if(db->Query("select * from  `language` where `id`=\"" + languageID + "\";"))
					{
						string  languageTitle = db->Get(0, "title");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersLanguageID\":\"" << usersLanguagesID << "\",";
							ostResult << "\"notificationLanguageID\":\"" << languageID << "\",";
							ostResult << "\"notificationLanguageTitle\":\"" << languageTitle << "\",";
							ostResult << "\"notificationLanguageLevel\":\"" << languageLevel << "\",";
							ostResult << "\"notificationLanguageImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationLanguageImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: language.id[" + languageID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_languages.id[" + usersLanguagesID + "]");
				}
			} // --- if(likeType == "language")

			if(commentType == "likeCompany")
			{
				string	  usersCompanysID = messageID;

				if(db->Query("select * from `users_company` where `id`=\"" + usersCompanysID + "\";"))
				{
					string	  companyID = db->Get(0, "company_id");
					string	  positionTitleID = db->Get(0, "position_title_id");
					string	  positionTitle = "";

					if(db->Query("select * from  `company` where `id`=\"" + companyID + "\";"))
					{
						string  companyName = db->Get(0, "name");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");

						if(positionTitleID.length() && db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + positionTitleID + "\";"))
							positionTitle = db->Get(0, "title");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersCompanyID\":\"" << usersCompanysID << "\",";
							ostResult << "\"notificationCompanyID\":\"" << companyID << "\",";
							ostResult << "\"notificationCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCompanyCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCompanyPositionTitleID\":\"" << positionTitleID << "\",";
							ostResult << "\"notificationCompanyPositionTitle\":\"" << positionTitle << "\",";
							ostResult << "\"notificationCompanyImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationCompanyImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: company.id[" + companyID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_companys.id[" + usersCompanysID + "]");
				}
			} // --- if(likeType == "company")

			if(commentType == "likeUniversityDegree")
			{
				string	  usersUniversityDegreeID = messageID;

				if(db->Query("select * from `users_university` where `id`=\"" + usersUniversityDegreeID + "\";"))
				{
					string	  universityID = db->Get(0, "university_id");
					string	  degree = db->Get(0, "degree");
					string	  studyStart = db->Get(0, "occupation_start");
					string	  studyFinish = db->Get(0, "occupation_finish");

					if(db->Query("select * from  `university` where `id`=\"" + universityID + "\";"))
					{
						string  universityTitle = db->Get(0, "title");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");
						string  universityCountryID = "";
						string  universityCountryName = "";
						string  universityRegionID = db->Get(0, "geo_region_id");
						string  universityRegionName = "";

						if(universityRegionID.length() && db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + universityRegionID + "\";"))
						{
							universityRegionName = db->Get(0, "title");
							universityCountryID = db->Get(0, "geo_country_id");

							if(universityCountryID.length() && db->Query("SELECT * FROM `geo_country` WHERE `id`=\"" + universityCountryID + "\";"))
								universityCountryName = db->Get(0, "title");
						}


						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersUniversityID\":\"" << messageID << "\",";
							ostResult << "\"notificationUniversityID\":\"" << universityID << "\",";
							ostResult << "\"notificationUniversityTitle\":\"" << universityTitle << "\",";
							ostResult << "\"notificationUniversityDegree\":\"" << degree << "\",";
							ostResult << "\"notificationUniversityStart\":\"" << studyStart << "\",";
							ostResult << "\"notificationUniversityFinish\":\"" << studyFinish << "\",";
							ostResult << "\"notificationUniversityRegionID\":\"" << universityRegionID << "\",";
							ostResult << "\"notificationUniversityRegionName\":\"" << universityRegionName << "\",";
							ostResult << "\"notificationUniversityCountryID\":\"" << universityCountryID << "\",";
							ostResult << "\"notificationUniversityCountryName\":\"" << universityCountryName << "\",";
							ostResult << "\"notificationUniversityImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationUniversityImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: university.id[" + universityID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_universitys.id[" + usersUniversityDegreeID + "]");
				}
			} // --- if(commentType == "university")



		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from feed_message_params");
		}
	}

	// --- message disliked
	if(typeID == 50)
	{
		unsigned long   messageID = actionID;

		{

			ost.str("");
			ost << "select * from  `feed_message` where `id`=\"" << messageID << "\";";
			if(db->Query(ost.str()))
			{
				string	  messageTitle = db->Get(0, "title");
				string	  messageBody = db->Get(0, "message");
				string	  messageImageSetID = db->Get(0, "imageSetID");
				string	  imageSetFolder = "";
				string	  imageSetPic = "";
				string	  messageMediaType = "";

				if(db->Query("select * from `feed_images` where `setID`=\"" + messageImageSetID + "\";"))
				{
					imageSetFolder = db->Get(0, "folder");
					imageSetPic = db->Get(0, "filename");
					messageMediaType = db->Get(0, "mediaType");
				}
				else
				{
					CLog log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: typeID=50: there is no media assigned to message");
				}

				ostResult << "\"notificationMessageID\":\"" << messageID << "\",";
				ostResult << "\"notificationMessageTitle\":\"" << messageTitle << "\",";
				ostResult << "\"notificationMessageBody\":\"" << messageBody << "\",";
				ostResult << "\"notificationMessageMediaType\":\"" << messageMediaType << "\",";
				ostResult << "\"notificationMessageImageFolder\":\"" << imageSetFolder << "\",";
				ostResult << "\"notificationMessageImageName\":\"" << imageSetPic << "\"";
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR finding message int feed_message");
			}
		}
	}

	// --- birthday notification
	if(typeID == 58)
	{
		string   birthdayUserID = to_string(actionID);

		if(birthdayUserID.length() && db->Query("select * from `users` where `id`='" + birthdayUserID + "';"))
		{
			string  birthday_userName = db->Get(0, "name");
			string  birthday_userNameLast = db->Get(0, "nameLast");
			string	birthdayDate = db->Get(0, "birthday");


			ostResult << "\"notificationBirthdayDate\":\"" << birthdayDate << "\",";
			ostResult << "\"notificationFriendUserID\":\"" << birthdayUserID << "\",";
			ostResult << "\"notificationFriendUserName\":\"" << birthday_userName << "\",";
			ostResult << "\"notificationFriendUserNameLast\":\"" << birthday_userNameLast << "\",";
			ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(birthdayUserID, db) << "\"";
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + birthdayUserID + "]");
		}
	}

	// --- gift thank notification
	if(typeID == 66)
	{
		string   giftThankID = to_string(actionID);

		if(giftThankID.length() && db->Query("select * from `gift_thanks` where `id`='" + giftThankID + "';"))
		{
			string  friend_userID = db->Get(0, "src_user_id");
			string  dst_user_id = db->Get(0, "dst_user_id");
			string  gift_id = db->Get(0, "gift_id");
			string  comment = db->Get(0, "comment");

			if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
			{
				string	  friend_userName = db->Get(0, "name");
				string	  friend_userNameLast = db->Get(0, "nameLast");

				ostResult << "\"gifts\":[" << GetGiftListInJSONFormat("SELECT * FROM `gifts` WHERE `id`=\"" + gift_id + "\";", db, user) << "],";
				ostResult << "\"notificationComment\":\"" << comment << "\",";
				ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
				ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
				ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
				ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from gift_thanks[id = " + giftThankID + "]");
		}
	}

	// --- Vacancy Rejected
	if(typeID == 59)
	{
		string   company_candidates_id = to_string(actionID);

		if(company_candidates_id.length() && db->Query("select * from `company_candidates` where `id`='" + company_candidates_id + "';"))
		{
			string  user_id = db->Get(0, "user_id");
			string  vacancy_id = db->Get(0, "vacancy_id");

			if(vacancy_id.length() && db->Query("select * from `company_vacancy` where `id`='" + vacancy_id + "';"))
			{
				string		company_id = db->Get(0, "company_id");

				ostResult << "\"notificationCompanyCandidatesID\":\"" << company_candidates_id << "\",";
				ostResult << "\"notificationVacancy\":[" << GetOpenVacanciesInJSONFormat("SELECT * FROM `company_vacancy` WHERE `id`=\"" + vacancy_id + "\";", db) << "],";
				ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_vacancy[id = " + vacancy_id + "]");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_candidates[id = " + company_candidates_id + "]");
		}
	}

	// --- Company Posession Request
	if(typeID == 60)
	{
		string   company_posesison_request_id = to_string(actionID);

		if(company_posesison_request_id.length() && db->Query("select * from `company_posession_request` where `id`='" + company_posesison_request_id + "';"))
		{
			string  friendUserID = db->Get(0, "requester_user_id");
			string  company_id = db->Get(0, "requested_company_id");
			string	description = db->Get(0, "description");
			string	status = db->Get(0, "status");
			string	eventTimestamp = db->Get(0, "eventTimestamp");

			if(company_id.length() && db->Query("select * from `company` where `id`='" + company_id + "';"))
			{

				if(friendUserID.length() && db->Query("select * from `users` where `id`='" + friendUserID + "';"))
				{
					string  friend_userName = db->Get(0, "name");
					string  friend_userNameLast = db->Get(0, "nameLast");

					ostResult << "\"notificationCompanyPosessionRequestID\":\"" << company_posesison_request_id << "\",";
					ostResult << "\"notificationDescription\":\"" << description << "\",";
					ostResult << "\"notificationFriendUserID\":\"" << friendUserID << "\",";
					ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
					ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
					ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friendUserID, db) << "\",";
					ostResult << "\"notificationRequestedCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "],";
					ostResult << "\"notificationPosessionStatus\":\"" << status << "\",";
					ostResult << "\"notificationEventTimestamp\":\"" << eventTimestamp << "\"";

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + friendUserID + "]");
				}
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_vacancy[id = " + company_id + "]");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_posession_request[id = " + company_posesison_request_id + "]");
		}
	}

	// --- company posession approved / rejected
	if((typeID == 61) || (typeID == 62))
	{
		string   company_id = to_string(actionID);

		if(company_id.length())
		{
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_candidates[id = " + company_id + "]");
		}
	}

	// --- 67 - Event host added
	// --- 68 - Event guest added
	// --- 69 - Guest accepted
	if((typeID == 67) || (typeID == 68) || (typeID == 69))
	{
		string		event_host_id = to_string(actionID);
		string		host_guest_sql;

		if(typeID == 67)
			host_guest_sql = "SELECT * FROM `event_hosts` WHERE `id`=\"" + event_host_id + "\"";
		else
			host_guest_sql = "SELECT * FROM `event_guests` WHERE `id`=\"" + event_host_id + "\"";

		if(event_host_id.length() && db->Query(host_guest_sql))
		{
			string  event_id = db->Get(0, "event_id");
			string	eventTimestamp = db->Get(0, "eventTimestamp");
			string  friendUserID = "";

			if(typeID == 69) friendUserID = db->Get(0, "user_id");

			if(event_id.length() && db->Query("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";"))
			{
				if(!friendUserID.length()) friendUserID = db->Get(0, "owner_id");

				if(friendUserID.length() && db->Query("select * from `users` where `id`='" + friendUserID + "';"))
				{
					string  friend_userName = db->Get(0, "name");
					string  friend_userNameLast = db->Get(0, "nameLast");
					string  friend_userSex = db->Get(0, "sex");

					ostResult << "\"notificationFriendUserID\":\"" << friendUserID << "\",";
					ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
					ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
					ostResult << "\"notificationFriendUserSex\":\"" << friend_userSex << "\",";
					ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friendUserID, db) << "\",";
					ostResult << "\"notificationEvent\":[" << GetEventListInJSONFormat("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";", db, NULL) << "]";
					// ostResult << "\"notificationEventTimestamp\":\"" << eventTimestamp << "\"";

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + friendUserID + "]");
				}
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from event[id = " + event_id + "]");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from event_hosts/event_guests[id = " + event_host_id + "]");
		}
	}

	// --- 70 - 1 day notification about start
	if(typeID == 70)
	{
		string		event_id = to_string(actionID);

		if(event_id.length() && db->Query("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";"))
		{
			string	eventTimestamp = db->Get(0, "startTimestamp");
			string  friendUserID = db->Get(0, "owner_id");

			if(friendUserID.length() && db->Query("select * from `users` where `id`='" + friendUserID + "';"))
			{
				string  friend_userName = db->Get(0, "name");
				string  friend_userNameLast = db->Get(0, "nameLast");

				ostResult << "\"notificationFriendUserID\":\"" << friendUserID << "\",";
				ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
				ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
				ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friendUserID, db) << "\",";
				ostResult << "\"notificationEvent\":[" << GetEventListInJSONFormat("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";", db, NULL) << "]";
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + friendUserID + "]");
			}
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from event[id = " + event_id + "]");
		}

	}

	if(typeID == NOTIFICATION_GENERAL_FROM_USER)
	{
		string	from_company_id = to_string(actionID);

		if(from_company_id.length())
		{
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + from_company_id + "\";", db, NULL) << "]";
		}
/*
		string	from_user_id = to_string(actionID);
		
		if(from_user_id.length() && db->Query("select * from `users` where `id`='" + from_user_id + "';"))
		{
			ostResult << "\"notificationFriendUserID\":\"" << from_user_id << "\",";
			ostResult << "\"notificationFriendUserName\":\"" << db->Get(0, "name") << "\",";
			ostResult << "\"notificationFriendUserNameLast\":\"" << db->Get(0, "nameLast") << "\",";
			ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(from_user_id, db) << "\"";
		}
*/
	}

	if(typeID == NOTIFICATION_GENERAL_FROM_COMPANY)
	{
		string	from_company_id = to_string(actionID);

		if(from_company_id.length())
		{
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + from_company_id + "\";", db, NULL) << "]";
		}
	}

	if(typeID == NOTIFICATION_CHANGE_CUSTOMER_NAME)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id` IN ("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
							"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\""
						")"
					")"
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardCustomersInJSONFormat("SELECT * FROM `timecard_customers` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by customer.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_CHANGE_PROJECT_NAME)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id` IN ("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\""
					")"
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardProjectsInJSONFormat("SELECT * FROM `timecard_projects` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by project.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_CHANGE_TASK_NAME)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + id + "\""
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardTasksInJSONFormat("SELECT * FROM `timecard_tasks` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by task.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_CHANGE_TASK_DURATION)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\""
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardTaskAssignmentInJSONFormat("SELECT * FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by task.id(" + id + ")");
		}
	}



	{
		CLog	log;
		ostringstream   ost;

		ost.str("");
		ost << __func__ << "(typeID=" << typeID << ", actionID=" << actionID << ")[" << __LINE__ << "]: end (return strlen=" << ostResult.str().length() << ")";
		log.Write(DEBUG, ost.str());
	}

	return  ostResult.str();
}

string  GetUserNotificationInJSONFormat(string sqlRequest, CMysql *db, CUser *user)
{
	int			 affected;
	ostringstream   ostUserNotifications;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: start");
	}

	ostUserNotifications.str("");
	ostUserNotifications << "[";

	affected = db->Query(sqlRequest);
	if(affected)
	{
		class DBUserNotification
		{
			public:
				string	  notificationID;
				string	  notificationStatus;
				string	  notification_title;
				string	  feed_eventTimestamp;
				string	  feed_actionId;
				string	  feed_actionTypeId;
				string	  action_types_title;
				string	  action_types_title_male;
				string	  action_types_title_female;
				string	  user_id;
				string	  user_name;
				string	  user_nameLast;
				string	  user_sex;
				string	  user_email;
				string	  action_category_title;
				string	  action_category_title_male;
				string	  action_category_title_female;
				string	  action_category_id;
		};

		vector<DBUserNotification>  dbResult;

		for(int i = 0; i < affected; ++i)
		{
			DBUserNotification	  tmpObj;

			tmpObj.notificationID = db->Get(i, "users_notification_id");
			tmpObj.notificationStatus = db->Get(i, "users_notification_notificationStatus");
			tmpObj.notification_title = db->Get(i, "users_notification_title");
			tmpObj.feed_actionTypeId = db->Get(i, "feed_actionTypeId");
			tmpObj.action_types_title = db->Get(i, "action_types_title");
			tmpObj.action_types_title_male = db->Get(i, "action_types_title_male");
			tmpObj.action_types_title_female = db->Get(i, "action_types_title_female");
			tmpObj.action_category_title = db->Get(i, "action_category_title");
			tmpObj.action_category_title_male = db->Get(i, "action_category_title_male");
			tmpObj.action_category_title_female = db->Get(i, "action_category_title_female");
			tmpObj.feed_eventTimestamp = db->Get(i, "feed_eventTimestamp");
			tmpObj.user_id = db->Get(i, "user_id");
			tmpObj.user_name = db->Get(i, "user_name");
			tmpObj.user_nameLast = db->Get(i, "user_nameLast");
			tmpObj.user_sex = db->Get(i, "user_sex");
			tmpObj.user_email = db->Get(i, "user_email");
			tmpObj.action_category_id = db->Get(i, "action_category_id");
			tmpObj.feed_actionId = db->Get(i, "feed_actionId");

			dbResult.push_back(tmpObj);
		}


		for(auto &it: dbResult)
		{
			string	  userNotificationEnrichment = "";

			if(ostUserNotifications.str().length() > 20) ostUserNotifications << ",";
			ostUserNotifications << "{";

			// --- common part
			ostUserNotifications << "\"notificationID\":\"" << it.notificationID << "\",";
			ostUserNotifications << "\"notificationTypeID\":\"" << it.feed_actionTypeId << "\",";
			ostUserNotifications << "\"notificationTypeTitle\":\"" << it.action_types_title << "\",";
			ostUserNotifications << "\"notificationTypeTitleMale\":\"" << it.action_types_title_male << "\",";
			ostUserNotifications << "\"notificationTypeTitleFemale\":\"" << it.action_types_title_female << "\",";
			ostUserNotifications << "\"notificationCategoryID\":\"" << it.action_category_id << "\",";
			ostUserNotifications << "\"notificationCategoryTitle\":\"" << it.action_category_title << "\",";
			ostUserNotifications << "\"notificationCategoryTitleMale\":\"" << it.action_category_title_male << "\",";
			ostUserNotifications << "\"notificationCategoryTitleFemale\":\"" << it.action_category_title_female << "\",";
			ostUserNotifications << "\"notificationEventTimestamp\":\"" << it.feed_eventTimestamp << "\",";
			ostUserNotifications << "\"notificationOwnerUserID\":\"" << it.user_id << "\",";
			ostUserNotifications << "\"notificationOwnerUserName\":\"" << it.user_name << "\",";
			ostUserNotifications << "\"notificationOwnerUserNameLast\":\"" << it.user_nameLast << "\",";
			ostUserNotifications << "\"notificationOwnerUserSex\":\"" << it.user_sex << "\",";
			ostUserNotifications << "\"notificationActionID\":\"" << it.feed_actionId << "\",";
			ostUserNotifications << "\"notificationTitle\":\"" << it.notification_title << "\",";
			ostUserNotifications << "\"notificationStatus\":\"" << it.notificationStatus << "\"";

			userNotificationEnrichment = GetUserNotificationSpecificDataByType(atol(it.feed_actionTypeId.c_str()), atol(it.feed_actionId.c_str()), db, user);
			if(userNotificationEnrichment.length()) ostUserNotifications << "," << userNotificationEnrichment;

			ostUserNotifications << "}";
		}
	}
	ostUserNotifications << "]";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: finish");
	}

	return ostUserNotifications.str();
}

// --- function removes message image from FileSystems and cleanup DB
// --- as input require SWL WHERE clause (because of using SELECT and DELETE statements)
// --- input params:
// --- 1) SQL WHERE statement
// --- 2) db reference
void	RemoveMessageImages(string sqlWhereStatement, CMysql *db)
{
	int			 affected;
	ostringstream   ost;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: start (sqlWhereStatement: " + sqlWhereStatement + ")");
	}

	ost.str("");
	ost << "select * from `feed_images` where " << sqlWhereStatement;
	affected = db->Query(ost.str());
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			string  filename = "";
			string  mediaType = db->Get(i, "mediaType");

			if(mediaType == "image" || mediaType == "video")
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: file must be deleted [" + filename + "]");
				}

				if(mediaType == "image") filename = IMAGE_FEED_DIRECTORY;
				if(mediaType == "video") filename = VIDEO_FEED_DIRECTORY;

				filename +=  "/";
				filename +=  db->Get(i, "folder");
				filename +=  "/";
				filename +=  db->Get(i, "filename");


				if(isFileExists(filename))
				{
					unlink(filename.c_str());
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) +  "]:ERROR: file is not exists  [filename=" + filename + "]");
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: mediaType[" + mediaType + "] doesn't have local file");
				}

			}

		}
		// --- cleanup DB with images pre-populated for posted message
		ost.str("");
		ost << "delete from `feed_images` where " << sqlWhereStatement;
		db->Query(ost.str());
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: finish");
	}
}

// --- function removes message image from FileSystems and cleanup DB
// --- as input require SWL WHERE clause (because of using SELECT and DELETE statements)
// --- input params:
// --- 1) SQL WHERE statement
// --- 2) db reference
void	RemoveBookCover(string sqlWhereStatement, CMysql *db)
{
	int			 affected;
	ostringstream   ost;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: start (sqlWhereStatement: " + sqlWhereStatement + ")");
	}

	ost.str("");
	ost << "select * from `book` where " << sqlWhereStatement;
	affected = db->Query(ost.str());
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			string  filename = "";
			string  mediaType = "image";

			if(mediaType == "image" || mediaType == "video")
			{

				if(mediaType == "image") filename = IMAGE_BOOKS_DIRECTORY;

				filename +=  "/";
				filename +=  db->Get(i, "coverPhotoFolder");
				filename +=  "/";
				filename +=  db->Get(i, "coverPhotoFilename");

				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: file must be deleted [" + filename + "]");
				}

				if(isFileExists(filename))
				{
					unlink(filename.c_str());
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) +  "]:ERROR: file doesn't exists  [filename=" + filename + "]");
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: mediaType[" + mediaType + "] doesn't have local file");
				}

			}

		}
		// --- cleanup DB with images pre-populated for posted message
		ost.str("");
		ost << "UPDATE `book` SET `coverPhotoFolder`=\"\",`coverPhotoFilename`=\"\" where " << sqlWhereStatement;
		db->Query(ost.str());
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: finish");
	}
}

// --- function removes specified image from FileSystems and cleanup DB
// --- input params:
// --- 1) id
// --- 2) type
// --- 3) db reference
bool	RemoveSpecifiedCover(string itemID, string itemType, CMysql *db)
{
	int			 affected;
	bool			result = true;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: start (itemID = " + itemID + ", itemType = " + itemType + ")");
	}

	affected = db->Query(GetSpecificData_SelectQueryItemByID(itemID, itemType));
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			string  filename = "";
			string  mediaType = "image";

			if(mediaType == "image" || mediaType == "video")
			{

				if(mediaType == "image") filename = GetSpecificData_GetBaseDirectory(itemType);

				filename +=  "/";
				filename +=  db->Get(i, GetSpecificData_GetDBCoverPhotoFolderString(itemType).c_str());
				filename +=  "/";
				filename +=  db->Get(i, GetSpecificData_GetDBCoverPhotoFilenameString(itemType).c_str());

				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: file must be deleted [" + filename + "]");
				}

				if(isFileExists(filename))
				{
					unlink(filename.c_str());
				}
				else
				{
					result = false;
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) +  "]:ERROR: file doesn't exists  [filename=" + filename + "]");
					}
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: mediaType[" + mediaType + "] doesn't have local file");
				}

			}

		}
		// --- cleanup DB with images pre-populated for posted message
		db->Query(GetSpecificData_UpdateQueryItemByID(itemID, itemType, "", ""));
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: finish (result = " + (result ? "true" : "false") + ")");
	}

	return result;
}


bool is_base64(BYTE c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64_encode(BYTE const* buf, unsigned int bufLen)
{
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--) {
	char_array_3[i++] = *(buf++);
	if (i == 3) {
	  char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
	  char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
	  char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
	  char_array_4[3] = char_array_3[2] & 0x3f;

	  for(i = 0; (i <4) ; i++)
		ret += base64_chars[char_array_4[i]];
	  i = 0;
	}
  }

  if (i)
  {
	for(j = i; j < 3; j++)
	  char_array_3[j] = '\0';

	char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
	char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
	char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
	char_array_4[3] = char_array_3[2] & 0x3f;

	for (j = 0; (j < i + 1); j++)
	  ret += base64_chars[char_array_4[j]];

	while((i++ < 3))
	  ret += '=';
  }

  return ret;
}

string base64_decode(std::string const& encoded_string)
{
  size_t in_len = encoded_string.size();
  size_t i = 0;
  size_t j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
	char_array_4[i++] = encoded_string[in_]; in_++;
	if (i ==4) {
	  for (i = 0; i <4; i++)
		char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

	  char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
	  char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
	  char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

	  for (i = 0; (i < 3); i++)
		ret += char_array_3[i];
	  i = 0;
	}
  }

  if (i) {
	for (j = i; j <4; j++)
	  char_array_4[j] = 0;

	for (j = 0; j <4; j++)
	  char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

	char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
	char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
	char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

	for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

bool RedirStdoutToFile(string fname)
{
	bool		result = true;
	FILE		*fRes;

	fRes = freopen(fname.c_str(), "w", stdout);
	if(!fRes)
	{
		result = false;
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: redirect stderr to " + fname);
		}
	}

	return  result;
}

bool RedirStderrToFile(string fname)
{
	bool		result = true;
	FILE		*fRes;

	fRes = freopen(fname.c_str(), "w", stderr);
	if(!fRes)
	{
		result = false;
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: redirect stderr to " + fname);
		}
	}

	return  result;
}




bool CheckImageFileInTempFolder(string src, string dst, string f_type)
{
	bool		result = false;

	MESSAGE_DEBUG("", "", "start (src: " + src + ", dst: " + dst + ")");

#ifndef IMAGEMAGICK_DISABLE
	// Construct the image object. Seperating image construction from the
	// the read operation ensures that a failure to read the image file
	// doesn't render the image object useless.
	try {
		Magick::Image		   image;
		Magick::OrientationType imageOrientation;
		Magick::Geometry		imageGeometry;

		// Read a file into image object
		image.read( src );

		imageGeometry = image.size();
		imageOrientation = image.orientation();

		MESSAGE_DEBUG("", "", "imageOrientation = " + to_string(imageOrientation) + ", xRes = " + to_string(imageGeometry.width()) + ", yRes = " + to_string(imageGeometry.height()))

		if(imageOrientation == Magick::TopRightOrientation) image.flop();
		if(imageOrientation == Magick::BottomRightOrientation) image.rotate(180);
		if(imageOrientation == Magick::BottomLeftOrientation) { image.flop(); image.rotate(180); }
		if(imageOrientation == Magick::LeftTopOrientation) { image.flop(); image.rotate(-90); }
		if(imageOrientation == Magick::RightTopOrientation) image.rotate(90);
		if(imageOrientation == Magick::RightBottomOrientation) { image.flop(); image.rotate(90); }
		if(imageOrientation == Magick::LeftBottomOrientation) image.rotate(-90);

		if((imageGeometry.width() > GetSpecificData_GetMaxWidth(f_type)) || (imageGeometry.height() > GetSpecificData_GetMaxHeight(f_type)))
		{
			int   newHeight, newWidth;
			if(imageGeometry.width() >= imageGeometry.height())
			{
				newWidth = GetSpecificData_GetMaxWidth(f_type);
				newHeight = newWidth * imageGeometry.height() / imageGeometry.width();
			}
			else
			{
				newHeight = GetSpecificData_GetMaxHeight(f_type);
				newWidth = newHeight * imageGeometry.width() / imageGeometry.height();
			}

			image.resize(Magick::Geometry(newWidth, newHeight, 0, 0));
		}

		// --- strip EXIF info
		image.strip();

		// Write the image to a file
		image.write( dst );
		result = true;
	}
	catch( Magick::Exception &error_ )
	{
		MESSAGE_DEBUG("", "", "ImageMagick read/write trown exception [" + error_.what() + "]");
	}
#else

	MESSAGE_DEBUG("", "", "simple file coping, because ImageMagick++ is not activated");

	CopyFile(src, dst);
	result = true;

#endif

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string	SaveImageFileFromHandler(string f_name, string f_type, CFiles *files)
{
	string		result = "";

	MESSAGE_DEBUG("", "", "start (f_name = " + f_name + ", f_type = " + f_type + ")");
	
	if(f_name.length())
	{
		if(f_type.length())
		{
			string		filePrefix = "";
			string		finalFilename, originalFilename, preFinalFilename, fileName, fileExtention;

			if(files->GetSize(f_name) && (files->GetSize(f_name) <= GetSpecificData_GetMaxFileSize(f_type)))
			{
				int		folderID = 0;
				FILE	*f;

				//--- check logo file existing
				do
				{
					std::size_t  	foundPos;

					folderID = (int)(rand()/(RAND_MAX + 1.0) * GetSpecificData_GetNumberOfFolders(f_type)) + 1;
					filePrefix = GetRandom(20);

					if((foundPos = f_name.rfind(".")) != string::npos) 
						fileExtention = f_name.substr(foundPos, f_name.length() - foundPos);
					else
						fileExtention = ".jpg";

					originalFilename = "/tmp/tmp_" + filePrefix + fileExtention;
					preFinalFilename = "/tmp/" + filePrefix + ".jpg";
					finalFilename = GetSpecificData_GetBaseDirectory(f_type) + "/" + to_string(folderID) + "/" + filePrefix + ".jpg";
				} while(isFileExists(finalFilename) || isFileExists(originalFilename) || isFileExists(preFinalFilename));

				MESSAGE_DEBUG("", "", "Save file to /tmp for checking of image validity [" + originalFilename + "]");

				// --- Save file to "/tmp/" for checking of image validity
				f = fopen(originalFilename.c_str(), "w");
				if(f)
				{

					fwrite(files->Get(f_name), files->GetSize(f_name), 1, f);
					fclose(f);

					// --- file written to temporary directory to original f_name
					if(CheckImageFileInTempFolder(originalFilename, preFinalFilename, f_type))
					{
						unlink(originalFilename.c_str());

						CopyFile(preFinalFilename, finalFilename);

						unlink(preFinalFilename.c_str());

						result = to_string(folderID) + "/" + filePrefix + ".jpg";
					}
					else
					{
						MESSAGE_ERROR("", "", "file(" + originalFilename + ") is not an image");
						unlink(originalFilename.c_str());
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "fail writing to temp file " + originalFilename);
				}
			}
			else
			{
				MESSAGE_ERROR("", "", f_name + " size(" + to_string(GetSpecificData_GetMaxFileSize(f_type)) + ") is beyond the limit (1, " + to_string(GetSpecificData_GetMaxFileSize(f_type)) + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "f_name is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "f_name is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

int GetSpecificData_GetNumberOfFolders(string itemType)
{
	int	  result = 0;

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = CERTIFICATIONSLOGO_NUMBER_OF_FOLDERS;
	else if(itemType == "course")					result = CERTIFICATIONSLOGO_NUMBER_OF_FOLDERS;
	else if(itemType == "university")				result = UNIVERSITYLOGO_NUMBER_OF_FOLDERS;
	else if(itemType == "school")					result = SCHOOLLOGO_NUMBER_OF_FOLDERS;
	else if(itemType == "language")					result = FLAG_NUMBER_OF_FOLDERS;
	else if(itemType == "book")						result = BOOKCOVER_NUMBER_OF_FOLDERS;
	else if(itemType == "company")					result = COMPANYLOGO_NUMBER_OF_FOLDERS;
	else if(itemType == "company_profile_logo")		result = COMPANYLOGO_NUMBER_OF_FOLDERS;
	else if(itemType == "gift")						result = GIFTIMAGE_NUMBER_OF_FOLDERS;
	else if(itemType == "event")					result = EVENTIMAGE_NUMBER_OF_FOLDERS;
	else if(itemType == "expense_line")				result = EXPENSELINE_NUMBER_OF_FOLDERS;
	else if(itemType == "template_sow")				result = TEMPLATE_SOW_NUMBER_OF_FOLDERS;
	else if(itemType == "template_psow")			result = TEMPLATE_PSOW_NUMBER_OF_FOLDERS;
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

int GetSpecificData_GetMaxFileSize(string itemType)
{
	int	  result = 0;

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = CERTIFICATIONSLOGO_MAX_FILE_SIZE;
	else if(itemType == "course")					result = CERTIFICATIONSLOGO_MAX_FILE_SIZE;
	else if(itemType == "university")				result = UNIVERSITYLOGO_MAX_FILE_SIZE;
	else if(itemType == "school")					result = SCHOOLLOGO_MAX_FILE_SIZE;
	else if(itemType == "language")					result = FLAG_MAX_FILE_SIZE;
	else if(itemType == "book")						result = BOOKCOVER_MAX_FILE_SIZE;
	else if(itemType == "company")					result = COMPANYLOGO_MAX_FILE_SIZE;
	else if(itemType == "company_profile_logo")		result = COMPANYLOGO_MAX_FILE_SIZE;
	else if(itemType == "gift")						result = GIFTIMAGE_MAX_FILE_SIZE;
	else if(itemType == "event")					result = EVENTIMAGE_MAX_FILE_SIZE;
	else if(itemType == "expense_line")				result = EXPENSELINE_MAX_FILE_SIZE;
	else if(itemType == "template_sow")				result = TEMPLATE_SOW_MAX_FILE_SIZE;
	else if(itemType == "template_psow")			result = TEMPLATE_PSOW_MAX_FILE_SIZE;
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

unsigned int GetSpecificData_GetMaxWidth(string itemType)
{
	int	  result = 0;

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = CERTIFICATIONSLOGO_MAX_WIDTH;
	else if(itemType == "course")					result = CERTIFICATIONSLOGO_MAX_WIDTH;
	else if(itemType == "university")				result = UNIVERSITYLOGO_MAX_WIDTH;
	else if(itemType == "school")					result = SCHOOLLOGO_MAX_WIDTH;
	else if(itemType == "language")					result = FLAG_MAX_WIDTH;
	else if(itemType == "book")						result = BOOKCOVER_MAX_WIDTH;
	else if(itemType == "company")					result = COMPANYLOGO_MAX_WIDTH;
	else if(itemType == "company_profile_logo")		result = COMPANYLOGO_MAX_WIDTH;
	else if(itemType == "gift")						result = GIFTIMAGE_MAX_WIDTH;
	else if(itemType == "event")					result = EVENTIMAGE_MAX_WIDTH;
	else if(itemType == "expense_line")				result = EXPENSELINE_IMAGE_MAX_WIDTH;
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

unsigned int GetSpecificData_GetMaxHeight(string itemType)
{
	int	  result = 0;

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = CERTIFICATIONSLOGO_MAX_HEIGHT;
	else if(itemType == "course")					result = CERTIFICATIONSLOGO_MAX_HEIGHT;
	else if(itemType == "university")				result = UNIVERSITYLOGO_MAX_HEIGHT;
	else if(itemType == "school")					result = SCHOOLLOGO_MAX_HEIGHT;
	else if(itemType == "language")					result = FLAG_MAX_HEIGHT;
	else if(itemType == "book")						result = BOOKCOVER_MAX_HEIGHT;
	else if(itemType == "company")					result = COMPANYLOGO_MAX_HEIGHT;
	else if(itemType == "company_profile_logo")		result = COMPANYLOGO_MAX_HEIGHT;
	else if(itemType == "gift")	  					result = GIFTIMAGE_MAX_HEIGHT;
	else if(itemType == "event")	  				result = EVENTIMAGE_MAX_HEIGHT;
	else if(itemType == "expense_line")				result = EXPENSELINE_IMAGE_MAX_HEIGHT;
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string GetSpecificData_GetBaseDirectory(string itemType)
{
	string	  result = "";

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = IMAGE_CERTIFICATIONS_DIRECTORY;
	else if(itemType == "course")					result = IMAGE_CERTIFICATIONS_DIRECTORY;
	else if(itemType == "university")				result = IMAGE_UNIVERSITIES_DIRECTORY;
	else if(itemType == "school")					result = IMAGE_SCHOOLS_DIRECTORY;
	else if(itemType == "language")					result = IMAGE_FLAGS_DIRECTORY;
	else if(itemType == "book")						result = IMAGE_BOOKS_DIRECTORY;
	else if(itemType == "company")					result = IMAGE_COMPANIES_DIRECTORY;
	else if(itemType == "company_profile_logo")		result = IMAGE_COMPANIES_DIRECTORY;
	else if(itemType == "gift")						result = IMAGE_GIFTS_DIRECTORY;
	else if(itemType == "event")					result = IMAGE_EVENTS_DIRECTORY;
	else if(itemType == "expense_line")				result = IMAGE_EXPENSELINES_DIRECTORY;
	else if(itemType == "template_sow")				result = TEMPLATE_SOW_DIRECTORY;
	else if(itemType == "template_psow")			result = TEMPLATE_PSOW_DIRECTORY;
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetSpecificData_GetFinalFileExtenstion(string itemType)
{
	string	  result = ".jpg";

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "template_sow")				result = ".txt";
	else if(itemType == "template_psow")		result = ".txt";
	else
	{
		MESSAGE_ERROR("", "", "default extension taken");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetSpecificData_SelectQueryItemByID(string itemID, string itemType)
{
	string	  result = "";

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = "SELECT * FROM `certification_tracks` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "course")					result = "SELECT * FROM `certification_tracks` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "university")				result = "SELECT * FROM `university` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "school")					result = "SELECT * FROM `school` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "language")					result = "SELECT * FROM `language` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "book")						result = "SELECT * FROM `book` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "company")					result = "SELECT * FROM `company` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "company_profile_logo")		result = "SELECT * FROM `company` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "gift")						result = "SELECT * FROM `gifts` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "event")					result = "SELECT * FROM `events` WHERE `id`=\"" + itemID + "\";";
	else if(itemType == "template_sow")				result = "SELECT * FROM `contract_sow_custom_fields` WHERE `id`=\"" + itemID + "\" AND `type`=\"file\";";
	else if(itemType == "template_psow")			result = "SELECT * FROM `contract_psow_custom_fields` WHERE `id`=\"" + itemID + "\" AND `type`=\"file\";";
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetSpecificData_UpdateQueryItemByID(string itemID, string itemType, string folderID, string fileName)
{
	string		result = "";
	string		logo_folder = "";
	string		logo_filename = "";

	MESSAGE_DEBUG("", "", "start");

	logo_folder = GetSpecificData_GetDBCoverPhotoFolderString(itemType);
	logo_filename = GetSpecificData_GetDBCoverPhotoFilenameString(itemType);

	if(logo_filename.length())
	{
		if(itemType == "certification")					result = "update `certification_tracks` 		set	`" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "course")					result = "update `certification_tracks` 		set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "university")				result = "update `university`					set	`" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "school")					result = "update `school`						set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "language")					result = "update `language`						set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "book")						result = "update `book`							set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "company")					result = "update `company`						set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "company_profile_logo")		result = "update `company`						set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "gift")						result = "update `gifts`						set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "event")					result = "update `events`						set `" + logo_folder + "`='" + folderID + "', `" + logo_filename + "`='" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "template_sow")				result = "update `contract_sow_custom_fields`	set `" + logo_filename + "`='" + folderID + "/" + fileName + "' where `id`=\"" + itemID + "\";";
		else if(itemType == "template_psow")			result = "update `contract_psow_custom_fields`	set `" + logo_filename + "`='" + folderID + "/" + fileName + "' where `id`=\"" + itemID + "\";";
		else
		{
			MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "logo_filename not found for itemType (" + itemType + ")");
	}


	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetSpecificData_GetDBCoverPhotoFolderString(string itemType)
{
	string	  result = "";

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")	 				result = "logo_folder";
	else if(itemType == "course")	   				result = "logo_folder";
	else if(itemType == "university")   			result = "logo_folder";
	else if(itemType == "school")	   				result = "logo_folder";
	else if(itemType == "language")	 				result = "logo_folder";
	else if(itemType == "book")		 				result = "coverPhotoFolder";
	else if(itemType == "company")					result = "logo_folder";
	else if(itemType == "company_profile_logo")		result = "logo_folder";
	else if(itemType == "gift")	  					result = "logo_folder";
	else if(itemType == "event")	  				result = "logo_folder";
	else if(itemType == "template_sow")				result = "";
	else if(itemType == "template_psow")			result = "";
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetSpecificData_GetDBCoverPhotoFilenameString(string itemType)
{
	string	  result = "";

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "certification")					result = "logo_filename";
	else if(itemType == "course")					result = "logo_filename";
	else if(itemType == "university")				result = "logo_filename";
	else if(itemType == "school")					result = "logo_filename";
	else if(itemType == "language")					result = "logo_filename";
	else if(itemType == "book")						result = "coverPhotoFilename";
	else if(itemType == "company")					result = "logo_filename";
	else if(itemType == "company_profile_logo")		result = "logo_filename";
	else if(itemType == "gift")						result = "logo_filename";
	else if(itemType == "event")					result = "logo_filename";
	else if(itemType == "template_sow")				result = "value";
	else if(itemType == "template_psow")			result = "value";
	else
	{
		MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetSpecificData_GetDataTypeByItemType(const string &itemType)
{
	auto	result = "image"s;

	MESSAGE_DEBUG("", "", "start");

	if(itemType == "template_sow") result = "template";
	if(itemType == "template_psow") result = "template";

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}


// --- Does the owner user allowed to change it ?
// --- For example:
// ---	*) university or school logo can be changed by administartor only.
// ---	*) gift image could be changed by owner
bool GetSpecificData_AllowedToChange(string itemID, string itemType, CMysql *db, CUser *user)
{
	bool	  result = false;

	MESSAGE_DEBUG("", "", "start");


	if(itemType == "template_sow")
	{
		if((user->GetType() == "agency"))
		{
			if(db->Query("SELECT `id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_sow`=\"Y\" AND `company_id`=("
							"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
								"SELECT `contract_sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + itemID + "\""
							")"
						");"))
				result = true;
			else
			{
				MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") doesn't allowed to change contract_sow_custom_fields.id(" + itemID + ")");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "user.type(" + user->GetType() + ") must be agency employee to change");
		}
		
	}
	else if(itemType == "template_psow")
	{
		if((user->GetType() == "agency"))
		{
			if(db->Query("SELECT `id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_sow`=\"Y\" AND `company_id`=("
							"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
								"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id`=("
									"SELECT `contract_psow_id` FROM `contract_psow_custom_fields` WHERE `id`=\"" + itemID + "\""
								")"
							")"
						");"))
				result = true;
			else
			{
				MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") doesn't allowed to change contract_psow_custom_fields.id(" + itemID + ")");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "user.type(" + user->GetType() + ") must be agency employee to change");
		}
		
	}
	else if(itemType == "company_profile_logo")
	{
		if((user->GetType() == "subcontractor"))
		{
			if(db->Query("SELECT `id` FROM `company` WHERE `id`=\"" + itemID + "\" AND `admin_userID`=\"" + user->GetID() + "\";"))
				result = true;
			else
			{
				MESSAGE_DEBUG("", "", "subcontractor user.id(" + user->GetID() + ") doesn't allowed to change company.id(" + itemID + ") logo");
			}
		}
		else if((user->GetType() == "agency"))
		{
			if(db->Query("SELECT `id` FROM `company_employees` WHERE `company_id`=\"" + itemID + "\" AND `user_id`=\"" + user->GetID() + "\" AND `allowed_change_agency_data`=\"Y\";"))
				result = true;
			else
			{
				MESSAGE_DEBUG("", "", "agency user.id(" + user->GetID() + ") doesn't allowed to change company.id(" + itemID + ") logo");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "user.type(" + user->GetType() + ") not allowed to change");
		}

	}
	else if(db->Query(GetSpecificData_SelectQueryItemByID(itemID, itemType))) // --- item itemID exists ?
	{
		if((itemType == "course") || (itemType == "university") || (itemType == "school") || (itemType == "language") || (itemType == "book") || (itemType == "company") || (itemType == "certification"))
		{
			string	  coverPhotoFolder = db->Get(0, GetSpecificData_GetDBCoverPhotoFolderString(itemType).c_str());
			string	  coverPhotoFilename = db->Get(0, GetSpecificData_GetDBCoverPhotoFilenameString(itemType).c_str());

			if(coverPhotoFolder.empty() && coverPhotoFilename.empty())
				result = true;
			else
			{
				result = false;
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: access to " + itemType + "(" + itemID + ") denied, because logo already uploaded");
				}
			}
		}
		else if(itemType == "event")
		{
			if(user)
			{
				if(db->Query("SELECT `id` FROM `event_hosts` WHERE `event_id`=\"" + itemID + "\" AND `user_id`=\"" + user->GetID() + "\";"))
					result = true;
				else
				{
					result = false;
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: access to " + itemType + "(" + itemID + ") denied, you are not the event host");
					}
				}
			}
			else
			{
				result = false;

				MESSAGE_ERROR("", "", "user object is NULL")
			}
		}
		else if(itemType == "gift")
		{
			string		user_id = db->Get(0, "user_id");

			if(user)
			{
				if(user_id == user->GetID())
					result = true;
				else
				{
					result = false;
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: access to " + itemType + "(" + itemID + ") denied, you are not the gift owner");
					}
				}
			}
			else
			{
				result = false;

				MESSAGE_ERROR("", "", "user object is NULL")
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "itemType (" + itemType + ") is unknown");
		}
	}
	else
	{
		result = false;
		
		MESSAGE_ERROR("", "", itemType + "(" + itemID + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result: " + (result ? "true" : "false") + ")");

	return result;
}

pair<struct tm, struct tm> GetFirstAndLastDateOfThisMonth()
{
	time_t rawtime;
	struct tm * timeinfo;

	struct tm end_of_curr_mon, start_of_curr_mon;

	MESSAGE_DEBUG("", "", "start");

	start_of_curr_mon.tm_sec	= 0;   // seconds of minutes from 0 to 61
	start_of_curr_mon.tm_min	= 0;   // minutes of hour from 0 to 59
	start_of_curr_mon.tm_hour	= 0;  // hours of day from 0 to 24
	start_of_curr_mon.tm_mday	= 0;  // day of month from 1 to 31
	start_of_curr_mon.tm_mon	= 0;   // month of year from 0 to 11
	start_of_curr_mon.tm_year	= 0;  // year since 1900
	start_of_curr_mon.tm_wday	= 0;  // days since sunday
	start_of_curr_mon.tm_yday	= 0;  // days since January 1st
	start_of_curr_mon.tm_isdst	= 0; // hours of daylight savings time

	end_of_curr_mon.tm_sec	= 0;   // seconds of minutes from 0 to 61
	end_of_curr_mon.tm_min	= 0;   // minutes of hour from 0 to 59
	end_of_curr_mon.tm_hour	= 0;  // hours of day from 0 to 24
	end_of_curr_mon.tm_mday	= 0;  // day of month from 1 to 31
	end_of_curr_mon.tm_mon	= 0;   // month of year from 0 to 11
	end_of_curr_mon.tm_year	= 0;  // year since 1900
	end_of_curr_mon.tm_wday	= 0;  // days since sunday
	end_of_curr_mon.tm_yday	= 0;  // days since January 1st
	end_of_curr_mon.tm_isdst	= 0; // hours of daylight savings time

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	start_of_curr_mon.tm_year	= timeinfo->tm_year;
	start_of_curr_mon.tm_mon	= timeinfo->tm_mon;
	start_of_curr_mon.tm_mday	= 1;

	end_of_curr_mon.tm_year		= start_of_curr_mon.tm_year;
	end_of_curr_mon.tm_mon		= start_of_curr_mon.tm_mon + 1;
	end_of_curr_mon.tm_mday		= 0;

	mktime(&start_of_curr_mon);
	mktime(&end_of_curr_mon);

	MESSAGE_DEBUG("", "", "finish");

	return make_pair(start_of_curr_mon, end_of_curr_mon);
}

pair<struct tm, struct tm> GetFirstAndLastDateOfLastMonth()
{
	time_t rawtime;
	struct tm * timeinfo;

	struct tm end_of_last_mon, start_of_last_mon;

	MESSAGE_DEBUG("", "", "start");

	start_of_last_mon.tm_sec	= 0;   // seconds of minutes from 0 to 61
	start_of_last_mon.tm_min	= 0;   // minutes of hour from 0 to 59
	start_of_last_mon.tm_hour	= 0;  // hours of day from 0 to 24
	start_of_last_mon.tm_mday	= 0;  // day of month from 1 to 31
	start_of_last_mon.tm_mon	= 0;   // month of year from 0 to 11
	start_of_last_mon.tm_year	= 0;  // year since 1900
	start_of_last_mon.tm_wday	= 0;  // days since sunday
	start_of_last_mon.tm_yday	= 0;  // days since January 1st
	start_of_last_mon.tm_isdst	= 0; // hours of daylight savings time

	end_of_last_mon.tm_sec	= 0;   // seconds of minutes from 0 to 61
	end_of_last_mon.tm_min	= 0;   // minutes of hour from 0 to 59
	end_of_last_mon.tm_hour	= 0;  // hours of day from 0 to 24
	end_of_last_mon.tm_mday	= 0;  // day of month from 1 to 31
	end_of_last_mon.tm_mon	= 0;   // month of year from 0 to 11
	end_of_last_mon.tm_year	= 0;  // year since 1900
	end_of_last_mon.tm_wday	= 0;  // days since sunday
	end_of_last_mon.tm_yday	= 0;  // days since January 1st
	end_of_last_mon.tm_isdst	= 0; // hours of daylight savings time


	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	start_of_last_mon.tm_year	= timeinfo->tm_year;
	start_of_last_mon.tm_mon	= timeinfo->tm_mon - 1;
	start_of_last_mon.tm_mday	= 1;

	end_of_last_mon.tm_year		= start_of_last_mon.tm_year;
	end_of_last_mon.tm_mon		= start_of_last_mon.tm_mon + 1;
	end_of_last_mon.tm_mday		= 0;

	mktime(&start_of_last_mon);
	mktime(&end_of_last_mon);

	MESSAGE_DEBUG("", "", "finish");

	return make_pair(start_of_last_mon, end_of_last_mon);
}

struct tm GetTMObject(string date)
{
	struct tm	result;
	smatch		sm;

	MESSAGE_DEBUG("", "", "start");
	
	result.tm_sec	= 0;   // seconds of minutes from 0 to 61
	result.tm_min	= 0;   // minutes of hour from 0 to 59
	result.tm_hour	= 0;  // hours of day from 0 to 24
	result.tm_mday	= 1;  // day of month from 1 to 31
	result.tm_mon	= 1;   // month of year from 0 to 11
	result.tm_year	= 70;  // year since 1900
	result.tm_wday	= 0;  // days since sunday
	result.tm_yday	= 0;  // days since January 1st
	result.tm_isdst	= 0; // hours of daylight savings time

	// --- normalize start, end format
	if(regex_match(date, sm, regex("^([[:digit:]]{1,2})\\/([[:digit:]]{1,2})\\/([[:digit:]]{2,4})$")))
	{
		result.tm_mday = stoi(sm[1]);
		result.tm_mon = stoi(sm[2]) - 1;
		if(stoi(sm[3]) < 100)
			result.tm_year = 2000 + stoi(sm[3]) - 1900;
		else
			result.tm_year = stoi(sm[3]) - 1900;
	}
	else if(regex_match(date, sm, regex("^([[:digit:]]{2,4})\\-([[:digit:]]{1,2})\\-([[:digit:]]{1,2})$")))
	{
		result.tm_mday = stoi(sm[3]);
		result.tm_mon = stoi(sm[2]) - 1;
		if(stoi(sm[1]) < 100)
			result.tm_year = 2000 + stoi(sm[1]) - 1900;
		else
			result.tm_year = stoi(sm[1]) - 1900;
	}
	else
	{
		MESSAGE_ERROR("", "", "incorrect date(" + date + ") format");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

bool operator <(const struct tm &param_1, const struct tm &param_2)
{
	bool		result = false;
	struct tm	tm_1 = param_1;
	struct tm	tm_2 = param_2;

	MESSAGE_DEBUG("", "", "start");

	if(difftime(mktime(&tm_2), mktime(&tm_1)) > 0) result = true;

	MESSAGE_DEBUG("", "", "finish (result = " + (result ? "true" : "false") + ")");

	return result;
}

bool operator <=(const struct tm &param_1, const struct tm &param_2)
{
	bool		result = false;
	struct tm	tm_1 = param_1;
	struct tm	tm_2 = param_2;

	MESSAGE_DEBUG("", "", "start");

	if(difftime(mktime(&tm_2), mktime(&tm_1)) >= 0) result = true;

	MESSAGE_DEBUG("", "", "finish (result = " + (result ? "true" : "false") + ")");

	return result;
}

bool operator >(const struct tm &param_1, const struct tm &param_2)
{
	bool		result = false;
	struct tm	tm_1 = param_1;
	struct tm	tm_2 = param_2;

	MESSAGE_DEBUG("", "", "start");

	if(difftime(mktime(&tm_2), mktime(&tm_1)) < 0) result = true;

	MESSAGE_DEBUG("", "", "finish (result = " + (result ? "true" : "false") + ")");

	return result;
}

bool operator >=(const struct tm &param_1, const struct tm &param_2)
{
	bool		result = false;
	struct tm	tm_1 = param_1;
	struct tm	tm_2 = param_2;

	MESSAGE_DEBUG("", "", "start");

	if(difftime(mktime(&tm_2), mktime(&tm_1)) <= 0) result = true;

	MESSAGE_DEBUG("", "", "finish (result = " + (result ? "true" : "false") + ")");

	return result;
}

bool operator ==(const struct tm &param_1, const struct tm &param_2)
{
	bool		result = false;
	struct tm	tm_1 = param_1;
	struct tm	tm_2 = param_2;

	MESSAGE_DEBUG("", "", "start");

	if(difftime(mktime(&tm_2), mktime(&tm_1)) == 0) result = true;

	MESSAGE_DEBUG("", "", "finish (result = " + (result ? "true" : "false") + ")");

	return result;
}

string PrintDate(const struct tm &_tm)
{
	string	result = "";

	result = to_string(_tm.tm_mday) + "/" + to_string(_tm.tm_mon + 1) + "/" + to_string(_tm.tm_year + 1900);

	return result;
}

string PrintSQLDate(const struct tm &_tm)
{
	string	result = "";

	result = to_string(_tm.tm_year + 1900) + "-" + to_string(_tm.tm_mon + 1) + "-" + to_string(_tm.tm_mday);

	return result;
}

string PrintDateTime(const struct tm &_tm)
{
	return PrintTime(_tm, "%T");
}

string PrintTime(const struct tm &_tm, string format)
{
	char		buffer[256];
	string		result = "";
	struct tm	temp_tm = _tm;

	mktime(&temp_tm);
	memset(buffer, 0, sizeof(buffer));
	strftime(buffer, 255, format.c_str(), &temp_tm);
	result = buffer;

	return result;
}