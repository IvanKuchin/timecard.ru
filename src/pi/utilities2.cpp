#include "utilities.h"

auto	GetSHA512(const string &src) -> string
{
	string			result = "";
    unsigned char	digest[SHA512_DIGEST_LENGTH];

	MESSAGE_DEBUG("", "", "start");

    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, src.c_str(), src.length());
    SHA512_Final(digest, &ctx);

    char mdString[SHA512_DIGEST_LENGTH*2+1];
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
		sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    result = mdString;

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + " symbols)");

	return result;
}

string GetCandidatesListAppliedToVacancyInJSONFormat(string dbQuery, CMysql *db)
{
	string			result = "";
	int				itemsCount;
	ostringstream	ostResult;

	struct ItemClass
	{
		string	id;
		string	vacancy_id;
		string	user_id;
		string	answer1;
		string	answer2;
		string	answer3;
		string	language1;
		string	language2;
		string	language3;
		string	skill1;
		string	skill2;
		string	skill3;
		string	description;
		string	status;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");

	itemsCount = db->Query(dbQuery);
	for(int i = 0; i < itemsCount; ++i)
	{
		ItemClass	item;

		item.id = db->Get(i, "id");
		item.vacancy_id = db->Get(i, "vacancy_id");
		item.user_id = db->Get(i, "user_id");
		item.answer1 = db->Get(i, "answer1");
		item.answer2 = db->Get(i, "answer2");
		item.answer3 = db->Get(i, "answer3");
		item.language1 = db->Get(i, "language1");
		item.language2 = db->Get(i, "language2");
		item.language3 = db->Get(i, "language3");
		item.skill1 = db->Get(i, "skill1");
		item.skill2 = db->Get(i, "skill2");
		item.skill3 = db->Get(i, "skill3");
		item.description = db->Get(i, "description");
		item.status = db->Get(i, "status");
		item.eventTimestamp = db->Get(i, "eventTimestamp");

		itemsList.push_back(item);
	}

	for(int i = 0; i < itemsCount; ++i)
	{
		string	name = "", nameLast = "";

		if(ostResult.str().length()) ostResult << ", ";

		ostResult << "{"
					  "\"id\":\"" << itemsList[i].id << "\","
					  "\"vacancy_id\":\"" << itemsList[i].vacancy_id << "\","
					  "\"user_id\":\"" << itemsList[i].user_id << "\","
					  "\"user\":" << GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + itemsList[i].user_id + "\" AND `isblocked`=\"N\";", db, NULL) << ","
					  "\"answer1\":\"" << itemsList[i].answer1 << "\","
					  "\"answer2\":\"" << itemsList[i].answer2 << "\","
					  "\"answer3\":\"" << itemsList[i].answer3 << "\","
					  "\"language1\":\"" << itemsList[i].language1 << "\","
					  "\"language2\":\"" << itemsList[i].language2 << "\","
					  "\"language3\":\"" << itemsList[i].language3 << "\","
					  "\"skill1\":\"" << itemsList[i].skill1 << "\","
					  "\"skill2\":\"" << itemsList[i].skill2 << "\","
					  "\"skill3\":\"" << itemsList[i].skill3 << "\","
					  "\"description\":\"" << itemsList[i].description << "\","
					  "\"status\":\"" << itemsList[i].status << "\","
					  "\"eventTimestamp\":\"" << itemsList[i].eventTimestamp << "\""
					  "}";
	}

	result = ostResult.str();

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (result.length() = " + to_string(result.length()) + ")");
	}
	return result;
}


// --- input:
//		dbQuery - example "SELECT * FROM `open_vacancies`;"
//		isAdminUser:
//			true - candidates count will be included
//			false - candidates count will NOT be included
//		db - pointer to CMysql object
//		user - pointer to user (if admin_user - return correct answers)
string GetOpenVacanciesInJSONFormat(string dbQuery, CMysql *db, CUser *user/* = NULL*/)
{
	struct ItemClass
	{
		string id;
		string company_id;
		string company_position_id;
		string geo_locality_id;
		string salary_min;
		string salary_max;
		string start_month;
		string work_format;
		string description;
		string question1;
		string question2;
		string question3;
		string correct_answer1;
		string correct_answer2;
		string correct_answer3;
		string answer11;
		string answer12;
		string answer13;
		string answer21;
		string answer22;
		string answer23;
		string answer31;
		string answer32;
		string answer33;
		string language1_id;
		string language2_id;
		string language3_id;
		string skill1_id;
		string skill2_id;
		string skill3_id;
		string publish_finish;
		string publish_period;
	};

	vector<ItemClass>		itemsList;
	ostringstream   		ostResult;
	string		 			result = "";
	int						itemsCount = 0;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");

	itemsCount = db->Query(dbQuery);
	for(int i = 0; i < itemsCount; ++i)
	{
		ItemClass   item;

		item.id = db->Get(i, "id");
		item.company_id = db->Get(i, "company_id");
		item.company_position_id = db->Get(i, "company_position_id");
		item.geo_locality_id = db->Get(i, "geo_locality_id");
		item.salary_min = db->Get(i, "salary_min");
		item.salary_max = db->Get(i, "salary_max");
		item.start_month = db->Get(i, "start_month");
		item.work_format = db->Get(i, "work_format");
		item.description = db->Get(i, "description");
		item.language1_id = db->Get(i, "language1_id");
		item.language2_id = db->Get(i, "language2_id");
		item.language3_id = db->Get(i, "language3_id");
		item.skill1_id = db->Get(i, "skill1_id");
		item.skill2_id = db->Get(i, "skill2_id");
		item.skill3_id = db->Get(i, "skill3_id");
		item.publish_finish = db->Get(i, "publish_finish");
		item.publish_period = db->Get(i, "publish_period");
		item.question1 = db->Get(i, "question1");
		item.question2 = db->Get(i, "question2");
		item.question3 = db->Get(i, "question3");
		item.correct_answer1 = db->Get(i, "correct_answer1");
		item.correct_answer2 = db->Get(i, "correct_answer2");
		item.correct_answer3 = db->Get(i, "correct_answer3");
		item.answer11 = db->Get(i, "answer11");
		item.answer12 = db->Get(i, "answer12");
		item.answer13 = db->Get(i, "answer13");
		item.answer21 = db->Get(i, "answer21");
		item.answer22 = db->Get(i, "answer22");
		item.answer23 = db->Get(i, "answer23");
		item.answer31 = db->Get(i, "answer31");
		item.answer32 = db->Get(i, "answer32");
		item.answer33 = db->Get(i, "answer33");

		itemsList.push_back(item);
	}

	for(int i = 0; i < itemsCount; ++i)
	{
		string		company_position_title = "";
		string		geo_locality_title = "";
		string		geo_region_id = "";
		string		geo_region_title = "";
		string		language1_title = "";
		string		language2_title = "";
		string		language3_title = "";
		string		skill1_title = "";
		string		skill2_title = "";
		string		skill3_title = "";
		string		number_of_applied_candidates = "";
		string		admin_user_id = "";

		if(db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + itemsList[i].company_position_id + "\";"))
			company_position_title = db->Get(0, "title");
		else
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting company_position by id");
		}

		if(db->Query("SELECT * FROM `geo_locality` WHERE `id`=\"" + itemsList[i].geo_locality_id + "\";"))
		{
			geo_locality_title = db->Get(0, "title");
			geo_region_id = db->Get(0, "geo_region_id");
			if(db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + geo_region_id + "\";"))
			{
				geo_region_title = db->Get(0, "title");
			}
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting geo_region by id");
			}
		}
		else
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting geo_locality by id");
		}

		if(itemsList[i].language1_id != "0")
		{
			if(db->Query("SELECT * FROM `language` WHERE `id`=\"" + itemsList[i].language1_id + "\";"))
				language1_title = db->Get(0, "title");
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting language1 by id");
			}
		}

		if(itemsList[i].language2_id != "0")
		{
			if(db->Query("SELECT * FROM `language` WHERE `id`=\"" + itemsList[i].language2_id + "\";"))
				language2_title = db->Get(0, "title");
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting language2 by id");
			}
		}

		if(itemsList[i].language3_id != "0")
		{
			if(db->Query("SELECT * FROM `language` WHERE `id`=\"" + itemsList[i].language3_id + "\";"))
				language3_title = db->Get(0, "title");
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting language3 by id");
			}
		}

		if(itemsList[i].skill1_id != "0")
		{
			if(db->Query("SELECT * FROM `skill` WHERE `id`=\"" + itemsList[i].skill1_id + "\";"))
				skill1_title = db->Get(0, "title");
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting skill1 by id");
			}
		}

		if(itemsList[i].skill2_id != "0")
		{
			if(db->Query("SELECT * FROM `skill` WHERE `id`=\"" + itemsList[i].skill2_id + "\";"))
				skill2_title = db->Get(0, "title");
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting skill2 by id");
			}
		}

		if(itemsList[i].skill3_id != "0")
		{
			if(db->Query("SELECT * FROM `skill` WHERE `id`=\"" + itemsList[i].skill3_id + "\";"))
				skill3_title = db->Get(0, "title");
			else
			{
				CLog			log;
				log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail getting skill3 by id");
			}
		}

		if(db->Query("SELECT COUNT(*) as `count` FROM `company_candidates` WHERE vacancy_id=\"" + itemsList[i].id + "\" AND `status`=\"applied\";"))
			number_of_applied_candidates = db->Get(0, "count");
		else
			number_of_applied_candidates = "0";

		if(db->Query("SELECT `admin_userID` FROM `company` WHERE `id`=\"" + itemsList[i].company_id + "\";"))
			admin_user_id = db->Get(0, "admin_userID");

		if(ostResult.str().length()) ostResult << ", ";

		ostResult	<< "{"
					<< "\"id\":\"" << itemsList[i].id << "\","
					<< "\"company_id\":\"" << itemsList[i].company_id << "\","
					<< "\"company_position_id\":\"" << itemsList[i].company_position_id << "\","
					<< "\"company_position_title\":\"" << company_position_title << "\","
					<< "\"geo_locality_id\":\"" << itemsList[i].geo_locality_id << "\","
					<< "\"geo_locality_title\":\"" << geo_locality_title << "\","
					<< "\"geo_region_id\":\"" << geo_region_id << "\","
					<< "\"geo_region_title\":\"" << geo_region_title << "\","
					<< "\"salary_min\":\"" << itemsList[i].salary_min << "\","
					<< "\"salary_max\":\"" << itemsList[i].salary_max << "\","
					<< "\"start_month\":\"" << itemsList[i].start_month << "\","
					<< "\"work_format\":\"" << itemsList[i].work_format << "\","
					<< "\"description\":\"" << itemsList[i].description << "\","
					<< "\"question1\":\"" << itemsList[i].question1 << "\","
					<< "\"question2\":\"" << itemsList[i].question2 << "\","
					<< "\"question3\":\"" << itemsList[i].question3 << "\","
					<< "\"correct_answer1\":\"" << (user && (admin_user_id == user->GetID()) ? itemsList[i].correct_answer1 : "") << "\","
					<< "\"correct_answer2\":\"" << (user && (admin_user_id == user->GetID()) ? itemsList[i].correct_answer2 : "") << "\","
					<< "\"correct_answer3\":\"" << (user && (admin_user_id == user->GetID()) ? itemsList[i].correct_answer3 : "") << "\","
					<< "\"answer11\":\"" << itemsList[i].answer11 << "\","
					<< "\"answer12\":\"" << itemsList[i].answer12 << "\","
					<< "\"answer13\":\"" << itemsList[i].answer13 << "\","
					<< "\"answer21\":\"" << itemsList[i].answer21 << "\","
					<< "\"answer22\":\"" << itemsList[i].answer22 << "\","
					<< "\"answer23\":\"" << itemsList[i].answer23 << "\","
					<< "\"answer31\":\"" << itemsList[i].answer31 << "\","
					<< "\"answer32\":\"" << itemsList[i].answer32 << "\","
					<< "\"answer33\":\"" << itemsList[i].answer33 << "\","
					<< "\"language1_id\":\"" << itemsList[i].language1_id << "\","
					<< "\"language1_title\":\"" << language1_title << "\","
					<< "\"language2_id\":\"" << itemsList[i].language2_id << "\","
					<< "\"language2_title\":\"" << language2_title << "\","
					<< "\"language3_id\":\"" << itemsList[i].language3_id << "\","
					<< "\"language3_title\":\"" << language3_title << "\","
					<< "\"skill1_id\":\"" << itemsList[i].skill1_id << "\","
					<< "\"skill1_title\":\"" << skill1_title << "\","
					<< "\"skill2_id\":\"" << itemsList[i].skill2_id << "\","
					<< "\"skill2_title\":\"" << skill2_title << "\","
					<< "\"skill3_id\":\"" << itemsList[i].skill3_id << "\","
					<< "\"skill3_title\":\"" << skill3_title << "\","
					<< "\"publish_finish\":\"" << itemsList[i].publish_finish << "\","
					<< "\"publish_period\":\"" << itemsList[i].publish_period << "\","
					<< "\"number_of_applied_candidates\":\"" << (user && (admin_user_id == user->GetID()) ? number_of_applied_candidates : "") << "\""
					<<"}";
	}

	result = ostResult.str();

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

