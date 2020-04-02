#ifndef __UTILITIES_SQL_QUERIES__H__
#define __UTILITIES_SQL_QUERIES__H__

#include <string>

inline auto Get_CustomerIDByBTID_sqlquery(const string &bt_id)
{
	return (
			" SELECT `customer_id` FROM `bt` WHERE `id` IN (" + bt_id + ")"
		);
}

inline auto Get_CustomersIDByTimecardID_sqlquery(const string &tc_id)
{
	return (
			" SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id` IN ("
				"SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id` IN ("
					"SELECT `timecard_task_id` FROM `timecard_lines` WHERE `timecard_id` IN (" + tc_id + ")"
				")"
			") "
		);
}

inline auto Get_PSoWIDByBTID_sqlquery(const string &bt_id) -> string
{
	return (
			" SELECT `id` FROM `contracts_psow` WHERE "
			" `contract_sow_id` IN ("
				" SELECT `contract_sow_id` FROM `bt` WHERE `id` IN (" + bt_id + ")"
			" )"
			" AND "
			" (`cost_center_id` IN ("
				" SELECT `cost_center_id` FROM `cost_center_assignment` WHERE"
				" `timecard_customer_id` IN (" + Get_CustomerIDByBTID_sqlquery(bt_id) + ")"
			" )) "
		);
}

inline auto Get_PSoWIDByTimecardID_sqlquery(const string &tc_id) -> string
{
	return (
			" SELECT `id` FROM `contracts_psow` WHERE "
			" `contract_sow_id`=( "
				"SELECT `contract_sow_id` FROM `timecards` WHERE `id` IN (" + tc_id + ")"
			" )"
			" AND " // --- this part is required in case of there are several PSoW assigned to SoW, 
					// --- but customers reported in this timecard belong to fewer PSoW(s)
			" (`cost_center_id` IN ( "
				"SELECT `cost_center_id` FROM `cost_center_assignment` WHERE "
				"`timecard_customer_id` IN (" + Get_CustomersIDByTimecardID_sqlquery(tc_id) + ")"
			" )) "
		);
}

inline auto Get_PSoWIDsBySoWID_sqlquery(const string &sow_id) -> string
{
	return (
			" SELECT `id` FROM `contracts_psow` WHERE `contract_sow_id` IN (" + sow_id + ") "
		);
}

inline auto Get_SoWIDsByApproverUserID_sqlquery(const string &approver_user_id, const string &approve_type) -> string
{
	return (
			" SELECT DISTINCT(`contract_sow_id`) FROM `contracts_psow` WHERE `id` IN (SELECT `contract_psow_id` FROM `" + approve_type + "_approvers` WHERE `approver_user_id` IN (" + approver_user_id + ")) "
		);
/*
	return (
			" SELECT `id` FROM `contracts_sow` WHERE "
			" `id` IN (SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id` IN (SELECT `contract_psow_id` FROM `" + approve_type + "_approvers` WHERE `approver_user_id` IN (" + approver_user_id + ")))"
		);
*/
}

// --- in case of requirement: get a list of all SoW-s been active in Mar 2020, 
// --- you may set date to 2020-03-01 and even if SoW starts date is 2020-03-15, it will be captured by this filter
inline auto Get_SoWDateFilter_sqlquery(const string &date) -> string
{
	return (
			"("
				"("
					"`timecard_period`=\"week\" "
					"AND "
					"`start_date`<=DATE_ADD(\"" + date + "\", INTERVAL 1 WEEK)  "
					"AND "
					"`end_date`>=DATE_SUB(\"" + date + "\", INTERVAL 31 DAY) "
				") "
				"OR "
				"("
					"`timecard_period`=\"month\" "
					"AND "
					"`start_date`<=DATE_ADD(\"" + date + "\", INTERVAL 1 MONTH)  "
					"AND "
					"`end_date`>=DATE_SUB(\"" + date + "\", INTERVAL 31 DAY) "
				")"
			")"
		);
}

inline auto Get_SoWIDsBySubcUserIDAndDate_sqlquery(const string &user_id, const string &date) -> string
{
	return (
		"SELECT `id` FROM `contracts_sow` WHERE "
			"`subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID` = \"" + user_id + "\") "
			"AND " + Get_SoWDateFilter_sqlquery(date)
		);
}

inline auto Get_TimecardDateFilter_sqlquery(const string &date_start, const string &date_end) -> string
{
	return (
			"\"" + date_start + "\"<=`timecards`.`period_end` "
			"AND "
			"`timecards`.`period_end`<=\"" + date_end + "\" "
		);
}

inline auto Get_SoWIDsByBTApproverUserID_sqlquery(const string &approver_user_id) -> string
{
	return Get_SoWIDsByApproverUserID_sqlquery(approver_user_id, "bt");
}

inline auto Get_SoWIDsByTimecardApproverUserID_sqlquery(const string &approver_user_id) -> string
{
	return Get_SoWIDsByApproverUserID_sqlquery(approver_user_id, "timecard");
}

inline auto Get_CompanyIDByUserID_sqlquery(const string &user_id) -> string
{
	return (" SELECT `company_id` FROM `company_employees` WHERE `user_id` IN (" + user_id + ") ");
}

inline auto Get_ApproverIDsByTimecardID_sqlquery(const string &timecard_id) -> string
{
	return(" SELECT `id` FROM `timecard_approvers` WHERE `contract_psow_id` IN (" + Get_PSoWIDByTimecardID_sqlquery(timecard_id) + ") ");
}

inline auto Get_ApproverIDsByBTID_sqlquery(const string &bt_id) -> string
{
	return(" SELECT `id` FROM `bt_approvers` WHERE `contract_psow_id` IN (" + Get_PSoWIDByBTID_sqlquery(bt_id) + ") ");
}

inline auto Get_ApprovalIDsByTimecardID_sqlquery(const string &id) -> string
{
	return(
			" SELECT `id` FROM `timecard_approvals` WHERE "
				" `timecard_id`IN (" + id + ")"
				" AND "
				" `decision`=\"approved\""
				" AND "
				" `eventTimestamp` > (SELECT `submit_date` FROM `timecards` WHERE `id` IN (" + id + ")) "
		);
}

inline auto Get_ApprovalIDsByBTID_sqlquery(const string &id) -> string
{
	return(
			" SELECT `id` FROM `bt_approvals` WHERE "
				" `bt_id` IN (" + id + ")"
				" AND "
				" `decision`=\"approved\""
				" AND "
				" `eventTimestamp` > (SELECT `submit_date` FROM `bt` WHERE `id` IN (" + id + ")) "
		);
}

inline auto	Get_TimecardTaskIDsByTimecardApproverUserID_sqlquery(const string &id)
{
	return (
			" SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
			    " SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id` IN ("
			        " SELECT `timecard_customer_id` FROM `cost_center_assignment` WHERE `cost_center_id` IN ("
			            " SELECT `cost_center_id` FROM `contracts_psow` WHERE `id` IN ("
			                " SELECT `contract_psow_id` FROM `timecard_approvers` WHERE `approver_user_id` IN (" + id + ")"
			            " )"
			        " )"
			    " )"
			" )"
		);
}


inline auto	Get_AgencyIDByUserID_sqlquery(const string &id)
{
	return (
			"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id` IN ("
				+ Get_CompanyIDByUserID_sqlquery(id) +
			")"
		);
}



#endif
