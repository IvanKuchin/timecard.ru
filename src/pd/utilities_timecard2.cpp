#include "utilities_timecard.h"

auto	GetServiceBTInvoicesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");
	
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
	static c_cache_obj				user_cache;

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(auto i = 0; i < affected; i++)
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
				error_message = gettext("SQL syntax error");
			}
			else
			{
				db->Query("DELETE FROM `invoice_cost_center_service` WHERE `id`=\"" + service_invoice_id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to delete from invoice_cost_center_service")
					error_message = gettext("SQL syntax error");
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
				error_message = gettext("SQL syntax error");
			}
			else
			{
				db->Query("DELETE FROM `invoice_cost_center_bt` WHERE `id`=\"" + bt_invoice_id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to delete from invoice_cost_center_bt")
					error_message = gettext("SQL syntax error");
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
								"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
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
								"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
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
						"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
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

		// --- all SoW with assigned tasks <- projects <- _CUSTOMER_ 
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

auto GetCostCenterIDByTaskID(string task_id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	cost_center_id = ""s;
	auto	customer_project_pair = GetCustomerIDProjectIDByTaskID(task_id, db);

	if(customer_project_pair.first.length())
	{
		cost_center_id = GetCostCenterIDByCustomerID(customer_project_pair.first, db);
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id(" + task_id + ") not found");
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
			if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\" AND `agency_company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + " AND `allowed_change_sow`=\"Y\");"))
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

string isSubcontractorAllowedToChangeSoW(string sow_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(user->GetType() == "subcontractor")
		{
			if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\" AND `subcontractor_company_id`=(SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user->GetID() + "\");"))
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
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") is not a subcontractor");
			error_message = gettext("You are not authorized");
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
								"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + " AND `allowed_change_sow`=\"Y\""
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
			if(db->Query("" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + " AND `allowed_change_agency_data`=\"Y\";"))
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
				if(action == "AJAX_deleteTimecardApproverFromPSoW")	sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `contracts_psow` WHERE `id`=(SELECT `contract_psow_id` FROM `timecard_approvers` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_deleteBTExpenseApproverFromPSoW")sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `contracts_psow` WHERE `id`=(SELECT `contract_psow_id` FROM `bt_approvers` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateSubcontractorCreateTasks")	sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true

				if(action == "AJAX_updateSoWNumber")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWAct")					sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWPosition")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWPaymentPeriodService")	sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWPaymentPeriodBT")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWDayRate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWSignDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWStartDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWEndDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWCustomField")			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteSoWCustomField")			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";

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
				if(action == "AJAX_deletePSoWCustomField")			sql_query = "SELECT `contract_psow_id` AS `sow_id` FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";";

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
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				if(
					new_value.length()									||
					(action == "AJAX_updateCompanyCustomField")			||	// --- custom field could be empty
					(action == "AJAX_updateCostCenterCustomField")		||	// --- custom field could be empty
					(action == "AJAX_updateSoWCustomField")				||	// --- custom field could be empty
					(action == "AJAX_updatePSoWCustomField")			||	// --- custom field could be empty
					(action == "AJAX_deleteCostCenterCustomField")		||	// --- custom field could be empty
					(action == "AJAX_deleteSoWCustomField")				||	// --- custom field could be empty
					(action == "AJAX_deletePSoWCustomField")			||	// --- custom field could be empty
					(action == "AJAX_updateCompanyActNumberPrefix")		||	// --- ActNumberPrefix could be empty
					(action == "AJAX_updateCompanyActNumberPostfix")		 // --- ActNumberPostfix could be empty
				)
				{
					// --- assignments can start / stop beyond SoW period, 
					if(action == "AJAX_updatePeriodStart")
					{
						if(sow_id.length())
						{
							auto	assignment_id = id;

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
						if(GetNumberOfLetters(new_value) < 3)
						{
							error_message = gettext("number of letters must be at least") + " 3"s;
							MESSAGE_DEBUG("", "", error_message);
						}
						else if(
							db->Query(
							"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + new_value + "\" AND `agency_company_id` = ( "
								"SELECT `agency_company_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\""
							");")
						)
						{
							error_message = "Заказчик " + new_value + " уже существует";
							MESSAGE_DEBUG("", "", "Customer with new_value already exists");
						}
					}
					else if(action == "AJAX_updateProjectTitle")
					{
						if(GetNumberOfLetters(new_value) < 3)
						{
							error_message = gettext("number of letters must be at least") + " 3"s;
							MESSAGE_DEBUG("", "", error_message);
						}
						else if(db->Query(
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
						if(GetNumberOfLetters(new_value) < 3)
						{
							error_message = gettext("number of letters must be at least") + " 3"s;
							MESSAGE_DEBUG("", "", error_message);
						}
						else if(db->Query(
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
					else if(action == "AJAX_updateCompanyCustomField")			{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWEditCapability")			{ /* --- good to go */ }
					else if(action == "AJAX_updateAgencyPosition")				{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWPosition")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyTitle")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyDescription")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyWebSite")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyTIN")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyVAT")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyVATCalculationType")	{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyActNumber")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyActNumberPrefix")		{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyActNumberPostfix")		{ /* --- good to go */ }
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
					else if(action == "AJAX_updateSoWPaymentPeriodService")		{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWPaymentPeriodBT")			{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWSignDate")					{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_deleteSoWCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_deleteTemplateAgreement_company")	{ /* --- good to go */ }
					else if(action == "AJAX_deleteTemplateAgreement_sow")		{ /* --- good to go */ }
					else if(action == "AJAX_updateAirfareLimitationByDirection"){ /* --- good to go */ }
					else if(action == "AJAX_updateHolidayCalendarTitle")		{ /* --- good to go */ }
					else if(action == "AJAX_deleteHolidayCalendar")				{ /* --- good to go */ }
					else if(action == "AJAX_deleteBTAllowance")					{ /* --- good to go */ }
					else if(action == "AJAX_updateBTAllowanceAmount")			{ /* --- good to go */ }
					else if(action == "AJAX_updateBTAllowanceCountry")
					{
						if(db->Query("SELECT `id` FROM `geo_country` WHERE `title`=" + quoted(new_value) + ";"))
						{
							if(db->Query(
								"SELECT `id` FROM `bt_allowance` WHERE "
									"`geo_country_id`=(SELECT `id` FROM `geo_country` WHERE `title`=\"" + new_value + "\") "
									"AND "
									"`agency_company_id`=(SELECT `agency_company_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\") "
								";"
								))
							{
								error_message = gettext("already exists");
								MESSAGE_DEBUG("", "", error_message);
							}
							else
							{
								// --- good to go
							}
						}
						else
						{
							error_message = gettext("country not found");
							MESSAGE_DEBUG("", "", error_message);
						}
					}
					else if(action == "AJAX_addBTAllowance")
					{
						if(db->Query("SELECT `id` FROM `bt_allowance` WHERE `geo_country_id`=" + quoted(new_value) + " AND `agency_company_id`=" + quoted(id) + ";"))
						{
							error_message = gettext("already exists");
							MESSAGE_DEBUG("", "", error_message);
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateHolidayCalendarDate")
					{
						if(db->Query("SELECT `id` FROM `holiday_calendar` WHERE `date`=" + quoted(new_value) + " AND `agency_company_id`=(SELECT `agency_company_id` FROM `holiday_calendar` WHERE `id`=\"" + id + "\");"))
						{
							error_message = gettext("already exists");
							MESSAGE_DEBUG("", "", error_message);
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_addHolidayCalendar")
					{
						if(db->Query("SELECT `id` FROM `holiday_calendar` WHERE `date`=" + quoted(new_value) + " AND `agency_company_id`=" + quoted(id) + ";"))
						{
							error_message = gettext("already exists");
							MESSAGE_DEBUG("", "", error_message);
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateTemplateAgreement_company_Title")
					{
						// if(db->Query("SELECT `id` FROM `company_agreement_files` WHERE `title`=\"" + new_value + "\" AND `company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ");"))
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
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateSoWEndDate")
					{
						if(db->Query("SELECT `status`,`start_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_status	= db->Get(0, "status");
							auto		string_start	= db->Get(0, "start_date");

							if(string_status != "expired")
							{
								auto		string_end	= new_value;
								auto		tm_start	= GetTMObject(string_start);
								auto		tm_end		= GetTMObject(string_end);

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
								error_message = gettext("contract already expired") + ", "s + gettext("changes prohibited") + ". ";
								MESSAGE_DEBUG("", "", error_message);
							}
						}
						else
						{
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateSoWDayRate")
					{
						c_float		num(new_value);

						// --- OR conditiona requires in case num is 10123 (matches by 1-st condition) or 10123.00 (matches by 2-nd condition)
						if((string(num) == new_value) || (num.PrintPriceTag() == new_value)) { /* --- good to go */ }
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
							error_message = gettext("SQL syntax error");
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
					else if(action == "AJAX_addTimecardApproverToPSoW")
					{
						auto	psow_id = id;

						if(db->Query("SELECT `id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + new_value + "\" AND `contract_psow_id`=\"" + psow_id + "\";"))
						{
							error_message = gettext("already exists");
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
					else if(action == "AJAX_addBTExpenseApproverToPSoW")
					{
						auto	psow_id = id;

						if(db->Query("SELECT `id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + new_value + "\" AND `contract_psow_id`=\"" + psow_id + "\";"))
						{
							error_message = gettext("already exists");
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
					else if(action == "AJAX_deletePSoWCustomField")				{ /* --- good to go */ }
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
							error_message = gettext("SQL syntax error");
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
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updatePSoWDayRate")
					{
						c_float		num(new_value);

						// --- OR conditiona requires in case num is 10123 (matches by 1-st condition) or 10123.00 (matches by 2-nd condition)
						if((string(num) == new_value) || (num.PrintPriceTag() == new_value))
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

						// --- OR conditiona requires in case num is 10123 (matches by 1-st condition) or 10123.00 (matches by 2-nd condition)
						if((string(num) == new_value) || (num.PrintPriceTag() == new_value))
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
						if(db->Query("SELECT `end_date` FROM `cost_centers` WHERE `id`=\"" + id + "\";"))
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
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateCostCenterEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `cost_centers` WHERE `id`=\"" + id + "\";"))
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
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateTimecardApproverOrder")
					{
						auto	psow_id = id;
						auto	other_psow_id = GetValueFromDB("SELECT DISTINCT(`contract_psow_id`) FROM `timecard_approvers` WHERE `id` IN (" + new_value + ") AND `contract_psow_id`!=" + quoted(psow_id) + ";", db);

						if(other_psow_id.length())
						{
							error_message = gettext("approvers belong to different PSoW");
							MESSAGE_ERROR("", "", error_message);
						}
						else
						{
						}
					}
					else if(action == "AJAX_updateBTApproverOrder")
					{
						auto	psow_id = id;
						auto	other_psow_id = GetValueFromDB("SELECT DISTINCT(`contract_psow_id`) FROM `bt_approvers` WHERE `id` IN (" + new_value + ") AND `contract_psow_id`!=" + quoted(psow_id) + ";", db);

						if(other_psow_id.length())
						{
							error_message = gettext("approvers belong to different PSoW");
							MESSAGE_ERROR("", "", error_message);
						}
						else
						{
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
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(agency_id.length())
	{
		if(id.length())
		{
			if(action.length())
			{
				string		sql_query = "";

				if(action == "AJAX_updateCustomerTitle") 					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateProjectTitle") 					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateTaskTitle") 						sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=(SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\"));";

				if(action == "AJAX_updateAirfareLimitationByDirection")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `airfare_limits_by_direction` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteAirfarelimitationByDirection")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `airfare_limits_by_direction` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateBTAllowanceCountry")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateBTAllowanceAmount")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteBTAllowance")						sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateHolidayCalendarDate")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateHolidayCalendarTitle")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteHolidayCalendar")					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";

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

				if(action == "AJAX_updateCostCenterNumber")					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterAct")					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterSignDate")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterStartDate")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterEndDate")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterCustomField")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=(SELECT `cost_center_id` FROM `cost_center_custom_fields` WHERE `id`=\"" + id + "\");";
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

				if(action == "AJAX_updateTimecardApproverOrder")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateBTApproverOrder")					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateCompanyCustomField")				sql_query = "SELECT `company_id` AS `agency_id` FROM `company_custom_fields` WHERE `id`=\"" + id + "\";";

				if(
					(action == "AJAX_updateCompanyTitle")				||
					(action == "AJAX_updateCompanyDescription")			||
					(action == "AJAX_updateCompanyWebSite")				||
					(action == "AJAX_updateCompanyTIN")					||
					(action == "AJAX_updateCompanyVAT")					||
					(action == "AJAX_updateCompanyVATCalculationType")	||
					(action == "AJAX_updateCompanyActNumber")			||
					(action == "AJAX_updateCompanyActNumberPrefix")		||
					(action == "AJAX_updateCompanyActNumberPostfix")	||
					(action == "AJAX_updateCompanyAccount")				||
					(action == "AJAX_updateCompanyOGRN")				||
					(action == "AJAX_updateCompanyKPP")					||
					(action == "AJAX_updateCompanyLegalAddress")		||
					(action == "AJAX_updateCompanyMailingAddress")		||
					(action == "AJAX_updateCompanyMailingZipID")		||
					(action == "AJAX_updateCompanyLegalZipID")			||
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

	auto affected = db->Query(sqlQuery);
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
		for(auto i = 0; i < affected; i++)
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
	MESSAGE_DEBUG("", "", "start");

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
	auto					result = ""s;
	auto					affected  = db->Query(sqlQuery);

	if(affected)
	{
		for(auto i = 0; i < affected; i++)
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
			result += "\"company_id\":\""					+ item.company_id + "\",";
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
		MESSAGE_DEBUG("", "", "no custom fields assigned");
	}

	MESSAGE_DEBUG("", "", "finish");

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
							"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
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
		for(auto i = 0; i < affected; i++)
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
		for(auto i = 0; i < affected; i++)
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
		sql_query = "SELECT `id` FROM `company_agreement_files` WHERE `title`=\"" + title + "\" AND `company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ");";
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

auto	GetDBValueByAction(string action, string id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				string		sql_query = "";

				if(action == "AJAX_updateCustomerTitle") 					sql_query = "SELECT `title`							FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateProjectTitle") 					sql_query = "SELECT `title`							FROM `timecard_projects` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateTaskTitle") 						sql_query = "SELECT `title`							FROM `timecard_tasks` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodStart") 						sql_query = "SELECT `period_start`					FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodEnd") 						sql_query = "SELECT `period_end`					FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTitle")						sql_query = "SELECT `name`							FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyDescription")				sql_query = "SELECT `description`					FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyWebSite")					sql_query = "SELECT `webSite`						FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTIN")						sql_query = "SELECT `tin`							FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyActNumber")					sql_query = "SELECT `act_number`					FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyActNumberPrefix")			sql_query = "SELECT `act_number_prefix`				FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyActNumberPostfix")			sql_query = "SELECT `act_number_postfix` 			FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyVAT")						sql_query = "SELECT `vat`							FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyVATCalculationType")		sql_query = "SELECT `vat_calculation_type`			FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyAccount")					sql_query = "SELECT `account`						FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyOGRN")						sql_query = "SELECT `ogrn`							FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyKPP")						sql_query = "SELECT `kpp`							FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyLegalAddress")				sql_query = "SELECT `legal_address`					FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyMailingAddress")			sql_query = "SELECT `mailing_address`				FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyMailingZipID")				sql_query = "SELECT `mailing_geo_zip_id`			FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyLegalZipID")				sql_query = "SELECT `legal_geo_zip_id`				FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyBankID")					sql_query = "SELECT `bank_id`						FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyCustomField")				sql_query = "SELECT `value`							FROM `company_custom_fields` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyPosition")					sql_query = "SELECT `title`							FROM `company_position` WHERE `id`=(SELECT `position_id` FROM `company_employees` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateSoWPosition")						sql_query = "SELECT `title`							FROM `company_position` WHERE `id`=(SELECT `company_position_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");";
				if(action == "AJAX_updatePSoWPosition")						sql_query = "SELECT `title`							FROM `company_position` WHERE `id`=(SELECT `company_position_id` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\");";
				if(action == "AJAX_updateAgencyEditCapability")				sql_query = "SELECT `allowed_change_agency_data`	FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSoWEditCapability")				sql_query = "SELECT `allowed_change_sow`			FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSubcontractorCreateTasks")			sql_query = "SELECT `subcontractor_create_tasks`	FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAirfareLimitationByDirection")		sql_query = "SELECT `limit`							FROM `airfare_limits_by_direction` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateTitle")				sql_query = "SELECT `title`							FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateTaxable")			sql_query = "SELECT `taxable`						FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateAgencyComment") 	sql_query = "SELECT `agency_comment`				FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "SELECT `title`							FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "SELECT `description`					FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "SELECT `tooltip`						FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "SELECT `dom_type`						FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCash")	sql_query = "SELECT `payment`						FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCard")	sql_query = "SELECT `payment`						FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "SELECT `required`						FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterToCustomer")				sql_query = "SELECT `cost_center_id`				FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";";
				if(action == "AJAX_deleteCostCenterFromCustomer")			sql_query = "SELECT `cost_center_id`				FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";";
				if(action == "AJAX_deleteCostCenter")						sql_query = "SELECT `id`							FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterTitle")					sql_query = "SELECT `title`							FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterDescription")			sql_query = "SELECT `description`					FROM `cost_centers` WHERE `id`=\"" + id + "\";";
		
				if(action == "AJAX_updateSoWNumber")						sql_query = "SELECT `number`						FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWAct")							sql_query = "SELECT `act_number`					FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWDayRate")						sql_query = "SELECT `day_rate`						FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWPaymentPeriodService")			sql_query = "SELECT `payment_period_service`		FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWPaymentPeriodBT")				sql_query = "SELECT `payment_period_bt`				FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWSignDate")						sql_query = "SELECT `sign_date`						FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWStartDate")						sql_query = "SELECT `start_date`					FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWEndDate")						sql_query = "SELECT `end_date`						FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWCustomField")					sql_query = "SELECT `value`							FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteSoWCustomField")					sql_query = "SELECT `var_name`						FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updatePSoWNumber")						sql_query = "SELECT `number`						FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWAct")							sql_query = "SELECT `act_number`					FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWDayRate")						sql_query = "SELECT `day_rate`						FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWBTMarkup")						sql_query = "SELECT `bt_markup`						FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWBTMarkupType")					sql_query = "SELECT `bt_markup_type`				FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWSignDate")						sql_query = "SELECT `sign_date`						FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWStartDate")					sql_query = "SELECT `start_date`					FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWEndDate")						sql_query = "SELECT `end_date`						FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWCustomField")					sql_query = "SELECT `value`							FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deletePSoWCustomField")					sql_query = "SELECT `var_name`						FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateCostCenterNumber")					sql_query = "SELECT `number`						FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterAct")					sql_query = "SELECT `act_number`					FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterSignDate")				sql_query = "SELECT `sign_date`						FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterStartDate")				sql_query = "SELECT `start_date`					FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterEndDate")				sql_query = "SELECT `end_date`						FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterCustomField")			sql_query = "SELECT `value`							FROM `cost_center_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_deleteBTAllowance")						sql_query = "SELECT `title`							FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateBTAllowanceCountry")				sql_query = "SELECT `title`							FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateBTAllowanceAmount")				sql_query = "SELECT `amount`						FROM `bt_allowance` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateHolidayCalendarDate")				sql_query = "SELECT `date`							FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateHolidayCalendarTitle")				sql_query = "SELECT `title`							FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteHolidayCalendar")					sql_query = "SELECT `title`							FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateTimecardApproverOrder")			sql_query = "SELECT 'fake';";
				if(action == "AJAX_updateBTApproverOrder")					sql_query = "SELECT 'fake';";

				if(action == "AJAX_updateBTOriginalDocumentsDelivery")		sql_query = "SELECT `originals_received_date` 		FROM `bt` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateTimecardOriginalDocumentsDelivery")sql_query = "SELECT `originals_received_date` 		FROM `timecards` WHERE `id`=\"" + id + "\";";


				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						result = db->Get(0, 0);
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