bool AmIMessageOwner(string messageID, CUser *user, CMysql *db)
{
	bool		result = false;
	
	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start (messageID: " + messageID + ")");
	}

	if(messageID.length() && (messageID != "0"))
	{
		if(db->Query("SELECT `userId`,`srcType` FROM `feed` WHERE `actionTypeId`='11' AND `actionId`=\"" + messageID + "\";"))
		{
			string		messageOwnerID = db->Get(0, "userId");
			string		messageOwnerType = db->Get(0, "srcType");

			if((messageOwnerType == "user") && (messageOwnerID == user->GetID()))
			{
				result = true;
			}
			else if(messageOwnerType == "company")
			{
				if(db->Query("SELECT `id` FROM `company` WHERE `id`=\"" + messageOwnerID + "\" AND `admin_userID`=\"" + user->GetID() + "\";"))
				{
					result = true;
				}
				else
				{
					CLog			log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: user(id:" + user->GetID() + ") not owning company(id:" + messageOwnerID + ")");
				}
			}
			else if(messageOwnerType == "group")
			{
				if(db->Query("SELECT `id` FROM `groups` WHERE `id`=\"" + messageOwnerID + "\" AND `owner_id`=\"" + user->GetID() + "\";"))
				{
					result = true;
				}
				else
				{
					CLog			log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: user(id:" + user->GetID() + ") not owning group(id:" + messageOwnerID + ")");
				}
			}
			else
			{
				{
					CLog			log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: unknown message type (id:" + messageID + ", type:" + messageOwnerType + ")");
				}
			}
		}
		else
		{
			{
				CLog			log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: message.id(" + messageID + ") not found");
			}
		}
	}
	else
	{
		{
			CLog			log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: message.id(" + messageID + ") empty or 0");
		}
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning value is " + (result ? "true" : "false") + ")");
	}

	return result;	
}

pair<string, string> GetMessageOwner(string messageID, CUser *user, CMysql *db)
{
	string		messageOwnerID = "";
	string		messageOwnerType = "";
	
	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start (messageID: " + messageID + ")");
	}

	if(db->Query("SELECT `userId`,`srcType` FROM `feed` WHERE `actionTypeId`='11' AND `actionId`=\"" + messageID + "\";"))
	{
		messageOwnerID = db->Get(0, "userId");
		messageOwnerType = db->Get(0, "srcType");
	}
	else
	{
		{
			CLog			log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: message.id(" + messageID + ") not found");
		}
	}

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning value pair<messageOwnerType, messageOwnerID>(" + messageOwnerType + ", " + messageOwnerID + ")");
	}

	return make_pair(messageOwnerType, messageOwnerID);
}

string	GetUserSubscriptionsInJSONFormat(string sqlQuery, CMysql *db)
{
	int		affected;
	string	result;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + string(db->Get(i, "id")) + "\",";
			result +=	"\"entity_id\":\"" + string(db->Get(i, "entity_id")) + "\",";
			result +=	"\"entity_type\":\"" + string(db->Get(i, "entity_type")) + "\",";
			result +=	"\"eventTimestamp\":\"" + string(db->Get(i, "eventTimestamp")) + "\"";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "user have no active subscriptions to any companies or groups");
	}

	MESSAGE_DEBUG("", "", "end (returning result length(" + to_string(result.length()) + ")");

	return result;
}

