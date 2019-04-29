#ifndef __C_INVOICE_BT__H__
#define __C_INVOICE_BT__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_invoicing_vars.h"
#include "c_bt_to_print.h"
#include "c_print_bt.h"
#include "c_print_invoice_docs.h"
#include "c_print_vat.h"
#include "c_print_1c_costcenter.h"
#include "c_print_1c_subcontractor.h"
#include "c_archive.h"

using namespace std;

class C_Invoice_BT
{
	private:
		CMysql							*db = NULL;
		CUser							*user = NULL;
		vector<string>					bt_id_list;
		vector<C_BT_To_Print>			bt_obj_list;
		C_Invoicing_Vars				invoicing_vars;
		string							cost_center_id = "";

		long							invoice_cost_center_bt_id = 0;
		c_float							total_payment;

		string							temp_dir = "";
		string							temp_dir_bt = "";
		string							temp_dir_cost_center_invoices = "";
		string							temp_dir_1c = "";
		string							temp_archive_file = "";	// --- archive in temp_dir
		string							archive_folder = "";	// --- production archive folder
		string							archive_file = "";		// --- archive in production folder


		auto		UpdateDBWithInvoiceData(const string bt_id) -> string;
		auto		CreateTempDirectory() -> bool;
		auto		CreateBTObj(string bt_id) -> C_BT_To_Print;
		auto 		EnrichObjWithExpenseLines(string bt_id, C_BT_To_Print *obj) -> bool;

	public:
					C_Invoice_BT();
					C_Invoice_BT(CMysql *, CUser *);

		auto		SetBTList(const vector<string> &param1)			{ bt_id_list = param1; };

		auto		SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto		SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };

		auto		GetInvoiceID()									{ return invoice_cost_center_bt_id; };

		auto		GenerateDocumentArchive() -> string;

					~C_Invoice_BT();
};

ostream&	operator<<(ostream& os, const C_Invoice_BT &);

#endif
