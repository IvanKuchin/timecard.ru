#include "noauth.h"

static auto RemovePhoneConfirmationCodes(string sessid, CMysql *db)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	db->Query("DELETE FROM `phone_confirmation` WHERE `session`=\"" + sessid + "\";");

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}


int main()
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	MESSAGE_DEBUG("", "", __FILE__);

	signal(SIGSEGV, crash_handler); 

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	try
	{

	indexPage.ParseURL();

	if(!indexPage.SetTemplate("index.htmlt"))
	{
		CLog	log;

		log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: template file was missing");
		throw CException("Template file was missing");
	}

	if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
	{
		CLog	log;

		log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: Can not connect to mysql database");
		throw CExceptionHTML("MySql connection");
	}

	indexPage.SetDB(&db);


	action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));

	MESSAGE_DEBUG("", "", "action = " + action);

	// --- generate common parts
	{
		// --- it has to be run before session, because session relay upon Apache environment variable
		if(RegisterInitialVariables(&indexPage, &db, &user))
		{
		}
		else
		{
			MESSAGE_ERROR("", "", "RegisterInitialVariables failed, throwing exception");
			throw CExceptionHTML("environment variable error");
		}

		//------- Generate session
		action = GenerateSession(action, &indexPage, &db, &user);
	}
// ------------ end generate common parts

	MESSAGE_DEBUG("", "", "pre-condition action = " + action);

	if((action.length() > 10) && (action.compare(action.length() - 9, 9, "_template") == 0))
	{
		ostringstream	ost;
		string			strPageToGet, strFriendsOnSinglePage;

		MESSAGE_DEBUG("", action, "start");

		{
			string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_getGeoCountryList")
	{
		MESSAGE_DEBUG("", action, "start");

		auto			success_message = "\"countries\":[" + GetGeoCountryListInJSONFormat("SELECT * FROM `geo_country`;", &db, &user) + "]";

		AJAX_ResponseTemplate(&indexPage, success_message, "");

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_sendPhoneConfirmationSMS")
	{
		auto			error_message = ""s;

		{
			auto			country_code = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("country_code"));
			auto			phone_number = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("phone_number"));
			auto			confirmation_code = GetRandom(4);
			c_smsc			smsc(&db);

			MESSAGE_DEBUG("", action, "start");

			if(country_code.length() && phone_number.length())
			{
				auto	phone_confirmation_id = db.InsertQuery("INSERT INTO `phone_confirmation` (`session`, `confirmation_code`, `country_code`, `phone_number`, `eventTimestamp`)"
								" VALUES ("
								"\"" + indexPage.SessID_Get_FromHTTP() + "\","
								"\"" + confirmation_code + "\","
								"\"" + country_code + "\","
								"\"" + phone_number + "\","
								"UNIX_TIMESTAMP()"
								")"
								);

				if(phone_confirmation_id)
				{
					auto	ret = smsc.send_sms(country_code + phone_number, "Код для привязки телефона " + confirmation_code, 0, "", 0, 0, DOMAIN_NAME, "", "");
				}
				else
				{
					MESSAGE_ERROR("", action, "fail to insert to db");
					error_message = gettext("SQL syntax error");
				}
			}
			else
			{
				error_message = gettext("phone number is incorrect");
				MESSAGE_ERROR("", action, "country_code(" + country_code + ") or phone_number(" + phone_number + ") is empty");
			}
		}

		AJAX_ResponseTemplate(&indexPage, "", error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_checkPhoneConfirmationCode")
	{
		auto			template_file = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			extra_fields = ""s;
		ostringstream   ostResult;

		{
			auto			confirmation_code = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("confirmation_code"));

			MESSAGE_DEBUG("", action, "start");

			if(db.Query("SELECT * FROM `phone_confirmation` WHERE "
							"`confirmation_code`=\"" + confirmation_code + "\" AND "
							"`session`=\"" + indexPage.SessID_Get_FromHTTP() + "\" AND "
							"`attempt`<=\"3\" AND "
							"`eventTimestamp`>=(UNIX_TIMESTAMP() - " + to_string(SMSC_EXPIRATION) + ")"
							";"))
			{
				auto	country_code = db.Get(0, "country_code");
				auto	phone_number = db.Get(0, "phone_number");

				db.Query("UPDATE `users` SET `country_code`=\"" + country_code + "\", `phone`=\"" + phone_number + "\" , `is_phone_confirmed`=\"Y\" WHERE `id`=\"" + user.GetID() + "\";");
				if(db.isError())
				{
					MESSAGE_ERROR("", action, "fail to update db");
					error_message = gettext("SQL syntax error");
				}
				else
				{
					RemovePhoneConfirmationCodes(indexPage.SessID_Get_FromHTTP(), &db);
				}
			}
			else
			{
				db.Query("UPDATE `phone_confirmation` SET `attempt`=`attempt` + 1 WHERE"
							"`session`=\"" + indexPage.SessID_Get_FromHTTP() + "\" AND "
							"`eventTimestamp`>=(UNIX_TIMESTAMP() - " + to_string(SMSC_EXPIRATION) + ")"
							);

				if(db.Query("SELECT `attempt` FROM `phone_confirmation` WHERE "
								"`session`=\"" + indexPage.SessID_Get_FromHTTP() + "\" AND "
								"`eventTimestamp`>=(UNIX_TIMESTAMP() - " + to_string(SMSC_EXPIRATION) + ")"
								";"))
				{
					auto	attempts = db.Get(0, "attempt");

					if(stoi(attempts) >= 3) RemovePhoneConfirmationCodes(indexPage.SessID_Get_FromHTTP(), &db);
					extra_fields = "\"attempt\":\"" + attempts + "\",";
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to select data");
				}
				error_message = gettext("incorrect confirmation code");
				MESSAGE_ERROR("", action, "incorrect confirmation code");
			}
		}

		ostResult.str("");
		if(error_message.empty())
		{
			ostResult << "{" 
					  << "\"result\":\"success\""
					  << "}";
		}
		else
		{
			MESSAGE_ERROR("", action, "fail to send phone confirmation sms");

			ostResult << "{" 
					  << "\"result\":\"error\","
					  << extra_fields
					  << "\"description\":\"" + error_message + "\""
					  << "}";
		}

		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate(template_file))
		{
			MESSAGE_ERROR("", action, "template file " + template_file + " was missing");
			throw CException("Template file " + template_file + " was missing");
		}

		MESSAGE_DEBUG("", action, "finish");
	}

	MESSAGE_DEBUG("", action, "finish condition")

	indexPage.OutTemplate();

	}
	catch(CExceptionHTML &c)
	{
		CLog	log;

		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);

		log.Write(DEBUG, string(__func__) + ": catch CExceptionHTML: DEBUG exception reason: [", c.GetReason(), "]");

		if(!indexPage.SetTemplate(c.GetTemplate()))
		{
			return(-1);
		}
		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(0);
	}
	catch(CException &c)
	{
		CLog 	log;

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}

		log.Write(ERROR, string(__func__) + ": catch CException: exception: ERROR  ", c.GetReason());

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(-1);
	}
	catch(exception& e)
	{
		CLog 	log;
		log.Write(ERROR, string(__func__) + ": catch(exception& e): catch standard exception: ERROR  ", e.what());

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}
		indexPage.RegisterVariable("content", e.what());
		indexPage.OutTemplate();
		return(-1);
	}

	return(0);
}

