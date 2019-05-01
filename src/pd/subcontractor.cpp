#include "subcontractor.h"

static auto GetCompanyID(CUser *user, CMysql *db)
{
	auto 	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user && db)
	{
		if(user->GetType() == "subcontractor")
		{
				if(db->Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user->GetID() + "\";"))
				{
					result = db->Get(0, "id");
				}
				else
				{
					MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") isn't an subcontractor employee");
				}
		}
		else
		{
			MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") isn't an subcontractor employee");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user doesn't initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}
/*
static string isUserAllowedAccessToCompany(string company_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(company_id.length())
	{
		if(user->GetType() == "subcontractor")
		{
			if(db->Query("SELECT `id` FROM `company` WHERE `id`=\"" + company_id + "\" AND `admin_userID`=\"" + user->GetID() + "\");"))
			{
				// --- everything is good
			}
			else
			{
				MESSAGE_DEBUG("", "", "user(" + user->GetID() + ") have not rights to change company.id(" + company_id + ") data");
				error_message = "Вы не можете менять данные kompanii";
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") must be a subcontractor(" + user->GetType() + ")");
			error_message = "Информация доступна только для subcontractor";
		}
	}
	else
	{
		error_message = "Неизвестный номер kompanii";
		MESSAGE_ERROR("", "", "company_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}
*/
static auto	GetTimecardsSOWTaskAssignement_Reusable_InJSONFormat(string date, CMysql *db, CUser *user)
{
	auto		result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(date.length())
	{
		if(user)
		{
			if(db)
			{
				result =
						"\"timecards\":[" + GetTimecardsInJSONFormat(
								"SELECT * FROM `timecards` WHERE "
									"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\")) "
									"AND "
									"`period_start`<=\"" + date + "\" "
									"AND "
									"`period_end`>=\"" + date + "\""
								";", db, user) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\") "
									"AND "
									"`start_date`<=\"" + date + "\" "
									"AND "
									"`end_date`>=\"" + date + "\""
								";", db, user) + "],"
						"\"task_assignments\":[" + GetTimecardTaskAssignmentInJSONFormat(
								"SELECT * FROM `timecard_task_assignment` WHERE "
									"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
																"`subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\") "
																"AND "
																"`start_date`<=\"" + date + "\" "
																"AND "
																"`end_date`>=\"" + date + "\""
															");", db, user) + "]";
			}
			else
			{
				MESSAGE_ERROR("", "", "user is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "date is empty");
	}



	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + " symbols)");

	return result;
}

int main()
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	{
		MESSAGE_DEBUG("", action, __FILE__);
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
		MESSAGE_DEBUG("", action, "template file was missing");
		throw CException("Template file was missing");
	}

	if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
	{
		MESSAGE_DEBUG("", action, "Can not connect to mysql database");
		throw CExceptionHTML("MySql connection");
	}

	indexPage.SetDB(&db);


	action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));
	{
		MESSAGE_DEBUG("", "", " action = " + action);
	}

	// --- generate common parts
	{
#ifndef IMAGEMAGICK_DISABLE
		Magick::InitializeMagick(NULL);
#endif

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

	{
		MESSAGE_DEBUG("", "", "pre-condition if(action == \"" + action + "\")");
	}

	if((action.length() > 10) && (action.compare(action.length() - 9, 9, "_template") == 0))
	{
		ostringstream	ost;
		string			strPageToGet, strFriendsOnSinglePage;

		MESSAGE_DEBUG("", action, "start");

		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			indexPage.Redirect("/" + GUEST_USER_DEFAULT_ACTION + "?rand=" + GetRandom(10));
		}
		else
		{
			string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_getMyTimecard")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			string			period_start_date, period_start_month, period_start_year;
			string			period_length;
			string			dbQuery;

			period_start_date = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("period_start_date"));
			period_start_month = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("period_start_month"));
			period_start_year = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("period_start_year"));

			if(period_start_date.length() && period_start_month.length() && period_start_year.length())
			{
				int		affected = 0;

				// --- 0-0-0 means today
				if(period_start_year == "0")
				{
				    time_t t = time(0);   // get time now
				    tm* now = localtime(&t);

				    period_start_year = to_string(now->tm_year + 1900);
				    period_start_month = to_string(now->tm_mon + 1);
				    period_start_date = to_string(now->tm_mday);

				}

				affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");
				if(affected)
				{
					auto		companies_list= ""s;
					auto		temp = GetTimecardsSOWTaskAssignement_Reusable_InJSONFormat(period_start_year + "-" + period_start_month + "-" + period_start_date, &db, &user);

/*
					for(int i = 0; i < affected; ++i)
					{
						if(companies_list.length()) companies_list += ",";
						companies_list += db.Get(i, "id");
					}
*/
					if(temp.empty())
					{
						MESSAGE_ERROR("", action, "timecard object is empty. check previous error message.")
						ostResult << "{\"status\":\"error\",\"description\":\"Ошибка в алгоритме.\"}";
					}
					else
					{
						ostResult	<< "{"
									<< "\"status\":\"success\","
									<< temp
									<< "}";
/*
					ostResult << "{"
									"\"status\":\"success\","
									"\"timecards\":[" << GetTimecardsInJSONFormat(
											"SELECT * FROM `timecards` WHERE "
												"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + ")) "
												"AND "
												"`period_start`<=\"" + period_start_year + "-" + period_start_month + "-" + period_start_date + "\" "
												"AND "
												"`period_end`>=\"" + period_start_year + "-" + period_start_month + "-" + period_start_date + "\""
											";", &db, &user) << "],"
									"\"sow\":[" << GetSOWInJSONFormat(
											"SELECT * FROM `contracts_sow` WHERE "
												"`subcontractor_company_id` IN (" + companies_list + ") "
												"AND "
												"`start_date`<=\"" + period_start_year + "-" + period_start_month + "-" + period_start_date + "\" "
												"AND "
												"`end_date`>=\"" + period_start_year + "-" + period_start_month + "-" + period_start_date + "\""
											";", &db, &user) << "],"
									"\"task_assignments\":[" << GetTimecardTaskAssignmentInJSONFormat(
											"SELECT * FROM `timecard_task_assignment` WHERE "
												"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
																			"`subcontractor_company_id` IN (" + companies_list + ") "
																			"AND "
																			"`start_date`<=\"" + period_start_year + "-" + period_start_month + "-" + period_start_date + "\" "
																			"AND "
																			"`end_date`>=\"" + period_start_year + "-" + period_start_month + "-" + period_start_date + "\""
																		");", &db, &user) << "]"
								"}";
*/
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
				}

			}
			else
			{
				{
					MESSAGE_DEBUG("", action, "period_start or period_length missed");
				}
				ostResult << "{\"status\":\"error\",\"description\":\"Пропущены обязательные параметры. Обратитьесь в тех. поддержку\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getSoWList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			int				affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");
			bool			include_bt = indexPage.GetVarsHandler()->Get("include_bt") == "true";
			bool			include_tasks = true;

			if(string(indexPage.GetVarsHandler()->Get("include_tasks")) == "false") include_tasks = false;

			if(affected)
			{
				string		companies_list= "";

				for(int i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, "id");
				}



				ostResult << "{"
								"\"status\":\"success\","
								"\"sow\":[" << GetSOWInJSONFormat(
										"SELECT * FROM `contracts_sow` WHERE "
											"`subcontractor_company_id` IN (" + companies_list + ") "
										";", &db, &user, include_tasks, include_bt) << "]";

				if(include_tasks)
					ostResult <<	","
									"\"task_assignments\":[" << GetTimecardTaskAssignmentInJSONFormat(
											"SELECT * FROM `timecard_task_assignment` WHERE "
												"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
													"`subcontractor_company_id` IN (" + companies_list + ") "
											");", &db, &user) << "]";
				ostResult << "}";
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
				ostResult << "{\"status\":\"error\",\"description\":\"Вы не создали компанию\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getCurrencyRateList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			string			date = indexPage.GetVarsHandler()->Get("date");
			string			error_message = "";
			string			rates_array = "";

			if(regex_match(date, regex("^[[:alnum:]]{4}\\-[[:alnum:]]{1,2}\\-[[:alnum:]]{1,2}$") ))
			{
				rates_array = GetCurrencyRatesInJSONFormat("SELECT * FROM `currency_rate` WHERE `date`=\"" + date + "\";", &db, &user);
			}
			else
			{
				error_message = "date format wrong(YYYY-MM-DD not matches " + date + ")";
			}

			if(error_message.empty())
			{

				ostResult << "{\"status\":\"success\",\"rates\":[" + rates_array + "]}";
			}
			else
			{
				MESSAGE_ERROR("", action, error_message);
				ostResult << "{\"status\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getBTList")
	{

		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			int				affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

			if(affected)
			{
				string		companies_list= "";

				for(int i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, "id");
				}

				ostResult << "{"
								"\"status\":\"success\","
								"\"sow\":[" << GetSOWInJSONFormat(
										"SELECT * FROM `contracts_sow` WHERE "
											"`subcontractor_company_id` IN (" + companies_list + ") "
										";", &db, &user) << "],"
								"\"bt\":[" << GetBTsInJSONFormat(
										"SELECT * FROM `bt` WHERE "
											"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + "))"
										";", &db, &user, false) << "]"
							"}";
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
				ostResult << "{\"status\":\"error\",\"description\":\"Вы не создали компанию\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	// --- this AJAX might be used by any user role 
	if(action == "AJAX_getBTEntry")
	{

		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";

			string			bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));

			if(bt_id.length())
			{
				string	error_message = isUserAllowedAccessToBT(bt_id, &db, &user);
				if(error_message.empty())
				{
					ostResult << "{"
									"\"status\":\"success\","
									"\"bt\":[" << GetBTsInJSONFormat("SELECT * FROM `bt` WHERE ""(`id`=\"" + bt_id + "\");", &db, &user, INCLUDE_ADDITIONAL_INFO) << "]"
								"}";
/*
					int				affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

					if(affected)
					{
						string		companies_list= "";

						for(int i = 0; i < affected; ++i)
						{
							if(companies_list.length()) companies_list += ",";
							companies_list += db.Get(i, "id");
						}

						ostResult << "{"
										"\"status\":\"success\","
										"\"bt\":[" << GetBTsInJSONFormat(
												"SELECT * FROM `bt` WHERE "
													"(`id`=\"" + bt_id + "\") "
													"AND "
													"(`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + ")))"
												";", &db, &user, INCLUDE_ADDITIONAL_INFO) << "]"
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
						ostResult << "{\"status\":\"error\",\"description\":\"Вы не создали компанию\"}";
					}
*/
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't have accee to bt.id(" + bt_id + ")");
					ostResult << "{\"status\":\"error\",\"description\":\"У Вас нет доступа\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "bt_id is empty")
				ostResult << "{\"status\":\"error\",\"description\":\"Некорректные параметры\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getTimecardList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			int				affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

			if(affected)
			{
				string		companies_list= "";

				for(int i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, "id");
				}

				ostResult << "{"
								"\"status\":\"success\","
								"\"sow\":[" << GetSOWInJSONFormat(
										"SELECT * FROM `contracts_sow` WHERE "
											"`subcontractor_company_id` IN (" + companies_list + ") "
										";", &db, &user) << "],"
								"\"timecards\":[" << GetTimecardsInJSONFormat(
										"SELECT * FROM `timecards` WHERE "
											"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + "))"
										";", &db, &user) << "]"
							"}";
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
				ostResult << "{\"status\":\"error\",\"description\":\"Вы не создали компанию\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getTimecardEntry")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";
			int				affected = 0;
			string			timecard_id = "";

			timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));

			if(timecard_id.length())
			{
				affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

				if(affected)
				{
					string		companies_list= "";

					for(int i = 0; i < affected; ++i)
					{
						if(companies_list.length()) companies_list += ",";
						companies_list += db.Get(i, "id");
					}

					// --- check ability to see this timecard
					if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + "))"))
					{
						ostResult << "{"
										"\"status\":\"success\","
										"\"timecards\":[" << GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";", &db, &user, true) << "]"
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
						ostResult << "{\"status\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"status\":\"error\",\"description\":\"Вы не создали компанию\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "parameter timecard_id is empty");
				ostResult << "{\"status\":\"error\",\"description\":\"Некорректые параметры\"}";
			}


			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_saveMyTimecard")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;
		struct ItemClass
		{
			string	customer;
			string	project;
			string	task;
			string	timereports;
		};
		vector<ItemClass>		itemsList;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			template_name = "json_response.htmlt";

			string			requested_status = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("status"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			string			current_period_start_year = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_start_year"));
			string			current_period_start_month = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_start_month"));
			string			current_period_start_date = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_start_date"));
			string			current_period_finish_year = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_finish_year"));
			string			current_period_finish_month = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_finish_month"));
			string			current_period_finish_date = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_finish_date"));

			if(sow_id.length() && requested_status.length() && current_period_start_year.length() && current_period_start_month.length() && current_period_start_date.length() && current_period_finish_year.length() && current_period_finish_month.length() && current_period_finish_date.length())
			{
				auto		error_description = ""s;
				auto		success_description = ""s;

				if(isUserAssignedToSoW(user.GetID(), sow_id, &db))
				{
					string		period_start = current_period_start_year + "-" + current_period_start_month + "-" + current_period_start_date;
					string		period_end = current_period_finish_year + "-" + current_period_finish_month + "-" + current_period_finish_date;
					auto		timecard_id = ""s;
					auto		timecard_status = ""s;
					auto		agency_id = ""s;

					timecard_id = GetTimecardID(sow_id, period_start, period_end, &db);
					timecard_status = GetTimecardStatus(timecard_id, &db);
					agency_id = GetAgencyID(sow_id, &db);


					if(timecard_id.length() && timecard_status.length() && agency_id.length())
					{
						int			i = 0;
						bool		isAgain;

						// --- task aggregation
						do
						{
							string	task = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task_" + to_string(i)));
							string	project = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project_" + to_string(i)));
							string	customer = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer_" + to_string(i)));

							isAgain = false;

							if(task.length() && project.length() && customer.length())
							{
								string 		timereports = CheckHTTPParam_Timeentry(indexPage.GetVarsHandler()->Get("timereports_" + to_string(i)));

								isAgain = true;

								if(timereports.length())
								{
									bool	isDuplicate = false;

									for (auto& item : itemsList)
									{
										if((item.task == task) && (item.project == project) && (item.customer == customer))
										{
											MESSAGE_DEBUG("", action, "http post parameter duplicate found (" + customer + " / " + project + " / " + task + ")");
											isDuplicate = true;

											item.timereports = SummarizeTimereports(item.timereports, timereports);
										}
									}

									if(!isDuplicate)
									{
										ItemClass	item;

										item.customer = customer;
										item.project = project;
										item.task = task;
										item.timereports = timereports;

										itemsList.push_back(item);
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "emtpy timereport on step " + to_string(i) + "");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "http post paramters walkthrough: stop on " + to_string(i) + "");
							}

							++i;
						} while(isAgain);


						// --- save timeentries in DB
						for(auto item: itemsList)
						{
							if(isTimecardEntryEmpty(item.timereports))
							{
								MESSAGE_DEBUG("", action, "timecard entry (" + item.timereports + ") is empty, don't add it to DB");
							}
							else
							{
								string		task_id = GetTaskIDFromSOW(item.customer, item.project, item.task, sow_id, &db);
								string		timecard_line_id;

								if(task_id.empty())
								{
									MESSAGE_DEBUG("", action, "task (" + item.customer + " / " + item.project + " / " + item.task + ") doesn't assigned to SoW (" + sow_id + ")");

									if(isSoWAllowedToCreateTask(sow_id, &db))
									{
										if(db.Query("SELECT `start_date`, `end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
										{
											string	sow_start_date = db.Get(0, "start_date");
											string	sow_end_date = db.Get(0, "end_date");
											string	task_assignment_id = "";
											bool	notify_about_task_creation = false;

											task_id = GetTaskIDFromAgency(item.customer, item.project, item.task, agency_id, &db);
											if(task_id.empty())
											{
												task_id = CreateTaskBelongsToAgency(item.customer, item.project, item.task, agency_id, &db);
												notify_about_task_creation = true;
											}
											else
											{
												MESSAGE_DEBUG("", action, "Customer/Project/Task already exists for this agency.id(" + agency_id + ")");
											}

											task_assignment_id = CreateTaskAssignment(task_id, sow_id, sow_start_date, sow_end_date, &db, &user);

											if(task_assignment_id.length())
											{
												if(notify_about_task_creation)
												{
													if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_addTask", task_id, sow_id, "", item.task, &db, &user))
													{
													}
													else
													{
														MESSAGE_ERROR("", "", "fail to notify SoW parties");
													}
												}
												else
												{
													MESSAGE_DEBUG("", "", "no notification about task creation");
												}
												if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_addTaskAssignment", task_assignment_id, sow_id, "", item.customer + " / " + item.project + " / " + item.task + " ( с " + sow_start_date + " по " + sow_end_date + ")", &db, &user))
												{
												}
												else
												{
													MESSAGE_ERROR("", "", "fail to notify SoW parties");
												}
											}
											else
											{
												MESSAGE_DEBUG("", action, "fail to create assignment sow.id(" + sow_id + ") task.id(" + task_id + ")");
												error_description = "Неудалось создать назначение";
												break;
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "sow(" + sow_id + ") start and end period can't be defined");
											error_description = "Ошибка БД";
											break;
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "SoW(" + sow_id + ") doesn't allows create tasks or consistensy issue");
										error_description = "SoW не позволяет создавать новые задачи";
										break;
									}
								}

								if((timecard_status == "saved") || (timecard_status == "rejected"))
								{
									error_description = isValidToReportTime(timecard_id, task_id, item.timereports, &db, &user);
									if(error_description.empty())
									{

									// --- update DB with timeentries
										timecard_line_id = GetTimecardLineID(timecard_id, task_id, &db);
										if(timecard_line_id.length())
										{
											// --- timeentry exists and need to be updated
											db.Query("UPDATE `timecard_lines` SET `row`=\"" + item.timereports + "\" WHERE `id`=\"" + timecard_line_id + "\";");
											if(db.isError())
											{
												MESSAGE_ERROR("", action, "DB INSERT issue: (" + db.GetErrorMessage() + ")");
												error_description = "ошибка БД";
												break;
											}
										}
										else
										{
											// --- timeentry doesn't exists and need to be created

											if(timecard_id.length() && task_id.length())
											{
												long int temp;

												temp = db.InsertQuery("INSERT INTO `timecard_lines` SET "
															"`timecard_id`=\"" + timecard_id + "\","
															"`timecard_task_id`=\"" + task_id + "\","
															"`row`=\"" + item.timereports + "\""
															";");
												if(!temp)
												{
													MESSAGE_ERROR("", action, "DB INSERT issue: (" + db.GetErrorMessage() + ")");
													error_description = "ошибка БД";
													break;
												}
											}
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") can't report time to task.id(" + task_id + ") in timecard_id(" + timecard_id + "). Timereport validity failed.");
										break;
									}
								}
								else if((timecard_status == "submit") || (timecard_status == "approved"))
								{
									MESSAGE_DEBUG("", action, "timecard_id(" + timecard_id + ") already submit, you can't change it");
									error_description = "Таймкарта уже отправлена, изменить нельзя";
									break;
								}
								else
								{
									MESSAGE_ERROR("", action, "unknown timecard_id(" + timecard_id + ") status (" + timecard_status + ")");
									error_description = "Ошибка таймкарты";
									break;
								}
							}
						}
					}
					else
					{
						error_description = "Таймкарта не найдена";
						MESSAGE_ERROR("", action, "timecard_id(" + timecard_id + ") or status (" + timecard_status + ") or agency_id(" + agency_id + ") is empty");
					}

					// --- updated timecard
					if(error_description.length())
					{
					}
					else
					{
						if((timecard_status == "saved") || (timecard_status == "rejected"))
						{
								if(requested_status == "saved")
								{
								}
								else if (requested_status == "submit")
								{
									db.Query("UPDATE `timecards` SET `status`=\"submit\", `submit_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
									if(!db.isError())
									{
										// --- auto_approve
										// --- UNIX_TIMESTAMP() + 1 needed to make visibility that approve been done after submission
										db.Query("INSERT INTO `timecard_approvals` (`timecard_id`, `approver_id`, `decision`, `comment`, `eventTimestamp`) "
														"SELECT \"" + timecard_id + "\", `id`, \"approved\", \"auto-approve\",UNIX_TIMESTAMP()+1 FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `auto_approve`=\"Y\"");


										if(SubmitTimecard(timecard_id, &db, &user))
										{
											success_description = GetTimecardsSOWTaskAssignement_Reusable_InJSONFormat(current_period_start_year + "-" + current_period_start_month + "-" + current_period_start_date, &db, &user);
										}
										else
										{
											error_description = "ошибка отправки таймкарты";
											MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + ")");
										}
									}
									else
									{
										error_description = "ошибка обновления таймкарты";
										MESSAGE_ERROR("", action, "fail to update timecards table with timecard_id(" + timecard_id + ")");
									}
								}
								else
								{
									MESSAGE_ERROR("", action, "unknown requested timecard status(" + requested_status + ")")
								}
						}
						else if((timecard_status == "submit") || (timecard_status == "approved"))
						{
							MESSAGE_DEBUG("", action, "timecard_id(" + timecard_id + ") already submit, you can't change it");
							error_description = "Таймкарта уже отправлена, изменить нельзя";
						}
						else
						{
							MESSAGE_ERROR("", action, "unknown timecard_id(" + timecard_id + ") status (" + timecard_status + ")");
							error_description = "Ошибка таймкарты";
						}
					}

					if(error_description.length())
						ostResult << "{\"result\":\"error\", \"description\":\"" + error_description + "\"}";
					else
						ostResult << "{\"result\":\"success\"" + (success_description.length() ? ", " + success_description : "") + "}";
				}
				else
				{
					MESSAGE_ERROR("", action, "SoW(" + sow_id + ") is not assigned to the user(" + user.GetID() + ") company");

					ostResult << "{\"result\":\"error\",\"description\":\"Данный SoW подписан не с Вами\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "mandatory parameter missed (" +
											"sow_id:" + sow_id +
											"status:" + requested_status +
											"current_period_start_year:" + current_period_start_year +
											"current_period_start_month:" + current_period_start_month +
											"current_period_start_date:" + current_period_start_date +
											"current_period_finish_year:" + current_period_finish_year +
											"current_period_finish_month:" + current_period_finish_month +
											"current_period_finish_date:" + current_period_finish_date + ")");

				ostResult << "{\"result\":\"error\",\"description\":\"Пропущены обязательные параметры\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			} // if(!indexPage.SetTemplate("my_network.htmlt"))
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getAutocompleteCustomerList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string		template_name = "json_response.htmlt";
			string		sow_id;
			string		lookForKey;

			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			lookForKey = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("lookForKey"));

			if(sow_id.length() && lookForKey.length())
			{

				if(db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\" AND `id` IN (SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");"))
				{
					string		items = "";
					string		company_id = "";

					company_id = db.Get(0, "id");

					items = GetTimecardCustomersInJSONFormat(
								"SELECT * FROM `timecard_customers` WHERE `title` LIKE \"%" + lookForKey + "%\" AND "
									"`id` IN (SELECT `timecard_customers_id` FROM `timecard_projects` WHERE "
										"`id` IN (SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE "
											"`id` IN (SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\")))", &db, &user);

					ostResult << "{\"status\":\"success\",\"items\":[" + items + "]}";
				}
				else
				{
					MESSAGE_ERROR("", action, "company not found");
					ostResult << "{\"status\":\"error\",\"description\":\"Не найдена компания с которой заключен этот договор\"}";
				}
			}
			else
			{
				{
					MESSAGE_ERROR("", action, "lookForKey or sow_id is missed");
				}
				ostResult << "{\"status\":\"error\",\"description\":\"Пропущены обязательные параметры. Обратитьесь в тех. поддержку\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getDashboardData")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string		template_name = "json_response.htmlt";
			int			affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

			if(affected)
			{
				string		companies_list= "";
				bool		successFlag = true;
				string		rejected_timecards = "";
				string		rejected_bt = "";

				for(int i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, "id");
				}

				// --- gather failed timecards
				rejected_timecards = GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `status`=\"rejected\" AND `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + "));", &db, &user, false);
				// --- gather failed bt
				rejected_bt = GetBTsInJSONFormat("SELECT * FROM `bt` WHERE `status`=\"rejected\" AND `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + "));", &db, &user, false);

				if(successFlag)
				{
					ostResult << "{"
									"\"status\":\"success\","
									"\"rejected_timecards\":[" << rejected_timecards << "],"
									"\"rejected_bt\":[" << rejected_bt << "]"
								"}";
				}
				else
				{
					MESSAGE_ERROR("", action, "dashboard data not gathered completely");
					ostResult << "{\"status\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
				ostResult << "{\"status\":\"error\",\"description\":\"Вы не создали компанию\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_submitBT")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string					template_name = "json_response.htmlt";
			string					error_message = "";
			CVars					*http_params = indexPage.GetVarsHandler();
			string					expense_random_name;
			string					expense_line_random_name;
			regex					regex1("expense_item_random_");
			regex					regex2("expense_item_doc_random_");
			C_BT					bt;
			vector<C_Expense>		expenses;

			bt.SetDB			(&db);
			bt.SetUser			(&user);
			bt.SetID			(CheckHTTPParam_Number	(http_params->Get("bt_id")));
			bt.SetSowID			(CheckHTTPParam_Number	(http_params->Get("sow_id")));
			bt.SetCustomerID	(CheckHTTPParam_Number	(http_params->Get("customer_id")));
			bt.SetDestination	(CheckHTTPParam_Text	(http_params->Get("destination")));
			bt.SetPurpose		(CheckHTTPParam_Text	(http_params->Get("bt_purpose")));
			bt.SetStartDate		(CheckHTTPParam_Date	(http_params->Get("bt_start_date")));
			bt.SetEndDate		(CheckHTTPParam_Date	(http_params->Get("bt_end_date")));
			bt.SetAction		(CheckHTTPParam_Text	(http_params->Get("action_type")));
	
			expense_random_name = http_params->GetNameByRegex(regex1);
			while(expense_random_name.length() && error_message.empty())
			{
				string expense_random = CheckHTTPParam_Number(http_params->Get(expense_random_name));

				if(expense_random.length())
				{
					C_Expense	expense;

					expense.SetRandom				(expense_random);
					expense.SetID					(CheckHTTPParam_Number	(http_params->Get("bt_expense_id_" + expense_random)));
					expense.SetDate					(CheckHTTPParam_Date	(http_params->Get("expense_item_date_" + expense_random)));
					expense.SetPaymentType			(CheckHTTPParam_Text	(http_params->Get("expense_item_payment_type_" + expense_random)));
					expense.SetExpenseTemplateID	(CheckHTTPParam_Number	(http_params->Get("expense_item_type_" + expense_random)));
					expense.SetComment				(CheckHTTPParam_Text	(http_params->Get("expense_item_comment_" + expense_random)));
					expense.SetPriceDomestic		(CheckHTTPParam_Float	(http_params->Get("expense_item_price_domestic_" + expense_random)));
					expense.SetPriceForeign			(CheckHTTPParam_Float	(http_params->Get("expense_item_price_foreign_" + expense_random)));
					expense.SetCurrencyNominal		(CheckHTTPParam_Number	(http_params->Get("expense_item_currency_nominal_" + expense_random)));
					expense.SetCurrencyName			(CheckHTTPParam_Text	(http_params->Get("expense_item_currency_name_" + expense_random)));
					expense.SetCurrencyValue		(CheckHTTPParam_Float	(http_params->Get("expense_item_currency_value_" + expense_random)));

					expenses.push_back(expense);
					bt.AddExpense(expense);
				}
				else
				{
					error_message = "некорректный параметер random у растраты";
					MESSAGE_ERROR("", action, "Can't convert expense_random(" + http_params->Get(expense_random_name) + ") to number")
				}

				http_params->Delete(expense_random_name);
				expense_random_name = http_params->GetNameByRegex(regex1);
			}

			expense_line_random_name = http_params->GetNameByRegex(regex2);
			while(expense_line_random_name.length() && error_message.empty())
			{
				string expense_line_random = CheckHTTPParam_Number(http_params->Get(expense_line_random_name));

				if(expense_line_random.length())
				{
					C_ExpenseLine	expense_line;
					string			parent_random = 	CheckHTTPParam_Number	(http_params->Get("expense_item_parent_random_" + expense_line_random));

					if(parent_random.length())
					{
						string		dom_type = 			CheckHTTPParam_Text	(http_params->Get("expense_item_doc_dom_type_" + expense_line_random));

						if((dom_type == "image") || (dom_type == "input"))
						{
							expense_line.SetRandom					(expense_line_random);
							expense_line.SetParentRandom			(parent_random);
							expense_line.SetID						(CheckHTTPParam_Number	(http_params->Get("expense_line_id_" + expense_line_random)));
							expense_line.SetExpenseLineTemplateID	(CheckHTTPParam_Number	(http_params->Get("expense_item_doc_id_" + expense_line_random)));
							expense_line.SetComment					(CheckHTTPParam_Text	(http_params->Get("expense_item_doc_comment_" + expense_line_random)));

							if(dom_type == "input")
								expense_line.SetValue				(CheckHTTPParam_Text	(http_params->Get("expense_item_doc_main_field_" + expense_line_random)));
							if(dom_type == "image")
							{
								string		path_to_file = "";
								int			file_size = indexPage.GetFilesHandler()->GetSize("expense_item_doc_main_field_" + expense_line_random + ".jpg");

								if(file_size > 0)
								{
									// --- take care of an image file
									MESSAGE_DEBUG("", action, "expense_item_doc_main_field_" + expense_line_random + ".jpg file size is " + to_string(file_size));
									path_to_file = SaveImageFileFromHandler("expense_item_doc_main_field_" + expense_line_random + ".jpg", "expense_line", indexPage.GetFilesHandler());
									expense_line.SetValue(path_to_file);

									if(path_to_file.length())
									{
										MESSAGE_DEBUG("", action, "image to expense_line(random = " + expense_line_random + ") uploaded to " + path_to_file + "");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "file to expense_line(random = " + expense_line_random + ") not uploaded");
								}
							}

							if(bt.AssignExpenseLineByParentRandom(expense_line))
							{
							}
							else
							{
								error_message = "неполучилось определить принажделжность док-та";
								MESSAGE_ERROR("", action, "AssignExpenseLineByParentRandom fail");
							}
						}
						else
						{
							error_message = "некорректный тип основного док-та";
							MESSAGE_ERROR("", action, "primary doc-t wrong type(" + dom_type + ")");
						}
					}
					else
					{
						error_message = "невозможно определить принажделжность документа";
						MESSAGE_ERROR("", action, "parent expense.id missed for expense_line.rand(" + expense_line_random + ")");
					}
				}
				else
				{
					error_message = "некорректный параметер random у док-та";
					MESSAGE_ERROR("", action, "Can't convert expense_line_random(" + http_params->Get(expense_random_name) + ") to number")
				}

				http_params->Delete(expense_line_random_name);
				expense_line_random_name = http_params->GetNameByRegex(regex2);
			}

			if(error_message.empty())
			{
				error_message = bt.CheckValidity();
				if(error_message.empty())
				{
					// --- update DB
					error_message = bt.SaveToDB();
					if(error_message.length()) MESSAGE_ERROR("", action, "fail saving BT to DB");
					
				}
				else
				{
					MESSAGE_DEBUG("", action, "BT validity chack failed");
				}
			}

			{
				ostringstream	ost;

				ost.str("");
				ost << bt;
				MESSAGE_DEBUG("", action, "C_BT obj dump: " + ost.str());
			}

			if(error_message.empty())
			{
				ostResult << "{\"status\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "business trip validity check failed");

				if(bt.RemoveUnsavedLinesImages())
				{
				}
				else
				{
					MESSAGE_ERROR("", action, "fail to remove unsaved images");
				}

				ostResult << "{\"status\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}
		}

		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_removeExpense")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string					template_name = "json_response.htmlt";
			string 					expense_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_expense_id"));
			string					error_message = "";

			if(expense_id.length())
			{
				if(db.Query("SELECT `bt_id` FROM `bt_expenses` WHERE `id`=\"" + expense_id + "\";"))
				{
					string		bt_id = db.Get(0, "bt_id");

					if(db.Query("SELECT `status`, `contract_sow_id` FROM `bt` WHERE `id`= \"" + bt_id + "\";"))
					{
						string	contract_sow_id = db.Get(0, "contract_sow_id");
						string	status = db.Get(0, "status");

						if((status != "approved"))
						{

							if(db.Query(
								"SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\" AND `id` IN ("
									"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + contract_sow_id + "\""
								")"
							))
							{
								int		expense_count = db.Query("SELECT `id` FROM `bt_expenses` WHERE `bt_id`=\"" + bt_id + "\";");

								if(expense_count > 1)
								{
									// --- remove expense

									int affected = db.Query(
										"SELECT `bt_expense_lines`.`value` as `value`, `bt_expense_line_templates`.`dom_type` as `dom_type` FROM `bt_expense_lines` "
										"INNER JOIN `bt_expense_line_templates` on `bt_expense_lines`.`bt_expense_line_template_id`=`bt_expense_line_templates`.`id` "
										"WHERE `bt_expense_id`=\"" + expense_id + "\";"
										);
									for(int i = 0; i < affected; ++i)
									{
										string value = db.Get(i, "value");
										string dom_type = db.Get(i, "dom_type");

										if((dom_type == "image") && (value.length()))
										{
											MESSAGE_DEBUG("", action, "remove image (" + IMAGE_EXPENSELINES_DIRECTORY + "/" + value + ")");
											unlink((IMAGE_EXPENSELINES_DIRECTORY + "/" + value).c_str());
										}
									}

									db.Query("DELETE FROM `bt_expense_lines` WHERE `bt_expense_id`=\"" + expense_id + "\";");
									db.Query("DELETE FROM `bt_expenses` WHERE `id`=\"" + expense_id + "\";");
								}
								else
								{
									error_message = "Нельзя удалять единственный расход";
									MESSAGE_DEBUG("", action, "bt.id() have single bt_expense.id(" + expense_id + "), prohibited to remove single expense");
								}

							}
							else
							{
								error_message = "У вас нет прав на удаление";
								MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") can't remove bt_expense.id(" + expense_id + ")");
							}
						}
						else
						{
							error_message = "Нельзя менять подтвержденный отчет";
							MESSAGE_DEBUG("", action, "bt.id(" + bt_id + ") can't be changed, because it approved");
						}
					}
					else
					{
						error_message = "Отчет которому принадлежит расход не найден";
						MESSAGE_DEBUG("", action, "bt.id not found by bt_expense.id(" + expense_id + ")");
					}

				}
				else
				{
					error_message = "Расход не найден";
					MESSAGE_DEBUG("", action, "bt_expense.id(" + expense_id + ") not found");
				}

			}
			else
			{
				error_message = "Неизвестный идентификатор удаления";
				MESSAGE_DEBUG("", action, "HTTP param expense.id is not a number");
			}

			if(error_message.empty())
			{
				ostResult << "{\"status\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "bt_expense(" + expense_id + ") removal failed");
				ostResult << "{\"status\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}
		}

		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_removeBT")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		{
			MESSAGE_DEBUG("", action, "start");
		}

		ostResult.str("");
		if(user.GetLogin() == "Guest")
		{
			{
				MESSAGE_DEBUG("", action, "re-login required");
			}

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string					template_name = "json_response.htmlt";
			string 					bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
			string					error_message = "";

			if(bt_id.length())
			{
				if(db.Query("SELECT `status`, `contract_sow_id` FROM `bt` WHERE `id`= \"" + bt_id + "\";"))
				{
					string	contract_sow_id = db.Get(0, "contract_sow_id");
					string	status = db.Get(0, "status");

					if((status != "approved"))
					{

						if(db.Query(
							"SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\" AND `id` IN ("
								"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + contract_sow_id + "\""
							")"
						))
						{
							{
								// --- remove expense

								int affected = db.Query(
									"SELECT `bt_expense_lines`.`value` as `value`, `bt_expense_line_templates`.`dom_type` as `dom_type` FROM `bt_expense_lines` "
									"INNER JOIN `bt_expense_line_templates` on `bt_expense_lines`.`bt_expense_line_template_id`=`bt_expense_line_templates`.`id` "
									"WHERE `bt_expense_id` IN ( "
										" SELECT `id` FROM `bt_expenses` WHERE `bt_id`=\"" + bt_id + "\" "
									");"
									);
								for(int i = 0; i < affected; ++i)
								{
									string value = db.Get(i, "value");
									string dom_type = db.Get(i, "dom_type");

									if((dom_type == "image") && (value.length()))
									{
										MESSAGE_DEBUG("", action, "remove image (" + IMAGE_EXPENSELINES_DIRECTORY + "/" + value + ")");
										unlink((IMAGE_EXPENSELINES_DIRECTORY + "/" + value).c_str());
									}
								}

								db.Query("DELETE FROM `bt_expense_lines` WHERE `bt_expense_id` IN ( "
											" SELECT `id` FROM `bt_expenses` WHERE `bt_id`=\"" + bt_id + "\" "
										");");
								db.Query("DELETE FROM `bt_expenses` WHERE `bt_id`=\"" + bt_id + "\";");
								db.Query("DELETE FROM `bt` WHERE `id`=\"" + bt_id + "\";");
								db.Query("DELETE FROM `bt_approvals` WHERE `bt_id`=\"" + bt_id + "\";");
							}
						}
						else
						{
							error_message = "У вас нет прав на удаление";
							MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") can't remove bt.id(" + bt_id + ")");
						}
					}
					else
					{
						error_message = "Нельзя удалить подтвержденный отчет";
						MESSAGE_DEBUG("", action, "bt.id(" + bt_id + ") can't be куьщмув, because it approved");
					}
				}
				else
				{
					error_message = "Отчет  не найден";
					MESSAGE_DEBUG("", action, "bt.id(" + bt_id + ") not found");
				}
			}
			else
			{
				error_message = "Неизвестный идентификатор удаления";
				MESSAGE_DEBUG("", action, "HTTP param expense.id is not a number");
			}

			if(error_message.empty())
			{
				ostResult << "{\"status\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "bt.id(" + bt_id + ") removal failed");
				ostResult << "{\"status\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}
		}

		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getCompanyInfo")
	{
		ostringstream	ostResult;
		string			error_message = "";
		string			template_name = "json_response.htmlt";

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
		if(user.GetType() != "subcontractor")
		{
			MESSAGE_DEBUG("", action, "re-login required");
			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			string			agency_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			include_countries = indexPage.GetVarsHandler()->Get("include_countries") == "true";

			if(db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";"))
			{
				string	company_id = db.Get(0, "id");
				string	company_obj = GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", &db, &user);

				if(company_obj.length())
				{
					// --- get short info
					ostResult << "{\"result\":\"success\","
									"\"companies\":[" + company_obj + "]"
									+ (include_countries ? ","s + "\"countries\":[" + GetCountryListInJSONFormat("SELECT * FROM `geo_country`;", &db, &user) + "]" : "")
									+ "}";
				}
				else
				{
					error_message = "Company не найденa";
					MESSAGE_DEBUG("", action, "agency(" + agency_id + ") not found");
				}
			}
			else
			{
				error_message = "You are not the company owner";
				MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") doesn't own any company");
			}
		}

		if(error_message.empty())
		{
		}
		else
		{
			MESSAGE_DEBUG("", action, "failed");
			ostResult.str("");
			ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
		}

		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(
		(action == "AJAX_updateCompanyTitle")					||
		(action == "AJAX_updateCompanyDescription")				||
		(action == "AJAX_updateCompanyWebSite")					||
		(action == "AJAX_updateCompanyTIN")						||
		(action == "AJAX_updateCompanyVAT")						||
		(action == "AJAX_updateCompanyAccount")					||
		(action == "AJAX_updateCompanyOGRN")					||
		(action == "AJAX_updateCompanyKPP")						||
		(action == "AJAX_updateCompanyLegalAddress")			||
		(action == "AJAX_updateCompanyMailingAddress")			||
		(action == "AJAX_updateCompanyMailingZipID")			||
		(action == "AJAX_updateCompanyLegalZipID")				||
		(action == "AJAX_updateCompanyBankID")
	)
	{
		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
		{
			string			template_name = "json_response.htmlt";
			string			error_message = "";

			string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			if(new_value.length())
			{
				{
					auto	company_id = GetCompanyID(&user, &db);
					if(company_id.length())
					{
						if(action == "AJAX_updateCompanyTitle") 		{	id = company_id; }
						if(action == "AJAX_updateCompanyDescription")	{	id = company_id; }
						if(action == "AJAX_updateCompanyWebSite")		{	id = company_id; }
						if(action == "AJAX_updateCompanyTIN")			{	id = company_id; }
						if(action == "AJAX_updateCompanyVAT")			{	id = company_id; }
						if(action == "AJAX_updateCompanyAccount")		{	id = company_id; }
						if(action == "AJAX_updateCompanyOGRN")			{	id = company_id; }
						if(action == "AJAX_updateCompanyKPP")			{	id = company_id; }
						if(action == "AJAX_updateCompanyLegalAddress") 	{	id = company_id; }
						if(action == "AJAX_updateCompanyMailingAddress"){	id = company_id; }
						if(action == "AJAX_updateCompanyMailingZipID") 	{	new_value = id; id = company_id; }
						if(action == "AJAX_updateCompanyLegalZipID") 	{	new_value = id; id = company_id; }
						if(action == "AJAX_updateCompanyBankID") 		{	new_value = id; id = company_id; }

						// error_message = isActionEntityBelongsToCompany(action, id, company_id, &db, &user);
						// if(error_message.empty())
						// {
							// error_message = CheckNewValueByAction(action, id, "", new_value, &db, &user);
							// if(error_message.empty())
							// {
								if(action.find("update"))
								{
									string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

									error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
									if(error_message.empty())
									{
										ostResult << "{\"result\":\"success\"}";

										if(GeneralNotifySoWContractPartiesAboutChanges(action, id, "", existing_value, new_value, &db, &user))
										{
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to notify subcontractor about changes");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
									}
								}
								else if(action.find("insert"))
								{
									
								}
								else if(action.find("delete"))
								{

								}
								else
								{
									MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
								}

/*
							}
							else
							{
								MESSAGE_DEBUG("", action, "new value failed to pass validity check");
							}
*/
/*
						}
						else
						{
							MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + company_id + ")");
						}
*/
					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "company.id not found by user.id(" + user.GetID() + ") employment");
					}
				}
/*
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
				}
*/
			}
			else
			{
				error_message = "Поле не должно быть пустым";
				MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_agreeSoW")
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
		{

			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			if(sow_id.length())
			{
				auto		error_description = ""s;
				auto		success_description = ""s;

				if(isUserAssignedToSoW(user.GetID(), sow_id, &db))
				{
					db.Query("UPDATE `contracts_sow` SET `status`=\"signed\" WHERE `id`=\"" + sow_id + "\";");
					if(db.isError())
					{
						error_message = gettext("SQL syntax issue");
						MESSAGE_ERROR("", "", error_message);
					}
					
					if(error_message.empty())
					{
						NotifySoWContractPartiesAboutChanges(to_string(NOTIFICATION_SUBCONTRACTOR_SIGNED_SOW), sow_id, &db, &user);
					}

				}
				else
				{
					error_message = gettext("You are not authorized");
					MESSAGE_ERROR("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_ERROR("", action, error_message);
			}


		}

		if(error_message.empty())
		{
			ostResult << "{\"result\":\"success\"" + success_message + "}";
		}
		else
		{
			MESSAGE_DEBUG("", action, "failed");
			ostResult.str("");
			ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
		}

		indexPage.RegisterVariableForce("result", ostResult.str());

		if(!indexPage.SetTemplate(template_name))
		{
			MESSAGE_ERROR("", action, "can't find template " + template_name);
		}
	}

	{
		MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");
	}

	indexPage.OutTemplate();

	}
/*
	catch(CExceptionRedirect &c) {
		{
			MESSAGE_DEBUG("", "", "catch CRedirectHTML: exception used for redirection");
		}

		c.SetDB(&db);

		if(!indexPage.SetTemplate(c.GetTemplate()))
		{
			MESSAGE_ERROR("", "", "catch CRedirectHTML: ERROR, template redirect.htmlt not found");
			throw CException("Template file was missing");
		}

		indexPage.RegisterVariableForce("content", "redirect page");
		indexPage.OutTemplate();

	}
*/
	catch(CExceptionHTML &c)
	{

		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);
		MESSAGE_DEBUG("", action, "catch CExceptionHTML: DEBUG exception reason: [" + c.GetReason() + "]");

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
		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}
		{
			MESSAGE_ERROR("", action, "catch CException: exception: ERROR  " + c.GetReason());
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(-1);
	}
	catch(exception& e)
	{
		{
			MESSAGE_ERROR("", action, "catch(exception& e): catch standard exception: ERROR  " + e.what());
		}

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
