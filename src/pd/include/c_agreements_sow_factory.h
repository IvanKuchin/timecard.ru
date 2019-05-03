#ifndef __C_AGREEMENTS_SOW_FACTORY__H__
#define __C_AGREEMENTS_SOW_FACTORY__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_invoicing_vars.h"
#include "c_agreements_sow_object.h"
#include "c_archive.h"

using namespace std;

class C_Agreements_SoW_Factory
{
	private:
		CMysql							*db = NULL;
		CUser							*user = NULL;
		vector<C_Agreements_SoW_Object>	agreement_list;

		C_Invoicing_Vars				invoicing_vars;

		string							sow_id = "";

		string							temp_dir = "";
		string							temp_dir_agency = "";
		string							temp_dir_sow = "";
		string							temp_archive_file = "";	// --- archive in temp_dir
		string							archive_folder = "";	// --- production archive folder
		string							archive_file = "";		// --- archive in production folder


		auto		UpdateDBWithAgreementData(const string bt_id) -> string;
		auto		CreateTempDirectory() -> bool;

	public:
					C_Agreements_SoW_Factory()									{};
					C_Agreements_SoW_Factory(CMysql *param1, CUser *param2) : db(param1), user(param2) {};

		auto		SetSoWID(const string &param1)					{ sow_id = param1; };
		auto		SetSoWID(string &&param1) 						{ sow_id = move(param1); };

		auto		GenerateDocumentArchive() -> string;

					~C_Agreements_SoW_Factory();
};

ostream&	operator<<(ostream& os, const C_Agreements_SoW_Factory &);

#endif
