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
			error_message = utf8_to_cp1251(gettext("fail to create temp directory"));
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
				error_message = utf8_to_cp1251(gettext("fail to craft timecard obj"));
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
				filename_xls = temp_dir + "timecard_" + to_string(i) + ".xls";
				filename_pdf = temp_dir + "timecard_" + to_string(i) + ".pdf";
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
		temp_dir = TEMP_DIRECTORY_PREFIX + GetRandom(15);
	} while(isDirExists(temp_dir));

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
									obj.SetSignatureTitle1(db->Get(0, "name"));
									
									if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=\"" + cost_center_id + "\";"))
									{
										obj.SetSignatureTitle2(db->Get(0, "title"));
										
										if(db->Query("SELECT `value` FROM `contract_psow_custom_fields` WHERE `var_name`=\"Department\" AND `contract_psow_id`=\"" + psow_id + "\" AND `type`=\"input\";"))
											obj.SetProjectNumber(db->Get(0, "value"));
										else
											MESSAGE_DEBUG("", "", "optional field Department not found for psow.id(" + psow_id + ")");
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

/*
auto C_Invoice_Service::GeneratePDF() -> string
{
	HPDF_Doc  pdf;

	pdf = HPDF_NewEx (error_handler, NULL, NULL, 0, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
    }
    HPDF_Free (pdf);

    return "ok";
}

auto C_Invoice_Service::GenerateXLSTimecard(string timecard_id) -> string
{
	string		error_message = "";
	
	MESSAGE_DEBUG("", "", "start (timecard_id: " + timecard_id + ")");

	auto f_name = temp_dir + "example" + GetRandom(3) + ".xls"; 
    libxl::Book* book = xlCreateBook();
    if(book)
    {    
        libxl::Sheet* sheet = book->addSheet("Sheet1");
        if(sheet)
        {
            sheet->writeStr(2, 1, "Hello, World !");
            sheet->writeNum(3, 1, 1000);
            
            libxl::Format* dateFormat = book->addFormat();
            dateFormat->setNumFormat(libxl::NUMFORMAT_DATE);
            sheet->writeNum(4, 1, book->datePack(2008, 4, 22), dateFormat);
            
            sheet->setCol(1, 1, 12);
        }

        if(book->save(f_name.c_str())) {
    	}
        book->release();
    } 

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}
*/

C_Invoice_Service::~C_Invoice_Service()
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

ostream& operator<<(ostream& os, const C_Invoice_Service &var)
{
	os << "object C_Invoice_Service [empty for now]";

	return os;
}

