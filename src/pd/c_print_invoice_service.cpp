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

auto	C_Print_Invoice_Service::Restart() -> void
{
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

				auto			font_big			= book->addFont();
				auto			font_bold			= book->addFont();
				auto			format_big			= book->addFormat();
				auto			format_bold			= book->addFormat();
				auto			format_number_d2	= book->addFormat();


				// --- print proerties
				sheet->setPaper(libxl::PAPER_A4);
				sheet->setLandscape(false);
				// sheet->setPrintZoom(50);

				font_big->setSize(12);
				font_big->setBold();
				font_bold->setBold();

				format_big->setFont(font_big);
				format_bold->setFont(font_bold);
				format_number_d2->setNumFormat(libxl::NUMFORMAT_NUMBER_D2);

				// --- set columns width
				sheet->setCol(0, 0, 1);
				sheet->setCol(1, 1, 5);
				sheet->setCol(8, 9, 11);  // --- price and total columns
//				sheet->setCol(2, 2, 31 + 2);
				// sheet->setCol(2, 31 + 2, 5);

				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("agency_bank_title")).c_str());
				sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("Bank Identifier")).c_str());
				sheet->writeStr(row_counter, 7, multibyte_to_wide(vars->Get("agency_bank_bik")).c_str());

				++row_counter;
				sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("Account")).c_str());
				sheet->writeStr(row_counter, 7, multibyte_to_wide(vars->Get("agency_bank_account")).c_str());

				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Destination bank")).c_str());

				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("TIN")).c_str());
				sheet->writeStr(row_counter, 2, multibyte_to_wide(vars->Get("agency_tin")).c_str());
				sheet->writeStr(row_counter, 4, multibyte_to_wide(vars->Get("KPP") + " " + vars->Get("agency_kpp")).c_str());
				sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("Account")).c_str());
				sheet->writeStr(row_counter, 7, multibyte_to_wide(vars->Get("agency_account")).c_str());

				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("agency_name")).c_str());

				++row_counter;
				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Recipient")).c_str());

				++row_counter;
				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Invoice") + " " + vars->Get("invoice_agreement")).c_str(), format_big);

				++row_counter;
				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Supplier")).c_str());
				sheet->writeStr(row_counter, 3, multibyte_to_wide(vars->Get("agency_name") + ", " + vars->Get("TIN") + " " + vars->Get("agency_tin") + ", " + vars->Get("KPP") + " " + vars->Get("agency_kpp") + ", " + vars->Get("agency_legal_geo_zip") + " " + vars->Get("agency_legal_locality_title") + ", " + vars->Get("agency_legal_address")).c_str());

				++row_counter;
				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Customer")).c_str());
				sheet->writeStr(row_counter, 3, multibyte_to_wide(vars->Get("cost_center_name") + ", " + vars->Get("TIN") + " " + vars->Get("cost_center_tin") + ", " + vars->Get("KPP") + " " + vars->Get("cost_center_kpp") + ", " + vars->Get("cost_center_legal_geo_zip") + " " + vars->Get("cost_center_legal_locality_title") + ", " + vars->Get("cost_center_legal_address")).c_str());

				++row_counter;
				++row_counter;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Basis")).c_str());
				sheet->writeStr(row_counter, 3, multibyte_to_wide(vars->Get("act_basis")).c_str());

				// --- table header
				++row_counter;
				++row_counter;
				++row_counter;
				sheet->writeStr(row_counter, 1, L"№", format_bold);
				sheet->writeStr(row_counter, 2, multibyte_to_wide(vars->Get("Goods")).c_str(), format_bold);
				sheet->writeStr(row_counter, 5, multibyte_to_wide(vars->Get("Quantity short")).c_str(), format_bold);
				sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("Items short")).c_str(), format_bold);
				sheet->writeStr(row_counter, 7, multibyte_to_wide(vars->Get("Price")).c_str(), format_bold);
				sheet->writeStr(row_counter, 8, multibyte_to_wide(vars->Get("Total")).c_str(), format_bold);

				// --- table body
				for(auto i = 1; vars->Get("timecard_index_" + to_string(i)).length(); ++i)
				{
					++row_counter;
					sheet->writeNum(row_counter, 1, stod_noexcept(vars->Get("timecard_index_" + to_string(i))));
					sheet->writeStr(row_counter, 2, multibyte_to_wide("service index ").c_str());
					sheet->writeNum(row_counter, 5, stod_noexcept(vars->Get("timecard_quantity_" + to_string(i))));
					sheet->writeStr(row_counter, 6, multibyte_to_wide(vars->Get("timecard_item_" + to_string(i))).c_str());
					sheet->writeNum(row_counter, 7, stod_noexcept(vars->Get("timecard_price_" + to_string(i))));
					sheet->writeNum(row_counter, 8, stod_noexcept(vars->Get("timecard_total_" + to_string(i))));
				}

				// --- table summary
				row_counter++;
				sheet->writeStr(row_counter, 1, multibyte_to_wide(vars->Get("Total")).c_str(), format_bold);

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

