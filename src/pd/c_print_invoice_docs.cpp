#include "c_print_invoice_docs.h"


auto	C_Print_Invoice_Docs_Base::GetSupplierCompanyDetails() -> string
{
	return GetSupplierName() + ", " + vars->Get("TIN") + " " + GetSupplierTIN() + ", " + vars->Get("KPP") + " " + GetSupplierKPP() + ", " + GetSupplierLegalZIP() + " " + GetSupplierLegalLocality() + ", " + GetSupplierLegalAddress();
}

auto	C_Print_Invoice_Docs_Base::GetCustomerCompanyDetails() -> string
{
	return GetCustomerName() + ", " + vars->Get("TIN") + " " + GetCustomerTIN() + ", " + vars->Get("KPP") + " " + GetCustomerKPP() + ", " + GetCustomerLegalZIP() + " " + GetCustomerLegalLocality() + ", " + GetCustomerLegalAddress();
}

auto	C_Print_Invoice_Docs_Base::SpellPrice() -> string
{
	MESSAGE_DEBUG("", "", "start");

	C_Price_Spelling	price(stod_noexcept(GetTableTotal()));
	auto				result = GetTotalPaymentSpelling() + price.Spelling();

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

// --- XLS part
auto	C_Print_Invoice_Docs_Base::__XLS_DrawBorder(int left, int top, int right, int bottom) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(right < left)
	{
		// --- swap left right
		int temp = right;

		right = left;
		left = temp;
	}

	if(bottom < top)
	{
		// --- swap top bottom
		int temp = top;

		top = bottom;
		bottom = temp;
	}

	if((left == right) && (bottom == top))
	{
		// --- single cell border
		auto	format_border_cell = __book->addFormat();

		format_border_cell->setBorder(libxl::BORDERSTYLE_THIN);
		__sheet->setCellFormat(top, left, format_border_cell);
	}
	else if((left == right) && (top < bottom))
	{
		auto	format_top_cell = __book->addFormat();
		auto	format_middle_cell = __book->addFormat();
		auto	format_bottom_cell = __book->addFormat();

		format_top_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_top_cell->setBorderRight(libxl::BORDERSTYLE_THIN);
		format_top_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_bottom_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_bottom_cell->setBorderRight(libxl::BORDERSTYLE_THIN);
		format_bottom_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderRight(libxl::BORDERSTYLE_THIN);

		for(auto i = top + 1; i < bottom; ++i)
			__sheet->setCellFormat(i, left, format_middle_cell);

		__sheet->setCellFormat(top, left, format_top_cell);
		__sheet->setCellFormat(bottom, left, format_bottom_cell);
	}
	else if((left < right) && (top == bottom))
	{
		auto	format_left_cell = __book->addFormat();
		auto	format_middle_cell = __book->addFormat();
		auto	format_right_cell = __book->addFormat();

		format_left_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_left_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_left_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);
		format_right_cell->setBorderRight(libxl::BORDERSTYLE_THIN);
		format_right_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_right_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);

		for(auto i = left + 1; i < right; ++i)
			__sheet->setCellFormat(top, i, format_middle_cell);

		__sheet->setCellFormat(top, left, format_left_cell);
		__sheet->setCellFormat(bottom, right, format_right_cell);
	}
	else
	{
		auto	format_top_left			= __book->addFormat();
		auto	format_top				= __book->addFormat();
		auto	format_top_right		= __book->addFormat();
		auto	format_right			= __book->addFormat();
		auto	format_bottom_right		= __book->addFormat();
		auto	format_bottom			= __book->addFormat();
		auto	format_bottom_left		= __book->addFormat();
		auto	format_left				= __book->addFormat();

		format_top_left			->setBorderTop		(libxl::BORDERSTYLE_THIN);
		format_top_left			->setBorderLeft		(libxl::BORDERSTYLE_THIN);
		format_top				->setBorderTop		(libxl::BORDERSTYLE_THIN);
		format_top_right		->setBorderTop		(libxl::BORDERSTYLE_THIN);
		format_top_right		->setBorderRight	(libxl::BORDERSTYLE_THIN);
		format_right			->setBorderRight	(libxl::BORDERSTYLE_THIN);
		format_bottom_right		->setBorderBottom	(libxl::BORDERSTYLE_THIN);
		format_bottom_right		->setBorderRight	(libxl::BORDERSTYLE_THIN);
		format_bottom			->setBorderBottom	(libxl::BORDERSTYLE_THIN);
		format_bottom_left		->setBorderBottom	(libxl::BORDERSTYLE_THIN);
		format_bottom_left		->setBorderLeft		(libxl::BORDERSTYLE_THIN);
		format_left				->setBorderLeft		(libxl::BORDERSTYLE_THIN);

		for(auto i = left + 1; i < right; ++i)
		{
			__sheet->setCellFormat(top, i, format_top);
			__sheet->setCellFormat(bottom, i, format_bottom);
		}
		for(auto i = top + 1; i < bottom; ++i)
		{
			__sheet->setCellFormat(i, left, format_left);
			__sheet->setCellFormat(i, right, format_right);
		}

		__sheet->setCellFormat(top, left, format_top_left);
		__sheet->setCellFormat(top, right, format_top_right);
		__sheet->setCellFormat(bottom, left, format_bottom_left);
		__sheet->setCellFormat(bottom, right, format_bottom_right);

	}


	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_Invoice_Docs_Base::__PrintXLSHeaderTable() -> string
{
	auto	result = ""s;
	auto	top = __row_counter;

	MESSAGE_DEBUG("", "", "start");

	__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetSupplierBankName()).c_str());
	__sheet->writeStr(__row_counter, 6, multibyte_to_wide(vars->Get("Bank Identifier")).c_str());
	__sheet->writeStr(__row_counter, 7, multibyte_to_wide(GetSupplierBIK()).c_str());

	++__row_counter;
	__sheet->writeStr(__row_counter, 6, multibyte_to_wide(vars->Get("Account")).c_str());
	__sheet->writeStr(__row_counter, 7, multibyte_to_wide(GetSupplierBankAccount()).c_str());

	++__row_counter;
	__sheet->writeStr(__row_counter, 1, multibyte_to_wide(vars->Get("Destination bank")).c_str());

	++__row_counter;
	__sheet->writeStr(__row_counter, 1, multibyte_to_wide(vars->Get("TIN")).c_str());
	__sheet->writeStr(__row_counter, 2, multibyte_to_wide(GetSupplierTIN()).c_str());
	__sheet->writeStr(__row_counter, 4, multibyte_to_wide(vars->Get("KPP") + " " + GetSupplierKPP()).c_str());
	__sheet->writeStr(__row_counter, 6, multibyte_to_wide(vars->Get("Account")).c_str());
	__sheet->writeStr(__row_counter, 7, multibyte_to_wide(GetSupplierAccount()).c_str());

	++__row_counter;
	__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetSupplierName()).c_str());

	++__row_counter;
	++__row_counter;
	__sheet->writeStr(__row_counter, 1, multibyte_to_wide(vars->Get("Recipient")).c_str());

	++__row_counter;
	++__row_counter;

	// bottom = __row_counter;

	__XLS_DrawBorder(1, top	, 5, top + 2);											__XLS_DrawBorder(6, top	, 6, top	);	__XLS_DrawBorder(7, top	, 9, top	);
																					__XLS_DrawBorder(6, top + 1, 6, top + 2);	__XLS_DrawBorder(7, top + 1, 9, top + 2);
	__XLS_DrawBorder(1, top + 3, 3, top + 3);	__XLS_DrawBorder(4, top + 3, 5, top + 3);	__XLS_DrawBorder(6, top + 3, 6, top + 6);	__XLS_DrawBorder(7, top + 3, 9, top + 6);
	__XLS_DrawBorder(1, top + 4, 5, top + 6);

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto C_Print_Invoice_Docs_Base::__PrintXLSSignature() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	font_bold = __book->addFont();
	auto	font_small = __book->addFont();
	auto	format_bold = __book->addFormat();
	auto	format_right = __book->addFormat();
	auto	format_right_small = __book->addFormat();
	auto	format_underline = __book->addFormat();

	font_small->setSize(7);
	font_bold->setBold();

	format_bold->setFont(font_bold);
	format_right->setAlignH(libxl::ALIGNH_RIGHT);
	format_right_small->setFont(font_small);
	format_right_small->setAlignH(libxl::ALIGNH_RIGHT);
	format_underline->setBorderBottom(libxl::BORDERSTYLE_THIN);

	++__row_counter;
	if(GetSignatureTitle1().length())
	{
		__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetSignatureTitle1()).c_str(), format_bold);
		__sheet->writeStr(__row_counter, 3, L"", format_underline);
		__sheet->writeStr(__row_counter, 4, L"", format_underline);
	}
	if(GetSignatureTitle2().length())
	{
		__sheet->writeStr(__row_counter, 6, multibyte_to_wide(GetSignatureTitle2()).c_str(), format_bold);
		__sheet->writeStr(__row_counter, 8, L"", format_underline);
		__sheet->writeStr(__row_counter, 9, L"", format_underline);
	}

	++__row_counter;
	if(GetSignatureTitle1().length()) __sheet->writeStr(__row_counter, 4, multibyte_to_wide(GetSignatureName1()).c_str(), format_right);
	if(GetSignatureTitle2().length()) __sheet->writeStr(__row_counter, 9, multibyte_to_wide(GetSignatureName2()).c_str(), format_right);

	++__row_counter;
	if(GetSignatureTitle1().length()) __sheet->writeStr(__row_counter, 4, multibyte_to_wide(GetSignatureInfo1()).c_str(), format_right_small);
	if(GetSignatureTitle2().length()) __sheet->writeStr(__row_counter, 9, multibyte_to_wide(GetSignatureInfo2()).c_str(), format_right_small);


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_Invoice_Docs_Base::__DrawXLSRowUnderline() -> string
{
	auto	result = ""s;
	auto	format_underline = __book->addFormat();

	MESSAGE_DEBUG("", "", "start");

	format_underline->setBorderBottom(libxl::BORDERSTYLE_THICK);

	__sheet->setRow(__row_counter, LIBXL_DEFAULT_ROW_HEIGHT / 2);
	__sheet->writeStr(__row_counter, 1, L"", format_underline);
	__sheet->writeStr(__row_counter, 2, L"", format_underline);
	__sheet->writeStr(__row_counter, 3, L"", format_underline);
	__sheet->writeStr(__row_counter, 4, L"", format_underline);
	__sheet->writeStr(__row_counter, 5, L"", format_underline);
	__sheet->writeStr(__row_counter, 6, L"", format_underline);
	__sheet->writeStr(__row_counter, 7, L"", format_underline);
	__sheet->writeStr(__row_counter, 8, L"", format_underline);
	__sheet->writeStr(__row_counter, 9, L"", format_underline);

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_Invoice_Docs_Base::__PrintXLSComment() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	result				= ""s;
	auto	format_top_left		= __book->addFormat();

	format_top_left->setWrap(true);
	format_top_left->setBorderTop		(libxl::BORDERSTYLE_THIN);
	format_top_left->setBorderLeft		(libxl::BORDERSTYLE_THIN);

	++__row_counter;
	++__row_counter;
	__sheet->setRow(__row_counter, LIBXL_DEFAULT_ROW_HEIGHT * (ceil(double(GetFooterComment().length()) / 120)));
	__sheet->setMerge(__row_counter, __row_counter, 1, 9);
	__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetFooterComment()).c_str(), format_top_left);

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_Invoice_Docs_Base::PrintAsXLS() -> string
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
			__book->setKey(L"Ivan Kuchin", L"linux-e7d7147298a7a2140508293e47t5j7sf");

			__sheet = __book->addSheet(L"Sheet1");
			if(__sheet)
			{

				auto			font_big					= __book->addFont();
				auto			font_bold					= __book->addFont();
				auto			format_big					= __book->addFormat();
				auto			format_bold					= __book->addFormat();
				auto			format_bold_right			= __book->addFormat();
				auto			format_bold_border			= __book->addFormat();
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
				__sheet->setLandscape(false);
				// __sheet->setPrintZoom(50);

				font_big->setSize(12);
				font_big->setBold();
				font_bold->setBold();

				format_big->setFont(font_big);
				format_bold->setFont(font_bold);
				format_bold_right->setFont(font_bold);
				format_bold_right->setAlignH(libxl::ALIGNH_RIGHT);
				format_bold_border->setFont(font_bold);
				format_bold_border->setBorder(libxl::BORDERSTYLE_THIN);
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
				__sheet->setCol(0, 0, 0.1);
				__sheet->setCol(1, 1, 5);
				__sheet->setCol(5, 5, 18.45);
				__sheet->setCol(6, 6, 6.71);
				__sheet->setCol(7, 7, 3);
				__sheet->setCol(8, 9, 11);  // --- price and total columns
//				__sheet->setCol(2, 2, 31 + 2);
				// __sheet->setCol(2, 31 + 2, 5);

				PrintXLSHeader();

				__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetDocumentTitle()).c_str(), format_big);

				++__row_counter;
				__DrawXLSRowUnderline();

				++__row_counter;
				++__row_counter;
				__sheet->setMerge(__row_counter, __row_counter, 1, 2);
				__sheet->setMerge(__row_counter, __row_counter, 3, 9);
				__sheet->setRow(__row_counter, LIBXL_DEFAULT_ROW_HEIGHT * 2);
				__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetSupplierSpelling()).c_str(), format_top_left);
				__sheet->writeStr(__row_counter, 3, multibyte_to_wide(GetSupplierCompanyDetails()).c_str(), format_top_left);

				++__row_counter;
				++__row_counter;
				__sheet->setMerge(__row_counter, __row_counter, 1, 2);
				__sheet->setMerge(__row_counter, __row_counter, 3, 9);
				__sheet->setRow(__row_counter, LIBXL_DEFAULT_ROW_HEIGHT * 2);
				__sheet->writeStr(__row_counter, 1, multibyte_to_wide(GetCustomerSpelling()).c_str(), format_top_left);
				__sheet->writeStr(__row_counter, 3, multibyte_to_wide(GetCustomerCompanyDetails()).c_str(), format_top_left);

				++__row_counter;
				++__row_counter;
				__sheet->setMerge(__row_counter, __row_counter, 1, 2);
				__sheet->setMerge(__row_counter, __row_counter, 3, 9);
				__sheet->setRow(__row_counter, LIBXL_DEFAULT_ROW_HEIGHT * 2);
				__sheet->writeStr(__row_counter, 1, multibyte_to_wide(vars->Get("Basis")).c_str(), format_top_left);
				__sheet->writeStr(__row_counter, 3, multibyte_to_wide(vars->Get("act_basis")).c_str(), format_top_left);

				// --- table header
				++__row_counter;
				++__row_counter;
				++__row_counter;
				__sheet->setMerge(__row_counter, __row_counter, 2, 5);
				__sheet->writeStr(__row_counter, 1, L"№", format_bold_border);
				__sheet->writeStr(__row_counter, 2, multibyte_to_wide(GetTableMainTitleSpelling()).c_str(), format_bold_border);
				__sheet->writeStr(__row_counter, 3, L"", format_bold_border);
				__sheet->writeStr(__row_counter, 4, L"", format_bold_border);
				__sheet->writeStr(__row_counter, 5, L"", format_bold_border);
				__sheet->writeStr(__row_counter, 6, multibyte_to_wide(vars->Get("Quantity short")).c_str(), format_bold_border);
				__sheet->writeStr(__row_counter, 7, multibyte_to_wide(vars->Get("Items short")).c_str(), format_bold_border);
				__sheet->writeStr(__row_counter, 8, multibyte_to_wide(vars->Get("Price")).c_str(), format_bold_border);
				__sheet->writeStr(__row_counter, 9, multibyte_to_wide(vars->Get("Total")).c_str(), format_bold_border);

				// --- table body
				for(auto i = 1; isTableRowExists(i); ++i)
				{
					auto	description = GetTableRowDescription(i);

					++__row_counter;

					__sheet->setMerge(__row_counter, __row_counter, 2, 5);
					__sheet->setRow(__row_counter, ceil(double(description.length()) / 60) * LIBXL_DEFAULT_ROW_HEIGHT);

					__sheet->writeNum(__row_counter, 1, stod_noexcept(GetTableRowIndex(i)), format_table_center);
					__sheet->writeStr(__row_counter, 2, multibyte_to_wide(description).c_str(), format_table_left);

					__sheet->writeStr(__row_counter, 3, L"", format_table_left);
					__sheet->writeStr(__row_counter, 4, L"", format_table_left);
					__sheet->writeStr(__row_counter, 5, L"", format_table_left);

					__sheet->writeNum(__row_counter, 6, stod_noexcept(GetTableRowQuantity(i)), format_table_right);
					__sheet->writeStr(__row_counter, 7, multibyte_to_wide(GetTableRowItem(i)).c_str(), format_table_left);
					__sheet->writeNum(__row_counter, 8, stod_noexcept(GetTableRowPrice(i)), format_table_right);
					__sheet->writeNum(__row_counter, 9, stod_noexcept(GetTableRowPrice(i)), format_table_right);

					++total_table_items;
				}

				// --- table summary
				++__row_counter;
				++__row_counter;
				__sheet->writeStr(__row_counter, 7, multibyte_to_wide(vars->Get("Total") + ":").c_str(), format_bold_right);
				__sheet->writeNum(__row_counter, 9, stod_noexcept(GetTableSum()), format_number_d2_bold_right);

				if(stod_noexcept(GetTableVAT()))
				{
					++__row_counter;
					__sheet->writeStr(__row_counter, 7, multibyte_to_wide(vars->Get("VAT sum") + ":").c_str(), format_bold_right);
					__sheet->writeNum(__row_counter, 9, stod_noexcept(GetTableVAT()), format_number_d2_bold_right);
				}
				else
				{
					MESSAGE_DEBUG("", "", "don't print VAT-line due to it is zero")
				}

				++__row_counter;
				__sheet->writeStr(__row_counter, 7, multibyte_to_wide(vars->Get("Total payment") + ":").c_str(), format_bold_right);
				__sheet->writeNum(__row_counter, 9, stod_noexcept(GetTableTotal()), format_number_d2_bold_right);

				++__row_counter;
				++__row_counter;
				__sheet->setMerge(__row_counter, __row_counter, 1, 9);
				__sheet->writeStr(__row_counter, 1, multibyte_to_wide(SpellTotalItemsAndSum()).c_str());

				++__row_counter;
				__sheet->setRow(__row_counter, LIBXL_DEFAULT_ROW_HEIGHT * 2);
				__sheet->setMerge(__row_counter, __row_counter, 1, 9);
				__sheet->writeStr(__row_counter, 1, multibyte_to_wide(SpellPrice()).c_str(), format_top_left_bold);

				PrintXLSComment();
				PrintXLSFooter();

				++__row_counter;
				++__row_counter;
				__DrawXLSRowUnderline();

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
auto	C_Print_Invoice_Docs_Base::__PrintPDFHeaderTable() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierBankName()), 0 + 1, 50 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Bank Identifier")), 50 + 1, 65 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierBIK()), 65 + 1, 100 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Account")), 50 + 1, 65 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierBankAccount()), 65 + 1, 100 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Destination bank")), 0 + 1, 50 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("TIN") + " " + GetSupplierTIN()), 0 + 1, 25 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("KPP") + " " + GetSupplierKPP()), 25 + 1, 50 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Account")), 50 + 1, 65 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierAccount()), 65 + 1, 100 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierName()), 0 + 1, 50 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Recipient")), 0 + 1, 50 - 1, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		// --- Bank name
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(0, -6, 50, -4)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}
		// --- BIK
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(50, -6, 65, -6)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}
		// --- BIK number
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(65, -6, 100, -6)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}
		// --- Bank acc
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(50, -5, 65, -4)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}
		// --- Bank acc number
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(65, -5, 100, -4)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}

		// --- Company acc
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(50, -3, 65, 0)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}
		// --- Company acc number
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(65, -3, 100, 0)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}

		// --- TIN
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(0, -3, 25, -3)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}
		// --- KPP
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(25, -3, 50, -3)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}

		// --- Company name
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawRect(0, -2, 50, 0)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to draw rectangle"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
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

