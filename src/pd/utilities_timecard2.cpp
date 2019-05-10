#include "utilities_timecard.h"

auto	GetServiceBTInvoicesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	auto	affected = 0;
	auto	result = ""s;;

	struct ItemClass
	{
		string		id;
		string		cost_center_id;
		string		file;
		string		owner_user_id;
		string		owner_company_id;
		string		eventTimestamp;
	};
	vector<ItemClass>		itemsList;
	c_cache_obj				user_cache;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.cost_center_id = db->Get(i, "cost_center_id");
			item.file = db->Get(i, "file");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.owner_company_id = db->Get(i, "owner_company_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"cost_center_id\":\"" + item.cost_center_id + "\",";
			result += "\"file\":\"" + item.file + "\",";
			result += "\"owner_user_id\":\"" + item.owner_user_id + "\",";
			result += "\"owner_company_id\":\"" + item.owner_company_id + "\",";
			result += "\"users\":[" + user_cache.Get("SELECT * FROM `users` WHERE `id`=\"" + item.owner_user_id + "\";", db, user, GetUserListInJSONFormat) + "],";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no invoices submit to cost_center");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

auto	isServiceInvoiceBelongsToUser(string service_invoice_id, CMysql *db, CUser *user) -> bool
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `id` FROM `invoice_cost_center_service` WHERE `id`=\"" + service_invoice_id + "\" AND `owner_user_id`=\"" + user->GetID() + "\";"))
	{
		result = true;
	}
	else
	{
		MESSAGE_DEBUG("", "", "invoice_cost_center_service.id(" + service_invoice_id + ") doesn't belongs to user.id(" + user->GetID() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + (result ? "true" : "false") + ")");

	return result;
}

auto	isBTInvoiceBelongsToUser(string bt_invoice_id, CMysql *db, CUser *user) -> bool
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `id` FROM `invoice_cost_center_bt` WHERE `id`=\"" + bt_invoice_id + "\" AND `owner_user_id`=\"" + user->GetID() + "\";"))
	{
		result = true;
	}
	else
	{
		MESSAGE_DEBUG("", "", "invoice_cost_center_service.id(" + bt_invoice_id + ") doesn't belongs to user.id(" + user->GetID() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + (result ? "true" : "false") + ")");

	return result;
}