string	SubscribeToCompany(string companyID, CUser *user, CMysql *db)
{
	ostringstream	ostResult;

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");

	if(user && companyID.length())
	{
		if(db->Query("SELECT * FROM `users_subscriptions` WHERE `user_id`=\"" + user->GetID() + "\" AND `entity_type`=\"company\" AND `entity_id`=\"" + companyID + "\";"))
		{
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: already subscribed [companyID: " + companyID + ", userID: " + user->GetID() + "]");
			}

			ostResult << "\"result\": \"success\", \"description\": \"вы уже подписаны на новости этой компании\"";
		}
		else
		{
			if(db->InsertQuery("INSERT INTO `users_subscriptions` SET "
								"`user_id`=\"" + user->GetID() + "\","
								"`entity_type`=\"company\","
								"`entity_id`=\"" + companyID + "\","
								"`eventTimestamp`=UNIX_TIMESTAMP();"))
			{
				ostResult << "\"result\": \"success\"";

				// --- insert notification into feed
				if(db->Query("SELECT `id` FROM `feed` WHERE `userId`=\"" + user->GetID() + "\" AND `srcType`=\"user\" AND `actionTypeId`=\"63\" AND `actionId`=\"" + companyID + "\";"))
				{
					CLog		log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: already subscribed [companyID: " + companyID + ", userID: " + user->GetID() + "]");
				}
				else
				{
					if(db->InsertQuery("INSERT INTO `feed` (`userId`, `actionTypeId`, `actionId`, `eventTimestamp`) values(\"" + user->GetID() + "\", \"63\", \"" + companyID + "\", NOW())"))
					{
					}
					else
					{
						CLog		log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: inserting into `feed` table");
					}
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: error inserting into users subscription [companyID: " + companyID + ", userID: " + user->GetID() + "]");
				}

				ostResult << "\"result\": \"error\",\"description\": \"Ошибка БД\"";
			}
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: (companyID is empty) || (user == NULL) [companyID: " + companyID + ", userID: " + (user ? "not-NULL" : "NULL") + "]");
		}

		ostResult << "\"result\": \"error\",\"description\": \"Компания не найдена или пользователь неопределен\"";
	}


	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning result length(" + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

string	UnsubscribeFromCompany(string companyID, CUser *user, CMysql *db)
{
	ostringstream	ostResult;

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");

	if(user && companyID.length())
	{
		if(db->Query("SELECT `id` FROM `users_subscriptions` WHERE `user_id`=\"" + user->GetID() + "\" AND `entity_type`=\"company\" AND `entity_id`=\"" + companyID + "\";"))
		{
			string		subscriptionID = db->Get(0, "id");

			if(db->Query("SELECT `admin_userID` FROM `company` WHERE `id`=\"" + companyID + "\" AND `admin_userID`=\"" + user->GetID() + "\";"))
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: user can't unsubscribe from own company [companyID: " + companyID + ", userID: " + user->GetID() + "]");
				}

				ostResult << "\"result\": \"error\",\"description\": \"Вы не можете отписаться от собственной компании\"";
			}
			else
			{
				db->Query("DELETE FROM `users_subscriptions` WHERE `id`=\"" + subscriptionID + "\";");
				ostResult << "\"result\": \"success\"";

				// --- remove notification from feed
				if(db->Query("SELECT `id` FROM `feed` WHERE `userId`=\"" + user->GetID() + "\" AND `srcType`=\"user\" AND `actionTypeId`=\"63\" AND `actionId`=\"" + companyID + "\";"))
				{
					db->Query("DELETE FROM `feed` WHERE `userId`=\"" + user->GetID() + "\" AND `actionTypeId`=\"63\" AND `actionId`=\"" + companyID + "\";");
					if(db->isError())
					{
						CLog		log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: removeing form `feed` table");
					}
				}
				else
				{
					CLog		log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: feed subscription missed [companyID: " + companyID + ", userID: " + user->GetID() + "]");
				}

			}
		}
		else
		{
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: user not subscribed to company [companyID: " + companyID + ", userID: " + user->GetID() + "]");
			}

			ostResult << "\"result\": \"success\", \"description\": \"Вы не подписаны\"";
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: (companyID is empty) || (user == NULL) [companyID: " + companyID + ", userID: " + (user ? "not-NULL" : "NULL") + "]");
		}

		ostResult << "\"result\": \"error\",\"description\": \"Компания не найдена или пользователь неопределен\"";
	}


	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning result length(" + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

string	SubscribeToGroup(string groupID, CUser *user, CMysql *db)
{
	ostringstream	ostResult;

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");

	if(user && groupID.length())
	{
		if(db->Query("SELECT * FROM `users_subscriptions` WHERE `user_id`=\"" + user->GetID() + "\" AND `entity_type`=\"group\" AND `entity_id`=\"" + groupID + "\";"))
		{
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: already subscribed [groupID: " + groupID + ", userID: " + user->GetID() + "]");
			}

			ostResult << "\"result\": \"success\", \"description\": \"вы уже подписаны на новости этой группы\"";
		}
		else
		{
			if(db->InsertQuery("INSERT INTO `users_subscriptions` SET "
								"`user_id`=\"" + user->GetID() + "\","
								"`entity_type`=\"group\","
								"`entity_id`=\"" + groupID + "\","
								"`eventTimestamp`=UNIX_TIMESTAMP();"))
			{
				ostResult << "\"result\": \"success\"";

				// --- insert notification into feed
				if(db->Query("SELECT `id` FROM `feed` WHERE `userId`=\"" + user->GetID() + "\" AND `srcType`=\"user\" AND `actionTypeId`=\"64\" AND `actionId`=\"" + groupID + "\";"))
				{
					CLog		log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: already subscribed [groupID: " + groupID + ", userID: " + user->GetID() + "]");
				}
				else
				{
					if(db->InsertQuery("INSERT INTO `feed` (`userId`, `actionTypeId`, `actionId`, `eventTimestamp`) values(\"" + user->GetID() + "\", \"64\", \"" + groupID + "\", NOW())"))
					{
					}
					else
					{
						CLog		log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: inserting into `feed` table");
					}
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: error inserting into users subscription [groupID: " + groupID + ", userID: " + user->GetID() + "]");
				}

				ostResult << "\"result\": \"error\",\"description\": \"Ошибка БД\"";
			}
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: (groupID is empty) || (user == NULL) [groupID: " + groupID + ", userID: " + (user ? "not-NULL" : "NULL") + "]");
		}

		ostResult << "\"result\": \"error\",\"description\": \"Компания не найдена или пользователь неопределен\"";
	}


	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning result length(" + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

string	UnsubscribeFromGroup(string groupID, CUser *user, CMysql *db)
{
	ostringstream	ostResult;

	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostResult.str("");

	if(user && groupID.length())
	{
		if(db->Query("SELECT `id` FROM `users_subscriptions` WHERE `user_id`=\"" + user->GetID() + "\" AND `entity_type`=\"group\" AND `entity_id`=\"" + groupID + "\";"))
		{
			string		subscriptionID = db->Get(0, "id");

			if(db->Query("SELECT `owner_id` FROM `groups` WHERE `id`=\"" + groupID + "\" AND `owner_id`=\"" + user->GetID() + "\";"))
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: user can't unsubscribe from own group [groupID: " + groupID + ", userID: " + user->GetID() + "]");
				}

				ostResult << "\"result\": \"error\",\"description\": \"Вы не можете отписаться от собственной группы\"";
			}
			else
			{
				db->Query("DELETE FROM `users_subscriptions` WHERE `id`=\"" + subscriptionID + "\";");
				ostResult << "\"result\": \"success\"";
				
				// --- remove notification from feed
				if(db->Query("SELECT `id` FROM `feed` WHERE `userId`=\"" + user->GetID() + "\" AND `srcType`=\"user\" AND `actionTypeId`=\"64\" AND `actionId`=\"" + groupID + "\";"))
				{
					db->Query("DELETE FROM `feed` WHERE `userId`=\"" + user->GetID() + "\" AND `actionTypeId`=\"64\" AND `actionId`=\"" + groupID + "\";");
					if(db->isError())
					{
						CLog		log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: removeing form `feed` table");
					}
				}
				else
				{
					CLog		log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: feed subscription missed [groupID: " + groupID + ", userID: " + user->GetID() + "]");
				}
			}
		}
		else
		{
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: user not subscribed to group [groupID: " + groupID + ", userID: " + user->GetID() + "]");
			}

			ostResult << "\"result\": \"success\", \"description\": \"Вы не подписаны\"";
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: (groupID is empty) || (user == NULL) [groupID: " + groupID + ", userID: " + (user ? "not-NULL" : "NULL") + "]");
		}

		ostResult << "\"result\": \"error\",\"description\": \"Компания не найдена или пользователь неопределен\"";
	}


	{
		CLog			log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (returning result length(" + to_string(ostResult.str().length()) + ")");
	}

	return ostResult.str();
}

string GetGroupListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	link;
		string	title;
		string	description;
		string	logo_folder;
		string	logo_filename;
		string	owner_id;
		string	isBlocked;
		string	eventTimestampCreation;
		string	eventTimestampLastPost;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		groupsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int						groupCounter = affected;

		groupsList.reserve(groupCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			ItemClass	group;

			group.id = db->Get(i, "id");
			group.link = db->Get(i, "link");
			group.title = db->Get(i, "title");
			group.description = db->Get(i, "description");
			group.logo_folder = db->Get(i, "logo_folder");
			group.logo_filename = db->Get(i, "logo_filename");
			group.owner_id = db->Get(i, "owner_id");
			group.isBlocked = db->Get(i, "isBlocked");
			group.eventTimestampCreation = db->Get(i, "eventTimestampCreation");
			group.eventTimestampLastPost = db->Get(i, "eventTimestampLastPost");

			groupsList.push_back(group);
		}

		for(int i = 0; i < groupCounter; i++)
		{
				string		numberOfMembers = "0";

				if(ostFinal.str().length()) ostFinal << ", ";

				if(db->Query("SELECT COUNT(*) as numberOfMembers FROM `users_subscriptions` WHERE `entity_type`=\"group\" AND `entity_id`=\"" + groupsList[i].id + "\";"))
					numberOfMembers = db->Get(0, "numberOfMembers");

				ostFinal << "{";
				ostFinal << "\"id\": \""				  	<< groupsList[i].id << "\",";
				ostFinal << "\"link\": \""					<< groupsList[i].link << "\",";
				ostFinal << "\"title\": \""					<< groupsList[i].title << "\",";
				ostFinal << "\"description\": \""			<< groupsList[i].description << "\",";
				ostFinal << "\"logo_folder\": \""			<< groupsList[i].logo_folder << "\",";
				ostFinal << "\"logo_filename\": \""			<< groupsList[i].logo_filename << "\",";
				ostFinal << "\"isMine\": \""				<< (user ? groupsList[i].owner_id == user->GetID() : false) << "\",";
				ostFinal << "\"numberOfMembers\": \""		<< numberOfMembers << "\",";
				ostFinal << "\"isBlocked\": \""				<< groupsList[i].isBlocked << "\",";
				ostFinal << "\"eventTimestampCreation\": \""<< groupsList[i].eventTimestampCreation << "\",";
				ostFinal << "\"eventTimestampLastPost\": \""<< groupsList[i].eventTimestampLastPost << "\"";
				ostFinal << "}";
		} // --- for loop through group list
	} // --- if sql-query on group selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no groups returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

