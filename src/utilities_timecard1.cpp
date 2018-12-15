#include "utilities.h"

// --- difference with "split" is that empty element treat as 0 instead of skip it
vector<string> SplitTimeentry(const string& s, const char c)
{

	string buff{""};
	vector<string> v;

	MESSAGE_DEBUG("", "", "start");

	for(auto n:s)
	{
		if(n == c)
		{
			c_float	num(buff);

			v.push_back(string(num));
			buff = "";
		}
		else
			buff+=n;
	}
	{
		c_float	num(buff);
		v.push_back(string(num));
	}

	MESSAGE_DEBUG("", "", "finish (result.size = " + to_string(v.size()) + ")");

	return v;
}

string	GetTimecardCustomersInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	title;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"title\":\"" + item.title + "\",";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_ERROR("", "", "customer not found (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardProjectsInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_customers_id;
		string	title;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_customers_id = db->Get(i, "timecard_customers_id");
			item.title = db->Get(i, "title");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"title\":\"" + item.title + "\",";
			result +=	"\"customers\":[" + GetTimecardCustomersInJSONFormat("SELECT * FROM `timecard_customers` WHERE `id`=\"" + item.timecard_customers_id + "\";", db, user) + "],";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_ERROR("", "", "project not found (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardTasksInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_projects_id;
		string	title;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_projects_id = db->Get(i, "timecard_projects_id");
			item.title = db->Get(i, "title");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"title\":\"" + item.title + "\",";
			result +=	"\"projects\":[" + GetTimecardProjectsInJSONFormat("SELECT * FROM `timecard_projects` WHERE `id`=\"" + item.timecard_projects_id + "\";", db, user) + "],";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no tasks returned by SQL-query(" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardTaskAssignmentInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_tasks_id;
		string	assignee_user_id;
		string	period_start;
		string	period_end;
		string	hour_limit;
		string	contract_sow_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_tasks_id = db->Get(i, "timecard_tasks_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.period_start = db->Get(i, "period_start");
			item.period_end = db->Get(i, "period_end");
			item.hour_limit = db->Get(i, "hour_limit");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"timecard_tasks_id\":\"" + item.timecard_tasks_id + "\",";
			result +=	"\"assignee_user\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.assignee_user_id + "\" AND `isblocked`=\"N\";", db, NULL) + "],";
			result +=	"\"period_start\":\"" + item.period_start + "\",";
			result +=	"\"period_end\":\"" + item.period_end + "\",";
			result +=	"\"hour_limit\":\"" + item.hour_limit + "\",";
			result +=	"\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "tasks are not assigned (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTExpenseAssignmentInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	auto	result = ""s;
	int		affected;

	struct ItemClass
	{
		string	id;
		string	sow_id;
		string	bt_expense_template_id;
		string	assignee_user_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_expense_template_id = db->Get(i, "bt_expense_template_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.sow_id = db->Get(i, "sow_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"bt_expense_template_id\":\"" + item.bt_expense_template_id + "\",";
			result +=	"\"assignee_user\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.assignee_user_id + "\" AND `isblocked`=\"N\";", db, NULL) + "],";
			result +=	"\"sow_id\":\"" + item.sow_id + "\",";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "bt_expenses are not assigned (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardLinesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_task_id;
		string	row;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_task_id = db->Get(i, "timecard_task_id");
			item.row = db->Get(i, "row");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"tasks\":[" + GetTimecardTasksInJSONFormat("SELECT * FROM `timecard_tasks` WHERE `id`=\"" + item.timecard_task_id + "\";", db, user) + "],";
			result +=	"\"row\":\"" + item.row + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "timecard entry not found (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
/*
string	GetSOWTimecardApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	type;
		string	rate;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.approver_user_id = db->Get(i, "approver_user_id");
			item.type = db->Get(i, "type");
			item.rate = db->Get(i, "rate");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"approver_user_id\":\"" + item.approver_user_id + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"rate\":\"" + item.rate + "\",";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.approver_user_id + "\";", db, user) + "]";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
*/
string	GetTimecardApprovalsInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	timecard_id;
		string	approver_id;
		string	decision;
		string	comment;
		string	eventTimestamp;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_id = db->Get(i, "timecard_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"timecard_id\":\"" + item.timecard_id + "\",";
			result += "\"approver_id\":\"" + item.approver_id + "\",";
			result += "\"decision\":\"" + item.decision + "\",";
			result += "\"comment\":\"" + item.comment + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTApprovalsInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	bt_id;
		string	approver_id;
		string	decision;
		string	comment;
		string	eventTimestamp;
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
			item.bt_id = db->Get(i, "bt_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"bt_id\":\"" + item.bt_id + "\",";
			result += "\"approver_id\":\"" + item.approver_id + "\",";
			result += "\"decision\":\"" + item.decision + "\",";
			result += "\"comment\":\"" + item.comment + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") there are no bt approvals");
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
/*
string	GetTimecardApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	contract_sow_id;
		string	auto_approve;
		string	type;
		string	rate;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.approver_user_id = db->Get(i, "approver_user_id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.auto_approve = db->Get(i, "auto_approve");
			item.type = db->Get(i, "type");
			item.rate = db->Get(i, "rate");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"approver_user_id\":\"" + item.approver_user_id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user) + "],";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.approver_user_id + "\";", db, user) + "],";
			result += "\"auto_approve\":\"" + item.auto_approve + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"rate\":\"" + item.rate + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "approvers list is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
*/
string	GetApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool include_sow)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	contract_sow_id;
		string	auto_approve;
		string	type;
		string	rate;
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
			item.approver_user_id = db->Get(i, "approver_user_id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.auto_approve = db->Get(i, "auto_approve");
			item.type = db->Get(i, "type");
			item.rate = db->Get(i, "rate");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"approver_user_id\":\"" + item.approver_user_id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			if(include_sow)
				result += "\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user) + "],";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.approver_user_id + "\";", db, user) + "],";
			result += "\"auto_approve\":\"" + item.auto_approve + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"rate\":\"" + item.rate + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "approvers list is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetCostCentersInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	title;
		string	agency_company_id;
		string	assignee_user_id;
		string	eventTimestamp;
	};

	auto					affected = 0;
	auto					result = ""s;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(auto i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"agency_company_id\":\"" + item.agency_company_id + "\",";
			result += "\"assignee_user_id\":\"" + item.assignee_user_id + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "cost_center list is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string	GetTimecardsInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool isExtended)
{
	int		affected;
	string	result;

	struct ItemClass
	{
		string				id;
		string				contract_sow_id;
		string				period_start;
		string				period_end;
		string				status;
		string				submit_date;
		string				approve_date;
		string				eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.period_start = db->Get(i, "period_start");
			item.period_end = db->Get(i, "period_end");
			item.submit_date = db->Get(i, "submit_date");
			item.approve_date = db->Get(i, "approve_date");
			item.status = db->Get(i, "status");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"period_start\":\"" + item.period_start + "\",";
			result += "\"period_end\":\"" + item.period_end + "\",";
			result += "\"submit_date\":\"" + item.submit_date + "\",";
			result += "\"approve_date\":\"" + item.approve_date + "\",";
			result += "\"status\":\"" + item.status + "\",";
			result += "\"lines\":[" + GetTimecardLinesInJSONFormat("SELECT * FROM `timecard_lines` WHERE `timecard_id`=\"" + item.id + "\";", db, user) + "],";
			if(isExtended)
			{
				// result += "\"approvers\":[" + GetSOWTimecardApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.contract_sow_id + "\";", db, user) + "],";
				result += "\"approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.contract_sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
				result += "\"approvals\":[" + GetTimecardApprovalsInJSONFormat("SELECT * FROM `timecard_approvals` WHERE `timecard_id`=\"" + item.id + "\";", db, user) + "],";
			}
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTsInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool isExtended)
{
	int		affected;
	string	result;

	struct ItemClass
	{
		string	id;
		string	contract_sow_id;
		string	date_start;
		string	date_end;
		string	place;
		string	purpose;
		string	status;
		string	submit_date;
		string	approve_date;
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
			item.date_start = db->Get(i, "date_start");
			item.date_end = db->Get(i, "date_end");
			item.place = db->Get(i, "place");
			item.purpose = db->Get(i, "purpose");
			item.status = db->Get(i, "status");
			item.submit_date = db->Get(i, "submit_date");
			item.approve_date = db->Get(i, "approve_date");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"date_start\":\"" + item.date_start + "\",";
			result += "\"date_end\":\"" + item.date_end + "\",";
			result += "\"place\":\"" + item.place + "\",";
			result += "\"purpose\":\"" + item.purpose + "\",";
			result += "\"status\":\"" + item.status + "\",";
			result += "\"submit_date\":\"" + item.submit_date + "\",";
			result += "\"approve_date\":\"" + item.approve_date + "\",";
			if(isExtended)
			{
				result += "\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user, DO_NOT_INCLUDE_TIMECARD_TASKS, INCLUDE_BT_EXPENSE_TEMPLATES) + "],";
				result += "\"expenses\":[" + GetBTExpensesInJSONFormat("SELECT * FROM `bt_expenses` WHERE `bt_id`=\"" + item.id + "\";", db, user) + "],";
				result += "\"approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + item.contract_sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
				result += "\"approvals\":[" + GetBTApprovalsInJSONFormat("SELECT * FROM `bt_approvals` WHERE `bt_id`=\"" + item.id + "\";", db, user) + "],";
			}
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}


string	GetSOWInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool include_tasks, bool include_bt)
{
	int		affected;
	string	result;
	struct ItemClass
	{
		string	id;
		string	subcontractor_company_id;
		string	agency_company_id;
		string	start_date;
		string	end_date;
		string	number;
		string	sign_date;
		string	timecard_period;
		string	subcontractor_create_tasks;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start (include_bt = " + to_string(include_bt) + ", include_tasks = " + to_string(include_tasks) + ")");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.subcontractor_company_id = db->Get(i, "subcontractor_company_id");
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.start_date = db->Get(i, "start_date");
			item.end_date = db->Get(i, "end_date");
			item.number = db->Get(i, "number");
			item.sign_date = db->Get(i, "sign_date");
			item.timecard_period = db->Get(i, "timecard_period");
			item.subcontractor_create_tasks = db->Get(i, "subcontractor_create_tasks");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"subcontractor_company_id\":\"" + item.subcontractor_company_id + "\",";
			result += "\"agency_company_id\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `isBlocked`='N' AND `id`=\"" + item.agency_company_id + "\";", db, user) + "],";
			result += "\"start_date\":\"" + item.start_date + "\",";
			result += "\"end_date\":\"" + item.end_date + "\",";
			result += "\"number\":\"" + item.number + "\",";
			result += "\"sign_date\":\"" + item.sign_date + "\",";
			result += "\"timecard_period\":\"" + item.timecard_period + "\",";

			// --- either BT or timecard tasks (could be changed in future)
			// --- configured to keep MySQL DB load low
			if(include_bt)
			{
				result += "\"bt_expense_templates\":[" + GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `id` IN (SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `sow_id`=\"" + item.id + "\");", db, user) + "],";
			}
			if(include_tasks)
			{
				result += "\"tasks\":[" + GetTimecardTasksInJSONFormat(
					"SELECT * FROM `timecard_tasks` WHERE "
							"`id` IN (SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + item.id + "\" "
									");", db, user) + "],";
			}
			result += "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
			result += "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
			result += "\"subcontractor_create_tasks\":\"" + item.subcontractor_create_tasks + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

auto	GetBTExpenseTemplatesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	title;
		string	agency_comment;
		string	is_default;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.agency_comment = db->Get(i, "agency_comment");
			item.is_default = db->Get(i, "is_default");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"agency_comment\":\"" + item.agency_comment + "\",";
			result += "\"line_templates\":[" + GetBTExpenseLineTemplatesInJSONFormat("SELECT * FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=\"" + item.id + "\";", db, user) + "],";
			result += "\"is_default\":\"" + item.is_default + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt items assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTExpenseLineTemplatesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	int		affected;
	string	result;
	struct ItemClass
	{
		string	id;
		string	bt_expense_template_id;
		string	dom_type;
		string	title;
		string	description;
		string	tooltip;
		string	payment;
		string	required;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_expense_template_id = db->Get(i, "bt_expense_template_id");
			item.dom_type = db->Get(i, "dom_type");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.tooltip = db->Get(i, "tooltip");
			item.payment = db->Get(i, "payment");
			item.required = db->Get(i, "required");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"bt_expense_template_id\":\"" + item.bt_expense_template_id + "\",";
			result += "\"dom_type\":\"" + item.dom_type + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"description\":\"" + item.description + "\",";
			result += "\"tooltip\":\"" + item.tooltip + "\",";
			result += "\"payment\":\"" + item.payment + "\",";
			result += "\"required\":\"" + item.required + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt line templates assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTExpenseLinesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	int		affected;
	string	result;
	struct ItemClass
	{
		string	id;
		string	bt_expense_id;
		string	bt_expense_line_template_id;
		string	value;
		string	comment;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_expense_id = db->Get(i, "bt_expense_id");
			item.bt_expense_line_template_id = db->Get(i, "bt_expense_line_template_id");
			item.value = db->Get(i, "value");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"bt_expense_id\":\"" + item.bt_expense_id + "\",";
			result += "\"bt_expense_line_template_id\":\"" + item.bt_expense_line_template_id + "\",";
			result += "\"value\":\"" + item.value + "\",";
			result += "\"comment\":\"" + item.comment + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt lines assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string SummarizeTimereports(string timereport1, string timereport2)
{
	string			result = "";
	vector<string>	timereport1_vector, timereport2_vector, sum;

	MESSAGE_DEBUG("", "", "start");

	timereport1_vector = SplitTimeentry(timereport1, ',');
	timereport2_vector = SplitTimeentry(timereport2, ',');

	if(timereport1_vector.size() == timereport2_vector.size())
	{
		for(unsigned int i = 0; i < timereport1_vector.size(); ++i)
		{
			c_float	term1 = { timereport1_vector.at(i) };
			c_float	term2 = { timereport2_vector.at(i) };
			c_float	sum_result = term1 + term2;

			sum.push_back(sum_result.Get() ? string(sum_result) : "");
		}

		for(unsigned int i = 0; i < sum.size(); ++i)
		{
			if(i) result += ",";
			result += sum.at(i);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "can't summarize: " + timereport1 + ", " + timereport2 + " had different lengths " + to_string(timereport1_vector.size()) + "/" + to_string(timereport2_vector.size()) + " ");
	}

	MESSAGE_DEBUG("", "", "finish (" + timereport1 + " + " + timereport2 + " = " + result + ")");

	return	result;
}

bool isTimecardEntryEmpty(string timereports)
{
	bool	result = true;
	vector<string>	timereport_vector;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	timereport_vector = SplitTimeentry(timereports, ',');
	for(auto &item: timereport_vector)
	{
		float	digit = 0;

		try
		{
			digit = stof(item);
		}
		catch(...)
		{
			MESSAGE_ERROR("", "", "can't convert " + item + " to float");
		}

		if(digit)
		{
			result = false;
			break;
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");
	}

	return	result;
}

bool isUserAssignedToSoW(string user_id, string sow_id, CMysql *db)
{
	bool 	result = false;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(user_id.length())
	{
		if(sow_id.length())
		{
			if(db->Query("SELECT `admin_userID` FROM `company` WHERE "
									"`type`=\"subcontractor\" "
									"AND "
									"(`id` IN (SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\"))"
				))
			{
				string sow_user_id = db->Get(0, "admin_userID");

				if(user_id == sow_user_id) result = true;
				else
				{
					MESSAGE_DEBUG("", "", "**user_id (" + user_id + ") doesn't assigned to SoW(" + sow_id + ")");
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "*user_id (" + user_id + ") doesn't assigned to SoW(" + sow_id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "sow_id is empty")
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user_id is empty")
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");
	}

	return result;
}

bool isSoWAllowedToCreateTask(string sow_id, CMysql *db)
{
	bool 	result = false;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(sow_id.length())
	{
		if(db->Query("SELECT `subcontractor_create_tasks` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
		{
			string task_change_flag = db->Get(0, "subcontractor_create_tasks");

			if(task_change_flag == "Y") result = true;
		}
		else
		{
			MESSAGE_DEBUG("", "", "SoW(" + sow_id + ") not found");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty")
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");
	}

	return result;
}

string GetTaskIDFromAgency(string customer, string project, string task, string agency_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(agency_id.length())
				{
					if(db->Query(
							"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + task + "\" AND `timecard_projects_id` IN ("
							    "SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
							        "SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\" AND `agency_company_id`=\"" + agency_id + "\""
							    ")"
							");"
					))
					{
						result = db->Get(0, "id");
					}
					else
					{
						MESSAGE_DEBUG("", "", "triplet customer/project/task doesn't assigned to agency.id(" + agency_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "agency_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string GetTaskIDFromSOW(string customer, string project, string task, string sow_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(sow_id.length())
				{
					if(db->Query(
							"SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN ("
								"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + task + "\" AND `timecard_projects_id` IN ("
									"SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
										"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\""
									")"
								")"
							") LIMIT 0,1;"
					))
					{
						result = db->Get(0, "timecard_tasks_id");
					}
					else
					{
						MESSAGE_DEBUG("", "", "triplet customer/project/task doesn't assigned to sow.id(" + sow_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "sow_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

auto GetBTExpenseTemplateAssignmentToSoW(string bt_expense_template_id, string sow_id, CMysql *db) -> string
{
	auto 	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(bt_expense_template_id.length())
	{
		if(sow_id.length())
		{
			if(db->Query(
				"SELECT `id` FROM `bt_sow_assignment` WHERE `sow_id`=\"" + sow_id + "\" AND `bt_expense_template_id`=\"" + bt_expense_template_id + "\";"
			))
			{
				result = db->Get(0, "id");
			}
			else
			{
				MESSAGE_DEBUG("", "", "bt_expense_template_id(" + bt_expense_template_id + ") doesn't assigned to sow_id(" + sow_id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "sow_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_template_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetTaskAssignmentID(string customer, string project, string task, string sow_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(sow_id.length())
				{
					if(db->Query(
							"SELECT `id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN ("
								"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + task + "\" AND `timecard_projects_id` IN ("
									"SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
										"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\""
									")"
								")"
							") LIMIT 0,1;"
					))
					{
						result = db->Get(0, "id");
					}
					else
					{
						MESSAGE_DEBUG("", "", "triplet customer/project/task doesn't assigned to sow_id(" + sow_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "sow_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string CreateTaskBelongsToAgency(string customer, string project, string task, string agency_id, CMysql *db)
{
	string	customer_id = "";
	string	project_id = "";
	string	task_id = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(agency_id.length())
				{
					int affected = db->Query(
						"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + agency_id + "\";"
					);
					if(affected)
					{
						string	sow_list = "";

						for(int i = 0; i < affected; ++i)
						{
							if(i) sow_list += ",";
							sow_list += db->Get(i, "id");
						}

						// --- try to define project_id from customers and projects belong to SoW
						if(db->Query(
							    "SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
							        "SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\" AND `agency_company_id`=\"" + agency_id + "\""
							    ")"
						))
						{
							project_id = db->Get(0, "id");
							customer_id = "fake number";
						}
						else
						{
							// --- try to define customer_id from customers belong to SoW
							if(db->Query(
						        "SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\" AND `agency_company_id`=\"" + agency_id + "\""
							))
							{
								customer_id = db->Get(0, "id");
							}
							else
							{
								MESSAGE_DEBUG("", "", "neither customer nor project exists in agency portfolio");
							}
						}

						if(customer_id.empty())
						{
							long int	temp = db->InsertQuery("INSERT INTO `timecard_customers` (`agency_company_id`,`title`,`eventTimestamp`) VALUES (\"" + agency_id + "\", \"" + customer + "\", UNIX_TIMESTAMP());");

							if(temp) { customer_id = to_string(temp); }
							else
							{
								MESSAGE_ERROR("", "", "fail to insert to db");
							}
						}

						if(customer_id.length() && project_id.empty())
						{
							long int	temp = db->InsertQuery("INSERT INTO `timecard_projects` (`timecard_customers_id`,`title`,`eventTimestamp`) VALUES (\"" + customer_id + "\",\"" + project + "\", UNIX_TIMESTAMP());");

							if(temp) { project_id = to_string(temp); }
							else
							{
								MESSAGE_ERROR("", "", "fail to insert to db");
							}
						}

						if(customer_id.length() && project_id.length() && task_id.empty())
						{
							long int	temp = db->InsertQuery("INSERT INTO `timecard_tasks` (`timecard_projects_id`,`title`,`eventTimestamp`) VALUES (\"" + project_id + "\",\"" + task + "\", UNIX_TIMESTAMP());");

							if(temp) { task_id = to_string(temp); }
							else
							{
								MESSAGE_ERROR("", "", "fail to insert to db");
							}
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to define all sow's signed with the same agency.id(" + agency_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "agency_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + task_id + ")");
	}

	return task_id;
}

string CreateTaskAssignment(string task_id, string sow_id, string assignment_start, string assignment_end, CMysql *db, CUser *user)
{
	string	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(task_id.length() && sow_id.length() && assignment_start.length() && assignment_end.length() && db)
	{
		long int	assignment_id = db->InsertQuery("INSERT INTO `timecard_task_assignment` SET "
													"`timecard_tasks_id`=\"" + task_id + "\","
													"`assignee_user_id`=\"" + user->GetID() + "\","
													"`period_start`=\"" + assignment_start + "\","
													"`period_end`=\"" + assignment_end + "\","
													"`hour_limit`=\"" + "0" + "\","
													"`contract_sow_id`=\"" + sow_id + "\","
													"`eventTimestamp`=UNIX_TIMESTAMP()"
													";");

		if(assignment_id)
		{
			result = to_string(assignment_id);
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to insert to db");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "one of mandatory parameters missed");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

auto CreateBTExpenseTemplateAssignmentToSoW(string new_bt_expense_template_id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(new_bt_expense_template_id.length() && sow_id.length() && db)
	{
		long int	assignment_id = db->InsertQuery("INSERT INTO `bt_sow_assignment` SET "
													"`bt_expense_template_id`=\"" + new_bt_expense_template_id + "\","
													"`sow_id`=\"" + sow_id + "\","
													"`assignee_user_id`=\"" + user->GetID() + "\","
													"`eventTimestamp`=UNIX_TIMESTAMP()"
													";");

		if(assignment_id)
		{
			result = to_string(assignment_id);
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to insert to db");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "one of mandatory parameters missed");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetTimecardID(string sow_id, string period_start, string period_end, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(period_start.length() && period_end.length())
	{
		if(sow_id.length())
		{
			if(db->Query("SELECT `id` FROM `timecards` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `period_start`=\"" + period_start + "\" AND `period_end`=\"" + period_end + "\";"))
			{
				result = db->Get(0, "id");
			}
			else
			{
				long int temp;

				MESSAGE_DEBUG("", "", "timecard(sow_id/period_start/period_end = " + sow_id + "/" + period_start + "/" + period_end + ") not found, create new one");

				temp = db->InsertQuery("INSERT INTO `timecards` SET "
												"`contract_sow_id`=\"" + sow_id + "\","
												"`period_start`=\"" + period_start + "\","
												"`period_end`=\"" + period_end + "\","
												"`status`=\"saved\","
												"`eventTimestamp`=UNIX_TIMESTAMP();");

				if(temp) result = to_string(temp);
				else MESSAGE_ERROR("", "", "DB issue: insert request failed");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "sow_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "period not defined (start:" + period_end + " end:" + period_end + ")");
	}


	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string GetTimecardStatus(string timecard_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(timecard_id.length())
	{
			if(db->Query("SELECT `status` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
			{
				result = db->Get(0, "status");
			}
			else
			{
				MESSAGE_ERROR("", "", "timecard_id(" + timecard_id + ") not fount in DB");
			}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_id is empty");
	}


	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string GetAgencyID(string sow_id, CMysql *db)
{
	string 	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
			if(db->Query("SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
			{
				result = db->Get(0, "agency_company_id");
			}
			else
			{
				MESSAGE_ERROR("", "", "sow_id(" + sow_id + ") not fount in DB");
			}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetAgencyID(CUser *user, CMysql *db)
{
	string 	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(user && db)
	{
		if(user->GetType() == "agency")
		{
				if(db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";"))
				{
					result = db->Get(0, "company_id");
				}
				else
				{
					MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") isn't an agency employee");
				}
		}
		else
		{
			MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") isn't an agency employee");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user doesn't initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string	GetDBValueByAction(string action, string id, string sow_id, CMysql *db, CUser *user)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				string		sql_query = "";

				if(action == "AJAX_updateCustomerTitle") 			sql_query = "SELECT `title`				as `value` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateProjectTitle") 			sql_query = "SELECT `title`				as `value` FROM `timecard_projects` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateTaskTitle") 				sql_query = "SELECT `title`				as `value` FROM `timecard_tasks` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodStart") 				sql_query = "SELECT `period_start`		as `value` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodEnd") 				sql_query = "SELECT `period_end`		as `value` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTitle")				sql_query = "SELECT `name`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyDescription")		sql_query = "SELECT `description`		as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyWebSite")			sql_query = "SELECT `webSite`			as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTIN")				sql_query = "SELECT `tin`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyAccount")			sql_query = "SELECT `account`			as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyOGRN")				sql_query = "SELECT `ogrn`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyKPP")				sql_query = "SELECT `kpp`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyLegalAddress")		sql_query = "SELECT `legal_address`		as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyMailingAddress")	sql_query = "SELECT `mailing_address`	as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyMailingZipID")		sql_query = "SELECT `mailing_geo_zip_id`as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyLegalZipID")		sql_query = "SELECT `legal_geo_zip_id`	as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyBankID")			sql_query = "SELECT `bank_id`			as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyPosition")			sql_query = "SELECT `title`				as `value` FROM `company_position` WHERE `id`=(SELECT `position_id` FROM `company_employees` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateAgencyEditCapability")		sql_query = "SELECT `allowed_change_agency_data`	as `value` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSoWEditCapability")		sql_query = "SELECT `allowed_change_sow`			as `value` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSubcontractorCreateTasks")	sql_query = "SELECT `subcontractor_create_tasks`	as `value` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateTitle")		sql_query = "SELECT `title`							as `value` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateAgencyComment") 	sql_query = "SELECT `agency_comment`		as `value` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "SELECT `title`					as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "SELECT `description`			as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "SELECT `tooltip`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "SELECT `dom_type`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCash")	sql_query = "SELECT `payment`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCard")	sql_query = "SELECT `payment`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "SELECT `required`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";

				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						result = db->Get(0, "value");
					}
					else
					{
						MESSAGE_DEBUG("", "", "" + action + " with id(" + id + ") returned empty result");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "unknown action (" + action + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "id is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string GetTimecardLineID(string timecard_id, string task_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(timecard_id.length())
	{
		if(task_id.length())
		{
			if(db->Query("SELECT `id` FROM `timecard_lines` WHERE `timecard_id`=\"" + timecard_id + "\" AND `timecard_task_id`=\"" + task_id + "\";"))
			{
				result = db->Get(0, "id");
			}
			else
			{
				MESSAGE_DEBUG("", "", "timecard_line(timecard_id / task_id: " + timecard_id + "/" + task_id + ") not fount in DB");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "task_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_id is empty");
	}


	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}
/*
vector<string> GetTimecardApprovals(string timecard_id, CMysql *db, CUser *user)
{
	vector<string>		result;

	MESSAGE_DEBUG("", "", "start");

	int		affected = db->Query(
				"SELECT `id` FROM `timecard_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
				";");

	// --- get timecard approvals
	for(int i = 0; i < affected; ++i)
	{
		result.push_back(db->Get(i, "id"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

vector<string> GetTimecardApprovers(string timecard_id, CMysql *db, CUser *user)
{
	vector<string>		result;

	MESSAGE_DEBUG("", "", "start");

	int		affected = db->Query(
				"SELECT `id` FROM `timecard_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
				";");

	// --- get timecard approvers
	for(int i = 0; i < affected; ++i)
	{
		result.push_back(db->Get(i, "id"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}
*/
bool SubmitTimecard(string timecard_id, CMysql *db, CUser *user)
{
	bool 	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(timecard_id.length())
	{
		struct ApproveClass
		{
			string	id;
			string	timecard_id;
			string	approver_id;
			string	decision;
			string	comment;
			string	eventTimestamp;
		};
		map<string, bool>			approvers;
		vector<ApproveClass>		approves_list;
		int							affected;
		bool						all_approvers_confirm = true;

		affected = db->Query(
				"SELECT `id` FROM `timecard_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
				";");
		for(int i = 0; i < affected; i++)
		{
			approvers[db->Get(i, "id")] = false;
		}

		affected = db->Query(
				"SELECT * FROM `timecard_approvals` WHERE "
					"`eventTimestamp` > (SELECT `eventTimestamp` FROM `timecards` WHERE `id`=\"" + timecard_id + "\") "
					"AND "
					"`timecard_id`=\"" + timecard_id + "\""
				";");
		for(int i = 0; i < affected; i++)
		{
			ApproveClass	item;

			item.id = db->Get(i, "id");
			item.timecard_id = db->Get(i, "timecard_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			approves_list.push_back(item);

			if(approvers.find(item.approver_id) == approvers.end())
			{
				MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") not found in approvers list. Check DB structure !");
			}
			else
			{
				if(item.decision == "approved")
					approvers[item.approver_id] = true;
				else
				{
					MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") did (" + item.decision + ") action on timecard(" + item.timecard_id + "). According to timecard approve workflow, this branch can be used for \"approved\" timecards only. Check timecard workflow !");
				}
			}
		}

		for(auto &approver: approvers)
		{
			if(!approver.second) { all_approvers_confirm = false; break; }
		}

		if(all_approvers_confirm)
		{
			db->Query("UPDATE `timecards` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
			if(db->isError())
			{
				MESSAGE_ERROR("", "", "fail to update timecards table with timecard_id(" + timecard_id + ")");
			}
			else
			{
				result = true;
			}
		}
		else
		{
			result = true;
			MESSAGE_DEBUG("", "", "not all approvers approved timecard.id(" + timecard_id + ")");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string GetNumberOfTimecardsInPendingState(CMysql *db, CUser *user)
{
	struct PendingTimecardClass
	{
		string	id;
		string	contract_sow_id;
		string	submit_date;

		PendingTimecardClass(string p1, string p2, string p3) : id(p1), contract_sow_id(p2), submit_date(p3) {};
		PendingTimecardClass() {};
	};
	vector<PendingTimecardClass>		timecards_list;

	int			affected = 0;
	int			pending_timecards = 0;
	string		company_id;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());

		affected = db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";");
		if(affected)
		{
			company_id = db->Get(0, "company_id");

			affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `timecards` WHERE "
										"`status`=\"submit\" "
										"AND "
										"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + company_id + "\");");
			for(int i = 0; i < affected; ++i)
			{
				PendingTimecardClass	item;

				item.id = db->Get(i, "id");
				item.contract_sow_id = db->Get(i, "contract_sow_id");
				item.submit_date = db->Get(i, "submit_date");

				timecards_list.push_back(item);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") doesn't belong to any agency");
		}
	}
	else if(user->GetType() == "approver")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());


		affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `timecards` WHERE "
									"`status`=\"submit\" "
									"AND "
									"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\");");
		for(int i = 0; i < affected; ++i)
		{
			PendingTimecardClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.submit_date = db->Get(i, "submit_date");

			timecards_list.push_back(item);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	for(auto &timecard: timecards_list)
	{
		if(db->Query("SELECT `decision` FROM `timecard_approvals` WHERE "
						"`timecard_id` = \"" + timecard.id + "\" "
						"AND "
						"`eventTimestamp` > \"" + timecard.submit_date + "\" "
						"AND "
						"`approver_id` IN (SELECT `id` FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + timecard.contract_sow_id + "\" AND `approver_user_id`=\"" + user->GetID() + "\")"))
		{
			string		decision = db->Get(0, "decision");

			if(decision == "approved")
			{
				// --- everything ok
			}
			else
			{
				MESSAGE_ERROR("", "", "check approval workflow for contract(" + timecard.contract_sow_id + ") at this time(" + timecard.submit_date + ") timcard.id(" + timecard.id + ") must have only \"approved\" approvals from user.id(" + user->GetID() + ")");
			}
		}
		else
		{
			++pending_timecards;
		}
	}

	MESSAGE_DEBUG("", "", "finish (number of pending_timecards is " + to_string(pending_timecards) + ")");

	return to_string(pending_timecards);
}

string GetNumberOfBTInPendingState(CMysql *db, CUser *user)
{
	struct PendingBTClass
	{
		string	id = "";
		string	contract_sow_id = "";
		string	submit_date = "";

		PendingBTClass(string p1, string p2, string p3) : id(p1), contract_sow_id(p2), submit_date(p3) {};
		PendingBTClass() {};
	};
	vector<PendingBTClass>		bt_list;

	int			affected = 0;
	int			pending_bt = 0;
	string		company_id;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());

		affected = db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";");
		if(affected)
		{
			company_id = db->Get(0, "company_id");

			affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `bt` WHERE "
										"`status`=\"submit\" "
										"AND "
										"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + company_id + "\");");
			for(int i = 0; i < affected; ++i)
			{
				PendingBTClass	item(db->Get(i, "id"), db->Get(i, "contract_sow_id"), db->Get(i, "submit_date"));

				item.id = db->Get(i, "id");
				item.contract_sow_id = db->Get(i, "contract_sow_id");
				item.submit_date = db->Get(i, "submit_date");

				bt_list.push_back(item);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") doesn't belong to any agency");
		}
	}
	else if(user->GetType() == "approver")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());

		affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `bt` WHERE "
									"`status`=\"submit\" "
									"AND "
									"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\");");
		for(int i = 0; i < affected; ++i)
		{
			PendingBTClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.submit_date = db->Get(i, "submit_date");

			bt_list.push_back(item);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	for(auto &bt: bt_list)
	{
		if(db->Query("SELECT `decision` FROM `bt_approvals` WHERE "
						"`bt_id` = \"" + bt.id + "\" "
						"AND "
						"`eventTimestamp` > \"" + bt.submit_date + "\" "
						"AND "
						"`approver_id` IN (SELECT `id` FROM `bt_approvers` WHERE `contract_sow_id`=\"" + bt.contract_sow_id + "\" AND `approver_user_id`=\"" + user->GetID() + "\")"))
		{
			string		decision = db->Get(0, "decision");

			if(decision == "approved")
			{
				// --- everything ok
			}
			else
			{
				MESSAGE_ERROR("", "", "check approval workflow for contract(" + bt.contract_sow_id + ") at this time(" + bt.submit_date + ") timcard.id(" + bt.id + ") must have only \"approved\" approvals from user.id(" + user->GetID() + ")");
			}
		}
		else
		{
			++pending_bt;
		}
	}

	MESSAGE_DEBUG("", "", "finish (number of pending_bt is " + to_string(pending_bt) + ")");

	return to_string(pending_bt);
}

string	GetObjectsSOW_Reusable_InJSONFormat(string object, string filter, CMysql *db, CUser *user)
{
	string		result = "";

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if((user->GetType() == "agency") || (user->GetType() == "approver"))
			{

				if(object == "timecard")
				{
					string		sql_query_where_statement = "`contract_sow_id` IN ( SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\")";

					if(filter == "submit") sql_query_where_statement += " AND `status` = \"submit\" ";

					result += 
						"\"timecards\":[" + GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE " + sql_query_where_statement, db, user, true) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`id` IN ( SELECT `contract_sow_id` FROM `timecards` WHERE " + sql_query_where_statement + ") "
								";", db, user) + "]";
				}
				else if(object == "bt")
				{
					string		sql_query_where_statement = "`contract_sow_id` IN ( SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\")";

					if(filter == "submit") sql_query_where_statement += " AND `status` = \"submit\" ";

					result +=
						"\"bt\":[" + GetBTsInJSONFormat("SELECT * FROM `bt` WHERE " + sql_query_where_statement, db, user, true) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`id` IN ( SELECT `contract_sow_id` FROM `bt` WHERE " + sql_query_where_statement + ") "
								";", db, user) + "]";
				}
				else
				{
					MESSAGE_ERROR("", "", "unknown object type (" + object + ")");
				}

			}
			else
			{
				MESSAGE_ERROR("", "", "unknown user.id(" + user->GetID() + ") type(" + user->GetID() + ")");
			}

		}
		else
		{
			MESSAGE_ERROR("", "", "db is NULL");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is NULL");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + " symbols)");

	return result;
}

string	GetCurrencyRatesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	date;
		string	char_code;
		string	nominal;
		string	name;
		string	value;
	};
	vector<ItemClass>		itemsList;
	int						affected;
	string					result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.date = db->Get(i, "date");
			item.char_code = db->Get(i, "char_code");
			item.nominal = db->Get(i, "nominal");
			item.name = db->Get(i, "name");
			item.value = db->Get(i, "value");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"date\":\"" + item.date + "\",";
			result += "\"char_code\":\"" + item.char_code + "\",";
			result += "\"nominal\":\"" + item.nominal + "\",";
			result += "\"name\":\"" + item.name + "\",";
			result += "\"value\":\"" + item.value + "\"";

			result +=	"}";
		}
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + " symbols)");

	return result;
}


bool	SubmitBT(string bt_id, CMysql *db, CUser *user)
{
	bool 	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(bt_id.length())
	{
		struct ApproveClass
		{
			string	id;
			string	bt_id;
			string	approver_id;
			string	decision;
			string	comment;
			string	eventTimestamp;
		};
		map<string, bool>			approvers;
		vector<ApproveClass>		approves_list;
		int							affected;
		bool						all_approvers_confirm = true;

		affected = db->Query(
				"SELECT `id` FROM `bt_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
				";");
		for(int i = 0; i < affected; i++)
		{
			approvers[db->Get(i, "id")] = false;
		}

		affected = db->Query(
				"SELECT * FROM `bt_approvals` WHERE "
					"`eventTimestamp` > (SELECT `eventTimestamp` FROM `bt` WHERE `id`=\"" + bt_id + "\") "
					"AND "
					"`bt_id`=\"" + bt_id + "\""
				";");
		for(int i = 0; i < affected; i++)
		{
			ApproveClass	item;

			item.id = db->Get(i, "id");
			item.bt_id = db->Get(i, "bt_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			approves_list.push_back(item);

			if(approvers.find(item.approver_id) == approvers.end())
			{
				MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") not found in approvers list. Check DB structure !");
			}
			else
			{
				if(item.decision == "approved")
					approvers[item.approver_id] = true;
				else
				{
					MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") did (" + item.decision + ") action on bt(" + item.bt_id + "). According to bt approve workflow, this branch can be used for \"approved\" bt only. Check bt workflow !");
				}
			}
		}

		for(auto &approver: approvers)
		{
			if(!approver.second) { all_approvers_confirm = false; break; }
		}

		if(all_approvers_confirm)
		{
			db->Query("UPDATE `bt` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + bt_id + "\";");
			if(db->isError())
			{
				MESSAGE_ERROR("", "", "fail to update bt table with bt_id(" + bt_id + ")");
			}
			else
			{
				result = true;
			}
		}
		else
		{
			result = true;
			MESSAGE_DEBUG("", "", "not all approvers approved bt.id(" + bt_id + ")");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "bt_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string isCBCurrencyRate(string date, string currency_name, string currency_nominal, string currency_value, CMysql *db)
{
	string	result = "N";

	MESSAGE_DEBUG("", "", "start");

	if(date.length())
	{
		if(currency_name.length())
		{
			if(currency_nominal.length())
			{
				if(currency_value.length())
				{
					if(db->Query("SELECT `id` FROM `currency_rate` WHERE `date`=\"" + date + "\" AND `name`=\"" + currency_name + "\" AND `nominal`=\"" + currency_nominal + "\" AND `value`=\"" + currency_value + "\";"))
					{
						result = "Y";
					}
					else
					{
						MESSAGE_DEBUG("", "", "currency rate(" + currency_nominal + "/" + currency_value + ") on date(" + date + ") doesn't aligned with Centro Bank");
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", "currency_value is empty");
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "currency_nominal is empty");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "currency_name is empty");
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "date is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + result + ")");

	return result;
}

string	GetBTExpensesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	bt_id;
		string	bt_expense_template_id;
		string	comment;
		string	date;
		string	payment_type;
		string	price_foreign;
		string	price_domestic;
		string	currency_nominal;
		string	currency_name;
		string	currency_value;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;
	string					result = "";
	int						affected;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id							= db->Get(i, "id");
			item.bt_id			 			= db->Get(i, "bt_id");
			item.bt_expense_template_id		= db->Get(i, "bt_expense_template_id");
			item.comment					= db->Get(i, "comment");
			item.date						= db->Get(i, "date");
			item.payment_type				= db->Get(i, "payment_type");
			item.price_foreign				= db->Get(i, "price_foreign");
			item.price_domestic				= db->Get(i, "price_domestic");
			item.currency_nominal			= db->Get(i, "currency_nominal");
			item.currency_name				= db->Get(i, "currency_name");
			item.currency_value				= db->Get(i, "currency_value");
			item.eventTimestamp				= db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""						+ item.id + "\",";
			result += "\"bt_id\":\""					+ item.bt_id + "\",";
			result += "\"bt_expense_template_id\":\""	+ item.bt_expense_template_id + "\",";
			result += "\"bt_expense_templates\":["		+ GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `id`=\"" + item.bt_expense_template_id + "\";", db, user) + "],";
			result += "\"bt_expense_lines\":["			+ GetBTExpenseLinesInJSONFormat("SELECT * FROM `bt_expense_lines` WHERE `bt_expense_id`=\"" + item.id + "\";", db, user) + "],";
			result += "\"comment\":\""					+ item.comment + "\",";
			result += "\"date\":\""						+ item.date + "\",";
			result += "\"payment_type\":\""				+ item.payment_type + "\",";
			result += "\"price_foreign\":\""			+ item.price_foreign + "\",";
			result += "\"price_domestic\":\""			+ item.price_domestic + "\",";
			result += "\"currency_nominal\":\""			+ item.currency_nominal + "\",";
			result += "\"currency_name\":\""			+ item.currency_name + "\",";
			result += "\"currency_value\":\""			+ item.currency_value + "\",";
			result += "\"is_cb_currency_rate\":\""		+ isCBCurrencyRate(item.date, item.currency_name, item.currency_nominal, item.currency_value, db) + "\",";
			result += "\"eventTimestamp\":\""			+ item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") bt have no expenses assigned");
		}
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string isUserAllowedAccessToBT(string bt_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(bt_id.length())
	{
		if(user->GetType() == "subcontractor")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN ("
						"SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user->GetID() + "\""
					")"
				")"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = "У Вас нет доступа";
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else if(user->GetType() == "agency")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN ("
						"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
					")"
				")"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = "У Вас нет доступа";
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else if(user->GetType() == "approver")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\""
					")"
				";"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = "У Вас нет доступа";
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else
		{
			error_message = "Неизвестный тип пользователя";
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
		}
	}
	else
	{
		error_message = "Командировка не найдена";
		MESSAGE_ERROR("", "", "bt_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isExpenseTemplateIDValidToRemove(string bt_expense_template_id, CMysql *db) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(bt_expense_template_id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `bt_expenses` WHERE `bt_expense_template_id`=\"" + bt_expense_template_id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				// --- task.id valid to remve
			}
			else
			{
				error_message = "На этот расход субконтракторы отчитались " + counter + " раз. Поэтому нельзя удалить.";
				MESSAGE_DEBUG("", "", "subcontractors reported " + counter + " times on bt_expense_templates.id(" + bt_expense_template_id + ")");
			}
		}
		else
		{
			error_message = "Ошибка БД.";
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		error_message = "Неопределен идентификатор удаляемого шаблона.";
		MESSAGE_ERROR("", "", "bt_expense_template_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isExpenseTemplateLineIDValidToRemove(string bt_expense_line_template_id, CMysql *db) -> string
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(bt_expense_line_template_id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + bt_expense_line_template_id + "\");"))
		{
			string	counter = db->Get(0, "counter");
			if(stoi(counter) > 1)
			{
				if(db->Query("SELECT COUNT(*) AS `counter` FROM `bt_expense_lines` WHERE `bt_expense_line_template_id`=\"" + bt_expense_line_template_id + "\";"))
				{
					string	counter = db->Get(0, "counter");
					if(counter == "0")
					{
						// --- task.id valid to remve
					}
					else
					{
						error_message = "Этот документ присутствует в " + counter + " отчетных документах. Нельзя удалить.";
						MESSAGE_DEBUG("", "", "subcontractors reported " + counter + " times on bt_expense_templates.id(" + bt_expense_line_template_id + ")");
					}
				}
				else
				{
					error_message = "Ошибка БД.";
					MESSAGE_ERROR("", "", "fail in sql-query syntax");
				}
			}
			else
			{
				error_message = "Нельзя удалять единственный документ из расхода.";
				MESSAGE_DEBUG("", "", "Can't remove single bt_expense_line_templates.id(" + bt_expense_line_template_id + ") from BT expense");
			}
		}
		else
		{
			error_message = "Ошибка БД.";
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		error_message = "Некорректные параметры.";
		MESSAGE_ERROR("", "", "bt_expense_line_template_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isTaskIDValidToRemove(string task_id, CMysql *db) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(task_id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_lines` WHERE `timecard_task_id`=\"" + task_id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + task_id + "\";"))
				{
					string	counter = db->Get(0, "counter");
					if(counter == "0")
					{
						// --- task.id valid to remve
					}
					else
					{
						MESSAGE_DEBUG("", "", counter + "subcontractors assigned on task.id(" + task_id + ")");
						error_message = "Эта задача назначена " + counter + " субконтракторам. Поэтому нельзя удалить.";
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "fail in sql-query syntax");
					error_message = "Ошибка БД.";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "subcontractors reported " + counter + " times on task.id(" + task_id + ")");
				error_message = "На эту задачу субконтракторы отчитались. Поэтому нельзя удалить.";
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
			error_message = "Ошибка БД.";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id is empty");
		error_message = "Задача не найдена";
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string isEmployeeIDValidToRemove(string employee_id, CMysql *db)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(employee_id.length())
	{
		if(db->Query("SELECT `user_id` FROM `company_employees` WHERE `id`=\"" + employee_id + "\";"))
		{
			string	user_id = db->Get(0, "user_id");

			if(db->Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user_id + "\";"))
			{
				error_message = "Нельзя уволить владельца компании";
				MESSAGE_DEBUG("", "", "company_employee.id(" + employee_id + ") is the company.id(" + db->Get(0, "id") + ") owner. He can't be fired.");
			}
			else
			{
				// --- good to go
			}
		}
		else
		{
			error_message = "Сотрудник не найден";
			MESSAGE_DEBUG("", "", "employee.id(" + employee_id + ") not found");
		}
	}
	else
	{
		error_message = "Сотрудник не найден";
		MESSAGE_ERROR("", "", "employee_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

bool	isProjectIDValidToRemove(string id, CMysql *db)
{
	bool	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				// --- project.id valid to remve
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "there are " + counter + " task(s) belong to project.id(" + id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

bool	isCustomerIDValidToRemove(string id, CMysql *db)
{
	bool	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				// --- customer.id valid to remve
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "there are " + counter + " project(s) belong to customer.id(" + id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

pair<string, string> GetCustomerIDProjectIDByTaskID(string task_id, CMysql *db)
{
	string		project_id = "", customer_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
		"SELECT `timecard_projects_id`,`timecard_customers_id` FROM `timecard_tasks` "
		"INNER JOIN `timecard_projects` ON `timecard_projects`.`id`=`timecard_tasks`.`timecard_projects_id`"
		"WHERE `timecard_tasks`.`id`=\"" + task_id + "\";"
	))
	{
		project_id = db->Get(0, "timecard_projects_id");
		customer_id = db->Get(0, "timecard_customers_id");
	}

	MESSAGE_DEBUG("", "", "finish (cust_id/proj_id=" + customer_id + "/" + project_id + ")");

	return make_pair(customer_id, project_id);
}

/*
static string	isTimeReportedOnTaskIDBeyondPeriod(string task_id, string start, string end, CMysql *db)
{
	string	error_message = "";
	smatch	cm;
	string	start_year, start_month, start_date;
	string	end_year, end_month, end_date;


	MESSAGE_DEBUG("", "", "start");

	if(start.length())
	{
		// --- normalize start, end format
		if(regex_match(start, cm, regex("^([[:digit:]]{1,2})\\/([[:digit:]]{1,2})\\/([[:digit:]]{2,4})$")))
		{
			start_date = cm[1];
			start_month = cm[2];
			start_year = cm[3];
		}
		else if(regex_match(start, cm, regex("^([[:digit:]]{2,4})\\-([[:digit:]]{1,2})\\-([[:digit:]]{1,2})$")))
		{
			start_date = cm[3];
			start_month = cm[2];
			start_year = cm[1];
		}
		else
		{
			error_message = "Некрректный формат даты начала (" + start + ")";
			MESSAGE_ERROR("", "", "incorrect start date(" + start + ") format");
		}

		if(error_message.empty())
		{
			// --- coarse check moving start date
			if(db->Query(	"SELECT `timecard_id` FROM `timecard_lines` "
							"INNER JOIN `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id` "
							"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`period_start`<\"" + start_year + "-" + start_month + "-" + start_date + "\";"
						))
			{
				// --- placeholder for precise check
				
				error_message = "Невозможно сдвинуть дату, поскольку существует таймкарта с отчетным периодом раньше выбранной.";
				MESSAGE_DEBUG("", "", "Can't change period_start to " + start_year + "-" + start_month + "-" + start_date + ", due to exists timecard.id(" + db->Get(0, "timecard_id") + ") with earlier reporting period");
			}
		}

	}
	else
	{
		MESSAGE_DEBUG("", "", "start time is empty, don't checking start");
	}

	if(end.length())
	{
		// --- normalize start, end format
		if(regex_match(end, cm, regex("^([[:digit:]]{1,2})\\/([[:digit:]]{1,2})\\/([[:digit:]]{2,4})$")))
		{
			end_date = cm[1];
			end_month = cm[2];
			end_year = cm[3];
		}
		else if(regex_match(end, cm, regex("^([[:digit:]]{2,4})\\-([[:digit:]]{1,2})\\-([[:digit:]]{1,2})$")))
		{
			end_date = cm[3];
			end_month = cm[2];
			end_year = cm[1];
		}
		else
		{
			error_message = "Некрректный формат даты окончания (" + end + ")";
			MESSAGE_ERROR("", "", "incorrect end date(" + end + ") format");
		}

		if(error_message.empty())
		{
			// --- coarse check moving start date
			if(db->Query(	"SELECT `timecard_id` FROM `timecard_lines` "
							"INNER JOIN `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id` "
							"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`period_end`>\"" + end_year + "-" + end_month + "-" + end_date + "\";"
						))
			{
				// --- placeholder for precise check
				
				error_message = "Невозможно сдвинуть дату, поскольку существует таймкарта с отчетным периодом позднее выбранной.";
				MESSAGE_DEBUG("", "", "Can't change period_end to " + end_year + "-" + end_month + "-" + end_date + ", due to exists timecard.id(" + db->Get(0, "timecard_id") + ") with later reporting period");
			}
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "end time is empty, don't checking finish");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

double DatesDifference(string start, string finish)
{
	double		result = 0;
	struct tm	start_tm, finish_tm;

	MESSAGE_DEBUG("", "", "start");

	start_tm = GetTMObject(start);
	finish_tm = GetTMObject(finish);

	MESSAGE_DEBUG("", "", "end (result is " + to_string(result) + ")");

	return difftime(mktime(&start_tm), mktime(&finish_tm));
}
*/

pair<int, int> FirstAndLastReportingDaysFromTimereport(const vector<string> &timereport)
{
	int start = 0, end = 0;

	MESSAGE_DEBUG("", "", "start");

	for(auto i = timereport.begin(); i != timereport.end(); ++i)
	{
		if((*i).length() && ((*i) != "0")) break;
		else ++start;
	}

	for(auto i = timereport.rbegin(); i != timereport.rend(); ++i)
	{
		if((*i).length() && ((*i) != "0")) break;
		else ++end;
	}

	MESSAGE_DEBUG("", "", "end (result = " + to_string(start) + "/" + to_string(end) + ")");

	return make_pair(start, end);
}

string	isValidToReportTime(string timecard_id, string task_id, string timereport, CMysql *db, CUser *user)
{
	string		error_message = "";
	struct tm	sow_start_date;
	struct tm	sow_end_date;
	struct tm	task_start_date;
	struct tm	task_end_date;
	struct tm	timecard_start_date;
	struct tm	timecard_end_date;
	struct tm	reporting_timeframe_start_date;
	struct tm	reporting_timeframe_end_date;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
	{
		string	sow_id = db->Get(0, "contract_sow_id");

		timecard_start_date = GetTMObject(db->Get(0, "period_start"));
		timecard_end_date = GetTMObject(db->Get(0, "period_end"));

		if(db->Query("SELECT `start_date`, `end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
		{
			sow_start_date = GetTMObject(db->Get(0, "start_date"));
			sow_end_date = GetTMObject(db->Get(0, "end_date"));

			if(db->Query("SELECT `period_start`, `period_end` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + task_id + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
			{
				vector<string>	timeentry;

				task_start_date = GetTMObject(db->Get(0, "period_start"));
				task_end_date = GetTMObject(db->Get(0, "period_end"));
				timeentry = SplitTimeentry(timereport);

				if(timeentry.size() == round(difftime(mktime(&timecard_end_date), mktime(&timecard_start_date)) / 3600 / 24 + 1))
				{
					int		first_reporting_day, last_reporting_day;

					tie(first_reporting_day, last_reporting_day) = FirstAndLastReportingDaysFromTimereport(timeentry);

					reporting_timeframe_start_date = timecard_start_date;
					reporting_timeframe_end_date = timecard_end_date;

					reporting_timeframe_start_date.tm_mday += first_reporting_day;
					reporting_timeframe_end_date.tm_mday -= last_reporting_day;

					if(reporting_timeframe_start_date <= reporting_timeframe_end_date)
					{
						struct tm available_reporting_start = (task_start_date > sow_start_date ? task_start_date : sow_start_date);
						struct tm available_reporting_end = (task_end_date < sow_end_date ? task_end_date : sow_end_date);

						if(available_reporting_start <= available_reporting_end)
						{
							if(	(available_reporting_start <= reporting_timeframe_start_date) &&
								(available_reporting_end   >= reporting_timeframe_end_date)
								)
							{

							}
							else
							{
								error_message = "Вы не можете отчитываться на задачу. Задача( с " + PrintDate(available_reporting_start) + " по " + PrintDate(available_reporting_end) + "), Ваш отчет( с " + PrintDate(reporting_timeframe_start_date) + " по " + PrintDate(reporting_timeframe_end_date) + ")";
								MESSAGE_ERROR("", "", "Can't report to task_id(" + task_id + "). Available reporting period(" + PrintDate(available_reporting_start) + " - " + PrintDate(available_reporting_end) + "), timereport(" + PrintDate(reporting_timeframe_start_date) + " - " + PrintDate(reporting_timeframe_end_date) + ")");
							}
						}
						else
						{
							error_message = "Задача не активна в данном контракте. Задача( с " + PrintDate(task_start_date) + " по " + PrintDate(task_end_date) + "), догвор( с " + PrintDate(sow_start_date) + " по " + PrintDate(sow_end_date) + ")";
							MESSAGE_ERROR("", "", "task_id(" + task_id + ") not active in sow_id(" + sow_id + "). Task(" + PrintDate(task_start_date) + " - " + PrintDate(task_end_date) + "), sow(" + PrintDate(sow_start_date) + " - " + PrintDate(sow_end_date) + ")");
						}
					}
					else
					{
						error_message = "Начало Вашего отчета(" + PrintDate(reporting_timeframe_start_date) + ") позднее окончания(" + PrintDate(reporting_timeframe_end_date) + ")";
						MESSAGE_ERROR("", "", "reporting start (" + PrintDate(reporting_timeframe_start_date) + ") later than finish(" + PrintDate(reporting_timeframe_end_date) + ")");
					}
				}
				else
				{
					error_message = "Ваш отчет содержит " + to_string(timeentry.size()) + " дней, хотя ожидается " + to_string(round(difftime(mktime(&timecard_end_date), mktime(&timecard_start_date)) / 3600 / 24) + 1);
					MESSAGE_ERROR("", "", "timecard_id(" + timecard_id + ") size(" + to_string(round(difftime(mktime(&timecard_end_date), mktime(&timecard_start_date)) / 3600 / 24) + 1) + ") differ in size from reported timeentry(" + timereport + ") size (" + to_string(timeentry.size()) + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task_assignment(task_id/sow_id - " + task_id + "/" + sow_id + ") start and end period can't be defined");
				error_message = "Вы не назначены на задачу";
			}

		}
		else
		{
			MESSAGE_ERROR("", "", "sow(" + sow_id + ") start and end period can't be defined");
			error_message = "Не найден договор";
		}
	}
	else
	{
		error_message = "Тамкарты нет в БД";
		MESSAGE_ERROR("", "", "fail to find timecard.id(" + timecard_id + ") in db");
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length is " + to_string(error_message.length()) + ")");

	return error_message;	
}

string	GetInfoToReturnByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(action == "AJAX_addTimecardApproverToSoW")	result = "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "]";
	if(action == "AJAX_addBTExpenseApproverToSoW")	result = "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "]";

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(id.length())
	{
		if(action.length())
		{
			if(new_value.length())
			{
					if(action == "AJAX_deleteTimecardApproverFromSoW")
					{
						if(sow_id.length())
						{
							int	affected = db->Query("SELECT `id` FROM `timecards` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";");
							vector<string> timecard_list;

							for(int i = 0; i < affected; ++i)
							{
								timecard_list.push_back(db->Get(i, "id"));
							}

							for (auto &timecard_id: timecard_list)
							{
								if(SubmitTimecard(timecard_id, db, user))
								{

								}
								else
								{
									MESSAGE_ERROR("", "", "fail to re-submit timecard.id(" + timecard_id + ")");
								}
							}
							
						}
						else
						{
							error_message = "Не указан номер договора";
							MESSAGE_ERROR("", "", "sow.id is empty")
						}
					}
					else if(action == "AJAX_deleteBTExpenseApproverFromSoW")
					{
						if(sow_id.length())
						{
							int	affected = db->Query("SELECT `id` FROM `bt` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";");
							vector<string> bt_list;

							for(int i = 0; i < affected; ++i)
							{
								bt_list.push_back(db->Get(i, "id"));
							}

							for (auto &bt_id: bt_list)
							{
								if(SubmitBT(bt_id, db, user))
								{

								}
								else
								{
									MESSAGE_ERROR("", "", "fail to re-submit timecard.id(" + bt_id + ")");
								}
							}
							
						}
						else
						{
							error_message = "Не указан номер договора";
							MESSAGE_ERROR("", "", "sow.id is empty")
						}
					}
					else
					{
						error_message = "Неизвестное действие";
						MESSAGE_ERROR("", "", "unknown action (" + action + ")");
					}

			}
			else
			{
				error_message = "Новое значение некорректно";
				MESSAGE_ERROR("", "", "new_value is empty");
			}
		}
		else
		{
			error_message = "Неизвестное действие";
			MESSAGE_ERROR("", "", "action is empty");
		}
	}
	else
	{
		error_message = "Неизвестный идентификатор поля";
		MESSAGE_ERROR("", "", "id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}


string	SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				if(new_value.length())
				{
					if(error_message.empty())
					{
						string		sql_query = "";

						if(action == "AJAX_updateCustomerTitle") 					sql_query = "UPDATE `timecard_customers`	SET `title`				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateProjectTitle") 					sql_query = "UPDATE `timecard_projects`		SET `title`				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateTaskTitle") 						sql_query = "UPDATE `timecard_tasks`		SET `title`				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateTitle")				sql_query = "UPDATE `bt_expense_templates`	SET `title`				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateAgencyComment")		sql_query = "UPDATE `bt_expense_templates`	SET `agency_comment`	=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "UPDATE `bt_expense_line_templates`	SET `title`			=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "UPDATE `bt_expense_line_templates`	SET `description`	=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "UPDATE `bt_expense_line_templates`	SET `tooltip`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "UPDATE `bt_expense_line_templates`	SET `dom_type`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "UPDATE `bt_expense_line_templates`	SET `required`		=\"" + (new_value == "true" ? "Y"s : "N"s) + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePeriodStart") 				sql_query = "UPDATE `timecard_task_assignment`	SET `period_start`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\") WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePeriodEnd") 				sql_query = "UPDATE `timecard_task_assignment`	SET `period_end`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\") WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyTitle")				sql_query = "UPDATE	`company`					SET `name` 				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyDescription")		sql_query = "UPDATE	`company`					SET `description`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyWebSite")			sql_query = "UPDATE	`company`					SET `webSite`			=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyTIN")				sql_query = "UPDATE	`company`					SET `tin` 				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyAccount")			sql_query = "UPDATE	`company`					SET `account`			=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyOGRN")				sql_query = "UPDATE	`company`					SET `ogrn` 				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyKPP")				sql_query = "UPDATE	`company`					SET `kpp` 				=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyLegalAddress")		sql_query = "UPDATE	`company`					SET `legal_address`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyMailingAddress")	sql_query = "UPDATE	`company`					SET `mailing_address`	=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyMailingZipID")		sql_query = "UPDATE	`company`					SET `mailing_geo_zip_id`=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyLegalZipID")		sql_query = "UPDATE	`company`					SET `legal_geo_zip_id`	=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyBankID")			sql_query = "UPDATE	`company`					SET `bank_id`			=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSoWEditCapability")		sql_query = "UPDATE	`company_employees`			SET `allowed_change_sow`=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateAgencyEditCapability")		sql_query = "UPDATE	`company_employees`			SET `allowed_change_agency_data` =\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSubcontractorCreateTasks")	sql_query = "UPDATE	`contracts_sow`				SET `subcontractor_create_tasks` =\"" + new_value + "\" WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_addTimecardApproverToSoW")		sql_query = "INSERT INTO `timecard_approvers` (`approver_user_id`,`contract_sow_id`) VALUES (\"" + new_value + "\", \"" + sow_id + "\");";
						if(action == "AJAX_addBTExpenseApproverToSoW")				sql_query = "INSERT INTO `bt_approvers` (`approver_user_id`,`contract_sow_id`) VALUES (\"" + new_value + "\", \"" + sow_id + "\");";

						// --- expense line template payment part
						if(action == "AJAX_updateExpenseTemplateLinePaymentCash")
						{
							if(db->Query("SELECT `payment` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";"))
							{
								string	curr_payment = db->Get(0, "payment");
								auto	find_position = curr_payment.find("card");
								auto	card = (find_position != string::npos);
								auto	cash = (new_value == "true");
								
								if(cash && card) new_value = "cash and card";
								else if(card) new_value = "card";
								else if(cash) new_value = "cash";
								else
								{
									MESSAGE_ERROR("", "", "fail to set payment method cash/card(" + to_string(cash) + "/" + to_string(card) + ")");
								}
							}
							else
							{
								new_value = "cash and card";
								MESSAGE_ERROR("", "", "fail to take payment value from bt_expense_line_templates table");
							}
							sql_query = "UPDATE `bt_expense_line_templates`	SET `payment`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						}
						if(action == "AJAX_updateExpenseTemplateLinePaymentCard")			
						{
							if(db->Query("SELECT `payment` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";"))
							{
								string	curr_payment = db->Get(0, "payment");
								auto	find_position = curr_payment.find("cash");
								auto	cash = (find_position != string::npos);
								auto	card = (new_value == "true");
								
								if(cash && card) new_value = "cash and card";
								else if(card) new_value = "card";
								else if(cash) new_value = "cash";
								else
								{
									MESSAGE_ERROR("", "", "fail to set payment method cash/card(" + to_string(cash) + "/" + to_string(card) + ")");
								}
							}
							else
							{
								new_value = "cash and card";
								MESSAGE_ERROR("", "", "fail to take payment value from bt_expense_line_templates table");
							}
							sql_query = "UPDATE `bt_expense_line_templates`	SET `payment`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						}

						if(action == "AJAX_updateAgencyPosition")
						{
							string	position_id = "";
							if(db->Query("SELECT `id` FROM `company_position` WHERE `title`=\"" + new_value + "\";"))
							{
								position_id = db->Get(0, "id");
							}
							else
							{
								long int temp = db->InsertQuery("INSERT INTO `company_position` (`title`) VALUES (\"" + new_value + "\");");
								if(temp)
								{
									position_id = to_string(temp);
									MESSAGE_ERROR("", "", "new company_position.id(" + position_id + ") created. Check the title.");
								}
								else
								{
									position_id = "1";
									MESSAGE_ERROR("", "", "fail to insert to company_position table");
								}
							}

							sql_query = "UPDATE	`company_employees`	SET `position_id` =\"" + position_id + "\" WHERE `id`=\"" + id + "\";";
						}
						if(action == "AJAX_deleteTimecardApproverFromSoW")
						{

							db->Query("DELETE FROM `timecard_approvals` WHERE `approver_id`=\"" + id + "\";");
							sql_query =  "DELETE FROM `timecard_approvers` WHERE `id`=\"" + id + "\";";
						}
						if(action == "AJAX_deleteBTExpenseApproverFromSoW")
						{
							db->Query("DELETE FROM `bt_approvals` WHERE `approver_id`=\"" + id + "\";");
							sql_query =  "DELETE FROM `bt_approvers` WHERE `id`=\"" + id + "\";";
						}

						if(sql_query.length())
						{
							db->Query(sql_query);
							if(db->isError())
							{
								error_message = "Ошибка БД";
								MESSAGE_ERROR("", "", "sql_query has failed (" + db->GetErrorMessage() + ")");
							}
							else
							{
								MESSAGE_DEBUG("", "", "" + action + " with id(" + id + ") returned empty result");
							}
						}
						else
						{
							error_message = "Неизвестное действие";
							MESSAGE_ERROR("", "", "unknown action (" + action + ")");
						}
					}
				}
				else
				{
					error_message = "Новое значен некорректно";
					MESSAGE_ERROR("", "", "new_value is empty");
				}
			}
			else
			{
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Неизвестный идентификатор поля";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	GetSpelledEmployeeByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
			"SELECT"
				"`users`.`name` as `user_name`,"
				"`users`.`nameLast` as `user_nameLast`,"
				"`company_position`.`title` as `position_title`"
			"FROM `company_employees`"
			"INNER JOIN `users` ON `users`.`id`=`company_employees`.`user_id`"
			"INNER JOIN `company_position` ON `company_position`.`id`=`company_employees`.`position_id`"
			"WHERE `company_employees`.`id`=\"" + id + "\";"
		))	
	{
		result = string(db->Get(0, "user_name")) + " " + db->Get(0, "user_nameLast") + " (" + db->Get(0, "position_title") + ")";
	}
	else
	{
		MESSAGE_ERROR("", "", "customer_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardCustomerByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `timecard_customers` WHERE `id`=\"" + id + "\";"))	
	{
		result = db->Get(0, "title");
	}
	else
	{
		MESSAGE_ERROR("", "", "customer_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardProjectByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	customer_id = db->Get(0, "timecard_customers_id");

		result = GetSpelledTimecardCustomerByID(customer_id, db) + " / " + title;
	}
	else
	{
		MESSAGE_ERROR("", "", "project_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardTaskByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	project_id = db->Get(0, "timecard_projects_id");

		result = GetSpelledTimecardProjectByID(project_id, db) + " / " + title;
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledBTExpenseTemplateByID(string id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `agency_comment` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	agency_comment = db->Get(0, "agency_comment");

		result = title + " ("s + agency_comment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseTemplateByAssignmentID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `agency_comment` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\");"))
	{
		string 	title = db->Get(0, "title");
		string	agency_comment = db->Get(0, "agency_comment");

		result = title + " ("s + agency_comment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseTemplateByLineID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `agency_comment` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");"))
	{
		string 	title = db->Get(0, "title");
		string	agency_comment = db->Get(0, "agency_comment");

		result = title + " ("s + agency_comment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledCostCenterByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");

		result = title;
	}
	else
	{
		MESSAGE_ERROR("", "", "cost_centers.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseTemplateLineByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `dom_type`, `description`, `payment` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	description = db->Get(0, "description");
		string	dom_type = db->Get(0, "dom_type");
		string	payment = db->Get(0, "payment");

		result = title + " ("s + description + " / "s + dom_type + " / "s + payment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledTimecardTaskAssignmentByID(string id, CMysql *db) -> string
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `period_start`, `period_end`, `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";"))
	{
		struct tm	period_start = GetTMObject(db->Get(0, "period_start"));
		struct tm	period_end = GetTMObject(db->Get(0, "period_end"));
		string		task_id = db->Get(0, "timecard_tasks_id");

		result = GetSpelledTimecardTaskByID(task_id, db) + " c " + PrintTime(period_start, RU_DATE_FORMAT) + " по " + PrintTime(period_end, RU_DATE_FORMAT);
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id couldn't be found from task_assignment");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseAssignmentByID(string id, CMysql *db) -> string
{
	string	result = "";
	string	bt_expense_template_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";"))
	{
		string		bt_expense_template_id = db->Get(0, "bt_expense_template_id");

		result = GetSpelledBTExpenseTemplateByID(id, db);
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_template_id couldn't be found from task_assignment");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledUserNameByID(string id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `name`, `nameLast` FROM `users` WHERE `id`=\"" + id + "\";"))
	{
		result = string(db->Get(0, "name")) + " " + db->Get(0, "nameLast");
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardApproverNameByID(string id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `name`, `nameLast` FROM `users` WHERE `id`=( SELECT `approver_user_id` FROM `timecard_approvers` WHERE `id`=\"" + id + "\");"))
	{
		result = string(db->Get(0, "name")) + " " + db->Get(0, "nameLast");
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_approvers.id(" + id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledBTExpenseApproverNameByID(string id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `name`, `nameLast` FROM `users` WHERE `id`=( SELECT `approver_user_id` FROM `bt_approvers` WHERE `id`=\"" + id + "\");"))
	{
		result = string(db->Get(0, "name")) + " " + db->Get(0, "nameLast");
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_approvers.id(" + id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledSoWByID(string sow_id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `start_date`, `end_date`, `number` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
	{
		result = string(db->Get(0, "number")) + " с " + db->Get(0, "start_date") + " по " + db->Get(0, "end_date");
	}
	else
	{
		MESSAGE_ERROR("", "", "contract_sow.id(" + sow_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

static pair<string, string> GetNotificationDescriptionAndSoWQuery(string action, string id, string sow_id, string existing_value, string new_value, CMysql *db, CUser *user)
{
	string	notification_description, sql_query;

	MESSAGE_DEBUG("", "", "start");

	if(action == "AJAX_updateCustomerTitle") 	
	{
		notification_description = "Данные таймкарты: изменилось название заказчика " + existing_value + " -> " + new_value;
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
							"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\""
						")"
					");";
	}
	if(action == "AJAX_updateProjectTitle") 	
	{
		notification_description = "Данные таймкарты: изменилось название проекта " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardProjectByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\""
					");";
	}
	if(action == "AJAX_updateTaskTitle") 		
	{
		notification_description = "Данные таймкарты: изменилось название задачи " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardTaskByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + id + "\";";
	}
	if(action == "AJAX_addTaskAssignment")
	{
		notification_description = "Данные таймкарты: добавили назначение на задачу " + new_value + ".";
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_updatePeriodStart") 		
	{
		notification_description = "Данные таймкарты: изменилась дата начала работы над задачей " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardTaskAssignmentByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_updatePeriodEnd") 		
	{
		notification_description = "Данные таймкарты: изменилась дата окончания работы над задачей " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardTaskAssignmentByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_deleteTaskAssignment")
	{
		notification_description = "Данные таймкарты: удалено назначение на задачу " + GetSpelledTimecardTaskAssignmentByID(id, db) + ".";
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_deleteBTExpenseAssignment")
	{
		notification_description = "Данные командировки: удалено возмещение расхода " + GetSpelledBTExpenseTemplateByAssignmentID(id, db) + " из SoW(" + GetSpelledSoWByID(sow_id, db) + ").";
		sql_query = "SELECT DISTINCT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_addTask")
	{
		notification_description = "Данные таймкарты: добавили новую задачу (" + new_value + "). Полная информация: " + GetSpelledTimecardTaskByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteTask")
	{
		notification_description = "Данные таймкарты: удалили задачу " + GetSpelledTimecardTaskByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteProject")
	{
		notification_description = "Данные таймкарты: удалили проект " + GetSpelledTimecardProjectByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteCustomer")
	{
		notification_description = "Данные таймкарты: удалили заказчика " + GetSpelledTimecardCustomerByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteEmployee")
	{
		notification_description = "Данные компании: уволили " + GetSpelledEmployeeByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCompanyTitle")
	{
		notification_description = "Данные компании: Название компании " + existing_value + " изменилось на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyDescription")
	{
		notification_description = "Данные компании: Описание компании изменилось на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyWebSite")
	{
		notification_description = "Данные компании: Web-сайт компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyTIN")
	{
		notification_description = "Данные компании: ИНН компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyAccount")
	{
		notification_description = "Данные компании: Р/C компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyOGRN")
	{
		notification_description = "Данные компании: ОГРН компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyKPP")
	{
		notification_description = "Данные компании: КПП компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyLegalAddress")
	{
		notification_description = "Данные компании: Юр. адрес компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyMailingAddress")
	{
		notification_description = "Данные компании: Фактический адрес компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateAgencyPosition")
	{
		notification_description = "Данные сотрудника: " + GetSpelledEmployeeByID(id, db) + " перешел(шла) с должности (" + existing_value + ")";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateAgencyEditCapability")
	{
		notification_description = "Данные сотрудника: " + GetSpelledEmployeeByID(id, db) + (new_value == "N" ? " не" : "") + " может менять данные агенства";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateSoWEditCapability")
	{
		notification_description = "Данные сотрудника: " + GetSpelledEmployeeByID(id, db) + (new_value == "N" ? " не" : "") + " может менять SoW";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCompanyMailingZipID")
	{
		notification_description = "Данные компании: Изменился фактический адрес компании с " + GetSpelledZIPByID(existing_value, db) + " на " + GetSpelledZIPByID(new_value, db);
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyLegalZipID")
	{
		notification_description = "Данные компании: Изменился юридический адрес компании с " + GetSpelledZIPByID(existing_value, db) + " на " + GetSpelledZIPByID(new_value, db);
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyBankID")
	{
		notification_description = "Данные компании: Изменился банк компании с " + GetSpelledBankByID(existing_value, db) + " на " + GetSpelledBankByID(new_value, db);
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_addTimecardApproverToSoW")
	{
		notification_description = "Данные таймкарты: Добавили нового утвердителя " + GetSpelledUserNameByID(new_value, db) + "  (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_addBTExpenseApproverToSoW")
	{
		notification_description = "Данные командировки: Добавили нового утвердителя " + GetSpelledUserNameByID(new_value, db) + "  (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteTimecardApproverFromSoW")
	{
		notification_description = "Данные таймкарты: Удалили утвердителя таймкарт " + GetSpelledTimecardApproverNameByID(id, db) + "  (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteBTExpenseApproverFromSoW")
	{
		notification_description = "Данные командировки: Удалили утвердителя командировочных расходов " + GetSpelledBTExpenseApproverNameByID(id, db) + " (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSubcontractorCreateTasks")
	{
		notification_description = "Данные таймкарты: Субконтрактор " + (new_value == "N" ? string("не ") : string("")) + "может создавать задачи самостоятельно (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteExpenseTemplate")
	{
		notification_description = "Данные командировки: удалили возмещаемый расход " + GetSpelledBTExpenseTemplateByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteExpenseTemplateLine")
	{
		notification_description = "Данные командировки: удалили отчетный документ " + GetSpelledBTExpenseTemplateLineByID(id, db) + " из возмещаемого расхода "  + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addExpenseTemplate")
	{
		notification_description = "Данные командировки: добавили возмещаемый расход "  + GetSpelledBTExpenseTemplateByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addExpenseTemplateLine")
	{
		notification_description = "Данные командировки: добавили отчетный документ " + GetSpelledBTExpenseTemplateLineByID(id, db) + " к возмещаемому расходу "  + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addCostCenter")
	{
		notification_description = gettext("Agency: cost center added") + " "s + GetSpelledCostCenterByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}

	if(action == "AJAX_updateExpenseTemplateTitle")
	{
		notification_description = "Данные командировки: изменилось название расхода с " + existing_value + " на " + new_value;
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + id + "\";";
	}
	if(action == "AJAX_updateExpenseTemplateAgencyComment")
	{
		notification_description = "Данные командировки: изменилось описание расхода (" + GetSpelledBTExpenseTemplateByID(id, db) + ") с " + existing_value + " на " + new_value;
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + id + "\";";
	}
	if(action == "AJAX_updateExpenseTemplateLineTitle")
	{
		notification_description = "Данные командировки: изменилось название отчетного документа с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineDescription")
	{
		notification_description = "Данные командировки: изменилось описание отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineTooltip")
	{
		notification_description = "Данные командировки: изменилась подсказка у отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineDomType")
	{
		notification_description = "Данные командировки: изменился тип отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLinePaymentCash")
	{
		notification_description = "Данные командировки: изменилась оплата у отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLinePaymentCard")
	{
		notification_description = "Данные командировки: изменилась оплата у отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineRequired")
	{
		notification_description = "Данные командировки: изменилось требование к отчетному документу (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_addBTExpenseTemplateAssignment")
	{
		notification_description = "Данные командировки: добавили возмещаемый расход (" + GetSpelledBTExpenseTemplateByAssignmentID(id, db) + ") к SoW(" + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT DISTINCT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";";
	}

	MESSAGE_DEBUG("", "", "finish");

	return make_pair(notification_description, sql_query);
}

bool NotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *db, CUser *user)
{
	bool	result = false;

	MESSAGE_DEBUG("", "", "start (action=" + action + ")");

	if(id.length())
	{
		if(action.length())
		{
			string		actionTypeId;
			string		notification_description = "";
			string		sow_list_sql_query = "";
			string		agency_list_sql_query = "";

			// --- definition sow_list affected by action
			tie(notification_description, sow_list_sql_query) = GetNotificationDescriptionAndSoWQuery(action, id, sow_id, existing_value, new_value, db, user);
			
			if(notification_description.length())
			{
				vector<string>		sow_list, agency_list;
				int					affected = 0;

				if(sow_list_sql_query.length())
				{
					// --- collect list sow to notify
					affected = db->Query(sow_list_sql_query);

					for(int i = 0; i < affected; ++i) 
						sow_list.push_back(db->Get(i, "contract_sow_id"));

					if(sow_list.size())
					{
						agency_list_sql_query = "SELECT DISTINCT `agency_company_id` FROM `contracts_sow` WHERE `id` IN (" + join(sow_list) + ") AND `start_date`<=NOW() AND `end_date`>=NOW();";
					}
					else
					{
						MESSAGE_DEBUG("", "", "no active sow at the moment, nobody should be notified.");
					}
				}
				else
				{
					// --- don't notify subcontractors, only agency
					agency_list_sql_query = "SELECT `company_id` AS `agency_company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";";
					MESSAGE_DEBUG("", "", "no subcontractors needs to be notified");
				}

				if(agency_list_sql_query.length())
				{
					affected = db->Query(agency_list_sql_query);

					for(int i = 0; i < affected; ++i) 
						agency_list.push_back(db->Get(i, "agency_company_id"));

					// --- define agency
					// --- ERROR, if more than one agency. Mean that they have shared CU/proj/task - FORBIDDEN
					if((user->GetType() == "agency") && (agency_list.size() > 1))
					{
						MESSAGE_ERROR("", "", "Two or more agencies are sharing entities (either Customer/Project/Task or BTexpense/line templates) (" + action + ":" + id + ") - FORBIDDEN");
					}

					if(agency_list.size())
					{
						auto				initiator_company_id = ""s;
						vector<string>		persons_to_notify;
						string				notificator_company_id = "";

						if(user->GetType() == "agency")
						{
							actionTypeId = to_string(NOTIFICATION_GENERAL_FROM_COMPANY);
							if(db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";"))
							{
								notificator_company_id = db->Get(0, "company_id");
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to get company.id from `company_employees`.`user_id`(" + user->GetID() + ")");
							}
						}
						else if(user->GetType() == "subcontractor")
						{
							actionTypeId = to_string(NOTIFICATION_GENERAL_FROM_USER);
							if(db->Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user->GetID() + "\";"))
							{
								notificator_company_id = db->Get(0, "id");
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to get company.id from `company`.`admin_userID`(" + user->GetID() + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "fail to define notification initiator type(" + user->GetType() + ")");
						}

						if(notificator_company_id.length())
						{
							// --- gather agency employee list
							affected = db->Query("SELECT DISTINCT `user_id` FROM `company_employees` WHERE `company_id` IN (" + join(agency_list) + ");");
							for(int i = 0; i < affected; ++i)
								persons_to_notify.push_back(db->Get(i, "user_id"));
							
							// --- gather subcontractor list
							if(sow_list.size())
							{
								affected = db->Query("SELECT DISTINCT `admin_userID` FROM `company` WHERE `id` IN ("
												"SELECT DISTINCT `subcontractor_company_id` FROM `contracts_sow` WHERE `id` IN (" + join(sow_list) + ")"
											");");
								for(int i = 0; i < affected; ++i)
									persons_to_notify.push_back(db->Get(i, "admin_userID"));
							}
							else
							{
								MESSAGE_DEBUG("", "", "no subcontractors need to be notified");
							}

							if(persons_to_notify.size())
							{
								string		sql_query = "";

								for(auto &user_id : persons_to_notify)
								{
									if(sql_query.length()) sql_query += ",";
									sql_query += "(\"" + user_id + "\", \"" + notification_description + "\", \"" + actionTypeId + "\", \"" + notificator_company_id + "\", UNIX_TIMESTAMP())";
								}

								sql_query = "INSERT INTO `users_notification` (`userId`, `title`, `actionTypeId`, `actionId`, `eventTimestamp`) VALUES " + sql_query;

								if(db->InsertQuery(sql_query))
								{
									result = true;
								}
								else
								{
									MESSAGE_ERROR("", "", "fail to insert to notification.table");
								}
							}
							else
							{
								MESSAGE_DEBUG("", "", "users notification array is empty");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "notificator_company_id is empty");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "Company not found");
					}

				}
				else
				{
					MESSAGE_DEBUG("", "", "empty sql-query, nobody should be notified.");
				}

			}
			else
			{
				MESSAGE_ERROR("", "", "unknown action_type(" + action + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "action is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;	
}

string GetAgencyEmployeesInJSONFormat(string company_id, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected = 0;
	string	sql_query;

	MESSAGE_DEBUG("", "", "start");

	if(company_id.length())
	{
		sql_query = 
				"SELECT"
					"`company_employees`.`id` as `id`,"
					"`users`.`id` as `user_id`,"
					"`users`.`name` as `name`,"
					"`users`.`nameLast` as `nameLast`,"
					"`company_employees`.`allowed_change_agency_data` as `allowed_change_agency_data`,"
					"`company_employees`.`allowed_change_sow` as `allowed_change_sow`,"
					"`company_position`.`id` as `company_position_id`,"
					"`company_position`.`title` as `company_position_title`"
				"FROM `company_employees`"
				"INNER JOIN `users` ON `users`.`id`=`company_employees`.`user_id`"
				"INNER JOIN `company_position` ON `company_position`.`id`=`company_employees`.`position_id`"
				"WHERE `company_employees`.`company_id`=\"" + company_id + "\";";

		affected = db->Query(sql_query);
		if(affected)
		{
			for(int i = 0; i < affected; ++i)
			{
				if(i) result += ",";
				result += 
					"{"
						"\"id\":\"" + string(db->Get(i, "id")) + "\","
						"\"allowed_change_agency_data\":\"" + db->Get(i, "allowed_change_agency_data") + "\","
						"\"allowed_change_sow\":\"" + db->Get(i, "allowed_change_sow") + "\","
						"\"user\":{"
							"\"id\":\"" + db->Get(i, "user_id") + "\","
							"\"name\":\"" + db->Get(i, "name") + "\","
							"\"nameLast\":\"" + db->Get(i, "nameLast") + "\""
						"},"
						"\"position\":{"
							"\"id\":\"" + db->Get(i, "company_position_id") + "\","
							"\"title\":\"" + db->Get(i, "company_position_title") + "\""
						"}"
					"}"
				;
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "no agency employees found");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "comapny_id is empty");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string GetAgencyObjectInJSONFormat(string agency_id, bool include_tasks, bool include_bt, CMysql *db, CUser *user)
{
	string	result = "";
	string	sql_query;

	MESSAGE_DEBUG("", "", "start (agency_id=" + agency_id + ")");

	if(agency_id.length())
	{
		// --- get short info
		result += "{"
						"\"companies\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + agency_id + "\";", db, user) + "]"
						"}";
	}
	else
	{
		// --- get full info about agency, if you are employee
		agency_id = GetAgencyID(user, db);
		if(agency_id.length())
		{
			result += "{"
							"\"companies\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + agency_id + "\";", db, user, true, true) + "],"
							"\"tasks\":[" + ( include_tasks ?
												GetTimecardTasksInJSONFormat("SELECT * FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
																				"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id` IN ("
																					"SELECT `id` FROM `timecard_customers` WHERE `agency_company_id`=\"" + agency_id + "\""
																				")"
																			");", db, user) : "") + "],"
							"\"cost_centers\":[" + ( include_tasks ? GetCostCentersInJSONFormat("SELECT * FROM `cost_centers` WHERE `agency_company_id`=\"" + agency_id + "\";", db, user) : "") + "],"
							
							"\"bt_expense_templates\":[" + ( include_bt ? 
																			GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `agency_company_id`=\"" + agency_id + "\";", db, user)
																			 : "") + "]"
						"}";

		}
		else
		{
			MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is not an agency employee");
		}
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}
