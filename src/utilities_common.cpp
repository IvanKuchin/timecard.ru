#include "utilities_common.h"

bool RegisterInitialVariables(CCgi *indexPage, CMysql *db, CUser *user)
{
	bool	result = true;

	MESSAGE_DEBUG("", "", "start");

	indexPage->RegisterVariableForce("rand", GetRandom(10));
	indexPage->RegisterVariableForce("random", GetRandom(10));
	indexPage->RegisterVariableForce("DOMAIN_NAME", DOMAIN_NAME);
	indexPage->RegisterVariableForce("site_theme", DEFAULT_SITE_THEME);

	if(getenv("REMOTE_ADDR")) indexPage->RegisterVariableForce("REMOTE_ADDR", getenv("REMOTE_ADDR"));
	else
	{
		MESSAGE_ERROR("", "", "env variable REMOTE_ADDR doesn't defined");
		result = false;
	}
	if(getenv("SERVER_NAME")) indexPage->RegisterVariableForce("SERVER_NAME", getenv("SERVER_NAME"));
	else
	{
		MESSAGE_ERROR("", "", "env variable SERVER_NAME doesn't defined");
		result = false;
	}

	if(db->Query("SELECT `value` FROM `settings_default` WHERE `setting`=\"keywords_head\""))
		indexPage->RegisterVariable("keywords_head", db->Get(0, "value"));
	else
	{
		MESSAGE_ERROR("", "", "getting keywords_head FROM settings_default");
	}

	if(db->Query("SELECT `value` FROM `settings_default` WHERE `setting`=\"title_head\""))
		indexPage->RegisterVariable("title_head", db->Get(0, "value"));
	else
	{
		MESSAGE_ERROR("", "", "getting title_head FROM settings_default");
	}

	if(db->Query("SELECT `value` FROM `settings_default` WHERE `setting`=\"description_head\""))
		indexPage->RegisterVariable("description_head", db->Get(0, "value"));
	else
	{
		MESSAGE_ERROR("", "", "getting description_head FROM settings_default");
	}

	if(db->Query("SELECT `value` FROM `settings_default` WHERE `setting`=\"NewsOnSinglePage\""))
		indexPage->RegisterVariable("NewsOnSinglePage", db->Get(0, "value"));
	else
	{
		MESSAGE_ERROR("", "", "getting NewsOnSinglePage FROM settings_default");

		indexPage->RegisterVariable("NewsOnSinglePage", to_string(NEWS_ON_SINGLE_PAGE));
	}

	if(db->Query("SELECT `value` FROM `settings_default` WHERE `setting`=\"FriendsOnSinglePage\""))
		indexPage->RegisterVariable("FriendsOnSinglePage", db->Get(0, "value"));
	else
	{
		MESSAGE_ERROR("", "", "getting FriendsOnSinglePage FROM settings_default");

		indexPage->RegisterVariable("FriendsOnSinglePage", to_string(FRIENDS_ON_SINGLE_PAGE));
	}
			
	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string GenerateSession(string action, CCgi *indexPage, CMysql *db, CUser *user)
{
	string			lng, sessidHTTP;
	ostringstream	ost;

	MESSAGE_DEBUG("", "", "start");

	sessidHTTP = indexPage->SessID_Get_FromHTTP();
	if(sessidHTTP.length() < 5)
	{
		MESSAGE_DEBUG("", action, "session cookie doesn't exists, generating new session");

		sessidHTTP = indexPage->SessID_Create_HTTP_DB();
		if(sessidHTTP.length() < 5)
		{
			MESSAGE_ERROR("", action, "in generating session ID");
			throw CExceptionHTML("session can't be created");
		}

		if(action.length())
		{
		}
		else
		{
			// --- Safari browser issue:
			// --- 		1) cookie set via HTTP-response
			// ---		2) JS removed sessid cookie
			// ---		3) issue: cookie set on (1) step keeps reappear after 5 sec.
			// --- To address Safari JS cookie issue, cookie should not be set on request "/"
			MESSAGE_DEBUG("", "", "session cookie doesn't exists. Flow before check persitense session, no need to generate cookie. (workaround with JS cookie issue in Safari)");
		}

		action = GUEST_USER_DEFAULT_ACTION;
	}
	else
	{
		if(indexPage->SessID_Load_FromDB(sessidHTTP))
		{
			if(indexPage->SessID_CheckConsistency())
			{
				if(indexPage->SessID_Update_HTTP_DB())
				{
					indexPage->RegisterVariableForce("loginUser", "");
					indexPage->RegisterVariableForce("loginUserID", "");

					user->SetDB(db);
					if(indexPage->SessID_Get_UserFromDB() != "Guest")
					{
						// --- place 2change user FROM user->email to user->id
						if(user->GetFromDBbyEmail(indexPage->SessID_Get_UserFromDB()))
						{
							string			avatarPath = "empty";

							indexPage->RegisterVariableForce("loginUser", indexPage->SessID_Get_UserFromDB());
							indexPage->RegisterVariableForce("loginUserID", user->GetID());
							indexPage->RegisterVariableForce("myLogin", user->GetLogin());
							indexPage->RegisterVariableForce("myFirstName", user->GetName());
							indexPage->RegisterVariableForce("myLastName", user->GetNameLast());
							indexPage->RegisterVariableForce("myUserAvatar", user->GetAvatar());
							indexPage->RegisterVariableForce("user_type", user->GetType());

							if((action.find("AJAX_") != string::npos) || (action.find("JSON_") != string::npos))
							{
								// --- don't look for user theme if it is AJAX_ or JSON_ request
								// --- Site theme required only if it is template request
								// --- this will save one request to DB
							}
							else
								indexPage->RegisterVariableForce("site_theme", user->GetSiteTheme());

/*							// --- Get user avatars
							if(db->Query("SELECT * FROM `users_avatars` WHERE `userid`=\"" + user->GetID() + "\" and `isActive`=\"1\";"))
								avatarPath = "/images/avatars/avatars" + string(db->Get(0, "folder")) + "/" + string(db->Get(0, "filename"));

							indexPage->RegisterVariableForce("myUserAvatar", avatarPath);
*/
							MESSAGE_DEBUG("", action, "user (" + user->GetLogin() + ") logged in");
						}
						else
						{
							// --- enforce to close session
							action = "logout";

							MESSAGE_ERROR("", action, "user (" + indexPage->SessID_Get_UserFromDB() + ") session exists on client device, but not in the DB. Change action to logout");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user (" + user->GetLogin() + ") surfing");

						if(user->GetFromDBbyLogin(user->GetLogin()))
						{
							indexPage->RegisterVariableForce("loginUser", user->GetLogin());
							indexPage->RegisterVariableForce("loginUserID", user->GetID());
							indexPage->RegisterVariableForce("myLogin", user->GetLogin());
							indexPage->RegisterVariableForce("myFirstName", user->GetName());
							indexPage->RegisterVariableForce("myLastName", user->GetNameLast());
							indexPage->RegisterVariableForce("user_type", user->GetType());
						}
						else
						{
							// --- enforce to close session
							action = "logout";

							MESSAGE_ERROR("", action, "user (" + indexPage->SessID_Get_UserFromDB() + ") session exists on client device, but not in the DB. Change the [action = logout].");
						}
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "update session in HTTP or DB failed");
				}
			}
			else
			{
				MESSAGE_ERROR("", action, "session consistency check failed");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "cookie session and DB session is not equal. Need to recreate session")

			if(!indexPage->Cookie_Expire()) MESSAGE_ERROR("", action, "fail to expire session");

			indexPage->Redirect("/" + GUEST_USER_DEFAULT_ACTION + "?rand=" + GetRandom(10));
		} // --- if(indexPage->SessID_Load_FromDB(sessidHTTP))
	} // --- if(sessidHTTP.length() < 5)

	// --- check default action if needed
	if(action.empty())
	{
		if(user->GetLogin().length() and (user->GetLogin() != "Guest"))
		{
			action = GetDefaultActionFromUserType(user->GetType(), db);

			MESSAGE_DEBUG("", "", "META-registration: action has been overriden to 'logged-in user' default action [action = " + action + "]");
		}
		else
		{
			action = GUEST_USER_DEFAULT_ACTION;

			MESSAGE_DEBUG("", "", "META-registration: action has been overriden to 'guest user' default action [action = " + action + "]");

			// --- user login should not be empty, either "Guest", or actual user login
			if(user->GetLogin().empty()) MESSAGE_ERROR("", "", "user login is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (action = " + action + ")");

	return action;
}

string 		LogoutIfGuest(string action, CCgi *indexPage, CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start");

	if(user->GetLogin() == "Guest")
	{
		if(action.compare(0, 5, "AJAX_"))
		{
			string	template_name = "json_response.htmlt";

			indexPage->RegisterVariableForce("result", "{\"result\":\"error\",\"description\":\"re-login required\"}");

			if(!indexPage->SetTemplate(template_name))
			{
				MESSAGE_ERROR("", "", "can't find template " + template_name);
			}
		}
		else
		{
			// --- eventually it will throw exception (may need to be reworked)
			indexPage->Redirect("/" + GUEST_USER_DEFAULT_ACTION + "?rand=" + GetRandom(10));
		}

		action = GUEST_USER_DEFAULT_ACTION;
		MESSAGE_DEBUG("", "", "re-login required, reset action");
	}

	MESSAGE_DEBUG("", "", "finish");

	return action;

}
