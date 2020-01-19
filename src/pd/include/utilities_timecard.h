#ifndef __UTILITIES_TIMECARD__H__
#define __UTILITIES_TIMECARD__H__

#include "c_tc_bt_submit.h"
#include "utilities.h"
#include "c_float.h"
#include "c_cache_obj.h"

// --- IMPORTANT !!!
// --- do NOT add any header file with potentialy recursive indludes

// --- timecard functions
auto	 		CheckHTTPParam_Timeentry(const string &srcText) -> string;

auto 			GetNewsFeedInJSONFormat(string whereStatement, int currPage, int newsOnSinglePage, CUser *, CMysql *) -> string;
// auto      		GetCompanyListInJSONFormat(string dbQuery, CMysql *, CUser *, bool quickSearch = true, bool includeEmployedUsersList = false) -> string;
auto      		GetCompanyListInJSONFormat(string dbQuery, CMysql *, CUser *, bool quickSearch, bool includeEmployedUsersList) -> string;
auto      		GetCompanyListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto      		GetUserNotificationSpecificDataByType(unsigned long typeID, unsigned long actionID, CMysql *, CUser *) -> string;
auto      		GetUserNotificationInJSONFormat(string sqlRequest, CMysql *, CUser *) -> string;

auto			GetTimecardCustomersInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardProjectsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardTasksInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardTaskAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseLinesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended = false) -> string;
auto			GetServiceBTInvoicesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetSOWInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_tasks = true, bool include_bt = false, bool include_cost_centers = false, bool include_subc_company = false) -> string;
auto			GetTemplateCompanyAgreementFiles(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTemplateSoWAgreementFiles(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCountryListInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetPSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCostCenterCustomFieldsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCompanyCustomFieldsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetPSoWInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCustomersInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
// auto			SubmitTimecard(string timecard_id, CMysql *, CUser *) -> bool;
// auto			SubmitBT(string bt_id, CMysql *, CUser *) -> bool;
auto			GetNumberOfTimecardsInPendingState(CMysql *, CUser *) -> string;
auto			GetNumberOfBTInPendingState(CMysql *, CUser *) -> string;
auto			GetNumberOfTimecardsInPaymentPendingState(string sow_sql, CMysql *, CUser *) -> string;
auto			GetNumberOfBTInPaymentPendingState(string sow_sql, CMysql *, CUser *) -> string;
vector<string>	GetTimecardsWithExpiredPayment(string multiplier, string sow_sql, CMysql *, CUser *);
vector<string>	GetBTWithExpiredPayment(string multiplier, string sow_sql, CMysql *, CUser *);
auto 			SummarizeTimereports(string timereport1, string timereport2) -> string;
vector<string> 	SplitTimeentry(const string& s, const char c = ',');
auto			isTimecardEntryEmpty(string	timereports) -> bool;
auto			isUserAssignedToSoW(string user_id, string sow_id, CMysql *) -> bool;
auto 			isSoWAllowedToCreateTask(string sow_id, CMysql *) -> bool;
auto			GetTaskIDFromAgency(string customer, string project, string task, string agency_id, CMysql *) -> string;
auto			GetTaskIDFromSOW(string customer, string project, string task, string sow_id, CMysql *) -> string;
auto			CreateTaskBelongsToAgency(string customer, string project, string task, string agency_id, CMysql *) -> string;
auto			GetTimecardID(string sow_id, string period_start, string period_end, CMysql *) -> string;
auto			GetTimecardStatus(string timecard_id, CMysql *) -> string;
auto			GetTimecardLineID(string timecard_id, string task_id, CMysql *) -> string;
auto			GetAgencyID(string sow_id, CMysql *) -> string;
auto			GetAgencyID(CUser *, CMysql *) -> string;
auto			GetObjectsSOW_Reusable_InJSONFormat(string object, string filter, CMysql *, CUser *) -> string;
auto			GetBTExpenseTemplateAssignmentToSoW(string bt_expense_template_id, string sow_id, CMysql *) -> string;
auto			GetTaskAssignmentID(string customer, string project, string task, string sow_id, CMysql *) -> string;
auto			CreateTaskAssignment(string task_id, string sow_id, string assignment_start, string assignment_end, CMysql *, CUser *) -> string;
auto 			CreateBTExpenseTemplateAssignmentToSoW(string new_bt_expense_template_id, string sow_id, CMysql *, CUser *) -> string;
auto			GetCostCentersInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCostCentersAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetApproversInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetApproversInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_sow) -> string;
auto			GetAbsenceListInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetAbsenceTypesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;

// --- notification spellers
auto			GetSpelledTimecardCustomerByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardProjectByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardTaskByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardTaskAssignmentByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateLineByID(string id, CMysql *) -> string;
auto			GetSpelledBTAllowanceByID(string id, CMysql *) -> string;
auto			GetSpelledCostCenterByID(string id, CMysql *) -> string;
auto			GetSpelledCostCenterByCustomFieldID(string id, CMysql *) -> string;
auto			GetSpelledAirfareDirectionLimitByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseAssignmentByID(string id, CMysql *) -> string;
auto			GetSpelledUserNameByID(string id, CMysql *) -> string;
auto			GetSpelledEmployeeByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardApproverNameByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseApproverNameByID(string id, CMysql *) -> string;
auto			GetSpelledSoWCustomFieldNameByID(string custom_field_id, CMysql *) -> string;
auto			GetSpelledPSoWCustomFieldNameByID(string custom_field_id, CMysql *) -> string;
auto			GetSpelledCostCenterCustomFieldNameByID(string custom_field_id, CMysql *) -> string;
auto			GetSpelledSoWByID(string sow_id, CMysql *) -> string;
auto			GetSpelledPSoWByID(string sow_id, CMysql *) -> string;
auto			GetSpelledZIPByID(string id, CMysql *) -> string;
auto			GetSpelledBankByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateByLineID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateByAssignmentID(string id, CMysql *) -> string;
auto			GetSpelledBT(string id, CMysql *db) -> string;
auto			GetSpelledTimecard(string id, CMysql *db) -> string;

auto			isValidToReportTime(string timecard_id, string task_id, string timereport, CMysql *, CUser *) -> string;
auto			isTaskIDValidToRemove(string task_id, CMysql *) -> string;
auto			isProjectIDValidToRemove(string task_id, CMysql *) -> bool;
auto			isCustomerIDValidToRemove(string task_id, CMysql *) -> bool;
auto			isExpenseTemplateIDValidToRemove(string bt_expense_template_id, CMysql *) -> string;
auto			isExpenseTemplateLineIDValidToRemove(string bt_expense_template_line_id, CMysql *) -> string;
auto			isEmployeeIDValidToRemove(string employee_id, CMysql *) -> string;
pair<string, string> GetCustomerIDProjectIDByTaskID(string task_id, CMysql *);
pair<int, int>	FirstAndLastReportingDaysFromTimereport(const vector<string> &timereport);

auto			GetHolidaysSet(string day_around, CMysql *) -> unordered_set<string>;

auto			GetBTApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseLineTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended) -> string;
auto			GetBTAllowanceInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetHolidayCalendarInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;

