#include "c_print_invoice_service.h"

static void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	char	buffer[512];

    snprintf(buffer, sizeof(buffer) - 1, "error_no=%04X, detail_no=%d\n", (unsigned int) error_no, (int) detail_no);
    MESSAGE_ERROR("", "", "libhpdf: " +  buffer);
    throw std::exception (); /* throw exception on error */
}

C_Print_Invoice_Service::C_Print_Invoice_Service()
{
	MESSAGE_DEBUG("", "", "start");

	__pdf = HPDF_New(error_handler, NULL);

	if(__pdf)
	{
		try
		{
			__pdf_font_name = HPDF_LoadTTFontFromFile (__pdf, (PDF_FONT_INSTALL_DIR + "/ttf/arial.ttf").c_str(), HPDF_TRUE);
		}
		catch(...)
		{
			MESSAGE_ERROR("", "", "HPDF trown exception");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "fail to initialize hpdf library");
	}

	MESSAGE_DEBUG("", "", "start");
}

auto	C_Print_Invoice_Service::SetTimecards(const vector<C_Timecard_To_Print> &param1) -> void
{
	timecards = param1;
	__pdf_line = -1;
}


auto	C_Print_Invoice_Service::BuildInvoiceData() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db)
	{
		if(error_message.empty())
		{
			if(vars.Get("cost_center_id").empty())
			{
				MESSAGE_DEBUG("", "", "build cost center id");

				if(cost_center_id.length())
				{
					vars.Add("cost_center_id", cost_center_id);
				}
				else
				{
					MESSAGE_ERROR("", "", "cost center id is empty");
					error_message = gettext("cost center id is empty");
				}
			}
		}
		if(error_message.empty())
		{
			if(vars.Get("agency_id").empty())
			{
				MESSAGE_DEBUG("", "", "build agency id");

				if(db->Query("SELECT * FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
				{
					vars.Add("agency_id", db->Get(0, "agency_company_id"));

					// --- take same results 
					vars.Add("cost_center_title", ConvertHTMLToText(db->Get(0, "title")));
					vars.Add("cost_center_agreement_start_date", db->Get(0, "start_date"));
					vars.Add("cost_center_agreement_end_date", db->Get(0, "end_date"));
					vars.Add("cost_center_agreement_number", db->Get(0, "number"));
					vars.Add("cost_center_agreement_sign_date", db->Get(0, "sign_date"));
				}
				else
				{
					MESSAGE_ERROR("", "", "agency id not found");
					error_message = gettext("agency id not found");
				}
			}
		}
		if(error_message.empty())
		{
			if(vars.Get("cost_center_title").empty())
			{
				MESSAGE_DEBUG("", "", "build cost center title");

				if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
				{
					vars.Add("cost_center_title", ConvertHTMLToText(db->Get(0, "title")));
				}
				else
				{
					MESSAGE_ERROR("", "", "cost center title not found");
					error_message = gettext("cost center title not found");
				}
			}
		}
		if(error_message.empty())
		{
			if(vars.Get("cost_center_agreement_start_date").empty())
			{
				MESSAGE_DEBUG("", "", "build cost center start date");

				if(db->Query("SELECT `start_date` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
				{
					vars.Add("cost_center_agreement_start_date", ConvertHTMLToText(db->Get(0, "start_date")));
				}
				else
				{
					MESSAGE_ERROR("", "", "cost center start date not found");
					error_message = gettext("cost center start date not found");
				}
			}
		}
		if(error_message.empty())
		{
			if(vars.Get("cost_center_agreement_end_date").empty())
			{
				MESSAGE_DEBUG("", "", "build cost center end date");

				if(db->Query("SELECT `end_date` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
				{
					vars.Add("cost_center_agreement_end_date", ConvertHTMLToText(db->Get(0, "end_date")));
				}
				else
				{
					MESSAGE_ERROR("", "", "cost center end date not found");
					error_message = gettext("cost center end date not found");
				}
			}
		}
		if(error_message.empty())
		{
			if(vars.Get("cost_center_agreement_number").empty())
			{
				MESSAGE_DEBUG("", "", "build cost center number");

				if(db->Query("SELECT `number` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
				{
					vars.Add("cost_center_agreement_number", ConvertHTMLToText(db->Get(0, "number")));
				}
				else
				{
					MESSAGE_ERROR("", "", "cost center number not found");
					error_message = gettext("cost center number not found");
				}
			}
		}
		if(error_message.empty())
		{
			if(vars.Get("cost_center_agreement_sign_date").empty())
			{
				MESSAGE_DEBUG("", "", "build cost center sign date");

				if(db->Query("SELECT `sign_date` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
				{
					vars.Add("cost_center_agreement_sign_date", ConvertHTMLToText(db->Get(0, "sign_date")));
				}
				else
				{
					MESSAGE_ERROR("", "", "cost center sign date not found");
					error_message = gettext("cost center sign date not found");
				}
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db is not initialized");
		error_message = gettext("db is not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Print_Invoice_Service::PrintInvoiceAsXLS() -> string
{
	auto	error_message = ""s;
	auto	f_name = GetFilename(); 

	MESSAGE_DEBUG("", "", "start");

	// locale cp1251_locale("ru_RU.UTF-8");
	// std::locale::global(cp1251_locale);

	if((error_message = BuildInvoiceData()).empty())
	{
		// --- after instantiation libxl-object locale switch over to wide-byte string representation
		// --- gettext is not working properly with multibyte strings, therefore all gettext constants
		// --- must be defined before libxl-instantiation

		libxl::Book* book = xlCreateBook();
		if(book)
		{
			libxl::Sheet* sheet = book->addSheet(L"Sheet1");
			if(sheet)
			{
				auto			row_counter = 6;
				auto			column_counter = 0;

				auto			font_big			= book->addFont();
				auto			font_bold			= book->addFont();
				auto			format_big			= book->addFormat();
				auto			format_bold			= book->addFormat();
				auto			format_number_d2	= book->addFormat();


				// --- print proerties
				sheet->setPaper(libxl::PAPER_A4);
				sheet->setLandscape(false);
				// sheet->setPrintZoom(50);

				font_big->setSize(24);
				font_bold->setBold();

				format_big->setFont(font_big);
				format_bold->setFont(font_bold);
				format_number_d2->setNumFormat(libxl::NUMFORMAT_NUMBER_D2);

				// --- set columns width
				sheet->setCol(1, 1, 45);
				sheet->setCol(2, 31 + 2, 5);

				sheet->writeStr(2, 2, L"spelled_title");
				sheet->writeStr(3, 1, L"spelled_psow");
				sheet->writeStr(4, 1, L"spelled_projectid");

				// --- table header
				sheet->writeStr(5, 2, L"put header here", format_bold);

				// --- table body
				for(auto &timecard: timecards)
				{
					sheet->writeStr(row_counter, 1, (L"timecard " + to_wstring(row_counter)).c_str());

					row_counter++;
				}

				// --- table summary
				sheet->writeStr(row_counter, 1, L"Итого:", format_bold);
				row_counter++;

				{
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"spelled_totalhours");
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, 98742.34987, format_number_d2);

					row_counter++;
				}
			}

			if(book->save(multibyte_to_wide(GetFilename()).c_str())) {}
			else
			{
				MESSAGE_ERROR("", "", "can't save " + filename + ".xls")
			}
			book->release();
		} 
	}
	else
	{
		MESSAGE_ERROR("", "", "print XLS-invoice to cost_center failed");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}





ostream& operator<<(ostream& os, const C_Print_Invoice_Service &var)
{
	os << "object C_Print_Invoice_Service [empty for now]";

	return os;
}