bool isBotIP(string ip)
{
	bool			result = false;
	vector<string>	botsIPs = {
		// --- Google bots
		string("66.249.64."),
		string("66.249.65."),
		string("66.249.66."),
		string("66.249.67."),
		string("66.249.68."),
		string("66.249.69."),
		string("66.249.71."),
		string("66.249.72."),
		string("66.249.73."),
		string("66.249.74."),
		string("66.249.75."),
		string("66.249.76."),
		string("66.249.77."),
		string("66.249.78."),
		string("66.249.79."),
		string("66.249.80."),
		string("66.249.81."),
		string("66.249.82."),
		string("66.249.83."),
		string("66.249.84."),
		string("66.249.85."),
		string("66.249.86."),
		string("66.249.87."),
		string("66.249.88."),
		string("66.249.89."),
		string("66.249.90."),
		string("66.249.91."),
		string("66.249.92."),
		string("66.249.93."),
		string("66.249.94."),
		string("66.249.95.")
		};

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	if(ip.length())
	{
		for(string &tmp : botsIPs)
		{
			if(ip.find(tmp) != string::npos)
			{
				result = true;
				break;
			}
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: IP is empty");
		}
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result  = " + (result ? "true" : "false") + ")");
	}

	return result;
}

string GetGiftListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	link;
		string	title;
		string	description;
		string	folder;
		string	filename;
		string	requested_quantity;
		string	gained_quantity;
		string	estimated_price;
		string	user_id;
		string	eventTimestamp;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		groupsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int			groupCounter = affected;

		groupsList.reserve(groupCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			ItemClass	gift;

			gift.id = db->Get(i, "id");
			gift.link = db->Get(i, "link");
			gift.title = db->Get(i, "title");
			gift.description = db->Get(i, "description");
			gift.folder = db->Get(i, "logo_folder");
			gift.filename = db->Get(i, "logo_filename");
			gift.requested_quantity = db->Get(i, "requested_quantity");
			gift.gained_quantity = db->Get(i, "gained_quantity");
			gift.estimated_price = db->Get(i, "estimated_price");
			gift.user_id = db->Get(i, "user_id");
			gift.eventTimestamp = db->Get(i, "eventTimestamp");

			groupsList.push_back(gift);
		}

		for(int i = 0; i < groupCounter; i++)
		{
				if(ostFinal.str().length()) ostFinal << ", ";

				ostFinal << "{";
				ostFinal << "\"id\": \""				  	<< groupsList[i].id << "\",";
				ostFinal << "\"link\": \""					<< groupsList[i].link << "\",";
				ostFinal << "\"title\": \""					<< groupsList[i].title << "\",";
				ostFinal << "\"description\": \""			<< groupsList[i].description << "\",";
				ostFinal << "\"logo_folder\": \""			<< groupsList[i].folder << "\",";
				ostFinal << "\"logo_filename\": \""			<< groupsList[i].filename << "\",";
				ostFinal << "\"requested_quantity\": \""	<< groupsList[i].requested_quantity << "\",";
				ostFinal << "\"gained_quantity\": \""		<< groupsList[i].gained_quantity << "\",";
				ostFinal << "\"estimated_price\": \""		<< groupsList[i].estimated_price << "\",";
				ostFinal << "\"user_id\": \""				<< groupsList[i].user_id << "\",";
				ostFinal << "\"eventTimestamp\": \""		<< groupsList[i].eventTimestamp << "\"";
				ostFinal << "}";
		} // --- for loop through gift list

	} // --- if sql-query on gift selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no gifts returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

string GetGiftToGiveListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	user_id;
		string	gift_id;
		string	reserve_period;
		string	visibility;
		string	eventTimestamp;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		gifts_to_give_list;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int		groupCounter = affected;

		gifts_to_give_list.reserve(groupCounter);	// --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			ItemClass	gift;

			gift.id = db->Get(i, "id");
			gift.user_id = db->Get(i, "user_id");
			gift.gift_id = db->Get(i, "gift_id");
			gift.reserve_period = db->Get(i, "reserve_period");
			gift.visibility = db->Get(i, "visibility");
			gift.eventTimestamp = db->Get(i, "eventTimestamp");

			gifts_to_give_list.push_back(gift);
		}

		for(int i = 0; i < groupCounter; i++)
		{
				if(ostFinal.str().length()) ostFinal << ", ";

				ostFinal << "{";
				ostFinal << "\"id\": \""				<< gifts_to_give_list[i].id << "\",";
				ostFinal << "\"user_id\": \""			<< (user && (user->GetID() == gifts_to_give_list[i].user_id) ? user->GetID() : "") << "\",";
				ostFinal << "\"gift_id\": \""			<< gifts_to_give_list[i].gift_id << "\",";
				ostFinal << "\"reserve_period\": \""	<< gifts_to_give_list[i].reserve_period << "\",";
				ostFinal << "\"visibility\": \""		<< gifts_to_give_list[i].visibility << "\",";
				ostFinal << "\"eventTimestamp\": \""	<< gifts_to_give_list[i].eventTimestamp << "\"";
				ostFinal << "}";
		} // --- for loop through gift list

	} // --- if sql-query on gift selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no gifts returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}


bool isAdverseWordsHere(string text, CMysql *db)
{
	bool			result = false;
	int				affected;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	affected = db->Query("SELECT * FROM `dictionary_adverse`;");
	if(affected)
	{
		text = toLower(text);

		for(int i = 0; i < affected; i++)
		{
			string		checkingWord = db->Get(i, "word");

			if(text.find(checkingWord) != string::npos)
			{
				{
					CLog	log;
					log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: adverse word.id[" + db->Get(i, "id") + "]");
				}

				result = true;
				break;
			}
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: adverse_word table is empty");
		}

	}

	
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result  = " + (result ? "true" : "false") + ")");
	}
	
	return result;
}

string GetEventListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	link;
		string	title;
		string	address;
		string	accessType;
		string	startTimestamp;
		string	description;
		string	logo_folder;
		string	logo_filename;
		string	owner_id;
		string	isBlocked;
		string	eventTimestampCreation;
		string	eventTimestampLastPost;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		eventsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int						eventCounter = affected;

		eventsList.reserve(eventCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			ItemClass	event;

			event.id = db->Get(i, "id");
			event.link = db->Get(i, "link");
			event.title = db->Get(i, "title");
			event.address = db->Get(i, "address");
			event.accessType = db->Get(i, "accessType");
			event.startTimestamp = db->Get(i, "startTimestamp");
			event.description = db->Get(i, "description");
			event.logo_folder = db->Get(i, "logo_folder");
			event.logo_filename = db->Get(i, "logo_filename");
			event.owner_id = db->Get(i, "owner_id");
			event.isBlocked = db->Get(i, "isBlocked");
			event.eventTimestampCreation = db->Get(i, "eventTimestampCreation");
			event.eventTimestampLastPost = db->Get(i, "eventTimestampLastPost");

			eventsList.push_back(event);
		}

		for(int i = 0; i < eventCounter; i++)
		{
				if(ostFinal.str().length()) ostFinal << ", ";

				ostFinal << "{";
				ostFinal << "\"id\": \""				  	<< eventsList[i].id << "\",";
				ostFinal << "\"link\": \""					<< eventsList[i].link << "\",";
				ostFinal << "\"title\": \""					<< eventsList[i].title << "\",";
				ostFinal << "\"address\": \""				<< eventsList[i].address << "\",";
				ostFinal << "\"accessType\": \""			<< eventsList[i].accessType << "\",";
				ostFinal << "\"startTimestamp\": \""		<< eventsList[i].startTimestamp << "\",";
				ostFinal << "\"description\": \""			<< eventsList[i].description << "\",";
				ostFinal << "\"logo_folder\": \""			<< eventsList[i].logo_folder << "\",";
				ostFinal << "\"logo_filename\": \""			<< eventsList[i].logo_filename << "\",";
				ostFinal << "\"isMine\": \""				<< (user ? eventsList[i].owner_id == user->GetID() : false) << "\",";
				ostFinal << "\"hosts\": ["					<< GetEventHostsListInJSONFormat("SELECT * FROM `event_hosts` WHERE `event_id`=\"" + eventsList[i].id + "\";", db, user) << "],";
				ostFinal << "\"guests\": ["					<< GetEventGuestsListInJSONFormat("SELECT * FROM `event_guests` WHERE `event_id`=\"" + eventsList[i].id + "\";", db, user) << "],";
				ostFinal << "\"isBlocked\": \""				<< eventsList[i].isBlocked << "\",";
				ostFinal << "\"eventTimestampCreation\": \""<< eventsList[i].eventTimestampCreation << "\",";
				ostFinal << "\"eventTimestampLastPost\": \""<< eventsList[i].eventTimestampLastPost << "\"";
				ostFinal << "}";
		} // --- for loop through event list
	} // --- if sql-query on event selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no events returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

string GetEventHostsListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	event_id;
		string	user_id;
		string	eventTimestamp;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		eventHostsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int		eventHostsCounter = affected;

		eventHostsList.reserve(eventHostsCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < eventHostsCounter; i++)
		{
			ItemClass	event_host;

			event_host.id = db->Get(i, "id");
			event_host.event_id = db->Get(i, "event_id");
			event_host.user_id = db->Get(i, "user_id");
			event_host.eventTimestamp = db->Get(i, "eventTimestamp");

			eventHostsList.push_back(event_host);
		}

		for(int i = 0; i < eventHostsCounter; i++)
		{
				if(db->Query("SELECT `id`,`name`,`nameLast` FROM `users` WHERE `id`=\"" + eventHostsList[i].user_id + "\";"))
				{
					string		userName = db->Get(0, "name");
					string		userNameLast = db->Get(0, "nameLast");
					string		avatar = "";

					if(db->Query("SELECT `folder`, `filename` FROM `users_avatars` WHERE `isActive`=\"1\" AND `userid`=\"" + eventHostsList[i].user_id + "\";"))
						avatar = string("/images/avatars/avatars") + db->Get(0, "folder") + "/" + db->Get(0, "filename");

					if(ostFinal.str().length()) ostFinal << ", ";

					ostFinal << "{";
					ostFinal << "\"id\": \""				<< eventHostsList[i].id << "\",";
					ostFinal << "\"user_id\": \""			<< eventHostsList[i].user_id << "\",";
					ostFinal << "\"name\": \""				<< userName << "\",";
					ostFinal << "\"nameLast\": \""			<< userNameLast << "\",";
					ostFinal << "\"avatar\": \""			<< avatar << "\",";
					ostFinal << "\"status\": \""			<< "accepted\",";
					ostFinal << "\"gifts\": ["				<< GetGiftListInJSONFormat("SELECT * FROM `gifts` WHERE `user_id`=\"" + eventHostsList[i].user_id + "\";", db, user) << "],";
					ostFinal << "\"gifts_to_give\": [" 		<< GetGiftToGiveListInJSONFormat("SELECT * FROM `gifts_to_give` WHERE `gift_id` in (SELECT `id` FROM `gifts` WHERE `user_id`=\"" + eventHostsList[i].user_id + "\");", db, user) << "],";
					ostFinal << "\"eventTimestamp\": \""	<< eventHostsList[i].eventTimestamp << "\"";
					ostFinal << "}";

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: event_id(" + eventHostsList[i].event_id + ") user_id(" + eventHostsList[i].user_id + "), host_user_id not found");
				}
		}
	} // --- if sql-query on event selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no events returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

string GetEventGuestsListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	event_id;
		string	user_id;
		string	quick_registration_id;
		string	status;
		string	adults;
		string	kids;
		string	eventTimestamp;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		eventGuestsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int		eventGuestsCounter = affected;

		eventGuestsList.reserve(eventGuestsCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < eventGuestsCounter; i++)
		{
			ItemClass	event_guest;

			event_guest.id = db->Get(i, "id");
			event_guest.event_id = db->Get(i, "event_id");
			event_guest.user_id = db->Get(i, "user_id");
			event_guest.quick_registration_id = db->Get(i, "quick_registration_id");
			event_guest.status = db->Get(i, "status");
			event_guest.adults = db->Get(i, "adults");
			event_guest.kids = db->Get(i, "kids");
			event_guest.eventTimestamp = db->Get(i, "eventTimestamp");

			eventGuestsList.push_back(event_guest);
		}

		for(int i = 0; i < eventGuestsCounter; i++)
		{
				if(eventGuestsList[i].user_id != "0")
				{
					if(db->Query("SELECT `id`,`name`,`nameLast` FROM `users` WHERE `id`=\"" + eventGuestsList[i].user_id + "\";"))
					{
						string		userName = db->Get(0, "name");
						string		userNameLast = db->Get(0, "nameLast");
						string		avatar = "";

						if(db->Query("SELECT `folder`, `filename` FROM `users_avatars` WHERE `isActive`=\"1\" AND `userid`=\"" + eventGuestsList[i].user_id + "\";"))
							avatar = string("/images/avatars/avatars") + db->Get(0, "folder") + "/" + db->Get(0, "filename");

						if(ostFinal.str().length()) ostFinal << ", ";

						ostFinal << "{";
						ostFinal << "\"id\": \""				  	<< eventGuestsList[i].id << "\",";
						ostFinal << "\"user_id\": \""				<< eventGuestsList[i].user_id << "\",";
						ostFinal << "\"status\": \""				<< eventGuestsList[i].status << "\",";
						ostFinal << "\"name\": \""					<< userName << "\",";
						ostFinal << "\"nameLast\": \""				<< userNameLast << "\",";
						ostFinal << "\"avatar\": \""				<< avatar << "\",";
						ostFinal << "\"adults\": \""				<< eventGuestsList[i].adults << "\",";
						ostFinal << "\"kids\": \""					<< eventGuestsList[i].kids << "\",";
						ostFinal << "\"gifts\": ["					<< GetGiftListInJSONFormat("SELECT * FROM `gifts` WHERE `user_id`=\"" + eventGuestsList[i].user_id + "\";", db, user) << "],";
						ostFinal << "\"gifts_to_give\": [" 			<< GetGiftToGiveListInJSONFormat("SELECT * FROM `gifts_to_give` WHERE `gift_id` in (SELECT `id` FROM `gifts` WHERE `user_id`=\"" + eventGuestsList[i].user_id + "\");", db, user) << "],";
						ostFinal << "\"eventTimestamp\": \""		<< eventGuestsList[i].eventTimestamp << "\"";
						ostFinal << "}";
					}
					else
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]:  user_id(" + eventGuestsList[i].user_id + ") not found in `users` table)");
					}

				}
				else if(eventGuestsList[i].quick_registration_id.length())
				{
					if(user)
					{
						if(db->Query("SELECT `id` FROM `event_hosts` WHERE `event_id`=\"" + eventGuestsList[i].event_id + "\" AND `user_id`=\"" + user->GetID() + "\";"))
						{
							// --- email can be exposed to "event host"

							if(db->Query("SELECT `email` FROM `quick_registration` WHERE `id`=\"" + eventGuestsList[i].quick_registration_id + "\";"))
							{
								if(ostFinal.str().length()) ostFinal << ", ";

								ostFinal << "{";
								ostFinal << "\"id\": \""				  	<< eventGuestsList[i].id << "\",";
								ostFinal << "\"user_id\": \""				<< eventGuestsList[i].user_id << "\",";
								ostFinal << "\"status\": \""				<< eventGuestsList[i].status << "\",";
								ostFinal << "\"name\": \""					<< "" << "\",";
								ostFinal << "\"nameLast\": \""				<< "" << "\",";
								ostFinal << "\"avatar\": \""				<< "" << "\",";
								ostFinal << "\"email\": \""					<< db->Get(0, "email") << "\",";
								ostFinal << "\"adults\": \""				<< eventGuestsList[i].adults << "\",";
								ostFinal << "\"kids\": \""					<< eventGuestsList[i].kids << "\",";
								ostFinal << "\"gifts\": ["					<< "" << "],";
								ostFinal << "\"gifts_to_give\": [" 			<< "" << "],";
								ostFinal << "\"eventTimestamp\": \""		<< eventGuestsList[i].eventTimestamp << "\"";
								ostFinal << "}";
							}
							else
							{
								CLog	log;
								log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: quick_registration_id(" + eventGuestsList[i].quick_registration_id + ") not found");
							}

						}
						else
						{
							CLog	log;
							log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: email could be exposed to event host only. You are not event host. (event_id(" + eventGuestsList[i].event_id + ") user_id(" + user->GetID() + "))");
						}
					}
					else
					{
						CLog	log;
						log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: user is not an event host, therefore he have no access to email information");
					}
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: user_id and email are empty in event_guests table.");
				}
		}
	} // --- if sql-query on event selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no events returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

string ParseGPSLongitude(const string longitudeStr)
{
	string  result = "";
	smatch  cm;
	regex   format1("[-+]?[[:digit:]]+(\\.[[:digit:]]+)?");
	regex   format2("[EW]\\:[[:space:]]*([[:digit:]]+)((\\/)([[:digit:]]+))?\\,[[:space:]]*([[:digit:]]+)((\\/)([[:digit:]]+))?\\,[[:space:]]*([[:digit:]]+)((\\/)([[:digit:]]+))?");
	regex   format3(".*unknown.*");
	regex   format4(".[[:space:]]*");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start(" + longitudeStr + ")");
	}

	// --- format: +74.56 or 74.56 or 74
	if(regex_match(longitudeStr, format1))
	{
		result = longitudeStr;
	}
	else if(regex_match(longitudeStr, cm, format2))
	{
		string  degreeNumerator = cm[1];
		string  degreeDivisor = cm[4];
		string  minutesNumerator = cm[5];
		string  minutesDivisor = cm[8];
		string  secondsNumerator = cm[9];
		string  secondsDivisor = cm[12];
		float	temp = 0;

		if(!degreeDivisor.length() || !stof(degreeDivisor)) degreeDivisor = "1";
		if(!minutesDivisor.length() || !stof(minutesDivisor)) minutesDivisor = "1";
		if(!secondsDivisor.length() || !stof(secondsDivisor)) secondsDivisor = "1";

		if(stof(minutesNumerator)/stof(minutesDivisor) > 60)
		{
			// --- minutes couldn't be greater than 60
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: " + minutesNumerator + "/" + minutesDivisor + " minutes can't be greater 60");
			}
		}
		else if(stof(secondsNumerator)/stof(secondsDivisor) > 60)
		{
			// --- seconds couldn't be greater than 60
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: " + secondsNumerator + "/" + secondsDivisor + " seconds can't be greater 60");
			}
		}
		else
		{
			temp = stof(degreeNumerator)/stof(degreeDivisor) + stof(minutesNumerator)/stof(minutesDivisor)/60 + stof(secondsNumerator)/stof(secondsDivisor)/3600;
			result = (temp > 0 ? "+" : "") + to_string(temp);
		}

	}
	else if(regex_match(longitudeStr, format3))
	{
	}
	else if(regex_match(longitudeStr, format4))
	{
	}
	else if(longitudeStr.length() == 0)
	{
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: longitude(" + longitudeStr + ") didn't match any pattern");
		}
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (" + result + ")");
	}

	return result;
}