auto	RecallServiceInvoice(string service_invoice_id, CMysql *db, CUser *user) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(service_invoice_id.length())
	{
		if(db->Query("SELECT `file` FROM `invoice_cost_center_service` WHERE `id`=\"" + service_invoice_id + "\";"))
		{
			auto	file_name = db->Get(0, 0);

			db->Query("DELETE FROM `invoice_cost_center_service_details` WHERE `invoice_cost_center_service_id`=\"" + service_invoice_id + "\";");
			if(db->isError())
			{
				MESSAGE_ERROR("", "", "fail to delete from invoice_cost_center_service_details")
				error_message = gettext("SQL syntax issue");
			}
			else
			{
				db->Query("DELETE FROM `invoice_cost_center_service` WHERE `id`=\"" + service_invoice_id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to delete from invoice_cost_center_service")
					error_message = gettext("SQL syntax issue");
				}
				else
				{
					if(file_name.length())
					{
						unlink((INVOICES_CC_DIRECTORY + file_name).c_str());
					}
					else
					{
						MESSAGE_ERROR("", "", "invoice file doesn't exists");
					}
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "invoice_cost_center_service(" + service_invoice_id + ") not found");
			error_message = gettext("unknown invoice");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "invoice_cost_center_service is empty");
		error_message = gettext("unknown invoice");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	RecallBTInvoice(string bt_invoice_id, CMysql *db, CUser *user) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(bt_invoice_id.length())
	{
		if(db->Query("SELECT `file` FROM `invoice_cost_center_bt` WHERE `id`=\"" + bt_invoice_id + "\";"))
		{
			auto	file_name = db->Get(0, 0);

			db->Query("DELETE FROM `invoice_cost_center_bt_details` WHERE `invoice_cost_center_bt_id`=\"" + bt_invoice_id + "\";");
			if(db->isError())
			{
				MESSAGE_ERROR("", "", "fail to delete from invoice_cost_center_bt_details")
				error_message = gettext("SQL syntax issue");
			}
			else
			{
				db->Query("DELETE FROM `invoice_cost_center_bt` WHERE `id`=\"" + bt_invoice_id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to delete from invoice_cost_center_bt")
					error_message = gettext("SQL syntax issue");
				}
				else
				{
					if(file_name.length())
					{
						unlink((INVOICES_CC_DIRECTORY + file_name).c_str());
					}
					else
					{
						MESSAGE_ERROR("", "", "invoice file doesn't exists");
					}
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "invoice_cost_center_bt(" + bt_invoice_id + ") not found");
			error_message = gettext("unknown invoice");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "invoice_cost_center_bt is empty");
		error_message = gettext("unknown invoice");
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isServiceInvoiceBelongsToAgency(string service_invoice_id, CMysql *db, CUser *user) -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		if(service_invoice_id.length())
		{
			if(db->Query("SELECT `id` FROM `invoice_cost_center_service` WHERE `id`=\"" + service_invoice_id + "\" AND `owner_company_id`=("
							"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
								"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
							")"
						");"))
			{
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "service_invoice_id(" + service_invoice_id + ") doesn't belongs to company user.id(" + user->GetID() + ") employeed");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "service_invoice_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}
	
	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto isBTInvoiceBelongsToAgency(string bt_invoice_id, CMysql *db, CUser *user) -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		if(bt_invoice_id.length())
		{
			if(db->Query("SELECT `id` FROM `invoice_cost_center_bt` WHERE `id`=\"" + bt_invoice_id + "\" AND `owner_company_id`=("
							"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
								"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
							")"
						");"))
			{
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "bt_invoice_id(" + bt_invoice_id + ") doesn't belongs to company user.id(" + user->GetID() + ") employeed");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "bt_invoice_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}
	
	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto GetAgencyIDByUserID(CMysql *db, CUser *user) -> string
{
	auto result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		if(db->Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
						"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
						")"))
		{
			result = db->Get(0, "id");
		}
		else
		{
			MESSAGE_DEBUG("", "", "agency employeed user.id(" + user->GetID() + ") not found");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

auto	CreatePSoWfromTimecardCustomerIDAndCostCenterID(string timecard_customer_id, string cost_center_id, CMysql *db, CUser *user) -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		// --- get SoW by CustomerID
		map<string, string> sow_to_positions;
		auto number_of_sow = db->Query(	"SELECT `id`, `company_position_id` FROM `contracts_sow` WHERE `id` IN ("
											"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
												"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
													"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + timecard_customer_id + "\""
												")"
											")"
										")");

		for(auto i = 0; i < number_of_sow; ++i)
			sow_to_positions[db->Get(i, "id")] = db->Get(i, "company_position_id");

		for(const auto &sow_to_position: sow_to_positions)
		{
			auto	sow_id = sow_to_position.first;
			auto	company_position_id = sow_to_position.second;

			if(db->Query("SELECT `id` FROM `contracts_psow` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `cost_center_id`=\"" + cost_center_id + "\";"))
			{
				auto	psow_id = db->Get(0, "id");
				MESSAGE_DEBUG("", "", "psow.id(" + psow_id + ") already exists with cost_center.id(" + cost_center_id + ") and sow.id(" + sow_id + ")");
			}
			else
			{
				auto	new_psow_id = db->InsertQuery("INSERT INTO `contracts_psow` (`contract_sow_id`, `cost_center_id`, `company_position_id`, `eventTimestamp`) VALUES ("
														+ quoted(sow_id) + ","
														+ quoted(cost_center_id) + ","
														+ quoted(company_position_id) + ","
														+ "UNIX_TIMESTAMP()"
														+ ")");
				if(new_psow_id)
				{
					result = true;
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to insert to contract_psow table");
				}
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto	CreatePSoWfromTimecardTaskIDAndSoWID(string timecard_task_id, string sow_id, CMysql *db, CUser *user) -> bool
{
	MESSAGE_DEBUG("", "", "start");

	auto	customer_id_project_id = GetCustomerIDProjectIDByTaskID(timecard_task_id, db);
	auto	result = false;

	if(customer_id_project_id.first.length())
	{
		result = CreatePSoWfromTimecardCustomerIDAndSoWID(customer_id_project_id.first, sow_id, db, user);
	}
	else
	{
		MESSAGE_ERROR("", "", "cant find project.id by task.id(" + timecard_task_id + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");


	return result;
}

auto	CreatePSoWfromTimecardCustomerIDAndSoWID(string timecard_customer_id, string sow_id, CMysql *db, CUser *user) -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		auto	cost_center_id = GetCostCenterIDByCustomerID(timecard_customer_id, db);

		if(cost_center_id.length())
		{
			// --- if psow already exists
			if(db->Query("SELECT `id` FROM `contracts_psow` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `cost_center_id`=\"" + cost_center_id + "\";"))
			{
				MESSAGE_DEBUG("", "", "psow.id(" + db->Get(0, 0) + ") already exists with sow.id(" + sow_id + ") and cost_center_id(" + cost_center_id + ") assigned");
				result = true;
			}
			else
			{
				auto	company_position_id = GetPositionIDFromSoW(sow_id, db);

				if(company_position_id.length())
				{
					auto	new_psow_id = db->InsertQuery("INSERT INTO `contracts_psow` (`contract_sow_id`, `cost_center_id`, `company_position_id`, `start_date`, `end_date`, `sign_date`, `eventTimestamp`) VALUES ("
															+ quoted(sow_id) + ","
															+ quoted(cost_center_id) + ","
															+ quoted(company_position_id) + ","
															+ "NOW(),"
															+ "DATE_SUB(DATE_ADD(NOW(), INTERVAL 1 YEAR), INTERVAL 1 DAY),"
															+ "NOW(),"
															+ "UNIX_TIMESTAMP()"
															+ ")");
					if(new_psow_id)
					{
						result = true;
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to insert to contract_psow table");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "company_position_id not found.");
				}
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "cost_center_id not found.");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto GetCostCenterIDByCustomerID(string customer_id, CMysql *db) -> string
{
	auto cost_center_id = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `cost_center_id` FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + customer_id + "\";"))
	{
		cost_center_id = db->Get(0, 0);
	}
	else
	{
		MESSAGE_DEBUG("", "", "cost_center.id by timecard_customer_id(" + customer_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (cost_center_id: " + cost_center_id + ")");

	return cost_center_id;
}

auto GetPositionIDFromSoW(string sow_id, CMysql *db) -> string
{
	auto position_id = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `company_position_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
	{
		position_id = db->Get(0, 0);
	}
	else
	{
		MESSAGE_ERROR("", "", "sow.id(" + sow_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (position_id: " + position_id + ")");

	return position_id;
}

string isTimecardsHavePSOWAssigned(const vector<string> &timecard_list, string cost_center_id, CMysql *db, CUser *user)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");
	
	if(db)
	{
		for(auto &timecard_id: timecard_list)
		{
			auto	psow_id = GetPSoWIDByTimecardIDAndCostCenterID(timecard_id, cost_center_id, db, user);

			if(psow_id.empty())
			{
				MESSAGE_ERROR("", "", "timecard.id(" + timecard_id + ") have no PSoW associated");
				error_message = gettext("Some timecards have no PSoW associated");
				break;
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db is NULL");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length = " + to_string(error_message.length()) + ")");

	return error_message;
}

string isBTsHavePSOWAssigned(const vector<string> &bt_list, string cost_center_id, CMysql *db, CUser *user)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");
	
	if(db)
	{
		for(auto &bt_id: bt_list)
		{
			auto	psow_id = GetPSoWIDByBTIDAndCostCenterID(bt_id, cost_center_id, db, user);

			if(psow_id.empty())
			{
				MESSAGE_ERROR("", "", "bt.id(" + bt_id + ") have no PSoW associated");
				error_message = gettext("Some timecards have no PSoW associated");
				break;
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db is NULL");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length = " + to_string(error_message.length()) + ")");

	return error_message;
}

string isTimecardsBelongToAgency(const vector<string> &timecard_list, string agency_id, CMysql *db, CUser *user)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");
	
	if(db)
	{
		for(auto &timecard_id: timecard_list)
		{
			if(db->Query("SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=(SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\");"))
			{
				if(agency_id == db->Get(0, "agency_company_id"))
				{

				}
				else
				{
					MESSAGE_ERROR("", "", "timecard.id(" + timecard_id + ") doesn't belongs to agency.id(" + agency_id + ")");
					error_message = gettext("Timecard doesn't belongs to agency");
					break;
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "timecard.id(" + timecard_id + ") doesn't exists");
				error_message = gettext("Timecard not found");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db is NULL");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length = " + to_string(error_message.length()) + ")");

	return error_message;
}

string isBTsBelongToAgency(const vector<string> &bt_list, string agency_id, CMysql *db, CUser *user)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");
	
	if(db)
	{
		for(auto &bt_id: bt_list)
		{
			if(db->Query("SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=(SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\");"))
			{
				if(agency_id == db->Get(0, "agency_company_id"))
				{

				}
				else
				{
					MESSAGE_ERROR("", "", "bt.id(" + bt_id + ") doesn't belongs to agency.id(" + agency_id + ")");
					error_message = gettext("BT doesn't belongs to agency");
					break;
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "bt.id(" + bt_id + ") doesn't exists");
				error_message = gettext("BT not found");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db is NULL");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length = " + to_string(error_message.length()) + ")");

	return error_message;
}

string isAgencyEmployeeAllowedToChangeSoW(string sow_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(user->GetType() == "agency")
		{
			if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\" AND `agency_company_id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_sow`=\"Y\");"))
			{
				// --- everything is good
			}
			else
			{
				MESSAGE_DEBUG("", "", "user(" + user->GetID() + ") have not rights to change SoW(" + sow_id + ")");
				error_message = gettext("You are not authorized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") is not an agency employee");
			error_message = gettext("You are not agency employee");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string isAgencyEmployeeAllowedToChangePSoW(string sow_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(user->GetType() == "agency")
		{
			if(db->Query(	"SELECT `id` FROM `contracts_sow` WHERE `id`=("
								"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\""
							") AND `agency_company_id`=("
								"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_sow`=\"Y\""
							")"
							";"))
			{
				// --- everything is good
			}
			else
			{
				MESSAGE_DEBUG("", "", "user(" + user->GetID() + ") have not rights to change SoW(" + sow_id + ")");
				error_message = gettext("You are not authorized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") is not an agency employee");
			error_message = gettext("You are not agency employee");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string isAgencyEmployeeAllowedToChangeAgencyData(CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(user && db)
	{
		if(user->GetType() == "agency")
		{
			if(db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_agency_data`=\"Y\";"))
			{
				// --- everything is good
			}
			else
			{
				MESSAGE_DEBUG("", "", "user(" + user->GetID() + ") have no rights to change agency data");
				error_message = gettext("You are not authorized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") is not an agency employee");
			error_message = gettext("You are not agency employee");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user is NULL");
		error_message = gettext("Data has not been initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

struct tm GetFirstDayReportedOnAssignmentID(string sow_id, string task_id, CMysql *db)
{
	struct tm	result;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
			"SELECT `row`, `timecards`.`period_start` as `period_start` FROM `timecard_lines`"
			"INNER JOIN  `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id`"
			"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`contract_sow_id` = \"" + sow_id + "\" "
			"ORDER BY `timecards`.`period_start` ASC LIMIT 0, 1"
		))
	{
		int			first_reporting_day, last_reporting_day;
		string		row = db->Get(0, "row");

		result = GetTMObject(db->Get(0, "period_start"));
		tie(first_reporting_day, last_reporting_day) = FirstAndLastReportingDaysFromTimereport(SplitTimeentry(row));
		result.tm_mday += first_reporting_day;

	}
	else
	{
		result = GetTMObject("3333-01-01");

		MESSAGE_DEBUG("", "", "no time reported on sow_id/task_id(" + sow_id + "/" + task_id + "), assign fake date further in future");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + PrintTime(result, RU_DATE_FORMAT) + ")");


	return result;	
}

struct tm GetLastDayReportedOnAssignmentID(string sow_id, string task_id, CMysql *db)
{
	struct tm	result;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
			"SELECT `row`, `timecards`.`period_end` as `period_end` FROM `timecard_lines`"
			"INNER JOIN  `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id`"
			"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`contract_sow_id` = \"" + sow_id + "\" "
			"ORDER BY `timecards`.`period_end` DESC LIMIT 0, 1"
		))
	{
		int			first_reporting_day, last_reporting_day;
		string		row = db->Get(0, "row");

		result = GetTMObject(db->Get(0, "period_end"));
		tie(first_reporting_day, last_reporting_day) = FirstAndLastReportingDaysFromTimereport(SplitTimeentry(row));
		result.tm_mday -= last_reporting_day;
	}
	else
	{
		result = GetTMObject("1900-01-01");

		MESSAGE_DEBUG("", "", "no time reported on sow_id/task_id(" + sow_id + "/" + task_id + "), assign fake date further in future");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + PrintTime(result, RU_DATE_FORMAT) + ")");


	return result;	
}

auto	isActionEntityBelongsToSoW(string action, string id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(id.length())
		{
			if(action.length())
			{

				auto		sql_query = ""s;

				if(action == "AJAX_updateCustomerTitle") 			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN (SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN (SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\"));";
				if(action == "AJAX_updateProjectTitle") 			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN (SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\");";
				if(action == "AJAX_updateTaskTitle") 				sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodStart") 				sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodEnd") 				sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteTimecardApproverFromSoW")	sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_approvers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteBTExpenseApproverFromSoW")	sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `bt_approvers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSubcontractorCreateTasks")	sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true

				if(action == "AJAX_updateSoWNumber")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWAct")					sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWPosition")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWDayRate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWSignDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWStartDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWEndDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWCustomField")			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updatePSoWNumber")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWAct")					sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWPosition")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWDayRate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWBTMarkup")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWBTMarkupType")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWSignDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWStartDate")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWEndDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWCustomField")			sql_query = "SELECT `contract_psow_id` AS `sow_id` FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateCostCenterNumber")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterAct")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterSignDate")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterStartDate")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterEndDate")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterCustomField")	sql_query = "SELECT `cost_center_id` AS `sow_id` FROM `cost_center_custom_fields` WHERE `id`=\"" + id + "\";";

				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						string	sow_id_from_db = db->Get(0, "sow_id");

						if(sow_id == sow_id_from_db)
						{
							// --- everything is good
						}
						else
						{
							error_message = "Нет доступа";
							MESSAGE_ERROR("", "", "Probably hacker action(" + action + "), because action(" + action + ") entity.id(" + id + ") doesn't belong to sow.id(" + sow_id + ")");
						}
					}
					else
					{
						error_message = "Нет доступа";
						MESSAGE_ERROR("", "", "action(" + action + ") element.id(" + id + ") doesn't belong to sow.id(" + sow_id + ")");
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
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Идентификатор поля не задан";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}
	else
	{
		error_message = "Неизвестный договор";
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	CheckNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
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
					// --- assignments can start / stop beyond SoW period, 
					if(action == "AJAX_updatePeriodStart")
					{
						if(sow_id.length())
						{
							string	assignment_id = id;

							// --- find existing period_start and period_end
							if(error_message.empty())
							{
								if(db->Query("SELECT `period_start`, `period_end`,`timecard_tasks_id` FROM `timecard_task_assignment` WHERE `id`=\"" + assignment_id + "\";"))
								{
									string		task_id = db->Get(0, "timecard_tasks_id");
									struct tm	existing_assignment_start	= GetTMObject(db->Get(0, "period_start"));
									struct tm	existing_assignment_end		= GetTMObject(db->Get(0, "period_end"));
									struct tm	new_assignment_start		= GetTMObject(new_value);

									if(new_assignment_start <= existing_assignment_end)
									{
										if(new_assignment_start <= existing_assignment_start)
										{
											// --- start date can be moved earlier
										}
										else
										{
											struct tm	first_reporting_day	= GetFirstDayReportedOnAssignmentID(sow_id, task_id, db);

											if(new_assignment_start <= first_reporting_day)
											{
												// --- start date can be moved earlier
											}
											else
											{
												error_message = "Часы уже были заполнены на " + PrintTime(first_reporting_day, RU_DATE_FORMAT) + " дату.";
												MESSAGE_DEBUG("", "", "assignment.id(" + assignment_id + ") has first reported day on " + PrintTime(first_reporting_day, RU_DATE_FORMAT) + ". Start can't be later than this date.");
											}
										}
									}
									else
									{
										error_message = "Начало должно быть раньше окончания";
										MESSAGE_DEBUG("", "", "timecard_task_assignment.id(" + id + ") requsted start(" + PrintTime(new_assignment_start, RU_DATE_FORMAT) + ") must be earlier than existing finish(" + PrintTime(existing_assignment_end, RU_DATE_FORMAT) + ").")
									}
								}
								else
								{
									error_message = "Назначение на задачу не найдено в БД";
									MESSAGE_ERROR("", "", "timecard_task_assignment.id(" + id + ") not found")
								}
							}

							// --- compare new date against SoW start
							if(error_message.empty())
							{
								if(db->Query("SELECT `start_date`, `end_date` FROM `contracts_sow` WHERE `id`=("
												"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + assignment_id + "\""
											");"))
								{
									struct tm	sow_start					= GetTMObject(db->Get(0, "start_date"));
									struct tm	sow_end						= GetTMObject(db->Get(0, "end_date"));
									struct tm	new_assignment_date			= GetTMObject(new_value);

									if((sow_start <= new_assignment_date) && (new_assignment_date <= sow_end))
									{
										// --- good2go
									}
									else
									{
										error_message = gettext("Assignment must be inside SoW") + " ("s + PrintTime(sow_start, RU_DATE_FORMAT) + " - " + PrintTime(sow_end, RU_DATE_FORMAT) + ")";
										MESSAGE_DEBUG("", "", "timecard_task_assignment.id(" + id + ") requsted start(" + PrintTime(new_assignment_date, RU_DATE_FORMAT) + ")." + error_message);
									}
								}
								else
								{
									error_message = "Назначение на задачу не найдено в БД";
									MESSAGE_ERROR("", "", "timecard_task_assignment.id(" + id + ") not found")
								}
							}


						}
						else
						{
							error_message = "Неизвестный договор";
							MESSAGE_ERROR("", "", "sow_id is empty");
						}
					}
					else if(action == "AJAX_updatePeriodEnd")
					{
						if(sow_id.length())
						{
							string	assignment_id = id;

							// --- find existing period_start and period_end
							if(error_message.empty())
							{
								if(db->Query("SELECT `period_start`, `period_end`,`timecard_tasks_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";"))
								{
									string		task_id = db->Get(0, "timecard_tasks_id");
									struct tm	existing_assignment_start	= GetTMObject(db->Get(0, "period_start"));
									struct tm	existing_assignment_end 	= GetTMObject(db->Get(0, "period_end"));
									struct tm	new_assignment_end			= GetTMObject(new_value);

									if(existing_assignment_start <= new_assignment_end)
									{
										if(existing_assignment_end <= new_assignment_end)
										{
											// --- start date can be moved earlier
										}
										else
										{
											struct tm	last_reporting_day	= GetLastDayReportedOnAssignmentID(sow_id, task_id, db);

											if(last_reporting_day <= new_assignment_end)
											{
												// --- start date can be moved earlier
											}
											else
											{
												error_message = "Часы уже были заполнены на " + PrintTime(last_reporting_day, RU_DATE_FORMAT) + " дату.";
												MESSAGE_DEBUG("", "", "assignment.id(" + assignment_id + ") has last reported day on " + PrintTime(last_reporting_day, RU_DATE_FORMAT) + ". Finish can't be earlier than this date.");
											}
										}
									}
									else
									{
										error_message = "Окончание должно быть позднее начала";
										MESSAGE_DEBUG("", "", "timecard_task_assignment.id(" + id + ") requsted finish(" + PrintTime(new_assignment_end, RU_DATE_FORMAT) + ") must be later than existing start(" + PrintTime(existing_assignment_start, RU_DATE_FORMAT) + ").")
									}
								}
								else
								{
									error_message = "Назначение на задачу не найдено в БД";
									MESSAGE_ERROR("", "", "timecard_task_assignment.id(" + id + ") not found");
								}
							}

							// --- compare new date against SoW start
							if(error_message.empty())
							{
								if(db->Query("SELECT `start_date`, `end_date` FROM `contracts_sow` WHERE `id`=("
												"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + assignment_id + "\""
											");"))
								{
									struct tm	sow_start					= GetTMObject(db->Get(0, "start_date"));
									struct tm	sow_end						= GetTMObject(db->Get(0, "end_date"));
									struct tm	new_assignment_date			= GetTMObject(new_value);

									if((sow_start <= new_assignment_date) && (new_assignment_date <= sow_end))
									{
										// --- good2go
									}
									else
									{
										error_message = gettext("Assignment must be inside SoW") + " ("s + PrintTime(sow_start, RU_DATE_FORMAT) + " - " + PrintTime(sow_end, RU_DATE_FORMAT) + ")";
										MESSAGE_DEBUG("", "", "timecard_task_assignment.id(" + id + ") requsted start(" + PrintTime(new_assignment_date, RU_DATE_FORMAT) + ")." + error_message);
									}
								}
								else
								{
									error_message = "Назначение на задачу не найдено в БД";
									MESSAGE_ERROR("", "", "timecard_task_assignment.id(" + id + ") not found")
								}
							}
			

						}
						else
						{
							error_message = "Неизвестный договор";
							MESSAGE_ERROR("", "", "sow_id is empty");
						}
					}
					else if(action == "AJAX_updateCustomerTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + new_value + "\" AND `agency_company_id` = ( "
								"SELECT `agency_company_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Заказчик " + new_value + " уже существует";
							MESSAGE_DEBUG("", "", "Customer with new_value already exists");
						}
					}
					else if(action == "AJAX_updateProjectTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + new_value + "\" AND `timecard_customers_id` = ( "
								"SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\""
							");"
							))
						{
							error_message = "Проект " + new_value + " уже есть у этого Заказчика";
							MESSAGE_DEBUG("", "", "Project with new_value already assigned to the same Customer");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateTaskTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + new_value + "\" AND `timecard_projects_id` = ( "
								"SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\""
							");"
							))
						{
							error_message = "Задача " + new_value + " уже существует в этом проекте";
							MESSAGE_DEBUG("", "", "Customer with new_value already assigned to the same Customer/Project");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateAgencyEditCapability")
					{
						if(db->Query(
							"SELECT `id` FROM `company` WHERE `admin_userID`=("
								"SELECT `user_id` FROM `company_employees` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Нельзя запретить редактирование компании ее владельцу";
							MESSAGE_DEBUG("", "", "employee.id(" + id + ") is company owner, he should be able edit company details any time");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateSoWEditCapability")			{ /* --- good to go */ }
					else if(action == "AJAX_updateAgencyPosition")				{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWPosition")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyTitle")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyDescription")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyWebSite")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyTIN")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyVAT")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyAccount")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyOGRN")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyKPP")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyLegalAddress")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyMailingAddress")		{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyMailingZipID")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyLegalZipID")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyBankID")				{ /* --- good to go */ }
					else if(action == "AJAX_updateSubcontractorCreateTasks")	{ /* --- good to go */ }
					else if(action == "AJAX_deleteCostCenterFromCustomer")		{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterTitle")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterDescription")		{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWAct")						{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWSignDate")					{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_deleteTemplateAgreement_company")	{ /* --- good to go */ }
					else if(action == "AJAX_deleteTemplateAgreement_sow")		{ /* --- good to go */ }
					else if(action == "AJAX_updateTemplateAgreement_company_Title")
					{
						// if(db->Query("SELECT `id` FROM `company_agreement_files` WHERE `title`=\"" + new_value + "\" AND `company_id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\");"))
						{
							if((error_message = CheckAgreementSoWTitle(new_value, sow_id, db, user)).empty()) {}
							else
							{
								MESSAGE_DEBUG("", "", "error returned from CheckAgreementSoWTitle");
							}
						}
					}
					else if(action == "AJAX_updateTemplateAgreement_sow_Title")
					{
						// if(db->Query("SELECT `id` FROM `contract_sow_agreement_files` WHERE `title`=\"" + new_value + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
						{
							if((error_message = CheckAgreementSoWTitle(new_value, sow_id, db, user)).empty()) {}
							else
							{
								MESSAGE_DEBUG("", "", "error returned from CheckAgreementSoWTitle");
							}
						}
					}
					else if(action == "AJAX_updateSoWStartDate")
					{
						if(db->Query("SELECT `end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = new_value;
							auto		string_end = db->Get(0, "end_date");
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							if(tm_start <= tm_end)
							{
								if(db->Query("SELECT `period_start`, `period_end` FROM `timecards` where `contract_sow_id`=\"" + sow_id + "\" ORDER BY `period_start` ASC LIMIT 0,1;"))
								{
									auto	string_timecard_report_date = db->Get(0, "period_start");
									auto	tm_timecard_report_date = GetTMObject(string_timecard_report_date);

									if(tm_start <= tm_timecard_report_date)
									{
										// --- good to go
									}
									else
									{
										error_message = gettext("Subcontractor reported time on") + " "s + PrintDate(tm_timecard_report_date);
										MESSAGE_DEBUG("", "", "Subcontractor reported time on " + PrintDate(tm_timecard_report_date) + " earlier than SoW start " + PrintDate(tm_start));
									}
								}
								else
								{
									// --- good to go
									MESSAGE_DEBUG("", "", "No timecards subcontractor reported time on this SoW start ");
								}
							}
							else
							{
								error_message = gettext("period start have to precede period end") + " ("s + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateSoWEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = db->Get(0, "start_date");
							auto		string_end = new_value;
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							mktime(&tm_start);
							mktime(&tm_end);

							if(tm_start <= tm_end)
							{
								if(db->Query("SELECT `period_start`, `period_end` FROM `timecards` where `contract_sow_id`=\"" + sow_id + "\" ORDER BY `period_end` DESC LIMIT 0,1;"))
								{
									auto	string_timecard_report_date = db->Get(0, "period_end");
									auto	tm_timecard_report_date = GetTMObject(string_timecard_report_date);

									if(tm_timecard_report_date <= tm_end)
									{
										// --- good to go
									}
									else
									{
										error_message = gettext("Subcontractor reported time on") + " "s + PrintDate(tm_timecard_report_date);
										MESSAGE_DEBUG("", "", "Subcontractor reported time on " + PrintDate(tm_timecard_report_date) + " earlier than SoW start " + PrintDate(tm_start));
									}
								}
								else
								{
									// --- good to go
									MESSAGE_DEBUG("", "", "No timecards subcontractor reported time on this SoW start ");
								}
							}
							else
							{
								error_message = gettext("period start have to precede period end") + " ("s + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateSoWDayRate")
					{
						c_float		num(new_value);

						if(string(num) == new_value) { /* --- good to go */ }
						else
						{
							MESSAGE_ERROR("", "", "input DayRate(" + new_value + ") wrongly formatted, needed to be " + string(num));
						}
					}
					else if(action == "AJAX_deleteCostCenter")
					{
						if(db->Query("SELECT COUNT(*) AS `counter` FROM `cost_center_assignment` WHERE `cost_center_id`=\"" + id + "\";"))
						{
							auto		counter = stoi(db->Get(0, "counter"));

							if(counter)
							{
								char	buffer[50];
								sprintf(buffer, ngettext("%d customers", "%d customers", counter), counter);

								error_message = gettext("cost center assigned") + " "s + (buffer) + ". " +  gettext("removal prohibited");
								MESSAGE_DEBUG("", "", "cost_center.id(" + id + ") assigned to " + to_string(counter) + " customers. Removal prohibited.");
							}
							else
							{
								// --- good to go
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}					
					else if(action == "AJAX_updateSoWNumber")
					{
						if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `number`=\"" + new_value + "\" AND `id`!=\"" + sow_id + "\" AND `agency_company_id`=(SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");"))
						{
							error_message = gettext("SoW number already exists");
							MESSAGE_DEBUG("", "", "sow.number already exists in agency.id");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_addTimecardApproverToSoW")
					{
						if(db->Query("SELECT `id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + new_value + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
						{
							error_message = "Уже в списке утвердителей";
							MESSAGE_DEBUG("", "", "user.id(" + new_value + ") already in sow.id(" + sow_id + ") approver list");
						}
						else
						{
							// --- good to go
						}
					}					
					else if(action == "AJAX_addCostCenter")
					{
						if(db->Query("SELECT `id` FROM `company` WHERE `id`=\"" + new_value + "\";"))
						{
							// --- good to go
						}
						else
						{
							error_message = gettext("company doesn't exists");
							MESSAGE_DEBUG("", "", "company.id(" + new_value + ") doesn't exists");
						}
					}
					else if(action == "AJAX_updateCostCenterToCustomer")
					{
						// --- check that assigning cost center belongs to the same agency as timecard_customer
						if(db->Query("SELECT `id` FROM `timecard_customers` WHERE `id`=\"" + id + "\" AND `agency_company_id`=(SELECT `agency_company_id` FROM `cost_centers` WHERE `id`=\"" + new_value + "\");"))
						{
							// --- good to go
						}
						else
						{
							error_message = gettext("cost center doesn't belongs to your company");
							MESSAGE_DEBUG("", "", "cost center doesn't belongs to your company");
						}
					}
					else if(action == "AJAX_addBTExpenseApproverToSoW")
					{
						if(db->Query("SELECT `id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + new_value + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
						{
							error_message = "Уже в списке утвердителей";
							MESSAGE_DEBUG("", "", "user.id(" + new_value + ") already in sow.id(" + sow_id + ") approver list");
						}
						else
						{
							// --- good to go
						}
					}					
					// --- Expense templates
					else if(action == "AJAX_updateExpenseTemplateTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `bt_expense_templates` WHERE `title`=\"" + new_value + "\" AND `agency_company_id` = ( "
								"SELECT `agency_company_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Расход "s + new_value + " уже существует";
							MESSAGE_DEBUG("", "", "BT expense template with same title already exists");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateTaxable")				{ /* --- good to go */ }
					else if(action == "AJAX_updateExpenseTemplateAgencyComment")		{ /* --- good to go */ }
					// --- Expense line templates
					else if(action == "AJAX_updateExpenseTemplateLineTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `bt_expense_line_templates` WHERE `title`=\"" + new_value + "\" AND `bt_expense_template_id` = ( "
								"SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Документ с таким названием (" + new_value + ") уже есть в расходе";
							MESSAGE_DEBUG("", "", "BT expense template line with same title already exists belongs to the same expense");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateLineDescription") 		{ /* --- good to go */ }
					else if(action == "AJAX_updateExpenseTemplateLineTooltip") 			{ /* --- good to go */ }
					else if(action == "AJAX_updateExpenseTemplateLineDomType") 
					{
						error_message = "Нельзя поменять тип документа";
						MESSAGE_DEBUG("", "", "dom_type can't be changed");
					}
					else if(action == "AJAX_updateExpenseTemplateLinePaymentCash") 
					{
						if(
							(new_value == "false")
							&&
							db->Query("SELECT `id` FROM `bt_expense_line_templates` WHERE `payment`=\"cash\" AND `id`=\"" + id + "\";")
						)
						{
							error_message = "Необходимо оставить какой-нибудь способ оплаты";
							MESSAGE_DEBUG("", "", "Fail to disable single methid of payment");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateLinePaymentCard")
					{
						if(
							(new_value == "false")
							&&
							db->Query("SELECT `id` FROM `bt_expense_line_templates` WHERE `payment`=\"card\" AND `id`=\"" + id + "\";")
						)
						{
							error_message = "Необходимо оставить какой-нибудь способ оплаты";
							MESSAGE_DEBUG("", "", "Fail to disable single methid of payment");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateLineRequired") 		{ /* --- good to go */ }

					// --- PSoW part
					else if(action == "AJAX_updatePSoWPosition")				{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWAct")						{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWSignDate")				{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWStartDate")
					{
						if(db->Query("SELECT `end_date` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = new_value;
							auto		string_end = db->Get(0, "end_date");
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = gettext("period start have to precede period end") + " ("s + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updatePSoWEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = db->Get(0, "start_date");
							auto		string_end = new_value;
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							mktime(&tm_start);
							mktime(&tm_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = gettext("period start have to precede period end") + " ("s + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updatePSoWDayRate")
					{
						c_float		num(new_value);

						if(string(num) == new_value)
						{
							/* --- good to go */
						}
						else
						{
							MESSAGE_ERROR("", "", "input DayRate(" + new_value + ") wrongly formatted, needed to be " + string(num));
						}
					}
					else if(action == "AJAX_updatePSoWBTMarkup")
					{
						c_float		num(new_value);

						if(string(num) == new_value)
						{
							/* --- good to go */
						}
						else
						{
							MESSAGE_ERROR("", "", "input bt markup(" + new_value + ") wrongly formatted, needed to be " + string(num));
						}
					}
					else if(action == "AJAX_updatePSoWBTMarkupType") { /* --- good to go */ }
					else if(action == "AJAX_updatePSoWNumber")
					{
						if(db->Query("SELECT `id` FROM `contracts_psow` WHERE `number`=\"" + new_value + "\" AND `id`!=\"" + sow_id + "\" AND `cost_center_id`=(SELECT `cost_center_id` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\");"))
						{
							error_message = gettext("PSoW number already exists");
							MESSAGE_DEBUG("", "", "psow.number already exists in agency.id");
						}
						else
						{
							// --- good to go
						}
					}

					// --- cost_center part
					else if(action == "AJAX_updateCostCenterAct")						{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterSignDate")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterNumber")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterStartDate")
					{
						if(db->Query("SELECT `end_date` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = new_value;
							auto		string_end = db->Get(0, "end_date");
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = gettext("period start have to precede period end") + " ("s + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateCostCenterEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = db->Get(0, "start_date");
							auto		string_end = new_value;
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							mktime(&tm_start);
							mktime(&tm_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = gettext("period start have to precede period end") + " ("s + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}

 					else
					{
						MESSAGE_ERROR("", "", "unknown action(" + action + "), no validity check performed.");
					}

				}
				else
				{
					error_message = "Новое значение не может быть пустым";
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

string	isActionEntityBelongsToAgency(string action, string id, string agency_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(agency_id.length())
	{
		if(id.length())
		{
			if(action.length())
			{
				string		sql_query = "";

				if(action == "AJAX_updateCustomerTitle") 				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateProjectTitle") 				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateTaskTitle") 					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=(SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\"));";

				if(action == "AJAX_updateExpenseTemplateTitle")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateTaxable")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateAgencyComment")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCash")	sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCard")	sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";

				if(action == "AJAX_updateCostCenterToCustomer")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteCostCenterFromCustomer")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_deleteCostCenter")						sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterTitle")					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterDescription")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";


				if(action == "AJAX_deleteTask")							sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=(SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\"));";
				if(action == "AJAX_deleteExpenseTemplate")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_addExpenseTemplateLine")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteExpenseTemplateLine")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_addBTExpenseTemplateAssignment")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_getCompanyInfoBySoWID")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteEmployee")						sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyPosition")				sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyEditCapability")			sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSoWEditCapability")			sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateTemplateAgreement_company_Title")	sql_query = "SELECT `company_id` AS `agency_id` FROM `company_agreement_files` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteTemplateAgreement_company")		sql_query = "SELECT `company_id` AS `agency_id` FROM `company_agreement_files` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateTemplateAgreement_sow_Title")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=(SELECT `contract_sow_id` FROM `contract_sow_agreement_files` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_deleteTemplateAgreement_sow")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=(SELECT `contract_sow_id` FROM `contract_sow_agreement_files` WHERE `id`=\"" + id + "\");";

				if(
					(action == "AJAX_updateCompanyTitle")			||
					(action == "AJAX_updateCompanyDescription")		||
					(action == "AJAX_updateCompanyWebSite")			||
					(action == "AJAX_updateCompanyTIN")				||
					(action == "AJAX_updateCompanyVAT")				||
					(action == "AJAX_updateCompanyAccount")			||
					(action == "AJAX_updateCompanyOGRN")			||
					(action == "AJAX_updateCompanyKPP")				||
					(action == "AJAX_updateCompanyLegalAddress")	||
					(action == "AJAX_updateCompanyMailingAddress")	||
					(action == "AJAX_updateCompanyMailingZipID")	||
					(action == "AJAX_updateCompanyLegalZipID")		||
					(action == "AJAX_updateCompanyBankID")
				)
				{
					sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE "
									"`user_id`=\"" + user->GetID() + "\""
									"AND"
									"("
										"`company_id` = ("
											"SELECT `company_id` FROM `company_employees` WHERE `user_id`= ("
												"SELECT `admin_userID` FROM `company` WHERE `id`=\"" + id + "\""
											")"
										")"
									")";
				}

				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						string	agency_id_from_db = db->Get(0, "agency_id");

						if(agency_id == agency_id_from_db)
						{
							// --- everything is good
						}
						else
						{
							error_message = "Нет доступа";
							MESSAGE_ERROR("", "", "Probably hacker action(" + action + "), because action(" + action + ") entity.id(" + id + ") doesn't belong to agency.id(" + agency_id + ")");
						}
					}
					else
					{
						error_message = "Нет доступа";
						MESSAGE_ERROR("", "", "action(" + action + ") element.id(" + id + ") doesn't belong to agency.id(" + agency_id + ")");
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
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Идентификатор поля не задан";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}
	else
	{
		error_message = "Неизвестное агенство";
		MESSAGE_ERROR("", "", "agency_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	GetCountryListInJSONFormat(string sqlQuery, CMysql *db, CUser *) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	auto affected = db->Query("SELECT * FROM `geo_country`;");
	if(affected)
	{
		for(auto i = 0; i < affected; ++i)
			result += (i ? ","s : ""s) + "{\"id\":\"" + db->Get(i, "id") + "\",\"title\":\"" + db->Get(i, "title") + "\"}";
	}
	else
	{
		MESSAGE_ERROR("", "", "country list is empty");
	}	

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

auto	GetCostCenterCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	cost_center_id;
		string	title;
		string	description;
		string	type;
		string	var_name;
		string	value;
		string	visible_by_subcontractor;
		string	editable_by_subcontractor;
		string	owner_user_id;
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
			item.cost_center_id = db->Get(i, "cost_center_id");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.type = db->Get(i, "type");
			item.var_name = db->Get(i, "var_name");
			item.value = db->Get(i, "value");
			item.visible_by_subcontractor = db->Get(i, "visible_by_subcontractor");
			item.editable_by_subcontractor = db->Get(i, "editable_by_subcontractor");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""							+ item.id + "\",";
			result += "\"cost_center_id\":\""				+ item.cost_center_id + "\",";
			result += "\"title\":\""						+ item.title + "\",";
			result += "\"description\":\""					+ item.description + "\",";
			result += "\"type\":\""							+ item.type + "\",";
			result += "\"var_name\":\""						+ item.var_name + "\",";
			result += "\"value\":\""						+ item.value + "\",";
			result += "\"visible_by_subcontractor\":\""		+ item.visible_by_subcontractor + "\",";
			result += "\"editable_by_subcontractor\":\""	+ item.editable_by_subcontractor + "\",";
			result += "\"owner_user_id\":\""				+ item.owner_user_id + "\",";
			result += "\"eventTimestamp\":\""				+ item.eventTimestamp + "\"";

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

auto	GetCompanyCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	company_id;
		string	title;
		string	description;
		string	type;
		string	var_name;
		string	value;
		string	visible_by_subcontractor;
		string	editable_by_subcontractor;
		string	owner_user_id;
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
			item.company_id = db->Get(i, "company_id");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.type = db->Get(i, "type");
			item.var_name = db->Get(i, "var_name");
			item.value = db->Get(i, "value");
			item.visible_by_subcontractor = db->Get(i, "visible_by_subcontractor");
			item.editable_by_subcontractor = db->Get(i, "editable_by_subcontractor");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""							+ item.id + "\",";
			result += "\"company_id\":\""				+ item.company_id + "\",";
			result += "\"title\":\""						+ item.title + "\",";
			result += "\"description\":\""					+ item.description + "\",";
			result += "\"type\":\""							+ item.type + "\",";
			result += "\"var_name\":\""						+ item.var_name + "\",";
			result += "\"value\":\""						+ item.value + "\",";
			result += "\"visible_by_subcontractor\":\""		+ item.visible_by_subcontractor + "\",";
			result += "\"editable_by_subcontractor\":\""	+ item.editable_by_subcontractor + "\",";
			result += "\"owner_user_id\":\""				+ item.owner_user_id + "\",";
			result += "\"eventTimestamp\":\""				+ item.eventTimestamp + "\"";

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

auto GetPositionByCompanyID(string company_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start (company.id = " + company_id + ")");

	if(db->Query("SELECT `company_position_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + company_id + "\";"))
	{
		result = db->Get(0, 0);
	}
	else if(db->Query(	"SELECT `company_position_id` FROM `contracts_sow` WHERE `agency_company_id`=("
							"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
						") LIMIT 0,1;"))
	{
		result = db->Get(0, 0);
	}
	else if(db->Query("SELECT `id` FROM `company_position` LIMIT 0,1;"))
	{
		result = db->Get(0, 0);
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to define company_position.id");
	}

	MESSAGE_DEBUG("", "", "finish (position.id = " + result + ")");

	return result;
}

auto	GetTemplateSoWAgreementFiles(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	contract_sow_id;
		string	title;
		string	filename;
		string	owner_user_id;
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
			item.title = db->Get(i, "title");
			item.filename = db->Get(i, "filename");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""							+ item.id + "\",";
			result += "\"entity_id\":\""					+ item.contract_sow_id + "\",";
			result += "\"contract_sow_id\":\""				+ item.contract_sow_id + "\",";
			result += "\"title\":\""						+ item.title + "\",";
			result += "\"filename\":\""						+ item.filename + "\",";
			result += "\"owner_user_id\":\""				+ item.owner_user_id + "\",";
			result += "\"eventTimestamp\":\""				+ item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no bt items assigned");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetTemplateCompanyAgreementFiles(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	company_id;
		string	title;
		string	filename;
		string	owner_user_id;
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
			item.company_id = db->Get(i, "company_id");
			item.title = db->Get(i, "title");
			item.filename = db->Get(i, "filename");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""							+ item.id + "\",";
			result += "\"entity_id\":\""					+ item.company_id + "\",";
			result += "\"company_id\":\""					+ item.company_id + "\",";
			result += "\"title\":\""						+ item.title + "\",";
			result += "\"filename\":\""						+ item.filename + "\",";
			result += "\"owner_user_id\":\""				+ item.owner_user_id + "\",";
			result += "\"eventTimestamp\":\""				+ item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no bt items assigned");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	CheckAgreementSoWTitle(string title, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	error_message = ""s;
	auto	sql_query = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		sql_query = "SELECT `id` FROM `contract_sow_agreement_files` WHERE `title`=\"" + title + "\" AND `contract_sow_id`=\"" + sow_id + "\";";
	}
	else
	{
		sql_query = "SELECT `id` FROM `company_agreement_files` WHERE `title`=\"" + title + "\" AND `company_id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\");";
	}

	if(db->Query(sql_query) == 0)
	{
		if(title.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890 _.-") == string::npos)
		{
			// --- good2go
		}
		else
		{
			error_message = gettext("Only english alphabet allowed and -_.");
			MESSAGE_DEBUG("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("already exists");
		MESSAGE_DEBUG("", "", "agreement_title already exists with id(" + db->Get(0, 0) + ")");		
	}

	MESSAGE_DEBUG("", "", "finish (error_message: " + error_message + ")");

	return error_message;
}

auto isDemoDomain() -> bool
{
	auto	result = false;
	string	domain_name = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(getenv("SERVER_NAME")) domain_name = getenv("SERVER_NAME");

	if(domain_name.find("demo") != string::npos) result = true;

	MESSAGE_DEBUG("", "", "result (" + to_string(result) + ")");

	return result;
}
