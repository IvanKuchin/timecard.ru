#include "c_invoice_service_subc_to_agency.h"

static auto GetTimecardLines_By_TimecardID(string timecard_id, CMysql *db, CUser *user)
{
	vector<tuple<string, string, string, string>>	result;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
			if(db)
			{
				auto affected = db->Query(
"SELECT `timecard_customers`.`title` as `customer`, `timecard_projects`.`title` as `project`, `timecard_tasks`.`title` as `task`,  `timecard_lines`.`row` as `hours`"
"FROM `timecard_lines` "
"INNER JOIN `timecard_tasks` ON `timecard_tasks`.`id`=`timecard_lines`.`timecard_task_id` "
"INNER JOIN `timecard_projects` ON `timecard_projects`.`id`=`timecard_tasks`.`timecard_projects_id` "
"INNER JOIN `timecard_customers` ON `timecard_customers`.`id`=`timecard_projects`.`timecard_customers_id` "
"WHERE "
	"("
		"`timecard_lines`.`timecard_id`=\"" + timecard_id + "\""
	")"
					);
				if(affected)
				{
					for(int i = 0; i < affected; ++i)
					{
						auto	temp = make_tuple<string, string, string, string>(db->Get(i, "customer"), db->Get(i, "project"), db->Get(i, "task"), db->Get(i, "hours"));
						result.push_back(temp);
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "it is expected that valid entries will be here, otherwise timecard.id(" + timecard_id + ") should not appear in GUI");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "db not initialized");
			}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result.size is " + to_string(result.size()) + ")");

	return result;
}


