#ifndef __C_INVOICE_SERVICE__H__
#define __C_INVOICE_SERVICE__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_print_timecard.h"

using namespace std;

class C_Invoice_Service
{
	private:
		CMysql							*db = NULL;
		CUser							*user = NULL;
		vector<string>					timecard_id_list;
		vector<C_Timecard_To_Print>		timecard_obj_list;
		string							cost_center_id = "";

		string							temp_dir = "";

		auto		CreateTempDirectory() -> bool;
		auto		CreateTimecardObj(string timecard_id) -> C_Timecard_To_Print;

		// auto		GeneratePDF() -> string;
		// auto		GenerateXL() -> string;
		// auto		GenerateXLSTimecard(string timecard_id) -> string;

	public:
					C_Invoice_Service();
					C_Invoice_Service(CMysql *, CUser *);

		auto		SetTimecardList(const vector<string> &param1)	{ timecard_id_list = param1; };

		auto		SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto		SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };

		auto		GenerateDocumentArchive() -> string;

					~C_Invoice_Service();
};

ostream&	operator<<(ostream& os, const C_Invoice_Service &);

#endif