string ParseGPSLatitude(const string latitudeStr)
{
	string  result = "";
	smatch  cm;
	regex   format1("[-+]?[[:digit:]]+(\\.[[:digit:]]+)?");
	regex   format2("[NS]\\:[[:space:]]*([[:digit:]]+)((\\/)([[:digit:]]+))?\\,[[:space:]]*([[:digit:]]+)((\\/)([[:digit:]]+))?\\,[[:space:]]*([[:digit:]]+)((\\/)([[:digit:]]+))?");
	regex   format3(".*unknown.*");
	regex   format4(".[[:space:]]*");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start(" + latitudeStr + ")");
	}
	
	// --- format: +74.56 or 74.56 or 74
	if(regex_match(latitudeStr, regex(format1)))
	{
		result = latitudeStr;
	}
	else if(regex_match(latitudeStr, cm, format2))
	{
		string  degreeNumerator = cm[1];
		string  degreeDivisor = cm[4];
		string  minutesNumerator = cm[5];
		string  minutesDivisor = cm[8];
		string  secondsNumerator = cm[9];
		string  secondsDivisor = cm[12];
		float	temp = 0;

		if(!degreeDivisor.length() || !stof(degreeDivisor)) degreeDivisor = "1";
		if(!minutesDivisor.length() || !stof(minutesDivisor)) minutesDivisor = "1";
		if(!secondsDivisor.length() || !stof(secondsDivisor)) secondsDivisor = "1";

		if(stof(minutesNumerator)/stof(minutesDivisor) > 60)
		{
			// --- minutes couldn't be greater than 60
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: " + minutesNumerator + "/" + minutesDivisor + " minutes can't be greater 60");
			}
		}
		else if(stof(secondsNumerator)/stof(secondsDivisor) > 60)
		{
			// --- seconds couldn't be greater than 60
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: " + secondsNumerator + "/" + secondsDivisor + " seconds can't be greater 60");
			}
		}
		else
		{
			temp = stof(degreeNumerator)/stof(degreeDivisor) + stof(minutesNumerator)/stof(minutesDivisor)/60 + stof(secondsNumerator)/stof(secondsDivisor)/3600;
			result = (temp > 0 ? "+" : "") + to_string(temp);
		}

	}
	else if(regex_match(latitudeStr, format3))
	{
	}
	else if(regex_match(latitudeStr, format4))
	{
	}
	else if(latitudeStr.length() == 0)
	{
	}

	else
	{
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: latitude(" + latitudeStr + ") didn't match any pattern");
			}
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (" + result + ")");
	}

	return result;
}

string ParseGPSAltitude(const string altitudeStr)
{
	string  result = "";
	smatch  cm;
	regex   format1("[-+]?[[:digit:]]+(\\.[[:digit:]]+)?");
	regex   format2(".*\\:[[:space:]]*([[:digit:]]+)(\\/)([[:digit:]]+).*");
	regex   format3(".*unknown.*");
	regex   format4(".[[:space:]]*");
	regex   format5("([[:digit:]]+)(\\/)([[:digit:]]+)\n.*");
	regex   format6("[^[:digit:]]*([[:digit:]]+)(\\/)([[:digit:]]+)[^[:digit:]]*");

	MESSAGE_DEBUG("", "", "start(" + altitudeStr + ")");
	
	// --- format: +74.56 or 74.56 or 74
	if(regex_match(altitudeStr, regex(format1)))
	{
		result = altitudeStr;
	}
	else if(regex_match(altitudeStr, cm, format2) || regex_match(altitudeStr, cm, format5) || regex_match(altitudeStr, cm, format6))
	{
		string		altitudeNumerator = cm[1];
		string		altitudeDivisor = cm[3];
		float		temp = 0;

		if(!altitudeDivisor.length() || !stof(altitudeDivisor)) altitudeDivisor = "1";

		temp = stof(altitudeNumerator)/stof(altitudeDivisor);
		result = (temp > 0 ? "+" : "") + to_string(temp);
	}
	else if(regex_match(altitudeStr, format3))
	{
	}
	else if(regex_match(altitudeStr, format4))
	{
	}
	else if(altitudeStr.length() == 0)
	{
	}
	else
	{
		MESSAGE_ERROR("", "", "altitude(" + altitudeStr + ") didn't match any pattern");
	}

	MESSAGE_DEBUG("", "", "finish (" + result + ")");

	return result;
}

string ParseGPSSpeed(const string speedStr)
{
	string  result = "";
	smatch  cm;
	regex   format1("[-+]?[[:digit:]]+(\\.[[:digit:]]+)?");
	regex   format2("K\\:[[:space:]]*([[:digit:]]+)(\\/)([[:digit:]]+)");
	regex   format3(".*unknown.*");
	regex   format4(".[[:space:]]*");

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start(" + speedStr + ")");
	}

	// --- format: +74.56 or 74.56 or 74
	if(regex_match(speedStr, regex(format1)))
	{
		result = speedStr;
	}
	else if(regex_match(speedStr, cm, format2))
	{
		string	speedNumerator = cm[1];
		string	speedDivisor = cm[3];
		float		temp = 0;

		if(!speedDivisor.length() || !stof(speedDivisor)) speedDivisor = "1";

		temp = stof(speedNumerator)/stof(speedDivisor);
		result = (temp > 0 ? "+" : "") + to_string(temp);
	}
	else if(regex_match(speedStr, format3))
	{
	}
	else if(regex_match(speedStr, format4))
	{
	}
	else if(speedStr.length() == 0)
	{
	}
	else
	{
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: speed(" + speedStr + ") didn't match any pattern");
			}
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: finish (" + result + ")");
	}

	return result;
}

string	GetMySQLDateInJSONFormat(string dateString)
{
	string	result = "{}";
	regex   format1("([[:digit:]]+)\\-([[:digit:]]+)\\-([[:digit:]]+)");
	smatch  cm;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(regex_match(dateString, cm, format1))
	{
		string	dateYear = cm[1];
		string	dateMonth = cm[2];
		string	dateDate = cm[3];

		result = "{"
				"\"date\":" + to_string(stol(dateDate)) + ", "
				"\"month\":" + to_string(stol(dateMonth)) + ", "
				"\"year\":" + dateYear + ""
				"}";
	}
	else
	{
		MESSAGE_ERROR("", "", "date format taken from DB have to be in format YYYY-MM-DD, but gotten(" + dateString + ")");
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string GetCompanyPositionsInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	title;
		string	area;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		eventsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int						eventCounter = affected;

		eventsList.reserve(eventCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			ItemClass	event;

			event.id = db->Get(i, "id");
			event.title = db->Get(i, "title");
			event.area = db->Get(i, "area");

			eventsList.push_back(event);
		}

		for(int i = 0; i < eventCounter; i++)
		{
				if(ostFinal.str().length()) ostFinal << ", ";

				ostFinal << "{";
				ostFinal << "\"id\": \""				  	<< eventsList[i].id << "\",";
				ostFinal << "\"title\": \""					<< eventsList[i].title << "\",";
				ostFinal << "\"area\": \""					<< eventsList[i].area << "\"";
				ostFinal << "}";
		} // --- for loop through event list
	} // --- if sql-query on event selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no themes returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

string GetSiteThemesInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	name;
		string	path;
	};

	ostringstream			ost, ostFinal;
	string					sessid, lookForKey;
	int						affected;
	vector<ItemClass>		eventsList;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	ostFinal.str("");

	if((affected = db->Query(dbQuery)) > 0)
	{
		int						eventCounter = affected;

		eventsList.reserve(eventCounter);  // --- reserving allows avoid moving vector in memory
											// --- to fit vector into continous memory piece

		for(int i = 0; i < affected; i++)
		{
			ItemClass	event;

			event.id = db->Get(i, "id");
			event.name = db->Get(i, "name");
			event.path = db->Get(i, "path");

			eventsList.push_back(event);
		}

		for(int i = 0; i < eventCounter; i++)
		{
				if(ostFinal.str().length()) ostFinal << ", ";

				ostFinal << "{";
				ostFinal << "\"id\": \""				  	<< eventsList[i].id << "\",";
				ostFinal << "\"name\": \""					<< eventsList[i].name << "\",";
				ostFinal << "\"path\": \""					<< eventsList[i].path << "\"";
				ostFinal << "}";
		} // --- for loop through event list
	} // --- if sql-query on event selection success
	else
	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: there are no themes returned by request [", dbQuery, "]");
	}

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) + "]: end (result length = " + to_string(ostFinal.str().length()) + ")");
	}

	return ostFinal.str();
}

