#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <exception>

#include "localy.h"
#include "ccgi.h"
#include "cvars.h"
#include "clog.h"
#include "cmysql.h"
#include "cexception.h"
#include "cactivator.h"
#include "cuser.h"
#include "cmail.h"
#include "cstatistics.h"
#include "utilities_timecard.h"
// #include "cforum.h"
// #include "cmenu.h"
// #include "ccatalog.h"

int main()
{
	CStatistics			appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi				indexPage(EXTERNAL_TEMPLATE);
	CUser				user;
	string				action, partnerID;
	CMysql				db;
	struct timeval		tv;
	map<string,string>	mapResult;

	{
		MESSAGE_DEBUG("", action, __FILE__);
	}

	signal(SIGSEGV, crash_handler);

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	try
	{

		indexPage.ParseURL();

		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;
			log.Write(ERROR, "template file was missing");
			throw CException("Template file was missing");
		}

		if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
		{
			CLog	log;
			log.Write(ERROR, "Can not connect to mysql database");
			throw CExceptionHTML("MySql connection");
		}

		indexPage.SetDB(&db);

#ifndef MYSQL_3
	db.Query("set names utf8;");
#endif

		action = indexPage.GetVarsHandler()->Get("action");
		{
			MESSAGE_DEBUG("", action, "action = " + action);
		}

	// ------------ generate common parts
		{
			ostringstream			query, ost1, ost2;
			string				partNum;
			map<string, string>		menuHeader;
			map<string, string>::iterator	iterMenuHeader;
			string				content;
			// Menu				m;

			indexPage.RegisterVariableForce("rand", GetRandom(10));
			indexPage.RegisterVariableForce("random", GetRandom(10));
			indexPage.RegisterVariableForce("style", "style.css");

	//------- Generate session
			{
				string			lng, sessidHTTP;
				ostringstream	ost;

				mapResult["session"] = "false";
				mapResult["user"] = "false";

				sessidHTTP = indexPage.SessID_Get_FromHTTP();
				if(sessidHTTP.length() < 5) {
					{
						MESSAGE_DEBUG("", action, "session checks: session cookie is not_exists/expired, UA must be redirected to / page.");
					}
				}
				else
				{
					{
						MESSAGE_DEBUG("", action, "session checks: session cookie provided [" + sessidHTTP + "]");
					}

					if(indexPage.SessID_Load_FromDB(sessidHTTP))
					{
						{
							MESSAGE_DEBUG("", action, "session checks: DB session loaded");
						}

						if(indexPage.SessID_CheckConsistency())
						{

							mapResult["session"] = "true";

							indexPage.RegisterVariableForce("loginUser", "");

							if(indexPage.SessID_Get_UserFromDB() != "Guest")
							{

								user.SetDB(&db);

								if(user.GetFromDBbyEmailNoPassword(indexPage.SessID_Get_UserFromDB()))
								{

									indexPage.RegisterVariableForce("loginUser", indexPage.SessID_Get_UserFromDB());
									mapResult["user"] = "true";

									{
										CLog	log;
										ostringstream	ost;

										ost << __func__ + string("[") + to_string(__LINE__) + "]: session checks: user [" << user.GetLogin() << "] logged in";
										log.Write(DEBUG, ost.str());
									}
								}
								else
								{
									// --- enforce to close session
									mapResult["user"] = "false";
									action = "logout";

									{
										CLog	log;
										ostringstream	ost;

										ost << __func__ + string("[") + to_string(__LINE__) + "]: ERROR user [" << indexPage.SessID_Get_UserFromDB() << "] session exists on client device, but not in the DB. Change the [action = logout].";
										log.Write(ERROR, ost.str());
									}
								}
							}
							else
							{
								{
									MESSAGE_DEBUG("", action, "session checks: session assigned to Guest user");
								}

							}
						}
						else
						{
							CLog	log;
							log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]: ERROR session consistency check failed");
						}
					}
					else
					{
						ostringstream	ost;

						{
							MESSAGE_DEBUG("", action, "cookie session and DB session is not equal. Need to recreate session");
						}

/*						if(!indexPage.Cookie_Expire())
						{
							CLog	log;
							log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]: Error in session expiration");
						} // --- if(!indexPage.Cookie_Expire())

						// --- redirect URL
						ost.str("");
						ost << "/?rand=" << GetRandom(10);

						indexPage.Redirect(ost.str());
*/
					} // --- if(indexPage.SessID_Load_FromDB(sessidHTTP))
				} // --- if(sessidHTTP.length() < 5)




			}
	//------- End generate session

		}
	// ------------ end generate common parts


		if(action == "EchoRequest")
		{
			{
				MESSAGE_DEBUG("", action, "start");
			}
			mapResult["type"] = "EchoResponse";

			if(user.GetLogin() != "Guest")
			{
				user.UpdatePresence();
			}
		}

		if(action == "GetUserRequestNotifications")
		{
			ostringstream	ost, result, ostUserNotifications;
			int				numberOfFriendshipRequests;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			result.str("");

			ost.str("");
			ost << "SELECT * FROM  `users_friends` \
					WHERE `users_friends`.`userID`='" << user.GetID() << "' and `users_friends`.`state`='requested';";
			numberOfFriendshipRequests = db.Query(ost.str());
			// if(numberOfFriendshipRequests > 0)
			// {
				result << "[";
				for(int i = 0; i < numberOfFriendshipRequests; i++)
				{
					if(i > 0) result << ",";

					result << "{";
					result << "\"id\" : \"" << db.Get(i, "id") << "\",";
					result << "\"userID\" : \"" << db.Get(i, "userID") << "\",";
					result << "\"friendID\" : \"" << db.Get(i, "friendID") << "\",";
					result << "\"typeID\" : \"friendshipRequest\",";
					result << "\"state\" : \"" << db.Get(i, "state") << "\",";
					result << "\"date\" : \"" << db.Get(i, "date") << "\"";
					result << "}";
				}
				result << "]";
			// }

			{
				// ostUserNotifications.str("");
				// ostUserNotifications << "[";

				ost.str("");
				ost << "SELECT `users_notification`.`eventTimestamp` as `feed_eventTimestamp`, `users_notification`.`actionId` as `feed_actionId` , `users_notification`.`actionTypeId` as `feed_actionTypeId`, "
					<< " `users_notification`.`id` as `users_notification_id`, `users_notification`.`notificationStatus` as `users_notification_notificationStatus`, `users_notification`.`title` as `users_notification_title`, "
					<< " `users`.`id` as `user_id`, `users`.`name` as `user_name`, `users`.`nameLast` as `user_nameLast`, `users`.`sex` as `user_sex`, `users`.`email` as `user_email`, "
					<< " `action_types`.`title` as `action_types_title`, `action_types`.`title_male` as `action_types_title_male`, `action_types`.`title_female` as `action_types_title_female`, "
					<< " `action_category`.`title` as `action_category_title`, `action_category`.`title_male` as `action_category_title_male`, `action_category`.`title_female` as `action_category_title_female`, `action_category`.`id` as `action_category_id` "
					<< " FROM `users_notification` "
					<< " INNER JOIN  `action_types` 		ON `users_notification`.`actionTypeId`=`action_types`.`id` "
					<< " INNER JOIN  `action_category` 	ON `action_types`.`categoryID`=`action_category`.`id` "
					<< " INNER JOIN  `users` 			ON `users_notification`.`userId`=`users`.`id` "
					<< " WHERE `users_notification`.`userId`=\"" << user.GetID() << "\" AND `action_types`.`isShowNotification`='1' AND `users_notification`.`notificationStatus`='unread' "
					<< " ORDER BY  `users_notification`.`eventTimestamp` DESC LIMIT 0 , 21";


				mapResult["userNotificationsArray"] = GetUserNotificationInJSONFormat(ost.str(), &db, &user);
			}

			mapResult["friendshipNotificationsArray"] = result.str();
			mapResult["type"] = "UnreadUserNotifications";


			{
				MESSAGE_DEBUG("", action, "action == " + action + ": end (# of friendship notifications = " + to_string(numberOfFriendshipRequests) + ")");
			}
		}

		// --- JSON get user info
		if(action == "GetUserInfo")
		{
			ostringstream	ost, ostFinal;
			string			sessid, userID, userList;
			char			*convertBuffer;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			if(user.GetLogin() == "Guest")
			{
				ostringstream	ost;

				{
					MESSAGE_DEBUG("", action, "]::GetUserInfo: re-login required");
				}

			}
			else
			{
				userID = indexPage.GetVarsHandler()->Get("userID");

				convertBuffer = new char[1024];
				memset(convertBuffer, 0, 1024);
				convert_utf8_to_windows1251(userID.c_str(), convertBuffer, 1024);
				userID = convertBuffer;
				trim(userID);

				delete[] convertBuffer;

				// --- Clean-up the text
				userID = ReplaceDoubleQuoteToQuote(userID);
				userID = DeleteHTML(userID);
				userID = SymbolReplace(userID, "\r\n", "");
				userID = SymbolReplace(userID, "\r", "");
				userID = SymbolReplace(userID, "\n", "");

				ost << "select * from `users` where `isActivated`='Y' and `isblocked`='N' and `id` in (" << userID << ") ;";

				userList = GetUserListInJSONFormat(ost.str(), &db, &user);

				ostFinal.str("");
				ostFinal << "[" << std::endl << userList << std::endl << "]" << std::endl;


				mapResult["userArray"] = ostFinal.str();
				mapResult["type"] = "UserInfo";
			} // --- if(user.GetLogin() == "Guest")

			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		// --- JSON get chat status
		if(action == "GetNavMenuChatStatus")
		{
			ostringstream	ost, ostFinal, friendsSqlQuery;
			string			sessid, lookForKey, userList, messageList;
			int				affected;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			if(user.GetLogin() == "Guest")
			{
				ostringstream	ost;

				{
					MESSAGE_DEBUG("", action, "]::GetNavMenuChatStatus: re-login required");
				}

				ost.str("");
				ost << "/?rand=" << GetRandom(10);
				indexPage.Redirect(ost.str());
			}

			friendsSqlQuery.str("");
			ost.str("");
			ost << "select `fromID` from `chat_messages` where `toID`='" << user.GetID() << "' and (`messageStatus`='unread' or `messageStatus`='sent' or `messageStatus`='delivered');";
			affected = db.Query(ost.str());
			if(affected)
			{
				string	tempUserIDList {""};

				for(int i = 0; i < affected; i++)
				{
					tempUserIDList += (tempUserIDList.length() ? "," : "");
					tempUserIDList += db.Get(i, "fromID");
				}

				friendsSqlQuery << "select * from `users` where `isActivated`='Y' and `isblocked`='N' and `id` IN (";
				friendsSqlQuery << UniqueUserIDInUserIDLine(tempUserIDList);
				friendsSqlQuery << ");";

				{
					MESSAGE_DEBUG("", action, "query for JSON prepared [" + friendsSqlQuery.str() + "]");
				}

				userList = GetUserListInJSONFormat(friendsSqlQuery.str(), &db, &user);
				messageList = GetUnreadChatMessagesInJSONFormat(&user, &db);
			}

			userList = "[" + userList + "]";
			messageList = "[" + messageList + "]";
			mapResult["userArray"] = userList;
			mapResult["unreadMessagesArray"] = messageList;
			mapResult["type"] = "ChatStatus";

			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}


		// --- JSON get user info
		if(action == "CheckSessionPersistence")
		{
			ostringstream	ost, ostFinal;
			string			sessidPersistence, userID, userList;
			char			*remoteAddr = getenv("REMOTE_ADDR");

			sessidPersistence = indexPage.GetVarsHandler()->Get("sessid");

			{
				MESSAGE_DEBUG("", action, "start (persistence sessid" + sessidPersistence + ")");
			}

			mapResult["result"] = "error";
			mapResult["sessionPersistence"] = "false";
			mapResult["userPersistence"] = "false";
			mapResult["redirect"] = GetDefaultActionFromUserType(user.GetType(), &db);

			if(remoteAddr && (!isPersistenceRateLimited(remoteAddr, &db)))
			{
				if(sessidPersistence.length() < 5)
				{
					{
						MESSAGE_DEBUG("", action, "session checks: persistence sessid doesn't exists, UA must be redirected to / page.");
					}
				}
				else
				{
					{
						MESSAGE_DEBUG("", action, "session checks: HTTP-param sessid exists");
					}

					if(indexPage.SessID_Load_FromDB(sessidPersistence))
					{
						{
							MESSAGE_DEBUG("", action, "session checks: DB session loaded");
						}

						if(indexPage.SessID_CheckConsistency())
						{
							string		sessidHTTP("");

							{
								MESSAGE_DEBUG("", action, "session checks: HTTP session consistent with DB session");
							}

							sessidHTTP = indexPage.SessID_Get_FromHTTP();
							if(sessidHTTP.length() < 5)
							{
								// --- CheckSessionPersistence called from autologin
								// --- Temporary Guest cookie must be generated before getting here
								// --- if workflow gets here, you need to check why cookies has not been created before.
								// ---
								// --- 2 cases so far:
								// --- 1) bug on iPhone / Safari: cookie doesn't assigned in short time interval up to 5 sec
								// --- 2) transit state:
								// ---- 		2.a) network I connected to relayed HTTP requests to Symantec for AV-analysis, Symantec didn't keep cookie in HTTP-requests
								// ---			2.b) our server saw two similar requests at the same time with different IP addresses (change IP address in logs)
								{
									MESSAGE_ERROR("", action, "\"sessid\" cookie doesn't exists or expired, UA must be redirected to / page. This is AJAX request called from "s + (getenv("HTTP_REFERER") ? getenv("HTTP_REFERER") : "") + ". Supposed that parent script assign cookie:sessid, BUT cookie hasn't been assigned. Probably this script has been called from cached page or page opened in neighbour tab (check that parallel stream may exists with the same persistency key).");
								}
							}
							else
							{
								indexPage.RegisterVariableForce("loginUser", "");
								mapResult["sessionPersistence"] = "true";

								if(indexPage.SessID_Get_UserFromDB() == "Guest")
								{
									{
										MESSAGE_DEBUG("", action, "persisted session doesn't belongs to any registered user");
									}
								}
								else
								{
									{
										MESSAGE_DEBUG("", action, "session checks: session active for registered user");
									}

									mapResult["userPersistence"] = "true";

									user.SetDB(&db);
									user.GetFromDBbyEmail(indexPage.SessID_Get_UserFromDB());
									indexPage.RegisterVariableForce("loginUser", indexPage.SessID_Get_UserFromDB());

									// --- copy session from persisted user to his/her new session
									if(db.Query("SELECT `user`, `expire`, `remove_flag`, `remove_flag_timestamp` FROM `sessions` WHERE `id`=\"" + sessidPersistence + "\";"))
									{
										string	persistedUser = db.Get(0, "user");
										string	persistedExpire = db.Get(0, "expire");
										string	remove_flag = db.Get(0, "remove_flag");
										string	remove_flag_timestamp = db.Get(0, "remove_flag_timestamp");

										if(remove_flag == "Y")
											MESSAGE_ERROR("", "", "session(" + sessidPersistence + ") would be deleted at " + remove_flag_timestamp + ", but it is re-used. Investigate why it is re-used.");

										if(sessidPersistence == sessidHTTP)
										{
											// --- you can get here in case Safari iPhone cache problem
											// --- 1) session expired on server before triggering /news_feed
											// --- 2) redirect to /autologin
											// --- 3) /autologin use completely different sessid (WRONG behavior)
											// --- 4) /checkSessionPersistense have sessidHTTP == sessidPersistence
											// --- 5) you are here ! No need to remove session from DB
											MESSAGE_ERROR("", action, "]:action == " + action + ": cache issue");
										}
										else
										{
											// --- following lines used to track re-use legacy sessions. 
											// --- instead of removal, it will be "marked for removal"
											// db.Query("DELETE FROM `sessions` WHERE `id`=\"" + sessidPersistence + "\";");
											db.Query("UDATE `sessions` SET `remove_flag`=\"Y\", `remove_flag_timestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sessidPersistence + "\";");
										}

										mapResult["redirect"] = GetDefaultActionFromUserType(user.GetType(), &db);

										db.Query("UPDATE `sessions` SET `user`=\"" + persistedUser + "\", expire=\"" + persistedExpire + "\" WHERE `id`=\"" + sessidHTTP + "\";");
										if(db.isError())
										{
											CLog	log;
											log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]:action == " + action + ": error updating user session in DB(`sessions`)");
										}
										else
										{
											mapResult["result"] = "success";

											{
												MESSAGE_DEBUG("", action, "session checks: DB session (" + sessidHTTP + ") switched over to user [" + user.GetLogin() + "]");
											}
										}

									}
									else
									{
										CLog	log;
										log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]:action == " + action + ": persisted session(" + sessidPersistence + ") _disappeared_ from DB");
									}
								}
							} // --- if(sessidHTTP.length() < 5)
						}
						else
						{
							CLog	log;
							log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]:action == " + action + ": session consistency check failed");
						}
					}
					else
					{
						ostringstream	ost;

						{
							MESSAGE_DEBUG("", action, "persisted session not found in DB. Need to recreate session");
						}

	/*						if(!indexPage.Cookie_Expire())
						{
							CLog	log;
							log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]: Error in session expiration");
						} // --- if(!indexPage.Cookie_Expire())

						// --- redirect URL
						ost.str("");
						ost << "/?rand=" << GetRandom(10);

						indexPage.Redirect(ost.str());
	*/
					} // --- if(indexPage.SessID_Load_FromDB(sessidHTTP))
				} // --- if(sessidHTTP.length() < 5)
			} // --- if(isRateLimited)
			else
			{
				{
					CLog	log;
					log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]:action == " + action + ":ERROR: BruteForce detected REMOTE_ADDR [", (remoteAddr ? string(remoteAddr) : ""), "]");
				}
			}

			{
				MESSAGE_DEBUG("", action, "finish");
			}
		} // --- if(action == "CheckSessionPersistence")


		if(action == "logout")
		{
			ostringstream	ost;
			string		sessid;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			sessid = indexPage.GetCookie("sessid");
			if(sessid.length() > 0)
			{
				ostringstream	ost;
				ost.str("");
				ost << "update sessions set `user`='Guest', `expire`='1' where `id`='" << sessid << "'";
				db.Query(ost.str());

				if(!indexPage.Cookie_Expire()) {
					CLog	log;
					log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]: Error in session expiration");
				} // --- if(!indexPage.Cookie_Expire())

			}

			{
				MESSAGE_DEBUG("", action, "action == " + action + ": end");
			}
		}


		// --- Build the response
		{ // --- Just for definition of ost
			ostringstream	ost;

			ost.str("");
			ost << "{" << endl;
			for(map<string, string>::iterator it = mapResult.begin(); it != mapResult.end(); it++)
			{
				if((it->first).find("Array") != string::npos)
				{
					ost << "\"" << it->first << "\" : " << it->second;
				}
				else if((it->first).find("Object") != string::npos)
				{
					ost << "\"" << it->first << "\" : " << it->second;
				}
				else
				{
					ost << "\"" << it->first << "\" : \"" << it->second << "\"";
				}
				if(next(it) != mapResult.end()) ost << ",	";
				ost << endl;
			}
			ost << "}" << endl;

			indexPage.RegisterVariableForce("result", ost.str());
		}


		if(!indexPage.SetTemplate("json_response.htmlt"))
		{
			CLog	log;
			log.Write(ERROR, __func__ + string("[") + to_string(__LINE__) + "]: action == " + action + ": ERROR can't find template json_response.htmlt");
			throw CExceptionHTML("user not activated");
		} // if(!indexPage.SetTemplate("AJAX_getNewsFeed.htmlt"))

		indexPage.OutTemplate();

	}
	catch(CExceptionHTML &c)
	{
		CLog	log;

		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);

		log.Write(DEBUG, "catch CExceptionHTML: DEBUG exception reason: [", c.GetReason(), "]");

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

		log.Write(ERROR, "catch CException: exception: ERROR  ", c.GetReason());

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(-1);
	}
	catch(exception& e)
	{
		CLog 	log;
		log.Write(ERROR, "catch standard exception: ERROR  ", e.what());

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