static auto __Get_NumberOfTimecardsOrBT_InPaymentPendingState(string entity_type, string sow_sql, CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start");

	auto	pending_items = "0"s;

	if(db->Query("SELECT COUNT(*) FROM `" + entity_type + "` WHERE "
													"`payed_date`=\"0\" "
													"AND "
													"`expected_pay_date`!=\"0\" "
													"AND "
													"`contract_sow_id` IN (" + sow_sql + ");")
		)
	{
		pending_items = db->Get(0,0);
	}

	MESSAGE_DEBUG("", "", "finish (number of pending_items is " + pending_items + ")");

	return pending_items;
}

string GetNumberOfTimecardsInPaymentPendingState(string sow_sql, CMysql *db, CUser *user)
{
	return __Get_NumberOfTimecardsOrBT_InPaymentPendingState("timecards", sow_sql, db, user);
}

string GetNumberOfBTInPaymentPendingState(string sow_sql, CMysql *db, CUser *user)
{
	return __Get_NumberOfTimecardsOrBT_InPaymentPendingState("bt", sow_sql, db, user);
}

static auto __Get_TimecardsORBT_WithExpiredPayment(string entity_type, string multiplier, string sow_sql, CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start");

	vector<string>	result;
	auto			table = ""s;
	auto			table_column = ""s;

	if(entity_type == "timecards")
	{
		table = "timecards"s;
		table_column = "payment_period_service"s;
	}
	else if(entity_type == "bt")
	{
		table = "bt"s;
		table_column = "payment_period_bt"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "unknown entity_type(" + entity_type + ")");
	}

	auto affected = db->Query(
				"SELECT `a`.`id`, `b`.`id`, `b`.`payment_period_service`, `b`.`payment_period_bt` FROM `" + table + "` as `a` "
				"INNER JOIN `contracts_sow` as `b` ON `a`.`contract_sow_id`=`b`.`id` "
				"WHERE  "
					"(`a`.`expected_pay_date` - 3600 * 24 * `b`.`" + table_column + "` * " + multiplier + "<UNIX_TIMESTAMP())  "
					"AND  "
					"(`a`.`expected_pay_date`!=\"0\")  "
				    "AND  "
				    "(`a`.`payed_date`=\"0\") "
					"AND  "
					"(`a`.`contract_sow_id` IN (" + sow_sql + "));");
		
	for(auto i = 0; i < affected; ++i)
		result.push_back(db->Get(i, 0));

	MESSAGE_DEBUG("", "", "finish (" + to_string(result.size()) + ")");

	return result;
}

