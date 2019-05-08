#ifndef __C_INVOICE_SERVICE_SUBC_TO_AGENCY__H__
#define __C_INVOICE_SERVICE_SUBC_TO_AGENCY__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_print_timecard.h"
#include "c_print_invoice_docs.h"
#include "c_print_vat.h"


#include "c_archive.h"

using namespace std;

class C_Invoice_Service_Subc_To_Agency
{
	private:
		CMysql							*db = NULL;
		CUser							*user = NULL;
		vector<string>					timecard_id_list;
		vector<C_Timecard_To_Print>		timecard_obj_list;
		C_Invoicing_Vars				invoicing_vars;


		long							invoice_agency_service_id = 0;
		c_float							total_payment;

		string							temp_dir = "";
		string							temp_dir_timecards = "";
		string							temp_dir_agency_invoices = "";

		string							temp_archive_file = "";	// --- archive in temp_dir
		string							archive_folder = "";	// --- production archive folder
		string							archive_file = "";		// --- archive in production folder


		auto		UpdateDBWithInvoiceData(const string timecard_id) -> string;
		auto		CreateTempDirectory() -> bool;
		auto		CreateTimecardObj(string timecard_id) -> C_Timecard_To_Print;

	public:
					C_Invoice_Service_Subc_To_Agency();
					C_Invoice_Service_Subc_To_Agency(CMysql *, CUser *);

		auto		SetTimecardList(const vector<string> &param1)	{ timecard_id_list = param1; };

		auto		GetInvoiceID()									{ return invoice_agency_service_id; };

		auto		GenerateDocumentArchive() -> string;

					~C_Invoice_Service_Subc_To_Agency();
};

ostream&	operator<<(ostream& os, const C_Invoice_Service_Subc_To_Agency &);

#endif
