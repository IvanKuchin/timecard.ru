#ifndef __C_INVOICE_SERVICE_AGENCY_TO_CC__H__
#define __C_INVOICE_SERVICE_AGENCY_TO_CC__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_print_timecard.h"
#include "c_print_invoice_docs.h"
#include "c_print_vat.h"
#include "c_print_1c_costcenter.h"
#include "c_print_1c_subcontractor.h"
#include "c_archive.h"

using namespace std;

class C_Invoice_Service_Agency_To_CC
{
	private:
		c_config						*config = NULL;
		CMysql							*db = NULL;
		CUser							*user = NULL;
		vector<string>					timecard_id_list;
		vector<C_Timecard_To_Print>		timecard_obj_list;
		C_Invoicing_Vars				invoicing_vars;
		string							cost_center_id = "";

		long							invoice_cost_center_service_id = 0;
		c_float							total_payment;

		string							temp_dir = "";
		string							temp_dir_timecards = "";
		string							temp_dir_cost_center_invoices = "";
		string							temp_dir_1c = "";
		string							temp_dir_1c_subc_payments = "";
		string							temp_dir_1c_subc_payment_orders = "";
		string							temp_archive_file = "";	// --- archive in temp_dir
		string							archive_folder = "";	// --- production archive folder
		string							archive_file = "";		// --- archive in production folder


		auto		UpdateDBWithInvoiceData(const string timecard_id) -> string;
		auto		CreateTempDirectory() -> bool;
		auto		CreateTimecardObj(string timecard_id) -> C_Timecard_To_Print;

	public:
					C_Invoice_Service_Agency_To_CC(c_config *param1, CMysql *param2, CUser *param3) : config(param1), db(param2), user(param3) {};

		auto		SetTimecardList(const vector<string> &param1)	{ timecard_id_list = param1; };

		auto		SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto		SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };

		auto		GetInvoiceID()	const							{ return invoice_cost_center_service_id; };

		auto		GenerateDocumentArchive() -> string;

					~C_Invoice_Service_Agency_To_CC();
};

ostream&	operator<<(ostream& os, const C_Invoice_Service_Agency_To_CC &);

#endif
