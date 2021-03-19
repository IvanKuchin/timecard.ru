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
					db->Query	("UPDATE `company` SET `act_number`=`act_number`+1 WHERE `id`=(" + Get_SubcCompanyIDByTimecardID_sqlquery(timecard_id) + ");");
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
					db->Query	("UPDATE `company` SET `act_number`=`act_number`+1 WHERE `id`=(" + Get_SubcCompanyIDByBTID_sqlquery(bt_id) + ");");
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
