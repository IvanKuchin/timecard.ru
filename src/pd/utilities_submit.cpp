#include "utilities_submit.h"

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
							auto	item_list = GetValuesFromDB("SELECT `id` FROM `timecards` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";", db);

							for (auto &timecard_id: item_list)
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
							auto	item_list = GetValuesFromDB("SELECT `id` FROM `bt` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";", db);

							for (auto &bt_id: item_list)
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

bool SubmitTimecard(string timecard_id, CMysql *db, CUser *user)
{
	MESSAGE_DEBUG("", "", "start");

	auto 			result = false;

	if(timecard_id.length())
	{

/*
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
		bool						all_approvers_confirmed = true;

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
			if(!approver.second) { all_approvers_confirmed = false; break; }
		}

		if(all_approvers_confirmed)
*/
		C_TC_BT_Submit	submit_obj(db, user);

		submit_obj.Submit("timecard", timecard_id);
		if(submit_obj.isCompletelyApproved())
		{
			auto	error_message = ""s;

			C_Invoice_Service_Subc_To_Agency	c_invoice(db, user);
			
			c_invoice.SetTimecardList({timecard_id});
			error_message = c_invoice.GenerateDocumentArchive();

			if(error_message.empty())
			{
				db->Query("UPDATE `timecards` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to update timecards table with timecard_id(" + timecard_id + ")");
				}
				else
				{
					db->Query	("UPDATE `company` SET `act_number`=`act_number`+1 WHERE `id`=("
									"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=("
										"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\""
									")"
								");");
					if(db->isError())
					{
						MESSAGE_ERROR("", "", "fail to update contracts_sow table by timecard_id(" + timecard_id + ")");
					}
					else
					{
						result = true;
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

bool	SubmitBT(string bt_id, CMysql *db, CUser *user)
{
	bool 	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(bt_id.length())
	{
/*
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
		bool						all_approvers_confirmed = true;

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
			if(!approver.second) { all_approvers_confirmed = false; break; }
		}

		if(all_approvers_confirmed)
*/
		C_TC_BT_Submit	submit_obj(db, user);

		submit_obj.Submit("bt", bt_id);
		if(submit_obj.isCompletelyApproved())
		{
			auto	error_message = ""s;
			C_Invoice_BT_Subc_To_Agency	c_invoice(db, user);

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
					db->Query	("UPDATE `company` SET `act_number`=`act_number`+1 WHERE `id`=("
									"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=("
										"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\""
									")"
								");");
					if(db->isError())
					{
						MESSAGE_ERROR("", "", "fail to update contracts_sow table by bt_id(" + bt_id + ")");
					}
					else
					{
						result = true;
					}
				}

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
