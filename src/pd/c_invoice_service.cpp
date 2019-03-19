#include "c_invoice_service.h"

/*
static void error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
}
*/


static auto GetTimecardLines_By_TimecardID_And_CostCenterID(string timecard_id, string cost_center_id, CMysql *db, CUser *user)
{
	vector<tuple<string, string, string, string>>	result;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(user->GetType() == "agency")
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
	"AND"
	"("
		"`timecard_lines`.`timecard_task_id` IN "
		"("
			"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN"
			"("
				"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id` IN"
				"("
					"SELECT `timecard_customers_id` FROM `cost_center_assignment` WHERE `cost_center_id`=\"" + cost_center_id + "\" "
				")"
			")"
		")"
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
					MESSAGE_ERROR("", "", "it is expected that valid entries will be here, otherwise cost_center.id(" + cost_center_id + ") and timecard.id(" + timecard_id + ") should not appear in GUI");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "db not initialized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") is not an agency employee (" + user->GetType() + ")");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result.size is " + to_string(result.size()) + ")");

	return result;
}


C_Invoice_Service::C_Invoice_Service() {}

C_Invoice_Service::C_Invoice_Service(CMysql *param1, CUser *param2) : db(param1), user(param2) {}

auto C_Invoice_Service::GenerateDocumentArchive() -> string
{
	C_Print_Timecard	printer;
	auto				error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty())
	{
		if(CreateTempDirectory())
		{

		}
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

			if(temp_obj.isValid())
				timecard_obj_list.push_back(temp_obj);
			else
			{
				MESSAGE_ERROR("", "", "timecard_obj(" + timecard_id + ") is broken don't add it to the list");
				error_message = (gettext("fail to craft timecard obj"));
			}
		}
	}

	if(error_message.empty())
	{
		auto	i = 0;

		// --- print timecard to XLS
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

			printer.SetTimecard(timecard);

			printer.SetFilename(filename_xls);
			error_message = printer.PrintAsXLS();
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", "fail to build timecard_" + to_string(i) + ".xls");
				break;
			}

			printer.SetFilename(filename_pdf);
			error_message = printer.PrintAsPDF();
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", "fail to build timecard_" + to_string(i) + ".pdf");
				break;
			}

			// --- we don't want to count total payment again, instead take it from printer
			// --- this may not be a good idea
			if((error_message = UpdateDBWithInvoiceData(timecard.GetID(), printer.GetTotalPayment())).length())
			{
				break;
			}
		}

		// --- important to keep it scoped
		// --- archive closed in destructor
		{
			c_archive	ar;

			ar.SetFilename(INVOICES_CC_DIRECTORY + archive_folder + "/" + archive_file);
			ar.SetFolderToArchive(temp_dir);
			ar.Archive();
		}
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Invoice_Service::CreateTempDirectory() -> bool
{
	auto result = false;

	do
	{
		auto		__random = GetRandom(15);

		archive_folder = to_string( (int)(rand()/(RAND_MAX + 1.0) * INVOICES_CC_NUMBER_OF_FOLDERS) + 1);
		archive_file = GetRandom(15) + ARCHIVE_FILE_EXTENSION;

		temp_dir = TEMP_DIRECTORY_PREFIX + __random;
		temp_archive_file = TEMP_DIRECTORY_PREFIX + __random + ARCHIVE_FILE_EXTENSION;
	} while(isDirExists(temp_dir) || isFileExists(temp_archive_file) || isFileExists(INVOICES_CC_DIRECTORY + archive_folder + "/" + archive_file));

	if(CreateDir(temp_dir))
	{
		temp_dir += "/";
		temp_dir_timecards = temp_dir + "timecards/";
		temp_dir_cost_center_invoices = temp_dir + "invoices/";

		if(CreateDir(temp_dir_timecards))
		{
			if(CreateDir(temp_dir_cost_center_invoices))
			{
				result = true;
			}
			else
			{
				MESSAGE_ERROR("", "", "fail to create " + temp_dir_cost_center_invoices);
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

auto C_Invoice_Service::CreateTimecardObj(string timecard_id) -> C_Timecard_To_Print
{
	C_Timecard_To_Print		obj;

	MESSAGE_DEBUG("", "", "start (timecard_id: " + timecard_id + ")");

	if(db)
	{
		if(timecard_id.length())
		{
			if(cost_center_id.length())
			{
				string	psow_id = GetPSoWIDByTimecardIDAndCostCenterID(timecard_id, cost_center_id, db, user);

				if(psow_id.length())
				{
					// string	sow_id = GetSoWIDByTimecardID(timecard_id, db, user);
					string	sow_id = "fake uncomment prev line";

					if(sow_id.length())
					{
						if(db->Query("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
						{
							obj.SetID(timecard_id);
							obj.SetDateStart(db->Get(0, "period_start"));
							obj.SetDateFinish(db->Get(0, "period_end"));

							if(db->Query("SELECT * FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\";"))
							{
								obj.SetAgreementNumber(db->Get(0, "number"));
								obj.SetDateSign(db->Get(0, "sign_date"));
								obj.SetDayrate(db->Get(0, "day_rate"));

								if(db->Query("SELECT `name` FROM `company` WHERE `id`=("
												"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
													"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\""
												")"
											");"))
								{
									obj.SetSignatureTitle1(ConvertHTMLToText(db->Get(0, "name")));
									
									if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=\"" + cost_center_id + "\";"))
									{
										obj.SetSignatureTitle2(ConvertHTMLToText(db->Get(0, "title")));
										
										if(db->Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `var_name`=\"Department\" AND `contract_psow_id`=\"" + psow_id + "\" AND `type`=\"input\";"))
											obj.SetProjectNumber(db->Get(0, "value"));
										else
											MESSAGE_DEBUG("", "", "optional field Department not found for psow.id(" + psow_id + ")");

										{
											auto		timecard_lines = GetTimecardLines_By_TimecardID_And_CostCenterID(timecard_id, cost_center_id, db, user);
											
											for(auto &timecard_line : timecard_lines)
											{
												obj.AddTimecardLine(get<0>(timecard_line), get<1>(timecard_line), get<2>(timecard_line), get<3>(timecard_line));
											}
										}
									}
									else
										MESSAGE_ERROR("", "", "CostCenter title not found for cost_center.id(" + cost_center_id + ")");
								}
								else
									MESSAGE_ERROR("", "", "Agency title not found for psow.id(" + psow_id + ")");

							}
							else
							{
								MESSAGE_ERROR("", "", "psow_id(" + psow_id + ") not found");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "timecard_id(" + timecard_id + ") not found");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "sow_id not found");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "psow_id not found");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "cost_center_id is empty");
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

auto C_Invoice_Service::UpdateDBWithInvoiceData(const string timecard_id, c_float amount) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(invoice_cost_center_service_id == 0)
			{
				auto	owner_company_id = GetAgencyIDByUserID(db, user);
				
				if(owner_company_id.length())
				{
					// --- new invoice
					invoice_cost_center_service_id = db->InsertQuery( "INSERT INTO `invoice_cost_center_service` (`cost_center_id`, `file`, `owner_company_id`, `owner_user_id`, `eventTimestamp`)"
										"VALUES (" + 
											quoted(cost_center_id) + "," +
											quoted(archive_folder + "/" + archive_file) + "," +
											quoted(owner_company_id) + "," +
											quoted(user->GetID()) + "," +
											"UNIX_TIMESTAMP()"
										");");
					if(invoice_cost_center_service_id)
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
			if(invoice_cost_center_service_id)
			{
				if(db->InsertQuery( "INSERT INTO `invoice_cost_center_service_details` (`invoice_cost_center_service_id`, `timecard_id`, `amount`)"
									"VALUES (" + 
										quoted(to_string(invoice_cost_center_service_id)) + "," +
										quoted(timecard_id) + "," +
										quoted(string(amount)) +
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

C_Invoice_Service::~C_Invoice_Service()
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

ostream& operator<<(ostream& os, const C_Invoice_Service &var)
{
	os << "object C_Invoice_Service [empty for now]";

	return os;
}

