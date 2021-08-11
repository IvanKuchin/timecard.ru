#include "agency.h"

int main(void)
{
	CStatistics		appStat;  // --- CStatistics must be first statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	c_config		config(CONFIG_DIR);
	CUser			user;
	CMysql			db;
	string			action = "";

	MESSAGE_DEBUG("", action, __FILE__);

	signal(SIGSEGV, crash_handler);

	// --- randomization
	{
		struct timeval	tv;
		gettimeofday(&tv, NULL);
		srand(tv.tv_sec * tv.tv_usec * 100000);  /* Flawfinder: ignore */
	}

	try
	{

		indexPage.ParseURL();
		indexPage.AddCookie("lng", "ru", nullptr, "", "/");

		if(!indexPage.SetTemplate("index.htmlt"))
		{
			MESSAGE_ERROR("", action, "template file was missing");
			throw CException("Template file was missing");
		}

		if(db.Connect(&config) < 0)
		{
			MESSAGE_ERROR("", action, "Can not connect to mysql database");
			throw CException("MySql connection");
		}

		indexPage.SetDB(&db);


		action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));

		MESSAGE_DEBUG("", "", "action taken from HTTP is " + action);

		// --- generate common parts
		{
			// --- it has to be run before session, because session relay upon Apache environment variable
			if(RegisterInitialVariables(&indexPage, &db, &user))
			{
			}
			else
			{
				MESSAGE_ERROR("", action, "RegisterInitialVariables failed, throwing exception");
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

			{
				string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getApprovalsList")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name = "json_response.htmlt";
				auto			object = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("object"));;
				auto			filter = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter"));;

				if((object == "timecard") || (object == "bt"))
				{
					if((user.GetType() == "agency") || (user.GetType() == "approver"))
					{
						string	temp = GetObjectsSOW_Reusable_InJSONFormat(object, filter, &db, &user);

						if(temp.length())
						{
							ostResult	<< "{"
										<< "\"result\":\"success\","
										<< temp
										<< "}";
						}
						else
						{
							MESSAGE_ERROR("", action, "error get reusable data");
							ostResult << "{\"result\":\"error\",\"description\":\"Ошибка получения данных\"}";
						}

					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") neither approver, nor agency");
						ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны согласовывать\"}";
					}
				}
				else
				{
					ostResult << "{\"result\":\"error\",\"description\":\"Неизвестный тип обьекта\"}";
					MESSAGE_ERROR("", action, "unknown object type (" + object + ")");
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}


			MESSAGE_DEBUG("", action, "finish");
		}