// --- this function different from other "InJSONFormat"-functions (zip_id vs sql_query)
// --- because sql doing INNER JOIN
string GetZipInJSONFormat(string zip_id, CMysql *db, CUser *user)
{
	ostringstream			ostFinal;
	string					dbQuery;

	MESSAGE_DEBUG("", "", "start");

	ostFinal.str("");

	dbQuery = 
				"SELECT  "
					"`geo_zip`.`id` AS `geo_zip_id`, "
					"`geo_locality_id`, "
					"`geo_region_id`, "
					"`geo_country_id`, "
					"`zip`, "
					"`geo_locality`.`title` AS `geo_locality_title`, "
					"`geo_region`.`title` AS `geo_region_title`, "
					"`geo_country`.`title` AS `geo_country_title` "
				"FROM `geo_zip` "
				"INNER JOIN `geo_locality` ON `geo_locality`.`id`=`geo_zip`.`geo_locality_id` "
				"INNER JOIN `geo_region` ON `geo_region`.`id`=`geo_locality`.`geo_region_id` "
				"INNER JOIN `geo_country` ON `geo_country`.`id`=`geo_region`.`geo_country_id` "
				"WHERE `geo_zip`.`id`=\"" + zip_id + "\";";

	if(db->Query(dbQuery))
	{
		ostFinal<< "{"
				<< 	"\"id\": \""	<< db->Get(0, "geo_zip_id") << "\","
				<< 	"\"zip\": \""	<< db->Get(0, "zip") << "\","
				<< 	"\"locality\": {"
				<< 		"\"id\": \""	<< db->Get(0, "geo_locality_id") << "\","
				<< 		"\"title\": \""	<< db->Get(0, "geo_locality_title") << "\","
				<<			"\"region\": {"
				<<				"\"id\": \""	<< db->Get(0, "geo_region_id") << "\","
				<<				"\"title\": \""	<< db->Get(0, "geo_region_title") << "\","
				<<				"\"country\": {"
				<<					"\"id\": \""	<< db->Get(0, "geo_country_id") << "\","
				<<					"\"title\": \""	<< db->Get(0, "geo_country_title") << "\""
				<<				"}"
				<<			"}"
				<< 	"}"
				<< "}";
	} // --- if sql-query on event selection success
	else
	{
		MESSAGE_DEBUG("", "", "zip.id(" + zip_id + ") not found or failed structure");
	}

	MESSAGE_DEBUG("", "", "finish (result length = " + to_string(ostFinal.str().length()) + ")");

	return ostFinal.str();
}

string GetBankInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	title;
		string	bik;
		string	account;
		string	geo_zip_id;
		string	address;
		string	phone;
		string	okato;
		string	okpo;
		string	regnum;
		string	srok;
	};

	vector<ItemClass>		items;
	ostringstream			ostFinal;
	int						affected;

	MESSAGE_DEBUG("", "", "start");

	ostFinal.str("");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id			= db->Get(i, "id");
			item.title		= db->Get(i, "title");
			item.bik		= db->Get(i, "bik");
			item.account	= db->Get(i, "account");
			item.geo_zip_id	= db->Get(i, "geo_zip_id");
			item.address	= db->Get(i, "address");
			item.phone		= db->Get(i, "phone");
			item.okato		= db->Get(i, "okato");
			item.okpo		= db->Get(i, "okpo");
			item.regnum		= db->Get(i, "regnum");
			item.srok		= db->Get(i, "srok");

			items.push_back(item);
		}

		for(auto i = 0u; i < items.size(); ++i)
		{
			if(i) ostFinal << ",";

			ostFinal<< "{"
					<<	"\"id\":\""			<< items[i].id << "\","
					<<	"\"title\":\""		<< items[i].title << "\","
					<<	"\"bik\":\""		<< items[i].bik << "\","
					<<	"\"account\":\""	<< items[i].account << "\","
					<<	"\"geo_zip_id\":["	<< GetZipInJSONFormat(items[i].geo_zip_id, db, user) << "],"
					<<	"\"geo_zip\":\""	<< items[i].geo_zip_id << "\","
					<<	"\"address\":\""	<< items[i].address << "\","
					<<	"\"phone\":\""		<< items[i].phone << "\","
					<<	"\"okato\":\""		<< items[i].okato << "\","
					<<	"\"okpo\":\""		<< items[i].okpo << "\","
					<<	"\"regnum\":\""		<< items[i].regnum << "\","
					<<	"\"srok\":\""		<< items[i].srok << "\""
					<< "}";
		}
	} // --- if sql-query on event selection success
	else
	{
		MESSAGE_DEBUG("", "", "no banks found from SQL-request");
	}

	MESSAGE_DEBUG("", "", "finish (result length = " + to_string(ostFinal.str().length()) + ")");

	return ostFinal.str();
}

