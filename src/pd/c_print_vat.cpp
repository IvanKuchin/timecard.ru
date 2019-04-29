#include "c_print_vat.h"


auto	C_Print_VAT_Base::GetSupplierCompanyDetails() -> string
{
	return GetSupplierName() + ", " + vars->Get("TIN") + " " + GetSupplierTIN() + ", " + vars->Get("KPP") + " " + GetSupplierKPP() + ", " + GetSupplierLegalZIP() + " " + GetSupplierLegalLocality() + ", " + GetSupplierLegalAddress();
}

auto	C_Print_VAT_Base::GetCustomerCompanyDetails() -> string
{
	return GetCustomerName() + ", " + vars->Get("TIN") + " " + GetCustomerTIN() + ", " + vars->Get("KPP") + " " + GetCustomerKPP() + ", " + GetCustomerLegalZIP() + " " + GetCustomerLegalLocality() + ", " + GetCustomerLegalAddress();
}

auto	C_Print_VAT_Base::SpellPrice() -> string
{
	MESSAGE_DEBUG("", "", "start");

	C_Price_Spelling	price(stod_noexcept(GetTableTotal()));
	auto				result = vars->Get("Total payment") + ": " + price.Spelling();

	MESSAGE_DEBUG("", "", "finish");

	return result;
}


