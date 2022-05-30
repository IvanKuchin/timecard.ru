#include "c_agreements_sow_factory.h"

auto C_Agreements_SoW_Factory::GenerateDocumentArchive() -> string
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db == nullptr)
	{
		error_message = gettext("SQL syntax error");
		MESSAGE_ERROR("", "", "db not initialized");
	}

	if(sow_id.empty())
	{
		error_message = gettext("Data has not been initialized");
		MESSAGE_ERROR("", "", "sow_id not initialized");
	}

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
		if((ProduceObjectVector()).empty()) {}
		else
		{
			error_message = (gettext("fail to produce object set"));
			MESSAGE_ERROR("", "", error_message);
		}
	}

	// --- generate variable for invoicing
	if(error_message.empty())
	{
		invoicing_vars.SetConfig(config);
		invoicing_vars.SetDB(db);
		invoicing_vars.SetUser(user);
		invoicing_vars.SetSoWID(sow_id);

		error_message = invoicing_vars.GenerateSoWVariableSet();
	}
	else
	{
		MESSAGE_ERROR("", "", "invoicing_vars won't be generated due to previous error");
	}

	// --- agreements sow generator
	if(error_message.empty())
	{
		C_Template2PDF_Printer	template2pdf_printer;
		wkhtmltox_wrapper		wrapper;

		template2pdf_printer.SetDB(db);
		template2pdf_printer.SetVariableSet(&invoicing_vars);

		// --- print bt to XLS / PDF
		for(auto &agreement: agreement_list)
		{
			if(agreement.GetFilename().length())
			{
				auto		output_filename = ""s;
				auto		i = 0;

				do
				{
					output_filename = temp_dir + agreement.GetTitle() + (i ? "_" + to_string(i) : "") + ".pdf";
					++i;
				} while(isFileExists(output_filename));

				template2pdf_printer.SetProdTemplate(agreement.GetFilename());
				template2pdf_printer.SetFilename(output_filename);

				if((error_message = template2pdf_printer.RenderTemplate()).length())
				{
					MESSAGE_ERROR("", "", "fail to render template " + agreement.GetFilename());
				}

				wrapper.SetSrc(output_filename + ".html");
				wrapper.SetDst(output_filename);
				wrapper.Convert();

				if(error_message.length()) break;
			}
			else
			{
				MESSAGE_DEBUG("", "", "skip agreement due to empty template");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "Agreements won't be written to files due to previous error");
	}

	if(error_message.empty())
	{
		if((error_message = RemoveExistingAgreementFile()).length())
		{
			MESSAGE_ERROR("", "", "fail to update DB");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "old file won't be removed due to previous error");
	}

	// --- update DB only if no errors earlier
	if(error_message.empty())
	{
		if((error_message = UpdateDBWithAgreementFile()).length())
		{
			MESSAGE_ERROR("", "", "fail to update DB");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db won't be updated due to previous error");
	}

	if(error_message.empty())
	{
		// --- important to keep it scoped
		// --- archive closed in destructor
		{
			c_archive	ar;

			ar.SetFilename(config->GetFromFile("image_folders", "AGREEMENTS_SOW_DIRECTORY") + GetShortFilename());
			ar.SetFolderToArchive(temp_dir);
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

auto C_Agreements_SoW_Factory::CreateTempDirectory() -> bool
{
	auto result = false;

	do
	{
		auto		__random = GetRandom(15);

		archive_folder = to_string( (int)(rand()/(RAND_MAX + 1.0) * stod_noexcept(config->GetFromFile("number_of_folders", "AGREEMENTS_SOW_NUMBER_OF_FOLDERS"))) + 1);
		archive_file = GetRandom(15) + ARCHIVE_FILE_EXTENSION;

		temp_dir = config->GetFromFile("image_folders", "TEMP_DIRECTORY_PREFIX") + __random;
		temp_archive_file = config->GetFromFile("image_folders", "TEMP_DIRECTORY_PREFIX") + __random + ARCHIVE_FILE_EXTENSION;
	} while(isDirExists(temp_dir) || isFileExists(temp_archive_file) || isFileExists(config->GetFromFile("image_folders", "AGREEMENTS_SOW_DIRECTORY") + archive_folder + "/" + archive_file));

	if(CreateDir(temp_dir))
	{
		temp_dir += "/";
		result = true;
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to create " + temp_dir);
	}

	return result;
}

auto C_Agreements_SoW_Factory::UpdateDBWithAgreementFile() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(db)
		{
			if(GetShortFilename().length())
			{
				db->Query("UPDATE `contracts_sow` SET `agreement_filename`=\"" + GetShortFilename() + "\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to update filename in contracts_sow table");
				}
			}
			else
			{
				error_message = gettext("Data has not been initialized");
				MESSAGE_ERROR("", "", "archive filename is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		error_message = gettext("Data has not been initialized");
		MESSAGE_ERROR("", "", "sow_id is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Agreements_SoW_Factory::RemoveExistingAgreementFile() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db)
	{
		if(GetShortFilename().length())
		{
			if(db->Query("SELECT `agreement_filename` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
			{
				auto	existing_filename = db->Get(0, 0);

				if(existing_filename.length())
				{
					MESSAGE_DEBUG("", "", "removing: " + config->GetFromFile("image_folders", "AGREEMENTS_SOW_DIRECTORY") + existing_filename);
					unlink((config->GetFromFile("image_folders", "AGREEMENTS_SOW_DIRECTORY") + existing_filename).c_str());
				}
				else
				{
					MESSAGE_DEBUG("", "", "filename is empty. nothing to remove");
				}
			}
			else
			{
				error_message = gettext("SQL syntax error");
				MESSAGE_DEBUG("", "", "SoW doesn't exists in DB. This is unacceptable at this stage. Investigate the reason.");
			}
		}
		else
		{
			error_message = gettext("Data has not been initialized");
			MESSAGE_ERROR("", "", "archive filename is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db is not initialized");
		error_message = gettext("db is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}


auto C_Agreements_SoW_Factory::ProduceObjectVector() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	affected = db->Query("SELECT * FROM `contract_sow_agreement_files` WHERE `contract_sow_id`=\"" + sow_id + "\";");

	for(int i = 0; i < affected; ++i)
	{
		C_Agreements_SoW_Object	temp;

		temp.SetSoWID(sow_id);
		temp.SetID(db->Get(i, "id"));
		temp.SetTitle(db->Get(i, "title"));
		if(db->Get(i, "filename").length())
			temp.SetFilename(config->GetFromFile("image_folders", "template_agreement_sow") + "/" + db->Get(i, "filename"));

		agreement_list.push_back(temp);
	}

	affected = db->Query("SELECT * FROM `company_agreement_files` WHERE `company_id`=(SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");");

	for(int i = 0; i < affected; ++i)
	{
		C_Agreements_SoW_Object	temp;

		temp.SetSoWID(sow_id);
		temp.SetID(db->Get(i, "id"));
		temp.SetTitle(db->Get(i, "title"));
		if(db->Get(i, "filename").length())
			temp.SetFilename(config->GetFromFile("image_folders", "template_agreement_company") + "/" + db->Get(i, "filename"));

		agreement_list.push_back(temp);
	}


	MESSAGE_DEBUG("", "", "finish (error_message: " + error_message + ")");

	return error_message;
}

C_Agreements_SoW_Factory::~C_Agreements_SoW_Factory()
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

ostream& operator<<(ostream& os, const C_Agreements_SoW_Factory &var)
{
	os << "object C_Agreements_SoW_Factory [empty for now]";

	return os;
}