auto C_Invoice_Service_Subc_To_Agency::GenerateDocumentArchive() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto									error_message = ""s;

	C_Print_Timecard						timecard_printer;

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
		// --- create object list
		for(auto &timecard_id: timecard_id_list)
		{
			auto	temp_obj = CreateTimecardObj(timecard_id);

			if((error_message = temp_obj.isValid()).empty())
			{
				timecard_obj_list.push_back(temp_obj);
			}
			else
			{
				MESSAGE_ERROR("", "", "timecard_obj(" + timecard_id + ") is broken don't add it to the list");
				break;
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard won't be built due to previous error");
	}

	// --- generate variable for invoicing
	if(error_message.empty())
	{
		invoicing_vars.SetDB(db);
		invoicing_vars.SetUser(user);
		invoicing_vars.SetTimecards(timecard_obj_list);

		error_message = invoicing_vars.GenerateServiceVariableSet_SubcToAgency();
	}
	else
	{
		MESSAGE_ERROR("", "", "invoicing_vars won't be generated due to previous error");
	}

	// --- timecard generator
	if(error_message.empty())
	{
		auto	i = 0;

		// --- print timecard to XLS / PDF
		for(auto &timecard: timecard_obj_list)
		{
			auto		filename_xls = ""s;
			auto		filename_pdf = ""s;

			do
			{
				++i;
				filename_xls = temp_dir_timecards + "timecard_" + to_string(i) + ".xls";
				filename_pdf = temp_dir_timecards + "timecard_" + to_string(i) + ".pdf";
			} while(isFileExists(filename_xls) || isFileExists(filename_pdf));

			timecard_printer.SetTimecard(timecard);
			timecard_printer.SetVariableSet(&invoicing_vars);
			timecard_printer.SetHolidays(GetHolidaysSet(GetSpellingFormattedDate(timecard.GetDateStart(), "%F"), db));

			timecard_printer.SetFilename(filename_xls);
			error_message = timecard_printer.PrintAsXLS();
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", "fail to build timecard_" + to_string(i) + ".xls");
				break;
			}

			timecard_printer.SetFilename(filename_pdf);
			error_message = timecard_printer.PrintAsPDF();
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", "fail to build timecard_" + to_string(i) + ".pdf");
				break;
			}

			// --- we don't want to count total payment again, instead take it from printer
			// --- this may not be a good idea
			timecard.SetTotalPayment(timecard_printer.GetTotalPayment());
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecards won't be written to files due to previous error");
	}

	// --- invoice generator
	if(error_message.empty())
	{
		auto		i = 0;
		auto		invoice_filename_xls = ""s;
		auto		invoice_filename_pdf = ""s;
		auto		act_filename_xls = ""s;
		auto		act_filename_pdf = ""s;
		auto		vat_filename_xls = ""s;
		auto		vat_filename_pdf = ""s;

		do
		{
			++i;
			invoice_filename_xls			= temp_dir_agency_invoices + "invoice_" + to_string(i) + ".xls";
			invoice_filename_pdf			= temp_dir_agency_invoices + "invoice_" + to_string(i) + ".pdf";
			act_filename_xls				= temp_dir_agency_invoices + "act_" + to_string(i) + ".xls";
			act_filename_pdf				= temp_dir_agency_invoices + "act_" + to_string(i) + ".pdf";
			vat_filename_xls				= temp_dir_agency_invoices + "vat_" + to_string(i) + ".xls";
			vat_filename_pdf				= temp_dir_agency_invoices + "vat_" + to_string(i) + ".pdf";
		} while(
				isFileExists(invoice_filename_xls)		|| isFileExists(invoice_filename_pdf) ||
				isFileExists(act_filename_xls)			|| isFileExists(act_filename_pdf) ||
				isFileExists(vat_filename_xls)			|| isFileExists(vat_filename_pdf)
				);

		if(error_message.empty())
		{
			C_Print_Invoice_Subc					__invoice_agency;
			C_Print_Invoice_Docs_Base				*invoice_printer = &__invoice_agency;

			invoice_printer->SetDB(db);
			invoice_printer->SetVariableSet(&invoicing_vars);

			if(error_message.empty()) 
			{
				invoice_printer->SetFilename(invoice_filename_xls);
				error_message = invoice_printer->PrintAsXLS();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build invoice (xls format)");
				}
			}
			if(error_message.empty()) 
			{
				invoice_printer->SetFilename(invoice_filename_pdf);
				error_message = invoice_printer->PrintAsPDF();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build invoice (pdf format)");
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "due to previous error invoice won't be printed");
		}
 
		if(error_message.empty())
		{
			C_Print_Act_Subc						__act_agency;
			C_Print_Invoice_Docs_Base				*act_printer = &__act_agency;

			act_printer->SetDB(db);
			act_printer->SetVariableSet(&invoicing_vars);

			if(error_message.empty()) 
			{
				act_printer->SetFilename(act_filename_xls);
				error_message = act_printer->PrintAsXLS();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build act (xls format)");
				}
			}
			if(error_message.empty()) 
			{
				act_printer->SetFilename(act_filename_pdf);
				error_message = act_printer->PrintAsPDF();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build act (pdf format)");
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "due to previous error act won't be printed");
		}

	
		if(error_message.empty())
		{
			if(invoicing_vars.Get("subcontractor_company_vat_1") == "Y")
			{
				C_Print_VAT_Subc						__vat_obj;
				C_Print_VAT_Base						*vat_printer = &__vat_obj;

				vat_printer->SetDB(db);
				vat_printer->SetVariableSet(&invoicing_vars);

				if(error_message.empty()) 
				{
					vat_printer->SetFilename(vat_filename_xls);
					error_message = vat_printer->PrintAsXLS();
					if(error_message.empty()) {}
					else
					{
						MESSAGE_ERROR("", "", "fail to build vat (xls format)");
					}
				}
				if(error_message.empty()) 
				{
					vat_printer->SetFilename(vat_filename_pdf);
					error_message = vat_printer->PrintAsPDF();
					if(error_message.empty()) {}
					else
					{
						MESSAGE_ERROR("", "", "fail to build vat (pdf format)");
					}
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "company(" + invoicing_vars.Get("subcontractor_company_id_1") + ") works w/o vat, no need to generate vat-docs");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "due to previous error vat won't be printed");
		}
	
	}	
	else
	{
		MESSAGE_ERROR("", "", "invoice to agency won't be written to files due to previous error");
	}

	// --- update DB only if no errors earlier
	if(error_message.empty())
	{
		for(auto &timecard: timecard_obj_list)
		{
			if((error_message = UpdateDBWithInvoiceData(timecard.GetID())).length())
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

	if(error_message.empty())
	{
		// --- important to keep it scoped
		// --- archive closed in destructor
		{
			c_archive	ar;

			ar.SetFilename(INVOICES_SUBC_DIRECTORY + archive_folder + "/" + archive_file);
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

auto C_Invoice_Service_Subc_To_Agency::CreateTempDirectory() -> bool
{
	auto result = false;

	do
	{
		auto		__random = GetRandom(15);

		archive_folder = to_string( (int)(rand()/(RAND_MAX + 1.0) * INVOICES_SUBC_NUMBER_OF_FOLDERS) + 1);
		archive_file = GetRandom(15) + ARCHIVE_FILE_EXTENSION;

		temp_dir = TEMP_DIRECTORY_PREFIX + __random;
		temp_archive_file = TEMP_DIRECTORY_PREFIX + __random + ARCHIVE_FILE_EXTENSION;
	} while(isDirExists(temp_dir) || isFileExists(temp_archive_file) || isFileExists(INVOICES_SUBC_DIRECTORY + archive_folder + "/" + archive_file));

	if(CreateDir(temp_dir))
	{
		temp_dir += "/";
		temp_dir_timecards = temp_dir + "timecards/";
		temp_dir_agency_invoices = temp_dir + "invoices/";

		if(CreateDir(temp_dir_timecards))
		{
			if(CreateDir(temp_dir_agency_invoices))
			{
				result = true;
			}
			else
			{
				MESSAGE_ERROR("", "", "fail to create " + temp_dir_agency_invoices);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to create " + temp_dir_timecards);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to create " + temp_dir);
	}

	return result;
}

auto C_Invoice_Service_Subc_To_Agency::CreateTimecardObj(string timecard_id) -> C_Timecard_To_Print
{
	C_Timecard_To_Print		obj;

	MESSAGE_DEBUG("", "", "start (timecard_id: " + timecard_id + ")");

	if(db)
	{
		if(timecard_id.length())
		{
			{
				{
					{

						if(db->Query("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
						{
							auto	sow_id = db->Get(0, "contract_sow_id");

							obj.SetID(timecard_id);
							obj.SetSoWID(sow_id);
							obj.SetDateStart(db->Get(0, "period_start"));
							obj.SetDateFinish(db->Get(0, "period_end"));

							if(db->Query("SELECT * FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
							{

								obj.SetAgreementNumber(db->Get(0, "number"));
								obj.SetDateSign(db->Get(0, "sign_date"));
								obj.SetDayrate(db->Get(0, "day_rate"));

								if(db->Query("SELECT `name`,`vat` FROM `company` WHERE `id`=("
												"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\""
											");"))
								{
									obj.SetSignatureTitle1(ConvertHTMLToText(db->Get(0, "name")));
									obj.SetSupplierVAT(ConvertHTMLToText(db->Get(0, "vat")));

									if(db->Query("SELECT `name` FROM `company` WHERE `id`=("
													"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\""
												");"))
									{
										obj.SetSignatureTitle2(ConvertHTMLToText(db->Get(0, 0)));
										
										obj.SetApprovers(GetTimecard_ApprovalChain(timecard_id, db));

										obj.SetPosition1(GetValueFromDB("SELECT `value` FROM `company_custom_fields` WHERE `var_name`=\"subc2agency_timecard_position1\";", db));
										obj.SetPosition2(GetValueFromDB("SELECT `value` FROM `company_custom_fields` WHERE `var_name`=\"subc2agency_timecard_position2\";", db));
										obj.SetInitials1(GetValueFromDB("SELECT `value` FROM `company_custom_fields` WHERE `var_name`=\"subc2agency_timecard_signature1\";", db));
										obj.SetInitials2(GetValueFromDB("SELECT `value` FROM `company_custom_fields` WHERE `var_name`=\"subc2agency_timecard_signature2\";", db));

										{
											auto		timecard_lines = GetTimecardLines_By_TimecardID(timecard_id, db, user);
											
											for(auto &timecard_line : timecard_lines)
											{
												obj.AddTimecardLine(get<0>(timecard_line), get<1>(timecard_line), get<2>(timecard_line), get<3>(timecard_line));
											}
										}
									}
									else
										MESSAGE_ERROR("", "", "Agency title not found by sow.id(" + sow_id + ")");
								}
								else
									MESSAGE_ERROR("", "", "Subcontractor title and vat not found for sow.id(" + sow_id + ")");
							}
							else
							{
								MESSAGE_ERROR("", "", "sow_id not found using timecard.id(" + timecard_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "timecard_id(" + timecard_id + ") not found");
						}
					}
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "timecard_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db not initialized");
	}

	MESSAGE_DEBUG("", "", "finish");

	return obj;
}

auto C_Invoice_Service_Subc_To_Agency::UpdateDBWithInvoiceData(const string timecard_id) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(db->Query("SELECT `invoice_filename` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
			{
				auto	curr_fname = db->Get(0, 0);

				if(curr_fname.length())
				{
					MESSAGE_DEBUG("", "", "removeing existing invoice(" + curr_fname + ")");
					unlink((INVOICES_SUBC_DIRECTORY + curr_fname).c_str());
				}
				else
				{
					MESSAGE_DEBUG("", "", "existing invoice is empty, nothing to remove");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "timecard.id(" + timecard_id + ") not found");
			}

			db->Query("UPDATE `timecards` SET `invoice_filename`=\"" + archive_folder + "/" + archive_file + "\" WHERE `id`=\"" + timecard_id + "\";");
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

C_Invoice_Service_Subc_To_Agency::~C_Invoice_Service_Subc_To_Agency()
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

ostream& operator<<(ostream& os, const C_Invoice_Service_Subc_To_Agency &var)
{
	os << "object C_Invoice_Service_Subc_To_Agency [empty for now]";

	return os;
}