/*
		if(action == "AJAX_getTimecardList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			string			template_name = "json_response.htmlt";
			int				affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

			if(affected)
			{
				string		companies_list= "";

				for(auto i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, 0);
				}
				ostResult << "{"
								"\"result\":\"success\","
								"\"sow\":[" << GetSOWInJSONFormat(
										"SELECT * FROM `contracts_sow` WHERE "
											"`agency_company_id` IN (" + companies_list + ") "
										";", &db, &user, false, false, false, true) << "],"
								"\"timecards\":[" << GetTimecardsInJSONFormat(
										"SELECT * FROM `timecards` WHERE "
											"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (" + companies_list + "))"
										";", &db, &user) << "],"
								"\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat("SELECT * FROM `holiday_calendar` WHERE `agency_company_id` IN (SELECT `agency_company_id` FROM `contracts_sow` WHERE `agency_company_id` IN (" + companies_list + "));", &db, &user) + "]"
							"}";
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
				ostResult << "{\"result\":\"error\",\"description\":\"Вы не создали компанию\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name))
			{
				MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}
*/
		if(action == "AJAX_getTimecardList")
		{
			MESSAGE_DEBUG("", action, "start");

			auto	error_message			= ""s;
			auto	success_message			= ""s;
			auto	filter_sow_date			= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("date"));
			auto	filter_not_sow_status	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter_not_sow_status"));
			auto	filter_sow_status		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter_sow_status"));
			auto	limit_page				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("page"));
			auto	companies_id			= GetValuesFromDB(Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()), &db);

			if(companies_id.size())
			{
				success_message = GetTimecardList("`agency_company_id` IN (" + join(companies_id, ",") + ")", filter_sow_date, filter_sow_status, filter_not_sow_status, limit_page, &db, &user);
			}
			else
			{
				error_message = gettext("you are not authorized");
				MESSAGE_ERROR("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
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
			auto	companies_id			= GetValuesFromDB(Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()), &db);

			if(companies_id.size())
			{
				success_message = GetBTList("`agency_company_id` IN (" + join(companies_id, ",") + ")", filter_sow_date, filter_sow_status, filter_not_sow_status, limit_page, isExtended, &db, &user);
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
					if(user.GetType() == "agency")
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + "))"))
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
					else if(user.GetType() == "approver")
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (" + Get_SoWIDsByTimecardApproverUserID_sqlquery(user.GetID()) + ")"))
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
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an approver(" + user.GetType() + ")");
						ostResult << "{\"result\":\"error\",\"description\":\"Вам не назначена роль согласования\"}";
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


			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_approveTimecard")
		{
			MESSAGE_DEBUG("", action, "start");

			{
				auto			timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
				auto			comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));
				auto			success_message = ""s;
				auto			error_message = ""s;

				if(timecard_id.length())
				{
					{
						// --- check ability to see this timecard
// TODO: redesign this part due to several approvers could be returned
/*
						if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
										"`approver_user_id`=\"" + user.GetID() + "\" "
										"AND "
										"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
										";"))
*/
						auto	approver_ids = GetValuesFromDB(	Get_ApproverIDsByTimecardID_sqlquery(timecard_id) +
																" AND "
																"`approver_user_id`=\"" + user.GetID() + "\" "
																";", &db);
						if(approver_ids.size())
						{
							if(db.Query("SELECT `status`, `submit_date` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								auto	timecard_state = db.Get(0, "status");
								auto	timecard_submit_date = db.Get(0, "submit_date");

								if(timecard_state == "submit")
								{
									{
										if(db.Query("SELECT `id` FROM `timecard_approvals` WHERE `approver_id` IN (" + join(approver_ids) + ") AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";"))
										{
											db.Query("UPDATE `timecard_approvals` SET `decision`=\"approved\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id` IN (" + join(approver_ids) + ") AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";");
											if(!db.isError())
											{
												if(SubmitTimecard(timecard_id, &config, &db, &user))
												{
													success_message = GetObjectsSOW_Reusable_InJSONFormat("timecard", "submit", &db, &user);

													if(success_message.length())
													{
													}
													else
													{
														error_message = gettext("SQL syntax error");
														MESSAGE_ERROR("", action, "error getting info about pending timecards");
													}
												}
												else
												{
													error_message = gettext("fail to submit timecard");
													MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + "): ");
												}
											}
											else
											{
												error_message = gettext("SQL syntax error");
												MESSAGE_ERROR("", action, "fail to update timecard_approvals table with timecard_id(" + timecard_id + ")");
											}
										}
										else
										{
											// --- this approver already approved this bt
											error_message = gettext("your approval is not required");
											MESSAGE_ERROR("", action, "approver.ids(" + join(approver_ids) + ") already approved timecard.id(" + timecard_id + ") at timestamp(" + db.Get(0, "eventTimestamp") + ")");
										}
										
									}
	/*								else
									{
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an timecard.id(" + timecard_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать таймкарту у данного договора\"}";
									}
	*/
								}
								else
								{
									error_message = gettext("your approval is not required");
									MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") have to be in submit state to be approved/rejected, but it is in \"" + timecard_state + "\" state");
								}
							}
							else
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, "issue get info about timecard.id(" + timecard_id + ") from DB");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
						}
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, "parameter timecard_id is empty");
				}


				AJAX_ResponseTemplate(&indexPage, success_message, error_message);
			}


			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_rejectTimecard")
		{
			MESSAGE_DEBUG("", action, "start");

			{
				auto			timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
				auto			comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));
				auto			success_message = ""s;
				auto			error_message = ""s;

				if(timecard_id.length())
				{
					if(comment.empty())
					{
						error_message = gettext("Reject reason is empty");
						MESSAGE_ERROR("", action, "reject reason is empty");
					}
					else
					{
						// --- check ability to see this timecard
/*						if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
										"`approver_user_id`=\"" + user.GetID() + "\" "
										"AND "
										"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
										";"))
*/
						auto	approver_ids = GetValuesFromDB(	Get_ApproverIDsByTimecardID_sqlquery(timecard_id) +
																" AND "
																"`approver_user_id`=\"" + user.GetID() + "\" "
																";", &db);
						if(approver_ids.size())
						{
							if(db.Query("SELECT `status`, `submit_date` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								auto	timecard_state = db.Get(0, "status");
								auto	timecard_submit_date = db.Get(0, "submit_date");

								if(timecard_state == "submit")
								{
									{
										if(db.Query("SELECT `id` FROM `timecard_approvals` WHERE `approver_id` IN (" + join(approver_ids) + ") AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";"))
										{
											db.Query("UPDATE `timecard_approvals` SET `decision`=\"rejected\",`comment`=" + quoted(comment) + ", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id` IN (" + join(approver_ids) + ") AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";");
											if(!db.isError())
											{
												if(SubmitTimecard(timecard_id, &config, &db, &user))
												{
													success_message = GetObjectsSOW_Reusable_InJSONFormat("timecard", "submit", &db, &user);

													if(success_message.length())
													{
													}
													else
													{
														error_message = gettext("SQL syntax error");
														MESSAGE_ERROR("", action, "error getting info about pending timecards");
													}
												}
												else
												{
													error_message = gettext("fail to submit timecard");
													MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + ")");
												}
											}
											else
											{
												error_message = gettext("SQL syntax error");
												MESSAGE_ERROR("", action, "fail to update timecard_id(" + timecard_id + "): " + db.GetErrorMessage());
											}
										}
										else
										{
											error_message = gettext("your approval is not required");
											MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") not in pending state on user.id(" + user.GetID() + ")");
										}
									}
								}
								else
								{
									error_message = gettext("your approval is not required");
									MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") have to be in submit state to be approved/rejected, but it is in \"" + timecard_state + "\" state");
								}
							}
							else
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, "issue get info about timecard.id(" + timecard_id + ") from DB");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
						}
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, "parameter timecard_id is empty");
				}

				AJAX_ResponseTemplate(&indexPage, success_message, error_message);
			}


			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_rejectBT")
		{
			MESSAGE_DEBUG("", action, "start");

			{
				auto	bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				auto	comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));
				auto	success_message = ""s;
				auto	error_message = ""s;

				if(bt_id.length())
				{
					if(comment.empty())
					{
						error_message = gettext("Reject reason is empty");
						MESSAGE_DEBUG("", action, error_message);
					}
					else
					{
						if((user.GetType() == "approver") || (user.GetType() == "agency"))
						{
							// --- check ability to see this bt
							auto	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
							if(error_string.empty())
							{
								if(db.Query("SELECT `status` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
								{
									auto	bt_state = db.Get(0, "status");

									if(bt_state == "submit")
									{
										auto	approver_id = GetValueFromDB("SELECT `id` FROM `bt_approvers` WHERE "
																			"`approver_user_id`=\"" + user.GetID() + "\" "
																			"AND "
																			"`contract_psow_id`=(" + Get_PSoWIDByBTID_sqlquery(bt_id) + ")"
																			";"
																			, &db);
										if(approver_id.length())
										{
											if(db.Query("SELECT `id` FROM `bt_approvals` WHERE `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";"))
											{
												db.Query("UPDATE `bt_approvals` SET `decision`=\"rejected\",`comment`=" + quoted(comment) + ", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";");
												if(!db.isError())
												{
													if(SubmitBT(bt_id, &config, &db, &user))
													{
														success_message = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

														if(success_message.length())
														{
														}
														else
														{
															error_message = gettext("SQL syntax error");
															MESSAGE_ERROR("", action, "error getting info about pending bts");
														}
													}
													else
													{
														error_message = gettext("fail to submit bt");
														MESSAGE_ERROR("", action, error_message + "("s + bt_id + ")");
													}
												}
												else
												{
													error_message = gettext("SQL syntax error");
													MESSAGE_ERROR("", action, "fail to update bt_id(" + bt_id + "): " + db.GetErrorMessage());
												}
											}
											else
											{
												error_message = gettext("your approval is not required");
												MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") not in pending state on user.id(" + user.GetID() + ")");
											}

										}
										else
										{
											error_message = gettext("your approval is not required");
											MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
										}

									}
									else
									{
										error_message = gettext("your approval is not required");
										MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
									}
								}
								else
								{
									error_message = gettext("SQL syntax error");
									MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
								}
							}
							else
							{
								error_message = gettext("You are not authorized");
								MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
						}
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
				}

				AJAX_ResponseTemplate(&indexPage, success_message, error_message);
			}

			MESSAGE_DEBUG("", action, "finish");
		}


		if(action == "AJAX_approveBT")
		{
			MESSAGE_DEBUG("", action, "start");

			{
				auto	bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				auto	comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));
				auto	success_message = ""s;
				auto	error_message = ""s;

				if(bt_id.length())
				{
					if((user.GetType() == "approver") || (user.GetType() == "agency"))
					{
						// --- check ability to see this bt
						auto	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
						if(error_string.empty())
						{
							if(db.Query("SELECT `status` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
							{
								auto	bt_state = db.Get(0, "status");

								if(bt_state == "submit")
								{

									auto	approver_id = GetValueFromDB("SELECT `id` FROM `bt_approvers` WHERE "
																		"`approver_user_id`=\"" + user.GetID() + "\" "
																		"AND "
																		"`contract_psow_id`=(" + Get_PSoWIDByBTID_sqlquery(bt_id) + ")"
																		";"
																		, &db);
									if(approver_id.length())
									{
										if(db.Query("SELECT `id` FROM `bt_approvals` WHERE `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";"))
										{
											db.Query("UPDATE `bt_approvals` SET `decision`=\"approved\",`comment`=" + quoted(comment) + ", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";");
											if(!db.isError())
											{
												if(SubmitBT(bt_id, &config, &db, &user))
												{
													success_message = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

													if(success_message.length())
													{
													}
													else
													{
														error_message = gettext("SQL syntax error");
														MESSAGE_ERROR("", action, "error getting info about pending bts");
													}
												}
												else
												{
													error_message = gettext("fail to submit bt");
													MESSAGE_ERROR("", action, "fail to submit bt_id(" + bt_id + ")");
												}
											}
											else
											{
												error_message = gettext("SQL syntax error");
												MESSAGE_ERROR("", action, "fail to update bt_id(" + bt_id + "): " + db.GetErrorMessage());
											}
										}
										else
										{
											error_message = gettext("your approval is not required");
											MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") not in pending state on user.id(" + user.GetID() + ")");
										}
									}
									else
									{
										error_message = gettext("your approval is not required");
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
									}

								}
								else
								{
									error_message = gettext("your approval is not required");
									MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
								}
							}
							else
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
						}
					}
					else
					{
						error_message = gettext("You are not authorized");
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
				}


				AJAX_ResponseTemplate(&indexPage, success_message, error_message);
			}


			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getDashboardPendingData")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_pending_timecards\":" << GetNumberOfTimecardsInPendingState(&db, &user) << ","
										"\"number_of_pending_bt\":" << GetNumberOfBTInPendingState(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}


			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getDashboardPendingPayment")
		{
			MESSAGE_DEBUG("", action, "start");

			auto		template_name = "json_response.htmlt"s;
			auto		error_message = ""s;
			auto		success_message = ""s;
			auto		affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

			if(affected)
			{
				auto sow_sql =	"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=("
									"SELECT `company_id` FROM `company_employees` WHERE `user_id`=" + quoted(user.GetID()) + 
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

		if(action == "AJAX_getDashboardSubmitTimecardsThisMonth")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_submit_timecards\":" << GetNumberOfApprovedTimecardsThisMonth(&db, &user) << ","
										"\"total_number_of_sow\":" << GetNumberOfSoWActiveThisMonth(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getDashboardSubmitTimecardsLastMonth")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_submit_timecards\":" << GetNumberOfApprovedTimecardsLastMonth(&db, &user) << ","
										"\"total_number_of_sow\":" << GetNumberOfSoWActiveLastMonth(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getDashboardSubmitTimecardsThisWeek")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_submit_timecards\":" << GetNumberOfApprovedTimecardsThisWeek(&db, &user) << ","
										"\"total_number_of_sow\":" << GetNumberOfSoWActiveThisWeek(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getDashboardSubmitTimecardsLastWeek")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_submit_timecards\":" << GetNumberOfApprovedTimecardsLastWeek(&db, &user) << ","
										"\"total_number_of_sow\":" << GetNumberOfSoWActiveLastWeek(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getSoWList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;

				bool			include_tasks 			= indexPage.GetVarsHandler()->Get("include_tasks") == "true";
				bool			include_bt 				= indexPage.GetVarsHandler()->Get("include_bt") == "true";
				bool			include_cost_centers 	= indexPage.GetVarsHandler()->Get("include_cost_centers") == "true";
				auto			sow_id 					= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				auto			date					= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("date"));
				auto			date_filter				= date.length() ? Get_SoWDateFilter_sqlquery(PrintSQLDate(GetTMObject(date))) + " AND " : "";

				if(sow_id.length())	sow_id = CheckHTTPParam_Number(sow_id);

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ");"))
					{
						auto		include_subc_company	= true;
						auto		agency_id				= db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"sow\":[" << GetSOWInJSONFormat(
												"SELECT * FROM `contracts_sow` WHERE "
													+ (sow_id.length() ? string("`id`=\"" + sow_id + "\" AND ") : "")
													+ date_filter + 
													"`agency_company_id`=\"" + agency_id + "\" "
												";", &db, &user, include_tasks, include_bt, include_cost_centers, include_subc_company) << "]";

						if(include_tasks)
							ostResult <<	","
											"\"task_assignments\":[" << GetTimecardTaskAssignmentInJSONFormat(
													"SELECT * FROM `timecard_task_assignment` WHERE "
														+ (sow_id.length() ? string("`contract_sow_id`=\"" + sow_id + "\" AND ") : "") +
														"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
															"`agency_company_id`=\"" + agency_id + "\" "
													");", &db, &user) << "]";
						if(include_bt)
							ostResult <<	","
											"\"bt_expense_assignments\":[" << GetBTExpenseAssignmentInJSONFormat(
													"SELECT * FROM `bt_sow_assignment` WHERE "
														+ (sow_id.length() ? string("`sow_id`=\"" + sow_id + "\" AND ") : "") +
														"`sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
															"`agency_company_id`=\"" + agency_id + "\" "
													");", &db, &user) << "]";
						ostResult << "}";
					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an agency employee");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getBTExpenseTemplates")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			sow_id 			= indexPage.GetVarsHandler()->Get("sow_id");

				if(sow_id.length())	sow_id = CheckHTTPParam_Number(sow_id);

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ");"))
					{
						string		agency_id = db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"bt_expense_templates\":[" << GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `agency_company_id`=\"" + agency_id + "\";", &db, &user) << "]";
						ostResult << "}";
					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an agency employee");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(
			(action == "AJAX_updateSoWNumber")					||
			(action == "AJAX_updateSoWAct")						||
			(action == "AJAX_updateSoWPosition")				||
			(action == "AJAX_updateSoWPaymentPeriodService")	||
			(action == "AJAX_updateSoWPaymentPeriodBT")			||
			(action == "AJAX_updateSoWDayRate")					||
			(action == "AJAX_updateSoWWorkingHoursPerDay")		||
			(action == "AJAX_updateSoWSignDate")				||
			(action == "AJAX_updateSoWStartDate")				||
			(action == "AJAX_updateSoWEndDate")					||
			(action == "AJAX_updateSoWCustomField")				||
			(action == "AJAX_deleteSoWCustomField")				||
			(action == "AJAX_updatePeriodStart")				||
			(action == "AJAX_updatePeriodEnd")					||
			(action == "AJAX_updateSubcontractorCreateTasks")
		)
		{
			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;

			ostResult.str("");
			{
				auto			template_name	= "json_response.htmlt"s;
				auto			error_message	= ""s;

				auto			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				auto			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(
					new_value.length()						||
					(action == "AJAX_updateSoWCustomField")	||
					(action == "AJAX_deleteSoWCustomField")
				)
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
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
										ostResult << "{\"result\":\"success\"}";
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + new_value + "])");
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
										ostResult << "{\"result\":\"success\"}";
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to remove item (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + new_value + "])");
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

		if(
			(action == "AJAX_updatePSoWNumber")					||
			(action == "AJAX_updatePSoWAct")					||
			(action == "AJAX_updatePSoWPosition")				||
			(action == "AJAX_updatePSoWDayRate")				||
			(action == "AJAX_updatePSoWWorkingHoursPerDay")		||
			(action == "AJAX_updatePSoWBTMarkup")				||
			(action == "AJAX_updatePSoWBTMarkupType")			||
			(action == "AJAX_updatePSoWSignDate")				||
			(action == "AJAX_updatePSoWStartDate")				||
			(action == "AJAX_updatePSoWEndDate")				||
			(action == "AJAX_updatePSoWCustomField")			||
			(action == "AJAX_deletePSoWCustomField")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;

				auto			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				auto			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(	
					new_value.length()							||
					(action == "AJAX_updatePSoWCustomField")	||
					(action == "AJAX_deletePSoWCustomField")
				)
				{
					error_message = isAgencyEmployeeAllowedToChangePSoW(sow_id, &db, &user);
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
										ostResult << "{\"result\":\"success\"}";
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + new_value + "])");
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
										ostResult << "{\"result\":\"success\"}";
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to remove item (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + new_value + "])");
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

		if(
			(action == "AJAX_updateCustomerTitle")					||
			(action == "AJAX_updateProjectTitle")					||
			(action == "AJAX_updateTaskTitle")						|| 
			(action == "AJAX_updateCompanyTitle")					||
			(action == "AJAX_updateCompanyDescription")				||
			(action == "AJAX_updateCompanyWebSite")					||
			(action == "AJAX_updateCompanyActNumber")				||
			(action == "AJAX_updateCompanyActNumberPrefix")			||
			(action == "AJAX_updateCompanyActNumberPostfix")		||
			(action == "AJAX_updateCompanyTIN")						||
			(action == "AJAX_updateCompanyVAT")						||
			(action == "AJAX_updateCompanyVATCalculationType")		||
			(action == "AJAX_updateCompanyAccount")					||
			(action == "AJAX_updateCompanyOGRN")					||
			(action == "AJAX_updateCompanyKPP")						||
			(action == "AJAX_updateCompanyLegalAddress")			||
			(action == "AJAX_updateCompanyMailingAddress")			||
			(action == "AJAX_updateAgencyPosition")					||
			(action == "AJAX_updateAgencyEditCapability")			||
			(action == "AJAX_updateSoWEditCapability")				||
			(action == "AJAX_updateCompanyMailingZipID")			||
			(action == "AJAX_updateCompanyLegalZipID")				||
			(action == "AJAX_updateCompanyBankID")					||
			(action == "AJAX_updateExpenseTemplateTitle")			||
			(action == "AJAX_updateExpenseTemplateTaxable")			||
			(action == "AJAX_updateExpenseTemplateAgencyComment")	||
			(action == "AJAX_updateExpenseTemplateLineTitle")		||
			(action == "AJAX_updateExpenseTemplateLineDescription")	||	
			(action == "AJAX_updateExpenseTemplateLineTooltip")		||
			(action == "AJAX_updateExpenseTemplateLineDomType")		||
			(action == "AJAX_updateExpenseTemplateLinePaymentCash")	||	
			(action == "AJAX_updateExpenseTemplateLinePaymentCard")	||	
			(action == "AJAX_updateExpenseTemplateLineRequired")	||
			(action == "AJAX_updateAirfareLimitationByDirection")	||
			(action == "AJAX_updateBTAllowanceCountry")				||
			(action == "AJAX_updateBTAllowanceAmount")				||
			(action == "AJAX_deleteBTAllowance")					||
			(action == "AJAX_updateHolidayCalendarDate")			||
			(action == "AJAX_updateHolidayCalendarTitle")			||
			(action == "AJAX_deleteHolidayCalendar")				||
			(action == "AJAX_updateCompanyCustomField")				||

			(action == "AJAX_updateCostCenterNumber")				||
			(action == "AJAX_updateCostCenterAct")					||
			(action == "AJAX_updateCostCenterSignDate")				||
			(action == "AJAX_updateCostCenterStartDate")			||
			(action == "AJAX_updateCostCenterEndDate")				||
			(action == "AJAX_updateCostCenterCustomField")			||

			(action == "AJAX_updateCostCenterToCustomer")			||
			(action == "AJAX_deleteCostCenterFromCustomer")			||
			(action == "AJAX_deleteCostCenter")						||
			(action == "AJAX_updateCostCenterTitle")				||
			(action == "AJAX_updateCostCenterDescription")
		)
		{
			MESSAGE_DEBUG("", action, "start");

			auto			success_message	= ""s;
			auto			error_message	= ""s;

			auto			company_id		= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("company_id"));
			auto			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			if(
				new_value.length() 									||
				(action == "AJAX_updateCompanyCustomField")			||	// --- custom field could be empty
				(action == "AJAX_updateCostCenterCustomField")		||	// --- custom field could be empty
				(action == "AJAX_updateCompanyActNumberPrefix")		||	// --- ActNumberPrefix could be empty
				(action == "AJAX_updateCompanyActNumberPostfix")		 // --- ActNumberPostfix could be empty
			)
			{
				error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
				if(error_message.empty())
				{
					string	agency_id = GetAgencyID(&user, &db);
					if(agency_id.length())
					{
						if(action == "AJAX_updateCompanyTitle") 		{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyDescription")	{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyWebSite")		{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyActNumber")		{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyActNumberPrefix")	{				if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyActNumberPostfix")	{				if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyTIN")			{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyVAT")			{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyVATCalculationType"){ new_value = (new_value == "Y" ? "sum_by_row" : "percentage");	if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyAccount")		{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyOGRN")			{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyKPP")			{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyLegalAddress") 	{					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyMailingAddress"){					if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyMailingZipID") 	{	new_value = id; if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyLegalZipID") 	{	new_value = id; if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }
						if(action == "AJAX_updateCompanyBankID") 		{	new_value = id; if(company_id.length())	id = company_id; else { error_message = gettext("Company not found"); MESSAGE_ERROR("", action, "Company not defined in HTTP parameters"); } }

						if(error_message.empty())
						{
							error_message = isActionEntityBelongsToAgency(action, id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = CheckNewValueByAction(action, id, "", new_value, &db, &user);
								if(error_message.empty())
								{
									if((action.find("update") != string::npos))
									{
										string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

										if(existing_value != new_value)
											error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
										else
										{
											MESSAGE_DEBUG("", action, "existing_value == new_value(" + new_value + "), no need to change DB");
										}
										if(error_message.empty())
										{
											// --- good finish

											if(GeneralNotifySoWContractPartiesAboutChanges(action, id, "", existing_value, new_value, &db, &user))
											{
											}
											else
											{
												MESSAGE_DEBUG("", action, "fail to notify agency about changes");
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + new_value + "])");
										}
									}
									else if(action.find("insert") != string::npos)
									{
										
									}
									else if(action.find("delete") != string::npos)
									{
										string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

										if(GeneralNotifySoWContractPartiesAboutChanges(action, id, "", existing_value, new_value, &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", action, "fail to notify agency about changes");
										}

										error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
										if(error_message.empty())
										{
											// --- good finish
										}
										else
										{
											MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + new_value + "])");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
									}

								}
								else
								{
									MESSAGE_DEBUG("", action, "new value failed to pass validity check");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "company.id not defined")
						}
					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
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
			(action == "AJAX_updateTimecardApproverOrder")	||
			(action == "AJAX_updateBTApproverOrder")
			)
		{
			{
				MESSAGE_DEBUG("", action, "start");

				auto			error_message = ""s;
				auto			success_message = ""s;

				auto			db_table		= (action == "AJAX_updateTimecardApproverOrder" ? "timecard_approvers"s : "bt_approvers");
				auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));
				auto			values			= split(new_value, ',');


				if(values.size())
				{
					auto		psow_ids		= GetValuesFromDB("SELECT DISTINCT(`contract_psow_id`) FROM " + db_table + " WHERE `id` IN (" + join(values) + ");", &db);

					if(psow_ids.size() == 1)
					{
						auto			psow_id			= psow_ids[0];
						auto			sow_id			= GetSoWIDByPSoWID(psow_id, &db, &user);

						if(sow_id.length())
						{
							error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
							if(error_message.empty())
							{
								string	agency_id = GetAgencyID(&user, &db);
								if(agency_id.length())
								{
									{
										error_message = isActionEntityBelongsToAgency(action, sow_id, agency_id, &db, &user);
										if(error_message.empty())
										{
											error_message = CheckNewValueByAction(action, psow_id, sow_id, new_value, &db, &user);
											if(error_message.empty())
											{
												if((action.find("update") != string::npos))
												{
													string		existing_value = GetDBValueByAction(action, sow_id, "", &db, &user);

													error_message = SetNewValueByAction(action, sow_id, "", new_value, &db, &user);
													if(error_message.empty())
													{
														// --- good finish

														if(GeneralNotifySoWContractPartiesAboutChanges(action, sow_id, "", existing_value, new_value, &db, &user))
														{

															if(action == "AJAX_updateTimecardApproverOrder")	success_message = "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_psow_id` IN (" + Get_PSoWIDsBySoWID_sqlquery(sow_id) + ");", &db, &user, DO_NOT_INCLUDE_PSOW_INFO) + "]";
															else if(action == "AJAX_updateBTApproverOrder")		success_message = "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_psow_id` IN (" + Get_PSoWIDsBySoWID_sqlquery(sow_id) + ");", &db, &user, DO_NOT_INCLUDE_PSOW_INFO) + "]";
															else
															{
																error_message = gettext("unsupported action");
																MESSAGE_ERROR("", action, error_message + " type(" + action + ")");
															}
														}
														else
														{
															MESSAGE_DEBUG("", action, "fail to notify agency about changes");
														}
													}
													else
													{
														MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + sow_id + "/[" + new_value + "])");
													}
												}
												else
												{
													MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
												}

											}
											else
											{
												MESSAGE_DEBUG("", action, "new value failed to pass validity check");
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
										}
									}
								}
								else
								{
									error_message = "Агенство не найдено";
									MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
							}
						}
						else
						{

						}
					}
					else
					{
						error_message = gettext("approvers belong to different PSoW") + ". "s + gettext("Quantity short") + " " + gettext("PSoW") + " " + to_string(psow_ids.size());
						MESSAGE_ERROR("", action, error_message);
					}
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
				}

				AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_addTaskAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			customer 		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer"));
				string			project			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project"));
				string			task			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task"));
				string			period_start	= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("period_start"));
				string			period_end		= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("period_end"));
				string			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(customer.length() && project.length() && task.length() && period_start.length() && period_end.length() && sow_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						if(db.Query("SELECT `agency_company_id`,`start_date`,`end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
								string agency_id				= db.Get(0, "agency_company_id");
								struct tm sow_start				= GetTMObject(db.Get(0, "start_date"));
								struct tm sow_end				= GetTMObject(db.Get(0, "end_date"));
								struct tm new_assignment_start	= GetTMObject(period_start);
								struct tm new_assignment_end	= GetTMObject(period_end);

								if(difftime(mktime(&new_assignment_start), mktime(&sow_start)) >= 0)
								{
									if(difftime(mktime(&sow_end), mktime(&new_assignment_end)) >= 0)
									{
										if(difftime(mktime(&new_assignment_end), mktime(&new_assignment_start)) >= 0)
										{
											string	task_assignment_id = GetTaskAssignmentID(customer, project, task, sow_id, &db);

											if(task_assignment_id.empty())
											{
												string	assignment_start = to_string(1900 + new_assignment_start.tm_year) + "-" + to_string(1 + new_assignment_start.tm_mon) + "-" + to_string(new_assignment_start.tm_mday);
												string	assignment_end = to_string(1900 + new_assignment_end.tm_year) + "-" + to_string(1 + new_assignment_end.tm_mon) + "-" + to_string(new_assignment_end.tm_mday);
												string	task_id = GetTaskIDFromAgency(customer, project, task, agency_id, &db);
												bool	notify_about_task_creation = false;

												if(task_id.empty())
												{
													task_id = CreateTaskBelongsToAgency(customer, project, task, agency_id, &db);
													notify_about_task_creation = true;
												}
												else
												{
													MESSAGE_DEBUG("", action, "Customer/Project/Task already exists for this agency.id(" + agency_id + ")");
												}

												{
													auto	cost_center_id = GetCostCenterIDByTaskID(task_id, &db);
													if(cost_center_id.length())
													{
														task_assignment_id = CreateTaskAssignment(task_id, sow_id, assignment_start, assignment_end, &db, &user);

														if(task_assignment_id.length())
														{
															if(CreatePSoWfromTimecardTaskIDAndSoWID(task_id, sow_id, &db, &user))
															{
																ostResult << "{\"result\":\"success\"}";

																if(notify_about_task_creation)
																{
																	if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_addTask", task_id, sow_id, "", task, &db, &user))
																	{
																	}
																	else
																	{
																		MESSAGE_ERROR("", action, "fail to notify SoW parties");
																	}
																}
																else
																{
																	MESSAGE_DEBUG("", "", "no notification about task creation");
																}
																if(GeneralNotifySoWContractPartiesAboutChanges(action, task_assignment_id, sow_id, "", customer + TIMECARD_ENTRY_TITLE_SEPARATOR + project + TIMECARD_ENTRY_TITLE_SEPARATOR + task + " ( с " + assignment_start + " по " + assignment_end + ")", &db, &user))
																{
																}
																else
																{
																	MESSAGE_ERROR("", action, "fail to notify SoW parties");
																}
															}
															else
															{
																error_message = gettext("unable to create psow");
																MESSAGE_DEBUG("", action, "there were no new PSoW(cost_center.id<-customer<-project<-task.id(" + task_id + ") / SoW.id(" + sow_id + ")) created due to either PSoW already exists, or Customer<-project<-tasks not assigned to any CostCenter")
															}
														}
														else
														{
															error_message = "Не удалось создать назначение";
															MESSAGE_DEBUG("", action, "fail to create assignment sow.id(" + sow_id + ") task.id(" + task_id + ")");
														}
													}
													else
													{
														error_message = gettext("task not assigned to a cost center");
														MESSAGE_DEBUG("", action, error_message);
													}
												}

											}
											else
											{
												error_message = "Такое назначение уже существует";
												MESSAGE_DEBUG("", action, "assignment Customer/Project/Task already exists for this sow.id(" + sow_id + ")");
											}
											
										}
										else
										{
											MESSAGE_DEBUG("", action, "new assignment finish(" + period_end + ")  earlier than start(" + period_start + ")");
											error_message = "Начало назначения должно быть раньше, чем окончание";
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "new assignment end " + period_end + " later than sow.id(" + sow_id + ") end");
										error_message = "Новое назначение не должно заканчиваться позднее контракта";
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "new assignment start " + period_start + " earlier than sow.id(" + sow_id + ") start");
									error_message = "Новое назначение не должно начинаться раньше контракта";
								}
						}
						else
						{
							error_message = "SoW "s + gettext("not found");
							MESSAGE_ERROR("", action, "sow.id" + sow_id + " not found");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Некоторые параметры не заданы";
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addBTExpenseTemplateAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			bt_expense_templates_param	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("bt_expense_templates"));
				string			sow_id 						= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(split(bt_expense_templates_param, ',').size() && sow_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						if(db.Query("SELECT `agency_company_id`,`start_date`,`end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							string agency_id				= db.Get(0, "agency_company_id");

							error_message = isActionEntityBelongsToAgency(action, sow_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								vector<string>	new_bt_expense_template_id_list = split(bt_expense_templates_param, ',');
								
								for(auto new_bt_expense_template_id: new_bt_expense_template_id_list)
								{
									string	bt_expense_template_sow_assignment_id = GetBTExpenseTemplateAssignmentToSoW(new_bt_expense_template_id, sow_id, &db);

									if(bt_expense_template_sow_assignment_id.empty())
									{
										bt_expense_template_sow_assignment_id = CreateBTExpenseTemplateAssignmentToSoW(new_bt_expense_template_id, sow_id, &db, &user);

										if(bt_expense_template_sow_assignment_id.length())
										{
											if(GeneralNotifySoWContractPartiesAboutChanges(action, bt_expense_template_sow_assignment_id, sow_id, "", "", &db, &user))
											{
											}
											else
											{
												MESSAGE_ERROR("", action, "fail to notify SoW parties");
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to create assignment sow.id(" + sow_id + ") new_bt_expense_template.id(" + new_bt_expense_template_id + ")");
											error_message = "Не удалось создать назначение";
											break;
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "assignment BTExpenseTemplate(" + new_bt_expense_template_id + ") already assigned to sow.id(" + sow_id + ")");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "sow.id" + sow_id + " not found");
							error_message = "Контракт не найден";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
					}

				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, error_message);
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addTask")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;

			auto			customer 		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer"));
			auto			project			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project"));
			auto			task			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task"));


			if(customer.length() && project.length() && task.length())
			{
				if(
						(GetNumberOfLetters(customer) >= 3) && 
						(GetNumberOfLetters(project	) >= 3) && 
						(GetNumberOfLetters(task	) >= 3)
					)
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							string	task_id = GetTaskIDFromAgency(customer, project, task, agency_id, &db);

							if(task_id.empty())
							{
								task_id = CreateTaskBelongsToAgency(customer, project, task, agency_id, &db);

								if(GeneralNotifySoWContractPartiesAboutChanges(action, task_id, "", "", task, &db, &user))
								{
								}
								else
								{
									MESSAGE_ERROR("", action, "fail to notify agency");
								}
							}
							else
							{
								error_message = "Задача (" + task + ") уже существует у заказчика " + customer + " в проекте " + project;
								MESSAGE_DEBUG("", action, "Customer/Project/Task already exists for this agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("number of letters in Customer/Project/Task must be at least") + " 3"s;
					MESSAGE_DEBUG("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, "", error_message);
			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addExpenseTemplate")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				auto			http_params = indexPage.GetVarsHandler();
				auto			expense_random_name = ""s;
				regex			regex1("bt_expense_template_line_random_");

				C_ExpenseTemplate	expense_template;

				expense_template.SetID			(CheckHTTPParam_Text(http_params->Get("bt_expense_template_id")));
				expense_template.SetTitle		(CheckHTTPParam_Text(http_params->Get("bt_expense_template_title")));
				expense_template.SetDescription	(CheckHTTPParam_Text(http_params->Get("bt_expense_template_description")));

				expense_random_name = http_params->GetNameByRegex(regex1);
				while(expense_random_name.length() && error_message.empty())
				{
					string expense_template_line_random = CheckHTTPParam_Number(http_params->Get(expense_random_name));

					if(expense_template_line_random.length())
					{
						C_ExpenseLineTemplate	expense_line_template;

						expense_line_template.title				= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_title_" + expense_template_line_random));
						expense_line_template.tooltip			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_tooltip_" + expense_template_line_random));
						expense_line_template.description		= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_description_" + expense_template_line_random));
						expense_line_template.dom_type			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_dom_type_" + expense_template_line_random));
						expense_line_template.required			= (http_params->Get("bt_expense_template_line_is_required_" + expense_template_line_random) == "true");
						expense_line_template.SetPaymentMethod	(
																	http_params->Get("bt_expense_template_line_is_cash_" + expense_template_line_random) == "true",
																	http_params->Get("bt_expense_template_line_is_card_" + expense_template_line_random) == "true"
																);

						expense_template.AddLine(expense_line_template);
					}
					else
					{
						error_message = "некорректный параметер random у документа";
						MESSAGE_ERROR("", action, "Can't convert expense_random(" + http_params->Get(expense_random_name) + ") to number")
					}

					http_params->Delete(expense_random_name);
					expense_random_name = http_params->GetNameByRegex(regex1);
				}

				if(error_message.empty())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							expense_template.SetCompanyID(agency_id);

							error_message = expense_template.CheckValidity(&db);
							if(error_message.empty())
							{
								error_message = expense_template.SaveToDB(&db);
								if(error_message.empty())
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, expense_template.GetID(), "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail to check bt_expense_template consistency");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "consistency check failed");
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addAirfareLimitationByDirection")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			from_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("from_id"));
				string			to_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("to_id"));
				string			limit = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("limit"));


				if(from_id.length() && to_id.length() && limit.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isValidToAddAirfareLimitByDirection(from_id, to_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								auto	id = db.InsertQuery("INSERT INTO `airfare_limits_by_direction` "
															"(`from`, `to`, `limit`, `agency_company_id`, `creator_user_id`, `eventTimestamp`) "
															"VALUES "
															"(" + quoted(from_id) + ", "
																+ quoted(to_id) + ", "
																+ quoted(limit) + ", "
																+ quoted(agency_id) + ", "
																+ quoted(user.GetID()) + ", "
																+ "UNIX_TIMESTAMP());");
								if(id)
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, to_string(id), "", "", limit, &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, error_message);
							}
						}
						else
						{
							error_message = gettext("agency not found");
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
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
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, error_message);
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}


		if(action == "AJAX_addExpenseTemplateLine")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			new_expense_tempate_line_obj = ""s;
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			bt_expense_template_id = ""s;

				auto			http_params = indexPage.GetVarsHandler();
				auto			expense_random_name = ""s;
				regex			regex1("bt_expense_template_line_random_");

				C_ExpenseLineTemplate	expense_line_template;

				bt_expense_template_id = CheckHTTPParam_Text(http_params->Get("bt_expense_template_id"));

				expense_random_name = http_params->GetNameByRegex(regex1);
				while(expense_random_name.length() && error_message.empty())
				{
					string expense_template_line_random = CheckHTTPParam_Number(http_params->Get(expense_random_name));

					if(expense_template_line_random.length())
					{

						expense_line_template.title				= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_title_" + expense_template_line_random));
						expense_line_template.tooltip			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_tooltip_" + expense_template_line_random));
						expense_line_template.description		= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_description_" + expense_template_line_random));
						expense_line_template.dom_type			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_dom_type_" + expense_template_line_random));
						expense_line_template.required			= (http_params->Get("bt_expense_template_line_is_required_" + expense_template_line_random) == "true");
						expense_line_template.SetPaymentMethod	(
																	http_params->Get("bt_expense_template_line_is_cash_" + expense_template_line_random) == "true",
																	http_params->Get("bt_expense_template_line_is_card_" + expense_template_line_random) == "true"
																);
					}
					else
					{
						error_message = "некорректный параметер random у документа";
						MESSAGE_ERROR("", action, "Can't convert expense_random(" + http_params->Get(expense_random_name) + ") to number")
					}

					http_params->Delete(expense_random_name);
					expense_random_name = http_params->GetNameByRegex(regex1);
				}


				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, bt_expense_template_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								expense_line_template.SetBTExpenseTemplateID(bt_expense_template_id);
								error_message = expense_line_template.CheckValidity(&db);
								if(error_message.empty())
								{
									error_message = expense_line_template.SaveToDB(&db);
									if(error_message.empty())
									{
										new_expense_tempate_line_obj = GetBTExpenseLineTemplatesInJSONFormat("SELECT * FROM `bt_expense_line_templates` WHERE `id`=\"" + expense_line_template.GetID() + "\";", &db, &user);

										if(GeneralNotifySoWContractPartiesAboutChanges(action, expense_line_template.GetID(), "", "", "", &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", action, "fail to notify agency");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "fail to check bt_expense_template consistency");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\",\"bt_expense_line_template\":" + new_expense_tempate_line_obj + "}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteTaskAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			timecard_task_assignment_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(timecard_task_assignment_id.length())
				{
					if(db.Query("SELECT `timecard_tasks_id`,`contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + timecard_task_assignment_id + "\";"))
					{
						string	sow_id = db.Get(0, "contract_sow_id");
						string	task_id = db.Get(0, "timecard_tasks_id");

						error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
						if(error_message.empty())
						{
							if(db.Query("SELECT COUNT(*) AS `counter` FROM `timecard_lines` WHERE `timecard_task_id`=\"" + task_id + "\" AND `timecard_id` IN ( "
											"SELECT `id` FROM `timecards` WHERE `contract_sow_id`=\"" + sow_id + "\" "
										");"
							))
							{
								string	counter = db.Get(0, "counter");

								if(counter == "0")
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, timecard_task_assignment_id, sow_id, "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify SoW parties");
									}

									// --- important that assignment removal come after notification
									// --- such as notification use information about assignment 
									db.Query("DELETE FROM `timecard_task_assignment` WHERE `id`=\"" + timecard_task_assignment_id + "\";");
									ostResult << "{\"result\":\"success\"}";
								}
								else
								{
									MESSAGE_DEBUG("", action, counter + " instances reported on timecard_task.id(" + task_id + ")");
									error_message = "Нельзя удалять, поскольку субконтрактор отчитался на это назначение " + counter + " раз(а)";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "can't define # of reported lines");
								error_message = "Ошибка БД";
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "timecard_task_assignment.id(" + timecard_task_assignment_id + ") not found");
						error_message = "Назначение не найдено";
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteBTExpenseAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			bt_expense_assignment_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(bt_expense_assignment_id.length())
				{
					if(db.Query("SELECT `bt_expense_template_id`,`sow_id` FROM `bt_sow_assignment` WHERE `id`=\"" + bt_expense_assignment_id + "\";"))
					{
						string	sow_id = db.Get(0, "sow_id");
						string	bt_expense_template_id = db.Get(0, "bt_expense_template_id");

						error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
						if(error_message.empty())
						{
							if(db.Query("SELECT COUNT(*) AS `counter` FROM `bt_expenses` WHERE `bt_expense_template_id`=\"" + bt_expense_template_id + "\" AND `bt_id` IN ( "
											"SELECT `id` FROM `bt` WHERE `contract_sow_id`=\"" + sow_id + "\" "
										");"
							))
							{
								string	counter = db.Get(0, "counter");

								if(counter == "0")
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, bt_expense_assignment_id, sow_id, "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify SoW parties");
									}

									// --- important that assignment removal comes after notification
									// --- such as notification use information about assignment 
									db.Query("DELETE FROM `bt_sow_assignment` WHERE `id`=\"" + bt_expense_assignment_id + "\";");
									ostResult << "{\"result\":\"success\"}";
								}
								else
								{
									MESSAGE_DEBUG("", action, counter + " instances reported on bt_expenses.id(" + bt_expense_template_id + ")");
									error_message = "Нельзя удалять, поскольку субконтрактор использовал этот расход " + counter + " раз(а)";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "can't define # of reported lines");
								error_message = "Ошибка БД";
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "bt_sow_assignment.id(" + bt_expense_assignment_id + ") not found");
						error_message = "Назначение не найдено";
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteTask")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			task_id		= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(task_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, task_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isTaskIDValidToRemove(task_id, &db);
								if(error_message.empty())
								{
									string		removal_sql_query = "";
									string		project_id, customer_id;

									tie(customer_id, project_id) = GetCustomerIDProjectIDByTaskID(task_id, &db);

									if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_deleteTask", task_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
									db.Query("DELETE FROM `timecard_tasks` WHERE `id`=\"" + task_id + "\";");
									ostResult << "{\"result\":\"success\"}";

									if(isProjectIDValidToRemove(project_id, &db))
									{
										if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_deleteProject", project_id, "", "", "", &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", action, "fail to notify agency");
										}
										db.Query("DELETE FROM `timecard_projects` WHERE `id`=\"" + project_id + "\";");

										if(isCustomerIDValidToRemove(customer_id, &db))
										{
											if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_deleteCustomer", customer_id, "", "", "", &db, &user))
											{
											}
											else
											{
												MESSAGE_ERROR("", action, "fail to notify agency");
											}
											db.Query("DELETE FROM `timecard_customers` WHERE `id`=\"" + customer_id + "\";");
										}
										else
										{
											MESSAGE_DEBUG("", action, "customer_id(" + customer_id + ") has another projects assigned. Not valid for removal.");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "project_id(" + project_id + ") has another tasks assigned. Not valid for removal.");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "can't remove task_id(" + task_id + ")");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "task_id missed");
					error_message = "Не указан номер задачи на удаление";
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteExpenseTemplate")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			expense_template_id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(expense_template_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, expense_template_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isExpenseTemplateIDValidToRemove(expense_template_id, &db);
								if(error_message.empty())
								{
									string		removal_sql_query = "";

									if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_deleteExpenseTemplate", expense_template_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
									db.Query("DELETE FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=\"" + expense_template_id + "\";");
									db.Query("DELETE FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + expense_template_id + "\";");
									db.Query("DELETE FROM `bt_expense_templates` WHERE `id`=\"" + expense_template_id + "\";");
									ostResult << "{\"result\":\"success\"}";
								}
								else
								{
									MESSAGE_DEBUG("", action, "subcontractors reported on expense_template_id(" + expense_template_id + "), can't remove it");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "expense_template_id missed");
					error_message = "Не указан номер расхода на удаление";
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteExpenseTemplateLine")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			expense_template_line_id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(expense_template_line_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, expense_template_line_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isExpenseTemplateLineIDValidToRemove(expense_template_line_id, &db);
								if(error_message.empty())
								{
									string		removal_sql_query = "";

									if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_deleteExpenseTemplateLine", expense_template_line_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
									
									db.Query("DELETE FROM `bt_expense_line_templates` WHERE `id`=\"" + expense_template_line_id + "\";");
								}
								else
								{
									MESSAGE_DEBUG("", action, "expense_template_line_id(" + expense_template_line_id + ") presents in expenses, can't remove it");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "expense_template_line_id missed");
					error_message = "Не указан номер расхода на удаление";
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteAirfarelimitationByDirection")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, id, agency_id, &db, &user);
							if(error_message.empty())
							{
								if(GeneralNotifySoWContractPartiesAboutChanges("AJAX_deleteAirfarelimitationByDirection", id, "", "", "", &db, &user))
								{
								}
								else
								{
									MESSAGE_ERROR("", action, "fail to notify agency");
								}
								
								db.Query("DELETE FROM `airfare_limits_by_direction` WHERE `id`=\"" + id + "\";");
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "id missed");
					error_message = "Не указан идентификатор ограничения на удаление";
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addCostCenter")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name	= "json_response.htmlt"s;
				auto			error_message	= ""s;

				auto			company_id		= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("company_id"));
				auto			cost_center_id	= 0l;

				if(company_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = CheckNewValueByAction(action, agency_id, "", company_id, &db, &user);
							if(error_message.empty())
							{
								cost_center_id = db.InsertQuery("INSERT INTO `cost_centers` (`company_id`, `agency_company_id`, `assignee_user_id`, `eventTimestamp`) VALUES (\"" + company_id + "\", \"" + agency_id + "\", \"" + user.GetID() + "\", UNIX_TIMESTAMP());");
								if(cost_center_id)
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, to_string(cost_center_id), "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = gettext("agency not found");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, error_message);
				}

				ostResult.str("");
				if(error_message.empty())
				{
					ostResult	<< "{"
								<< "\"result\":\"success\","
								<< "\"cost_centers\":[" << GetCostCentersInJSONFormat("SELECT * FROM `cost_centers` WHERE `id`=\"" + to_string(cost_center_id) + "\";", &db, &user) << "]"
								<< "}";
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

		if(action == "AJAX_addBTAllowance")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name	= "json_response.htmlt"s;
				auto			error_message	= ""s;

				auto			country_id		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("country_id"));
				auto			amount			= CheckHTTPParam_Float(indexPage.GetVarsHandler()->Get("amount"));

				if(country_id.length() && amount.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = CheckNewValueByAction(action, agency_id, "", country_id, &db, &user);
							if(error_message.empty())
							{
								auto	bt_allowance_id = db.InsertQuery("INSERT INTO `bt_allowance` (`agency_company_id`, `geo_country_id`, `amount`, `eventTimestamp`) VALUES (\"" + agency_id + "\", \"" + country_id + "\", \"" + amount + "\", UNIX_TIMESTAMP());");
								if(bt_allowance_id)
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, to_string(bt_allowance_id), "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, error_message);
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = gettext("agency not found");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, error_message);
				}

				ostResult.str("");
				if(error_message.empty())
				{
					ostResult	<< "{"
								<< "\"result\":\"success\""
								<< "}";
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

		if(action == "AJAX_addHolidayCalendar")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name	= "json_response.htmlt"s;
				auto			error_message	= ""s;

				auto			date		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("date"));
				auto			title		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("title"));

				if(date.length() && title.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = CheckNewValueByAction(action, agency_id, "", date, &db, &user);
							if(error_message.empty())
							{
								auto	holiday_calendar_id = db.InsertQuery("INSERT INTO `holiday_calendar` (`agency_company_id`, `date`, `title`, `eventTimestamp`) VALUES (\"" + agency_id + "\", \"" + date + "\", \"" + title + "\", UNIX_TIMESTAMP());");
								if(holiday_calendar_id)
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, to_string(holiday_calendar_id), "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, error_message);
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = gettext("agency not found");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_ERROR("", action, error_message);
				}

				ostResult.str("");
				if(error_message.empty())
				{
					ostResult	<< "{"
								<< "\"result\":\"success\""
								<< "}";
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

		if(action == "AJAX_submitTimecardsToInvoice")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			cost_center_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("cost_center_id"));
				auto			timecard_list_param = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("timecard_list"));
				auto			timecard_list = split(timecard_list_param, ',');
				auto			isListCorrect = true;

				for(auto &item: timecard_list)
				{
					if(CheckHTTPParam_Number(item).empty())
					{
						isListCorrect = false;
					}
				}

				if(isListCorrect)
				{
					if(cost_center_id.length())
					{
						if(user.GetType() == "agency")
						{
							error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
							if(error_message.empty())
							{
								if(isCostCenterBelongsToAgency(cost_center_id, &db, &user))
								{
									if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ");"))
									{
										string		agency_id = db.Get(0, "id");

										error_message = isTimecardsBelongToAgency(timecard_list, agency_id, &db, &user);
										if(error_message.empty())
										{
											error_message = isTimecardsHavePSOWAssigned(timecard_list, cost_center_id, &db, &user);
											if(error_message.empty())
											{
												C_Invoice_Service_Agency_To_CC	c_invoice(&config, &db, &user);

												c_invoice.SetTimecardList(timecard_list);
												c_invoice.SetCostCenterID(cost_center_id);

												error_message = c_invoice.GenerateDocumentArchive();
												if(error_message.empty())
												{
													ostResult << "{"
																	"\"result\":\"success\","
																	"\"invoice_id\":\"" << c_invoice.GetInvoiceID() << "\""
																 "}";
												}
												else
												{
													MESSAGE_ERROR("", action, "fail to generate invoice document archive");
												}
											}
											else
											{
												MESSAGE_ERROR("", action, "some timecard.id's(" + timecard_list_param + ") doesn't belong to agency(" + agency_id + ")");
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") isn't agency employee");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "Can't define agency.id by user.id(" + user.GetID() + ")");
										error_message = gettext("You are not agency employee");
									}
								}
								else
								{
									MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employed");
									error_message = gettext("You are not agency employee");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
							error_message = gettext("You are not agency employee");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "cost_center_id id is missed");
						error_message = gettext("mandatory parameter missed");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "timecard list(" + timecard_list_param + ") is incorrect");
					error_message = gettext("Incorrect timecard list");
				}

				if(error_message.empty())
				{
				}
				else
				{
					MESSAGE_ERROR("", action, "failed (" + error_message + ")");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + RemoveQuotas(error_message) + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_submitBTsToInvoice")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			cost_center_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("cost_center_id"));
				auto			bt_list_param = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("bt_list"));
				auto			bt_list = split(bt_list_param, ',');
				auto			isListCorrect = true;

				for(auto &item: bt_list)
				{
					if(CheckHTTPParam_Number(item).empty())
					{
						isListCorrect = false;
					}
				}

				if(isListCorrect)
				{
					if(cost_center_id.length())
					{
						if(user.GetType() == "agency")
						{
							error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
							if(error_message.empty())
							{
								if(isCostCenterBelongsToAgency(cost_center_id, &db, &user))
								{
									if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ");"))
									{
										string		agency_id = db.Get(0, "id");

										error_message = isBTsBelongToAgency(bt_list, agency_id, &db, &user);
										if(error_message.empty())
										{
											error_message = isBTsHavePSOWAssigned(bt_list, cost_center_id, &db, &user);
											if(error_message.empty())
											{
												C_Invoice_BT_Agency_To_CC	c_invoice(&config, &db, &user);

												c_invoice.SetBTList(bt_list);
												c_invoice.SetCostCenterID(cost_center_id);

												error_message = c_invoice.GenerateDocumentArchive();
												if(error_message.empty())
												{
													ostResult << "{"
																	"\"result\":\"success\","
																	"\"invoice_id\":\"" << c_invoice.GetInvoiceID() << "\""
																 "}";
												}
												else
												{
													MESSAGE_DEBUG("", action, "fail to generate invoice document archive");
												}

											}
											else
											{
												MESSAGE_ERROR("", action, "some bt.id's(" + bt_list_param + ") doesn't belong to agency(" + agency_id + ")");
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") isn't agency employee");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "Can't define agency.id by user.id(" + user.GetID() + ")");
										error_message = gettext("You are not agency employee");
									}
								}
								else
								{
									MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employed");
									error_message = gettext("You are not agency employee");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
							error_message = gettext("You are not agency employee");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "cost_center_id id is missed");
						error_message = gettext("mandatory parameter missed");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "bt list(" + bt_list_param + ") is incorrect");
					error_message = gettext("Incorrect bt list");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getAgencyInfo")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			agency_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				auto			include_bt = indexPage.GetVarsHandler()->Get("include_bt") == "true";
				auto			include_tasks = indexPage.GetVarsHandler()->Get("include_tasks") == "true";
				auto			include_countries = indexPage.GetVarsHandler()->Get("include_countries") == "true";

				string			agency_obj = GetAgencyObjectInJSONFormat(agency_id, include_tasks, include_bt, &db, &user);

				if(agency_obj.length())
				{
					// --- get short info
					ostResult << "{\"result\":\"success\","
									"\"agencies\":[" + agency_obj + "]"
									+ (include_countries ? ",\"countries\":[" + GetCountryListInJSONFormat("SELECT * FROM `geo_country`;", &db, &user) + "]" : "")
									+ "}";
				}
				else
				{
					error_message = "Агенство не найдено";
					MESSAGE_DEBUG("", action, "agency(" + agency_id + ") not found");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteEmployee")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			employee_id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(employee_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, employee_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isEmployeeIDValidToRemove(employee_id, &db);
								if(error_message.empty())
								{
									if(GeneralNotifySoWContractPartiesAboutChanges(action, employee_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to notify agency");
									}

									if(db.Query("SELECT `user_id` FROM `company_employees` WHERE `id`=\"" + employee_id + "\";"))
									{
										string		user_id = db.Get(0, "user_id");

										db.Query(
											"DELETE FROM `company_employees` WHERE `id`=\"" + employee_id + "\";"
											"UPDATE `users` SET `type`=\"no role\" WHERE `id`=\"" + user_id + "\";"
											);
										ostResult << "{\"result\":\"success\"}";
									}
									else
									{
										error_message = "Пользователь не найден в БД";
										MESSAGE_DEBUG("", action, "fail to get user.id by employee_id(" + employee_id + ")");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "employee_id(" + employee_id + ") can't be fired");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "employee_id missed");
					error_message = "Не указан номер задачи на удаление";
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getApproversAutocompleteList")
		{
			auto			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			auto			psow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("psow_id"));
			auto			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;

			if(psow_id.length()) sow_id = GetSoWIDByPSoWID(psow_id, &db, &user);
			else
			{
				MESSAGE_DEBUG("", action, "take sow_id(" + sow_id + ") from HTTP param");
			}

			if(name.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					auto	affected = db.Query("SELECT `id`, `name`, `nameLast` FROM `users` WHERE ((`type`=\"approver\") OR (`type`=\"agency\")) AND ((`name` LIKE \"%" + name + "%\") OR (`nameLast` LIKE \"%" + name + "%\")) LIMIT 0, 20;");

					for(auto i = 0; i < affected; ++i)
					{
						if(i) success_message += ",";
						success_message += "{\"id\":\"" + db.Get(i, "id") + "\","
								  + "\"label\":\"" + db.Get(i, "name") + " " + db.Get(i, "nameLast") + "\"}";
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
			}

			indexPage.RegisterVariableForce("result", "[" + success_message + "]");

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getUnemployedAgentAutocompleteList")
		{
			auto			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("name"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(name.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
				if(error_message.empty())
				{
					int	affected = db.Query("SELECT `id`, `name`, `nameLast` FROM `users` WHERE (`type`=\"agency\") AND ((`name` LIKE \"%" + name + "%\") OR (`nameLast` LIKE \"%" + name + "%\"))  AND NOT(EXISTS(SELECT * FROM `company_employees` WHERE `user_id`=`users`.`id`)) LIMIT 0, 20;");

					if(affected)
					{
						success_message = ",\"autocomplete_list\":[";
						for(auto i = 0; i < affected; ++i)
						{
							if(i) success_message += ",";
							success_message += "{\"id\":\"" + db.Get(i, "id") + "\","
									  + "\"label\":\"" + db.Get(i, "name") + " " + db.Get(i, "nameLast") + "\"}";
						}
						success_message += "]";
					}
					else
					{
						success_message = ",\"autocomplete_list\":[]";
						MESSAGE_DEBUG("", "", "no users found");
					}	
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
				}
			}
			else
			{
				MESSAGE_DEBUG("", action, "parameter is empty");
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_addNewEmployee")
		{
			auto			user_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("user_id"));
			auto			title = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("title"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(user_id.length() && title.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
				if(error_message.empty())
				{
					auto agency_id = GetAgencyID(&user, &db);
					if(agency_id.length())
					{
						if(db.Query("SELECT `id` FROM `users` WHERE `id`=\"" + user_id + "\" AND `type`=\"agency\";"))
						{
							if(db.Query("SELECT `id` FROM `company_employees` WHERE `user_id`=\"" + user_id + "\";"))
							{
								error_message = gettext("user employed in another agency");
								MESSAGE_ERROR("", action, error_message);
							}
							else
							{
								auto title_id = GetCompanyPositionIDByTitle(title, &db);

								if(title_id.length())
								{
									if(db.InsertQuery("INSERT INTO `company_employees` ("
																		"`user_id`,"
																		"`company_id`,"
																		"`position_id`,"
																		"`eventTimestamp`"
																	") VALUES ("
																		+ quoted(user_id) + ","
																		+ quoted(agency_id) + ","
																		+ quoted(title_id) + ","
																		+ "UNIX_TIMESTAMP()"
																		")"))
									{
									}
									else
									{
										error_message = gettext("SQL syntax error");
										MESSAGE_ERROR("", action, error_message);
									}
								}
								else
								{
									error_message = gettext("fail to find company position title");
									MESSAGE_ERROR("", action, error_message);
								}
							}	
						}
						else
						{
							error_message = gettext("user is not initialized");
							MESSAGE_ERROR("", action, error_message);
						}	
					}
					else
					{
						error_message = gettext("agency not found");
						MESSAGE_ERROR("", action, error_message);
					}

				}
				else
				{
					error_message = gettext("You are not authorized");
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(	action == "AJAX_addTimecardApproverToPSoW" ||
			action == "AJAX_addBTExpenseApproverToPSoW"
		)
		{
			auto			new_value = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("new_value"));
			auto			psow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("psow_id"));
			auto			sow_id = GetSoWIDByPSoWID(psow_id, &db, &user);

			auto			error_message = ""s;
			auto			success_message = ""s;

			if(new_value.length() && sow_id.length() && psow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					string	approver_id = "";

					error_message = CheckNewValueByAction(action, psow_id, sow_id, new_value, &db, &user);
					if(error_message.empty())
					{
						error_message = SetNewValueByAction(action, psow_id, sow_id, new_value, &db, &user);
						if(error_message.empty())
						{
							success_message = GetInfoToReturnByAction(action, psow_id, sow_id, new_value, &db, &user);

							if(GeneralNotifySoWContractPartiesAboutChanges(action, psow_id, sow_id, "fake_existing_value", new_value, &db, &user))
							{
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/value = " + action + "/" + sow_id + "/[" + new_value + "])");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "new value failed to pass validity check");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);
		}

		if(	action == "AJAX_deleteTimecardApproverFromPSoW" ||
			action == "AJAX_deleteBTExpenseApproverFromPSoW"
		)
		{
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

			auto			error_message = ""s;
			auto			success_message = ""s;

			if(id.length())
			{
				auto		db_table	= (action == "AJAX_deleteTimecardApproverFromPSoW" ? "timecard_approvers"s : "bt_approvers");
				auto		psow_id		= GetPSoWIDByApprover("SELECT `contract_psow_id` FROM `" + db_table + "` WHERE `id`=\"" + id + "\";", &db, &user);
				if(psow_id.length())
				{
					auto		sow_id	= GetSoWIDByPSoWID(psow_id, &db, &user);
					if(psow_id.length())
					{
						error_message	= isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
						if(error_message.empty())
						{
							error_message = isActionEntityBelongsToSoW(action, id, sow_id, &db, &user);
							if(error_message.empty())
							{
								// --- notify before actual removal
								if(GeneralNotifySoWContractPartiesAboutChanges(action, psow_id, sow_id, id, "fake_new_value", &db, &user))
								{
								}
								else
								{
									MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
								}

								error_message = SetNewValueByAction(action, id, sow_id, "fake_new_value", &db, &user);
								if(error_message.empty())
								{
									error_message = ResubmitEntitiesByAction(action, "fake", sow_id, "fake_new_value", &config, &db, &user);
									if(error_message.empty())
									{
										success_message = GetInfoToReturnByAction(action, psow_id, sow_id, "fake_new_value", &db, &user);
									}
									else
									{
										MESSAGE_DEBUG("", action, "fail to resubmit entities by action");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "unable to delete value (action/sow_id/id = " + action + "/" + sow_id + "/" + id + ")");
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
						error_message = gettext("mandatory parameter missed");
						MESSAGE_DEBUG("", action, error_message);
					}
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);
		}

		if(action == "AJAX_getCompanyInfoBySoWID")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ");"))
					{
						string	agency_id = db.Get(0, "id");
						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, sow_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								string			company_obj = GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=(SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");", &db, &user);

								if(company_obj.length())
								{
									// --- get short info
									ostResult << "{\"result\":\"success\","
													"\"companies\":[" + company_obj + "]"
													"}";
								}
								else
								{
									error_message = "Агенство не найдено";
									MESSAGE_DEBUG("", action, "sow_id(" + sow_id + ") not found");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							error_message = "Агенство не найдено";
							MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an agency employee");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}



		// --- service invoicing

		if(action == "AJAX_getCostCenterList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				if(user.GetType() == "agency")
				{
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"cost_centers\":[" << GetCostCentersInJSONFormat(
												"SELECT * FROM `cost_centers` WHERE `agency_company_id`=("
													"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
														+ Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + 
													")"
												");", &db, &user) << "]";
						ostResult << "}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if((action == "AJAX_getServiceInvoiceList") || (action == "AJAX_getBTInvoiceList"))
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			cost_center_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("cost_center_id"));

				if(user.GetType() == "agency")
				{
					if(isCostCenterBelongsToAgency(cost_center_id, &db, &user))
					{
						ostResult << "{"
										"\"result\":\"success\",";
						if(action == "AJAX_getBTInvoiceList")
						{
							ostResult <<    "\"bt_invoices\":[" << GetServiceBTInvoicesInJSONFormat(
																		"SELECT * FROM `invoice_cost_center_bt` WHERE `cost_center_id`=\"" + cost_center_id + "\";"
																		, &db, &user) << "]";
						}
						else
						{
							ostResult <<    "\"service_invoices\":[" << GetServiceBTInvoicesInJSONFormat(
																	"SELECT * FROM `invoice_cost_center_service` WHERE `cost_center_id`=\"" + cost_center_id + "\";"
																	, &db, &user) << "]";
						}
						ostResult << "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employed");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getServiceInvoiceDetails")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			service_invoice_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("service_invoice_id"));

				if(user.GetType() == "agency")
				{
					if(isServiceInvoiceBelongsToAgency(service_invoice_id, &db, &user))
					{
						ostResult << "{"
										"\"result\":\"success\",";
						ostResult <<	"\"timecards\":[" + GetTimecardsInJSONFormat(	"SELECT * FROM `timecards` WHERE `id` IN ("
																							"SELECT `timecard_id` FROM `invoice_cost_center_service_details` WHERE `invoice_cost_center_service_id`=\"" + service_invoice_id + "\""
																						");", &db, &user) + "]";
						ostResult << "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "invoice_cost_center_service.id(" + service_invoice_id + ") doesn't belongs to agency user.id(" + user.GetID() + ") working at");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}
		if(action == "AJAX_getBTInvoiceDetails")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			bt_invoice_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_invoice_id"));

				if(user.GetType() == "agency")
				{
					if(isBTInvoiceBelongsToAgency(bt_invoice_id, &db, &user))
					{
						ostResult << "{"
										"\"result\":\"success\",";
						ostResult <<	"\"bt\":[" + GetBTsInJSONFormat(	"SELECT * FROM `bt` WHERE `id` IN ("
																							"SELECT `bt_id` FROM `invoice_cost_center_bt_details` WHERE `invoice_cost_center_bt_id`=\"" + bt_invoice_id + "\""
																						");", &db, &user, true) + "]";
						ostResult << "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "invoice_cost_center_bt.id(" + bt_invoice_id + ") doesn't belongs to agency user.id(" + user.GetID() + ") working at");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}


		if(action == "AJAX_recallServiceInvoice")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			service_invoice_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("invoice_id"));

				if(user.GetType() == "agency")
				{
					if(isServiceInvoiceBelongsToUser(service_invoice_id, &db, &user))
					{
						if((error_message = RecallServiceInvoice(service_invoice_id, &config, &db, &user)).empty())
						{
							ostResult << "{\"result\":\"success\"}";
						}
						else
						{
							MESSAGE_ERROR("", action, "can't recall service invoice(" + service_invoice_id + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "invoice_cost_center_service.id(" + service_invoice_id + ") doesn't belongs to user(" + user.GetID() + ")");
						error_message = gettext("You are not authorized") + ". "s + gettext("Only owner can recall an invoice");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_recallBTInvoice")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			bt_invoice_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("invoice_id"));

				if(user.GetType() == "agency")
				{
					if(isBTInvoiceBelongsToUser(bt_invoice_id, &db, &user))
					{
						if((error_message = RecallBTInvoice(bt_invoice_id, &config, &db, &user)).empty())
						{
							ostResult << "{\"result\":\"success\"}";
						}
						else
						{
							MESSAGE_ERROR("", action, "can't recall bt invoice(" + bt_invoice_id + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "invoice_cost_center_bt.id(" + bt_invoice_id + ") doesn't belongs to user(" + user.GetID() + ")");
						error_message = gettext("You are not authorized") + ". "s + gettext("Only owner can recall an invoice");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getApprovedTimecardList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			cost_center_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("cost_center_id"));

				if(user.GetType() == "agency")
				{
					if(isCostCenterBelongsToAgency(cost_center_id, &db, &user))
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"timecards\":[" << GetTimecardsInJSONFormat(
																"SELECT * FROM `timecards` WHERE `status`=\"approved\" AND ("
																	"`id` IN ("
																		"SELECT `timecard_id` FROM `timecard_lines` WHERE `timecard_task_id` IN ("
																			"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
																				"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id` IN ("
																					"SELECT `timecard_customer_id` FROM `cost_center_assignment` WHERE `cost_center_id`=\"" + cost_center_id + "\""
																				")"
																			")"
																		")"
																	")"
																") AND NOT("
																	"`id` IN ("
																		"SELECT `timecard_id` FROM `invoice_cost_center_service_details` WHERE `invoice_cost_center_service_id` IN ("
																			"SELECT `id` FROM `invoice_cost_center_service` WHERE `cost_center_id`=\"" + cost_center_id + "\""
																		")"
																	")"
																")", &db, &user) << "]";
						ostResult << "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employed");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getApprovedBTList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			cost_center_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("cost_center_id"));

				if(user.GetType() == "agency")
				{
					if(isCostCenterBelongsToAgency(cost_center_id, &db, &user))
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"bt\":[" << GetBTsInJSONFormat(
																"SELECT * FROM `bt` WHERE `status`=\"approved\" AND ("
																	"`id` IN ("
																		"SELECT `id` FROM `bt` WHERE `customer_id` IN ("
																			"SELECT `timecard_customer_id` FROM `cost_center_assignment` WHERE `cost_center_id`=\"" + cost_center_id + "\""
																		")"
																	")"
																") AND NOT("
																	"`id` IN ("
																		"SELECT `bt_id` FROM `invoice_cost_center_bt_details` WHERE `invoice_cost_center_bt_id` IN ("
																			"SELECT `id` FROM `invoice_cost_center_bt` WHERE `cost_center_id`=\"" + cost_center_id + "\""
																		")"
																	")"
																")", &db, &user, true) << "]";
						ostResult << "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employed");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
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
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getSubcontractorCompaniesAutocompleteList")
		{
			string			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("name"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			autocomplete_str = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(user.GetType() == "agency")
			{
				if(name.length())
				{
					int	affected = db.Query("SELECT `id`, `name` FROM `company` WHERE `name` LIKE \"%" + name + "%\" AND `type`=\"subcontractor\" LIMIT 0, 20;");

					for(auto i = 0; i < affected; ++i)
					{
						if(i) autocomplete_str += ",";
						autocomplete_str += "{\"id\":\"" + db.Get(i, "id") + "\","
								             "\"label\":\"" + db.Get(i, "name") + "\"}";
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", "name is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
				error_message = gettext("You are not authorized");
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\","
						  << "\"autocomplete_list\":[" << autocomplete_str << "]}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_addSoW")
		{
			auto			subcontractor_company_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("company_id"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;
			auto			sow_id = 0l;

			ostResult.str("");

			if(user.GetType() == "agency")
			{
				error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
				if(error_message.empty())
				{
					auto	position_id = GetPositionByCompanyID(subcontractor_company_id, &db, &user);

					if(position_id.length())
					{
						auto	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							sow_id = db.InsertQuery("INSERT INTO `contracts_sow` ("
																					"`subcontractor_company_id`,"
																					"`agency_company_id`,"
																					"`company_position_id`,"
																					"`start_date`,"
																					"`end_date`,"
																					"`number`,"
																					"`sign_date`,"
																					"`timecard_period`,"
																					"`subcontractor_create_tasks`,"
																					"`day_rate`,"
																					"`creator_user_id`,"
																					"`eventTimestamp`"
																				") VALUES ("
																					"\"" + subcontractor_company_id + "\","
																					"\"" + agency_id + "\","
																					"\"" + position_id + "\","
																					"NOW(),"
																					"DATE_SUB(DATE_ADD(NOW(), INTERVAL 1 YEAR), INTERVAL 1 DAY),"
																					"\"--------------\","
																					"NOW(),"
																					"\"month\","
																					"\"N\","
																					"\"0\","
																					"\"" + user.GetID() + "\","
																					"UNIX_TIMESTAMP()"
																				")"
													);
							if(sow_id)
							{
								auto	subcontractor_user_id = ""s;
								auto	temp_error_message = NotifySoWContractPartiesAboutChanges(to_string(NOTIFICATION_AGENCY_INITIATED_SOW), to_string(sow_id), &db, &user);

								success_message = ",\"sow_id\":\""s + to_string(sow_id) + "\"";

								// --- notifications
								if(temp_error_message.length())
								{
									MESSAGE_ERROR("", action, temp_error_message);
								}
							}
							else
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, "fail to create sow");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "fail to determine agency_id");
						}
					}
					else
					{
						error_message = gettext("SQL syntax error");
						MESSAGE_ERROR("", action, "fail to determine position_id");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
				error_message = gettext("You are not authorized");
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_deleteSoW")
		{
			auto			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(sow_id.length())
			{
				if(user.GetType() == "agency")
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);

					if(error_message.empty())
					{
						auto	timecards_reported = 0l;
						auto	bt_reported = 0l;
						auto	airtickets_purchased = 0l;

						if(db.Query("SELECT count(*) FROM `bt` WHERE `contract_sow_id`=\"" + sow_id + "\";"))
						{
							bt_reported = stod_noexcept(db.Get(0, 0));

							if(db.Query("SELECT count(*) FROM `timecards` WHERE `contract_sow_id`=\"" + sow_id + "\";"))
							{
								timecards_reported = stod_noexcept(db.Get(0, 0));

								if(db.Query("SELECT count(*) FROM `airline_bookings` WHERE `contract_sow_id`=\"" + sow_id + "\";"))
								{
									airtickets_purchased = stod_noexcept(db.Get(0, 0));
								}
							}
						}

						if(bt_reported + timecards_reported + airtickets_purchased)
						{
							error_message = gettext("Can't remove")  + " ("s + to_string(timecards_reported) + " " + gettext("timecards reported") + " " + gettext("and") + " " + to_string(bt_reported) + " " + gettext("bt-s reported") + " " + gettext("and") + " " + to_string(airtickets_purchased) + " " + gettext("tickets purchased") + ").";
						}
						else
						{
							auto	affected = db.Query("SELECT `value` FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `type`=\"file\";");
							for(auto i = 0; i < affected; ++i)
							{
								unlink((config.GetFromFile("image_folders", "template_sow") + "/" + db.Get(i, "value")).c_str());
							}

							affected = db.Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `type`=\"file\" AND  `contract_psow_id` IN ("
													"SELECT `id` FROM `contracts_psow` WHERE `contract_sow_id`=\"" + sow_id + "\""
												");");
							for(auto i = 0; i < affected; ++i)
							{
								unlink((config.GetFromFile("image_folders", "template_psow") + "/" + db.Get(i, "value")).c_str());
							}

							affected = db.Query("SELECT `filename` FROM `contract_sow_agreement_files` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							for(auto i = 0; i < affected; ++i)
							{
								unlink((config.GetFromFile("image_folders", "template_agreement_sow") + "/" + db.Get(i, 0)).c_str());
							}

							affected = db.Query("SELECT `agreement_filename` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";");
							if(affected)
							{
								if(db.Get(0, 0).length())
								{
									unlink((config.GetFromFile("image_folders", "AGREEMENTS_SOW_DIRECTORY") + "/" + db.Get(0, 0)).c_str());
								}
								else
								{
									MESSAGE_DEBUG("", "", "no agreements assigned to SoW. nothing to delete.");
								}
							}

							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_AGENCY_GENERATED_AGREEMENTS) + "\" AND `actionId`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_AGENCY_INITIATED_SOW) + "\" AND `actionId`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_SUBCONTRACTOR_SIGNED_SOW) + "\" AND `actionId`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `contract_sow_agreement_files` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `timecard_approvers` WHERE `contract_psow_id` IN (" + Get_PSoWIDsBySoWID_sqlquery(sow_id) + ");");
							db.Query("DELETE FROM `bt_approvers` WHERE `contract_psow_id` IN (" + Get_PSoWIDsBySoWID_sqlquery(sow_id) + ");");
							db.Query("DELETE FROM `bt_sow_assignment` WHERE `sow_id`=\"" + sow_id + "\";");

							db.Query("DELETE FROM `contract_psow_custom_fields` WHERE `contract_psow_id` IN ("
										"SELECT `id` FROM `contracts_psow` WHERE `contract_sow_id`=\"" + sow_id + "\""
									");");

							db.Query("DELETE FROM `contracts_psow` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = gettext("You are not authorized");
				}
			}
			else
			{
				error_message = gettext("parameters incorrect");
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getTemplateAgreementFiles")
		{
			auto			entity_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			entity_type = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("type"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(entity_id.length())
			{
				if((entity_type == "company") || (entity_type == "sow"))
				{
					if(user.GetType() == "agency")
					{
						if(entity_type == "sow")
						{
							auto	sql_query = "SELECT * FROM `contract_sow_agreement_files` WHERE `contract_sow_id`=\"" + entity_id + "\"";
							success_message = ",\"template_agreement_files\":[" + GetTemplateSoWAgreementFiles(sql_query, &db, &user) + "]";
						}
						else if(entity_type == "company")
						{
							auto	sql_query = "SELECT * FROM `company_agreement_files` WHERE `company_id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ")";
							success_message = ",\"template_agreement_files\":[" + GetTemplateCompanyAgreementFiles(sql_query, &db, &user) + "]";
						}
						else
						{
							error_message = gettext("parameters incorrect");
							MESSAGE_ERROR("", action, "entity_type (" + entity_type + ") not found");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
						error_message = gettext("You are not authorized");
					}
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_ERROR("", action, "entity_type (" + entity_type + ") is incorrect");
				}
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_ERROR("", action, "entity_id is empty");
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_addTemplateAgreementFile")
		{
			auto			entity_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			entity_type = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("type"));
			auto			entity_title = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("title"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(entity_id.length() && entity_title.length())
			{
					if(entity_type == "company")
					{
						if((error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user)).empty())
						{
							auto	company_id = GetAgencyID(&user, &db);
							if(company_id.length())
							{
								if((error_message = CheckAgreementSoWTitle(entity_title, "", &db, &user)).empty())
								{
									auto	new_id = db.InsertQuery("INSERT INTO `company_agreement_files` (`company_id`, `title`, `owner_user_id`, `eventTimestamp`) VALUES ("
															"\"" + company_id + "\"," 
															"\"" + entity_title + "\"," 
															"\"" + user.GetID() + "\"," 
															"UNIX_TIMESTAMP()" 
														")");
									if(new_id)
									{
										success_message = ",\"id\":\"" + to_string(new_id) + "\"";
									}
									else
									{
										error_message = gettext("SQL syntax error");
										MESSAGE_ERROR("", action, error_message);
									}
								}
								else
								{
									MESSAGE_DEBUG("", "", "error returned from CheckAgreementSoWTitle");
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "can't define company id by user.id(" + user.GetID() + ")");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not allowed to change agency data");
						}
					}
					else if(entity_type == "sow")
					{
						if((error_message = isAgencyEmployeeAllowedToChangeSoW(entity_id, &db, &user)).empty())
						{
							if((error_message = CheckAgreementSoWTitle(entity_title, entity_id, &db, &user)).empty())
							{
								auto	new_id = db.InsertQuery("INSERT INTO `contract_sow_agreement_files` (`contract_sow_id`, `title`, `owner_user_id`, `eventTimestamp`) VALUES ("
														"\"" + entity_id + "\"," 
														"\"" + entity_title + "\"," 
														"\"" + user.GetID() + "\"," 
														"UNIX_TIMESTAMP()" 
													")");
								if(new_id)
								{
									success_message = ",\"id\":\"" + to_string(new_id) + "\"";
								}
								else
								{
									error_message = gettext("SQL syntax error");
									MESSAGE_ERROR("", action, error_message);
								}
							}
							else
							{
								MESSAGE_DEBUG("", "", "error returned from CheckAgreementSoWTitle");
							}
						}
						else
						{
							error_message = gettext("You are not authorized");
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not allowed to change SoW(" + entity_id + ")");
						}
					}
					else
					{
						error_message = gettext("parameters incorrect");
						MESSAGE_ERROR("", action, "entity_type (" + entity_type + ") is incorrect");
					}
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_ERROR("", action, "entity_id is empty");
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(
			(action == "AJAX_updateTemplateAgreement_company_Title") ||
			(action == "AJAX_deleteTemplateAgreement_company")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name	= "json_response.htmlt"s;
				auto			error_message	= ""s;

				auto			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(new_value.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						auto	agency_id = GetAgencyID(&user, &db);
						if(agency_id.length())
						{
							{
								error_message = isActionEntityBelongsToAgency(action, id, agency_id, &db, &user);
								if(error_message.empty())
								{
									error_message = CheckNewValueByAction(action, id, "", new_value, &db, &user);
									if(error_message.empty())
									{
										if((action.find("update") != string::npos))
										{
											// string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

											error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
											if(error_message.empty())
											{
												// --- good finish
											}
											else
											{
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + new_value + "])");
											}
										}
										else if(action.find("insert") != string::npos)
										{
											
										}
										else if(action.find("delete") != string::npos)
										{
											error_message = DeleteEntryByAction(action, id, &config, &db, &user);
											if(error_message.empty())
											{
												// --- good finish
											}
											else
											{
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + new_value + "])");
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
										}

									}
									else
									{
										MESSAGE_DEBUG("", action, "new value failed to pass validity check");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
								}
							}
						}
						else
						{
							error_message = "Агенство не найдено";
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
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


		if(
			(action == "AJAX_updateTemplateAgreement_sow_Title") ||
			(action == "AJAX_deleteTemplateAgreement_sow")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name	= "json_response.htmlt"s;
				auto			error_message	= ""s;

				auto			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));
				auto			sow_id			= ""s;

				if(db.Query("SELECT `contract_sow_id` FROM `contract_sow_agreement_files` WHERE `id`=\"" + id + "\";"))
				{
					sow_id = db.Get(0, 0);
				}
				else
				{
					MESSAGE_ERROR("", action, "fail to determine sow.id by contract_sow_agreement_files.id(" + id + ")");
				}

				if(new_value.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						auto	agency_id = GetAgencyID(&user, &db);
						if(agency_id.length())
						{

							{
								error_message = isActionEntityBelongsToAgency(action, id, agency_id, &db, &user);
								if(error_message.empty())
								{
									error_message = CheckNewValueByAction(action, id, sow_id, new_value, &db, &user);
									if(error_message.empty())
									{
										if((action.find("update") != string::npos))
										{
											// string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

											error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
											if(error_message.empty())
											{
												// --- good finish
											}
											else
											{
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + new_value + "])");
											}
										}
										else if(action.find("insert") != string::npos)
										{
											
										}
										else if(action.find("delete") != string::npos)
										{
											error_message = DeleteEntryByAction(action, id, &config, &db, &user);
											if(error_message.empty())
											{
												// --- good finish
											}
											else
											{
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + new_value + "])");
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
										}

									}
									else
									{
										MESSAGE_DEBUG("", action, "new value failed to pass validity check");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
								}
							}
						}
						else
						{
							error_message = "Агенство не найдено";
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
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

		if(action == "AJAX_generateSoWAgreementDocuments")
		{
			MESSAGE_DEBUG("", action, "start");

			auto	template_name	= "json_response.htmlt"s;
			auto	error_message	= ""s;
			auto	success_message	= ""s;

			auto	sow_id			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			if(sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					auto	agency_id = GetAgencyID(&user, &db);
					if(agency_id.length())
					{
						if(db.Query("SELECT `status` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))	
						{
							if(db.Get(0, 0) == "negotiating")
							{
								C_Agreements_SoW_Factory	agreements(&config, &db, &user);

								agreements.SetSoWID(sow_id);

								if((error_message = agreements.GenerateDocumentArchive()).empty())
								{
									success_message = "\"filename\":\"" + agreements.GetShortFilename() + "\"";

									{
										auto	temp_error_message = NotifySoWContractPartiesAboutChanges(to_string(NOTIFICATION_AGENCY_GENERATED_AGREEMENTS), sow_id, &db, &user);

										if(temp_error_message.length()) MESSAGE_ERROR("", action, temp_error_message);
									}

								}
								else
								{
									MESSAGE_ERROR("", action, error_message);
								}
							}
							else
							{
								error_message = gettext("Agreement already signed");
								MESSAGE_ERROR("", action, error_message + ". Workflow should not be here.");
							}
						}
						else
						{
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", action, error_message + ". Workflow should not be here.");
						}

					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
				}
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteSoWAgreementDocuments")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto	template_name	= "json_response.htmlt"s;
				auto	error_message	= ""s;

				auto	sow_id			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(sow_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						auto	agency_id = GetAgencyID(&user, &db);
						if(agency_id.length())
						{
							if(db.Query("SELECT `status` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))	
							{
								if(db.Get(0, 0) == "negotiating")
								{
									auto	affected = db.Query("SELECT `agreement_filename` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";");

									if(affected)
									{
										if(db.Get(0, 0).length())
										{
											unlink((config.GetFromFile("image_folders", "AGREEMENTS_SOW_DIRECTORY") + "/" + db.Get(0, 0)).c_str());
										}
										else
										{
											MESSAGE_DEBUG("", "", "no agreements assigned to SoW. nothing to delete.");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "sow_id(" + sow_id + ") not found. Workflow must not be here. Investigate reason.");
									}

									db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_AGENCY_GENERATED_AGREEMENTS) + "\" AND `actionId`=\"" + sow_id + "\";");
									db.Query("UPDATE `contracts_sow` SET `agreement_filename`=\"\" WHERE `id`=\"" + sow_id + "\";");
								}
								else
								{
									error_message = gettext("Agreement already signed");
									MESSAGE_ERROR("", action, error_message + ". Workflow should not be here.");
								}
							}
							else
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", action, error_message + ". Workflow should not be here.");
							}


						}
						else
						{
							error_message = "Агенство не найдено";
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
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

		if(action == "AJAX_payForTheList")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			entity = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("entity"));
			auto			list_param = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("list"));

			auto			error_message = ""s;
			auto			success_message = ""s;

			auto			list = split(list_param, ',');

			for(auto i = 0u; i < list.size() && error_message.empty(); ++i)
			{
				auto	sql_query = ""s;

				if(entity == "timecard") sql_query = "SELECT `contract_sow_id` FROM `timecards` WHERE `id`=" + quoted(list[i]) + ";";
				else if(entity == "bt") sql_query = "SELECT `contract_sow_id` FROM `bt` WHERE `id`=" + quoted(list[i]) + ";";
				else
				{
					error_message = gettext("entity type is unknown");
					MESSAGE_ERROR("", action, error_message);
				}

				if(error_message.empty())
				{
					if(db.Query(sql_query))
					{
						auto	sow_id = db.Get(0, 0);

						error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
						if(error_message.empty())
						{
							auto	sql_query = ""s;

							if(entity == "timecard") sql_query = "UPDATE `timecards` SET `payed_date`=UNIX_TIMESTAMP() WHERE `id`=" + quoted(list[i]) + ";";
							else if(entity == "bt") sql_query = "UPDATE `bt` SET `payed_date`=UNIX_TIMESTAMP() WHERE `id`=" + quoted(list[i]) + ";";
							else
							{
								error_message = gettext("entity type is unknown");
								MESSAGE_ERROR("", action, error_message);
							}

							db.Query(sql_query);
							if(db.isError())
							{
								error_message = db.GetErrorMessage();
								MESSAGE_ERROR("", action, error_message);
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
						}
					}
					else
					{
						error_message = gettext("SoW not known");
						MESSAGE_ERROR("", action, error_message);
					}
				}
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(
			(action == "AJAX_updateBTOriginalDocumentsDelivery") ||
			(action == "AJAX_updateTimecardOriginalDocumentsDelivery")
		)
		{
			MESSAGE_DEBUG("", action, "start");

			auto			timestamp	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timestamp"));
			auto			list_param	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("id"));
			auto			entity		= (
											action == "AJAX_updateTimecardOriginalDocumentsDelivery"? "timecard"s :
											action == "AJAX_updateBTOriginalDocumentsDelivery"		? "bt"s :
											""s
											);

			auto			error_message = ""s;
			auto			success_message = ""s;

			auto			list = split(list_param, ',');

			if(entity.length())
			{
				for(auto i = 0u; i < list.size() && error_message.empty(); ++i)
				{
					auto	sql_query = ""s;

					if(entity == "timecard")	sql_query = "SELECT `contract_sow_id` FROM `timecards` WHERE `id`=" + quoted(list[i]) + ";";
					else if(entity == "bt")		sql_query = "SELECT `contract_sow_id` FROM `bt` WHERE `id`=" + quoted(list[i]) + ";";
					else
					{
						error_message = gettext("entity type is unknown");
						MESSAGE_ERROR("", action, error_message);
					}

					if(error_message.empty())
					{
						if(db.Query(sql_query))
						{
							auto	sow_id = db.Get(0, 0);

							error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
							if(error_message.empty())
							{
								auto	sql_query = ""s;

								if(entity == "timecard")	sql_query = "UPDATE `timecards` SET `originals_received_date`=" + quoted(timestamp) + " WHERE `id`=" + quoted(list[i]) + ";";
								else if(entity == "bt")		sql_query = "UPDATE `bt` SET `originals_received_date`=" + quoted(timestamp) + " WHERE `id`=" + quoted(list[i]) + ";";
								else
								{
									error_message = gettext("entity type is unknown");
									MESSAGE_ERROR("", action, error_message);
								}

								db.Query(sql_query);
								if(db.isError())
								{
									error_message = db.GetErrorMessage();
									MESSAGE_ERROR("", action, error_message);
								}

								if(error_message.empty())
								{
									if((error_message = Update_TimecardBT_ExpectedPayDate(entity, list[i], &db, &user)).empty()) 
									{
										if(GeneralNotifySoWContractPartiesAboutChanges(action, list[i], sow_id, GetDBValueByAction(action, list[i], sow_id, &db, &user), timestamp, &db, &user))
										{
											// --- don't do anything here
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, error_message);
									}
								}
								
								if(error_message.empty())
								{
									if(entity == "timecard")
									{
										success_message += GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `id`=" + quoted(list[i]) + ";", &db, &user, false);
									}
									else if(entity == "bt")
									{
										success_message += GetBTsInJSONFormat("SELECT * FROM `bt` WHERE `id`=" + quoted(list[i]) + ";", &db, &user, false);
									}
									else
									{
										error_message = gettext("entity type is unknown");
										MESSAGE_ERROR("", action, error_message);
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
							}
						}
						else
						{
							error_message = gettext("SoW not known");
							MESSAGE_ERROR("", action, error_message);
						}
					}

				}
			}
			else
			{
				error_message = gettext("entity type is unknown");
				MESSAGE_ERROR("", action, error_message);
			}


			if(success_message.length()) success_message = "\"items\":[" + success_message + "]";

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getSoWListWithCustomFields")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			error_message = ""s;
			auto			success_message = ""s;

			auto			agency_id = GetAgencyID(&user, &db);

			if(agency_id.length())
			{
				unordered_set<string>	sow_s;
				auto					affected = db.Query("SELECT DISTINCT(`contract_sow_id`) FROM `contract_sow_custom_fields` WHERE `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=" + quoted(agency_id) + ");");

				for(auto i = 0; i < affected; ++i)
				{
					sow_s.insert(db.Get(i, 0));
				}

				affected = db.Query("SELECT DISTINCT(`contract_sow_id`) FROM `contracts_psow` WHERE "
										"`id` IN (SELECT `contract_psow_id` FROM `contract_psow_custom_fields`) "
										"AND "
										"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=" + quoted(agency_id) + ")"
										";"
									);

				for(auto i = 0; i < affected; ++i)
				{
					sow_s.insert(db.Get(i, 0));
				}

				success_message = "\"sow\":[";

				if(sow_s.size())
				{
					auto	sow_list = ""s;

					for(auto &sow: sow_s)
					{
						if(sow_list.length()) sow_list += ",";
						sow_list += sow;
					}

					success_message += GetSOWInJSONFormat(	"SELECT * FROM `contracts_sow` WHERE "
																"`id` IN (" + sow_list + ") "
															";", &db, &user, false, false, false, true);


				}
				else
				{
					MESSAGE_DEBUG("", "", "sow_list is empty");
				}

				success_message += "]";

			}
			else
			{
				error_message = gettext("you are not authorized");
				MESSAGE_ERROR("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_copyCustomFieldsFromSoW")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			error_message = ""s;
			auto			success_message = ""s;

			auto			sow_id_to = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id_to"));
			auto			sow_id_from = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id_from"));
			auto			with_values = indexPage.GetVarsHandler()->Get("with_values") == "Y";


			error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id_to, &db, &user);
			if(error_message.empty())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id_from, &db, &user);
				if(error_message.empty())
				{
					auto			agency_id = GetAgencyID(&user, &db);

					if(agency_id.length())
					{
						// --- copy SoW custom fields
						db.Query("INSERT INTO `contract_sow_custom_fields` ("
										"`contract_sow_id`,"
										"`var_name`,"
										"`title`,"
										"`description`,"
										"`type`,"
										"`value`,"
										"`visible_by_subcontractor`,"
										"`editable_by_subcontractor`,"
										"`owner_user_id`,"
									    "`eventTimestamp`"
									") "
									"SELECT "
										"\"" + sow_id_to + "\" AS `contract_sow_id`,"
										"a.`var_name` AS `var_name`,"
										"a.`title` AS `title`,"
										"a.`description` AS `description`,"
										"a.`type` AS `type`,"
										+ (with_values ? "a.`value`" : "\"\"") + " AS `value`,"
										"a.`visible_by_subcontractor` AS `visible_by_subcontractor`,"
										"a.`editable_by_subcontractor` AS `editable_by_subcontractor`,"
										"\"" + user.GetID() + "\" AS `owner_user_id`,"
										"UNIX_TIMESTAMP() AS `eventTimestamp` "
									"FROM (SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=" + quoted(sow_id_from) + ") a "
									"LEFT JOIN (SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=" + quoted(sow_id_to) + ") b "
									"USING(`var_name`) "
									"WHERE b.`id` is NULL"
									";"
									);

						if(db.isError())
						{
							error_message = gettext("SQL syntax error");
						}
						else
						{
							// --- copy PSoW custom fields

							map<string, string>	sow_id_from__cost_center_list;
							map<string, string>	sow_id_to__cost_center_list;
							auto				affected = db.Query("SELECT `cost_center_id`,`id` FROM `contracts_psow` WHERE `contract_sow_id`=" + quoted(sow_id_to) + ";");

							for(auto i = 0; i < affected; ++i)
							{
								sow_id_to__cost_center_list[db.Get(i, "cost_center_id")] = db.Get(i, "id");
							}

							affected = db.Query("SELECT `cost_center_id`,`id` FROM `contracts_psow` WHERE `contract_sow_id`=" + quoted(sow_id_from) + ";");

							for(auto i = 0; i < affected; ++i)
							{
								sow_id_from__cost_center_list[db.Get(i, "cost_center_id")] = db.Get(i, "id");
							}

							for(auto &curr_pair: sow_id_to__cost_center_list)
							{
								auto	cost_center_id = curr_pair.first;
								if(error_message.empty())
								{
									auto	psow_id_to = sow_id_to__cost_center_list[cost_center_id];
									auto	psow_id_from = sow_id_from__cost_center_list[cost_center_id];

									if(	psow_id_to.length() && psow_id_from.length())
									{
										db.Query("INSERT INTO `contract_psow_custom_fields` ("
														"`contract_psow_id`,"
														"`var_name`,"
														"`title`,"
														"`description`,"
														"`type`,"
														"`value`,"
														"`visible_by_subcontractor`,"
														"`editable_by_subcontractor`,"
														"`owner_user_id`,"
													    "`eventTimestamp`"
													") "
													"SELECT "
														"\"" + psow_id_to + "\" AS `contract_psow_id`,"
														"a.`var_name` AS `var_name`,"
														"a.`title` AS `title`,"
														"a.`description` AS `description`,"
														"a.`type` AS `type`,"
														+ (with_values ? "a.`value`" : "\"\"") + " AS `value`,"
														"a.`visible_by_subcontractor` AS `visible_by_subcontractor`,"
														"a.`editable_by_subcontractor` AS `editable_by_subcontractor`,"
														"\"" + user.GetID() + "\" AS `owner_user_id`,"
														"UNIX_TIMESTAMP() AS `eventTimestamp` "
													"FROM (SELECT * FROM `contract_psow_custom_fields` WHERE `contract_psow_id`=" + quoted(psow_id_from) + ") a "
													"LEFT JOIN (SELECT * FROM `contract_psow_custom_fields` WHERE `contract_psow_id`=" + quoted(psow_id_to) + ") b "
													"USING(`var_name`) "
													"WHERE b.`id` is NULL"
													";"
													);

										if(db.isError())
										{
											error_message = gettext("SQL syntax error");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "can't copy CostCenter.id(" + cost_center_id + ") custom fields: PSoW() -> PSoW(" + psow_id_to + ")");
									}
								}
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
					MESSAGE_ERROR("", action, error_message);
				}
			}
			else
			{
				MESSAGE_ERROR("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_downloadBTAttachments")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			error_message = ""s;
			auto			success_message = ""s;

			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));

			if(id.length())
			{
				if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(" + Get_CompanyIDByCompanyEmployeeUserID_sqlquery(user.GetID()) + ");"))
				{
					auto			agency_id = db.Get(0, "id");
					vector<string>	bts = {id};

					if((error_message = isBTsBelongToAgency(bts, agency_id, &db, &user)).empty())
					{
						C_Download_BT_Attachments	bt_attachments(&config, &db, &user);
						bt_attachments.SetBTID(id);

						if((error_message = bt_attachments.GenerateDocumentArchive()).empty())
						{
							success_message = "\"href\":\"" + bt_attachments.GetArchiveFilename() + "\"";
						}
						else
						{
							MESSAGE_ERROR("", action, error_message);
						}
					}
					else
					{
						error_message = gettext("you are not authorized");
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") isn't agency employee");
					}
				}
				else
				{
					error_message = gettext("You are not agency employee");
					MESSAGE_ERROR("", action, "Can't define agency.id by user.id(" + user.GetID() + ")");
				}
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_ERROR("", action, error_message);
			}


			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getHolidayCalendar")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			error_message = ""s;
			auto			success_message = ""s;

			auto			agency_id = GetAgencyID(&user, &db);

			if(agency_id.length())
			{
				success_message = "\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat("SELECT * FROM `holiday_calendar` WHERE `agency_company_id` IN (SELECT `company_id` FROM `company_employees` WHERE `user_id` IN (" + quoted(user.GetID()) + "));", &db, &user) + "]";
			}
			else
			{
				error_message = gettext("you are not authorized");
				MESSAGE_ERROR("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_recallTimecard")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
			auto			reason = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("reason"));

			auto			error_message = RecallTimecard(id, reason, &config, &db, &user);
			auto			success_message = ""s;

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");

		indexPage.OutTemplate();

	}
	catch(CExceptionHTML &c)
	{
		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);

		MESSAGE_DEBUG("", action, "catch CExceptionHTML: DEBUG exception reason: [" + c.GetReason() + "]");

		if(!indexPage.SetTemplate(c.GetTemplate()))
		{
			MESSAGE_ERROR("", "", "template (" + c.GetTemplate() + ") not found");
			return(-1);
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();

		return(-1);
	}
	catch(CException &c)
	{
		MESSAGE_ERROR("", action, "catch CException: exception: ERROR  " + c.GetReason());

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			MESSAGE_ERROR("", "", "template not found");
			return(-1);
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();

		return(-1);
	}
	catch(exception& e)
	{
		MESSAGE_ERROR("", action, "catch(exception& e): catch standard exception: ERROR  " + e.what());

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			MESSAGE_ERROR("", "", "template not found");
			return(-1);
		}
		
		indexPage.RegisterVariable("content", e.what());
		indexPage.OutTemplate();

		return(-1);
	}

	return(0);
}
