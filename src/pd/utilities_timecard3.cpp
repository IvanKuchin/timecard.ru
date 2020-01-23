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


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	result;
}

auto	GetTimecard_ApprovalChain(string timecard_id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = ""s;
	auto	error_message = ""s;


	if(timecard_id.length())
	{
		result = ApprovalChain(	"SELECT `name`, `nameLast`, `nameMiddle` FROM `users` WHERE `id` IN ("
									"SELECT DISTINCT(`approver_user_id`) FROM `timecard_approvers` WHERE `id` IN ("
										"SELECT `approver_id` FROM `timecard_approvals` WHERE `timecard_id`=" + quoted(timecard_id) + " AND `decision`=\"approved\""
									")"
								");", db);
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

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


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

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

auto	GetTimecardList(string where_companies_list, CMysql *db, CUser *user) -> string
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

// --- stub function: deny all 
// --- if some users need to be allowed to create BIK, algorithm must be written here
auto isUserAllowedToCreateBIK(CUser *user, CMysql *sql) -> string 
{
	return gettext("you are not authorized to create new BIK");
}

