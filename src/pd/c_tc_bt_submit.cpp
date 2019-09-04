#include "c_tc_bt_submit.h"

bool		C_TC_BT_Submit::isOrderedApprovals()
{
	return approvers.size() && (approvers[0].approver_order != "0");
}

string		C_TC_BT_Submit::FetchApprovers()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	if(db)
	{
		auto affected = db->Query(
				(type == "timecard")? "SELECT * FROM `timecard_approvers` WHERE "
											"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + id + "\") ORDER BY `approver_order` ASC"
										";" :
				(type == "bt")		? "SELECT * FROM `bt_approvers` WHERE "
											"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + id + "\") ORDER BY `approver_order` ASC"
										";" :
				"SELECT 'fake'"
			);

		for(auto i = 0; i < affected; i++)
		{
			ApproverClass	item;

			item.id					= db->Get(i, "id");
			item.approver_user_id	= db->Get(i, "approver_user_id");
			item.contract_sow_id	= db->Get(i, "contract_sow_id");
			item.approver_order		= db->Get(i, "approver_order");
			item.auto_approve		= db->Get(i, "auto_approve");

			approvers.push_back(item);
		}
	}
	else
	{
		error_message = gettext("DB is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finsh");

	return error_message;
}

string		C_TC_BT_Submit::FetchApprovals()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	if(db)
	{
		auto affected = db->Query(
				(type == "timecard")? "SELECT * FROM `timecard_approvals` WHERE "
											"`eventTimestamp` > (SELECT `submit_date` FROM `timecards` WHERE `id`=\"" + id + "\") "
											"AND "
											"`timecard_id`=\"" + id + "\" ORDER BY `eventTimestamp` ASC"
										";" :
				(type == "bt")		? "SELECT * FROM `bt_approvals` WHERE "
											"`eventTimestamp` > (SELECT `submit_date` FROM `bt` WHERE `id`=\"" + id + "\") "
											"AND "
											"`bt_id`=\"" + id + "\" ORDER BY `eventTimestamp` ASC"
										";" :
				"SELECT 'fake'"
			);

		approvals.empty();

		for(auto i = 0; i < affected; i++)
		{
			ApproveClass	item;

			item.id				= db->Get(i, "id");
			item.timecard_id	= (type == "timecard") ? db->Get(i, "timecard_id") : db->Get(i, "bt_id");
			item.approver_id	= db->Get(i, "approver_id");
			item.decision		= db->Get(i, "decision");
			item.comment		= db->Get(i, "comment");
			item.eventTimestamp	= db->Get(i, "eventTimestamp");

			approvals.push_back(item);
		}

	}
	else
	{
		error_message = gettext("DB is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finsh");

	return error_message;
}

string		C_TC_BT_Submit::FetchInitialData()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = FetchApprovals();

	if(error_message.empty())
	{
		error_message = FetchApprovers();
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finsh");

	return error_message;
}

bool		C_TC_BT_Submit::isItFirstSubmit()
{
	MESSAGE_DEBUG("", "", "start(approvers.size() = " + to_string(approvers.size()) +  ", approval.empty() = " + to_string(approvals.empty()) + ")");

	bool	result = approvers.size() && approvals.empty();

	MESSAGE_DEBUG("", "", "finish(" + to_string(result) + ")");

	return result;
}

bool		C_TC_BT_Submit::isItBeenRejected()
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = false;

	for(auto &approve: approvals)
	{
		if(approve.decision == "rejected")
		{
			result = true;
			break;
		}
	}

	MESSAGE_DEBUG("", "", "finish(" + to_string(result) + ")");

	return result;
}

string		C_TC_BT_Submit::CraftApprovalList()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	if(isOrderedApprovals())
	{
		// --- ordered approvals
		auto	next_approver_id = GetNextApproverID();
		if(next_approver_id.length())
		{
			if(db->InsertQuery("INSERT INTO `" + type + "_approvals` (`" + type + "_id`, `approver_id`, `decision`, `comment`, `eventTimestamp`) VALUES (\"" + id + "\", \"" + next_approver_id + "\", \"pending\", \"\", UNIX_TIMESTAMP() + 1);"))
			{

			}
			else
			{
				error_message = gettext("SQL syntax error");
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "no approvers left");
		}
	}
	else
	{
		// --- orderless approvals
		for(auto &approver: approvers)
		{
			if(db->InsertQuery("INSERT INTO `" + type + "_approvals` (`" + type + "_id`, `approver_id`, `decision`, `comment`, `eventTimestamp`) VALUES (\"" + id + "\", \"" + approver.id + "\", \"pending\", \"\", UNIX_TIMESTAMP() + 1);"))
			{

			}
			else
			{
				error_message = gettext("SQL syntax error");
				MESSAGE_ERROR("", "", error_message);
				break;
			}
		}
	}

	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}

