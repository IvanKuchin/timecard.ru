#include "agency.h"

int main(void)
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
			MESSAGE_ERROR("", action, "template file was missing");
			throw CException("Template file was missing");
		}

		if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
		{
			MESSAGE_ERROR("", action, "Can not connect to mysql database");
			throw CExceptionHTML("MySql connection");
		}

		indexPage.SetDB(&db);

#ifndef MYSQL_3
		db.Query("set names cp1251");
#endif

		action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));
		{
			MESSAGE_DEBUG("", "", "action taken from HTTP is " + action);
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
			action = LogoutIfGuest(action, &indexPage, &db, &user);

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
				if(sessidHTTP.length() < 5)
				{
					{
						MESSAGE_DEBUG("", action, "session cookie is not exist, generating new session.");
					}
					sessidHTTP = indexPage.SessID_Create_HTTP_DB();
					if(sessidHTTP.length() < 5)
					{
						MESSAGE_ERROR("", action, "error in generating session ID");
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

								user.SetDB(&db);
								if(indexPage.SessID_Get_UserFromDB() != "Guest")
								{
									// --- place 2change user from user.email to user.id
									if(user.GetFromDBbyEmail(indexPage.SessID_Get_UserFromDB()))
									{
										ostringstream	ost1;
										string			avatarPath;

										indexPage.RegisterVariableForce("loginUser", indexPage.SessID_Get_UserFromDB());
										indexPage.RegisterVariableForce("loginUserID", user.GetID());
										indexPage.RegisterVariableForce("myLogin", user.GetLogin());
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
											MESSAGE_DEBUG("", "", "user [" + user.GetLogin() + "] logged in");
										}
									}
									else
									{
										// --- enforce to close session
										action = "logout";

										{
											MESSAGE_DEBUG("", "", "user [" + indexPage.SessID_Get_UserFromDB() + "] session exists on client device, but not in the DB. Change the [action = logout]");
										}

									}
								}
								else
								{
									{
										MESSAGE_DEBUG("", action, "user [" + user.GetLogin() + "] surfing");
									}

									if(user.GetFromDBbyLogin(user.GetLogin()))
									{
										indexPage.RegisterVariableForce("loginUser", user.GetLogin());
										indexPage.RegisterVariableForce("loginUserID", user.GetID());
										indexPage.RegisterVariableForce("myLogin", user.GetLogin());
										indexPage.RegisterVariableForce("myFirstName", user.GetName());
										indexPage.RegisterVariableForce("myLastName", user.GetNameLast());
									}
									else
									{
										// --- enforce to close session
										action = "logout";

										{
											MESSAGE_DEBUG("", action, "user [" + indexPage.SessID_Get_UserFromDB() + "] session exists on client device, but not in the DB. Change the [action = logout].");
										}

									}

								}


							}
							else
							{
								MESSAGE_ERROR("", action, "error update session in HTTP or DB failed");
							}
						}
						else {
							MESSAGE_ERROR("", action, "error session consistency check failed");
						}
					}
					else
					{
						ostringstream	ost;

						{
							MESSAGE_DEBUG("", action, "cookie session and DB session is not equal. Need to recreate session");
						}

						ost.str("");
						ost << "/?rand=" << GetRandom(10);

						if(!indexPage.Cookie_Expire()) {
							MESSAGE_ERROR("", action, "error in session expiration");
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
				MESSAGE_ERROR("", action, "error getting keywords_head from settings_default");
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"title_head\"") > 0)
				indexPage.RegisterVariable("title_head", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting title_head from settings_default");
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"description_head\"") > 0)
				indexPage.RegisterVariable("description_head", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting description_head from settings_default");
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"NewsOnSinglePage\"") > 0)
				indexPage.RegisterVariable("NewsOnSinglePage", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting NewsOnSinglePage from settings_default");

				indexPage.RegisterVariable("NewsOnSinglePage", to_string(NEWS_ON_SINGLE_PAGE));
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"FriendsOnSinglePage\"") > 0)
				indexPage.RegisterVariable("FriendsOnSinglePage", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting FriendsOnSinglePage from settings_default");

				indexPage.RegisterVariable("FriendsOnSinglePage", to_string(FRIENDS_ON_SINGLE_PAGE));
			}

			if(action.empty() and user.GetLogin().length() and (user.GetLogin() != "Guest"))
			{
				action = GetDefaultActionFromUserType(user.GetType(), &db);

				{
					MESSAGE_DEBUG("", action, "META-registration: 'logged-in user' action has been overriden to [" + action + "]");
				}

			}
			else if(!action.length())
			{
				action = GUEST_USER_DEFAULT_ACTION;

				{
					MESSAGE_DEBUG("", action, "META-registration: 'guest user' action has been overriden to [" + action + "]");
				} // --- log block
			} // --- default action
	//------- End register html META-tags
		*/
		}
	// ------------ end generate common parts

		{
			MESSAGE_DEBUG("", "", "pre-condition if(action == \"" + action + "\")");
		}

		if((action.length() > 10) && (action.compare(action.length() - 9, 9, "_template") == 0))
		{
			ostringstream	ost;
			string			strPageToGet, strFriendsOnSinglePage;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			{
				string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}

			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getApprovalsList")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

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
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_approveTimecard")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

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
						// --- REPEAT #2 
	/*					if(db.Query("SELECT `id` FROM `timecards` WHERE "
										"`id`=\"" + timecard_id + "\" "
										"AND "
										"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
	*/
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
										// --- did approver approved after user submission ?
										if(db.Query("SELECT `eventTimestamp` FROM `timecard_approvals` WHERE `approver_id`=\"" + approver_id + "\" AND `timecard_id`=\"" + timecard_id + "\" AND `eventTimestamp`>\"" + timecard_submit_date + "\";"))
										{
											// --- this approver already approved this bt
											ostResult << "{\"result\":\"error\",\"description\":\"Вы уже подтвердили\"}";
											MESSAGE_ERROR("", action, "approver.id(" + approver_id + ") already approved timecard.id(" + timecard_id + ") at timestamp(" + db.Get(0, "eventTimestamp") + ")");
										}
										else
										{
											long int	temp;

											temp = db.InsertQuery(	"INSERT INTO `timecard_approvals` SET "
																	"`timecard_id`=\"" + timecard_id + "\","
																	"`approver_id`=\"" + approver_id + "\","
																	"`decision`=\"approved\","
																	"`comment`=\"" + comment + "\","
																	"`eventTimestamp`=UNIX_TIMESTAMP();"
																);

											if(temp)
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
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_rejectTimecard")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

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
						if(db.Query("SELECT `id` FROM `timecards` WHERE "
										"`id`=\"" + timecard_id + "\" "
										"AND "
										"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
						{

							if(db.Query("SELECT `status` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								string	timecard_state = db.Get(0, "status");

								if(timecard_state == "submit")
								{

									if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
													"`approver_user_id`=\"" + user.GetID() + "\" "
													"AND "
													"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
													";"))
									{
										string		approver_id = db.Get(0, "id");
										long int	temp;

										temp = db.InsertQuery(	"INSERT INTO `timecard_approvals` SET "
																"`timecard_id`=\"" + timecard_id + "\","
																"`approver_id`=\"" + approver_id + "\","
																"`decision`=\"rejected\","
																"`comment`=\"" + comment + "\","
																"`eventTimestamp`=UNIX_TIMESTAMP();"
															);

										if(temp)
										{
											db.Query("UPDATE `timecards` SET `status`=\"rejected\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
											if(!db.isError())
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
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
												MESSAGE_ERROR("", action, "fail to update timecard_id(" + timecard_id + "): " + db.GetErrorMessage());
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
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an timecard.id(" + timecard_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать таймкарту у данного договора\"}";
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
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
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
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_approveBT")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

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
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
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


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getSoWList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				bool			include_bt 		= indexPage.GetVarsHandler()->Get("include_bt") == "true";
				bool			include_tasks 	= indexPage.GetVarsHandler()->Get("include_tasks") == "true";
				string			sow_id 			= indexPage.GetVarsHandler()->Get("sow_id");

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
												";", &db, &user, include_tasks, include_bt) << "]";

						if(include_tasks)
							ostResult <<	","
											"\"task_assignments\":[" << GetTimecardTaskAssignmentInJSONFormat(
													"SELECT * FROM `timecard_task_assignment` WHERE "
														+ (sow_id.length() ? string("`contract_sow_id`=\"" + sow_id + "\" AND ") : "") +
														"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
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
					error_message = "Информация доступна только для агенства";
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

										if(NotifySoWContractPartiesAboutChanges(action, id, sow_id, existing_value, new_value, &db, &user))
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
					error_message = "Поле не должно быть пустым";
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
			(action == "AJAX_updateCustomerTitle")			||
			(action == "AJAX_updateProjectTitle")			||
			(action == "AJAX_updateTaskTitle")				|| 
			(action == "AJAX_updateCompanyTitle")			||
			(action == "AJAX_updateCompanyDescription")		||
			(action == "AJAX_updateCompanyWebSite")			||
			(action == "AJAX_updateCompanyTIN")				||
			(action == "AJAX_updateCompanyAccount")			||
			(action == "AJAX_updateCompanyOGRN")			||
			(action == "AJAX_updateCompanyKPP")				||
			(action == "AJAX_updateCompanyLegalAddress")	||
			(action == "AJAX_updateCompanyMailingAddress")	||
			(action == "AJAX_updateAgencyPosition")			||
			(action == "AJAX_updateAgencyEditCapability")	||
			(action == "AJAX_updateSoWEditCapability")		||
			(action == "AJAX_updateCompanyMailingZipID")	||
			(action == "AJAX_updateCompanyLegalZipID")		||
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
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);
						if(agency_id.length())
						{
							if(action == "AJAX_updateCompanyTitle") 		{	id = agency_id; }
							if(action == "AJAX_updateCompanyDescription")	{	id = agency_id; }
							if(action == "AJAX_updateCompanyWebSite")		{	id = agency_id; }
							if(action == "AJAX_updateCompanyTIN")			{	id = agency_id; }
							if(action == "AJAX_updateCompanyAccount")		{	id = agency_id; }
							if(action == "AJAX_updateCompanyOGRN")			{	id = agency_id; }
							if(action == "AJAX_updateCompanyKPP")			{	id = agency_id; }
							if(action == "AJAX_updateCompanyLegalAddress") 	{	id = agency_id; }
							if(action == "AJAX_updateCompanyMailingAddress"){	id = agency_id; }
							if(action == "AJAX_updateCompanyMailingZipID") 	{	new_value = id; id = agency_id; }
							if(action == "AJAX_updateCompanyLegalZipID") 	{	new_value = id; id = agency_id; }
							if(action == "AJAX_updateCompanyBankID") 		{	new_value = id; id = agency_id; }

							error_message = isActionEntityBelongsToAgency(action, id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = CheckNewValueByAction(action, id, "", new_value, &db, &user);
								if(error_message.empty())
								{
									if(action.find("update"))
									{
										string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

										error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
										if(error_message.empty())
										{
											ostResult << "{\"result\":\"success\"}";

											if(NotifySoWContractPartiesAboutChanges(action, id, "", existing_value, new_value, &db, &user))
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
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belong to agency.id(" + agency_id + ")");
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

											task_assignment_id = CreateTaskAssignment(task_id, sow_id, assignment_start, assignment_end, &db, &user);

											if(task_assignment_id.length())
											{
												ostResult << "{\"result\":\"success\"}";

												if(notify_about_task_creation)
												{
													if(NotifySoWContractPartiesAboutChanges("AJAX_addTask", task_id, sow_id, "", task, &db, &user))
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
												if(NotifySoWContractPartiesAboutChanges(action, task_assignment_id, sow_id, "", customer + " / " + project + " / " + task + " ( с " + assignment_start + " по " + assignment_end + ")", &db, &user))
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
												error_message = "Неудалось создать назначение";
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "assignment Customer/Project/Task already exists for this sow.id(" + sow_id + ")");
											error_message = "Такое назначение уже существует";
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

								if(NotifySoWContractPartiesAboutChanges("AJAX_addTask", task_id, "", "", task, &db, &user))
								{
								}
								else
								{
									MESSAGE_ERROR("", "", "fail to notify agency");
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
									if(NotifySoWContractPartiesAboutChanges(action, timecard_task_assignment_id, sow_id, "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify SoW parties");
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
					error_message = "Информация доступна только для агенства";
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
								if(isTaskIDValidToRemove(task_id, &db))
								{
									string		removal_sql_query = "";
									string		project_id, customer_id;

									tie(customer_id, project_id) = GetCustomerIDProjectIDByTaskID(task_id, &db);

									if(NotifySoWContractPartiesAboutChanges("AJAX_deleteTask", task_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}
									db.Query("DELETE FROM `timecard_tasks` WHERE `id`=\"" + task_id + "\";");
									ostResult << "{\"result\":\"success\"}";

									if(isProjectIDValidToRemove(project_id, &db))
									{
										if(NotifySoWContractPartiesAboutChanges("AJAX_deleteProject", project_id, "", "", "", &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", "", "fail to notify agency");
										}
										db.Query("DELETE FROM `timecard_projects` WHERE `id`=\"" + project_id + "\";");

										if(isCustomerIDValidToRemove(customer_id, &db))
										{
											if(NotifySoWContractPartiesAboutChanges("AJAX_deleteCustomer", customer_id, "", "", "", &db, &user))
											{
											}
											else
											{
												MESSAGE_ERROR("", "", "fail to notify agency");
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
									error_message = "На эту задачу субконтракторы отчитались. Поэтому нельзя удалить.";
									MESSAGE_DEBUG("", action, "task_id(" + task_id + ") has hours reported, can't remove it");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belong to agency.id(" + agency_id + ")");
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

		if(action == "AJAX_getAgencyInfo")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			agency_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				bool			include_bt = indexPage.GetVarsHandler()->Get("include_bt") == "true";
				bool			include_tasks = indexPage.GetVarsHandler()->Get("include_tasks") == "true";

				string			agency_obj = GetAgencyObjectInJSONFormat(agency_id, include_tasks, include_bt, &db, &user);

				if(agency_obj.length())
				{
					// --- get short info
					ostResult << "{\"result\":\"success\","
									"\"agencies\":[" + agency_obj + "]"
									"}";
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
									if(NotifySoWContractPartiesAboutChanges(action, employee_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
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
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belong to agency.id(" + agency_id + ")");
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
			string			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("name"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(name.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					int	affected = db.Query("SELECT `id`, `name`, `nameLast` FROM `users` WHERE ((`type`=\"approver\") OR (`type`=\"agency\")) AND ((`name` LIKE \"%" + name + "%\") OR (`nameLast` LIKE \"%" + name + "%\")) LIMIT 0, 20;");
					if(affected)
					{
						ostResult << "{\"result\":\"success\","
								  << "\"autocomplete_list\":[";
						for(int i = 0; i < affected; ++i)
						{
							if(i) ostResult << ",";
							ostResult << "{\"id\":\"" << db.Get(i, "id") << "\","
									  << "\"label\":\"" << db.Get(i, "name") << " " << db.Get(i, "nameLast") << "\"}";
						}
						ostResult << "]}";
					}
					else
					{
						ostResult << "{\"result\":\"success\",\"autocomplete_list\":[]}";
						MESSAGE_DEBUG("", "", "no users found");
					}	
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
				}
			}
			else
			{
				error_message = "Некорректные параметры";
				MESSAGE_DEBUG("", "", "mandatory parameter missed");
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

		if(	action == "AJAX_addTimecardApproverToSoW" ||
			action == "AJAX_addBTApproverToSoW"
		)
		{
			string			new_value = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("new_value"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			string			template_name = "json_response.htmlt";
			string			error_message = "";
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
							string info_to_return = GetInfoToReturnByAction(action, "fake_id", sow_id, new_value, &db, &user);

							ostResult << "{\"result\":\"success\"";
							if(info_to_return.length()) ostResult << "," << info_to_return;
							ostResult << "}";

							if(NotifySoWContractPartiesAboutChanges(action, "fake_id", sow_id, "fake_existing_value", new_value, &db, &user))
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
				error_message = "Некорректные параметры";
				MESSAGE_DEBUG("", "", "mandatory parameter missed");
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

		if(	action == "AJAX_deleteTimecardApproverFromSoW" ||
			action == "AJAX_deleteBTApproverFromSoW"
		)
		{
			string			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			string			template_name = "json_response.htmlt";
			string			error_message = "";
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
						if(NotifySoWContractPartiesAboutChanges(action, id, sow_id, "fake_existing_value", "fake_new_value", &db, &user))
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
								string info_to_return = GetInfoToReturnByAction(action, id, sow_id, "fake_new_value", &db, &user);

								ostResult << "{\"result\":\"success\"";
								if(info_to_return.length()) ostResult << "," << info_to_return;
								ostResult << "}";
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
				error_message = "Некорректные параметры";
				MESSAGE_DEBUG("", "", "mandatory parameter missed");
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



		{
			MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");
		}

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
