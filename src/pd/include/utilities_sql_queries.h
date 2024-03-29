#ifndef __UTILITIES_SQL_QUERIES__H__
#define __UTILITIES_SQL_QUERIES__H__

#include <string>

#include "localy_pd.h"

inline auto Get_CompanyIDByAdminUserID_sqlquery(const string &user_id) -> string
{
	return (" SELECT `id` FROM `company` WHERE `admin_userID` IN (" + user_id + ") ");
}

inline auto Get_CompanyIDByCompanyEmployeeUserID_sqlquery(const string &user_id) -> string
{
	return (" SELECT `company_id` FROM `company_employees` WHERE `user_id` IN (" + user_id + ") ");
}

inline auto Get_SubcCompanyIDBySoWID_sqlquery(const string &id) -> string
{
	return ("SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id` IN (" + id + ")");
}

inline auto Get_SubcCompanyIDByTimecardID_sqlquery(const string &id) -> string
{
	return (Get_SubcCompanyIDBySoWID_sqlquery(
			"SELECT `contract_sow_id` FROM `timecards` WHERE `id` IN (" + id + ")"
		));
/*
	return ("SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=("
				"SELECT `contract_sow_id` FROM `timecards` WHERE `id` IN (" + id + ")"
			")");
*/
}

inline auto Get_SubcCompanyIDByBTID_sqlquery(const string &id) -> string
{
	return (Get_SubcCompanyIDBySoWID_sqlquery(
			"SELECT `contract_sow_id` FROM `bt` WHERE `id` IN (" + id + ")"
		));
/*
	return ("SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=("
				"SELECT `contract_sow_id` FROM `bt` WHERE `id` IN (" + id + ")"
			")");
*/
}

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

inline auto Get_SubcontractorRecallBySoWID_sqlquery(const string &sow_id) -> string
{
	return (
				"SELECT `recall_by_subcontractor` FROM `contracts_sow` WHERE `id` IN (" + sow_id + ")"
		);
}

inline auto Get_AgencyRecallBySoWID_sqlquery(const string &sow_id) -> string
{
	return (
				"SELECT `recall_by_agency` FROM `contracts_sow` WHERE `id` IN (" + sow_id + ")"
		);
}

inline auto Get_InvoiceFileByTimecardID_sqlquery(const string &tc_id) -> string
{
	return (
				"SELECT `invoice_filename` FROM `timecards` WHERE `id` IN (" + tc_id + ")"
		);
}

inline auto Get_SoWIDByTimecardID_sqlquery(const string &tc_id) -> string
{
	return (
				"SELECT `contract_sow_id` FROM `timecards` WHERE `id` IN (" + tc_id + ")"
		);
}