vector<string> GetTimecardsWithExpiredPayment(string multiplier, string sow_sql, CMysql *db, CUser *user)
{
	return __Get_TimecardsORBT_WithExpiredPayment("timecards", multiplier, sow_sql, db, user);
}

vector<string> GetBTWithExpiredPayment(string multiplier, string sow_sql, CMysql *db, CUser *user)
{
	return __Get_TimecardsORBT_WithExpiredPayment("bt", multiplier, sow_sql, db, user);
}

string GetNumberOfTimecardsInPendingState(CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start");

	auto	number_of_pending_items = db->Query("SELECT DISTINCT(`timecard_id`) FROM `timecard_approvals` WHERE "
													"`decision`=\"pending\" "
													"AND "
													"`approver_id` IN (SELECT `id` FROM `timecard_approvers` WHERE `approver_user_id`=" + quoted(user->GetID()) + ")"
													";");

	MESSAGE_DEBUG("", "", "finish (number of pending items is " + to_string(number_of_pending_items) + ")");

	return to_string(number_of_pending_items);
}

string GetNumberOfBTInPendingState(CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start");

	auto	number_of_pending_items = db->Query("SELECT DISTINCT(`bt_id`) FROM `bt_approvals` WHERE "
													"`decision`=\"pending\" "
													"AND "
													"`approver_id` IN (SELECT `id` FROM `bt_approvers` WHERE `approver_user_id`=" + quoted(user->GetID()) + ")"
													";");

	MESSAGE_DEBUG("", "", "finish (number of pending items is " + to_string(number_of_pending_items) + ")");

	return to_string(number_of_pending_items);
}

/*
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

		affected = db->Query("" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ";");
		if(affected)
		{
			company_id = db->Get(0, "company_id");

			affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `bt` WHERE "
										"`status`=\"submit\" "
										"AND "
										"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + company_id + "\");");
			for(auto i = 0; i < affected; ++i)
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
		for(auto i = 0; i < affected; ++i)
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
		if(amIonTheApproverListForSoW("bt_approvers", bt.contract_sow_id, db, user))
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
		else
		{
			MESSAGE_DEBUG("", "", "I'm not on the approver list for SoW(" + bt.contract_sow_id + ")");
		}
	}

	MESSAGE_DEBUG("", "", "finish (number of pending_bt is " + to_string(pending_bt) + ")");

	return to_string(pending_bt);
}
*/
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
					auto		sql_query_where_statement = ""s;

					if(user->GetType() == "approver")
						sql_query_where_statement = "`contract_sow_id` IN ( " + Get_SoWIDsByTimecardApproverUserID_sqlquery(user->GetID()) + " )";
					else if(user->GetType() == "agency")
						sql_query_where_statement = "`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + "))";
					else
					{
						MESSAGE_ERROR("", "", "unknown user type");
					}

					if(filter == "submit") sql_query_where_statement += " AND `status` = \"submit\" ";

					result += 
						"\"timecards\":[" + GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE " + sql_query_where_statement, db, user, true) + "],"
						"\"holiday_calendar\":[" + GetHolidayCalendarInJSONFormat("SELECT * FROM `holiday_calendar` WHERE "
																					"`agency_company_id` IN (" // --- this part important if user is an approver
																						"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id` IN (" + Get_SoWIDsByTimecardApproverUserID_sqlquery(user->GetID()) + ")"
																					") "
																					"OR "
																					"`agency_company_id` IN (" // --- this part important if user is not an approver, but agency employee
																				    	+ Get_CompanyIDByUserID_sqlquery(user->GetID()) +
																					") "
																					";", db, user) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`id` IN ( SELECT `contract_sow_id` FROM `timecards` WHERE " + sql_query_where_statement + ") "
								";", db, user, false, false, false,  true) + "]";
				}
				else if(object == "bt")
				{
					auto		sql_query_where_statement = ""s;

					if(user->GetType() == "approver")
						sql_query_where_statement = "`contract_sow_id` IN ( " + Get_SoWIDsByBTApproverUserID_sqlquery(user->GetID()) + " )";
						// sql_query_where_statement = "`contract_sow_id` IN ( SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\")";
					else if(user->GetType() == "agency")
						sql_query_where_statement = "`contract_sow_id` IN ( SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + "))";
					else
					{
						MESSAGE_ERROR("", "", "unknown user type");
					}
													

					if(filter == "submit") sql_query_where_statement += " AND `status` = \"submit\" ";

					result +=
						"\"bt\":[" + GetBTsInJSONFormat("SELECT * FROM `bt` WHERE " + sql_query_where_statement, db, user, true) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`id` IN ( SELECT `contract_sow_id` FROM `bt` WHERE " + sql_query_where_statement + ") "
								";", db, user, true, false, false,  true) + "]"; // --- incl_tasks required to display BT comment with Customer name
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
		for(auto i = 0; i < affected; i++)
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


/*
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
		for(auto i = 0; i < affected; i++)
		{
			approvers[db->Get(i, "id")] = false;
		}

		affected = db->Query(
				"SELECT * FROM `bt_approvals` WHERE "
					"`eventTimestamp` > (SELECT `eventTimestamp` FROM `bt` WHERE `id`=\"" + bt_id + "\") "
					"AND "
					"`bt_id`=\"" + bt_id + "\""
				";");
		for(auto i = 0; i < affected; i++)
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
*/
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
		for(auto i = 0; i < affected; i++)
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

