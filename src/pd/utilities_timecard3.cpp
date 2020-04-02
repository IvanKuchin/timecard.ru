#include "utilities_timecard.h"

string	GetAbsenceListInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	company_id;
		string	absence_type_id;
		string	start_date;
		string	end_date;
		string	comment;
		string	request_date;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.company_id = db->Get(i, "company_id");
			item.absence_type_id = db->Get(i, "absence_type_id");
			item.start_date = db->Get(i, "start_date");
			item.end_date = db->Get(i, "end_date");
			item.comment = db->Get(i, "comment");
			item.request_date = db->Get(i, "request_date");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"company_id\":\"" + item.company_id + "\",";
			result += "\"absence_type_id\":\"" + item.absence_type_id + "\",";
			result += "\"companies\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + item.company_id + "\";", db, user) + "],";
			result += "\"absence_types\":[" + GetAbsenceTypesInJSONFormat("SELECT * FROM `absence_types` WHERE `id`=\"" + item.absence_type_id + "\";", db, user) + "],";
			result += "\"start_date\":\"" + item.start_date + "\",";
			result += "\"end_date\":\"" + item.end_date + "\",";
			result += "\"request_date\":\"" + item.request_date + "\",";
			result += "\"comment\":\"" + item.comment + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "absence list is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string	GetAbsenceTypesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	title;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "absence type not found");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto GetAbsenceOverlap(string company_id, string start_date, string end_date, CMysql *db, CUser *user, string exclude_id/* = "" */) -> string
{
	MESSAGE_DEBUG("", "", "start (company_id " + company_id + " (" + start_date + " - " + end_date + "))");

	auto	result			= ""s;
	auto	start_date_db	= "STR_TO_DATE(\"" + start_date + "\", '%d/%m/%Y')";
	auto	end_date_db		= "STR_TO_DATE(\"" + end_date + "\", '%d/%m/%Y')";

	if(db && user)
	{
		auto	sql_query = "SELECT * FROM `absence` WHERE `company_id`=\"" + company_id + "\" AND "
															"("
																"("
																	"(`start_date` >= " + start_date_db + ")"
																	" AND "
																	"(`start_date` <= " + end_date_db + ")"
																")"
																" OR "
																"("
																	"(`end_date` >= " + start_date_db + ")"
																	" AND "
																	"(`end_date` <= " + end_date_db + ")"
																")"
																" OR "
																"("
																	"(`start_date` <= " + start_date_db + ")"
																	" AND "
																	"(`end_date` >= " + start_date_db + ")"
																")"
																" OR "
																"("
																	"(`start_date` <= " + end_date_db + ")"
																	" AND "
																	"(`end_date` >= " + end_date_db + ")"
																")"
															")";
		if(exclude_id.length()) sql_query += " AND (`id` != " + quoted(exclude_id) + ")";

		if(db->Query(sql_query))
		{
			result = db->Get(0, "start_date") + " - " + db->Get(0, "end_date");
		}
		else
		{
			MESSAGE_DEBUG("", "", "no overlaps");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;
}

auto	GetAirfareLimitaionsByDirectionInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	from;
		string	to;
		string	limit;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.from = db->Get(i, "from");
			item.to = db->Get(i, "to");
			item.limit = db->Get(i, "limit");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"from\":[" + GetAirportCountryInJSONFormat("SELECT * FROM `airport_countries` WHERE `id`=\"" + item.from + "\";", db, user) + "],";
			result += "\"to\":[" + GetAirportCountryInJSONFormat("SELECT * FROM `airport_countries` WHERE `id`=\"" + item.to + "\";", db, user) + "],";
			result += "\"limit\":\"" + item.limit + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "there are no fare limitaions by direction in DB");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetAirportCountryInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{

	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	title;
		string	abbrev;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.abbrev = db->Get(i, "abbrev");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"abbrev\":\"" + item.abbrev + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "DB returned emtpy response");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}


