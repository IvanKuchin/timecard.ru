#ifndef __C_AGREEMENTS_SOW_FACTORY__H__
#define __C_AGREEMENTS_SOW_FACTORY__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_invoicing_vars.h"
#include "c_agreements_sow_object.h"
#include "c_template2pdf_printer.h"
#include "c_archive.h"
#include "wkhtmltox_wrapper.h"
#include "localy_pd.h"

using namespace std;

class C_Agreements_SoW_Factory
{
	private:
		c_config						*config = NULL;
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


		auto		RemoveExistingAgreementFile() -> string;
		auto		UpdateDBWithAgreementFile() -> string;
		auto		CreateTempDirectory() -> bool;
		auto		ProduceObjectVector() -> string;

	public:
					C_Agreements_SoW_Factory()						{};
					C_Agreements_SoW_Factory(c_config *param1, CMysql *param2, CUser *param3) : config(param1), db(param2), user(param3) {};

		auto		SetSoWID(const string &param1)					{ sow_id = param1; };
		auto		SetSoWID(string &&param1) 						{ sow_id = move(param1); };

		auto		GenerateDocumentArchive() -> string;
		auto		GetShortFilename()								{ return archive_folder + "/" + archive_file; };

					~C_Agreements_SoW_Factory();
};

ostream&	operator<<(ostream& os, const C_Agreements_SoW_Factory &);

#endif
