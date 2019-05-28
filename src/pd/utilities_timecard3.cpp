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