auto isValidToAddAirfareLimitByDirection(string from_id, string to_id, string agency_id, CMysql *db, CUser *user) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `id` FROM `airfare_limits_by_direction` WHERE "
						"(`from`=" + quoted(from_id) + " AND `to`=" + quoted(to_id) + " AND `agency_company_id`=" + quoted(agency_id) + ") "
						"OR "
						"(`to`=" + quoted(from_id) + " AND `from`=" + quoted(to_id) + " AND `agency_company_id`=" + quoted(agency_id) + ") "
						 ";"))
	{
		error_message = gettext("this limit already exists");
		MESSAGE_DEBUG("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	GetAirlineBookingsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	contract_sow_id;
		string	checkin;
		string	checkout;
		string	trip_id;
		string	destination;
		string	passport_type;
		string	status;
		string	amount;
		string	book_date;
		string	voucher_filename;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.checkin = db->Get(i, "checkin");
			item.checkout = db->Get(i, "checkout");
			item.trip_id = db->Get(i, "trip_id");
			item.destination = db->Get(i, "destination");
			item.passport_type = db->Get(i, "passport_type");
			item.status = db->Get(i, "status");
			item.amount = db->Get(i, "amount");
			item.book_date = db->Get(i, "book_date");
			item.voucher_filename = db->Get(i, "voucher_filename");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"checkin\":\"" + item.checkin + "\",";
			result += "\"checkout\":\"" + item.checkout + "\",";
			result += "\"trip_id\":\"" + item.trip_id + "\",";
			result += "\"destination\":\"" + item.destination + "\",";
			result += "\"passport_type\":\"" + item.passport_type + "\",";
			result += "\"status\":\"" + item.status + "\",";
			result += "\"amount\":\"" + item.amount + "\",";
			result += "\"book_date\":\"" + item.book_date + "\",";
			result += "\"voucher_filename\":\"" + item.voucher_filename + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetBTAllowanceInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	agency_company_id;
		string	geo_country_id;
		string	amount;
	};
	vector<ItemClass>		itemsList;


	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.geo_country_id = db->Get(i, "geo_country_id");
			item.amount = db->Get(i, "amount");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"agency_company_id\":\"" + item.agency_company_id + "\",";
			result += "\"countries\":[" + GetCountryListInJSONFormat("SELECT  * FROM `geo_country` WHERE `id`=" + quoted(item.geo_country_id) + ";", db, user) + "],";
			result += "\"amount\":\"" + item.amount + "\"";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "bt_allowance is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	amIonTheApproverListForSoW(const string &db_table, const string &sow_id, CMysql *db, CUser *user) -> bool
{
	return db->Query("SELECT `id` FROM `" + db_table + "` WHERE `contract_sow_id`=" + quoted(sow_id) + " AND `approver_user_id`=" + quoted(user->GetID()) + ";");
}

auto	GetHolidayCalendarInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	agency_company_id;
		string	date;
		string	type;
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
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.date = db->Get(i, "date");
			item.type = db->Get(i, "type");
			item.title = db->Get(i, "title");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"agency_company_id\":\"" + item.agency_company_id + "\",";
			result += "\"date\":\"" + item.date + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"title\":\"" + item.title + "\"";
			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "holiday_calendar is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}



auto	ApprovalChain(string sql, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	result = ""s;


	if(db)
	{
		auto	affected = db->Query(sql);

		for(auto i = 0; i < affected; ++i)
		{
			if(result.length()) result += ", ";

			result += db->Get(i, "nameLast") + " " + db->Get(i, "name");
			if(db->Get(i, "nameMiddle").length()) result += " " + db->Get(i, "nameMiddle");
		}

	}
	else
	{
		error_message = gettext("db is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message+ ")");

	return	result;
}

auto	GetTimecard_ApprovalChain(const string &timecard_id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start (timecard_id " + timecard_id + ")");

	auto	result = ""s;
	auto	error_message = ""s;


	if(timecard_id.length())
	{
/*
		result = ApprovalChain(	"SELECT `name`, `nameLast`, `nameMiddle` FROM `users` WHERE `id` IN ("
									"SELECT DISTINCT(`approver_user_id`) FROM `timecard_approvers` WHERE `id` IN ("
										"SELECT `approver_id` FROM `timecard_approvals` WHERE `timecard_id`=" + quoted(timecard_id) + " AND `decision`=\"approved\""
									")"
								");", db);
*/
		result = ApprovalChain(	"SELECT `name`, `nameLast`, `nameMiddle` FROM `users` WHERE `id` IN ("
									"SELECT DISTINCT(`approver_user_id`) FROM `timecard_approvers` WHERE `id` IN ("
										+ Get_ApproverIDsByTimecardID_sqlquery(timecard_id) +
									")"
								");", db);
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message+ ")");

	return	result;
}

auto	GetTimecard_ApprovalChain(const string &timecard_id, string psow_id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start (timecard_id " + timecard_id + ", psow_id " + psow_id + ")");

	auto	result = ""s;
	auto	error_message = ""s;


	if(timecard_id.length())
	{
		result = ApprovalChain(	"SELECT `name`, `nameLast`, `nameMiddle` FROM `users` WHERE `id` IN ("
									"SELECT DISTINCT(`approver_user_id`) FROM `timecard_approvers` WHERE `id` IN ("
										+ Get_ApproverIDsByTimecardID_sqlquery(timecard_id)
										+ (psow_id.length() ? " AND `contract_psow_id` IN (" + psow_id + ")" : "") +
									")"
								");", db);
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message+ ")");

	return	result;
}

auto	GetBT_ApprovalChain(string bt_id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	result = ""s;


	if(bt_id.length())
	{
		result = ApprovalChain(	"SELECT `name`, `nameLast`, `nameMiddle` FROM `users` WHERE `id` IN ("
									"SELECT DISTINCT(`approver_user_id`) FROM `bt_approvers` WHERE `id` IN ("
										"SELECT `approver_id` FROM `bt_approvals` WHERE `bt_id`=" + quoted(bt_id) + " AND `decision`=\"approved\""
									")"
								");", db);
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message+ ")");

	return	result;
}

auto GetHolidaysSet(string day_around, CMysql *db) -> unordered_set<string>
{
	MESSAGE_DEBUG("", "", "start");

	unordered_set<string>	result;
	auto					affected = db->Query("SELECT `date` FROM `holiday_calendar` WHERE DATE_SUB(" + quoted(day_around) + ", INTERVAL " + to_string(HOLIDAY_RANGE_FROM_TODAY) + " DAY)<=`date` AND `date`<=DATE_ADD(" + quoted(day_around) + ", INTERVAL " + to_string(HOLIDAY_RANGE_FROM_TODAY) + " DAY);");

	for(auto i = 0; i < affected; ++i)
	{
		result.emplace(db->Get(i, 0));
	}

	MESSAGE_DEBUG("", "", "finish (number of hoildays = " + to_string(result.size()) + ")");

	return result;
}

auto Update_TimecardBT_ExpectedPayDate(string entity, string id, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start(" + entity + ", " + id + ")");

	auto	error_message = ""s;

	if(entity.length())
	{
		auto	sql_query = ""s;

		if(entity == "timecard")
		{
			sql_query = "UPDATE `timecards` SET `expected_pay_date`=";
			if(db->Query("SELECT `contract_sow_id` FROM `timecards` WHERE `id`=" + quoted(id) + " AND `originals_received_date`>\"0\";"))
				sql_query += "`originals_received_date` + 3600 * 24 * (SELECT `payment_period_service` FROM `contracts_sow` WHERE `id`=" + quoted(db->Get(0, "contract_sow_id")) + ")";
			else
				sql_query += quoted("0"s);
			sql_query += " WHERE `id`=" + quoted(id) + ";";
		}
		else if(entity == "bt")
		{
			sql_query = "UPDATE `bt` SET `expected_pay_date`=";
			if(db->Query("SELECT `contract_sow_id` FROM `bt` WHERE `id`=" + quoted(id) + " AND `originals_received_date`>\"0\";"))
				sql_query += "`originals_received_date` + 3600 * 24 * (SELECT `payment_period_bt` FROM `contracts_sow` WHERE `id`=" + quoted(db->Get(0, "contract_sow_id")) + ")";
			else
				sql_query += quoted("0"s);
			sql_query += " WHERE `id`=" + quoted(id) + ";";
		}
		else
		{
			error_message = gettext("entity type is unknown");
			MESSAGE_ERROR("", "", error_message);
		}

		db->Query(sql_query);
		if(db->isError())
		{
			error_message = gettext("SQL syntax error");
			MESSAGE_ERROR("", "", error_message);
		}
		else
		{
		}

	}
	else
	{
		error_message = gettext("entity type is unknown");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	GetDashboardPaymentData(string sow_sql, CMysql *db, CUser *user) -> string
{
	return 
		"\"number_of_payment_pending_timecards\":"	+ GetNumberOfTimecardsInPaymentPendingState(sow_sql, db, user) + ","
		"\"timecard_late_payment\":["				+ join(GetTimecardsWithExpiredPayment("0", sow_sql, db, user)) + "],"
		"\"timecard_payment_will_be_late_soon\":["	+ join(GetTimecardsWithExpiredPayment("1/2", sow_sql, db, user)) + "],"
		"\"number_of_payment_pending_bt\":"			+ GetNumberOfBTInPaymentPendingState(sow_sql, db, user) + ","
		"\"bt_late_payment\":["						+ join(GetBTWithExpiredPayment("0", sow_sql, db, user)) + "],"
		"\"bt_payment_will_be_late_soon\":["		+ join(GetBTWithExpiredPayment("1/2", sow_sql, db, user)) + "]"
		;
}

auto	GetTimecardList(const string &where_companies_list, CMysql *db, CUser *user) -> string
{
	return
		"\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE " + where_companies_list + ";", db, user, false, false, false, true) + "],"
		"\"timecards\":[" + GetTimecardsInJSONFormat(
				"SELECT * FROM `timecards` WHERE "
					"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE " + where_companies_list + ")"
				";", db, user) + "],"
		"\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat("SELECT * FROM `holiday_calendar` WHERE `agency_company_id` IN (SELECT `agency_company_id` FROM `contracts_sow` WHERE " + where_companies_list + ");", db, user) + "]"
		;
}

// --- date is suppose to be the first day of period (month or week)
auto	GetTimecardList(const string &where_companies_list, const string &date, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start(" + where_companies_list + ", " + date + ")");

	auto		filter = Get_SoWDateFilter_sqlquery(PrintSQLDate(GetTMObject(date)));
	struct tm	period_start, period_end;
	tie(period_start, period_end) = GetFirstAndLastMonthDaysByDate(GetTMObject(date));

	auto	result = 
		"\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE " + where_companies_list + " AND " + filter + ";", db, user, false, false, false, true) + "],"
		"\"timecards\":[" + GetTimecardsInJSONFormat(
				"SELECT * FROM `timecards` WHERE "
					"`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE " + where_companies_list + " AND " + filter + ") "
					"AND "
					"(\"" + PrintSQLDate(period_start) + "\" <= `timecards`.`period_end`) "
					"AND "
					"(`timecards`.`period_end` <= \"" + PrintSQLDate(period_end) + "\") "
				";", db, user) + "],"
		"\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat("SELECT * FROM `holiday_calendar` WHERE `agency_company_id` IN (SELECT `agency_company_id` FROM `contracts_sow` WHERE " + where_companies_list + ");", db, user) + "]"
		;

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

// --- stub function: deny all 
// --- if some users need to be allowed to create BIK, algorithm must be written here
auto isUserAllowedToCreateBIK(CUser *user, CMysql *db) -> string 
{
	return gettext("you are not authorized to create new BIK");
}

auto isTimePeriodInsideSow(string sow_id, string period_start, string period_end, CMysql *db, CUser *user) -> string 
{
	MESSAGE_DEBUG("", "", "start(" + sow_id + ", " + period_start + ", " + period_end + ")");

	auto	error_message = ""s;

	if(sow_id.length() && period_start.length() && period_end.length() && db)
	{
		if(db->Query("SELECT `end_date` FROM `contracts_sow` WHERE `id`=" + quoted(sow_id) + " AND `end_date`<" + quoted(period_start) + ";"))
		{
			error_message = gettext("SoW expired prior to reporting period") + " ("s + db->Get(0, 0) + ")";
			MESSAGE_ERROR("", "", error_message);
		}
		else
		{
			if(db->Query("SELECT `start_date` FROM `contracts_sow` WHERE `id`=" + quoted(sow_id) + " AND " + quoted(period_end) + "<`start_date`;"))
			{
				error_message = gettext("SoW has not been activated yet") + " ("s + db->Get(0, 0) + ")";
				MESSAGE_ERROR("", "", error_message);
			}
			else
			{
				// --- good2go	
			}
		}
	}
	else
	{
		error_message = gettext("parameters incorrect");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}


auto GetUserBonusesAirlinesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
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


// --- Returns user list in JSON format grabbed from DB
// --- Input: dbQuery - SQL format returns users
//			db	  - DB connection
//			user	- current user object
string GetUserListInJSONFormat(string dbQuery, CMysql *db, CUser *user)
{
	ostringstream					 ost, ost1;
	string							 sessid, lookForKey;
	unordered_set<unsigned long>	 setOfUserID;

	struct	ItemClass
	{
		string	userID;
		string	userLogin;
		string	userName;
		string	userNameLast;
		string	userNameMiddle;
		string	first_name_en;
		string	last_name_en;
		string	middle_name_en;
		string	country_code;
		string	phone;
		string	email;
		string	email_changeable;
		string	userType;
		string	userSex;
		string	userBirthday;
		string	userBirthdayAccess;
		string	userAppliedVacanciesRender;
		string	userCurrentCityID;
		string	passport_series;
		string	passport_number;
		string	passport_issue_date;
		string	passport_issue_authority;
		string	foreign_passport_number;
		string	foreign_passport_expiration_date;
		string	citizenship_code;
		string	site_theme_id;
		string	userLastOnline;
		string	userLastOnlineSecondSinceY2k;
		string	helpdesk_subscription_S1_sms;
		string	helpdesk_subscription_S2_sms;
		string	helpdesk_subscription_S3_sms;
		string	helpdesk_subscription_S4_sms;
		string	helpdesk_subscription_S1_email;
		string	helpdesk_subscription_S2_email;
		string	helpdesk_subscription_S3_email;
		string	helpdesk_subscription_S4_email;
		string	pending_approval_notification_timecard;
		string	pending_approval_notification_bt;
	};
	vector<ItemClass>		itemsList;
	int						itemsCount;


	MESSAGE_DEBUG("", "", "start");

	if((itemsCount = db->Query(dbQuery)) > 0)
	{
		for(int i = 0; i < itemsCount; ++i)
		{
			ItemClass	item;
			item.userID								= db->Get(i, "id");
			item.userLogin							= db->Get(i, "login");
			item.userName							= db->Get(i, "name");
			item.userNameLast						= db->Get(i, "nameLast");
			item.userNameMiddle						= db->Get(i, "nameMiddle");
			item.country_code						= db->Get(i, "country_code");
			item.phone								= db->Get(i, "phone");
			item.email								= db->Get(i, "email");
			item.email_changeable					= db->Get(i, "email_changeable");
			item.userSex							= db->Get(i, "sex");
			item.userType							= db->Get(i, "type");
			item.userBirthday						= db->Get(i, "birthday");
			item.userBirthdayAccess					= db->Get(i, "birthdayAccess");
			item.userAppliedVacanciesRender			= db->Get(i, "appliedVacanciesRender");
			item.userCurrentCityID					= db->Get(i, "geo_locality_id");
			item.site_theme_id						= db->Get(i, "site_theme_id");
			item.passport_series					= db->Get(i, "passport_series");
			item.passport_number					= db->Get(i, "passport_number");
			item.passport_issue_date				= db->Get(i, "passport_issue_date");
			item.passport_issue_authority			= db->Get(i, "passport_issue_authority");
			item.citizenship_code					= db->Get(i, "citizenship_code");
			item.first_name_en						= db->Get(i, "first_name_en");
			item.last_name_en						= db->Get(i, "last_name_en");
			item.middle_name_en						= db->Get(i, "middle_name_en");
			item.foreign_passport_number			= db->Get(i, "foreign_passport_number");
			item.foreign_passport_expiration_date	= db->Get(i, "foreign_passport_expiration_date");
			item.userLastOnline						= db->Get(i, "last_online");
			item.userLastOnlineSecondSinceY2k		= db->Get(i, "last_onlineSecondsSinceY2k");
			item.helpdesk_subscription_S1_email		= db->Get(i, "helpdesk_subscription_S1_email");
			item.helpdesk_subscription_S2_email		= db->Get(i, "helpdesk_subscription_S2_email");
			item.helpdesk_subscription_S3_email		= db->Get(i, "helpdesk_subscription_S3_email");
			item.helpdesk_subscription_S4_email		= db->Get(i, "helpdesk_subscription_S4_email");
			item.helpdesk_subscription_S1_sms		= db->Get(i, "helpdesk_subscription_S1_sms");
			item.helpdesk_subscription_S2_sms		= db->Get(i, "helpdesk_subscription_S2_sms");
			item.helpdesk_subscription_S3_sms		= db->Get(i, "helpdesk_subscription_S3_sms");
			item.helpdesk_subscription_S4_sms		= db->Get(i, "helpdesk_subscription_S4_sms");
			item.pending_approval_notification_timecard	= db->Get(i, "pending_approval_notification_timecard");
			item.pending_approval_notification_bt	= db->Get(i, "pending_approval_notification_bt");

			itemsList.push_back(item);
		}


		ost.str("");
		for(int i = 0; i < itemsCount; i++)
		{
			// --- if user_list have duplicates(1, 2, 1), avoid duplications
			if(setOfUserID.find(stol(itemsList[i].userID)) == setOfUserID.end())
			{
				auto				userID					= itemsList[i].userID;
				auto				userBirthday			= itemsList[i].userBirthday;
				auto				userCurrentCityID		= itemsList[i].userCurrentCityID;
				auto				userCurrentCity			= ""s;
				auto				avatarPath				= ""s;
				auto				userLastOnline			= itemsList[i].userLastOnline;
				auto				userLastOnlineSecondSinceY2k = itemsList[i].userLastOnlineSecondSinceY2k;
				auto				isMe					= user && (userID == user->GetID());
				ostringstream		ost1;

				setOfUserID.insert(stol(userID));

				// --- Get user avatars
				avatarPath = "empty";
				if(db->Query("select * from `users_avatars` where `userid`='" + userID + "' and `isActive`='1';"))
				{
					avatarPath = "/images/avatars/avatars" + db->Get(0, "folder") + "/" + db->Get(0, "filename");
				}

				if(userCurrentCityID.length() && db->Query("SELECT `title` FROM `geo_locality` WHERE `id`=\"" + userCurrentCityID + "\";"))
					userCurrentCity = db->Get(0, "title");

				if(ost.str().length()) ost << ", ";

				if(isMe)
				{
					// --- user have to be able to see his own bday
				}
				else
				{
					if(itemsList[i].userBirthdayAccess == "private") userBirthday = "";
				}

				ost << "{"
						"\"id\": \""								<< itemsList[i].userID << "\", "
						"\"name\": \""								<< itemsList[i].userName << "\", "
						"\"nameLast\": \""							<< itemsList[i].userNameLast << "\","
						"\"nameMiddle\": \""						<< itemsList[i].userNameMiddle << "\","
						"\"userSex\": \""							<< itemsList[i].userSex << "\","
						"\"userType\": \""							<< itemsList[i].userType << "\","
						"\"birthday\": \""							<< userBirthday << "\","
						"\"birthdayAccess\": \""					<< itemsList[i].userBirthdayAccess << "\","
						"\"last_online\": \""						<< itemsList[i].userLastOnline << "\","
						"\"last_online_diff\": \""					<< to_string(GetTimeDifferenceFromNow(userLastOnline)) << "\","
						"\"last_onlineSecondsSinceY2k\": \""		<< userLastOnlineSecondSinceY2k << "\","
						"\"avatar\": \""							<< avatarPath << "\","
						"\"site_theme_id\": \""						<< itemsList[i].site_theme_id << "\","
						"\"themes\": ["								<< GetSiteThemesInJSONFormat("SELECT * FROM `site_themes`", db, user) << "],"
						// "\"numberUnreadMessages\": \""			<< numberUreadMessages << "\", "
						// "\"appliedVacanciesRender\": \""		<< userAppliedVacanciesRender << "\","
						// "\"userFriendship\": \""				<< userFriendship << "\","
						// "\"currentCity\": \""					<< userCurrentCity << "\", "
						// "\"currentEmployment\": "				<< userCurrentEmployment << ", "
						// "\"languages\": ["		 				<< GetLanguageListInJSONFormat("SELECT * FROM `language` WHERE `id` in (SELECT `language_id` FROM `users_language` WHERE `user_id`=\"" + userID + "\");", db) << "], "
						// "\"skills\": ["		 					<< GetSkillListInJSONFormat("SELECT * FROM `skill` WHERE `id` in (SELECT `skill_id` FROM `users_skill` WHERE `user_id`=\"" + userID + "\");", db) << "], "
						// "\"subscriptions\":[" 					<< (user && (user->GetID() == userID) ? GetUserSubscriptionsInJSONFormat("SELECT * FROM `users_subscriptions` WHERE `user_id`=\"" + userID + "\";", db) : "") << "],"
						"\"country_code\": \""						<< (isMe ? itemsList[i].country_code : "") << "\","
						"\"phone\": \""								<< (isMe ? itemsList[i].phone : "") << "\","
						"\"email\": \""								<< (isMe ? itemsList[i].email : "") << "\","
						"\"email_changeable\": \""					<< (isMe ? itemsList[i].email_changeable : "") << "\","
						"\"passport_series\": \""					<< (isMe ? itemsList[i].passport_series : "") << "\","
						"\"passport_number\": \""					<< (isMe ? itemsList[i].passport_number : "") << "\","
						"\"passport_issue_date\": \""				<< (isMe ? itemsList[i].passport_issue_date : "") << "\","
						"\"passport_issue_authority\": \""			<< (isMe ? itemsList[i].passport_issue_authority : "") << "\","
						"\"first_name_en\": \""						<< (isMe ? itemsList[i].first_name_en : "") << "\","
						"\"last_name_en\": \""						<< (isMe ? itemsList[i].last_name_en : "") << "\","
						"\"middle_name_en\": \""					<< (isMe ? itemsList[i].middle_name_en : "") << "\","
						"\"foreign_passport_number\": \""			<< (isMe ? itemsList[i].foreign_passport_number : "") << "\","
						"\"foreign_passport_expiration_date\": \""	<< (isMe ? itemsList[i].foreign_passport_expiration_date : "") << "\","
						"\"citizenship_code\": \""					<< (isMe ? itemsList[i].citizenship_code : "") << "\","
						"\"bonuses_airlines\": ["					<< (isMe ? GetUserBonusesAirlinesInJSONFormat("SELECT * FROM `user_bonuses_avia` WHERE `user_id`=\"" + itemsList[i].userID + "\";", db, user) : "") << "],"
						"\"bonuses_railroads\": ["					<< (isMe ? GetUserBonusesRailroadsInJSONFormat("SELECT * FROM `user_bonuses_railroads` WHERE `user_id`=\"" + itemsList[i].userID + "\";", db, user) : "") << "],"
						"\"bonuses_hotel_chains\": ["				<< (isMe ? GetUserBonusesHotelchainsInJSONFormat("SELECT * FROM `user_bonuses_hotels` WHERE `user_id`=\"" + itemsList[i].userID + "\";", db, user) : "") << "],"
						"\"helpdesk_subscriptions_sms\": ["			<< (isMe ? quoted(itemsList[i].helpdesk_subscription_S1_sms) + "," + quoted(itemsList[i].helpdesk_subscription_S2_sms) + "," + quoted(itemsList[i].helpdesk_subscription_S3_sms) + "," + quoted(itemsList[i].helpdesk_subscription_S4_sms)  : "") << "],"
						"\"helpdesk_subscriptions_email\": ["		<< (isMe ? quoted(itemsList[i].helpdesk_subscription_S1_email) + "," + quoted(itemsList[i].helpdesk_subscription_S2_email) + "," + quoted(itemsList[i].helpdesk_subscription_S3_email) + "," + quoted(itemsList[i].helpdesk_subscription_S4_email)  : "") << "],"
						"\"pending_approval_notification_timecard\": \"" << (isMe ? itemsList[i].pending_approval_notification_timecard : "") << "\","
						"\"pending_approval_notification_bt\": \""	<< (isMe ? itemsList[i].pending_approval_notification_bt : "") << "\","
						"\"isMe\": \""								<< (isMe ? "yes" : "no") << "\""
						"}";
			} // --- if user is not dupicated
		} // --- for loop through user list
	} // --- if sql-query on user selection success
	else
	{
		CLog	log;

		ost.str("");
		MESSAGE_DEBUG("", "", "there are users returned by request [" + dbQuery + "]");
	}

	{
		CLog	log;
		ostringstream	ostTemp;

		ostTemp.str("");
		ostTemp << "GetUserListInJSONFormat: end. returning string length " << ost.str().length();
		log.Write(DEBUG, ostTemp.str());
	}

	return ost.str();
}


// --- Function returns list of images belongs to imageSet
// --- input: imageSetID, db
// --- output: list of image objects
string GetMessageImageList(string imageSetID, CMysql *db)
{
	ostringstream	ost;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(imageSetID != "0")
	{
		int				affected;

		ost.str("");
		ost << "select * from `feed_images` where `setID`='" << imageSetID << "';";
		affected = db->Query(ost.str());
		if(affected > 0)
		{
			ost.str("");
			for(int i = 0; i < affected; i++)
			{
				if(i > 0) ost << "," << std::endl;
				ost << "{";
				ost << "\"id\":\"" << db->Get(i, "id") << "\",";
				ost << "\"folder\":\"" << db->Get(i, "folder") << "\",";
				ost << "\"filename\":\"" << db->Get(i, "filename") << "\",";
				ost << "\"mediaType\":\"" << db->Get(i, "mediaType") << "\",";
				ost << "\"isActive\":\"" << db->Get(i, "isActive") << "\"";
				ost << "}";
			}

			result = ost.str();
		}
	}

	{
		CLog			log;
		ostringstream	ost;

		ost.str();
		ost <<  "GetMessageImageList: end. returning string length " << result.length();
		log.Write(DEBUG, ost.str());
	}

	return result;
}

// --- Function returns list of users "liked" messageID in JSON-format
// --- input: messageID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetMessageLikesUsersList(string messageID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='like' and `messageID`='" + messageID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from users where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersBookID in JSON-format
// --- input: usersBookID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetBookLikesUsersList(string usersBookID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeBook' and `messageID`='" + usersBookID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersLanguageID in JSON-format
// --- input: usersLanguageID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetLanguageLikesUsersList(string usersLanguageID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeLanguage' and `messageID`='" + usersLanguageID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersCompanyID in JSON-format
// --- input: usersCompanyID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetCompanyLikesUsersList(string usersCompanyID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeCompany' and `messageID`='" + usersCompanyID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersCertificationID in JSON-format
// --- input: usersCertificationID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetCertificationLikesUsersList(string usersCertificationID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeCertification' and `messageID`='" + usersCertificationID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" usersCourseID in JSON-format
// --- input: usersCourseID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetCourseLikesUsersList(string usersCourseID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeCourse' and `messageID`='" + usersCourseID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" universityDegreeID in JSON-format
// --- input: universityDegreeID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetUniversityDegreeLikesUsersList(string universityDegreeID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `feed_message_params` where `parameter`='likeUniversityDegree' and `messageID`='" + universityDegreeID + "';");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from `users` where id in (";
		for(int i = 0; i < affected; i++)
		{
			if(i > 0) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}

// --- Function returns list of users "liked" bookID in JSON-format
// --- input: bookID, user, db
//			user - used for friendship status definition
// --- output: was this message "liked" by particular user or not
string GetBookRatingUsersList(string bookID, CUser *user, CMysql *db)
{
	ostringstream   ost;
	int			 affected;
	string		  result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query("select * from `users_books` where `bookID`=\"" + bookID + "\";");
	if(affected > 0)
	{
		ost.str("");
		ost << "select * from users where id in (";
		for(int i = 0; i < affected; ++i)
		{
			if(i) ost << ",";
			ost << db->Get(i, "userID");
		}
		ost << ") and `isactivated`='Y' and `isblocked`='N';";
		result = GetUserListInJSONFormat(ost.str(), db, user);
	}

	{
		CLog			log;
		MESSAGE_DEBUG("", "", "finish (returning string length " + to_string(result.length()) + ")");
	}

	return result;
}
