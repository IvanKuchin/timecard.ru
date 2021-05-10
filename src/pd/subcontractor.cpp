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
static auto	GetTimecardsSOWTaskAssignment_Reusable_InJSONFormat(string date, CMysql *db, CUser *user)
{
	auto		result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(date.length())
	{
		if(user)
		{
			if(db)
			{
				auto	timecard_query = 
								"FROM `timecards` WHERE "
									"`contract_sow_id` IN ("
										"SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN ("
											"SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\""
										")"
									") "
									"AND "
									"`period_start`<=\"" + date + "\" "
									"AND "
									"`period_end`>=\"" + date + "\" ";

				result =
						"\"timecards\":[" + GetTimecardsInJSONFormat("SELECT * " + timecard_query + ";", db, user) + "],"
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
															");", db, user) + "],"
						"\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat(
								"SELECT * FROM `holiday_calendar` WHERE "
									"`date`>=(DATE_SUB(" + quoted(date) + ", INTERVAL " + to_string(HOLIDAY_RANGE_FROM_TODAY) + " DAY)) "
									"AND "
									"`date`<=(DATE_ADD(" + quoted(date) + ", INTERVAL " + to_string(HOLIDAY_RANGE_FROM_TODAY) + " DAY)) "
								";", db, user) + "]";
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
*/
static auto	GetTimecardsSOWTaskAssignment_Reusable_InJSONFormat(string date, CMysql *db, CUser *user)
{
	auto		result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(date.length())
	{
		if(user)
		{
			if(db)
			{
				auto	timecard_query = 
								"FROM `timecards` WHERE "
									"`contract_sow_id` IN ("
										"SELECT `id` FROM `contracts_sow` WHERE "
										"`subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\") "
									") "
									"AND "
									"`period_start`<=\"" + date + "\" "
									"AND "
									"`period_end`>=\"" + date + "\" ";

				result =
						"\"timecards\":[" + GetTimecardsInJSONFormat("SELECT * " + timecard_query + ";", db, user) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE `id` IN (" + Get_SoWIDsBySubcUserIDAndDate_sqlquery(user->GetID(), date) + ");", db, user) + "],"
						"\"task_assignments\":[" + GetTimecardTaskAssignmentInJSONFormat(
								"SELECT * FROM `timecard_task_assignment` WHERE `contract_sow_id` IN (" + Get_SoWIDsBySubcUserIDAndDate_sqlquery(user->GetID(), date) + ");", db, user) + "],"
	/*					"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
										"`subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\") "
										"AND "
										"`start_date`<=\"" + date + "\" "
										"AND "
										"`end_date`>=(DATE_SUB(" + quoted(date) + ", INTERVAL " + to_string(SOW_EXPIRATION_BUFFER) + " DAY))"
								";", db, user) + "],"
						"\"task_assignments\":[" + GetTimecardTaskAssignmentInJSONFormat(
								"SELECT * FROM `timecard_task_assignment` WHERE "
									"`contract_sow_id` IN ("
										"SELECT `id` FROM `contracts_sow` WHERE "
										"`subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user->GetID() + "\") "
										"AND "
										"`start_date`<=\"" + date + "\" "
										"AND "
										"`end_date`>=(DATE_SUB(" + quoted(date) + ", INTERVAL " + to_string(SOW_EXPIRATION_BUFFER) + " DAY))"
									");", db, user) + "],"
	*/
						"\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat(
								"SELECT * FROM `holiday_calendar` WHERE "
									"`date`>=(DATE_SUB(" + quoted(date) + ", INTERVAL " + to_string(HOLIDAY_RANGE_FROM_TODAY) + " DAY)) "
									"AND "
									"`date`<=(DATE_ADD(" + quoted(date) + ", INTERVAL " + to_string(HOLIDAY_RANGE_FROM_TODAY) + " DAY)) "
								";", db, user) + "]";
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
	CStatistics		appStat;  // --- CStatistics must be first statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	c_config		config(CONFIG_DIR);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	{
		MESSAGE_DEBUG("", action, __FILE__);
	}

	signal(SIGSEGV, crash_handler);

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);  /* Flawfinder: ignore */

	try
	{

	indexPage.ParseURL();
	indexPage.AddCookie("lng", "ru", nullptr, "", "/");

	if(!indexPage.SetTemplate("index.htmlt"))
	{
		MESSAGE_DEBUG("", action, "template file was missing");
		throw CException("Template file was missing");
	}

	if(db.Connect(&config) < 0)
	{
		MESSAGE_DEBUG("", action, "Can not connect to mysql database");
		throw CException("MySql connection");
	}

	indexPage.SetDB(&db);


	action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));

	MESSAGE_DEBUG("", "", " action = " + action);

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
		action = GenerateSession(action, &config, &indexPage, &db, &user);
		action = LogoutIfGuest(action, &config, &indexPage, &db, &user);
	}
// ------------ end generate common parts

	MESSAGE_DEBUG("", "", "pre-condition if(action == \"" + action + "\")");

	if((action.length() > 10) && (action.compare(action.length() - 9, 9, "_template") == 0))
	{
		ostringstream	ost;
		string			strPageToGet, strFriendsOnSinglePage;

		MESSAGE_DEBUG("", action, "start");

/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			indexPage.Redirect("/" + GUEST_USER_DEFAULT_ACTION + "?rand=" + GetRandom(10));
		}
		else
