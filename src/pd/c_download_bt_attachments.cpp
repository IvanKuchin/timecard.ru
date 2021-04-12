#include "c_download_bt_attachments.h"

auto C_Download_BT_Attachments::GenerateDocumentArchive() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	if(error_message.empty())
	{
		if(CreateTempDirectory()) {}
		else
		{
			MESSAGE_ERROR("", "", "fail to create temp directory");
			error_message = (gettext("fail to create temp directory"));
		}
	}

	if(error_message.empty())
	{
		if((error_message = SaveBTAttachmentsToTempDirectory()).empty()) {}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}


	if(error_message.empty())
	{
		// --- important to keep it scoped
		// --- archive closed in destructor
		{
			c_archive	ar;

			ar.SetFilename(IMAGE_TEMP_DAILY_CLEANUPDIRECTORY + "/" + archive_file);
			ar.SetFolderToArchive(temp_dir + "/");
			ar.Archive();
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "archive won't be generated due to previous error");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Download_BT_Attachments::CreateTempDirectory() -> bool
{
	auto result = false;

	do
	{
		auto		__random = GetRandom(15);

		archive_file = GetRandom(15) + ARCHIVE_FILE_EXTENSION;

		temp_dir = config->GetFromFile("image_folders", "TEMP_DIRECTORY_PREFIX") + __random;
		temp_archive_file = config->GetFromFile("image_folders", "TEMP_DIRECTORY_PREFIX") + __random + ARCHIVE_FILE_EXTENSION;
	} while(isDirExists(temp_dir) || isFileExists(temp_archive_file) || isFileExists(config->GetFromFile("image_folders", "INVOICES_SUBC_DIRECTORY") + archive_folder + "/" + archive_file));

	if(CreateDir(temp_dir))
	{
		temp_dir += "/";
		temp_dir_bt = temp_dir + "bt";

		if(CreateDir(temp_dir_bt))
		{
			result = true;
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to create " + temp_dir_bt);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to create " + temp_dir);
	}

	return result;
}

auto C_Download_BT_Attachments::SaveBTAttachmentsToTempDirectory() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto error_message = ""s;

	if(db)
	{
		if(GetBTID().length())
		{
			struct ItemClass
			{
				string	id;
				string	bt_id;
				string	date;
				string	bt_expense_id;
				string	value;
			};
			vector<ItemClass>		itemsList;

			auto	affected = db->Query("SELECT * FROM `bt_download_attachments_view` WHERE `bt_id`=" + quoted(GetBTID()) + ";");

			for(auto i = 0; i < affected; ++i)
			{
				ItemClass	item;

				item.id				= db->Get(i, "id");
				item.bt_id			= db->Get(i, "bt_id");
				item.date			= db->Get(i, "date");
				item.bt_expense_id	= db->Get(i, "bt_expense_id");
				item.value			= db->Get(i, "value");

				itemsList.push_back(item);
			}

			for(auto &item: itemsList)
			{
				if(item.value.length())
				{
					auto	curr_subdir = temp_dir_bt + "/" + item.date;
					auto	curr_dir    = temp_dir_bt + "/" + item.date + "/" + item.bt_expense_id;

					if(isDirExists(curr_subdir)) {}
					else
					{
						if(CreateDir(curr_subdir)) {}
						else
						{
							error_message = "fail to create " + curr_subdir;
							MESSAGE_ERROR("", "", error_message);
							break;
						}
					}

					if(isDirExists(curr_dir)) {}
					else
					{
						if(CreateDir(curr_dir)) {}
						else
						{
							error_message = "fail to create " + curr_dir;
							MESSAGE_ERROR("", "", error_message);
							break;
						}
					}

					CopyFile(IMAGE_EXPENSELINES_DIRECTORY + "/"s + item.value, curr_dir + "/" + item.id + "." + GetFileExtension(item.value));
				}
				else
				{
					MESSAGE_DEBUG("", "", "no file uploaded for expense line.id(" + item.id + ")");
				}
			}
		}
		else
		{
			error_message = gettext("mandatory parameter missed");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("db is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish(" + error_message + ")");

	return error_message;
}


C_Download_BT_Attachments::~C_Download_BT_Attachments()
{
	if(temp_dir.length())
	{
		RmDirRecursive(temp_dir.c_str());
	}
	else
	{
		MESSAGE_DEBUG("", "", "temp_dir is not defined, no need to remove it");
	}
}

ostream& operator<<(ostream& os, const C_Download_BT_Attachments &var)
{
	os << "object C_Download_BT_Attachments [empty for now]";

	return os;
}