auto			ApprovalChain(string sql, CMysql *) -> string;
auto			GetTimecard_ApprovalChain(string timecard_id, CMysql *) -> string;
auto			GetBT_ApprovalChain(string bt_id, CMysql *) -> string;

auto			GetBTExpensesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetAirlineBookingsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCurrencyRatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto 			isUserAllowedAccessToBT(string bt_id, CMysql *, CUser *) -> string;
auto			isCostCenterBelongsToAgency(string cost_center_id, CMysql *, CUser *) -> bool;
auto			amIonTheApproverListForSoW(const string &db_table, const string &sow_id, CMysql *, CUser *) -> bool;

auto			NotifySoWContractPartiesAboutChanges(string action_type_id, string sow_id, CMysql *, CUser *) -> string;
auto			NotifyAgencyAboutChanges(string agency_id, string action_type_id, string action_id, CMysql *, CUser *) -> string;
auto			GeneralNotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *, CUser *) -> bool;
auto			GetDBValueByAction(string action, string id, string sow_id, CMysql *, CUser *) -> string;
auto			SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			DeleteEntryByAction(string action, string id, CMysql *, CUser *) -> string;
// auto			ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			GetAgencyEmployeesInJSONFormat(string sql_query, CMysql *, CUser *) -> string;
auto			GetAgencyObjectInJSONFormat(string agency_id, bool include_tasks, bool include_bt, CMysql *, CUser *) -> string;
auto			GetInfoToReturnByAction(string action, string, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			GetNumberOfApprovedTimecardsThisMonth(CMysql *, CUser *) -> string;
auto			GetNumberOfApprovedTimecardsLastMonth(CMysql *, CUser *) -> string;
auto 			GetNumberOfSoWActiveThisMonth(CMysql *, CUser *) -> string;
auto 			GetNumberOfSoWActiveLastMonth(CMysql *, CUser *) -> string;
auto			GetPSoWIDByTimecardIDAndCostCenterID(string timecard_id, string cost_center_id, CMysql *, CUser *) -> string;
auto			GetSoWIDByPSoWID(string psow_id, CMysql *, CUser *) -> string;
auto			GetPSoWIDByApprover(string sql_query, CMysql *, CUser *) -> string;
auto			GetSoWIDByTimecardID(string timecard_id, CMysql *, CUser *) -> string;
auto			GetPSoWIDByBTIDAndCostCenterID(string timecard_id, string cost_center_id, CMysql *, CUser *) -> string;
auto			GetPositionByCompanyID(string company_id, CMysql *, CUser *) -> string;

auto			isUserAllowedToCreateBIK(CUser *, CMysql *) -> string;
auto			isServiceInvoiceBelongsToUser(string service_invoice_id, CMysql *, CUser *) -> bool;
auto			RecallServiceInvoice(string service_invoice_id, CMysql *, CUser *) -> string;
auto			isBTInvoiceBelongsToUser(string service_invoice_id, CMysql *, CUser *) -> bool;
auto			RecallBTInvoice(string service_invoice_id, CMysql *, CUser *) -> string;
auto			isServiceInvoiceBelongsToAgency(string service_invoice_id, CMysql *, CUser *) -> bool;
auto			isBTInvoiceBelongsToAgency(string service_invoice_id, CMysql *, CUser *) -> bool;
auto			GetAgencyIDByUserID(CMysql *, CUser *) -> string;
auto 			GetCostCenterIDByCustomerID(string customer_id, CMysql *) -> string;
auto 			GetCostCenterIDByTaskID(string task_id, CMysql *) -> string;
auto 			GetPositionIDFromSoW(string sow_id, CMysql *) -> string;
auto			CreatePSoWfromTimecardCustomerIDAndCostCenterID(string timecard_customer_id, string cost_center_id, CMysql *, CUser *) -> bool;
auto			CreatePSoWfromTimecardTaskIDAndSoWID(string timecard_task_id, string sow_id, CMysql *, CUser *) -> bool;
auto			CreatePSoWfromTimecardCustomerIDAndSoWID(string timecard_customer_id, string sow_id, CMysql *, CUser *) -> bool;

auto			isTimecardsHavePSOWAssigned(const vector<string> &timecard_list, string cost_center_id, CMysql *, CUser *) -> string;
auto			isTimecardsBelongToAgency(const vector<string> &timecard_list, string agency_id, CMysql *, CUser *) -> string;
auto			isBTsHavePSOWAssigned(const vector<string> &timecard_list, string cost_center_id, CMysql *, CUser *) -> string;
auto			isBTsBelongToAgency(const vector<string> &timecard_list, string agency_id, CMysql *, CUser *) -> string;
auto			isSubcontractorAllowedToChangeSoW(string sow_id, CMysql *, CUser *) -> string;
auto			isAgencyEmployeeAllowedToChangeSoW(string sow_id, CMysql *, CUser *) -> string;
auto			isAgencyEmployeeAllowedToChangePSoW(string sow_id, CMysql *, CUser *) -> string;
auto			isAgencyEmployeeAllowedToChangeAgencyData(CMysql *, CUser *) -> string;
struct tm 		GetFirstDayReportedOnAssignmentID(string sow_id, string task_id, CMysql *);
struct tm 		GetLastDayReportedOnAssignmentID(string sow_id, string task_id, CMysql *);
auto			isActionEntityBelongsToSoW(string action, string id, string sow_id, CMysql *, CUser *) -> string;
auto			CheckNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			isActionEntityBelongsToAgency(string action, string id, string agency_id, CMysql *, CUser *) -> string;
auto			GetAirfareLimitaionsByDirectionInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetAirportCountryInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			isValidToAddAirfareLimitByDirection(string from_id, string to_id, string agency_id, CMysql *, CUser *) -> string;

auto			GetAbsenceOverlap(string company_id, string start_date, string end_date, CMysql *, CUser *, string exclude_id = "") -> string;

auto			Update_TimecardBT_ExpectedPayDate(string entity, string id, CMysql *db, CUser *user) -> string;

auto			CheckAgreementSoWTitle(string title, string sow_id, CMysql *, CUser *) -> string;

auto			GetDashboardPaymentData(string sow_sql, CMysql *db, CUser *user) -> string;
auto			GetTimecardList(string where_companies_list, CMysql *db, CUser *user) -> string;


auto			isDemoDomain() -> bool;

#endif
