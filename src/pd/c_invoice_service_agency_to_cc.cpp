#include "c_invoice_service_agency_to_cc.h"

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
					"SELECT `timecard_customer_id` FROM `cost_center_assignment` WHERE `cost_center_id`=\"" + cost_center_id + "\" "
				")"
			")"
		")"
	")"
					);
				if(affected)
				{
					for(int i = 0; i < affected; ++i)
					{
						auto	temp = make_tuple<string, string, string, string>(ConvertHTMLToText(db->Get(i, "customer")), ConvertHTMLToText(db->Get(i, "project")), ConvertHTMLToText(db->Get(i, "task")), db->Get(i, "hours"));
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


auto C_Invoice_Service_Agency_To_CC::GenerateDocumentArchive() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto									error_message = ""s;

	C_Print_Timecard						timecard_printer;

	C_Print_Invoice_Agency					__invoice_agency;
	C_Print_Invoice_Docs_Base				*invoice_printer = &__invoice_agency;

	C_Print_Act_Agency						__act_agency;
	C_Print_Invoice_Docs_Base				*act_printer = &__act_agency;

	C_Print_VAT_Agency						__vat_obj;
	C_Print_VAT_Base						*vat_printer = &__vat_obj;

	C_Print_1C_CostCenter_Service			__cc_1c_main_obj1;
	C_Print_1C_CostCenter_Base				*cc_1c_selling_printer = &__cc_1c_main_obj1;

	C_Print_1C_Subcontractor_Payment		__subc_1c_main_obj1;
	C_Print_1C_Subcontractor_Base			*subc_1c_payment_printer = &__subc_1c_main_obj1;

	C_Print_1C_Subcontractor_Payment_Order	__subc_1c_main_obj2;
	C_Print_1C_Subcontractor_Base			*subc_1c_payment_order_printer = &__subc_1c_main_obj2;

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
		invoicing_vars.SetConfig(config);
		invoicing_vars.SetDB(db);
		invoicing_vars.SetUser(user);
		invoicing_vars.SetCostCenterID(cost_center_id);
		invoicing_vars.SetTimecards(timecard_obj_list);

		error_message = invoicing_vars.GenerateServiceVariableSet_AgencyToCC();
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
		auto		cc_service_filename_1c = ""s;
		auto		subc_payment_filename_1c = ""s;
		auto		subc_payment_order_filename_1c = ""s;

		do
		{
			++i;
			invoice_filename_xls			= temp_dir_cost_center_invoices + "invoice_" + to_string(i) + ".xls";
			invoice_filename_pdf			= temp_dir_cost_center_invoices + "invoice_" + to_string(i) + ".pdf";
			act_filename_xls				= temp_dir_cost_center_invoices + "act_" + to_string(i) + ".xls";
			act_filename_pdf				= temp_dir_cost_center_invoices + "act_" + to_string(i) + ".pdf";
			vat_filename_xls				= temp_dir_cost_center_invoices + "vat_" + to_string(i) + ".xls";
			vat_filename_pdf				= temp_dir_cost_center_invoices + "vat_" + to_string(i) + ".pdf";
			cc_service_filename_1c			= temp_dir_1c + "costcenter_service_" + to_string(i) + ".xml";
			// subc_payment_filename_1c		= temp_dir_1c + "subcontractor_payment_" + to_string(i) + ".xml";
			// subc_payment_order_filename_1c	= temp_dir_1c + "subcontractor_payment_order_" + to_string(i) + ".xml";
		} while(
				isFileExists(invoice_filename_xls)		|| isFileExists(invoice_filename_pdf) ||
				isFileExists(act_filename_xls)			|| isFileExists(act_filename_pdf) ||
				isFileExists(vat_filename_xls)			|| isFileExists(vat_filename_pdf) ||
				isFileExists(cc_service_filename_1c)
				// isFileExists(subc_payment_filename_1c)	|| isFileExists(subc_payment_order_filename_1c)
				);

		if(error_message.empty())
		{
			invoice_printer->SetDB(db);
			invoice_printer->SetVariableSet(&invoicing_vars);

			invoice_printer->SetCostCenterID(cost_center_id);

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
			act_printer->SetDB(db);
			act_printer->SetVariableSet(&invoicing_vars);

			act_printer->SetCostCenterID(cost_center_id);

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
			vat_printer->SetDB(db);
			vat_printer->SetVariableSet(&invoicing_vars);

			vat_printer->SetCostCenterID(cost_center_id);

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
			MESSAGE_ERROR("", "", "due to previous error vat won't be printed");
		}

		if(error_message.empty())
		{
			cc_1c_selling_printer->SetDB(db);
			cc_1c_selling_printer->SetVariableSet(&invoicing_vars);

			if(error_message.empty()) 
			{
				cc_1c_selling_printer->SetFilename(cc_service_filename_1c);
				error_message = cc_1c_selling_printer->Print();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build 1c cost center service");
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "due to previous error 1c cost center service won't be printed");
		}

		if(error_message.empty())
		{
			subc_1c_payment_printer->SetDB(db);
			subc_1c_payment_printer->SetVariableSet(&invoicing_vars);

			for(auto i = 1; subc_1c_payment_printer->isTableRowExists(i) && error_message.empty(); ++i)
			{
				subc_1c_payment_printer->SetIdx(i);
				
				// subc_1c_payment_printer->SetFilename(subc_payment_filename_1c);
				subc_1c_payment_printer->SetFolder(temp_dir_1c_subc_payments);
				error_message = subc_1c_payment_printer->Print();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build 1c subcontractor payment");
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "due to previous error 1c subcontractor payments won't be printed");
		}

		if(error_message.empty())
		{
			subc_1c_payment_order_printer->SetDB(db);
			subc_1c_payment_order_printer->SetVariableSet(&invoicing_vars);

			for(auto i = 1; subc_1c_payment_order_printer->isTableRowExists(i) && error_message.empty(); ++i)
			{
				subc_1c_payment_order_printer->SetIdx(i);

				// subc_1c_payment_order_printer->SetFilename(subc_payment_order_filename_1c);
				subc_1c_payment_order_printer->SetFolder(temp_dir_1c_subc_payment_orders);
				error_message = subc_1c_payment_order_printer->Print();
				if(error_message.empty()) {}
				else
				{
					MESSAGE_ERROR("", "", "fail to build 1c subcontractor payment order");
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "due to previous error 1c subcontractor payment orders won't be printed");
		}

	}	
	else
	{
		MESSAGE_ERROR("", "", "invoice to cost center won't be written to files due to previous error");
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

			ar.SetFilename(config->GetFromFile("image_folders", "INVOICES_CC_DIRECTORY") + archive_folder + "/" + archive_file);
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

auto C_Invoice_Service_Agency_To_CC::CreateTempDirectory() -> bool
{
	auto result = false;

	do
	{
		auto		__random = GetRandom(15);

		archive_folder = to_string( (int)(rand()/(RAND_MAX + 1.0) * stod_noexcept(config->GetFromFile("number_of_folders", "INVOICES_CC_NUMBER_OF_FOLDERS"))) + 1);
		archive_file = GetRandom(15) + ARCHIVE_FILE_EXTENSION;

		temp_dir = config->GetFromFile("image_folders", "TEMP_DIRECTORY_PREFIX") + __random;
		temp_archive_file = config->GetFromFile("image_folders", "TEMP_DIRECTORY_PREFIX") + __random + ARCHIVE_FILE_EXTENSION;
	} while(isDirExists(temp_dir) || isFileExists(temp_archive_file) || isFileExists(config->GetFromFile("image_folders", "INVOICES_CC_DIRECTORY") + archive_folder + "/" + archive_file));

	if(CreateDir(temp_dir))
	{
		temp_dir += "/";
		temp_dir_timecards = temp_dir + "timecards/";
		temp_dir_cost_center_invoices = temp_dir + "invoices/";
		temp_dir_1c = temp_dir + "1c/";
		temp_dir_1c_subc_payments = temp_dir_1c + "subc_payments/";
		temp_dir_1c_subc_payment_orders = temp_dir_1c + "subc_payment_orders/";

		if(CreateDir(temp_dir_timecards))
		{
			if(CreateDir(temp_dir_cost_center_invoices))
			{
				if(CreateDir(temp_dir_1c))
				{
					if(CreateDir(temp_dir_1c_subc_payments))
					{
						if(CreateDir(temp_dir_1c_subc_payment_orders))
						{
							result = true;
						}
						else
						{
							MESSAGE_ERROR("", "", "fail to create " + temp_dir_1c_subc_payment_orders);
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to create " + temp_dir_1c_subc_payments);
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to create " + temp_dir_1c);
				}
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

auto C_Invoice_Service_Agency_To_CC::CreateTimecardObj(string timecard_id) -> C_Timecard_To_Print
{
	C_Timecard_To_Print		obj;

	MESSAGE_DEBUG("", "", "start (timecard_id: " + timecard_id + ")");

	if(db)
	{
		if(timecard_id.length())
		{
			
			if(cost_center_id.length())
			{
				auto	psow_id = GetPSoWIDByTimecardIDAndCostCenterID(timecard_id, cost_center_id, db, user);

				if(psow_id.length())
				{
					// auto	sow_id = GetSoWIDByTimecardID(timecard_id, db, user);
					auto	sow_id = "fake uncomment prev line"s;

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
								obj.SetWorkingHoursPerDay(db->Get(0, "working_hours_per_day"));

								if(db->Query("SELECT `name`,`vat` FROM `company` WHERE `id`=("
												"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
													"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\""
												")"
											");"))
								{
									obj.SetSignatureTitle1(ConvertHTMLToText(db->Get(0, "name")));
									obj.SetSupplierVAT(ConvertHTMLToText(db->Get(0, "vat")));

									if(db->Query("SELECT `name` FROM `company` WHERE `id`=("
													"SELECT `company_id` FROM `cost_centers` WHERE `id`=\"" + cost_center_id + "\""
												");"))
									{
										obj.SetSignatureTitle2(ConvertHTMLToText(db->Get(0, 0)));

										obj.SetApprovers(GetTimecard_ApprovalChain(timecard_id, psow_id, db));

										obj.SetPosition1(GetValueFromDB("SELECT `value` FROM `cost_center_custom_fields` WHERE  `cost_center_id`=" + quoted(cost_center_id) + " AND `var_name`=\"timecard_position1\";", db));
										obj.SetPosition2(GetValueFromDB("SELECT `value` FROM `cost_center_custom_fields` WHERE  `cost_center_id`=" + quoted(cost_center_id) + " AND `var_name`=\"timecard_position2\";", db));
										obj.SetInitials1(GetValueFromDB("SELECT `value` FROM `cost_center_custom_fields` WHERE  `cost_center_id`=" + quoted(cost_center_id) + " AND `var_name`=\"timecard_signature1\";", db));
										obj.SetInitials2(GetValueFromDB("SELECT `value` FROM `cost_center_custom_fields` WHERE  `cost_center_id`=" + quoted(cost_center_id) + " AND `var_name`=\"timecard_signature2\";", db));
										obj.SetSignRole1(GetValueFromDB("SELECT `value` FROM `cost_center_custom_fields` WHERE  `cost_center_id`=" + quoted(cost_center_id) + " AND `var_name`=\"timecard_role1\";", db));
										obj.SetSignRole2(GetValueFromDB("SELECT `value` FROM `cost_center_custom_fields` WHERE  `cost_center_id`=" + quoted(cost_center_id) + " AND `var_name`=\"timecard_role2\";", db));
										
										if(db->Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `var_name`=\"Department\" AND `contract_psow_id`=\"" + psow_id + "\" AND `type`=\"input\";"))
											obj.SetDepartmentNumber(db->Get(0, "value"));
										else
											MESSAGE_DEBUG("", "", "optional field Department not found for psow.id(" + psow_id + ")");

										if(db->Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `var_name`=\"purchase_order\" AND `contract_psow_id`=\"" + psow_id + "\" AND `type`=\"input\";"))
											obj.SetPurchaseOrder(db->Get(0, "value"));
										else
											MESSAGE_DEBUG("", "", "optional field PurchaseOrder not found for psow.id(" + psow_id + ")");

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
									MESSAGE_ERROR("", "", "Agency title and vat not found for psow.id(" + psow_id + ")");
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

auto C_Invoice_Service_Agency_To_CC::UpdateDBWithInvoiceData(const string timecard_id) -> string
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
					auto	act_id = CreateActInDB("--- full number must be here ---", db, NULL);

					if(act_id)
					{
						// --- new invoice
						invoice_cost_center_service_id = db->InsertQuery(
											"INSERT INTO `invoice_cost_center_service` (`cost_center_id`, `file`, `owner_company_id`, `owner_user_id`, `invoice_id`, `eventTimestamp`) "
											"VALUES (" + 
												quoted(cost_center_id) + "," +
												quoted(archive_folder + "/" + archive_file) + "," +
												quoted(owner_company_id) + "," +
												quoted(user->GetID()) + "," +
												quoted(to_string(act_id)) + "," +
												"UNIX_TIMESTAMP()"
											");");
						if(invoice_cost_center_service_id)
						{
							// --- everything is fine, increase act_number assigned to this cost_center
							error_message = AssignCurrentCompanyActNumberToActID_And_UpdateCompanyActNumber_by_ActID(to_string(act_id), owner_company_id, db, user);
							if(error_message.length())
							{
								MESSAGE_ERROR("", "", error_message);
							}
						}
						else
						{
							error_message = gettext("fail to insert to db");
							MESSAGE_ERROR("", "", error_message);
						}
					}
					else
					{
						error_message = gettext("fail to create act");
						MESSAGE_ERROR("", "", error_message);
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "agency not found where user.id(" + user->GetID() + ") working at");
					error_message = gettext("employer not found");
				}

			}

			// --- don't merge it with previous if()
			if(invoice_cost_center_service_id)
			{
				// --- find appropriate timecard.id in invoicing_vars
				auto	timecard_index = invoicing_vars.GetIndexByTimecardID(timecard_id);

				if(db->InsertQuery( "INSERT INTO `invoice_cost_center_service_details` (`invoice_cost_center_service_id`, `timecard_id`, `cc_amount_pre_tax`, `cc_amount_tax`, `cc_amount_total`, `subc_amount_pre_tax`, `subc_amount_tax`, `subc_amount_total`)"
									"VALUES (" + 
										quoted(to_string(invoice_cost_center_service_id)) + "," +
										quoted(timecard_id) + "," +
										quoted(invoicing_vars.Get("cost_center_price_" + timecard_index).length()	? invoicing_vars.Get("cost_center_price_" + timecard_index) : "0") + "," +
										quoted(invoicing_vars.Get("cost_center_vat_" + timecard_index).length()		? invoicing_vars.Get("cost_center_vat_" + timecard_index) : "0") + "," +
										quoted(invoicing_vars.Get("cost_center_total_" + timecard_index).length()	? invoicing_vars.Get("cost_center_total_" + timecard_index) : "0") + "," +
										quoted(invoicing_vars.Get("timecard_price_" + timecard_index).length()		? invoicing_vars.Get("timecard_price_" + timecard_index) : "0") + "," +
										quoted(invoicing_vars.Get("timecard_vat_" + timecard_index).length()		? invoicing_vars.Get("timecard_vat_" + timecard_index) : "0") + "," +
										quoted(invoicing_vars.Get("timecard_total_" + timecard_index).length()		? invoicing_vars.Get("timecard_total_" + timecard_index) : "0") +
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

C_Invoice_Service_Agency_To_CC::~C_Invoice_Service_Agency_To_CC()
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

ostream& operator<<(ostream& os, const C_Invoice_Service_Agency_To_CC &var)
{
	os << "object C_Invoice_Service_Agency_To_CC [empty for now]";

	return os;
}

