#include "agency.h"

int main(void)
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	CMysql			db;
	string			action = "";

	MESSAGE_DEBUG("", action, __FILE__);

	signal(SIGSEGV, crash_handler);

	// --- randomization
	{
		struct timeval	tv;
		gettimeofday(&tv, NULL);
		srand(tv.tv_sec * tv.tv_usec * 100000);
	}

	try
	{

		indexPage.ParseURL();
		indexPage.AddCookie("lng", "ru", "", "", "/");

		if(!indexPage.SetTemplate("index.htmlt"))
		{
			MESSAGE_ERROR("", action, "template file was missing");
			throw CException("Template file was missing");
		}

		if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
		{
			MESSAGE_ERROR("", action, "Can not connect to mysql database");
			throw CExceptionHTML("MySql connection");
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
			action = GenerateSession(action, &indexPage, &db, &user);
			action = LogoutIfGuest(action, &indexPage, &db, &user);
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
				string			template_name = "json_response.htmlt";
				string			object = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("object"));;
				string			filter = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter"));;

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

		if(action == "AJAX_getTimecardList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			string			template_name = "json_response.htmlt";
			int				affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

			if(affected)
			{
				string		companies_list= "";

				for(int i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, 0);
				}

				ostResult << "{"
								"\"status\":\"success\","
								"\"sow\":[" << GetSOWInJSONFormat(
										"SELECT * FROM `contracts_sow` WHERE "
											"`agency_company_id` IN (" + companies_list + ") "
										";", &db, &user) << "],"
								"\"timecards\":[" << GetTimecardsInJSONFormat(
										"SELECT * FROM `timecards` WHERE "
											"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (" + companies_list + "))"
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getBTList")
		{
			ostringstream	ostResult;
			auto			template_name = "json_response.htmlt"s;
			auto			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			if(affected)
			{
				auto		companies_list= ""s;

				for(auto i = 0; i < affected; ++i)
				{
					if(companies_list.length()) companies_list += ",";
					companies_list += db.Get(i, 0);
				}

				ostResult << "{"
								"\"status\":\"success\","
								"\"sow\":[" << GetSOWInJSONFormat(
										"SELECT * FROM `contracts_sow` WHERE "
											"`agency_company_id` IN (" + companies_list + ") "
										";", &db, &user) << "],"
								"\"bt\":[" << GetBTsInJSONFormat(
										"SELECT * FROM `bt` WHERE "
											"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (" + companies_list + "))"
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
			}

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
						if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\"))"))
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
						if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
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
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			timecard_id = "";
				string			comment = "";

				timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(timecard_id.length())
				{
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
										"`approver_user_id`=\"" + user.GetID() + "\" "
										"AND "
										"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
										";"))
						{
							string		approver_id = db.Get(0, "id");

							if(db.Query("SELECT `status`, `submit_date` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								string	timecard_state = db.Get(0, "status");
								string	timecard_submit_date = db.Get(0, "submit_date");

								if(timecard_state == "submit")
								{
									{
										if(db.Query("SELECT `id` FROM `timecard_approvals` WHERE `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";"))
										{
											db.Query("UPDATE `timecard_approvals` SET `decision`=\"approved\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";");
											if(!db.isError())
											{
												if(SubmitTimecard(timecard_id, &db, &user))
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("timecard", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending timecards");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка подтверждения таймкарты\"}";
													MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + "): ");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
												MESSAGE_ERROR("", action, "fail to insert to timecards table with timecard_id(" + timecard_id + ")");
											}
										}
										else
										{
											// --- this approver already approved this bt
											ostResult << "{\"result\":\"error\",\"description\":\"Вы уже подтвердили\"}";
											MESSAGE_ERROR("", action, "approver.id(" + approver_id + ") already approved timecard.id(" + timecard_id + ") at timestamp(" + db.Get(0, "eventTimestamp") + ")");
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
									MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") have to be in submit state to be approved/rejected, but it is in \"" + timecard_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"таймкарта не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about timecard.id(" + timecard_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
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

		if(action == "AJAX_rejectTimecard")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			timecard_id = "";
				string			comment = "";

				timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(timecard_id.length())
				{
					if(comment.empty())
					{
						MESSAGE_ERROR("", action, "reject reason is empty");
						ostResult << "{\"result\":\"error\",\"description\":\"Необходимо обосновать отказ\"}";
					}
					else
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
										"`approver_user_id`=\"" + user.GetID() + "\" "
										"AND "
										"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
										";"))
						{
							string		approver_id = db.Get(0, "id");

							if(db.Query("SELECT `status`, `submit_date` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								string	timecard_state = db.Get(0, "status");
								string	timecard_submit_date = db.Get(0, "submit_date");

								if(timecard_state == "submit")
								{
									{
										if(db.Query("SELECT `id` FROM `timecard_approvals` WHERE `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";"))
										{
											db.Query("UPDATE `timecard_approvals` SET `decision`=\"rejected\",`comment`=" + quoted(comment) + ", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `timecard_id`=" + quoted(timecard_id) + ";");
											if(!db.isError())
											{
												if(SubmitTimecard(timecard_id, &db, &user))
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("timecard", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending timecards");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка отправки таймкарты\"}";
													MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + ")");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
												MESSAGE_ERROR("", action, "fail to update timecard_id(" + timecard_id + "): " + db.GetErrorMessage());
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") not in pending state on user.id(" + user.GetID() + ")");
											ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать эту таймкарту\"}";
										}
									}
								}
								else
								{
									MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") have to be in submit state to be approved/rejected, but it is in \"" + timecard_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"таймкарта не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about timecard.id(" + timecard_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
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

		if(action == "AJAX_rejectBT")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			bt_id = "";
				string			comment = "";

				bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(bt_id.length())
				{
					if(comment.empty())
					{
						MESSAGE_ERROR("", action, "reject reason is empty");
						ostResult << "{\"result\":\"error\",\"description\":\"Необходимо обосновать отказ\"}";
					}
					else
					{
						if((user.GetType() == "approver") || (user.GetType() == "agency"))
						{
							// --- check ability to see this bt
							string	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
							if(error_string.empty())
							{
								if(db.Query("SELECT `status` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
								{
									string	bt_state = db.Get(0, "status");

									if(bt_state == "submit")
									{

										if(db.Query("SELECT `id` FROM `bt_approvers` WHERE "
														"`approver_user_id`=\"" + user.GetID() + "\" "
														"AND "
														"`contract_sow_id`=( SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
														";"))
										{
											string		approver_id = db.Get(0, "id");
/*
											long int	temp;

											temp = db.InsertQuery(	"INSERT INTO `bt_approvals` SET "
																	"`bt_id`=\"" + bt_id + "\","
																	"`approver_id`=\"" + approver_id + "\","
																	"`decision`=\"rejected\","
																	"`comment`=\"" + comment + "\","
																	"`eventTimestamp`=UNIX_TIMESTAMP();"
																);

											if(temp)
											{
												db.Query("UPDATE `bt` SET `status`=\"rejected\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + bt_id + "\";");
												if(!db.isError())
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending bt");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
													MESSAGE_ERROR("", action, "fail to update bt_id(" + bt_id + "): " + db.GetErrorMessage());
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
												MESSAGE_ERROR("", action, "fail to insert to bt table with bt_id(" + bt_id + ")");
											}
*/
											if(db.Query("SELECT `id` FROM `bt_approvals` WHERE `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";"))
											{
												db.Query("UPDATE `bt_approvals` SET `decision`=\"rejected\",`comment`=" + quoted(comment) + ", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";");
												if(!db.isError())
												{
													if(SubmitBT(bt_id, &db, &user))
													{
														string	success_description = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

														if(success_description.length())
														{
															ostResult	<< "{"
																		<< "\"result\":\"success\","
																		<< success_description
																		<< "}";
														}
														else
														{
															ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
															MESSAGE_ERROR("", action, "error getting info about pending bts");
														}
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка отправки таймкарты\"}";
														MESSAGE_ERROR("", action, "fail to submit bt_id(" + bt_id + ")");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
													MESSAGE_ERROR("", action, "fail to update bt_id(" + bt_id + "): " + db.GetErrorMessage());
												}
											}
											else
											{
												MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") not in pending state on user.id(" + user.GetID() + ")");
												ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать эту таймкарту\"}";
											}

										}
										else
										{
											MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
											ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать командировку у данного договора\"}";
										}

									}
									else
									{
										MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
										ostResult << "{\"result\":\"error\",\"description\":\"командировка не требует подтверждения\"}";
									}
								}
								else
								{
									MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
									ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
								ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой командировке\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
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


		if(action == "AJAX_approveBT")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			bt_id = "";
				string			comment = "";

				bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(bt_id.length())
				{
					if((user.GetType() == "approver") || (user.GetType() == "agency"))
					{
						// --- check ability to see this bt
						string	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
						if(error_string.empty())
						{
							if(db.Query("SELECT `status` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
							{
								string	bt_state = db.Get(0, "status");

								if(bt_state == "submit")
								{

									if(db.Query("SELECT `id` FROM `bt_approvers` WHERE "
													"`approver_user_id`=\"" + user.GetID() + "\" "
													"AND "
													"`contract_sow_id`=( SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
													";"))
									{
										string		approver_id = db.Get(0, "id");

										if(db.Query("SELECT `id` FROM `bt_approvals` WHERE `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";"))
										{
											db.Query("UPDATE `bt_approvals` SET `decision`=\"approved\",`comment`=" + quoted(comment) + ", `eventTimestamp`=UNIX_TIMESTAMP() WHERE  `approver_id`=" + quoted(approver_id) + " AND `decision`=\"pending\" AND `bt_id`=" + quoted(bt_id) + ";");
											if(!db.isError())
											{
												if(SubmitBT(bt_id, &db, &user))
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending bts");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка отправки командировки\"}";
													MESSAGE_ERROR("", action, "fail to submit bt_id(" + bt_id + ")");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
												MESSAGE_ERROR("", action, "fail to update bt_id(" + bt_id + "): " + db.GetErrorMessage());
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") not in pending state on user.id(" + user.GetID() + ")");
											ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать эту командировку\"}";
										}


									}
									else
									{
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать командировку у данного договора\"}";
									}

								}
								else
								{
									MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"командировка не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой командировке\"}";
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
						ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой командировке\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
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

/*
		if(action == "AJAX_approveBT")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			bt_id = "";
				string			comment = "";

				bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(bt_id.length())
				{
					if((user.GetType() == "approver") || (user.GetType() == "agency"))
					{
						// --- check ability to see this bt
						string	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
						if(error_string.empty())
						{

							if(db.Query("SELECT `status`, `submit_date` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
							{
								string	bt_state = db.Get(0, "status");
								string	bt_submit_date = db.Get(0, "submit_date");

								if(bt_state == "submit")
								{

									// --- find approver_id for user and sow
									if(db.Query("SELECT `id` FROM `bt_approvers` WHERE "
													"`approver_user_id`=\"" + user.GetID() + "\" "
													"AND "
													"`contract_sow_id`=( SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
													";"))
									{
										string		approver_id = db.Get(0, "id");

										// --- did approver approved after user submission ?
										if(db.Query("SELECT `eventTimestamp` FROM `bt_approvals` WHERE `approver_id`=\"" + approver_id + "\" AND `bt_id`=\"" + bt_id + "\" AND `eventTimestamp`>\"" + bt_submit_date + "\";"))
										{
											// --- this approver already approved this bt
											ostResult << "{\"result\":\"error\",\"description\":\"Вы уже подтвердили\"}";
											MESSAGE_ERROR("", action, "approver.id(" + approver_id + ") already approved bt.id(" + bt_id + ") at timestamp(" + db.Get(0, "eventTimestamp") + ")");
										}
										else
										{
											long int	temp;

											temp = db.InsertQuery(	"INSERT INTO `bt_approvals` SET "
																	"`bt_id`=\"" + bt_id + "\","
																	"`approver_id`=\"" + approver_id + "\","
																	"`decision`=\"approved\","
																	"`comment`=\"" + comment + "\","
																	"`eventTimestamp`=UNIX_TIMESTAMP();"
																);

											if(temp)
											{
												if(SubmitBT(bt_id, &db, &user))
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending bt");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка подтверждения таймкарты\"}";
													MESSAGE_ERROR("", action, "fail to submit bt_id(" + bt_id + "): ");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
												MESSAGE_ERROR("", action, "fail to insert to bt table with bt_id(" + bt_id + ")");
											}
										}

									}
									else
									{
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать таймкарту у данного договора\"}";
									}

								}
								else
								{
									MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"таймкарта не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
						ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
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
*/
		if(action == "AJAX_getDashboardPendingData")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

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
			auto		affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

			if(affected)
			{
				auto sow_sql =	"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=("
									"SELECT `company_id` FROM `company_employees` WHERE `user_id`=" + quoted(user.GetID()) + 
								")";

				success_message = 
								"\"number_of_payment_pending_timecards\":" + GetNumberOfTimecardsInPaymentPendingState(sow_sql, &db, &user) + ","
								"\"timecard_late_payment\":" + quoted(to_string(areThereTimecardsWithExpiredPayment("1", sow_sql, &db, &user))) + ","
								"\"timecard_payment_will_be_late_soon\":" + quoted(to_string(areThereTimecardsWithExpiredPayment("1/2", sow_sql, &db, &user))) + ","
								"\"number_of_payment_pending_bt\":" + GetNumberOfBTInPaymentPendingState(sow_sql, &db, &user) + ","
								"\"bt_late_payment\":" + quoted(to_string(areThereBTWithExpiredPayment("1", sow_sql, &db, &user))) + ","
								"\"bt_payment_will_be_late_soon\":" + quoted(to_string(areThereBTWithExpiredPayment("1/2", sow_sql, &db, &user))) + ""
							;
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
				int			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

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
				int			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

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

		if(action == "AJAX_getSoWList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				bool			include_tasks 			= indexPage.GetVarsHandler()->Get("include_tasks") == "true";
				bool			include_bt 				= indexPage.GetVarsHandler()->Get("include_bt") == "true";
				bool			include_cost_centers 	= indexPage.GetVarsHandler()->Get("include_cost_centers") == "true";
				string			sow_id 					= indexPage.GetVarsHandler()->Get("sow_id");

				if(sow_id.length())	sow_id = CheckHTTPParam_Number(sow_id);

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
					{
						string		agency_id = db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"sow\":[" << GetSOWInJSONFormat(
												"SELECT * FROM `contracts_sow` WHERE "
													+ (sow_id.length() ? string("`id`=\"" + sow_id + "\" AND ") : "") +
													"`agency_company_id`=\"" + agency_id + "\" "
												";", &db, &user, include_tasks, include_bt, include_cost_centers) << "]";

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
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
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
			(action == "AJAX_updateSoWDayRate")					||
			(action == "AJAX_updateSoWSignDate")				||
			(action == "AJAX_updateSoWStartDate")				||
			(action == "AJAX_updateSoWEndDate")					||
			(action == "AJAX_updateSoWCustomField")				||

			(action == "AJAX_updatePeriodStart")				||
			(action == "AJAX_updatePeriodEnd")					||
			(action == "AJAX_updateSubcontractorCreateTasks")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(new_value.length())
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
								if(action.find("update"))
								{
									string		existing_value = GetDBValueByAction(action, id, sow_id, &db, &user);

									error_message = SetNewValueByAction(action, id, sow_id, new_value, &db, &user);
									if(error_message.empty())
									{
										ostResult << "{\"result\":\"success\"}";

										if(GeneralNotifySoWContractPartiesAboutChanges(action, id, sow_id, existing_value, new_value, &db, &user))
										{
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
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
			(action == "AJAX_updatePSoWBTMarkup")				||
			(action == "AJAX_updatePSoWBTMarkupType")			||
			(action == "AJAX_updatePSoWSignDate")				||
			(action == "AJAX_updatePSoWStartDate")				||
			(action == "AJAX_updatePSoWEndDate")				||
			(action == "AJAX_updatePSoWCustomField")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(new_value.length())
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
								if(action.find("update"))
								{
									string		existing_value = GetDBValueByAction(action, id, sow_id, &db, &user);

									error_message = SetNewValueByAction(action, id, sow_id, new_value, &db, &user);
									if(error_message.empty())
									{
										ostResult << "{\"result\":\"success\"}";

										if(GeneralNotifySoWContractPartiesAboutChanges(action, id, sow_id, existing_value, new_value, &db, &user))
										{
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
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
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			company_id		= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("company_id"));
				string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(
					new_value.length() 									||
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

											error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
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
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
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
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
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
			(action == "AJAX_updateTimecardApproverOrder")	||
			(action == "AJAX_updateBTApproverOrder")
			)
		{
			{
				MESSAGE_DEBUG("", action, "start");

				auto			error_message = ""s;
				auto			success_message = ""s;

				auto			sow_id			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				auto			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				ostringstream	ostResult;

				ostResult.str("");

				if(new_value.length() && sow_id.length())
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
									error_message = CheckNewValueByAction(action, "fake", sow_id, new_value, &db, &user);
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

													if(action == "AJAX_updateTimecardApproverOrder")	success_message = "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=" + quoted(sow_id) + ";", &db, &user, false) + "]";
													else if(action == "AJAX_updateBTApproverOrder")		success_message = "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=" + quoted(sow_id) + ";", &db, &user, false) + "]";
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
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + sow_id + "/[" + FilterCP1251Symbols(new_value) + "])");
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
								struct tm new_assignemt_start	= GetTMObject(period_start);
								struct tm new_assignemt_end		= GetTMObject(period_end);

								if(difftime(mktime(&new_assignemt_start), mktime(&sow_start)) >= 0)
								{
									if(difftime(mktime(&sow_end), mktime(&new_assignemt_end)) >= 0)
									{
										if(difftime(mktime(&new_assignemt_end), mktime(&new_assignemt_start)) >= 0)
										{
											string	task_assignment_id = GetTaskAssignmentID(customer, project, task, sow_id, &db);

											if(task_assignment_id.empty())
											{
												string	assignment_start = to_string(1900 + new_assignemt_start.tm_year) + "-" + to_string(1 + new_assignemt_start.tm_mon) + "-" + to_string(new_assignemt_start.tm_mday);
												string	assignment_end = to_string(1900 + new_assignemt_end.tm_year) + "-" + to_string(1 + new_assignemt_end.tm_mon) + "-" + to_string(new_assignemt_end.tm_mday);
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
																MESSAGE_DEBUG("", action, "fail to create psow from sow.id(" + sow_id + ") and task.id(" + task_id + ")");
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
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Некоторые параметры не заданы";
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
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			customer 		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer"));
				string			project			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project"));
				string			task			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task"));

				if(customer.length() && project.length() && task.length())
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

								ostResult << "{\"result\":\"success\"}";

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
					MESSAGE_DEBUG("", action, "consistensy check failed");
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
									MESSAGE_DEBUG("", action, "expense_template_line_id(" + expense_template_line_id + ") presents in expences, can't remove it");
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
									if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
									{
										string		agency_id = db.Get(0, "id");

										error_message = isTimecardsBelongToAgency(timecard_list, agency_id, &db, &user);
										if(error_message.empty())
										{
											error_message = isTimecardsHavePSOWAssigned(timecard_list, cost_center_id, &db, &user);
											if(error_message.empty())
											{
												C_Invoice_Service_Agency_To_CC	c_invoice(&db, &user);

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
													MESSAGE_DEBUG("", action, "fail to generate invoice document archive");
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
									MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employeed");
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
					MESSAGE_DEBUG("", action, "failed");
					ostResult.str("");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
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
									if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
									{
										string		agency_id = db.Get(0, "id");

										error_message = isBTsBelongToAgency(bt_list, agency_id, &db, &user);
										if(error_message.empty())
										{
											error_message = isBTsHavePSOWAssigned(bt_list, cost_center_id, &db, &user);
											if(error_message.empty())
											{
												C_Invoice_BT_Agency_To_CC	c_invoice(&db, &user);

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
									MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employeed");
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
			auto			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;

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

		if(action == "AJAX_getUnemployeedAgentAutocompleteList")
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
						for(int i = 0; i < affected; ++i)
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
										error_message = gettext("SQL syntax issue");
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

		if(	action == "AJAX_addTimecardApproverToSoW" ||
			action == "AJAX_addBTExpenseApproverToSoW"
		)
		{
			string			new_value = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("new_value"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			string			template_name = "json_response.htmlt";
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(new_value.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					string	approver_id = "";

					error_message = CheckNewValueByAction(action, "fake_id", sow_id, new_value, &db, &user);
					if(error_message.empty())
					{
						error_message = SetNewValueByAction(action, "fake_id", sow_id, new_value, &db, &user);
						if(error_message.empty())
						{
							success_message = GetInfoToReturnByAction(action, "fake_id", sow_id, new_value, &db, &user);

							if(GeneralNotifySoWContractPartiesAboutChanges(action, "fake_id", sow_id, "fake_existing_value", new_value, &db, &user))
							{
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/value = " + action + "/" + sow_id + "/[" + FilterCP1251Symbols(new_value) + "])");
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

		if(	action == "AJAX_deleteTimecardApproverFromSoW" ||
			action == "AJAX_deleteBTExpenseApproverFromSoW"
		)
		{
			string			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			string			template_name = "json_response.htmlt";
			auto			error_message = ""s;
			auto			success_message = ""s;

			ostringstream	ostResult;

			ostResult.str("");

			if(id.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					error_message = isActionEntityBelongsToSoW(action, id, sow_id, &db, &user);
					if(error_message.empty())
					{
						// --- notify before actual removal
						if(GeneralNotifySoWContractPartiesAboutChanges(action, id, sow_id, "fake_existing_value", "fake_new_value", &db, &user))
						{
						}
						else
						{
							MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
						}

						error_message = SetNewValueByAction(action, id, sow_id, "fake_new_value", &db, &user);
						if(error_message.empty())
						{
							error_message = ResubmitEntitiesByAction(action, id, sow_id, "fake_new_value", &db, &user);
							if(error_message.empty())
							{
								success_message = GetInfoToReturnByAction(action, id, sow_id, "fake_new_value", &db, &user);
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
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
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
														"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\""
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
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employeed");
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
						if((error_message = RecallServiceInvoice(service_invoice_id, &db, &user)).empty())
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
						if((error_message = RecallBTInvoice(bt_invoice_id, &db, &user)).empty())
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
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employeed");
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
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agency user(" + user.GetID() + ") employeed");
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

					for(int i = 0; i < affected; ++i)
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
								auto	temp_error_message = NotifySoWContractPartiesAboutChanges(to_string(NOTIFICATION_AGENGY_INITIATED_SOW), to_string(sow_id), &db, &user);

								success_message = ",\"sow_id\":\""s + to_string(sow_id) + "\"";

								// --- notifications
								if(temp_error_message.length())
								{
									MESSAGE_ERROR("", action, temp_error_message);
								}
							}
							else
							{
								error_message = gettext("SQL syntax issue");
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
						error_message = gettext("SQL syntax issue");
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

							for(int i = 0; i < affected; ++i)
							{
								unlink((TEMPLATE_SOW_DIRECTORY + "/" + db.Get(i, "value")).c_str());
							}

							affected = db.Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `type`=\"file\" AND  `contract_psow_id` IN ("
													"SELECT `id` FROM `contracts_psow` WHERE `contract_sow_id`=\"" + sow_id + "\""
												");");

							for(int i = 0; i < affected; ++i)
							{
								unlink((TEMPLATE_PSOW_DIRECTORY + "/" + db.Get(i, "value")).c_str());
							}


							affected = db.Query("SELECT `filename` FROM `contract_sow_agreement_files` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							for(int i = 0; i < affected; ++i)
							{
								unlink((TEMPLATE_AGREEMENT_SOW_DIRECTORY + "/" + db.Get(i, 0)).c_str());
							}

							affected = db.Query("SELECT `agreement_filename` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";");
							if(affected)
							{
								if(db.Get(0, 0).length())
								{
									unlink((AGREEMENTS_SOW_DIRECTORY + "/" + db.Get(0, 0)).c_str());
								}
								else
								{
									MESSAGE_DEBUG("", "", "no agreements assigned to SoW. nothing to delete.");
								}
							}

							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_AGENGY_GENERATED_AGREEMENTS) + "\" AND `actionId`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_AGENGY_INITIATED_SOW) + "\" AND `actionId`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_SUBCONTRACTOR_SIGNED_SOW) + "\" AND `actionId`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `contract_sow_agreement_files` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\";");
							db.Query("DELETE FROM `bt_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\";");
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
							auto	sql_query = "SELECT * FROM `company_agreement_files` WHERE `company_id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\")";
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
										error_message = gettext("SQL syntax issue");
										MESSAGE_ERROR("", "", error_message);
									}
								}
								else
								{
									MESSAGE_DEBUG("", "", "error returned from CheckAgreementSoWTitle");
								}
							}
							else
							{
								MESSAGE_ERROR("", "", "can't define company id by user.id(" + user.GetID() + ")");
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
									error_message = gettext("SQL syntax issue");
									MESSAGE_ERROR("", "", error_message);
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
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
											}
										}
										else if(action.find("insert") != string::npos)
										{
											
										}
										else if(action.find("delete") != string::npos)
										{
											if(db.Query("SELECT `filename` FROM `company_agreement_files` WHERE `id`=\"" + id + "\";"))
											{
												auto	filename = db.Get(0, 0);

												if(filename.length()) unlink((TEMPLATE_AGREEMENT_COMPANY_DIRECTORY + "/" + filename).c_str());

												error_message = DeleteEntryByAction(action, id, &db, &user);
												if(error_message.empty())
												{
													// --- good finish
												}
												else
												{
													MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
												}
											}
											else
											{
												error_message = gettext("SQL syntax issue");
												MESSAGE_ERROR("", "", "can't find contract_sow_agreement_files.id(" + id + ") in DB")
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
					MESSAGE_ERROR("", "", "fail to determine sow.id by contract_sow_agreement_files.id(" + id + ")");
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
												MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
											}
										}
										else if(action.find("insert") != string::npos)
										{
											
										}
										else if(action.find("delete") != string::npos)
										{
											if(db.Query("SELECT `filename` FROM `contract_sow_agreement_files` WHERE `id`=\"" + id + "\";"))
											{
												auto	filename = db.Get(0, 0);

												if(filename.length()) unlink((TEMPLATE_AGREEMENT_SOW_DIRECTORY + "/" + filename).c_str());

												error_message = DeleteEntryByAction(action, id, &db, &user);
												if(error_message.empty())
												{
													// --- good finish
												}
												else
												{
													MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
												}
											}
											else
											{
												error_message = gettext("SQL syntax issue");
												MESSAGE_ERROR("", "", "can't find contract_sow_agreement_files.id(" + id + ") in DB")
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
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
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
									C_Agreements_SoW_Factory	agreements(&db, &user);

									agreements.SetSoWID(sow_id);

									if((error_message = agreements.GenerateDocumentArchive()).empty())
									{
										success_message = ",\"filename\":\"" + agreements.GetShortFilename() + "\"";

										{
											auto	temp_error_message = NotifySoWContractPartiesAboutChanges(to_string(NOTIFICATION_AGENGY_GENERATED_AGREEMENTS), sow_id, &db, &user);

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
									MESSAGE_ERROR("", "", error_message + ". Workflow should not be here.");
								}
							}
							else
							{
								error_message = gettext("SQL syntax issue");
								MESSAGE_ERROR("", "", error_message + ". Workflow should not be here.");
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
					ostResult << "{\"result\":\"success\"" + success_message + "}";
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
											unlink((AGREEMENTS_SOW_DIRECTORY + "/" + db.Get(0, 0)).c_str());
										}
										else
										{
											MESSAGE_DEBUG("", "", "no agreements assigned to SoW. nothing to delete.");
										}
									}
									else
									{
										MESSAGE_ERROR("", "", "sow_id(" + sow_id + ") not found. Workflow must not be here. Investigate reason.");
									}

									db.Query("DELETE FROM `users_notification` WHERE `actionTypeId`=\"" + to_string(NOTIFICATION_AGENGY_GENERATED_AGREEMENTS) + "\" AND `actionId`=\"" + sow_id + "\";");
									db.Query("UPDATE `contracts_sow` SET `agreement_filename`=\"\" WHERE `id`=\"" + sow_id + "\";");
								}
								else
								{
									error_message = gettext("Agreement already signed");
									MESSAGE_ERROR("", "", error_message + ". Workflow should not be here.");
								}
							}
							else
							{
								error_message = gettext("SQL syntax issue");
								MESSAGE_ERROR("", "", error_message + ". Workflow should not be here.");
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