inline auto Get_PSoWIDByTimecardID_sqlquery(const string &tc_id) -> string
{
	return (
			" SELECT `id` FROM `contracts_psow` WHERE "
			" `contract_sow_id`=( " + Get_SoWIDByTimecardID_sqlquery(tc_id) + " )"
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
// --- or SoW expiration date is 2020-02-15, it will be captured by this filter
inline auto Get_SoWDateFilter_sqlquery(const string &date) -> string
{
	return (
			"("
				"("
					"`timecard_period`=\"week\" "
					"AND "
					"DATE_SUB(\"" + date + "\", INTERVAL 31 DAY)<=`end_date` "
					"AND "
					"`start_date`<=DATE_ADD(\"" + date + "\", INTERVAL 1 WEEK)  "
				") "
				"OR "
				"("
					"`timecard_period`=\"month\" "
					"AND "
					"DATE_SUB(\"" + date + "\", INTERVAL " + to_string(SOW_EXPIRATION_BUFFER) + " DAY)<=`end_date` "
					"AND "
					"`start_date`<=DATE_ADD(\"" + date + "\", INTERVAL 1 MONTH)  "
				")"
			") "
		);
}

inline auto Get_SoWIDsBySubcUserIDAndDate_sqlquery(const string &user_id, const string &date) -> string
{
	return (
		"SELECT `id` FROM `contracts_sow` WHERE "
			"`subcontractor_company_id` IN (" + Get_CompanyIDByAdminUserID_sqlquery(user_id) + ") "
			"AND " + Get_SoWDateFilter_sqlquery(date)
		);
}

inline auto Get_SoWIDsBySubcCompanyID_sqlquery(const string &id) -> string
{
	return (
		"SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN (" + id + ") "
		);
}

inline auto Get_SoWIDsByAgencyCompanyID_sqlquery(const string &id) -> string
{
	return (
		"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (" + id + ") "
		);
}

inline auto Get_TimecardDateFilter_sqlquery(const string &date_start, const string &date_end) -> string
{
	return (
			"(\"" + date_start + "\"<=`timecards`.`period_end`) "
			"AND "
			"(`timecards`.`period_end`<=\"" + date_end + "\") "
		);
}

inline auto Get_BTDateFilter_sqlquery(const string &date_start, const string &date_end) -> string
{
	return (
			"(\"" + date_start + "\"<=`bt`.`date_end`) "
			"AND "
			"(`bt`.`date_end`<=\"" + date_end + "\") "
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

inline auto Get_TimecardsIDBySubcCompanyID_sqlquery(const string &id)
{
	return (
			"SELECT `id` FROM `timecards` WHERE `contract_sow_id` IN ("
				+ Get_SoWIDsBySubcCompanyID_sqlquery(id) +
			")"
		);
}

inline auto Get_TimecardsIDByAgencyCompanyID_sqlquery(const string &id)
{
	return (
			"SELECT `id` FROM `timecards` WHERE `contract_sow_id` IN ("
				+ Get_SoWIDsByAgencyCompanyID_sqlquery(id) +
			")"
		);
}

inline auto Get_BTsIDBySubcCompanyID_sqlquery(const string &id)
{
	return (
			"SELECT `id` FROM `bt` WHERE `contract_sow_id` IN ("
				+ Get_SoWIDsBySubcCompanyID_sqlquery(id) +
			")"
		);
}

inline auto Get_BTsIDByAgencyCompanyID_sqlquery(const string &id)
{
	return (
			"SELECT `id` FROM `bt` WHERE `contract_sow_id` IN ("
				+ Get_SoWIDsByAgencyCompanyID_sqlquery(id) +
			")"
		);
}

inline auto	Get_AgencyIDByUserID_sqlquery(const string &id)
{
	return (
			"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id` IN ("
				+ Get_CompanyIDByCompanyEmployeeUserID_sqlquery(id) +
			")"
		);
}

inline auto	Get_SoWIDByPSoWID_sqlquery(const string &id)
{
	return (
			"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id` IN ("
				+ id +
			")"
		);

}

inline auto	Get_AgencyIDByPSoWID_sqlquery(const string &id)
{
	return (
			"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id` IN ("
				+ Get_SoWIDByPSoWID_sqlquery(id) +
			")"
		);
}

inline auto Get_ActsIDBySubcCompanyID(const string &id)
{
	return (
			"SELECT DISTINCT(`id`) FROM `invoices` WHERE "
				"`id` IN (SELECT `invoice_id` FROM `timecards` WHERE `id` IN (" + Get_TimecardsIDBySubcCompanyID_sqlquery(id) + ")) "
				"OR "
				"`id` IN (SELECT `invoice_id` FROM `bt` WHERE `id` IN (" + Get_BTsIDBySubcCompanyID_sqlquery(id) + ")) "
		);
}

inline auto Get_ActsIDByAgencyCompanyID(const string &id)
{
	return (
			"SELECT DISTINCT(`id`) FROM `invoices` WHERE "
				"`id` IN (SELECT `invoice_id` FROM `invoice_cost_center_service` WHERE `owner_company_id` IN (" + id + ")) "
				"OR "
				"`id` IN (SELECT `invoice_id` FROM `invoice_cost_center_bt` WHERE `owner_company_id` IN (" + id + ")) "
		);
}

inline auto Get_ActIDByTimecardID(const string &id)
{
	return (
			"SELECT `invoice_id` FROM `timecards` WHERE `id` IN (" + id + ")"
		);
}

inline auto Get_ActIDByBTID(const string &id)
{
	return (
			"SELECT `invoice_id` FROM `bt` WHERE `id` IN (" + id + ")"
		);
}

inline auto Get_ActIDByServiceInvoiceIDToCC(const string &id)
{
	return (
			"SELECT `invoice_id` FROM `invoice_cost_center_service` WHERE `id` IN (" + id + ")"
		);
}

inline auto Get_ActIDByBTInvoiceIDToCC(const string &id)
{
	return (
			"SELECT `invoice_id` FROM `invoice_cost_center_bt` WHERE `id` IN (" + id + ")"
		);
}

inline auto Get_PSoWNumberUniqueByPSoWID_sqlquery(const string &id)
{
	return (
			"SELECT `psow_number_unique` FROM `company` WHERE `id` IN("
				+ Get_AgencyIDByPSoWID_sqlquery(id) +
			")"
		);
}


#endif