string		C_TC_BT_Submit::RemovePendingApprovals()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	db->Query("DELETE FROM `" + type + "_approvals` WHERE `" + type + "_id`=\"" + id + "\" AND `decision`=\"pending\";");
	if(db->isError())
	{
		error_message = gettext("SQL syntax error");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}

string		C_TC_BT_Submit::UpdateItToNewState(string new_state)
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	if(type == "timecard")
		db->Query("UPDATE `timecards` SET `status`=\"" + new_state + "\", `eventTimestamp`=UNIX_TIMESTAMP()+2 WHERE `id`=\"" + id + "\";");
	else if(type == "bt")
		db->Query("UPDATE `bt` SET `status`=\"" + new_state + "\", `eventTimestamp`=UNIX_TIMESTAMP()+2 WHERE `id`=\"" + id + "\";");
	else
	{
		db->Query("SELECT 'fake';");
		error_message = gettext("unknown type");
		MESSAGE_ERROR("", "", error_message);
	}

	if(db->isError())
	{
		error_message = gettext("SQL syntax error");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}

string		C_TC_BT_Submit::Autoapprove()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

		if(isOrderedApprovals())
		{
			// --- ordered approve
			auto current_approver_id = GetCurrentApproverID();
			if(current_approver_id.length())
			{
				if(ApproverIDHasAutoApprove(current_approver_id))
				{
					db->Query("UPDATE `" + type + "_approvals` SET `decision`=\"approved\", `comment`=\"auto-approve\" WHERE `approver_id`=\"" + current_approver_id + "\" AND `" + type + "_id`=\"" + id + "\";");
					if(db->isError())
					{
						error_message = gettext("SQL syntax error") + "("s + db->GetErrorMessage() + ")";
						MESSAGE_ERROR("", "", error_message);
					}
					else
					{
						CraftApprovalList();
						error_message = Autoapprove();
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", type  + "_approver.id have no auto-approve enabled");
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "no approvers left");
			}

		}
		else
		{
			// --- unordered approve
			for(auto &approver: approvers)
			{
				if(approver.auto_approve == "Y")
				{
					db->Query("UPDATE `" + type + "_approvals` SET `decision`=\"approved\", `comment`=\"auto-approve\" WHERE `approver_id`=\"" + approver.id + "\" AND `" + type + "_id`=\"" + id + "\";");
					if(db->isError())
					{
						error_message = gettext("SQL syntax error") + "("s + db->GetErrorMessage() + ")";
						MESSAGE_ERROR("", "", error_message);
						break;
					}
				}
			}
		}

	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}

bool C_TC_BT_Submit::ApproverIDHasAutoApprove(string approver_id)
{
	MESSAGE_DEBUG("", "", "start(" + approver_id + ")");

	auto	result = false;

	for(auto &approver: approvers)
	{
		if(approver.id == approver_id)
		{
			result = approver.auto_approve == "Y";
			break;
		}
	}

	MESSAGE_DEBUG("", "", "finish(" + to_string(result) + ")");

	return result;
}

string C_TC_BT_Submit::GetNextApproverID()
{
	MESSAGE_DEBUG("", "", "start");

	auto	next_approver_id = ""s;
	auto	next_approver_order = ""s;
	auto	last_approver_id = GetLastApproverID();
	auto	last_approver_order = "0"s;

	for(auto &approver: approvers)
	{
		if(approver.id == last_approver_id)
		{
			last_approver_order = approver.approver_order;
			break;
		}
	}

	next_approver_order = to_string(stoi(last_approver_order) + 1);

	for(auto &approver: approvers)
	{
		if(approver.approver_order == next_approver_order)
		{
			next_approver_id = approver.id;
			break;
		}
	}

	MESSAGE_DEBUG("", "", "finish(" + next_approver_id + ")");

	return next_approver_id;
}

string C_TC_BT_Submit::GetLastApproverID()
{
	MESSAGE_DEBUG("", "", "start");

	auto	last_approver_id = ""s;

	auto	sql_query = 
				(type == "timecard")? "SELECT `timecard_approvers_id` FROM `timecard_approvals_view` WHERE "
											"`timecard_approvals_eventTimestamp` > (SELECT `submit_date` FROM `timecards` WHERE `id`=\"" + id + "\") "
											"AND "
											"`timecard_approvals_timecard_id`=\"" + id + "\" ORDER BY `timecard_approvers_approver_order` DESC"
										";" :
				(type == "bt")		? "SELECT `bt_approvers_id` FROM `bt_approvals_view` WHERE "
											"`bt_approvals_eventTimestamp` > (SELECT `submit_date` FROM `bt` WHERE `id`=\"" + id + "\") "
											"AND "
											"`bt_approvals_bt_id`=\"" + id + "\" ORDER BY `bt_approvers_approver_order` DESC"
										";" :
				"";

	if(sql_query.length())
	{
		if(db->Query(sql_query))
		{
			last_approver_id = db->Get(0, 0);
		}
		else
		{
			MESSAGE_DEBUG("", "", "there is no pending approval");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "unknown type");
	}


	MESSAGE_DEBUG("", "", "finish(" + last_approver_id + ")");

	return last_approver_id;
}

