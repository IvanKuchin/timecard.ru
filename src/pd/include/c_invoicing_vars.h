#ifndef __C_INVOICING_VARS__H__
#define __C_INVOICING_VARS__H__

#include <string>
#include <vector>
#include <locale>

#include "c_date_spelling.h"
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


		string							cost_center_id = "";
		string							agency_company_id = "";

		auto		AssignVariableValue(const string &var_name, const string &value, bool isMandatory);
		auto		AssignVariableFromDB(const string &var_name, const string &sql_query, bool isMandatory);

	public:
					C_Invoicing_Vars();
					C_Invoicing_Vars(CMysql *param1, CUser *param2) : db(param1), user(param2) {};

		auto		SetTimecards(const vector<C_Timecard_To_Print> &param)	{ timecard_obj_list = param; };
		auto		SetCostCenterID(const string &param)	{ cost_center_id = param; };
		auto		SetCostCenterID(string &&param)			{ cost_center_id = move(param); };
		auto		SetAgencyCompanyID(const string &param)	{ agency_company_id = param; };
		auto		SetAgencyCompanyID(string &&param)		{ agency_company_id = move(param); };

		auto		GenerateVariableSet() -> string;

		auto		Get(const string &) -> string;


};

ostream&	operator<<(ostream& os, const C_Invoicing_Vars &);

#endif
