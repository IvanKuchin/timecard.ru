#include "utilities_timecard.h"

auto	GetServiceInvoicesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	auto	affected = 0;
	auto	result = ""s;;

	struct ItemClass
	{
		string		id;
		string		cost_center_id;
		string		file;
		string		owner_user_id;
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
					unlink((INVOICES_CC_DIRECTORY + file_name).c_str());
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
