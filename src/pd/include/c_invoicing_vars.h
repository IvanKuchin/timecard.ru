#ifndef __C_INVOICING_VARS__H__
#define __C_INVOICING_VARS__H__

#include <string>
#include <vector>
#include <locale>

#include "c_date_spelling.h"
#include "c_price_spelling.h"
#include "c_bt_to_print.h"
#include "c_timecard_to_print.h"
#include "utilities_timecard.h"
#include "c_cache_obj.h"
#include "c_float.h"

using namespace std;

class C_Invoicing_Vars
{
	private:
		c_cache_obj						cache;
		CMysql							*db = NULL;
		CUser							*user = NULL;
		CVars							vars;

		vector<C_Timecard_To_Print>		timecard_obj_list;
		vector<C_BT_To_Print>			bt_obj_list;


		string							cost_center_id = "";
		string							agency_company_id = "";
		string							__sow_id = "";

		auto		FillStaticDictionary() -> string;
		auto		CostCenter_VarSet() -> string;
		auto		Agency_VarSet(string) -> string;
		auto		CurrentTimestamp_1C_VarSet() -> string;
		auto		AgreementNumberSpelling_VarSet(string agreement_number) -> string;

		auto		SubcontractorsTotal_VarSet(string sum, string tax, string total) -> string;
		auto		CostCenterTotal_VarSet(c_float cost_center_sum) -> string;

		auto		Common_Index_VarSet(string index) -> string;
		auto		DocumentSubmissionDate_1C_Index_VarSet(string date_inside_month, string index) -> string;
		auto		SoW_Index_VarSet(string sql_query, string index) -> string;
		auto		PSoW_Index_VarSet(string sql_query, string index) -> string;
		auto		CostCenterPayment_Index_VarSet(c_float cost_center_price, string index) -> string;
		auto		Subcontractor_Index_VarSet(string subcontractor_company_id, string index) -> string;
		auto		SubcontractorAddress_Index_VarSet(string index) -> string;
		auto		SubcontractorPayment_Index_VarSet(c_float cost_center_price, string index) -> string;
		auto		Workperiod_Index_VarSet(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string;
		auto		Workperiod_vs_PSoWperiod_Index_VarSet(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string;
		auto		TableRowDecsriptions_Index_VarSet(string local_remote_service_description, string index) -> string;

		auto		ShortenWorkPeriodSpelling(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string;


	public:
					C_Invoicing_Vars()						{};
					C_Invoicing_Vars(CMysql *param1, CUser *param2) : db(param1), user(param2) {};

		auto		SetDB(CMysql *param)					{ db = param; };
		auto		SetUser(CUser *param)					{ user = param; };

		auto		AssignVariableValue(const string &var_name, const string &value, bool isMandatory) -> string;
		auto		AssignVariableFromDB(const string &var_name, const string &sql_query, bool isMandatory) -> string;

		auto		SetTimecards(const vector<C_Timecard_To_Print> &param)	{ timecard_obj_list = param; };
		auto		SetBTs(const vector<C_BT_To_Print> &param)	{ bt_obj_list = param; };
		auto		SetCostCenterID(const string &param)	{ cost_center_id = param; };
		auto		SetCostCenterID(string &&param)			{ cost_center_id = move(param); };
		auto		SetAgencyCompanyID(const string &param)	{ agency_company_id = param; };
		auto		SetAgencyCompanyID(string &&param)		{ agency_company_id = move(param); };
		auto		SetSoWID(const string &param)			{ __sow_id = param; };
		auto		SetSoWID(string &&param)				{ __sow_id = move(param); };

		auto		GetSoWID()								{ return __sow_id; }

		auto		GenerateServiceVariableSet_AgencyToCC() -> string;
		auto		GenerateServiceVariableSet_SubcToAgency() -> string;
		auto		GenerateBTVariableSet_AgencyToCC() -> string;
		auto		GenerateBTVariableSet_SubcToAgency() -> string;
		auto		GenerateSoWVariableSet() -> string;

		auto		Get(const string &) -> string;

		auto		GetVars()								{ return vars; };

		auto		GetIndexByTimecardID(string timecard_id) -> string;
		auto		GetIndexByBTID(string bt_id) -> string;
};

ostream&	operator<<(ostream& os, const C_Invoicing_Vars &);

#endif