string C_TC_BT_Submit::GetCurrentApproverID()
{
	MESSAGE_DEBUG("", "", "start");

	auto	current_approver_id = ""s;

	auto	sql_query = 
				(type == "timecard")? "SELECT `approver_id` FROM `timecard_approvals` WHERE "
											"`eventTimestamp` > (SELECT `submit_date` FROM `timecards` WHERE `id`=\"" + id + "\") "
											"AND "
											"`decision`=\"pending\" "
											"AND "
											"`timecard_id`=\"" + id + "\" ORDER BY `eventTimestamp` DESC"
										";" :
				(type == "bt")		? "SELECT `approver_id` FROM `bt_approvals` WHERE "
											"`eventTimestamp` > (SELECT `submit_date` FROM `bt` WHERE `id`=\"" + id + "\") "
											"AND "
											"`decision`=\"pending\" "
											"AND "
											"`bt_id`=\"" + id + "\" ORDER BY `eventTimestamp` DESC"
										";" :
				"";

	if(sql_query.length())
	{
		if(db->Query(sql_query))
		{
			current_approver_id = db->Get(0, 0);
		}
		else
		{
			MESSAGE_DEBUG("", "", "there is no pending approval");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "unknown type");
	}


	MESSAGE_DEBUG("", "", "finish(" + current_approver_id + ")");

	return current_approver_id;
}

bool		C_TC_BT_Submit::isItCompletelyApproved()
{
	MESSAGE_DEBUG("", "", "start");

	auto	sql_query = 
				(type == "timecard")? "SELECT `id` FROM `timecard_approvals` WHERE "
											"`eventTimestamp` > (SELECT `submit_date` FROM `timecards` WHERE `id`=\"" + id + "\") "
											"AND "
											"`decision`=\"pending\" "
											"AND "
											"`timecard_id`=\"" + id + "\" ORDER BY `eventTimestamp` ASC"
										";" :
				(type == "bt")		? "SELECT `id` FROM `bt_approvals` WHERE "
											"`eventTimestamp` > (SELECT `submit_date` FROM `bt` WHERE `id`=\"" + id + "\") "
											"AND "
											"`decision`=\"pending\" "
											"AND "
											"`bt_id`=\"" + id + "\" ORDER BY `eventTimestamp` ASC"
										";" :
				"";

	completely_approved = false;

	if(sql_query.length())
	{
		if(db->Query(sql_query))
		{
			MESSAGE_DEBUG("", "", "some approvers left");
		}
		else
		{
			completely_approved = true;
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "unknown type");
	}


	MESSAGE_DEBUG("", "", "finish(" + to_string(completely_approved) + ")");

	return completely_approved;
}

string		C_TC_BT_Submit::Submit(const string tc_bt_type_param, const string &id_param)
{
	MESSAGE_DEBUG("", "", "start(" + tc_bt_type_param +", " + id_param + ")");

	auto	error_message = ""s;

	if(tc_bt_type_param.length())
	{
		if(id_param.length())
		{
			id = id_param;
			type = tc_bt_type_param;

			if((error_message = FetchInitialData()).empty())
			{
				if(isItFirstSubmit() || isOrderedApprovals())
				{
					if((error_message = CraftApprovalList()).empty())
					{
						if((error_message = FetchApprovals()).empty())
						{

						}
						else
						{
							MESSAGE_ERROR("", "", error_message);
						}
					}
					else
					{
						MESSAGE_ERROR("", "", error_message);
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", "this is not the first submit");
				}

				if(error_message.empty())
				{
					if(isItBeenRejected())
					{
						if((error_message = RemovePendingApprovals()).empty())
						{
							if((error_message = UpdateItToNewState("rejected")).empty())
							{

							}
							else
							{
								MESSAGE_ERROR("", "", error_message);
							}
						}
						else
						{
							MESSAGE_ERROR("", "", error_message);
						}
					}
					else
					{
						if((error_message = Autoapprove()).empty())
						{
							if(isItCompletelyApproved())
							{
								if((error_message = UpdateItToNewState("approved")).empty())
								{
									
								}
								else
								{
									MESSAGE_ERROR("", "", error_message);
								}
							}
							else
							{
								MESSAGE_DEBUG("", "", "not every approve received");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", error_message);
						}
					}
				}
			}
			else
			{
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			error_message = gettext("id is not defined");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("type is not defined");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}
