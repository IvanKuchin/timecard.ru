#include "c_invoice_bt_agency_to_cc.h"

auto C_Invoice_BT_Agency_To_CC::GenerateDocumentArchive() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto									error_message = ""s;

	C_Print_BT_To_CC						bt_printer;

	C_Print_Invoice_Agency					__invoice_agency;
	C_Print_Invoice_Docs_Base				*invoice_printer = &__invoice_agency;

	C_Print_Act_Agency						__act_agency;
	C_Print_Invoice_Docs_Base				*act_printer = &__act_agency;

	C_Print_VAT_Agency						__vat_obj;
	C_Print_VAT_Base						*vat_printer = &__vat_obj;

	C_Print_1C_CostCenter_Selling			__cc_1c_main_obj1;
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
		for(auto &bt_id: bt_id_list)
		{
			auto	temp_obj = CreateBTObj(bt_id);

			if((error_message = temp_obj.isValid()).empty())
			{
				bt_obj_list.push_back(temp_obj);
			}
			else
			{
				MESSAGE_ERROR("", "", "bt_obj(" + bt_id + ") is broken don't add it to the list");
				break;
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "bt won't be built due to previous error");
	}

	// --- generate variable for invoicing
	if(error_message.empty())
	{
		invoicing_vars.SetDB(db);
		invoicing_vars.SetUser(user);
		invoicing_vars.SetCostCenterID(cost_center_id);
		invoicing_vars.SetBTs(bt_obj_list);

		error_message = invoicing_vars.GenerateBTVariableSet_AgencyToCC();
	}
	else
	{
		MESSAGE_ERROR("", "", "invoicing_vars won't be generated due to previous error");
	}

	// --- bt generator
	if(error_message.empty())
	{
		auto	i = 0;

		// --- print bt to XLS / PDF
		for(auto &bt: bt_obj_list)
		{
			auto		filename_xls = ""s;
			auto		filename_pdf = ""s;

			do
			{
				++i;
				filename_xls = temp_dir_bt + "bt_" + to_string(i) + ".xls";
				filename_pdf = temp_dir_bt + "bt_" + to_string(i) + ".pdf";
			} while(isFileExists(filename_xls) || isFileExists(filename_pdf));

			bt_printer.SetBT(bt);
			bt_printer.SetVariableSet(&invoicing_vars);

			bt_printer.SetFilename(filename_xls);
			error_message = bt_printer.PrintAsXLS();
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", "fail to build bt_" + to_string(i) + ".xls");
				break;
			}


			bt_printer.SetFilename(filename_pdf);
			error_message = bt_printer.PrintAsPDF();
			if(error_message.length())
			{
				MESSAGE_ERROR("", "", "fail to build bt_" + to_string(i) + ".pdf");
				break;
			}

		}
	}
	else
	{
		MESSAGE_ERROR("", "", "BTs won't be written to files due to previous error");
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
			cc_service_filename_1c			= temp_dir_1c + "costcenter_bt_" + to_string(i) + ".xml";
			subc_payment_filename_1c		= temp_dir_1c + "subcontractor_payment_" + to_string(i) + ".xml";
			subc_payment_order_filename_1c	= temp_dir_1c + "subcontractor_payment_order_" + to_string(i) + ".xml";
		} while(
				isFileExists(invoice_filename_xls)		|| isFileExists(invoice_filename_pdf) ||
				isFileExists(act_filename_xls)			|| isFileExists(act_filename_pdf) ||
				isFileExists(vat_filename_xls)			|| isFileExists(vat_filename_pdf) ||
				isFileExists(cc_service_filename_1c)	|| 
				isFileExists(subc_payment_filename_1c)	|| isFileExists(subc_payment_order_filename_1c)
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

			if(error_message.empty()) 
			{
				subc_1c_payment_printer->SetFilename(subc_payment_filename_1c);
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

			if(error_message.empty()) 
			{
				subc_1c_payment_order_printer->SetFilename(subc_payment_order_filename_1c);
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

	if(error_message.empty())
	{
		// --- important to keep it scoped
		// --- archive closed in destructor
		{
			c_archive	ar;

			ar.SetFilename(INVOICES_CC_DIRECTORY + archive_folder + "/" + archive_file);
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

auto C_Invoice_BT_Agency_To_CC::CreateTempDirectory() -> bool
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
		temp_dir_bt = temp_dir + "bt/";
		temp_dir_cost_center_invoices = temp_dir + "invoices/";
		temp_dir_1c = temp_dir + "1c/";

		if(CreateDir(temp_dir_bt))
		{
			if(CreateDir(temp_dir_cost_center_invoices))
			{
				if(CreateDir(temp_dir_1c))
				{
					result = true;
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
			MESSAGE_ERROR("", "", "fail to create " + temp_dir_bt);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to create " + temp_dir);
	}

	return result;
}

auto C_Invoice_BT_Agency_To_CC::CreateBTObj(string bt_id) -> C_BT_To_Print
{
	C_BT_To_Print		obj;

	MESSAGE_DEBUG("", "", "start (bt_id: " + bt_id + ")");

	if(db)
	{
		if(bt_id.length())
		{
			if(cost_center_id.length())
			{
				string	psow_id = GetPSoWIDByBTIDAndCostCenterID(bt_id, cost_center_id, db, user);

				if(psow_id.length())
				{
					// string	sow_id = GetSoWIDByTimecardID(bt_id, db, user);
					string	sow_id = "fake uncomment prev line";

					if(sow_id.length())
					{
						if(db->Query("SELECT * FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
						{
							obj.SetID(bt_id);
							obj.SetDateStart(db->Get(0, "date_start"));
							obj.SetDateFinish(db->Get(0, "date_end"));
							obj.SetLocation(db->Get(0, "place"));

							if(db->Query("SELECT * FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\";"))
							{
								obj.SetAgreementNumber(db->Get(0, "number"));
								obj.SetDateSign(db->Get(0, "sign_date"));
								obj.SetMarkupType(db->Get(0, "bt_markup_type"));
								obj.SetMarkupDB(db->Get(0, "bt_markup"));
								// obj.SetDayrate(db->Get(0, "day_rate"));

								if(db->Query("SELECT `name`,`vat` FROM `company` WHERE `id`=("
												"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
													"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\""
												")"
											");"))
								{
									obj.SetSignatureTitle1(ConvertHTMLToText(db->Get(0, "name")));
									obj.SetSupplierVAT(ConvertHTMLToText(db->Get(0, "vat")));

									if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=\"" + cost_center_id + "\";"))
									{
										obj.SetSignatureTitle2(ConvertHTMLToText(db->Get(0, "title")));
										
										if(db->Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `var_name`=\"Department\" AND `contract_psow_id`=\"" + psow_id + "\" AND `type`=\"input\";"))
											obj.SetProjectNumber(db->Get(0, "value"));
										else
											MESSAGE_DEBUG("", "", "optional field Department not found for psow.id(" + psow_id + ")");

										{
											if(EnrichObjWithExpenseLines(bt_id, &obj))
											{
												// --- all good
											}
											else
											{
												MESSAGE_ERROR("", "", "fail to add expanse lines to object");
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
							MESSAGE_ERROR("", "", "bt_id(" + bt_id + ") not found");
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
			MESSAGE_ERROR("", "", "bt_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db not initialized");
	}

	MESSAGE_DEBUG("", "", "finish");

	return obj;
}

auto C_Invoice_BT_Agency_To_CC::EnrichObjWithExpenseLines(string bt_id, C_BT_To_Print *obj) -> bool
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(user->GetType() == "agency")
		{
			if(db)
			{
				auto affected = db->Query(
"SELECT `bt_expenses`.`date` as `date`, `bt_expenses`.`price_foreign` as `price_foreign`, `bt_expenses`.`price_domestic` as `price_domestic`, `bt_expenses`.`currency_nominal` as `currency_nominal`, `bt_expenses`.`currency_value` as `currency_value`, `bt_expenses`.`currency_name` as `currency_name`, `bt_expense_templates`.`title` as `title`,  `bt_expense_templates`.`taxable` as `taxable`"
"FROM `bt_expenses` "
"INNER JOIN `bt_expense_templates` ON `bt_expense_templates`.`id`=`bt_expenses`.`bt_expense_template_id` "
"WHERE "
	"("
		"`bt_expenses`.`bt_id`=\"" + bt_id + "\""
	")"
				);
				if(affected)
				{
					for(int i = 0; i < affected; ++i)
					{
						obj->AddExpenseLine(db->Get(i, "date"), db->Get(i, "title"), db->Get(i, "price_domestic"), db->Get(i, "price_foreign"), db->Get(i, "currency_nominal"), db->Get(i, "currency_value"), db->Get(i, "currency_name"), db->Get(i, "taxable"));
					}
					result = true;
				}
				else
				{
					MESSAGE_ERROR("", "", "it is expected that valid entries will be here, otherwise cost_center.id(" + cost_center_id + ") and bt.id(" + bt_id + ") should not appear in GUI");
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

	MESSAGE_DEBUG("", "", "finish (result.size is " + to_string(result) + ")");

	return result;
}


auto C_Invoice_BT_Agency_To_CC::UpdateDBWithInvoiceData(const string bt_id) -> string
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
						db->Query("UPDATE `company` SET `act_number`=`act_number`+1 WHERE `id`=\"" + owner_company_id + "\";");
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

C_Invoice_BT_Agency_To_CC::~C_Invoice_BT_Agency_To_CC()
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

ostream& operator<<(ostream& os, const C_Invoice_BT_Agency_To_CC &var)
{
	os << "object C_Invoice_BT_Agency_To_CC [empty for now]";

	return os;
}