*/		{
			string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}
		}

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_getMyTimecard")
	{
		MESSAGE_DEBUG("", action, "start");

		auto			error_message = ""s;
		auto			success_message = ""s;
		auto			period_start_date = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("period_start_date"));
		auto			period_start_month = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("period_start_month"));
		auto			period_start_year = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("period_start_year"));
		auto			template_name = "json_response.htmlt"s;

		if(period_start_date.length() && period_start_month.length() && period_start_year.length())
		{
			auto		affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

			// --- 0-0-0 means today
			if(period_start_year == "0")
			{
			    time_t t = time(0);   // get time now
			    tm* now = localtime(&t);

			    period_start_year = to_string(now->tm_year + 1900);
			    period_start_month = to_string(now->tm_mon + 1);
			    period_start_date = to_string(now->tm_mday);
			}

			if(affected)
			{
				auto		companies_list= ""s;

				success_message = GetTimecardsSOWTaskAssignment_Reusable_InJSONFormat(period_start_year + "-" + period_start_month + "-" + period_start_date, &db, &user);

				if(success_message.empty())
				{
					error_message = gettext("algorithm error");
					MESSAGE_ERROR("", action, error_message)
				}
				else
				{
					// --- good2go
				}

			}
			else
			{
				error_message = gettext("you are not authorized");
				MESSAGE_ERROR("", action, error_message);
			}

		}
		else
		{
			error_message = gettext("mandatory parameter missed");
			MESSAGE_DEBUG("", action, error_message);
		}

		AJAX_ResponseTemplate(&indexPage, success_message, error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_getSoWList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getCurrencyRateList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getBTList")
	{
		MESSAGE_DEBUG("", action, "start");

		auto	error_message			= ""s;
		auto	success_message			= ""s;
		auto	filter_sow_date			= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("date"));
		auto	filter_not_sow_status	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter_not_sow_status"));
		auto	filter_sow_status		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter_sow_status"));
		auto	limit_page				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("page"));
		auto	isExtended				= indexPage.GetVarsHandler()->Get("extended") == "true";
		auto	companies_id			= GetValuesFromDB(Get_CompanyIDByAdminUserID_sqlquery(user.GetID()), &db);


		if(companies_id.size())
		{
			success_message = GetBTList("`subcontractor_company_id` IN (" + join(companies_id, ",") + ")", filter_sow_date, filter_sow_status, filter_not_sow_status, limit_page, isExtended, &db, &user);
		}
		else
		{
			error_message = gettext("you are not authorized");
			MESSAGE_ERROR("", action, error_message);
		}

		AJAX_ResponseTemplate(&indexPage, success_message, error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_getTravelList")
	{

		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

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
								"\"airline_bookings\":[" << GetAirlineBookingsInJSONFormat(
										"SELECT * FROM `airline_bookings` WHERE "
											"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + companies_list + ")) "
											"AND "
											"`status`=\"done\""
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
			}
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

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't have access to bt.id(" + bt_id + ")");
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
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getTimecardList")
	{
		MESSAGE_DEBUG("", action, "start");

		auto	error_message	= ""s;
		auto	success_message	= ""s;
		auto	filter_sow_date			= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("date"));
		auto	filter_not_sow_status	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter_not_sow_status"));
		auto	filter_sow_status		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter_sow_status"));
		auto	limit_page				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("page"));
		auto	companies_id	= GetValuesFromDB(Get_CompanyIDByAdminUserID_sqlquery(user.GetID()), &db);

		if(companies_id.size())
		{
			success_message = GetTimecardList("`subcontractor_company_id` IN (" + join(companies_id, ",") + ")", filter_sow_date, filter_sow_status, filter_not_sow_status, limit_page, &db, &user);
		}
		else
		{
			error_message = gettext("you are not authorized");
			MESSAGE_ERROR("", action, error_message);
		}

		AJAX_ResponseTemplate(&indexPage, success_message, error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_getTimecardEntry")
	{
		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
		{
			string			template_name = "json_response.htmlt";
			string			timecard_id = "";

			timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));

			if(timecard_id.length())
			{
				auto	affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

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
										"\"result\":\"success\","
										"\"timecards\":[" << GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";", &db, &user, true) << "]"
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
						ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не создали компанию\"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "parameter timecard_id is empty");
				ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
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

		{
			auto			template_name = "json_response.htmlt"s;

			auto			requested_status = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("status"));
			auto			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			auto			current_period_start_year = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_start_year"));
			auto			current_period_start_month = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_start_month"));
			auto			current_period_start_date = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_start_date"));
			auto			current_period_finish_year = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_finish_year"));
			auto			current_period_finish_month = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_finish_month"));
			auto			current_period_finish_date = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("current_period_finish_date"));

			if(sow_id.length() && requested_status.length() && current_period_start_year.length() && current_period_start_month.length() && current_period_start_date.length() && current_period_finish_year.length() && current_period_finish_month.length() && current_period_finish_date.length())
			{
				auto		error_description = ""s;
				auto		success_description = ""s;

				if(isUserAssignedToSoW(user.GetID(), sow_id, &db))
				{
					auto		period_start = current_period_start_year + "-" + current_period_start_month + "-" + current_period_start_date;
					auto		period_end = current_period_finish_year + "-" + current_period_finish_month + "-" + current_period_finish_date;
					auto		timecard_id = ""s;
					auto		timecard_status = ""s;
					auto		agency_id = ""s;


					if(error_description.length())
					{
					}
					else
					{
						int			i = 0;
						bool		isAgain;
						auto		is_time_reported = false;

						// --- task aggregation
						do
						{
							auto	task = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task_" + to_string(i)));
							auto	project = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project_" + to_string(i)));
							auto	customer = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer_" + to_string(i)));

							isAgain = false;

							if(task.length() && project.length() && customer.length())
							{
								auto 		timereports = CheckHTTPParam_Timeentry(indexPage.GetVarsHandler()->Get("timereports_" + to_string(i)));

								isAgain = true;

								if(timereports.length())
								{
									bool	isDuplicate = false;

									for (auto& item : itemsList)
									{
										if((item.task == task) && (item.project == project) && (item.customer == customer))
										{
											MESSAGE_DEBUG("", action, "http post parameter duplicate found (" + customer + TIMECARD_ENTRY_TITLE_SEPARATOR + project + TIMECARD_ENTRY_TITLE_SEPARATOR + task + ")");
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

										is_time_reported = is_time_reported || (!isTimecardEntryEmpty(item.timereports));
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "empty timereport on step " + to_string(i) + "");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "http post parameters walkthrough: stop on " + to_string(i) + "");
							}

							++i;
						} while(isAgain);

						if(is_time_reported)
						{
							// --- good2go
						}
						else
						{
							error_description = gettext("timecard requires tasks to report on");
							MESSAGE_DEBUG("", action, error_description);
						}
					}

					if(error_description.length())
					{
					}
					else
					{
						if((error_description = isTimePeriodInsideSow(sow_id, period_start, period_end, &db, &user)).empty())
						{
							timecard_id = GetTimecardID(sow_id, period_start, period_end, &db);
							timecard_status = GetTimecardStatus(timecard_id, &db);
							agency_id = GetAgencyID(sow_id, &db);
						}
					}

					if(error_description.length())
					{
					}
					else
					{
						if(timecard_id.length() && timecard_status.length() && agency_id.length())
						{
							error_description = RemoveTimecardLines(timecard_id, &db);
							if(error_description.empty())
							{
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
											MESSAGE_DEBUG("", action, "task (" + item.customer + TIMECARD_ENTRY_TITLE_SEPARATOR + item.project + TIMECARD_ENTRY_TITLE_SEPARATOR + item.task + ") doesn't assigned to SoW (" + sow_id + ")");

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
														if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_addTaskAssignment", task_assignment_id, sow_id, "", item.customer + TIMECARD_ENTRY_TITLE_SEPARATOR + item.project + TIMECARD_ENTRY_TITLE_SEPARATOR + item.task + " ( с " + sow_start_date + " по " + sow_end_date + ")", &db, &user))
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
												MESSAGE_ERROR("", action, "SoW(" + sow_id + ") doesn't allows create tasks or consistency issue");
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
								error_description = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, "timecard_id(" + timecard_id + ") or status (" + timecard_status + ") or agency_id(" + agency_id + ") is empty");
							}
						}
						else
						{
							error_description = "Таймкарта не найдена";
							MESSAGE_ERROR("", action, "timecard_id(" + timecard_id + ") or status (" + timecard_status + ") or agency_id(" + agency_id + ") is empty");
						}
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
										if(SubmitTimecard(timecard_id, &config, &db, &user))
										{
											success_description = GetTimecardsSOWTaskAssignment_Reusable_InJSONFormat(current_period_start_year + "-" + current_period_start_month + "-" + current_period_start_date, &db, &user);
										}
										else
										{
											error_description = "ошибка отправки таймкарты";
											MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + ")");
										}

/*
										C_TC_BT_Submit	submit_obj(&db, &user);

										if((error_message = submit_obj.Submit("timecard", timecard_id)).empty())
										{
											success_description = GetTimecardsSOWTaskAssignment_Reusable_InJSONFormat(current_period_start_year + "-" + current_period_start_month + "-" + current_period_start_date, &db, &user);
										}
										else
										{
											error_description = "ошибка отправки таймкарты";
											MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + ")");
										}
*/
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
			}
		}


		{
			MESSAGE_DEBUG("", action, "finish");
		}
	}

	if(action == "AJAX_getAutocompleteCustomerList")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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

	if(action == "AJAX_getDashboardPendingPayment")
	{
		MESSAGE_DEBUG("", action, "start");

		auto		template_name = "json_response.htmlt"s;
		auto		error_message = ""s;
		auto		success_message = ""s;
		auto		affected = db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";");

		if(affected)
		{
			auto sow_sql =	"SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id`=("
								"SELECT `id` FROM `company` WHERE `admin_userID`=" + quoted(user.GetID()) + 
							")";

			success_message = GetDashboardPaymentData(sow_sql, &db, &user);
		}
		else
		{
			error_message = gettext("you are not authorized");
			MESSAGE_ERROR("", action, error_message);
		}

		AJAX_ResponseTemplate(&indexPage, success_message, error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_submitBT")
	{
		MESSAGE_DEBUG("", action, "start");

		auto					template_name = "json_response.htmlt"s;
		auto					success_message = ""s;
		vector<pair<string, string>>	error_messages;
		CVars					*http_params = indexPage.GetVarsHandler();
		regex					regex1("expense_item_random_");
		regex					regex2("expense_item_doc_random_");
		C_BT					bt;
		vector<C_Expense>		expenses;
		vector<string>			expense_randoms;
		vector<string>			expense_line_randoms;

		// --- build HTTP-param arrays
		{
			auto	http_param_name = http_params->GetNameByRegex(regex1);
			while(http_param_name.length())
			{
				expense_randoms.push_back(http_params->Get(http_param_name));

				http_params->Delete(http_param_name);
				http_param_name = http_params->GetNameByRegex(regex1);
			}

		}
		{
			auto	http_param_name = http_params->GetNameByRegex(regex2);
			while(http_param_name.length())
			{
				expense_line_randoms.push_back(http_params->Get(http_param_name));

				http_params->Delete(http_param_name);
				http_param_name = http_params->GetNameByRegex(regex2);
			}
		}

		// --- craft BT object
		bt.SetConfig			(&config);
		bt.SetDB				(&db);
		bt.SetUser				(&user);
		bt.SetID				(CheckHTTPParam_Number	(http_params->Get("bt_id")));
		bt.SetSowID				(CheckHTTPParam_Number	(http_params->Get("sow_id")));
		bt.SetCustomerID		(CheckHTTPParam_Number	(http_params->Get("customer_id")));
		bt.SetDestination		(CheckHTTPParam_Text	(http_params->Get("destination")));
		bt.SetPurpose			(CheckHTTPParam_Text	(http_params->Get("bt_purpose")));
		bt.SetStartDate			(CheckHTTPParam_Date	(http_params->Get("bt_start_date")));
		bt.SetEndDate			(CheckHTTPParam_Date	(http_params->Get("bt_end_date")));
		bt.SetAction			(CheckHTTPParam_Text	(http_params->Get("action_type")));

		// --- assign expense objects to BT
		for(const auto &temp: expense_randoms)
		{
			auto expense_random = CheckHTTPParam_Number(temp);

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
				error_messages.push_back(make_pair("description"s, "некорректный параметер random у растраты"s));
				MESSAGE_ERROR("", action, "Can't convert expense_random(" + expense_random + ") to number")
			}

			if(error_messages.size()) break;
		}


		// --- check files send over HTTP, so that if some are not recognized as valid, rollback cleanly
		if(error_messages.size() == 0)
		{
			for(const auto &expense_line_random: expense_line_randoms)
			{
				if(expense_line_random == CheckHTTPParam_Number(expense_line_random))
				{
					if(expense_line_random.length())
					{
						auto	parent_random =	CheckHTTPParam_Number	(http_params->Get("expense_item_parent_random_" + expense_line_random));

						if(parent_random.length())
						{
							auto	dom_type = 	CheckHTTPParam_Text	(http_params->Get("expense_item_doc_dom_type_" + expense_line_random));

							if((dom_type == "image") || (dom_type == "input") || (dom_type == "allowance"))
							{
								if(dom_type == "image")							
								{
									auto		file_size = 0;
									auto		file_ext = ""s;

									for(auto &temp: {".jpg", ".pdf"})
									{
										if((file_size = indexPage.GetFilesHandler()->GetSize("expense_item_doc_main_field_" + expense_line_random + temp)) > 0)
										{
											file_ext = temp;
											break;
										}
									}

									if(file_size > 0)
									{
										// --- take care of an image/pdf file
										MESSAGE_DEBUG("", action, "expense_item_doc_main_field_" + expense_line_random + file_ext + " file size is " + to_string(file_size));

										auto	error_message = CheckFileFromHandler("expense_item_doc_main_field_" + expense_line_random + file_ext, "expense_line", indexPage.GetFilesHandler(), file_ext, &config);

										if(error_message.empty())
										{
										}
										else
										{
											error_messages.push_back(make_pair("description"s, error_message));
											error_messages.push_back(make_pair("file_id"s, "expense_item_doc_main_field_" + expense_line_random));
											MESSAGE_ERROR("", action, "file(" + "expense_item_doc_main_field_" + expense_line_random + file_ext + ") failed sanity check")
										}
									}
									else
									{
										MESSAGE_DEBUG("", "", "file for line item(" + expense_line_random + ") already uploaded to server");
									}

								}
							}
							else
							{
								error_messages.push_back(make_pair("description", "некорректный тип основного док-та"));
								MESSAGE_ERROR("", action, "primary doc-t wrong type(" + dom_type + ")");
							}
						}
						else
						{
							error_messages.push_back(make_pair("description", "невозможно определить принажделжность документа"));
							MESSAGE_ERROR("", action, "parent expense.id missed for expense_line.rand(" + expense_line_random + ")");
						}
					}
					else
					{
						error_messages.push_back(make_pair("description", "некорректный параметер random у док-та"));
						MESSAGE_ERROR("", action, "Can't convert expense_line_random(" + expense_line_random + ") to number")
					}
				}
				else
				{
					error_messages.push_back(make_pair("description", gettext("parameters incorrect")));
					MESSAGE_ERROR("", "", "error in parsing expense_line_random(" + expense_line_random + " != " + CheckHTTPParam_Number(expense_line_random) + ")");
				}


				if(error_messages.size()) break;
			}
		}


		// --- save files to final folders and assign lines to expenses
		// --- NOTE: such as files save to final folders, there is no place for mistake
		if(error_messages.size() == 0)
		{
			for(const auto &expense_line_random: expense_line_randoms)
			{
				C_ExpenseLine	expense_line;
				auto			parent_random =	CheckHTTPParam_Number	(http_params->Get("expense_item_parent_random_" + expense_line_random));
				auto			dom_type = 	CheckHTTPParam_Text	(http_params->Get("expense_item_doc_dom_type_" + expense_line_random));

				expense_line.SetRandom					(expense_line_random);
				expense_line.SetParentRandom			(parent_random);
				expense_line.SetID						(CheckHTTPParam_Number	(http_params->Get("expense_line_id_" + expense_line_random)));
				expense_line.SetExpenseLineTemplateID	(CheckHTTPParam_Number	(http_params->Get("expense_item_doc_id_" + expense_line_random)));
				expense_line.SetComment					(CheckHTTPParam_Text	(http_params->Get("expense_item_doc_comment_" + expense_line_random)));

				if(dom_type == "input")
					expense_line.SetValue				(CheckHTTPParam_Text	(http_params->Get("expense_item_doc_main_field_" + expense_line_random)));
				if(dom_type == "allowance")
					expense_line.SetValue				(CheckHTTPParam_Text	(http_params->Get("expense_item_doc_main_field_" + expense_line_random)));
				if(dom_type == "image")
				{
					string		path_to_file = "";
					auto		file_size = 0;
					auto		file_ext = ""s;

					for(auto &temp: {".jpg", ".pdf"})
					{
						if((file_size = indexPage.GetFilesHandler()->GetSize("expense_item_doc_main_field_" + expense_line_random + temp)) > 0)
						{
							file_ext = temp;
							break;
						}
					}

					if(file_size > 0)
					{
						// --- take care of an image/pdf file
						MESSAGE_DEBUG("", action, "expense_item_doc_main_field_" + expense_line_random + file_ext + " file size is " + to_string(file_size));
						path_to_file = SaveFileFromHandler("expense_item_doc_main_field_" + expense_line_random + file_ext, "expense_line", indexPage.GetFilesHandler(), file_ext, &config);
						expense_line.SetValue(path_to_file);

						if(path_to_file.length())
						{
							MESSAGE_DEBUG("", action, "image to expense_line(random = " + expense_line_random + ") uploaded to " + path_to_file + "");
						}
					}
					else
					{
						MESSAGE_DEBUG("", "", "file for line item(" + expense_line_random + ") already uploaded to server");
					}
				}

				if(bt.AssignExpenseLineByParentRandom(expense_line))
				{
				}
				else
				{
					error_messages.push_back(make_pair("description", "невозможно определить принажделжность документа"));
					MESSAGE_ERROR("", action, "AssignExpenseLineByParentRandom fail");
				}

				if(error_messages.size()) break;
			}

			if(error_messages.size() == 0)
			{
				auto	error_message = bt.CheckValidity();
				if(error_message.empty())
				{
					// --- update DB
					error_message = bt.SaveToDB();
					if(error_message.length())
					{
						error_messages.push_back(make_pair("description", error_message));
						MESSAGE_ERROR("", action, "fail saving BT to DB");
					}
				}
				else
				{
					error_messages.push_back(make_pair("description", error_message));
					MESSAGE_DEBUG("", action, "BT validity chack failed");
				}
			}

			{
				ostringstream	ost;

				ost.str("");
				ost << bt;
				MESSAGE_DEBUG("", action, "C_BT obj dump: " + ost.str());
			}

		}

/*			if(error_messages.size())
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
*/

		AJAX_ResponseTemplate(&indexPage, success_message, error_messages);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(action == "AJAX_removeExpense")
	{
		string			strPageToGet, strFriendsOnSinglePage;
		ostringstream	ostResult;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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
											MESSAGE_DEBUG("", action, "remove image (" + config.GetFromFile("image_folders", "expense_line") + "/" + value + ")");
											unlink((config.GetFromFile("image_folders", "expense_line") + "/" + value).c_str());
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

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
/*		if(user.GetLogin() == "Guest")
		{
			MESSAGE_DEBUG("", action, "re-login required");

			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
*/		{
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
								auto	act_id = GetValueFromDB(Get_ActIDByBTID(bt_id), &db);

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
										MESSAGE_DEBUG("", action, "remove image (" + config.GetFromFile("image_folders", "expense_line") + "/" + value + ")");
										unlink((config.GetFromFile("image_folders", "expense_line") + "/" + value).c_str());
									}
								}

								db.Query("DELETE FROM `bt_expense_lines` WHERE `bt_expense_id` IN ( "
											" SELECT `id` FROM `bt_expenses` WHERE `bt_id`=\"" + bt_id + "\" "
										");");
								db.Query("DELETE FROM `bt_expenses` WHERE `bt_id`=\"" + bt_id + "\";");
								db.Query("DELETE FROM `bt` WHERE `id`=\"" + bt_id + "\";");
								db.Query("DELETE FROM `bt_approvals` WHERE `bt_id`=\"" + bt_id + "\";");
								db.Query("DELETE FROM `invoices` WHERE `id`=(" + act_id + ");");
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
		auto			error_message = ""s;
		auto			template_name = "json_response.htmlt"s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");
		if(user.GetType() != "subcontractor")
		{
			MESSAGE_DEBUG("", action, "re-login required");
			ostResult << "{\"result\":\"error\",\"description\":\"re-login required\"}";
		}
		else
		{
			auto			agency_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			include_countries = indexPage.GetVarsHandler()->Get("include_countries") == "true";

			if(db.Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\";"))
			{
				auto	company_id = db.Get(0, "id");
				auto	company_obj = GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", &db, &user);

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
		(action == "AJAX_updateCompanyActNumber")				||
		(action == "AJAX_updateCompanyActNumberPrefix")			||
		(action == "AJAX_updateCompanyActNumberPostfix")		||
		(action == "AJAX_updateCompanyVAT")						||
		(action == "AJAX_updateCompanyVATCalculationType")		||
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
		MESSAGE_DEBUG("", action, "start");

		auto			error_message	= ""s;
		auto			success_message = ""s;

		auto			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
		auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

		if(
			new_value.length() 									||
			(action == "AJAX_updateCompanyActNumberPrefix")		||	// --- ActNumberPrefix could be empty
			(action == "AJAX_updateCompanyActNumberPostfix")		 // --- ActNumberPostfix could be empty
		)
		{
			auto	company_id = GetCompanyID(&user, &db);
			
			if(company_id.length())
			{
				if(action == "AJAX_updateCompanyTitle") 		{	id = company_id; }
				if(action == "AJAX_updateCompanyDescription")	{	id = company_id; }
				if(action == "AJAX_updateCompanyWebSite")		{	id = company_id; }
				if(action == "AJAX_updateCompanyActNumber")		{	id = company_id; }
				if(action == "AJAX_updateCompanyActNumberPrefix"){	id = company_id; }
				if(action == "AJAX_updateCompanyActNumberPostfix"){	id = company_id; }
				if(action == "AJAX_updateCompanyTIN")			{	id = company_id; }
				if(action == "AJAX_updateCompanyVAT")			{	id = company_id; }
				if(action == "AJAX_updateCompanyVATCalculationType"){ new_value = (new_value == "Y" ? "sum_by_row" : "percentage");	if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
				if(action == "AJAX_updateCompanyAccount")		{	id = company_id; }
				if(action == "AJAX_updateCompanyOGRN")			{	id = company_id; }
				if(action == "AJAX_updateCompanyKPP")			{	id = company_id; }
				if(action == "AJAX_updateCompanyLegalAddress") 	{	id = company_id; }
				if(action == "AJAX_updateCompanyMailingAddress"){	id = company_id; }
				if(action == "AJAX_updateCompanyMailingZipID") 	{	new_value = id; id = company_id; }
				if(action == "AJAX_updateCompanyLegalZipID") 	{	new_value = id; id = company_id; }
				if(action == "AJAX_updateCompanyBankID") 		{	new_value = id; id = company_id; }

/*
				error_message = isActionEntityBelongsToCompany(action, id, company_id, &db, &user);
				if(error_message.empty())
				{
					error_message = CheckNewValueByAction(action, id, "", new_value, &db, &user);
					if(error_message.empty())
					{
*/
						if(action.find("update"))
						{
							string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

							error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
							if(error_message.empty())
							{
								// ostResult << "{\"result\":\"success\"}";

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
								MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/" + new_value + ")");
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
				}
				else
				{
					MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + company_id + ")");
				}
*/
			}
			else
			{
				error_message = gettext("agency not found");
				MESSAGE_ERROR("", action, "company.id not found by user.id(" + user.GetID() + ") employment");
			}
		}
		else
		{
			error_message = gettext("field should not be empty");
			MESSAGE_DEBUG("", action, error_message);
		}

		AJAX_ResponseTemplate(&indexPage, success_message, error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	if(
		(action == "AJAX_updateSoWCustomField")	||
		(action == "AJAX_deleteSoWCustomField")
	)
	{
			MESSAGE_DEBUG("", action, "start");

			auto	error_message = ""s;
			auto	success_message = ""s;


			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(
					new_value.length()						||
					(action == "AJAX_updateSoWCustomField")	||
					(action == "AJAX_deleteSoWCustomField")
				)
				{
					error_message = isSubcontractorAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						error_message = isActionEntityBelongsToSoW(action, id, sow_id, &db, &user);
						if(error_message.empty())
						{
							error_message = CheckNewValueByAction(action, id, sow_id, new_value, &db, &user);
							if(error_message.empty())
							{
								auto		existing_value = ""s;

								if(action.find("update") != string::npos)
								{
									existing_value = GetDBValueByAction(action, id, sow_id, &db, &user);

									error_message = SetNewValueByAction(action, id, sow_id, new_value, &db, &user);
									if(error_message.empty())
									{
										// --- good2go
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/" + new_value + ")");
									}
								}
								else if(action.find("insert") != string::npos)
								{
									
								}
								else if(action.find("delete") != string::npos)
								{
									existing_value = GetDBValueByAction(action, id, sow_id, &db, &user);

									error_message = DeleteEntryByAction(action, id, &config, &db, &user);
									if(error_message.empty())
									{
										// --- good2go
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to remove item (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/" + new_value + ")");
									}
								}
								else
								{
									error_message = gettext("unsupported operation");
									MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
								}

								if(error_message.empty())
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, id, sow_id, existing_value, new_value, &db, &user))
									{
										// --- don't do anything here
									}
									else
									{
										MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "new value failed to pass validity check");
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belong to sow.id(" + sow_id + ")");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
					}
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new value in sow.id(" + sow_id + ")");
				}

				AJAX_ResponseTemplate(&indexPage, success_message, error_message);
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

/*		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
*/		{

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
						error_message = gettext("SQL syntax error");
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

	if(action == "AJAX_getAbsenceList")
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

/*		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
*/		{
			success_message = ",\"absences\":[" + 
			GetAbsenceListInJSONFormat("SELECT * FROM `absence` WHERE `company_id`=("
											"SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\" AND `type`=\"subcontractor\""
										");", &db, &user)
								+ "]";
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

	if(action == "AJAX_submitNewAbsence")
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

/*		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
*/		{
			auto	type_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("type_id"));
			auto	comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));
			auto	start_date = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("start_date"));
			auto	end_date = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("end_date"));


			if(type_id.length() && start_date.length() && end_date.length())
			{
				if(DateInPast(start_date) || DateInPast(end_date))
				{
					error_message = gettext("can't create absence in the past");
					MESSAGE_DEBUG("", action, error_message);
				}
				else
				{
					if(GetTMObject(start_date) <= GetTMObject(end_date))
					{
						auto	company_id = GetCompanyID(&user, &db);

						if(company_id.length())
						{
							auto overlap_period = GetAbsenceOverlap(company_id, start_date, end_date, &db, &user);

							if(overlap_period.empty())
							{
								auto	new_id = db.InsertQuery("INSERT INTO `absence` ("
																	"`company_id`,"
																	"`absence_type_id`,"
																	"`start_date`,"
																	"`end_date`,"
																	"`comment`,"
																	"`request_date`"
																") VALUES ("
																	+ quoted(company_id) + ","
																	+ quoted(type_id) + ","
																	"STR_TO_DATE(\"" + start_date + "\", '%d/%m/%Y'),"
																	"STR_TO_DATE(\"" + end_date + "\", '%d/%m/%Y'),"
																	+ quoted(comment) + ","
																	"NOW()"
																")");
								if(new_id)
								{
									success_message = ",\"absences\":[" + 
									GetAbsenceListInJSONFormat("SELECT * FROM `absence` WHERE `company_id`=("
																	"SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user.GetID() + "\" AND `type`=\"subcontractor\""
																");", &db, &user)
														+ "]";

									// --- notify all agencies about absence
									{
										vector<string>	agency_list = {};
										auto			affected = db.Query("SELECT DISTINCT( `agency_company_id`) FROM `contracts_sow` WHERE `subcontractor_company_id`=" + quoted(company_id) + ";");
										for(auto i = 0; i < affected; ++i)
											agency_list.push_back(db.Get(i, 0));

										for(auto &agency_company_id : agency_list)
										{
											auto	temp_error_message = NotifyAgencyAboutChanges(agency_company_id, to_string(NOTIFICATION_SUBC_BUILT_ABSENCE_REQUEST), to_string(new_id), &db, &user);

											if(temp_error_message.length()) MESSAGE_ERROR("", action, "fail to notify agency company.id(" + agency_company_id + ")")
										}
									}

								}
								else
								{
									error_message = gettext("SQL syntax error");
									MESSAGE_ERROR("", action, error_message);
								}
							}
							else
							{
								error_message = gettext("absence overlaps") + " "s + gettext("with") + " " + overlap_period;
								MESSAGE_DEBUG("", action, error_message);
							}
						}
						else
						{
							error_message = gettext("company not found");
							MESSAGE_ERROR("", action, error_message);
						}
					}
					else
					{
						error_message = gettext("period start have to precede period end");
						MESSAGE_ERROR("", action, error_message);
					}					
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message)
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

	if(	(action == "AJAX_updateAbsenceStartDate") || (action == "AJAX_updateAbsenceEndDate"))
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

/*		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
*/		{
			auto	id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto	value = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			if(id.length() && value.length())
			{
				auto	company_id = GetCompanyID(&user, &db);

				if(company_id.length())
				{

					if(db.Query("SELECT DATE_FORMAT(`start_date`, \"%d/%m/%Y\") as \"start_date\", DATE_FORMAT(`end_date`, \"%d/%m/%Y\") as \"end_date\" FROM `absence` WHERE `id`=\"" + id + "\" and `company_id`=" + quoted(company_id) + ";"))
					{
						auto	start_date = db.Get(0, "start_date");
						auto	end_date = db.Get(0, "end_date");

						if(DateInPast(value) || DateInPast(start_date) || DateInPast(end_date))
						{
							error_message = gettext("can't change dates in past");
							MESSAGE_DEBUG("", action, error_message);
						}
						else
						{
							if(action == "AJAX_updateAbsenceStartDate")	start_date = value;
							if(action == "AJAX_updateAbsenceEndDate")	end_date = value;

							if(GetTMObject(start_date) <= GetTMObject(end_date))
							{
									auto overlap_period = GetAbsenceOverlap(company_id, start_date, end_date, &db, &user, id);

									if(overlap_period.empty())
									{
										db.Query("UPDATE `absence` SET `start_date`=STR_TO_DATE(\"" + start_date + "\", '%d/%m/%Y'), `end_date`=STR_TO_DATE(\"" + end_date + "\", '%d/%m/%Y') WHERE `id`=" + quoted(id) + ";");

										if(db.isError())
										{
											error_message = gettext("SQL syntax error");
											MESSAGE_ERROR("", action, error_message);
										}
										else
										{
											// --- success

											// --- notify all agencies about absence
											{
												vector<string>	agency_list = {};
												auto			affected = db.Query("SELECT DISTINCT( `agency_company_id`) FROM `contracts_sow` WHERE `subcontractor_company_id`=" + quoted(company_id) + ";");
												for(auto i = 0; i < affected; ++i)
													agency_list.push_back(db.Get(i, 0));

												for(auto &agency_company_id : agency_list)
												{
													auto	temp_error_message = NotifyAgencyAboutChanges(agency_company_id, to_string(NOTIFICATION_SUBC_CHANGED_ABSENCE_REQUEST), id, &db, &user);

													if(temp_error_message.length()) MESSAGE_ERROR("", action, "fail to notify agency company.id(" + agency_company_id + ")")
												}
											}
										}
									}
									else
									{
										error_message = gettext("absence overlaps") + " "s + gettext("with") + " " + overlap_period;
										MESSAGE_DEBUG("", action, error_message);
									}
							}
							else
							{
								error_message = gettext("period start have to precede period end");
								MESSAGE_ERROR("", action, error_message);
							}
						}
					}
					else
					{
						error_message = gettext("absence not found");
						MESSAGE_ERROR("", action, error_message);
					}
				}
				else
				{
					error_message = gettext("company not found");
					MESSAGE_ERROR("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message)
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

	if(action == "AJAX_updateAbsenceComment")
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

/*		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
*/		{
			auto	id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto	value = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			if(id.length() && value.length())
			{
				auto	company_id = GetCompanyID(&user, &db);

				if(company_id.length())
				{

					db.Query("UPDATE `absence` SET `comment`=" + quoted(value) + " WHERE `id`=" + quoted(id) + " AND `company_id`=" + quoted(company_id) + ";");

					if(db.isError())
					{
						error_message = gettext("SQL syntax error");
						MESSAGE_ERROR("", action, error_message);
					}
					else
					{
						// --- success
					}

				}
				else
				{
					error_message = gettext("company not found");
					MESSAGE_ERROR("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message)
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

	if(action == "AJAX_deleteAbsence")
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

/*		if(user.GetLogin() == "Guest")
		{
			error_message = "re-login required";
			MESSAGE_DEBUG("", action, error_message);
		}
		else
*/		{
			auto	id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

			if(id.length())
			{
				auto	company_id = GetCompanyID(&user, &db);

				if(company_id.length())
				{
					if(db.Query("SELECT * FROM `absence` WHERE `id`=" + quoted(id) + " AND `company_id`=" + quoted(company_id) + ";"))
					{
						auto	start_date = db.Get(0, "start_date");
						auto	end_date = db.Get(0, "end_date");

						if(DateInPast(start_date) || DateInPast(end_date))
						{
							error_message = gettext("can't remove absence in the past");
							MESSAGE_DEBUG("", action, error_message);
						}
						else
						{
							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId` IN (112, 113) AND `actionId`=" + quoted(id) + ";");
							db.Query("DELETE FROM `absence` WHERE `id`=" + quoted(id) + " AND `company_id`=" + quoted(company_id) + ";");

							if(db.isError())
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, error_message);
							}
							else
							{
								// --- success
							}
						}
					}
					else
					{
						error_message = gettext("you are not authorized");
						MESSAGE_ERROR("", action, error_message);
					}

				}
				else
				{
					error_message = gettext("company not found");
					MESSAGE_ERROR("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message)
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

	if(action == "AJAX_enrollSmartway")
	{
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;

		MESSAGE_DEBUG("", action, "start");

		ostResult.str("");

		{
			C_Smartway		smartway(&config, &db, &user);

			// error_message = smartway.ping();
			if(error_message.empty())
			{
				error_message = smartway.employees_create(user.GetID());
				if(error_message.empty())
				{
					db.Query("UPDATE `users` SET `smartway_employee_id`=" + quoted(smartway.GetEmployeeID()) + " WHERE `id`=" + quoted(user.GetID()) + ";");
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to save employee");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "fail to reach server");
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

	if(action == "AJAX_findFlights")
	{
		MESSAGE_DEBUG("", action, "start");

		ostringstream			ostResult;
		auto					template_name = "json_response.htmlt"s;
		auto					error_message = ""s;
		auto					success_message = ""s;
		vector<C_Flight_Route>	flight_routes;
		auto					idx = 0;
		auto					sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

		if(sow_id.length())
		{
			while(CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("from_" + to_string(idx))).length())
			{
				C_Flight_Route	temp;
				
				temp.from = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("from_" + to_string(idx)));
				temp.to = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("to_" + to_string(idx)));
				temp.date = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("date_takeoff_" + to_string(idx)));

				++idx;
				
				flight_routes.push_back(temp);
			}

			ostResult.str("");

			if(flight_routes.size())
			{
				C_Smartway		smartway(&config, &db, &user);

				// error_message = smartway.ping();
				if(error_message.empty())
				{
					smartway.SetSoW(sow_id); // --- requires to apply filter
					error_message = smartway.airline_search(flight_routes);
					if(error_message.empty())
					{
						success_message = ",\"flights\":[" + smartway.GetFlightsJSON() + "]";
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to search flights");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to reach server");
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message);
			}
		}
		else
		{
			error_message = gettext("mandatory parameter missed");
			MESSAGE_ERROR("", action, error_message);
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

	if(action == "AJAX_purchaseAirTicket")
	{
		MESSAGE_DEBUG("", action, "start");

		ostringstream			ostResult;
		auto					template_name = "json_response.htmlt"s;
		auto					error_message = ""s;
		auto					success_message	= ""s;
		auto					passport_type	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("passport_type"));
		auto					search_id		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("search_id"));
		auto					trip_id			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("trip_id"));
		auto					fare_id			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("fare_id"));
		auto					sow_id			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

		ostResult.str("");

		if(search_id.length() && trip_id.length() && fare_id.length() && sow_id.length())
		{
			if(isUserAssignedToSoW(user.GetID(), sow_id, &db))
			{
				if(db.Query("SELECT `id` FROM `airline_bookings` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `search_id`=\"" + search_id + "\" AND `search_trip_id`=\"" + trip_id + "\" AND `search_fare_id`=\"" + fare_id + "\" AND `status`=\"done\";") == 0)
				{
					C_Smartway		smartway(&config, &db, &user);

					smartway.SetSoW(sow_id);
					error_message = smartway.airline_book(passport_type, search_id, trip_id, fare_id);
					if(error_message.empty())
					{
						auto	id = db.InsertQuery("INSERT INTO `airline_bookings` "s
													"(`contract_sow_id`, `search_id`, `search_trip_id`, `search_fare_id`,`passport_type`, `eventTimestamp`) "
													"VALUES "
													"("	+
														"\"" + sow_id + "\", "
														"\"" + search_id + "\", "
														"\"" + trip_id + "\", "
														"\"" + fare_id + "\", "
														"\"" + passport_type + "\", "
														"UNIX_TIMESTAMP());");

						if(id)
						{
							do
							{
								error_message = smartway.airline_result(smartway.GetAirlineBookingResultID());
								if(error_message.empty())
								{
									db.Query("UPDATE `airline_bookings` SET `status`=\"" + smartway.GetAirlineResultStatus() + "\" WHERE `id`=\"" + to_string(id) + "\";");
									if(db.isError())
									{
										error_message = gettext("SQL syntax error");
										MESSAGE_DEBUG("", "", error_message);
									}
									else
									{
										if(smartway.GetAirlineResultStatus() == "done")
										{
											db.Query("UPDATE `airline_bookings` SET `trip_id`=\"" + smartway.GetAirlineResultTripID() + "\" WHERE `id`=\"" + to_string(id) + "\";");
											if(db.isError())
											{
												error_message = gettext("SQL syntax error");
												MESSAGE_DEBUG("", "", error_message);
											}
											else
											{
											}
										}
										else if(smartway.GetAirlineResultStatus() == "pending")
										{
										}
										else if(smartway.GetAirlineResultStatus() == "failed")
										{
											error_message = smartway.GetAirlineResultError();
											MESSAGE_DEBUG("", "", error_message);
										}
										else
										{
											error_message = "Smartway "s + gettext("unknown method");
											MESSAGE_DEBUG("", "", error_message);
										}
									}
								}
								else
								{
									MESSAGE_ERROR("", "", "fail to get booking status");
								}

							} while(error_message.empty() && (smartway.GetAirlineResultStatus() == "pending"));

							if(error_message.empty())
							{
								error_message = smartway.trip_item_voucher(smartway.GetAirlineResultTripID());
								if(error_message.empty())
								{
									db.Query("UPDATE `airline_bookings` SET "
																				"`voucher_filename`=\"" + smartway.GetVoucherFile() + "\", "
																				"`destination`=\"" + smartway.GetPurchasedServiceDestination() + "\", "
																				"`checkin`=\"" + smartway.GetPurchasedServiceCheckin() + "\", "
																				"`checkout`=\"" + smartway.GetPurchasedServiceCheckout() + "\", "
																				"`book_date`=\"" + smartway.GetPurchasedServiceBookDate() + "\", "
																				"`amount`=\"" + smartway.GetPurchasedServiceAmount() + "\" "
																				"WHERE `id`=\"" + to_string(id) + "\";");
									if(db.isError())
									{
										error_message = gettext("SQL syntax error");
										MESSAGE_DEBUG("", "", error_message);
									}
									else
									{
										// --- good result
									}
								}
								else
								{
									MESSAGE_ERROR("", action, error_message);
								}
							}
							else
							{
								MESSAGE_ERROR("", action, error_message);
							}

						}
						else
						{
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "VERY IMPORTANT: fail to insert to DB, check Smartway web-site, ticket could be purchased");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to purchase air ticket (" + error_message + ")");
					}
				}
				else
				{
					error_message = gettext("this booking already exists");
					MESSAGE_DEBUG("", "", error_message);
				}
			}
			else
			{
				error_message = gettext("You are not authorized");
				MESSAGE_ERROR("", action, "SoW(" + sow_id + ") is not assigned to the user(" + user.GetID() + ") company");
			}
		}
		else
		{
			error_message = gettext("mandatory parameter missed");
			MESSAGE_ERROR("", action, error_message);
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

	if(action == "AJAX_getVoucher")
	{
		MESSAGE_DEBUG("", action, "start");

		auto			voucher_id = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("voucher_id"));
		ostringstream	ostResult;
		auto			template_name = "json_response.htmlt"s;
		auto			error_message = ""s;
		auto			success_message = ""s;


		if(voucher_id.length())
		{
			C_Smartway		smartway(&config, &db, &user);
			error_message = smartway.trip_item_voucher(voucher_id);
		}
		else
		{
			error_message = gettext("mandatory parameter missed");
			MESSAGE_ERROR("", action, error_message);
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

	if(action == "AJAX_getBTWorkerData")
	{
		MESSAGE_DEBUG("", action, "start");

		auto			error_message = ""s;
		auto			success_message = ""s;
		auto			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
		auto			agency_company_id = GetAgencyID(sow_id, &db);

		if(agency_company_id.length())
		{
			success_message = "\"allowances\": ["  + GetBTAllowanceInJSONFormat("SELECT * FROM `bt_allowance` WHERE `agency_company_id`=" + quoted(agency_company_id) + ";", &db, &user) + "]";
		}
		else
		{
			error_message = gettext("agency not found");
			MESSAGE_ERROR("", action, error_message);
		}

		AJAX_ResponseTemplate(&indexPage, success_message, error_message);

		MESSAGE_DEBUG("", action, "finish");
	}

	MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");

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
