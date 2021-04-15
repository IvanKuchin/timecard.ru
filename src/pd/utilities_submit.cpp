#include "utilities_submit.h"

string	ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, c_config *config, CMysql *db, CUser *user)
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
							auto	item_list = GetValuesFromDB("SELECT `id` FROM `timecards` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";", db);

							for (auto &timecard_id: item_list)
							{
								if(SubmitTimecard(timecard_id, config, db, user))
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
							auto	item_list = GetValuesFromDB("SELECT `id` FROM `bt` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";", db);

							for (auto &bt_id: item_list)
							{
								if(SubmitBT(bt_id, config, db, user))
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

static auto __isActNumberAvailable(const string &act_full_number, const string &company_id, CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start (" + act_full_number + ", " + company_id + ")");

	auto	error_message = ""s;
	auto	result = true;

	if(company_id.length() && db)
	{
		auto	company_type	= GetValueFromDB("SELECT `type` FROM `company` WHERE `id`=" + quoted(company_id) + ";", db);


		if(company_type == "subcontractor")
		{
			auto	affected = db->Query("SELECT `id` FROM `invoices` WHERE `full_number`=" + quoted(act_full_number) + " AND `id` IN (" + Get_ActsIDBySubcCompanyID(company_id) + ");");

			if(affected > 0)
			{
				result = false;
			}
		}
		else if(company_type == "agency")
		{
			auto	affected = db->Query("SELECT `id` FROM `invoices` WHERE `full_number`=" + quoted(act_full_number) + " AND `id` IN (" + Get_ActsIDByAgencyCompanyID(company_id) + ");");

			if(affected > 0)
			{
				result = false;
			}
		}
		else
		{
			error_message = gettext("entity type is unknown");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

// returns act_number start from act_number
// if company must have unique act numbers then this will be taken into account
// inputs:
//		act_number: notice that it is number only
//		company_id
// outputs:
//		tuple(act_prefix, act_number, act_postfix)
auto GetAvailableActNumber_StartFrom(int act_number, const string &company_id, CMysql *db, CUser *user) -> tuple<string, int, string>
{
	MESSAGE_DEBUG("", "", "start (" + to_string(act_number) + ", " + company_id + ")");

	auto	act_number_prefix	= ""s;
	auto	act_number_postfix	= ""s;

	if(company_id.length() && db)
	{
		if(db->Query("SELECT `act_number_unique`, `act_number_prefix`, `act_number_postfix` FROM `company` WHERE `id`=" + quoted(company_id) + ";"))
		{
			auto	act_number_unique	= db->Get(0, "act_number_unique") == "true"s;
			act_number_prefix	= db->Get(0, "act_number_prefix");
			act_number_postfix	= db->Get(0, "act_number_postfix");

			while(act_number_unique)
			{
				if(__isActNumberAvailable(act_number_prefix + to_string(act_number) + act_number_postfix, company_id, db, user))
				{
					break;
				}
				else
				{
					++act_number;
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", gettext("company not found"));
		}
	}
	else
	{
		MESSAGE_ERROR("", "", gettext("mandatory parameter missed"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return make_tuple<string, int, string>(string(act_number_prefix), int(act_number), string(act_number_postfix));
	// return make_tuple<string, int, string>(act_number_prefix, act_number, act_number_postfix);
}

auto GetActNumberByCompanyID(const string &company_id, CMysql *db, CUser *user) -> tuple<string, int, string>
{
	return GetAvailableActNumber_StartFrom(
											stoi(GetValueFromDB("SELECT `act_number` FROM `company` WHERE `id`=(" + company_id + ");", db)),
											company_id, 
											db,
											user
										);
}

auto AssignCurrentCompanyActNumberToActID_And_UpdateCompanyActNumber_by_ActID(const string &act_id, const string &company_id, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start (" + act_id + ", " + company_id + ")");

	auto	error_message	= ""s;

	auto	curr_act_full_number	= GetActNumberByCompanyID(company_id, db, user);
	auto	next_act_full_number	= GetAvailableActNumber_StartFrom(get<1>(curr_act_full_number) + 1, company_id, db, user);

	db->Query("UPDATE `invoices` SET `full_number`=" + quoted(get<0>(curr_act_full_number) + to_string(get<1>(curr_act_full_number)) + get<2>(curr_act_full_number)) + ",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=" + quoted(act_id) + ";");
	if(db->isError())
	{
		error_message = gettext("act not found");
		MESSAGE_ERROR("", "", error_message);
	}
	else
	{
		db->Query("UPDATE `company` SET `act_number`=" + quoted(to_string(get<1>(next_act_full_number))) + " WHERE `id`=" + quoted(company_id) + ";");

		if(db->isError())
		{
			error_message = gettext("SQL syntax error");
			MESSAGE_ERROR("", "", error_message);
		}
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

// update act in DB with current company.act_number and shift company.act_number to the next available
// IMPORTANT: it assumes that act already exists in DB
// inputs:
//		entity_id: (timecard_id|bt_id) 
//		entity_type: (timecard|bt) 
//		stage: (submitted|approved) - not in use currently 
// outputs:
//		error_message
auto AssignCurrentCompanyActNumberToActID_And_UpdateCompanyActNumber_by_Entity(const string &entity_id, const string &entity_type, const string &stage, CMysql *db, CUser *user) -> string
{
	MESSAGE_DEBUG("", "", "start (" + entity_id + ", " + entity_type + ", " + stage + ")");

	auto	error_message	= ""s;

	if(entity_id.length() && entity_type.length() && stage.length() && db)
	{
		auto	company_id				= GetValueFromDB(entity_type == "timecard" ? Get_SubcCompanyIDByTimecardID_sqlquery(entity_id) : Get_SubcCompanyIDByBTID_sqlquery(entity_id), db);
		auto	act_id					= ""s;

		if(entity_type == "timecard")
		{
			act_id = GetValueFromDB(Get_ActIDByTimecardID(entity_id), db);
		}
		else if(entity_type == "bt")
		{
			act_id = GetValueFromDB(Get_ActIDByBTID(entity_id), db);
		}
		else
		{
			error_message = gettext("entity type is unknown");
			MESSAGE_ERROR("", "", error_message);
		}

		if(act_id.length())
		{
			error_message = AssignCurrentCompanyActNumberToActID_And_UpdateCompanyActNumber_by_ActID(act_id, company_id, db, user);
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			error_message = gettext("entity type is unknown");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto SubmitTimecard(string timecard_id, c_config *config, CMysql *db, CUser *user) -> bool
{
	MESSAGE_DEBUG("", "", "start");

	auto 			result = false;

	if(timecard_id.length())
	{
		C_TC_BT_Submit	submit_obj(db, user);

		submit_obj.Submit("timecard", timecard_id);
		if(submit_obj.isCompletelyApproved())
		{
				auto								error_message = ""s;
				C_Invoice_Service_Subc_To_Agency	c_invoice(config, db, user);
				
				c_invoice.SetTimecardList({timecard_id});
				error_message = c_invoice.GenerateDocumentArchive();

				if(error_message.empty())
				{
					db->Query("UPDATE `timecards` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
					if(db->isError())
					{
						error_message = gettext("SQL syntax error");
						MESSAGE_ERROR("", "", "fail to update timecards table with timecard_id(" + timecard_id + ")");
					}
					else
					{
						error_message = AssignCurrentCompanyActNumberToActID_And_UpdateCompanyActNumber_by_Entity(timecard_id, "timecard", "submit", db, user);

						if(error_message.empty())
						{
							result = true;
						}
						else
						{
							MESSAGE_ERROR("", "", error_message);
						}
					}
				}
				else
				{
					error_message = gettext("fail to generate documents set");
					MESSAGE_ERROR("", "", error_message);
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

auto	SubmitBT(string bt_id, c_config *config, CMysql *db, CUser *user) -> bool
{
	auto 	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(bt_id.length())
	{
		C_TC_BT_Submit	submit_obj(db, user);

		submit_obj.Submit("bt", bt_id);
		if(submit_obj.isCompletelyApproved())
		{
				auto							error_message = ""s;
				C_Invoice_BT_Subc_To_Agency		c_invoice(config, db, user);

				c_invoice.SetBTList({bt_id});

				error_message = c_invoice.GenerateDocumentArchive();
				if(error_message.empty())
				{
					db->Query("UPDATE `bt` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + bt_id + "\";");
					if(db->isError())
					{
						MESSAGE_ERROR("", "", "fail to update bt table with bt_id(" + bt_id + ")");
					}
					else
					{
						error_message = AssignCurrentCompanyActNumberToActID_And_UpdateCompanyActNumber_by_Entity(bt_id, "bt", "submit", db, user);

						if(error_message.empty())
						{
							result = true;
						}
						else
						{
							MESSAGE_ERROR("", "", error_message);
						}
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", "fail to generate invoice document archive");
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
