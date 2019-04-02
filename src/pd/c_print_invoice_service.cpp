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

auto	C_Print_Invoice_Service::PrintInvoiceAsXLS() -> string
{
	auto	error_message = ""s;
	auto	f_name = GetFilename(); 

	MESSAGE_DEBUG("", "", "start");

	// locale cp1251_locale("ru_RU.UTF-8");
	// std::locale::global(cp1251_locale);

	if(vars)
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
				auto			row_counter = 1;
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
				sheet->setCol(0, 0, 1);
				sheet->setCol(1, 1, 5);
				sheet->setCol(2, 2, 31 + 2);
				// sheet->setCol(2, 31 + 2, 5);

				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("agency_bank_title")).c_str());
				sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("Bank Identifier")).c_str());
				sheet->writeStr(row_counter, 7, multibyte_to_wide(vars->Get("agency_bank_bik")).c_str());

				++row_counter;
				sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("Account")).c_str());
				sheet->writeStr(row_counter, 7, multibyte_to_wide(vars->Get("agency_bank_account")).c_str());

				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Destination bank")).c_str());


				// --- table header
				++row_counter;
				sheet->writeStr(row_counter, 2, L"put header here", format_bold);

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
		error_message = gettext("fail to initialize variable set");
		MESSAGE_ERROR("", "", "set variables before generating documanets. Invoke SetVariableSet before printing.");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}





ostream& operator<<(ostream& os, const C_Print_Invoice_Service &var)
{
	os << "object C_Print_Invoice_Service [empty for now]";

	return os;
}

