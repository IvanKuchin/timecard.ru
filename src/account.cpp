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
		log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + "]: " + __FILE__);
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

		log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: template file was missing");
		throw CException("Template file was missing");
	}

	if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
	{
		CLog	log;

		log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: Can not connect to mysql database");
		throw CExceptionHTML("MySql connection");
	}

	indexPage.SetDB(&db);

#ifndef MYSQL_3
	db.Query("set names cp1251");
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

// TODO: remove Jan 1 '19
	/*
		ostringstream		query, ost1, ost2;
		string				partNum;
		string				content;
		// Menu				m;

		indexPage.RegisterVariableForce("rand", GetRandom(10));
		indexPage.RegisterVariableForce("random", GetRandom(10));
		indexPage.RegisterVariableForce("style", "style.css");

		// --- Generate session
		{
			string			lng, sessidHTTP;
			ostringstream	ost;


			sessidHTTP = indexPage.SessID_Get_FromHTTP();
			if(sessidHTTP.length() < 5) {
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + ": session cookie is not exist, generating new session.");
				}
				sessidHTTP = indexPage.SessID_Create_HTTP_DB();
				if(sessidHTTP.length() < 5) {
					CLog	log;
					log.Write(ERROR, string(__func__) + ": error in generating session ID");
					throw CExceptionHTML("session can't be created");
				}
			} 
			else {
				if(indexPage.SessID_Load_FromDB(sessidHTTP)) 
				{
					if(indexPage.SessID_CheckConsistency()) 
					{
						if(indexPage.SessID_Update_HTTP_DB()) 
						{
							indexPage.RegisterVariableForce("loginUser", "");
							indexPage.RegisterVariableForce("loginUserID", "");

							if(indexPage.SessID_Get_UserFromDB() != "Guest") {
								user.SetDB(&db);
								// --- place 2change user from user.email to user.id 
								if(user.GetFromDBbyEmail(indexPage.SessID_Get_UserFromDB()))
								{
									ostringstream	ost1;
									string			avatarPath;

									indexPage.RegisterVariableForce("loginUser", indexPage.SessID_Get_UserFromDB());
									indexPage.RegisterVariableForce("loginUserID", user.GetID());
									indexPage.RegisterVariableForce("myFirstName", user.GetName());
									indexPage.RegisterVariableForce("myLastName", user.GetNameLast());

									// --- Get user avatars
									ost1.str("");
									ost1 << "select * from `users_avatars` where `userid`='" << user.GetID() << "' and `isActive`='1';";
									avatarPath = "empty";
									if(db.Query(ost1.str()))
									{
										ost1.str("");
										ost1 << "/images/avatars/avatars" << db.Get(0, "folder") << "/" << db.Get(0, "filename");
										avatarPath = ost1.str();
									}
									indexPage.RegisterVariableForce("myUserAvatar", avatarPath);


									{
										CLog	log;
										ostringstream	ost;

										ost << __func__ << "[" << __LINE__ << "]: user [" << user.GetLogin() << "] logged in";
										log.Write(DEBUG, ost.str());
									}
								}
								else
								{
									// --- enforce to close session
									action = "logout";

									{
										CLog	log;
										ostringstream	ost;

										ost << __func__ << "[" << __LINE__ << "]: ERROR user [" << indexPage.SessID_Get_UserFromDB() << "] session exists on client device, but not in the DB. Change the [action = logout].";
										log.Write(ERROR, ost.str());
									}

								}
							}
						

						}
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + ": ERROR update session in HTTP or DB failed");
						}
					}
					else {
						CLog	log;
						log.Write(ERROR, string(__func__) + ": ERROR session consistency check failed");
					}
				}
				else 
				{
					ostringstream	ost;

					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + ": cookie session and DB session is not equal. Need to recreate session");
					}

					ost.str("");
					ost << "/?rand=" << GetRandom(10);

					if(!indexPage.Cookie_Expire()) {
						CLog	log;
						log.Write(ERROR, string(__func__) + ": Error in session expiration");			
					} // --- if(!indexPage.Cookie_Expire())
					indexPage.Redirect(ost.str());
				} // --- if(indexPage.SessID_Load_FromDB(sessidHTTP)) 
			} // --- if(sessidHTTP.length() < 5)
		}
//------- End generate session


//------- Register html META-tags
		if(db.Query("select `value` from `settings_default` where `setting`=\"keywords_head\"") > 0)
			indexPage.RegisterVariable("keywords_head", db.Get(0, "value"));
		else
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + ": ERROR getting keywords_head from settings_default");
		}

		if(db.Query("select `value` from `settings_default` where `setting`=\"title_head\"") > 0)
			indexPage.RegisterVariable("title_head", db.Get(0, "value"));
		else
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + ": ERROR getting title_head from settings_default");
		}

		if(db.Query("select `value` from `settings_default` where `setting`=\"description_head\"") > 0)
			indexPage.RegisterVariable("description_head", db.Get(0, "value"));
		else
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + ": ERROR getting description_head from settings_default");
		}

		if(db.Query("select `value` from `settings_default` where `setting`=\"NewsOnSinglePage\"") > 0)
			indexPage.RegisterVariable("NewsOnSinglePage", db.Get(0, "value"));
		else
		{
			CLog			log;
			log.Write(ERROR, string(__func__) + ": ERROR getting NewsOnSinglePage from settings_default");

			indexPage.RegisterVariable("NewsOnSinglePage", to_string(NEWS_ON_SINGLE_PAGE));
		}

		if(db.Query("select `value` from `settings_default` where `setting`=\"FriendsOnSinglePage\"") > 0)
			indexPage.RegisterVariable("FriendsOnSinglePage", db.Get(0, "value"));
		else
		{
			CLog			log;
			log.Write(ERROR, string(__func__) + ": ERROR getting FriendsOnSinglePage from settings_default");

			indexPage.RegisterVariable("FriendsOnSinglePage", to_string(FRIENDS_ON_SINGLE_PAGE));
		}

		if(action.empty() and user.GetLogin().length() and (user.GetLogin() != "Guest"))
		{
			action = GetDefaultActionFromUserType(user.GetType(), &db);

			{
				CLog	log;
				log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + string("]:") + ": META-registration: action has been overriden to 'logged-in user' default action [action = ", action, "]");
			}

		}
		else if(!action.length())
		{
			action = GUEST_USER_DEFAULT_ACTION;

			{
				CLog	log;
				log.Write(DEBUG, __func__ + string("[") + to_string(__LINE__) + string("]:") + ": META-registration: action has been overriden to 'guest user' default action [action = ", action, "]");
			} // --- log block
		} // --- default action
//------- End register html META-tags
	*/}
// ------------ end generate common parts

	{
		CLog	log;
		ostringstream	ost;

		ost.str("");
		ost << __func__ << "[" << __LINE__ << "]: start (action's == \"" << action << "\") condition";
		log.Write(DEBUG, ost.str());
	}

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
				log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": re-login required");
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
							log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": Login already used");
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
						log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ":ERROR: login containing symbols \\ or /");
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
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ":ERROR: login too short");
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
				log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": re-login required");
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
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ":ERROR: wrong userSex[" + userSex + "]");
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
				log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": re-login required");
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
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ":ERROR: wrong date format dd/mm/yy userBirthday[" + userBirthday + "]");
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
				log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": re-login required");
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
					log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": city is empty");
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
				log.Write(DEBUG, string(__func__) + string("[") + to_string(__LINE__) + "]:action == " + action + ": re-login required");
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

