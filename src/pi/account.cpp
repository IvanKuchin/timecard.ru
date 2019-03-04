#include "account.h"

int main()
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	{
		CLog	log;
		log.Write(DEBUG, __func__ + "["s + to_string(__LINE__) + "]: " + __FILE__);
	}

	signal(SIGSEGV, crash_handler); 

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	try
	{

	indexPage.ParseURL();
	indexPage.AddCookie("lng", "ru", "", "", "/");

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

#ifndef MYSQL_3
	db.Query("set names utf8;");
#endif

	action = indexPage.GetVarsHandler()->Get("action");
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + ": action = ", action);
	}

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

	if(action == "AJAX_changeLogin")
	{
		ostringstream   ostResult;
		string		    loginFromUser;

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				CLog	log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			loginFromUser = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("login"));

			if(loginFromUser.length() >= 8)
			{

				if(loginFromUser.find_first_of(" \\/%?абвгдеёжзийклмнопрстуфхцчшщьыъэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ") == string::npos)
				{

					if(db.Query("SELECT `id` FROM `users` WHERE `login`=\"" + loginFromUser + "\" AND `id`!=\"" + user.GetID() + "\";"))
					{
						{
							CLog	log;
							log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": Login already used");
						}

						ostResult << "{" 
								  << "\"result\":\"error\","
								  << "\"description\":\"Имя уже занято\""
								  << "}";
					}
					else
					{

						db.Query("UPDATE `users` SET `login`=\"" + loginFromUser + "\" WHERE `id`=\"" + user.GetID() + "\";");
						if(!db.isError())
						{
							ostResult << "{" 
									  << "\"result\":\"success\","
									  << "\"userLogin\":\"" << loginFromUser << "\""
									  << "}";
						}
						else
						{
							{
								CLog	log;
								log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: updating users table [" + db.GetErrorMessage() + "]");
							}

							ostResult << "{" 
									  << "\"result\":\"error\","
									  << "\"description\":\"Внутренняя ошибка БД\""
									  << "}";
						}
					}
				}
				else
				{
					
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: login containing symbols \\ or /");
					}

					ostResult << "{" 
							  << "\"result\":\"error\","
							  << "\"description\":\"Нельзя использовать: русские буквы, пробел или символы /%?\""
							  << "}";
				}
			}
			else
			{
				
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: login too short");
				}

				ostResult << "{" 
						  << "\"result\":\"error\","
						  << "\"description\":\"Должен быть 8 и более символов\""
						  << "}";
			}

		} // --- if(user.GetLogin() == "Guest")


		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: template file json_response.htmlt was missing");
			throw CException("Template file json_response.htmlt was missing");
		}  // if(!indexPage.SetTemplate("AJAX_changeLogin.htmlt"))

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": end");
		}
	}   // if(action == "AJAX_changeLogin")

	if(action == "AJAX_changeUserSex")
	{
		ostringstream   ostResult;
		string		    userSex;

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				CLog	log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			userSex = indexPage.GetVarsHandler()->Get("userSex");

			if((userSex == "male") || (userSex == "female"))
			{

				db.Query("UPDATE `users` SET `sex`=\"" + userSex + "\" WHERE `id`=\"" + user.GetID() + "\";");
				if(!db.isError())
				{
					ostResult << "{" 
							  << "\"result\":\"success\","
							  << "\"userLogin\":\"" << userSex << "\""
							  << "}";
				}
				else
				{
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: updating users table [" + db.GetErrorMessage() + "]");
					}

					ostResult << "{" 
							  << "\"result\":\"error\","
							  << "\"description\":\"Внутренняя ошибка БД\""
							  << "}";
				}

			}
			else
			{
				
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: wrong userSex[" + userSex + "]");
				}

				ostResult << "{" 
						  << "\"result\":\"error\","
						  << "\"description\":\"некорректный параметер\""
						  << "}";
			}

		} // --- if(user.GetLogin() == "Guest")


		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: template file json_response.htmlt was missing");
			throw CException("Template file json_response.htmlt was missing");
		}  // if(!indexPage.SetTemplate("AJAX_changeUserSex.htmlt"))

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": end");
		}
	}   // if(action == "AJAX_changeUserSex")

	if(action == "AJAX_changeUserBirthday")
	{
		ostringstream   ostResult;
		string		    userBirthday;

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				CLog	log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			userBirthday = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			if(std::regex_match(userBirthday, std::regex("^[[:digit:]]{1,2}/[[:digit:]]{1,2}/[[:digit:]]{2,4}$")))
			{

				db.Query("UPDATE `users` SET `birthday`=\"" + userBirthday + "\" WHERE `id`=\"" + user.GetID() + "\";");
				if(!db.isError())
				{
					ostResult << "{" 
							  << "\"result\":\"success\","
							  << "\"birthday\":\"" << userBirthday << "\""
							  << "}";
				}
				else
				{
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: updating users table [" + db.GetErrorMessage() + "]");
					}

					ostResult << "{" 
							  << "\"result\":\"error\","
							  << "\"description\":\"Внутренняя ошибка БД\""
							  << "}";
				}

			}
			else
			{
				
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: wrong date format dd/mm/yy userBirthday[" + userBirthday + "]");
				}

				ostResult << "{" 
						  << "\"result\":\"error\","
						  << "\"description\":\"некорректный формат даты\""
						  << "}";
			}

		} // --- if(user.GetLogin() == "Guest")


		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: template file json_response.htmlt was missing");
			throw CException("Template file json_response.htmlt was missing");
		}  // if(!indexPage.SetTemplate("AJAX_changeUserBirthday.htmlt"))

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": end");
		}
	}   // if(action == "AJAX_changeUserBirthday")

	if(action == "AJAX_updateCity")
	{
		ostringstream   ostResult;
		string		    city;

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				CLog	log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string::size_type 	openBrace;
			string				cityID = "";

			city = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			openBrace = city.find("(");
			if(openBrace != string::npos)
			{
				city = city.substr(0, openBrace);
			}
			trim(city);

			cityID = GetGeoLocalityIDByCityAndRegion("", city, &db);
			if(cityID.empty()) cityID = "0";

			if(cityID.length())
			{

				db.Query("UPDATE `users` SET `geo_locality_id`=\"" + cityID + "\" WHERE `id`=\"" + user.GetID() + "\";");
				if(!db.isError())
				{
					ostResult << "{" 
							  << "\"result\":\"success\","
							  << "\"city\":\"" << city << "\""
							  << "}";
				}
				else
				{
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: updating users table [" + db.GetErrorMessage() + "]");
					}

					ostResult << "{" 
							  << "\"result\":\"error\","
							  << "\"description\":\"Внутренняя ошибка БД\""
							  << "}";
				}

			}
			else
			{
				
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": city is empty");
				}

				ostResult << "{" 
						  << "\"result\":\"error\","
						  << "\"description\":\"город не указан\""
						  << "}";
			}

		} // --- if(user.GetLogin() == "Guest")


		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: template file json_response.htmlt was missing");
			throw CException("Template file json_response.htmlt was missing");
		}  // if(!indexPage.SetTemplate("AJAX_updateCity.htmlt"))

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": end");
		}
	}   // if(action == "AJAX_updateCity")

	if((action == "AJAX_editProfile_setBirthdayPrivate") || (action == "AJAX_editProfile_setBirthdayPublic"))
	{
		ostringstream   ostResult;
		string		    birthdayAccess = (action == "AJAX_editProfile_setBirthdayPrivate" ? "private" : "public");

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				CLog	log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			db.Query("UPDATE `users` SET `birthdayAccess`=\"" + birthdayAccess + "\" WHERE `id`=\"" + user.GetID() + "\";");
			if(!db.isError())
			{
				ostResult << "{" 
						  << "\"result\":\"success\","
						  << "\"birthday\":\"" << birthdayAccess << "\""
						  << "}";
			}
			else
			{
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ":ERROR: updating users table [" + db.GetErrorMessage() + "]");
				}

				ostResult << "{" 
						  << "\"result\":\"error\","
						  << "\"description\":\"Внутренняя ошибка БД\""
						  << "}";
			}
		} // --- if(user.GetLogin() == "Guest")


		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;

			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: template file json_response.htmlt was missing");
			throw CException("Template file json_response.htmlt was missing");
		}  // if(!indexPage.SetTemplate("AJAX_editProfile_setBirthdayPrivate.htmlt"))

		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:action == " + action + ": end");
		}
	}   // if(action == "AJAX_editProfile_setBirthdayPrivate")





	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << __func__ << "[" << __LINE__ << "]: end (action's == \"" << action << "\") condition";
		log.Write(DEBUG, ost.str());
	}

	indexPage.OutTemplate();

	}
/*
	catch(CExceptionRedirect &c) {
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << string(__func__) + ":: catch CRedirectHTML: exception used for redirection";
		log.Write(DEBUG, ost.str());

		c.SetDB(&db);

		if(!indexPage.SetTemplate(c.GetTemplate())) {

			ost.str("");
			ost << string(__func__) + ":: catch CRedirectHTML: ERROR, template redirect.htmlt not found";
			log.Write(ERROR, ost.str());

			throw CException("Template file was missing");
		}

		indexPage.RegisterVariableForce("content", "redirect page");
		indexPage.OutTemplate();

	}
*/
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