string	GetSpelledZIPByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
				"SELECT  "
					"`geo_zip`.`id` AS `geo_zip_id`, "
					"`geo_locality_id`, "
					"`geo_region_id`, "
					"`geo_country_id`, "
					"`zip`, "
					"`geo_locality`.`title` AS `geo_locality_title`, "
					"`geo_region`.`title` AS `geo_region_title`, "
					"`geo_country`.`title` AS `geo_country_title` "
				"FROM `geo_zip` "
				"INNER JOIN `geo_locality` ON `geo_locality`.`id`=`geo_zip`.`geo_locality_id` "
				"INNER JOIN `geo_region` ON `geo_region`.`id`=`geo_locality`.`geo_region_id` "
				"INNER JOIN `geo_country` ON `geo_country`.`id`=`geo_region`.`geo_country_id` "
				"WHERE `geo_zip`.`id`=\"" + id + "\";"
	))
	{
		result = string(db->Get(0, "zip")) + " " + db->Get(0, "geo_country_title") + ", " + db->Get(0, "geo_region_title") + ", " + db->Get(0, "geo_locality_title");
	}
	else
	{
		MESSAGE_ERROR("", "", "geo_zip.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledBankByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT * FROM `banks` WHERE `id`=\"" + id + "\";"))
	{
		string	bank_title = db->Get(0, "title");
		string	bank_bik = db->Get(0, "bik");
		string	bank_geo_zip_id = db->Get(0, "geo_zip_id");
		string	bank_address = db->Get(0, "address");

		result = bank_title + " (БИК: " + bank_bik + ") " + GetSpelledZIPByID(bank_geo_zip_id, db) + ", " + bank_address;
	}
	else
	{
		MESSAGE_ERROR("", "", "bank_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	stod_noexcept(const string &num) -> double
{
	auto	result = 0.0;
	auto	decimal_point = (localeconv()->decimal_point ? localeconv()->decimal_point[0] : ',');
	auto	lc_original = ""s;
	auto	lc_en = "en_US.utf8"s;
	auto	space_pos = string::npos;	// --- type casting

	if(decimal_point != '.')
	{
		lc_original = setlocale(LC_NUMERIC, NULL);
		setlocale(LC_NUMERIC, lc_en.c_str());
		// --- commented due to performance
		// MESSAGE_DEBUG("", "", "temporary switch locale from " + lc_original + " to " + lc_en)
	}

	try
	{
		if((space_pos = num.find(' ')) != string::npos)
		{
			auto	temp = num;
			// --- remove any spaces
			do
			{
				temp.erase(space_pos, 1);
			} while((space_pos = temp.find(' ', space_pos)) != string::npos);

			result = stod(temp);
		}
		else
		{
			result = stod(num);
		}
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "can't convert " + num + " to double");
	}

	if(decimal_point != '.')
	{
		setlocale(LC_NUMERIC, lc_original.c_str());
		// --- commented due to performance
		// MESSAGE_DEBUG("", "", "switch locale back to " + lc_original)
	}

	return result;
}

auto	MaskSymbols(string src, int first_pos, int last_pos) -> string
{
	MESSAGE_DEBUG("", "", "start");

	for(auto i = first_pos; i < last_pos; ++i)
		src[i] = '.';

	MESSAGE_DEBUG("", "", "finish");

	return src;
}

auto CutTrailingZeroes(string number) -> string
{
    size_t   dec_point_place = number.find(".");
    
    if(dec_point_place != string::npos)
    {
        bool   domore = true;
        int    symbols_to_cut = 0;

        for(string::reverse_iterator rit = number.rbegin(); domore && (rit != number.rend()); ++rit)
        {
            if(*rit == '0') ++symbols_to_cut;
            else if(*rit == '.') { ++symbols_to_cut; domore = false;}
            else domore = false;
        }

        number.erase(number.length() - symbols_to_cut);
    }
    
    return number;
}

auto      		GetUserBonusesAirlinesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	user_id;
		string	program_id;
		string	number;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;
	int						affected;
	auto					result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			affected = db->Query(sqlQuery);
			if(affected)
			{
				for(int i = 0; i < affected; i++)
				{
					ItemClass	item;

					item.id							= db->Get(i, "id");
					item.user_id		 			= db->Get(i, "user_id");
					item.program_id					= db->Get(i, "airline_id");
					item.number						= db->Get(i, "number");
					item.eventTimestamp				= db->Get(i, "eventTimestamp");

					itemsList.push_back(item);
				}

				for (const auto& item : itemsList)
				{
					if(result.length()) result += ",";
					result +=	"{";

					result += "\"id\":\""						+ item.id + "\",";
					result += "\"user_id\":\""					+ item.user_id + "\",";
					result += "\"program_id\":\""				+ item.program_id + "\",";
					result += "\"number\":\""					+ item.number + "\",";
					result += "\"programs\":["					+ GetBonuseProgramsInJSONFormat("SELECT * FROM `airlines` WHERE `id`=\"" + item.program_id + "\";", db, user) + "],";
					result += "\"eventTimestamp\":\""			+ item.eventTimestamp + "\"";

					result +=	"}";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") doesn't participate in airline bonus programs");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (result length = " + to_string(result.length()) + ")");

	return result;
}

auto GetUserBonusesRailroadsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	user_id;
		string	program_id;
		string	number;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;
	int						affected;
	auto					result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			affected = db->Query(sqlQuery);
			if(affected)
			{
				for(int i = 0; i < affected; i++)
				{
					ItemClass	item;

					item.id							= db->Get(i, "id");
					item.user_id		 			= db->Get(i, "user_id");
					item.program_id					= db->Get(i, "railroad_id");
					item.number						= db->Get(i, "number");
					item.eventTimestamp				= db->Get(i, "eventTimestamp");

					itemsList.push_back(item);
				}

				for (const auto& item : itemsList)
				{
					if(result.length()) result += ",";
					result +=	"{";

					result += "\"id\":\""						+ item.id + "\",";
					result += "\"user_id\":\""					+ item.user_id + "\",";
					result += "\"program_id\":\""				+ item.program_id + "\",";
					result += "\"number\":\""					+ item.number + "\",";
					result += "\"programs\":["					+ GetBonuseProgramsInJSONFormat("SELECT * FROM `railroads` WHERE `id`=\"" + item.program_id + "\";", db, user) + "],";
					result += "\"eventTimestamp\":\""			+ item.eventTimestamp + "\"";

					result +=	"}";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") doesn't participate in railroads bonus programs");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (result length = " + to_string(result.length()) + ")");

	return result;
}

auto GetUserBonusesHotelchainsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	user_id;
		string	program_id;
		string	number;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;
	int						affected;
	auto					result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			affected = db->Query(sqlQuery);
			if(affected)
			{
				for(int i = 0; i < affected; i++)
				{
					ItemClass	item;

					item.id							= db->Get(i, "id");
					item.user_id		 			= db->Get(i, "user_id");
					item.program_id					= db->Get(i, "hotel_chain_id");
					item.number						= db->Get(i, "number");
					item.eventTimestamp				= db->Get(i, "eventTimestamp");

					itemsList.push_back(item);
				}

				for (const auto& item : itemsList)
				{
					if(result.length()) result += ",";
					result +=	"{";

					result += "\"id\":\""						+ item.id + "\",";
					result += "\"user_id\":\""					+ item.user_id + "\",";
					result += "\"program_id\":\""				+ item.program_id + "\",";
					result += "\"number\":\""					+ item.number + "\",";
					result += "\"programs\":["					+ GetBonuseProgramsInJSONFormat("SELECT * FROM `hotel_chains` WHERE `id`=\"" + item.program_id + "\";", db, user) + "],";
					result += "\"eventTimestamp\":\""			+ item.eventTimestamp + "\"";

					result +=	"}";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") doesn't participate in hotel chain bonus programs");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (result length = " + to_string(result.length()) + ")");

	return result;
}

auto  GetBonuseProgramsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	code;
		string	description_rus;
		string	description_eng;
		string	country;
	};
	vector<ItemClass>		itemsList;
	int						affected;
	auto					result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			affected = db->Query(sqlQuery);
			if(affected)
			{
				for(int i = 0; i < affected; i++)
				{
					ItemClass	item;

					item.id							= db->Get(i, "id");
					item.code		 				= db->Get(i, "code");
					item.description_rus			= db->Get(i, "description_rus");
					item.description_eng			= db->Get(i, "description_eng");
					item.country					= db->Get(i, "country");

					itemsList.push_back(item);
				}

				for (const auto& item : itemsList)
				{
					if(result.length()) result += ",";
					result +=	"{";

					result += "\"id\":\""				+ item.id + "\",";
					result += "\"code\":\""				+ item.code + "\",";
					result += "\"description_rus\":\""	+ item.description_rus + "\",";
					result += "\"description_eng\":\""	+ item.description_eng + "\",";
					result += "\"country\":\""			+ item.country + "\"";

					result +=	"}";
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "bonus programs not found");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (result length = " + to_string(result.length()) + ")");

	return result;
}

auto DateInPast(string date_to_check) -> bool
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = false;
	auto	date_to_check_obj = GetTMObject(date_to_check);

	time_t	rawtime;
	struct	tm *timeinfo_temp, timeinfo_now;

	time(&rawtime);
	timeinfo_temp = localtime (&rawtime);
	timeinfo_now = *timeinfo_temp;
	
	timeinfo_now.tm_sec	= 0;   // seconds of minutes from 0 to 61
	timeinfo_now.tm_min	= 0;   // minutes of hour from 0 to 59
	timeinfo_now.tm_hour = 0;  // hours of day from 0 to 24

	if(date_to_check_obj < timeinfo_now) result = true;

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto	GetHelpDeskTicketsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	customer_user_id;
		string	severity;
		string	title;
	};
	vector<ItemClass>		itemsList;


	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.customer_user_id = db->Get(i, "customer_user_id");
			item.title = db->Get(i, "title");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"customer_user_id\":\"" + item.customer_user_id + "\",";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=" + quoted(item.customer_user_id) + ";", db, user) + "],";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"history\":[" + GetHelpDeskTicketHistoryInJSONFormat("SELECT * FROM `helpdesk_ticket_history` WHERE `helpdesk_ticket_id`=\"" + item.id + "\";", db, user) + "]";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no ticket found");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetHelpDeskTicketHistoryInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	customer_user_id;
		string	user_id;
		string	state;
		string	severity;
		string	description;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;


	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.user_id = db->Get(i, "user_id");
			item.state = db->Get(i, "state");
			item.severity = db->Get(i, "severity");
			item.description = db->Get(i, "description");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=" + quoted(item.user_id) + ";", db, user) + "],";
			result += "\"files\":[" + GetHelpDeskTicketAttachInJSONFormat("SELECT * FROM `helpdesk_ticket_attaches` WHERE `helpdesk_ticket_history_id`=" + quoted(item.id) + ";", db, user) + "],";
			result += "\"state\":\"" + item.state + "\",";
			result += "\"severity\":\"" + item.severity + "\",";
			result += "\"description\":\"" + item.description + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "no history found for helpdesk_ticket (" + sqlQuery + ")");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetHelpDeskTicketAttachInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	helpdesk_ticket_history_id;
		string	filename;
	};
	vector<ItemClass>		itemsList;


	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.helpdesk_ticket_history_id = db->Get(i, "helpdesk_ticket_history_id");
			item.filename = db->Get(i, "filename");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"helpdesk_ticket_history_id\":\"" + item.helpdesk_ticket_history_id + "\",";
			result += "\"filename\":\"" + item.filename + "\"";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no attaches to the helpdesk_ticket_history");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto CleanupFilename(string filename) -> string
{
	MESSAGE_DEBUG("", "", "start (" + filename + ")");

	auto found = filename.find_first_not_of("."); // --- type derivation;

	while((found = filename.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_-().")) != string::npos)
	{
		filename.replace(found, 1, "_");
	}

	MESSAGE_DEBUG("", "", "finish (" + filename + ")");

	return filename;
}

auto	isHelpdeskTicketOwner(string ticket_id, string user_id, CMysql *db, CUser *user) -> bool
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = false;

	if(db->Query("SELECT `id` FROM `helpdesk_tickets` WHERE `id`=" + quoted(ticket_id) + " AND `customer_user_id`=" + quoted(user_id) + ";"))
	{
		result = true;
	}
	else
	{
		MESSAGE_DEBUG("", "", "helpdesk_ticket.id(" + ticket_id + ") not owned by user.id(" + user_id + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

auto isUserAllowedToChangeTicket(string ticket_id, string user_id, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start (ticket_id:" + ticket_id + ", user_id:" + user_id + ")");

	auto	result	 = ""s;

	if((db->Query("SELECT `id` FROM `users` WHERE "
					"(`id`=" + quoted(user_id) + " AND `type`=" + quoted(HELPDESK_USER_ROLE) + ") "
					"OR "
					"(" + quoted(user_id) + "=(SELECT `customer_user_id` FROM `helpdesk_tickets` WHERE `id`=" + quoted(ticket_id) + ")) "
					"LIMIT 0,1;"))
		)
	{
	}
	else
	{
		result = gettext("you are not authorized");
		MESSAGE_DEBUG("", "", "user (" + user_id + ") not allowed to change ticket")
	}

	MESSAGE_DEBUG("", "", "finish (" + result + ")");

	return result;
}

auto	GetFAQInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	title;
		string	description;
	};
	vector<ItemClass>		itemsList;


	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"description\":\"" + item.description + "\"";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no attaches to the helpdesk_ticket_history");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}


auto	GetValueFromDB(string sql, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = ""s;

	if(db->Query(sql)) result = db->Get(0, 0);

	MESSAGE_DEBUG("", "", "finish");

	return result;
}