auto C_Print_Invoice_Docs_Base::__HPDF_PrintSignature() -> string
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
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureTitle1()), 0, 20, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureTitle2()), 60, 60+20, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
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
				if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(20, 40)).length())
					{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
			}
		}
		if(GetSignatureTitle2().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(80, 100)).length())
					{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
			}
		}

		if(GetSignatureTitle1().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureName1()), 0, 40, HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(GetSignatureTitle2().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureName2()), 60, 60+40, HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
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
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureInfo1()), 0, 40, HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE - 2, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(GetSignatureTitle2().length())
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureInfo2()), 60, 60+40, HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE - 2, false)).length())
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

auto	C_Print_Invoice_Docs_Base::__HPDF_DrawHeader() -> string
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
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetDocumentTitle()), 0, 100, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE * 2, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
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
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierSpelling()), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierCompanyDetails()), 20, 100, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetCustomerSpelling()), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetCustomerCompanyDetails()), 20, 100, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if(vars->Get("act_basis").length())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Basis")), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
					{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("act_basis")), 20, 100, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
					{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
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

auto	C_Print_Invoice_Docs_Base::__HPDF_DrawTable() -> string
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
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(55);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(15);
			pdf_obj.AddColumn(15);

			if((error_message = pdf_obj.__HPDF_StartTable()).length())
			{
				MESSAGE_ERROR("", "", "fail to start table");
			}
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(0, utf8_to_cp1251("№"), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title index line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(GetTableMainTitleSpelling()), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title description line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(2, utf8_to_cp1251(vars->Get("Quantity short")), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title quantity line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(3, utf8_to_cp1251(vars->Get("Items short")), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title items line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(4, utf8_to_cp1251(vars->Get("Price")), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title price line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(5, utf8_to_cp1251(vars->Get("Total")), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}

		if(error_message.empty())
		{
			total_table_items = 0;
			for(auto i = 1; isTableRowExists(i); ++i)
			{
				auto	max_lines = max(pdf_obj.__HPDF_GetNumberOfLinesInTable(0, utf8_to_cp1251(GetTableRowIndex(i))		, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE),
									max(pdf_obj.__HPDF_GetNumberOfLinesInTable(1, utf8_to_cp1251(GetTableRowDescription(i))	, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE),
									max(pdf_obj.__HPDF_GetNumberOfLinesInTable(2, utf8_to_cp1251(GetTableRowQuantity(i))	, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE),
									max(pdf_obj.__HPDF_GetNumberOfLinesInTable(3, utf8_to_cp1251(GetTableRowItem(i))		, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE),
									max(pdf_obj.__HPDF_GetNumberOfLinesInTable(4, utf8_to_cp1251(GetTableRowPrice(i))		, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE),
										pdf_obj.__HPDF_GetNumberOfLinesInTable(5, utf8_to_cp1251(GetTableRowTotal(i))		, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE)
									)))));

				if((error_message = pdf_obj.__HPDF_PrintTextTableCell(0, utf8_to_cp1251(GetTableRowIndex(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE, false)).empty())
				{
					if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(GetTableRowDescription(i)), HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE, false)).empty())
					{
						if((error_message = pdf_obj.__HPDF_PrintTextTableCell(2, utf8_to_cp1251(GetTableRowQuantity(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE, false)).empty())
						{
							if((error_message = pdf_obj.__HPDF_PrintTextTableCell(3, utf8_to_cp1251(GetTableRowItem(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE, false)).empty())
							{
								if((error_message = pdf_obj.__HPDF_PrintTextTableCell(4, utf8_to_cp1251(GetTableRowPrice(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE, false)).empty())
								{
									if((error_message = pdf_obj.__HPDF_PrintTextTableCell(5, utf8_to_cp1251(GetTableRowPrice(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_TABLE_FONT_SIZE, false)).empty())
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
									MESSAGE_ERROR("", "", "fail to write table price (" + to_string(i) + ") line");
								}
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to write table item (" + to_string(i) + ") line");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "fail to write table quantity (" + to_string(i) + ") line");
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

auto	C_Print_Invoice_Docs_Base::__HPDF_DrawFooter() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Total") + ":"), 20, 85, HPDF_TALIGN_RIGHT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetTableSum()), 85, 100, HPDF_TALIGN_RIGHT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(stod_noexcept(GetTableVAT()))
		{
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT sum") + ":"), 20, 85, HPDF_TALIGN_RIGHT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetTableVAT()), 85, 100, HPDF_TALIGN_RIGHT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "don't print VAT-line due to it is zero")
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Total payment") + ":"), 20, 85, HPDF_TALIGN_RIGHT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetTableTotal()), 85, 100, HPDF_TALIGN_RIGHT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(SpellTotalItemsAndSum()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(SpellPrice()), 0, 100, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetFooterComment()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
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

auto	C_Print_Invoice_Docs_Base::PrintAsPDF() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	pdf_obj.SetFilename(GetFilename());
	pdf_obj.SetOrientation(HPDF_PAGE_PORTRAIT);
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

ostream& operator<<(ostream& os, const C_Print_Invoice_Docs_Base &var)
{
	os << "object C_Print_Invoice_Docs_Base [empty for now]";

	return os;
}



