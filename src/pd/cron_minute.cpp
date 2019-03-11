#include "cron_minute.h"

string	CleanUPData(CMysql *db)
{
	string		error_message = "";

	MESSAGE_DEBUG("", "", "start");
	
	db->Query("DELETE FROM `captcha` WHERE `timestamp`<=(NOW()-INTERVAL " + to_string(SESSION_LEN) + " MINUTE)");

	MESSAGE_DEBUG("", "", "finish (error_message.length = " + to_string(error_message.length()) + ")");

	return error_message;
}

int main()
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
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
		string	error_message = "";

		if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: Can not connect to mysql database");
			throw CExceptionHTML("MySql connection");
		}

		//--- start of minute cron main functionality

		error_message = CleanUPData(&db);
		if(error_message.length())
		{
			MESSAGE_ERROR("", "", error_message);
		}

		//--- end of minute cron main functionality
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

