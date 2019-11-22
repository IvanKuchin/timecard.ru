#ifndef __C_DOWNLOAD_BT_ATTACHMENTS__H__
#define __C_DOWNLOAD_BT_ATTACHMENTS__H__

#include <string>

#include "cuser.h"
#include "mysql.h"
#include "clog.h"
#include "utilities.h"
#include "c_archive.h"

using namespace std;

class C_Download_BT_Attachments
{	private:
		CMysql							*db = NULL;
		CUser							*user = NULL;

		string							bt_id = ""s;

		string							temp_dir = "";
		string							temp_dir_bt = "";

		string							temp_archive_file = "";	// --- archive in temp_dir
		string							archive_folder = "";	// --- production archive folder
		string							archive_file = "";		// --- archive in production folder


		auto		CreateTempDirectory() -> bool;
		auto 		SaveBTAttachmentsToTempDirectory() -> string;
	public:
					C_Download_BT_Attachments()									{};
					C_Download_BT_Attachments(CMysql *param1, CUser *param2) : db(param1), user(param2) {};

		void		SetBTID(const string &param) 				{ bt_id = param; }
		void		SetBTID(string &&param) noexcept			{ bt_id = move(param); }
		string		GetBTID() const								{ return bt_id; }
		string		GetArchiveFilename() const					{ return archive_file; }

		auto		GenerateDocumentArchive() -> string;

					~C_Download_BT_Attachments();
};

/*
--- MySQL view
bt_download_attachments_view

SELECT `exp_line`.`id`, `bt_id`,`date`,`bt_expense_id`,`value` FROM `bt_expense_lines`as `exp_line`
INNER JOIN `bt_expense_line_templates` as `exp_line_templ` ON `exp_line_templ`.`id`=`exp_line`.`bt_expense_line_template_id`
INNER JOIN `bt_expenses` as `exp` ON `exp`.`id`=`exp_line`.`bt_expense_id`
WHERE `exp_line_templ`.`dom_type`="image"
*/

ostream&	operator<<(ostream& os, const C_Download_BT_Attachments &);

#endif
