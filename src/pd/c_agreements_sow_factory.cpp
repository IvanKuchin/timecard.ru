#include "c_agreements_sow_factory.h"

auto C_Agreements_SoW_Factory::GenerateDocumentArchive() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto									error_message = ""s;

	if(db == nullptr)
	{
		error_message = gettext("SQL syntax issue");
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

				template2pdf_printer.SetTemplate(agreement.GetFilename());
				template2pdf_printer.SetFilename(output_filename);

				if((error_message = template2pdf_printer.RenderTemplate()).length())
				{
					MESSAGE_ERROR("", "", "fail to render template " + agreement.GetFilename());
				}

				if((error_message = template2pdf_printer.ConvertHTML2PDF()).length())
				{
					MESSAGE_ERROR("", "", "fail to conert html to pdf (" + agreement.GetFilename() + ")");
				}

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

/*
	// --- update DB only if no errors earlier
	if(error_message.empty())
	{
		for(auto &bt: bt_obj_list)
		{
			if((error_message = UpdateDBWithInvoiceData(bt.GetID())).length())
			{
				MESSAGE_ERROR("", "", "fail to update DB");
				break;
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db won't be updated due to previous error");
	}
*/
	if(error_message.empty())
	{
		// --- important to keep it scoped
		// --- archive closed in destructor
		{
			c_archive	ar;

			ar.SetFilename(AGREEMENTS_SOW_DIRECTORY + archive_folder + "/" + archive_file);
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

		archive_folder = to_string( (int)(rand()/(RAND_MAX + 1.0) * AGREEMENTS_SOW_NUMBER_OF_FOLDERS) + 1);
		archive_file = GetRandom(15) + ARCHIVE_FILE_EXTENSION;

		temp_dir = TEMP_DIRECTORY_PREFIX + __random;
		temp_archive_file = TEMP_DIRECTORY_PREFIX + __random + ARCHIVE_FILE_EXTENSION;
	} while(isDirExists(temp_dir) || isFileExists(temp_archive_file) || isFileExists(AGREEMENTS_SOW_DIRECTORY + archive_folder + "/" + archive_file));

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
/*
auto C_Agreements_SoW_Factory::UpdateDBWithInvoiceData(const string bt_id) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(invoice_cost_center_bt_id == 0)
			{
				auto	owner_company_id = GetAgencyIDByUserID(db, user);
				
				if(owner_company_id.length())
				{
					// --- new invoice
					invoice_cost_center_bt_id = db->InsertQuery( "INSERT INTO `invoice_cost_center_bt` (`cost_center_id`, `file`, `owner_company_id`, `owner_user_id`, `eventTimestamp`)"
										"VALUES (" + 
											quoted(cost_center_id) + "," +
											quoted(archive_folder + "/" + archive_file) + "," +
											quoted(owner_company_id) + "," +
											quoted(user->GetID()) + "," +
											"UNIX_TIMESTAMP()"
										");");
					if(invoice_cost_center_bt_id)
					{
						// --- everything is fine, increase act_number assigned to this cost_center
						db->Query("UPDATE `cost_centers` SET `act_number`=`act_number`+1 WHERE `id`=\"" + cost_center_id + "\";");
						if(db->isError())
						{
							MESSAGE_ERROR("", "", "fail to increase act_number in cost_center table");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to insert to db");
						error_message = gettext("fail to insert to db");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "agency not found where user.id(" + user->GetID() + ") working at");
					error_message = gettext("employeer not found");
				}

			}

			// --- don't merge it with previous if()
			if(invoice_cost_center_bt_id)
			{
				// --- find appropriate bt.id in invoicing_vars
				auto	bt_index = invoicing_vars.GetIndexByTimecardID(bt_id);

				if(db->InsertQuery( "INSERT INTO `invoice_cost_center_bt_details` (`invoice_cost_center_bt_id`, `bt_id`, `cc_amount_pre_tax`, `cc_amount_tax`, `cc_amount_total`, `subc_amount_pre_tax`, `subc_amount_tax`, `subc_amount_total`)"
									"VALUES (" + 
										quoted(to_string(invoice_cost_center_bt_id)) + "," +
										quoted(bt_id) + "," +
										quoted(invoicing_vars.Get("cost_center_price_" + bt_index).length() ? invoicing_vars.Get("cost_center_price_" + bt_index) : "0") + "," +
										quoted(invoicing_vars.Get("cost_center_vat_" + bt_index).length() ? invoicing_vars.Get("cost_center_vat_" + bt_index) : "0") + "," +
										quoted(invoicing_vars.Get("cost_center_total_" + bt_index).length() ? invoicing_vars.Get("cost_center_total_" + bt_index) : "0") + "," +
										quoted(invoicing_vars.Get("bt_price_" + bt_index).length() ? invoicing_vars.Get("bt_price_" + bt_index) : "0") + "," +
										quoted(invoicing_vars.Get("bt_vat_" + bt_index).length() ? invoicing_vars.Get("bt_vat_" + bt_index) : "0") + "," +
										quoted(invoicing_vars.Get("bt_total_" + bt_index).length() ? invoicing_vars.Get("bt_total_" + bt_index) : "0") +
									");"))
				{
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to insert to db");
					error_message = gettext("fail to insert to db");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "invoice id is empty. Most probably insert to invoice_cost_center_service failed");
				error_message = gettext("fail to insert to db");
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
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}
*/

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
			temp.SetFilename(TEMPLATE_AGREEMENT_SOW_DIRECTORY + "/" + db->Get(i, "filename"));

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
			temp.SetFilename(TEMPLATE_AGREEMENT_COMPANY_DIRECTORY + "/" + db->Get(i, "filename"));

		agreement_list.push_back(temp);
	}


	MESSAGE_DEBUG("", "", "finish (error_message: " + error_message + ")");

	return error_message;
}

C_Agreements_SoW_Factory::~C_Agreements_SoW_Factory()
{
	if(temp_dir.length())
	{
		// RmDirRecursive(temp_dir.c_str());
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

