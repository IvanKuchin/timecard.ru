#ifndef __C_INVOICE_SERVICE__H__
#define __C_INVOICE_SERVICE__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "hpdf.h"
#include "libxl.h"

using namespace std;

class C_Invoice_Service
{
	private:
		CMysql			*db = NULL;
		CUser			*user = NULL;
		vector<string>	timecard_list;
		string			cost_center_id = "";

		auto		GeneratePDF() -> string;
		auto		GenerateXL() -> string;

	public:
					C_Invoice_Service();
					C_Invoice_Service(CMysql *, CUser *);

		auto		SetTimecardList(const vector<string> &param1)	{ timecard_list = param1; };

		auto		SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto		SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };

		auto		GenerateDocumentArchive() -> string;
};

ostream&	operator<<(ostream& os, const C_Invoice_Service &);

#endif
