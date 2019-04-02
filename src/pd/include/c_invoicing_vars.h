#ifndef __C_INVOICING_VARS__H__
#define __C_INVOICING_VARS__H__

#include <string>
#include <vector>
#include <locale>

/*#include "cmysql.h"
#include "cuser.h"
#include "cvars.h"
#include "clog.h"
*/
#include "utilities_timecard.h"

using namespace std;

class C_Invoicing_Vars
{
	private:
		CMysql							*db = NULL;
		CUser							*user = NULL;
		CVars							vars;

		string							cost_center_id = "";
		string							agency_company_id = "";

		auto		AssignVariableValue(const string &var_name, const string &value, bool isMandatory);
		auto		AssignVariableFromDB(const string &var_name, const string &sql_query, bool isMandatory);

	public:
					C_Invoicing_Vars();
					C_Invoicing_Vars(CMysql *param1, CUser *param2) : db(param1), user(param2) {};

		auto		SetCostCenterID(const string &param)	{ cost_center_id = param; };
		auto		SetCostCenterID(string &&param)			{ cost_center_id = move(param); };
		auto		SetAgencyCompanyID(const string &param)	{ agency_company_id = param; };
		auto		SetAgencyCompanyID(string &&param)		{ agency_company_id = move(param); };

		auto		GenerateVariableSet() -> string;

		auto		Get(const string &) -> string;


};

ostream&	operator<<(ostream& os, const C_Invoicing_Vars &);

#endif