auto isCostCenterBelongsToAgency(string cost_center_id, CMysql *db, CUser *user) -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		if(cost_center_id.length())
		{
			if(db->Query("SELECT `id` FROM `cost_centers` WHERE `id`=\"" + cost_center_id + "\" AND `agency_company_id`=("
							"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
								"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
							")"
						");"))
			{
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "cost_center_id(" + cost_center_id + ") doesn't belongs to company user.id(" + user->GetID() + ") employeed");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "cost_center_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}
	
	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

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
				error_message = gettext("you are not authorized");
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else if(user->GetType() == "agency")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN ("
						"" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
					")"
				")"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = gettext("you are not authorized");
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else if(user->GetType() == "approver")
		{
/*
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\""
					")"
				";"
			))
*/
			if(db->Query("SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN (" + Get_SoWIDsByBTApproverUserID_sqlquery(user->GetID()) + ");"))
			{
				// --- everything is good
			}
			else
			{
				error_message = gettext("you are not authorized");
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
						if(db->Query("SELECT COUNT(*) AS `counter` FROM `cost_center_assignment` WHERE `timecard_customer_id` IN ("
										"SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id` IN ("
											"SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + task_id + "\""
										")"
									");"))
						{
							auto	counter = stoi(db->Get(0, "counter"));
							if(counter == 0)
							{
								// --- task.id valid to remve
							}
							else
							{
								MESSAGE_DEBUG("", "", "cost center assigned on task.id(" + task_id + ")");
								error_message = gettext("This task assigned to cost center") + ". "s + gettext("removal prohibited") + ".";
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

string	GetInfoToReturnByAction(string action, string temp_id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	auto	result = ""s;
	auto	psow_id = temp_id;

	MESSAGE_DEBUG("", "", "start");

	if((action == "AJAX_addTimecardApproverToPSoW") || (action == "AJAX_deleteTimecardApproverFromPSoW"))
		result = "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_psow_id`=\"" + psow_id + "\";", db, user, DO_NOT_INCLUDE_PSOW_INFO) + "]";
	if((action == "AJAX_addBTExpenseApproverToPSoW") || (action == "AJAX_deleteBTExpenseApproverFromPSoW"))
		result = "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_psow_id`=\"" + psow_id + "\";", db, user, DO_NOT_INCLUDE_PSOW_INFO) + "]";

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}
/*
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
					if(action == "AJAX_deleteTimecardApproverFromPSoW")
					{
						if(sow_id.length())
						{
							int	affected = db->Query("SELECT `id` FROM `timecards` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";");
							vector<string> timecard_list;

							for(auto i = 0; i < affected; ++i)
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
					else if(action == "AJAX_deleteBTExpenseApproverFromPSoW")
					{
						if(sow_id.length())
						{
							int	affected = db->Query("SELECT `id` FROM `bt` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";");
							vector<string> bt_list;

							for(auto i = 0; i < affected; ++i)
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
*/
auto DeleteEntryByAction(string action, string id, CMysql *db, CUser *user) -> string
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action == "AJAX_deleteTemplateAgreement_company")
			{
				db->Query("DELETE FROM `company_agreement_files` WHERE `id`=\"" + id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to remove from table company_agreement_files");
				}
			}
			else if(action == "AJAX_deleteTemplateAgreement_sow")
			{
				db->Query("DELETE FROM `contract_sow_agreement_files` WHERE `id`=\"" + id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to remove from table contract_sow_agreement_files");
				}
			}
			else if(action == "AJAX_deletePSoWCustomField")
			{
				db->Query("DELETE FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to remove from table contract_psow_custom_fields");
				}
			}
			else if(action == "AJAX_deleteSoWCustomField")
			{
				db->Query("DELETE FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to remove from table contract_sow_custom_fields");
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

string	SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				if(
					new_value.length()									||
					(action == "AJAX_updateCompanyCustomField")			||	// --- custom field could be empty
					(action == "AJAX_updateCostCenterCustomField")		||	// --- custom field could be empty
					(action == "AJAX_updateSoWCustomField")				||	// --- custom field could be empty
					(action == "AJAX_updatePSoWCustomField")			||	// --- custom field could be empty
					(action == "AJAX_updateCompanyActNumberPrefix")		||	// --- ActNumberPrefix could be empty
					(action == "AJAX_updateCompanyActNumberPostfix")		 // --- ActNumberPostfix could be empty
				)
				{
					if(error_message.empty())
					{
						string		sql_query = "";

						if(action == "AJAX_updateCustomerTitle") 					sql_query = "UPDATE `timecard_customers`		SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateProjectTitle") 					sql_query = "UPDATE `timecard_projects`			SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateTaskTitle") 						sql_query = "UPDATE `timecard_tasks`			SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateAirfareLimitationByDirection")		sql_query = "UPDATE `airfare_limits_by_direction`	SET `limit`						=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateTitle")				sql_query = "UPDATE `bt_expense_templates`		SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateTaxable")			sql_query = "UPDATE `bt_expense_templates`		SET `taxable`						=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateAgencyComment")		sql_query = "UPDATE `bt_expense_templates`		SET `agency_comment`				=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "UPDATE `bt_expense_line_templates`	SET `title`							=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "UPDATE `bt_expense_line_templates`	SET `description`					=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "UPDATE `bt_expense_line_templates`	SET `tooltip`						=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "UPDATE `bt_expense_line_templates`	SET `dom_type`						=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "UPDATE `bt_expense_line_templates`	SET `required`						=\"" + (new_value == "true" ? "Y"s : "N"s) + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePeriodStart") 						sql_query = "UPDATE `timecard_task_assignment`	SET `period_start`					=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePeriodEnd") 						sql_query = "UPDATE `timecard_task_assignment`	SET `period_end`					=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyTitle")						sql_query = "UPDATE	`company`					SET `name` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyDescription")				sql_query = "UPDATE	`company`					SET `description`					=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyWebSite")					sql_query = "UPDATE	`company`					SET `webSite`						=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyTIN")						sql_query = "UPDATE	`company`					SET `tin` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyActNumber")					sql_query = "UPDATE	`company`					SET `act_number`					=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyActNumberPrefix")			sql_query = "UPDATE	`company`					SET `act_number_prefix`				=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyActNumberPostfix")			sql_query = "UPDATE	`company`					SET `act_number_postfix`			=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyVAT")						sql_query = "UPDATE	`company`					SET `vat` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyVATCalculationType")		sql_query = "UPDATE	`company`					SET `vat_calculation_type`			=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyAccount")					sql_query = "UPDATE	`company`					SET `account`						=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyOGRN")						sql_query = "UPDATE	`company`					SET `ogrn` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyKPP")						sql_query = "UPDATE	`company`					SET `kpp` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyLegalAddress")				sql_query = "UPDATE	`company`					SET `legal_address`					=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyMailingAddress")			sql_query = "UPDATE	`company`					SET `mailing_address`				=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyMailingZipID")				sql_query = "UPDATE	`company`					SET `mailing_geo_zip_id`			=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyLegalZipID")				sql_query = "UPDATE	`company`					SET `legal_geo_zip_id`				=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyBankID")					sql_query = "UPDATE	`company`					SET `bank_id`						=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyCustomField")				sql_query = "UPDATE	`company_custom_fields`		SET `value`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSoWEditCapability")				sql_query = "UPDATE	`company_employees`			SET `allowed_change_sow`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateAgencyEditCapability")				sql_query = "UPDATE	`company_employees`			SET `allowed_change_agency_data`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSubcontractorCreateTasks")			sql_query = "UPDATE	`contracts_sow`				SET `subcontractor_create_tasks`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_deleteCostCenterFromCustomer")			sql_query = "DELETE FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";";
						if(action == "AJAX_deleteCostCenter")						sql_query = "DELETE FROM `cost_centers` WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterTitle")					sql_query = "UPDATE `cost_centers`				SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterDescription")			sql_query = "UPDATE `cost_centers`				SET `description`					=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";

						if(action == "AJAX_updateSoWCustomField")					sql_query = "UPDATE `contract_sow_custom_fields`SET `value`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSoWNumber")						sql_query = "UPDATE `contracts_sow`				SET `number`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWAct")							sql_query = "UPDATE `contracts_sow`				SET `act_number`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWSignDate")						sql_query = "UPDATE `contracts_sow`				SET `sign_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWStartDate")						sql_query = "UPDATE `contracts_sow`				SET `start_date`	=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWEndDate")						sql_query = "UPDATE `contracts_sow`				SET `end_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWDayRate") {c_float	num(new_value); sql_query = "UPDATE `contracts_sow` 		SET `day_rate`		=\"" + string(num) + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";"; }
						if(action == "AJAX_updateSoWPaymentPeriodService")			sql_query = "UPDATE `contracts_sow`				SET `payment_period_service`=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWPaymentPeriodBT")				sql_query = "UPDATE `contracts_sow`				SET `payment_period_bt`=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";

						if(action == "AJAX_updatePSoWCustomField")					sql_query = "UPDATE `contract_psow_custom_fields`SET `value`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePSoWNumber")						sql_query = "UPDATE `contracts_psow`			SET `number`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWAct")							sql_query = "UPDATE `contracts_psow`			SET `act_number`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWSignDate")						sql_query = "UPDATE `contracts_psow`			SET `sign_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWStartDate")					sql_query = "UPDATE `contracts_psow`			SET `start_date`	=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWEndDate")						sql_query = "UPDATE `contracts_psow`			SET `end_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWDayRate")  {c_float num(new_value); sql_query = "UPDATE `contracts_psow` 		SET `day_rate`		=\"" + string(num) + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";"; }
						if(action == "AJAX_updatePSoWBTMarkup") {c_float num(new_value); sql_query = "UPDATE `contracts_psow` 		SET `bt_markup`		=\"" + string(num) + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";"; }
						if(action == "AJAX_updatePSoWBTMarkupType")					sql_query = "UPDATE `contracts_psow` 			SET `bt_markup_type`=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";

						if(action == "AJAX_updateCostCenterCustomField")			sql_query = "UPDATE `cost_center_custom_fields` SET `value`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterNumber")					sql_query = "UPDATE `cost_centers`				SET `number`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterAct")					sql_query = "UPDATE `cost_centers`				SET `act_number`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterSignDate")				sql_query = "UPDATE `cost_centers`				SET `sign_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterStartDate")				sql_query = "UPDATE `cost_centers`				SET `start_date`	=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterEndDate")				sql_query = "UPDATE `cost_centers`				SET `end_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";

						if(action == "AJAX_updateTemplateAgreement_company_Title")	sql_query = "UPDATE `company_agreement_files`	SET `title`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateTemplateAgreement_sow_Title")		sql_query = "UPDATE `contract_sow_agreement_files` SET `title`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";

						if(action == "AJAX_updateHolidayCalendarDate")				sql_query = "UPDATE `holiday_calendar` 			SET `date`			=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateHolidayCalendarTitle")				sql_query = "UPDATE `holiday_calendar` 			SET `title`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_deleteHolidayCalendar")					sql_query = "DELETE FROM `holiday_calendar` WHERE `id`=\"" + id + "\";";

						if(action == "AJAX_updateBTAllowanceCountry")				sql_query = "UPDATE `bt_allowance` 				SET `geo_country_id`=(SELECT `id` FROM `geo_country` WHERE `title`=\"" + new_value + "\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateBTAllowanceAmount")				sql_query = "UPDATE `bt_allowance` 				SET `amount`=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_deleteBTAllowance")						sql_query = "DELETE FROM `bt_allowance` WHERE `id`=\"" + id + "\";";

						// --- add approver
						if(action == "AJAX_addTimecardApproverToPSoW")
						{
							auto	psow_id = id;

							// --- insert 0 if everybody else have 0 order
							// --- insert MAX+1 if approvers are ordered
							auto	order = "0"s;

							if(db->Query("SELECT `approver_order` FROM `timecard_approvers` WHERE `contract_psow_id`=" + quoted(psow_id) + " ORDER BY `approver_order` DESC LIMIT 0,1;"))
							{
								order = db->Get(0, 0);

								if(order == "0") {} else { order += "+1"; }
							}

							sql_query = "INSERT INTO `timecard_approvers` (`approver_user_id`,`contract_psow_id`, `approver_order`) VALUES (\"" + new_value + "\", \"" + psow_id + "\"," + order + ");";
						}
						if(action == "AJAX_addBTExpenseApproverToPSoW")
						{
							auto	psow_id = id;

							// --- insert 0 if everybody else have 0 order
							// --- insert MAX+1 if approvers are ordered
							auto	order = "0"s;

							if(db->Query("SELECT `approver_order` FROM `bt_approvers` WHERE `contract_psow_id`=" + quoted(psow_id) + " ORDER BY `approver_order` DESC LIMIT 0,1;"))
							{
								order = db->Get(0, 0);

								if(order == "0") {} else { order += "+1"; }
							}

							sql_query = "INSERT INTO `bt_approvers` (`approver_user_id`,`contract_psow_id`, `approver_order`) VALUES (\"" + new_value + "\", \"" + psow_id + "\", " + order + ");";
						}

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
							auto	position_id = GetCompanyPositionIDByTitle(new_value, db);

							if(position_id.length())
								sql_query = "UPDATE	`company_employees`	SET `position_id` =\"" + position_id + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
							else
								MESSAGE_ERROR("", "", "fail to get position.id");
						}
						if(action == "AJAX_updateSoWPosition")
						{
							auto	position_id = GetCompanyPositionIDByTitle(new_value, db);

							if(position_id.length())
								sql_query = "UPDATE	`contracts_sow`	SET `company_position_id` =\"" + position_id + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
							else
								MESSAGE_ERROR("", "", "fail to get position.id");
						}
						if(action == "AJAX_updatePSoWPosition")
						{
							auto	position_id = GetCompanyPositionIDByTitle(new_value, db);

							if(position_id.length())
								sql_query = "UPDATE	`contracts_psow` SET `company_position_id` =\"" + position_id + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
							else
								MESSAGE_ERROR("", "", "fail to get position.id");
						}
						if(action == "AJAX_updateCostCenterToCustomer")
						{
							auto	cost_center_assignment_id = ""s;

							if(db->Query("SELECT `id` FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";"))
							{
								cost_center_assignment_id = db->Get(0, "id");
							}
							else
							{
								long int temp = db->InsertQuery("INSERT INTO `cost_center_assignment` (`cost_center_id`,`timecard_customer_id`,`assignee_user_id`,`eventTimestamp`) VALUES (\"" + new_value + "\",\"" + id + "\",\"" + user->GetID() + "\", UNIX_TIMESTAMP());");
								if(temp)
								{
									cost_center_assignment_id = to_string(temp);
									MESSAGE_DEBUG("", "", "new cost_center_assignment.id(" + cost_center_assignment_id + ") created.");
								}
								else
								{
									cost_center_assignment_id = "0";
									MESSAGE_ERROR("", "", "fail to insert to cost_center_assignment table");
								}
							}

							if(CreatePSoWfromTimecardCustomerIDAndCostCenterID(id, new_value, db, user))
							{
								// --- good to go
							}
							else
							{
								MESSAGE_DEBUG("", "", "there were no new PSoW(cost_center.id(" + new_value + ") / SoW.id <- timecard_customer.id(" + id + ")) created due to either PSoW already exists, or Customer->projects->tasks not assigned to any SoW")
							}

							sql_query = "UPDATE	`cost_center_assignment` SET `cost_center_id` =\"" + new_value + "\" WHERE `timecard_customer_id`=\"" + id + "\";";
						}

						if(action == "AJAX_deleteTimecardApproverFromPSoW")
						{
							auto	affected = 0;
							auto	psow_id = GetValueFromDB("SELECT `contract_psow_id` FROM `timecard_approvers` WHERE `id`=" + quoted(id) + ";", db);

							if(psow_id.length())
							{
								db->Query("DELETE FROM `timecard_approvals` WHERE `approver_id`=" + quoted(id) + ";");
								db->Query("DELETE FROM `timecard_approvers` WHERE `id`=" + quoted(id) + ";");

								// --- renumber approvers
								if((affected = db->Query("SELECT `id` FROM `timecard_approvers` WHERE `contract_psow_id`=" + quoted(psow_id) + " AND `approver_order`>0 ORDER BY `approver_order` ASC;")))
								{
									vector<string>	id_arr;
									auto			i = 0;

									for(i = 0; i < affected; ++i)
									{
										id_arr.push_back(db->Get(i, 0));
									}

									i = 1;
									for(auto &id: id_arr)
									{
										db->Query("UPDATE `timecard_approvers` SET `approver_order`=" + quoted(to_string(i++)) + " WHERE `id`=" + quoted(id) + ";");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "no need to reorder");
								}
							}
							else
							{
								MESSAGE_ERROR("", "", "psow_id can't be found by approver_id(" + id + ")");
							}

							sql_query = "SELECT 'fake'";
						}
						if(action == "AJAX_deleteBTExpenseApproverFromPSoW")
						{
							auto	affected = 0;
							auto	psow_id = GetValueFromDB("SELECT `contract_psow_id` FROM `bt_approvers` WHERE `id`="s + quoted(id) + ";", db);

							if(psow_id.length())
							{
								db->Query("DELETE FROM `bt_approvals` WHERE `approver_id`=\"" + id + "\";");
								db->Query("DELETE FROM `bt_approvers` WHERE `id`=\"" + id + "\";");

								// --- renumber approvers
								if((affected = db->Query("SELECT `id` FROM `bt_approvers` WHERE `contract_psow_id`=" + quoted(psow_id) + " AND `approver_order`>0 ORDER BY `approver_order` ASC;")))
								{
									vector<string>	id_arr;
									auto			i = 0;

									for(i = 0; i < affected; ++i)
									{
										id_arr.push_back(db->Get(i, 0));
									}

									i = 1;
									for(auto &id: id_arr)
									{
										db->Query("UPDATE `bt_approvers` SET `approver_order`=" + quoted(to_string(i++)) + " WHERE `id`=" + quoted(id) + ";");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "no need to reorder");
								}
							}
							else
							{
								MESSAGE_ERROR("", "", "psow_id can't be found by approver_id(" + id + ")");
							}

							sql_query = "SELECT 'fake'";
						}
						if(action == "AJAX_updateTimecardApproverOrder")
						{
							auto	approver_id_list = split(new_value, ',');
							auto	i = 1;

							for(auto &approver_id: approver_id_list)
							{
								db->Query("UPDATE `timecard_approvers` SET `approver_order`=" + quoted(to_string(i++)) + " WHERE `id`=" + quoted(approver_id) + ";");
								if(db->isError())
								{
									error_message = "Ошибка БД";
									MESSAGE_ERROR("", "", "sql_query has failed (" + db->GetErrorMessage() + ")");
								}
							}

							sql_query = "SELECT 'fake';";
						}
						if(action == "AJAX_updateBTApproverOrder")
						{
							auto	approver_id_list = split(new_value, ',');
							auto	i = 1;

							for(auto &approver_id: approver_id_list)
							{
								db->Query("UPDATE `bt_approvers` SET `approver_order`=" + quoted(to_string(i++)) + " WHERE `id`=" + quoted(approver_id) + ";");
								if(db->isError())
								{
									error_message = "Ошибка БД";
									MESSAGE_ERROR("", "", "sql_query has failed (" + db->GetErrorMessage() + ")");
								}
							}

							sql_query = "SELECT 'fake';";
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
					error_message = gettext("field should not be empty");
					MESSAGE_ERROR("", "", error_message);
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
	auto	result = ""s;

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
	auto	result = ""s;

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
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	customer_id = db->Get(0, "timecard_customers_id");

		result = GetSpelledTimecardCustomerByID(customer_id, db) + TIMECARD_ENTRY_TITLE_SEPARATOR + title;
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
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	project_id = db->Get(0, "timecard_projects_id");

		result = GetSpelledTimecardProjectByID(project_id, db) + TIMECARD_ENTRY_TITLE_SEPARATOR + title;
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

auto	GetSpelledBT(string id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = ""s;

	if(db->Query("SELECT `place`, `purpose`, `date_start`, `date_end` FROM `bt` WHERE `id`=(" + id + ");"))
	{
		result = db->Get(0, "place") + " (" + db->Get(0, "purpose") + ") " + gettext("from") + " " + db->Get(0, "date_start") + " " + gettext("till") + " " + db->Get(0, "date_end");
	}
	else
	{
		MESSAGE_ERROR("", "", "bt.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (" + result + ")");

	return result;	
}

auto	GetSpelledTimecard(string id, CMysql *db) -> string
{
	MESSAGE_DEBUG("", "", "start");
	
	auto	result = ""s;

	if(db->Query("SELECT `period_start`, `period_end` FROM `timecards` WHERE `id`=(" + id + ");"))
	{
		result = db->Get(0, "period_start") + " - " + db->Get(0, "period_end");
	}
	else
	{
		MESSAGE_ERROR("", "", "timecards.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (" + result + ")");

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

auto	GetSpelledCostCenterByCustomFieldID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=(SELECT `cost_center_id` FROM `cost_center_custom_fields` WHERE `id`=\"" + id + "\");"))
	{
		result = db->Get(0, 0);
	}
	else
	{
		MESSAGE_ERROR("", "", "cost_centers.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTAllowanceByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `bt_allowance` WHERE `id`=\"" + id + "\");"))
	{
		result = db->Get(0, "title");
	}
	else
	{
		MESSAGE_ERROR("", "", "geo_country.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;	
}

auto	GetSpelledHolidayCalendarByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `holiday_calendar` WHERE `id`=" + quoted(id) + ";"))
	{
		result = db->Get(0, "title");
	}
	else
	{
		MESSAGE_ERROR("", "", "holiday_country.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;	
}

auto	GetSpelledAirfareDirectionLimitByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT * FROM `airfare_limits_by_direction` WHERE `id`=\"" + id + "\";"))
	{
		auto	from = db->Get(0, "from");
		auto	to = db->Get(0, "to");

		if(db->Query("SELECT * FROM `airport_countries` WHERE `id`=\"" + from + "\";"))
		{
			auto	from_title = db->Get(0, "title");

			if(db->Query("SELECT * FROM `airport_countries` WHERE `id`=\"" + to + "\";"))
			{
				auto	to_title = db->Get(0, "title");

				result = from_title + " -> "s + to_title;
			}
			else
			{
				MESSAGE_ERROR("", "", gettext("SQL syntax error"))
			}
		}
		else
		{
			MESSAGE_ERROR("", "", gettext("SQL syntax error"))
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "airfare_limits_by_direction.id(" + id + ") not found");
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

		result = title + " ("s + description + TIMECARD_ENTRY_TITLE_SEPARATOR + dom_type + TIMECARD_ENTRY_TITLE_SEPARATOR + payment + ")"s;
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
	auto	result = ""s;

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
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";"))
	{
		auto		bt_expense_template_id = db->Get(0, "bt_expense_template_id");

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
	auto	result = ""s;

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
	auto	result = ""s;

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
	auto	result = ""s;

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

string	GetSpelledSoWCustomFieldNameByID(string custom_field_id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`,`description` FROM `contract_sow_custom_fields` WHERE `id`=\"" + custom_field_id + "\";"))
	{
		auto	description = ""s + db->Get(0, "description");

		result = string(db->Get(0, "title")) + (description.length() ? TIMECARD_ENTRY_TITLE_SEPARATOR + description : "");
	}
	else
	{
		MESSAGE_ERROR("", "", "custom_field.id(" + custom_field_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledPSoWCustomFieldNameByID(string custom_field_id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`,`description` FROM `contract_psow_custom_fields` WHERE `id`=\"" + custom_field_id + "\";"))
	{
		auto	description = ""s + db->Get(0, "description");

		result = string(db->Get(0, "title")) + (description.length() ? TIMECARD_ENTRY_TITLE_SEPARATOR + description : "");
	}
	else
	{
		MESSAGE_ERROR("", "", "custom_field.id(" + custom_field_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledCostCenterCustomFieldNameByID(string custom_field_id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`,`description` FROM `cost_center_custom_fields` WHERE `id`=\"" + custom_field_id + "\";"))
	{
		auto	description = ""s + db->Get(0, "description");

		result = string(db->Get(0, "title")) + (description.length() ? TIMECARD_ENTRY_TITLE_SEPARATOR + description : "");
	}
	else
	{
		MESSAGE_ERROR("", "", "custom_field.id(" + custom_field_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledSoWByID(string sow_id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `sign_date`, `number` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
	{
		result = string(db->Get(0, "number")) + " " + gettext("agreement from") + " " + db->Get(0, "sign_date");
	}
	else
	{
		MESSAGE_ERROR("", "", "contract_sow.id(" + sow_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledPSoWByID(string psow_id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `sign_date`, `number` FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\";"))
	{
		result = string(db->Get(0, "number")) + " " + gettext("agreement from") + " " + db->Get(0, "sign_date");
	}
	else
	{
		MESSAGE_ERROR("", "", "contracts_psow.id(" + psow_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

static pair<string, string> GetNotificationDescriptionAndSoWQuery(string action, string id, string sow_id, string existing_value, string new_value, CMysql *db, CUser *user)
{
	string			notification_description, sql_query;

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
	if(action == "AJAX_updateCompanyVAT")
	{
		notification_description = "Данные компании: НДС компании изменился " + existing_value + " -> " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyVATCalculationType")
	{
		notification_description = "Данные компании: изменился способ расчета НДС " + existing_value + " -> " + new_value;
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
	if(
		action == "AJAX_updateCompanyActNumberPrefix"	||
		action == "AJAX_updateCompanyActNumber"			||
		action == "AJAX_updateCompanyActNumberPostfix"
	)
	{
		notification_description = "Данные компании: текущий номер акта " + existing_value + " изменился на " + new_value;
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
	if(action == "AJAX_updateCompanyCustomField")
	{
		auto field_name = ""s;

		if(db->Query("SELECT `title` FROM `company_custom_fields` WHERE `id`=" + quoted(id) + ";"))
		{
			field_name = db->Get(0, 0);
		}
		else
		{
			MESSAGE_ERROR("", "", "can't find custom name title by id(" + id + ")");
		}

		notification_description = gettext("Agency:") + " "s + gettext("additional field") + " " + field_name + " " + gettext("changed") + ": " + existing_value + " -> " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateAgencyPosition")
	{
		notification_description = gettext("Employee data") + ": "s + GetSpelledEmployeeByID(id, db) + " " + gettext("promouted from") + " " + existing_value;
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
	if(action == "AJAX_addTimecardApproverToPSoW")
	{
		auto	psow_id = id;
		notification_description = "Данные таймкарты: Добавили нового утвердителя " + GetSpelledUserNameByID(new_value, db) + " (PSoW " + GetSpelledPSoWByID(psow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_addBTExpenseApproverToPSoW")
	{
		auto	psow_id = id;
		notification_description = "Данные командировки: Добавили нового утвердителя " + GetSpelledUserNameByID(new_value, db) + " (PSoW " + GetSpelledPSoWByID(psow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteTimecardApproverFromPSoW")
	{
		auto	psow_id = id;
		notification_description = "Данные таймкарты: Удалили утвердителя таймкарт " + GetSpelledTimecardApproverNameByID(existing_value, db) + " (PSoW " + GetSpelledPSoWByID(psow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteBTExpenseApproverFromPSoW")
	{
		auto	psow_id = id;
		notification_description = "Данные командировки: Удалили утвердителя командировочных расходов " + GetSpelledBTExpenseApproverNameByID(existing_value, db) + " (PSoW " + GetSpelledPSoWByID(psow_id, db) + ")";
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
	if(action == "AJAX_addAirfareLimitationByDirection")
	{
		notification_description = "Ограничения на перелет: добавили лимит на перелет "  + GetSpelledAirfareDirectionLimitByID(id, db) + " " + new_value + " руб.";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateAirfareLimitationByDirection")
	{
		notification_description = "Ограничения на перелет: изменили лимит на перелет "  + GetSpelledAirfareDirectionLimitByID(id, db) + " с " + existing_value + " на " + new_value + " руб.";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteAirfarelimitationByDirection")
	{
		notification_description = "Ограничения на перелет: удалили лимит на перелет "  + GetSpelledAirfareDirectionLimitByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addCostCenter")
	{
		notification_description = gettext("Agency: cost center added") + " "s + GetSpelledCostCenterByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterToCustomer")
	{
		notification_description = gettext("Agency: cost center belongs to customer") + " "s + GetSpelledTimecardCustomerByID(id, db) + " "s + gettext("updated") + " "s + (existing_value.length() ? gettext("from") + " "s + GetSpelledCostCenterByID(existing_value, db) : "") + " "s + gettext("to") + " "s + GetSpelledCostCenterByID(new_value, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteCostCenterFromCustomer")
	{
		notification_description = gettext("Agency: cost center belongs to customer") + " "s + GetSpelledTimecardCustomerByID(id, db) + " "s + (existing_value.length() ? " ("s + GetSpelledCostCenterByID(existing_value, db) : "") + ") "s + gettext("removed");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteCostCenter")
	{
		notification_description = gettext("Agency: cost center") + " ("s + GetSpelledCostCenterByID(existing_value, db) + ") "s + gettext("removed");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterTitle")
	{
		notification_description = gettext("Agency: cost center title updated") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterDescription")
	{
		notification_description = gettext("Agency: cost center")  + " ("s + GetSpelledCostCenterByID(id, db) + ") "  + gettext("description updated") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateBTAllowanceCountry")
	{
		notification_description = gettext("Agency: bt allowance") + " ("s + GetSpelledBTAllowanceByID(id, db) + ") " + gettext("updated(female)") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateBTAllowanceAmount")
	{
		notification_description = gettext("Agency: bt allowance") + " ("s + GetSpelledBTAllowanceByID(id, db) + ") " + gettext("updated(female)") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteBTAllowance")
	{
		notification_description = gettext("Agency: bt allowance") + " ("s + GetSpelledBTAllowanceByID(id, db) + ") " + gettext("removed");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addBTAllowance")
	{
		notification_description = gettext("Agency: bt allowance") + " ("s + GetSpelledBTAllowanceByID(id, db) + ") " + gettext("added");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateTimecardApproverOrder")
	{
		notification_description = gettext("Agency: bt approver") + " "s + gettext("updated");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateBTApproverOrder")
	{
		notification_description = gettext("Agency: bt approver") + " "s + gettext("updated");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateHolidayCalendarDate")
	{
		notification_description = gettext("Agency: holiday calendar") + " "s + gettext("updated") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=(SELECT `agency_company_id` FROM `holiday_calendar` WHERE `id`=" + quoted(id) + ");";
	}
	if(action == "AJAX_updateHolidayCalendarTitle")
	{
		notification_description = gettext("Agency: holiday calendar") + " "s + gettext("updated") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=(SELECT `agency_company_id` FROM `holiday_calendar` WHERE `id`=" + quoted(id) + ");";
	}
	if(action == "AJAX_deleteHolidayCalendar")
	{
		notification_description = gettext("Agency: holiday calendar") + " ("s + GetSpelledHolidayCalendarByID(id, db) + ") " + gettext("removed");
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=(SELECT `agency_company_id` FROM `holiday_calendar` WHERE `id`=" + quoted(id) + ");";
	}
	if(action == "AJAX_addHolidayCalendar")
	{
		notification_description = gettext("Agency: holiday calendar") + " "s + gettext("added") + " ("s + GetSpelledHolidayCalendarByID(id, db) + ") ";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=(SELECT `agency_company_id` FROM `holiday_calendar` WHERE `id`=" + quoted(id) + ");";
	}



	if(action == "AJAX_updateSoWNumber")
	{
		notification_description = gettext("SoW: number") + " "s + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWAct")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("updated") + " " + gettext("act number") + " " + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWPosition")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("position changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWDayRate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("dayrate changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWSignDate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("sign date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWStartDate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("start date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWEndDate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("end date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWPaymentPeriodService")
	{
		char	buffer[50];
		sprintf(buffer, ngettext("%d days", "%d days", stoi(new_value)), stoi(new_value));

		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("service period payment") + " " + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + buffer;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWPaymentPeriodBT")
	{
		char	buffer[50];
		sprintf(buffer, ngettext("%d days", "%d days", stoi(new_value)), stoi(new_value));

		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("bt period payment") + " " + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + buffer;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWCustomField")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("custom field") + "(" + GetSpelledSoWCustomFieldNameByID(id, db) + ") " + gettext("changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
		if(user->GetType() == "agency")
			sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteSoWCustomField")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("custom field") + "(" + existing_value + ") " + gettext("removed");
		// sql_query = "SELECT `contract_sow_id` AS `contract_sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}

	if(action == "AJAX_updatePSoWNumber")
	{
		notification_description = gettext("PSoW: number") + " "s + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWAct")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("updated") + " " + gettext("act number") + " " + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWPosition")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("position changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWDayRate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("dayrate changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWBTMarkup")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("bt markup changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWBTMarkupType")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("bt markup type changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWSignDate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("sign date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWStartDate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("start date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWEndDate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("end date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWCustomField")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("custom field") + "(" + GetSpelledPSoWCustomFieldNameByID(id, db) + ") " + gettext("changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deletePSoWCustomField")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("custom field") + "(" + existing_value + ") " + gettext("removed");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}

	if(action == "AJAX_updateCostCenterNumber")
	{
		notification_description = gettext("Cost Center: number") + " "s + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterAct")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(id, db) + "): " + gettext("updated") + " " + gettext("act number") + " " + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterSignDate")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(id, db) + "): " + gettext("sign date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterStartDate")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(id, db) + "): " + gettext("start date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterEndDate")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(id, db) + "): " + gettext("end date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterCustomField")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByCustomFieldID(id, db) + "): " + gettext("custom field") + "(" + GetSpelledCostCenterCustomFieldNameByID(id, db) + ") " + gettext("changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_recallTimecard")
	{
		notification_description = gettext("Timecard") + " "s + GetSpelledTimecard(id, db) + " " + gettext("in scope") + " " + GetSpelledSoWByID(sow_id, db) + " " + gettext("recalled") + " " + GetSpelledUserNameByID(user->GetID(), db) + ": " + new_value + "";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}



	if(action == "AJAX_updateExpenseTemplateTitle")
	{
		notification_description = "Данные командировки: изменилось название расхода с " + existing_value + " на " + new_value;
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + id + "\";";
	}
	if(action == "AJAX_updateExpenseTemplateTaxable")
	{
		notification_description = "Данные командировки: изменилась налогооблагаемость расхода с " + existing_value + " на " + new_value;
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

	if(action == "AJAX_updateTimecardOriginalDocumentsDelivery")
	{
		if(new_value == "0")
 			notification_description = gettext("Timecard") + " ("s + GetSpelledTimecard(id, db) + "): " + gettext("original docs has not been received");
		else
 			notification_description = gettext("Timecard") + " ("s + GetSpelledTimecard(id, db) + "): " + GetSpellingDate(stol(new_value)) + " " + gettext("original docs received");
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}

	if(action == "AJAX_updateBTOriginalDocumentsDelivery")
	{
		if(new_value == "0")
 			notification_description = gettext("BT") + " ("s + GetSpelledBT(id, db) + "): " + gettext("original docs has not been received");
		else
 			notification_description = gettext("BT") + " ("s + GetSpelledBT(id, db) + "): " + GetSpellingDate(stol(new_value)) + " " + gettext("original docs received");
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}


	MESSAGE_DEBUG("", "", "finish");

	return make_pair(notification_description, sql_query);
}

auto NotifySoWContractPartiesAboutChanges(string action_type_id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto				error_message = "";

	MESSAGE_DEBUG("", "", "start (action_type_id=" + action_type_id + ", sow_id=" + sow_id + ")");

	if(sow_id.length())
	{
		if(action_type_id.length())
		{
			vector<string>		user_list;
			auto				affected = db->Query("SELECT `user_id` FROM `company_employees` where `company_id`=("
														"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\""
													")");
			for(auto i = 0; i < affected; ++i)
			{
				user_list.push_back(db->Get(i, 0));
			}

			affected = db->Query("SELECT `admin_userID` FROM `company` where `id`=("
									"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\""
								")");
			for(auto i = 0; i < affected; ++i)
			{
				user_list.push_back(db->Get(i, 0));
			}

			for(auto &user_id: user_list)
			{
				if(db->InsertQuery(
									"INSERT INTO `users_notification` (`userId`, `title`, `actionTypeId`, `actionId`, `eventTimestamp`) VALUES "
									"(\"" + user_id + "\", \"\", \"" + action_type_id + "\", \"" + sow_id + "\", UNIX_TIMESTAMP())"
									))
				{
					// --- success
				}
				else
				{
					error_message = gettext("SQL syntax error");
					MESSAGE_ERROR("", "", "fail to insert to db");
					break;
				}
			}
		}
		else
		{
			error_message = gettext("parameters incorrect");
			MESSAGE_ERROR("", "", "action_type_id is empty");
		}
	}
	else
	{
		error_message = gettext("parameters incorrect");
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message + ")");

	return error_message;	
}

auto NotifyAgencyAboutChanges(string agency_id, string action_type_id, string action_id, CMysql *db, CUser *user) -> string
{
	auto				error_message = "";

	MESSAGE_DEBUG("", "", "start (agency_id=" + agency_id + ", action_type_id=" + action_type_id + ", action_id=" + action_id + ")");

	if(agency_id.length())
	{
		if(action_type_id.length())
		{
			vector<string>		user_list;
			auto				affected = db->Query("SELECT `user_id` FROM `company_employees` where `company_id`=\"" + agency_id + "\";");
			for(auto i = 0; i < affected; ++i)
			{
				user_list.push_back(db->Get(i, 0));
			}

			for(auto &user_id: user_list)
			{
				if(db->InsertQuery(
									"INSERT INTO `users_notification` (`userId`, `title`, `actionTypeId`, `actionId`, `eventTimestamp`) VALUES "
									"(\"" + user_id + "\", \"\", \"" + action_type_id + "\", \"" + action_id + "\", UNIX_TIMESTAMP())"
									))
				{
					// --- success
				}
				else
				{
					error_message = gettext("SQL syntax error");
					MESSAGE_ERROR("", "", "fail to insert to db");
					break;
				}
			}
		}
		else
		{
			error_message = gettext("parameters incorrect");
			MESSAGE_ERROR("", "", "action_type_id is empty");
		}
	}
	else
	{
		error_message = gettext("parameters incorrect");
		MESSAGE_ERROR("", "", "agency_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message + ")");

	return error_message;	
}

static vector<string> GetAgencyListFromQuery(const string &sql_query, CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start (" + sql_query + ")");

	vector<string>		agency_list;

	// --- craft agency list to notify
	auto affected = db->Query(sql_query);

	for(auto i = 0; i < affected; ++i) 
		agency_list.push_back(db->Get(i, "agency_company_id"));

	// --- define agency
	// --- ERROR, if more than one agency. Mean that they have shared CU/proj/task - FORBIDDEN
	if((user->GetType() == "agency") && (agency_list.size() > 1))
	{
		MESSAGE_ERROR("", "", "Two or more agencies are sharing entities (either Customer/Project/Task or BTexpense/line templates) - FORBIDDEN");
	}

	// --- if agency must be notified but no SoW-s singed, then try to define AgencyID by UserID
	if((user->GetType() == "agency") && (agency_list.empty()))
	{
		auto	agency_id = GetValueFromDB(Get_AgencyIDByUserID_sqlquery(user->GetID()), db);

		if(agency_id.length()) agency_list.push_back(agency_id);
	}


	MESSAGE_DEBUG("", "", "finish (size of agency_list is " + to_string(agency_list.size()) + ")");

	return agency_list;
}

bool GeneralNotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *db, CUser *user)
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start (action=" + action + ")");

	if(id.length())
	{
		if(action.length())
		{
			// string		actionTypeId;
			auto		notification_description	= ""s;
			auto		sow_list_sql_query			= ""s;
			auto		agency_list_sql_query		= ""s;
			auto		actionTypeId				= ""s;

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

					for(auto i = 0; i < affected; ++i) 
						sow_list.push_back(db->Get(i, "contract_sow_id"));

					if(sow_list.size())
					{
						// --- NOW() can't be used here because NOW()-type is datetime, but CURDATE()-type is date
						// --- for example SELECT NOW()=CURDATE() gives false, 
						// --- reason behind is SELECT "YYYY-MM-DD HH:MM:SS" NOT equal "YYYY-MM-DD" due to type inconsistency
						// --- in comparison opertions "datetime" can't be compared to "date"
						agency_list_sql_query = "SELECT DISTINCT `agency_company_id` FROM `contracts_sow` WHERE `id` IN (" + join(sow_list) + ") AND (`start_date`<=CURDATE()) AND (CURDATE()<=`end_date`);";
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
/*					// --- craft agency list to notify
					affected = db->Query(agency_list_sql_query);

					for(auto i = 0; i < affected; ++i) 
						agency_list.push_back(db->Get(i, "agency_company_id"));

					// --- define agency
					// --- ERROR, if more than one agency. Mean that they have shared CU/proj/task - FORBIDDEN
					if((user->GetType() == "agency") && (agency_list.size() > 1))
					{
						MESSAGE_ERROR("", "", "Two or more agencies are sharing entities (either Customer/Project/Task or BTexpense/line templates) (" + action + ":" + id + ") - FORBIDDEN");
					}

					// --- if agency must be notified but no SoW-s singed, then try to define AgencyID by UserID
					if((user->GetType() == "agency") && (agency_list.empty()))
					{
						auto	agency_id = GetValueFromDB(Get_AgencyIDByUserID_sqlquery(user->GetID()), db);

						if(agency_id.length()) agency_list.push_back(agency_id);
					}
*/
					agency_list = GetAgencyListFromQuery(agency_list_sql_query, db, user);

					if(agency_list.size())
					{
						auto				initiator_company_id = ""s;
						vector<string>		persons_to_notify;
						string				notificator_company_id = "";

						if(user->GetType() == "agency")
						{
							actionTypeId = to_string(NOTIFICATION_GENERAL_FROM_COMPANY);
							if(db->Query("" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ";"))
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
							for(auto i = 0; i < affected; ++i)
								persons_to_notify.push_back(db->Get(i, "user_id"));
							
							// --- gather subcontractor list
							if(sow_list.size())
							{
								affected = db->Query("SELECT DISTINCT `admin_userID` FROM `company` WHERE `id` IN ("
												"SELECT DISTINCT `subcontractor_company_id` FROM `contracts_sow` WHERE `id` IN (" + join(sow_list) + ")"
											");");
								for(auto i = 0; i < affected; ++i)
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
						MESSAGE_ERROR("", "", "Contract entities list is empty (probably list is empty due to no signed SoW-s yet.)");
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
	auto	result = ""s;
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
			for(auto i = 0; i < affected; ++i)
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
	auto	result = ""s;
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
							"\"cost_center_assignment\":[" + ( include_tasks ? GetCostCentersAssignmentInJSONFormat("SELECT * FROM `cost_center_assignment` WHERE `timecard_customer_id` IN (SELECT `id` FROM `timecard_customers` WHERE `agency_company_id`=\"" + agency_id + "\");", db, user) : "") + "],"
							
							"\"bt_expense_templates\":[" + ( include_bt ? GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `agency_company_id`=\"" + agency_id + "\";", db, user) : "") + "],"
							"\"airfare_limitations_by_direction\":[" + ( include_bt ? GetAirfareLimitaionsByDirectionInJSONFormat("SELECT * FROM `airfare_limits_by_direction` WHERE `agency_company_id`=\"" + agency_id + "\";", db, user) : "") + "],"
							"\"allowances\":[" + ( include_bt ? GetBTAllowanceInJSONFormat("SELECT * FROM `bt_allowance` WHERE `agency_company_id`=" + quoted(agency_id) + ";", db, user) : "") + "],"
							"\"holiday_calendar\":[" + ( include_tasks ? GetHolidayCalendarInJSONFormat("SELECT * FROM `holiday_calendar` WHERE `agency_company_id`=" + quoted(agency_id) + ";", db, user) : "") + "]"
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

static auto __GetNumberOfTimecards(const string &timecard_status, const string &period_type, const string &period_start, const string &period_end, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = ""s;

	if(user->GetType() == "agency")
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecards` WHERE "
							"`status` IN (" + timecard_status + ") "
							"AND "
							"`period_end`>=\"" + period_start + "\"  "
							"AND "
							"`period_end`<=\"" + period_end + "\" "
							"AND "
							"`contract_sow_id` IN ("
								"SELECT `id` FROM `contracts_sow` WHERE "
									"`timecard_period`=" + quoted(period_type) + " "
									"AND "
									"`agency_company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ""
							")"
						");"))
		{
			result = db->Get(0, "counter");
		}
		else
		{
			MESSAGE_ERROR("", "", "SQL syntax error");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfApprovedTimecardsThisMonth(CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfThisMonth();

	auto	result = __GetNumberOfTimecards("\"approved\"", "month", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfApprovedTimecardsLastMonth(CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfLastMonth();

	auto	result = __GetNumberOfTimecards("\"approved\"", "month", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfApprovedTimecardsThisWeek(CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfThisWeek();

	auto	result = __GetNumberOfTimecards("\"approved\"", "week", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfApprovedTimecardsLastWeek(CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfLastWeek();

	auto	result = __GetNumberOfTimecards("\"approved\"", "week", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

static auto __GetNumberOfSoW(const string &sow_status, const string &period_type, const string &period_start, const string &period_end, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = ""s;

	if(user->GetType() == "agency")
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `contracts_sow` WHERE "
								"`status` IN (" + sow_status + ") "
								"AND "
								"`timecard_period`=\"" + period_type + "\"  "
								"AND "
								"`end_date`>=\"" + period_start + "\"  "
								"AND "
								"`start_date`<=\"" + period_end + "\" "
								"AND "
								"`agency_company_id`=(" + Get_CompanyIDByUserID_sqlquery(user->GetID()) + ")"
					))
		{
			result = db->Get(0, "counter");
		}
		else
		{
			MESSAGE_ERROR("", "", "SQL syntax error");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfSoWActiveThisMonth(CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfThisMonth();

	auto	result = __GetNumberOfSoW("\"signed\",\"expired\"", "month", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfSoWActiveLastMonth(CMysql *db, CUser *user) -> string
{

	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfLastMonth();

	auto	result = __GetNumberOfSoW("\"signed\",\"expired\"", "month", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;

}

auto GetNumberOfSoWActiveThisWeek(CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfThisWeek();

	auto	result = __GetNumberOfSoW("\"signed\",\"expired\"", "week", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfSoWActiveLastWeek(CMysql *db, CUser *user) -> string
{

	MESSAGE_DEBUG("", "", "start");

	struct tm	period_start, period_end;

	tie(period_start, period_end) = GetFirstAndLastDateOfLastWeek();

	auto	result = __GetNumberOfSoW("\"signed\",\"expired\"", "week", PrintSQLDate(period_start), PrintSQLDate(period_end), db, user);

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;

}

auto GetPSoWIDByTimecardIDAndCostCenterID(string timecard_id, string cost_center_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(user->GetType() == "agency")
		{
			if(db)
			{
				if(db->Query(
					"SELECT `id` FROM `contracts_psow` WHERE `cost_center_id`=\"" + cost_center_id + "\" AND `contract_sow_id`=("
						"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\""
					")"
				))
				{
					result = db->Get(0, "id");
				}
				else
				{
					MESSAGE_ERROR("", "", "can't define PSoW by timecard.id(" + timecard_id + ") and cost_center.id(" + cost_center_id + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "db not initialized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") is not an agency employee (" + user->GetType() + ")");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetPSoWIDByBTIDAndCostCenterID(string bt_id, string cost_center_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(user->GetType() == "agency")
		{
			if(db)
			{
				if(db->Query(
					"SELECT `id` FROM `contracts_psow` WHERE `cost_center_id`=\"" + cost_center_id + "\" AND `contract_sow_id`=("
						"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\""
					")"
				))
				{
					result = db->Get(0, "id");
				}
				else
				{
					MESSAGE_ERROR("", "", "can't define PSoW by bt.id(" + bt_id + ") and cost_center.id(" + cost_center_id + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "db not initialized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") is not an agency employee (" + user->GetType() + ")");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetSoWIDByTimecardID(string timecard_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(db->Query("SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
			{
				result = db->Get(0, "contract_sow_id");
			}
			else
			{
				MESSAGE_DEBUG("", "", "timecard(" + timecard_id + ") not found");
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

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetSoWIDByPSoWID(string psow_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(db->Query("SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\";"))
			{
				result = db->Get(0, 0);
			}
			else
			{
				MESSAGE_DEBUG("", "", "psow_id(" + psow_id + ") not found");
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

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetPSoWIDByApprover(string sql_query, CMysql *db, CUser *user) -> string
{
	return GetValueFromDB(sql_query, db);
}