// --- XLS part
auto C_Print_VAT_Base::__PrintXLSSignature() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	font_bold = __book->addFont();
	auto	font_small = __book->addFont();
	auto	format_bold = __book->addFormat();
	auto	format_bold_right = __book->addFormat();
	auto	format_right = __book->addFormat();
	auto	format_right_small = __book->addFormat();
	auto	format_underline = __book->addFormat();

	font_small->setSize(7);
	font_bold->setBold();

	format_bold->setFont(font_bold);
	format_right->setAlignH(libxl::ALIGNH_RIGHT);
	format_bold_right->setFont(font_bold);
	format_bold_right->setAlignH(libxl::ALIGNH_RIGHT);
	format_right_small->setFont(font_small);
	format_right_small->setAlignH(libxl::ALIGNH_RIGHT);
	format_underline->setBorderBottom(libxl::BORDERSTYLE_THIN);

	++__row_counter;
	__sheet->writeStr(__row_counter, 2, multibyte_to_wide(GetSignatureTitle1()).c_str(), format_bold);
	__sheet->writeStr(__row_counter, 8, multibyte_to_wide(GetSignatureTitle2()).c_str(), format_bold_right);
	__sheet->writeStr(__row_counter, 4, L"", format_underline);
	__sheet->writeStr(__row_counter, 5, L"", format_underline);
	if(GetSignatureTitle2().length())
	{
		__sheet->writeStr(__row_counter,  9, L"", format_underline);
		__sheet->writeStr(__row_counter, 10, L"", format_underline);
	}
	++__row_counter;
	__sheet->writeStr(__row_counter, 5, multibyte_to_wide(GetSignatureName1()).c_str(), format_right);
	if(GetSignatureTitle2().length())
		__sheet->writeStr(__row_counter, 10, multibyte_to_wide(GetSignatureName2()).c_str(), format_right);
	++__row_counter;
	__sheet->writeStr(__row_counter, 5, multibyte_to_wide(GetSignatureInfo1()).c_str(), format_right_small);
	if(GetSignatureTitle2().length())
		__sheet->writeStr(__row_counter, 10, multibyte_to_wide(GetSignatureInfo2()).c_str(), format_right_small);


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::PrintAsXLS() -> string
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

		__book = xlCreateBook();
		if(__book)
		{
			__sheet = __book->addSheet(L"Sheet1");
			if(__sheet)
			{

				auto			font_big					= __book->addFont();
				auto			font_bold					= __book->addFont();
				auto			font_small					= __book->addFont();
				auto			format_big					= __book->addFormat();
				auto			format_bold					= __book->addFormat();
				auto			format_bold_right			= __book->addFormat();
				auto			format_small_border			= __book->addFormat();
				auto			format_number_d2			= __book->addFormat();
				auto			format_number_d2_bold_right	= __book->addFormat();
				auto			format_table_left			= __book->addFormat();
				auto			format_table_right			= __book->addFormat();
				auto			format_table_center			= __book->addFormat();
				auto			format_top_left				= __book->addFormat();
				auto			format_top_left_bold		= __book->addFormat();

				total_table_items = 0;

				__row_counter = 1;

				// --- print proerties
				__sheet->setPaper(libxl::PAPER_A4);
				__sheet->setLandscape(true);
				// __sheet->setPrintZoom(50);

				font_small->setSize(6);
				font_big->setSize(12);
				font_big->setBold();
				font_bold->setBold();

				format_big->setFont(font_big);
				format_bold->setFont(font_bold);
				format_bold_right->setFont(font_bold);
				format_bold_right->setAlignH(libxl::ALIGNH_RIGHT);
				format_small_border->setFont(font_small);
				format_small_border->setBorder(libxl::BORDERSTYLE_THIN);
				format_small_border->setAlignH(libxl::ALIGNH_CENTER);
				format_small_border->setWrap(true);
				format_number_d2->setNumFormat(libxl::NUMFORMAT_NUMBER_D2);
				format_number_d2_bold_right->setNumFormat(libxl::NUMFORMAT_NUMBER_D2);
				format_number_d2_bold_right->setFont(font_bold);
				format_number_d2_bold_right->setAlignH(libxl::ALIGNH_RIGHT);
				format_table_left->setWrap(true);
				format_table_left->setAlignV(libxl::ALIGNV_TOP);
				format_table_left->setAlignH(libxl::ALIGNH_LEFT);
				format_table_left->setBorder(libxl::BORDERSTYLE_THIN);
				format_table_right->setWrap(true);
				format_table_right->setAlignV(libxl::ALIGNV_TOP);
				format_table_right->setAlignH(libxl::ALIGNH_RIGHT);
				format_table_right->setBorder(libxl::BORDERSTYLE_THIN);
				format_table_center->setWrap(true);
				format_table_center->setAlignV(libxl::ALIGNV_TOP);
				format_table_center->setAlignH(libxl::ALIGNH_CENTER);
				format_table_center->setBorder(libxl::BORDERSTYLE_THIN);
				format_top_left->setWrap(true);
				format_top_left->setAlignV(libxl::ALIGNV_TOP);
				format_top_left->setAlignH(libxl::ALIGNH_LEFT);
				format_top_left_bold->setFont(font_bold);
				format_top_left_bold->setWrap(true);
				format_top_left_bold->setAlignV(libxl::ALIGNV_TOP);
				format_top_left_bold->setAlignH(libxl::ALIGNH_LEFT);


				// --- set columns width
				__sheet->setCol(0, 0, 3);
				__sheet->setCol(1, 1, 44);
				__sheet->setCol(2, 5, 6);
				__sheet->setCol(6, 6, 10);
				__sheet->setCol(7, 7, 10);
				__sheet->setCol(8, 8, 6);
				__sheet->setCol(9, 9, 10);
				__sheet->setCol(10, 10, 10);

				PrintXLSHeader();

				__sheet->writeStr(__row_counter, 0, multibyte_to_wide(GetDocumentTitle()).c_str(), format_big);

				++__row_counter;
				__sheet->writeStr(__row_counter, 0, multibyte_to_wide(vars->Get("Seller") + ": " + GetSupplierCompanyDetails()).c_str());

				++__row_counter;
				__sheet->writeStr(__row_counter, 0, multibyte_to_wide(vars->Get("Buyer") + ": " + GetCustomerCompanyDetails()).c_str());

				++__row_counter;
				__sheet->writeStr(__row_counter, 0, multibyte_to_wide(vars->Get("VAT comment1")).c_str());

				// --- table header
				++__row_counter;
				__sheet->writeStr(__row_counter,  0, L"№", format_small_border);
				__sheet->writeStr(__row_counter,  1, multibyte_to_wide(vars->Get("Goods")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  2, multibyte_to_wide(vars->Get("Measure unit (code)")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  3, multibyte_to_wide(vars->Get("Measure unit (name)")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  4, multibyte_to_wide(vars->Get("Quantity short")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  5, multibyte_to_wide(vars->Get("Price")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  6, multibyte_to_wide(vars->Get("Total (tax free)")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  7, multibyte_to_wide(vars->Get("Excise")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  8, multibyte_to_wide(vars->Get("Tax rate")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter,  9, multibyte_to_wide(vars->Get("Tax amount")).c_str(), format_small_border);
				__sheet->writeStr(__row_counter, 10, multibyte_to_wide(vars->Get("Total")).c_str(), format_small_border);

				// --- table body
				for(auto i = 1; isTableRowExists(i); ++i)
				{
					auto	description = GetTableRowDescription(i);

					++__row_counter;

					__sheet->writeNum(__row_counter,  0, stod_noexcept(GetTableRowIndex(i)), format_table_center);
					__sheet->writeStr(__row_counter,  1, multibyte_to_wide(description).c_str(), format_table_left);
					__sheet->writeNum(__row_counter,  2, 796, format_table_left);
					__sheet->writeStr(__row_counter,  3, multibyte_to_wide(GetTableRowItem(i)).c_str(), format_table_left);
					__sheet->writeNum(__row_counter,  4, stod_noexcept(GetTableRowQuantity(i)), format_table_left);
					__sheet->writeStr(__row_counter,  5, L"", format_table_left);
					__sheet->writeNum(__row_counter,  6, stod_noexcept(GetTableRowPrice(i)), format_table_right);
					__sheet->writeStr(__row_counter,  7, multibyte_to_wide(vars->Get("no excise")).c_str(), format_table_right);
					__sheet->writeStr(__row_counter,  8, multibyte_to_wide(GetSupplierVATSpellingShort()).c_str(), format_table_right);
					__sheet->writeNum(__row_counter,  9, stod_noexcept(GetTableRowVAT(i)), format_table_right);
					__sheet->writeNum(__row_counter, 10, stod_noexcept(GetTableRowTotal(i)), format_table_right);

					++total_table_items;
				}

				++__row_counter;
				__sheet->writeStr(__row_counter,  0, L"", format_table_left);
				__sheet->writeStr(__row_counter,  1, multibyte_to_wide(vars->Get("Total payment") + ":").c_str(), format_table_left);
				__sheet->writeStr(__row_counter,  2, L"", format_table_left);
				__sheet->writeStr(__row_counter,  3, L"", format_table_left);
				__sheet->writeStr(__row_counter,  4, L"", format_table_left);
				__sheet->writeStr(__row_counter,  5, L"", format_table_left);
				__sheet->writeNum(__row_counter,  6, stod_noexcept(GetTableSum()), format_table_right);
				__sheet->writeStr(__row_counter,  7, L"", format_table_left);
				__sheet->writeStr(__row_counter,  8, L"", format_table_left);
				__sheet->writeNum(__row_counter,  9, stod_noexcept(GetTableVAT()), format_table_right);
				__sheet->writeNum(__row_counter, 10, stod_noexcept(GetTableTotal()), format_table_right);

				++__row_counter;
				__PrintXLSSignature();

			}

			if(__book->save(multibyte_to_wide(GetFilename()).c_str())) {}
			else
			{
				MESSAGE_ERROR("", "", "can't save " + filename + ".xls")
			}
			__book->release();
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




// --- PDF part
auto C_Print_VAT_Base::__HPDF_PrintSignature() -> string
{

	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureTitle1()), 0, 20, HPDF_TALIGN_LEFT, BOLD_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureTitle2()), 60, 60+20, HPDF_TALIGN_LEFT, BOLD_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		if(GetSignatureTitle1().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(10, 40)).length())
					{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
			}
		}
		if(GetSignatureTitle2().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(70, 100)).length())
					{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
			}
		}

		if(GetSignatureTitle1().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureName1()), 0, 40, HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(GetSignatureTitle2().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureName2()), 60, 60+40, HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		if(GetSignatureTitle1().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureInfo1()), 0, 40, HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size - 2, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(GetSignatureTitle2().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureInfo2()), 60, 60+40, HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size - 2, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
	}
	catch(...)
	{
		error_message = "hpdf: "s + gettext("fail to print header table");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawHeader() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = PrintPDFHeader()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print header"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetDocumentTitle()), 0, 100, HPDF_TALIGN_LEFT, BOLD_FONT, __pdf_font_size + 2, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Seller") + ": " + GetSupplierCompanyDetails()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Buyer") + ": " + GetCustomerCompanyDetails()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT comment1")), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print title");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawTable() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		if(error_message.empty())
		{
			pdf_obj.AddColumn(3);
			pdf_obj.AddColumn(45);
			pdf_obj.AddColumn(3);
			pdf_obj.AddColumn(3);
			pdf_obj.AddColumn(3);
			pdf_obj.AddColumn(3);
			pdf_obj.AddColumn(10);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(10);
			pdf_obj.AddColumn(10);

			if((error_message = pdf_obj.__HPDF_StartTable()).length())
			{
				MESSAGE_ERROR("", "", "fail to start table");
			}
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(0, utf8_to_cp1251("№"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title index line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(vars->Get("Goods")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title description line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(2, utf8_to_cp1251(vars->Get("Measure unit (code)")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title measure unit line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(3, utf8_to_cp1251(vars->Get("Measure unit (name)")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title measure unit line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(4, utf8_to_cp1251(vars->Get("Quantity short")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title quantity line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(5, utf8_to_cp1251(vars->Get("Price")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title price line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(6, utf8_to_cp1251(vars->Get("Total (tax free)")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title Total (tax free) line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(7, utf8_to_cp1251(vars->Get("Excise")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title excise line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(8, utf8_to_cp1251(vars->Get("Tax rate")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title tax rate line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(9, utf8_to_cp1251(vars->Get("Tax amount")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title tax amount line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(10, utf8_to_cp1251(vars->Get("Total")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveTableLineDown(2)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}

		if(error_message.empty())
		{
			total_table_items = 0;
			for(auto i = 1; isTableRowExists(i); ++i)
			{
				auto	max_lines = pdf_obj.__HPDF_GetNumberOfLinesInTable(1, utf8_to_cp1251(GetTableRowDescription(i))	, NORMAL_FONT, __pdf_font_size);

				if((error_message = pdf_obj.__HPDF_PrintTextTableCell(0, utf8_to_cp1251(GetTableRowIndex(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
				{
					if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(GetTableRowDescription(i)), HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).empty())
					{
						if((error_message = pdf_obj.__HPDF_PrintTextTableCell(2, utf8_to_cp1251("796"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
						{
							if((error_message = pdf_obj.__HPDF_PrintTextTableCell(3, utf8_to_cp1251(GetTableRowItem(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
							{
								if((error_message = pdf_obj.__HPDF_PrintTextTableCell(4, utf8_to_cp1251(GetTableRowQuantity(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
								{
									if((error_message = pdf_obj.__HPDF_PrintTextTableCell(6, utf8_to_cp1251(GetTableRowPrice(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).empty())
									{
										if((error_message = pdf_obj.__HPDF_PrintTextTableCell(7, utf8_to_cp1251(vars->Get("no excise")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).empty())
										{
											if((error_message = pdf_obj.__HPDF_PrintTextTableCell(8, utf8_to_cp1251(GetSupplierVATSpellingShort()), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
											{

												if((error_message = pdf_obj.__HPDF_PrintTextTableCell(9, utf8_to_cp1251(GetTableRowVAT(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).empty())
												{
													if((error_message = pdf_obj.__HPDF_PrintTextTableCell(10, utf8_to_cp1251(GetTableRowTotal(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).empty())
													{
														if((error_message = pdf_obj.__HPDF_MoveTableLineDown(max_lines)).empty())
														{

														}
														else
														{
															MESSAGE_ERROR("", "", "fail to move table line down " + to_string(max_lines) + " line(s)");
														}
													}
													else
													{
														MESSAGE_ERROR("", "", "fail to write table total (" + to_string(i) + ") line");
													}
												}
												else
												{
													MESSAGE_ERROR("", "", "fail to write table tax (" + to_string(i) + ") line");
												}



											}
											else
											{
												MESSAGE_ERROR("", "", "fail to write vat percentage (" + to_string(i) + ") line");
											}
										}
										else
										{
											MESSAGE_ERROR("", "", "fail to write table excise (" + to_string(i) + ") line");
										}
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to write table price (" + to_string(i) + ") line");
									}


								}
								else
								{
									MESSAGE_ERROR("", "", "fail to write table description (" + to_string(i) + ") line");
								}
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to write table item (" + to_string(i) + ") line");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "fail to write table measure unit (" + to_string(i) + ") line");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to write table description (" + to_string(i) + ") line");
					}

				}
				else
				{
					MESSAGE_ERROR("", "", "fail to write table index (" + to_string(i) + ") line");
				}

				if(error_message.length()) break;

				++total_table_items;
			}
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(vars->Get("Total payment") + ":"), HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer description line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(6, utf8_to_cp1251(GetTableSum()), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_pre_tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(9, utf8_to_cp1251(GetTableVAT()), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(10, utf8_to_cp1251(GetTableTotal()), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_post_tax line"); }
		}


		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_StopTable()).length())
			{ MESSAGE_ERROR("", "", "fail to stop table"); }
		}

	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print title");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawFooter() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
/*		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Total") + ":"), 20, 85, HPDF_TALIGN_RIGHT, BOLD_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetTableSum()), 85, 100, HPDF_TALIGN_RIGHT, BOLD_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Vat short") + ":"), 20, 85, HPDF_TALIGN_RIGHT, BOLD_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetTableVAT()), 85, 100, HPDF_TALIGN_RIGHT, BOLD_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Total payment") + ":"), 20, 85, HPDF_TALIGN_RIGHT, BOLD_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetTableTotal()), 85, 100, HPDF_TALIGN_RIGHT, BOLD_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(SpellTotalItemsAndSum()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(SpellPrice()), 0, 100, HPDF_TALIGN_LEFT, BOLD_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("act_footnote")), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}


		if(error_message.empty())
		{
			if((error_message = PrintPDFFooter()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print pdf footer"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(0, 100)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
*/
		if(error_message.empty())
		{
			if((error_message = __HPDF_PrintSignature()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print pdf footer"); }
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print title");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::PrintAsPDF() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	pdf_obj.SetFilename(GetFilename());
	pdf_obj.SetOrientation(HPDF_PAGE_LANDSCAPE);
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_init()).length())			{ MESSAGE_ERROR("", "", "fail to initialize hpdf library"); }
	}
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_SetDocProps()).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to set dc props"); }
	}
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_MoveLineDown(0)).length())	{ MESSAGE_ERROR("", "", "hpdf: fail to move pointer down"); }
	}
	if(error_message.empty())
	{
		if((error_message = __HPDF_DrawHeader()).length())				{ MESSAGE_ERROR("", "", "hpdf: fail to draw timecard title"); }
	}
	if(error_message.empty())
	{
		if((error_message = __HPDF_DrawTable()).length())				{ MESSAGE_ERROR("", "", "hpdf: fail to print table"); }
	}
	if(error_message.empty())
	{
		if((error_message = __HPDF_DrawFooter()).length())				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
	}
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_SaveToFile()).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to save to file"); }
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

ostream& operator<<(ostream& os, const C_Print_VAT_Base &var)
{
	os << "object C_Print_VAT_Base [empty for now]";

	return os;
}



