#ifndef __UTILITIES_TIMECARD__H__
#define __UTILITIES_TIMECARD__H__

#include "utilities.h"
#include "c_float.h"

// --- timecard functions
auto	 		CheckHTTPParam_Timeentry(const string &srcText) -> string;

auto 			GetNewsFeedInJSONFormat(string whereStatement, int currPage, int newsOnSinglePage, CUser *, CMysql *) -> string;
auto      		GetCompanyListInJSONFormat(string dbQuery, CMysql *, CUser *, bool quickSearch = true, bool includeEmployedUsersList = false) -> string;
auto      		GetUserNotificationSpecificDataByType(unsigned long typeID, unsigned long actionID, CMysql *, CUser *) -> string;
auto      		GetUserNotificationInJSONFormat(string sqlRequest, CMysql *, CUser *) -> string;

auto			GetTimecardCustomersInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardProjectsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardTasksInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardTaskAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended = false) -> string;
auto			GetSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string;
auto			GetSOWInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_tasks = true, bool include_bt = false, bool include_cost_centers = false) -> string;
auto			GetPSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string;
auto			GetPSoWInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string;
auto			GetTimecardApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			SubmitTimecard(string timecard_id, CMysql *, CUser *) -> bool;
auto			SubmitBT(string bt_id, CMysql *, CUser *) -> bool;
auto			GetNumberOfTimecardsInPendingState(CMysql *, CUser *) -> string;
auto			GetNumberOfBTInPendingState(CMysql *, CUser *) -> string;
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
auto			GetApproversInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_sow) -> string;
auto			GetSpelledTimecardCustomerByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardProjectByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardTaskByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardTaskAssignmentByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateLineByID(string id, CMysql *) -> string;
auto			GetSpelledCostCenterByID(string id, CMysql *) -> string;
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
auto			isValidToReportTime(string timecard_id, string task_id, string timereport, CMysql *, CUser *) -> string;
auto			isTaskIDValidToRemove(string task_id, CMysql *) -> string;
auto			isProjectIDValidToRemove(string task_id, CMysql *) -> bool;
auto			isCustomerIDValidToRemove(string task_id, CMysql *) -> bool;
auto			isExpenseTemplateIDValidToRemove(string bt_expense_template_id, CMysql *) -> string;
auto			isExpenseTemplateLineIDValidToRemove(string bt_expense_template_line_id, CMysql *) -> string;
auto			isEmployeeIDValidToRemove(string employee_id, CMysql *) -> string;
pair<string, string> GetCustomerIDProjectIDByTaskID(string task_id, CMysql *);
pair<int, int>	FirstAndLastReportingDaysFromTimereport(const vector<string> &timereport);


auto			GetBTApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseLineTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended) -> string;
auto			GetBTExpensesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCurrencyRatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto 			isUserAllowedAccessToBT(string bt_id, CMysql *, CUser *) -> string;
auto			isCostCenterBelongsToAgency(string cost_center_id, CMysql *db, CUser *user) -> bool;

// string			isAgencyEmployeeAllowedToChangeSoW(string sow_id, CMysql *, CUser *);
// string			isAgencyEmployeeAllowedToChangeAgencyData(CMysql *, CUser *);
// string			isActionEntityBelongsToSoW(string action, string is, string sow_id, CMysql *, CUser *);
// string			isActionEntityBelongsToAgency(string action, string id, string agency_id, CMysql *, CUser *);
auto			NotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *, CUser *) -> bool;
auto			GetDBValueByAction(string action, string id, string sow_id, CMysql *, CUser *) -> string;
// string			CheckNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *);
auto			SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			GetAgencyEmployeesInJSONFormat(string sql_query, CMysql *, CUser *) -> string;
auto			GetAgencyObjectInJSONFormat(string agency_id, bool include_tasks, bool include_bt, CMysql *, CUser *) -> string;
auto			GetInfoToReturnByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			GetNumberOfApprovedTimecardsThisMonth(CMysql *db, CUser *user) -> string;
auto			GetNumberOfApprovedTimecardsLastMonth(CMysql *db, CUser *user) -> string;
auto 			GetNumberOfSoWActiveThisMonth(CMysql *db, CUser *user) -> string;
auto 			GetNumberOfSoWActiveLastMonth(CMysql *db, CUser *user) -> string;
auto			GetPSoWIDByTimecardIDAndCostCenterID(string timecard_id, string cost_center_id, CMysql *db, CUser *user) -> string;
auto			GetSoWIDByTimecardID(string timecard_id, CMysql *db, CUser *user) -> string;
// auto			GetTimecardLines_By_TimecardID_And_CostCenterID(string timecard_id, string cost_center_id, CMysql *db, CUser *user);


#endif