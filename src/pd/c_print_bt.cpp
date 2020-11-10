#include "c_print_bt.h"

auto C_Print_BT::SetBT(const C_BT_To_Print &param1) -> void
{
	bt = param1;
	// effort_hours = 0;
	// effort_days = 0;
	effort_cost = 0;
	effort_cost_vat = 0;
	total_payment = 0;
}

auto	C_Print_BT::GetSpelledTitle() -> string
{
	auto			start_spelling_date = GetSpellingFormattedDate(bt.GetDateStart(), "%d %b %G");
	auto			finish_spelling_date = GetSpellingFormattedDate(bt.GetDateFinish(), "%d %b %G");

	return vars->Get("Remote service report over period") + " "s + vars->Get("from") + " " + start_spelling_date + " " + vars->Get("up to") + " " + finish_spelling_date;
}

auto	C_Print_BT::GetSpelledPSoW() -> string
{
	auto			sign_spelling_date = GetSpellingFormattedDate(bt.GetDateSign(), "%d %b %G");

	return vars->Get("PSoW") + " №"s + bt.GetAgreementNumber() + " " + vars->Get("agreement from") + " " + sign_spelling_date;
}

// TODO: --- spelled position clean-up
/*
auto	C_Print_BT::GetSpelledInitials(string idx) -> string
{
	auto	result = ""s;

	if(vars && vars->Get("subc2agency_bt_signature_name" + idx).length())
	{
		result += vars->Get("subc2agency_bt_signature_name" + idx);
	}
	else if(vars && vars->Get("agency2cc_bt_signature_name" + idx).length())
	{
		result += vars->Get("agency2cc_bt_signature_name" + idx);
	}
	else
	{
		MESSAGE_DEBUG("", "", "var not found");
	}

	return result;
}
*/

// TODO: --- spelled position clean-up
/*
auto	C_Print_BT::GetSpelledPosition(string idx) -> string
{
	auto	result = ""s;

	if(vars && vars->Get("subc2agency_bt_position" + idx).length())
	{
		result += vars->Get("subc2agency_bt_position" + idx);
	}
	else if(vars && vars->Get("agency2cc_bt_position" + idx).length())
	{
		result += vars->Get("agency2cc_bt_position" + idx);
	}
	else
	{
		MESSAGE_DEBUG("", "", "var not found");
	}

	return result;
}
*/

auto	C_Print_BT::__XLS_print_formatted_footer_line(string title, c_float temp) -> void
{
	MESSAGE_DEBUG("", "", "start");

	auto	spelled_rur					= multibyte_to_wide(GetSpelledRur());
	auto	spelled_kop					= multibyte_to_wide(GetSpelledKop());

	__xls_sheet->writeStr(__xls_row_counter, 1, multibyte_to_wide(title).c_str());
	__xls_sheet->writeNum(__xls_row_counter, 6, temp.GetWhole());
	__xls_sheet->writeStr(__xls_row_counter, 7, spelled_rur.c_str());
	__xls_sheet->writeNum(__xls_row_counter, 8, temp.GetFraction());
	__xls_sheet->writeStr(__xls_row_counter, 9, spelled_kop.c_str());

	MESSAGE_DEBUG("", "", "finish");
}

auto	C_Print_BT::__XLS_DrawBorder(int left, int top, int right, int bottom) -> string
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
		auto	format_border_cell = __xls_book->addFormat();

		format_border_cell->setBorder(libxl::BORDERSTYLE_THIN);
		__xls_sheet->setCellFormat(top, left, format_border_cell);
	}
	else if((left == right) && (top < bottom))
	{
		auto	format_top_cell = __xls_book->addFormat();
		auto	format_middle_cell = __xls_book->addFormat();
		auto	format_bottom_cell = __xls_book->addFormat();

		format_top_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_top_cell->setBorderRight(libxl::BORDERSTYLE_THIN);
		format_top_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_bottom_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_bottom_cell->setBorderRight(libxl::BORDERSTYLE_THIN);
		format_bottom_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderRight(libxl::BORDERSTYLE_THIN);

		for(auto i = top + 1; i < bottom; ++i)
			__xls_sheet->setCellFormat(i, left, format_middle_cell);

		__xls_sheet->setCellFormat(top, left, format_top_cell);
		__xls_sheet->setCellFormat(bottom, left, format_bottom_cell);
	}
	else if((left < right) && (top == bottom))
	{
		auto	format_left_cell = __xls_book->addFormat();
		auto	format_middle_cell = __xls_book->addFormat();
		auto	format_right_cell = __xls_book->addFormat();

		format_left_cell->setBorderLeft(libxl::BORDERSTYLE_THIN);
		format_left_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_left_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);
		format_right_cell->setBorderRight(libxl::BORDERSTYLE_THIN);
		format_right_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_right_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderTop(libxl::BORDERSTYLE_THIN);
		format_middle_cell->setBorderBottom(libxl::BORDERSTYLE_THIN);

		for(auto i = left + 1; i < right; ++i)
			__xls_sheet->setCellFormat(top, i, format_middle_cell);

		__xls_sheet->setCellFormat(top, left, format_left_cell);
		__xls_sheet->setCellFormat(bottom, right, format_right_cell);
	}
	else
	{
		auto	format_top_left			= __xls_book->addFormat();
		auto	format_top				= __xls_book->addFormat();
		auto	format_top_right		= __xls_book->addFormat();
		auto	format_right			= __xls_book->addFormat();
		auto	format_bottom_right		= __xls_book->addFormat();
		auto	format_bottom			= __xls_book->addFormat();
		auto	format_bottom_left		= __xls_book->addFormat();
		auto	format_left				= __xls_book->addFormat();

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
			__xls_sheet->setCellFormat(top, i, format_top);
			__xls_sheet->setCellFormat(bottom, i, format_bottom);
		}
		for(auto i = top + 1; i < bottom; ++i)
		{
			__xls_sheet->setCellFormat(i, left, format_left);
			__xls_sheet->setCellFormat(i, right, format_right);
		}

		__xls_sheet->setCellFormat(top, left, format_top_left);
		__xls_sheet->setCellFormat(top, right, format_top_right);
		__xls_sheet->setCellFormat(bottom, left, format_bottom_left);
		__xls_sheet->setCellFormat(bottom, right, format_bottom_right);

	}


	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_BT::__XLS_DrawUnderline(int left, int right) -> string
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

	{
		auto	format_bottom = __xls_book->addFormat();

		format_bottom->setBorderBottom	(libxl::BORDERSTYLE_THIN);

		for(auto i = left; i <= right; ++i)
		{
			__xls_sheet->setCellFormat(__xls_row_counter, i, format_bottom);
		}
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_BT::PrintAsXLS() -> string
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
		auto	spelled_title				= multibyte_to_wide(GetSpelledTitle());
		auto	spelled_psow				= multibyte_to_wide(GetSpelledPSoW());
		auto	spelled_projectid			= multibyte_to_wide(GetSpelledProjectID());
		auto	spelled_signature			= multibyte_to_wide(GetSpelledSignature());
		auto	spelled_initials			= multibyte_to_wide(GetSpelledInitials());
		auto	spelled_position			= multibyte_to_wide(GetSpelledPosition());
		auto	spelled_date				= multibyte_to_wide(GetSpelledDate());
		
		index = vars->GetIndexByBTID(bt.GetID());

		__xls_book = xlCreateBook();
		if(__xls_book)
		{
			__xls_book->setKey(L"Ivan Kuchin", L"linux-e7d7147298a7a2140508293e47t5j7sf");

			__xls_sheet = __xls_book->addSheet(L"Sheet1");
			if(__xls_sheet)
			{
				auto			format_title = __xls_book->addFormat();
				auto			format_weekend = __xls_book->addFormat();
				auto			format_table_number_d2 = __xls_book->addFormat();
				auto			format_summary_over = __xls_book->addFormat();
				auto			format_summary_right = __xls_book->addFormat();
				auto			format_number_d2 = __xls_book->addFormat();
				auto			font_summary_over = __xls_book->addFont();
				auto			font_summary_right = __xls_book->addFont();
				auto			start_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(bt.GetDateStart(), "%d %b %G"));
				auto			finish_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(bt.GetDateFinish(), "%d %b %G"));
				auto			bt_expenses = bt.GetExpenseLines();

				__xls_row_counter = 1;

				format_title->setBorder(libxl::BORDERSTYLE_THIN);

				format_weekend->setFillPattern(libxl::FILLPATTERN_SOLID);
				format_weekend->setPatternForegroundColor(libxl::COLOR_GRAY25);
				format_weekend->setBorder(libxl::BORDERSTYLE_THIN);
				format_weekend->setNumFormat(libxl::NUMFORMAT_GENERAL);

				format_table_number_d2->setBorder(libxl::BORDERSTYLE_THIN);
				format_table_number_d2->setNumFormat(libxl::NUMFORMAT_NUMBER_D2);

				font_summary_over->setColor(libxl::COLOR_DARKRED);
				format_summary_over->setFont(font_summary_over);
				format_summary_over->setFillPattern(libxl::FILLPATTERN_SOLID);
				format_summary_over->setPatternForegroundColor(libxl::COLOR_ROSE);
				format_summary_over->setBorder(libxl::BORDERSTYLE_THIN);

				font_summary_right->setColor(libxl::COLOR_DARKGREEN);
				format_summary_right->setFont(font_summary_right);
				format_summary_right->setFillPattern(libxl::FILLPATTERN_SOLID);
				format_summary_right->setPatternForegroundColor(libxl::COLOR_LIGHTGREEN);
				format_summary_right->setBorder(libxl::BORDERSTYLE_THIN);

				format_number_d2->setNumFormat(libxl::NUMFORMAT_NUMBER_D2);

				// --- print properties
				__xls_sheet->setPaper(libxl::PAPER_A4);
				__xls_sheet->setLandscape();
				__xls_sheet->setPrintZoom(75);

				// --- set columns width
				__xls_sheet->setCol(1, 1, 10);
				__xls_sheet->setCol(11, 13, 16);

				__xls_row_counter++;
				__xls_sheet->writeStr(__xls_row_counter, 6, spelled_title.c_str());

				__xls_row_counter++;
				__xls_row_counter++;
				__xls_sheet->writeStr(__xls_row_counter, 1, spelled_psow.c_str());

				__xls_row_counter++;
				__xls_sheet->writeStr(__xls_row_counter, 1, spelled_projectid.c_str());

				if(GetSpelledLocation().length())
				{
					__xls_row_counter++;
					__xls_sheet->writeStr(__xls_row_counter, 1, multibyte_to_wide(GetSpelledLocation()).c_str());
				}


				// --- table header
				{
					__xls_row_counter++;
					__xls_row_counter++;
					__xls_sheet->writeStr(__xls_row_counter, 1, multibyte_to_wide(GetTitleDate()).c_str()			, format_title);
					__xls_sheet->setMerge(__xls_row_counter, __xls_row_counter, 2, 10);
					__XLS_DrawBorder(2, __xls_row_counter, 10, __xls_row_counter);
					__xls_sheet->writeStr(__xls_row_counter, 2, multibyte_to_wide(GetTitleExpense()).c_str()		, format_title);
					__xls_sheet->writeStr(__xls_row_counter, 11, multibyte_to_wide(GetTitleSumRur()).c_str()			, format_title);
					__xls_sheet->writeStr(__xls_row_counter, 12, multibyte_to_wide(GetTitleSumCurrency()).c_str()	, format_title);
					__xls_sheet->writeStr(__xls_row_counter, 13, multibyte_to_wide(GetTitleRateExchange()).c_str()	, format_title);
				}

				// --- table body
				__xls_row_counter++;
				for(auto &bt_expense: bt_expenses)
				{
					wstring		bt_line_date = multibyte_to_wide(GetSpellingFormattedDate(bt_expense.date, "%d.%m.%Y"));
					wstring		bt_line_description = multibyte_to_wide(bt_expense.description);
					c_float		bt_line_price_domestic(bt_expense.price_domestic);
					c_float		bt_line_price_foreign(bt_expense.price_foreign);
					wstring		bt_line_price_rate_exch = multibyte_to_wide(bt_expense.currency_nominal + TIMECARD_ENTRY_TITLE_SEPARATOR + bt_expense.currency_value);

					__xls_sheet->writeStr(__xls_row_counter, 1, bt_line_date.c_str(), format_title);
					__xls_sheet->setMerge(__xls_row_counter, __xls_row_counter, 2, 10);
					__XLS_DrawBorder(2, __xls_row_counter, 10, __xls_row_counter);
					__xls_sheet->writeStr(__xls_row_counter, 2, bt_line_description.c_str(), format_title);
					__xls_sheet->writeNum(__xls_row_counter, 11, bt_line_price_domestic.Get(), format_table_number_d2);

					if(bt_line_price_foreign.Get())
					{
						__xls_sheet->writeNum(__xls_row_counter, 12, bt_line_price_foreign.Get(), format_table_number_d2);
						__xls_sheet->writeStr(__xls_row_counter, 13, bt_line_price_rate_exch.c_str(), format_title);
					}
					else
					{
						__xls_sheet->writeBlank(__xls_row_counter, 12, format_table_number_d2);
						__xls_sheet->writeBlank(__xls_row_counter, 13, format_title);
					}

					__xls_row_counter++;
				}

				// --- table summary
				{
					__xls_row_counter++;
				}

				{
					__XLS_print_formatted_footer_line(GetSpelledSumNonTaxable(), bt.GetSumNonTaxable());

					__xls_row_counter++;
				}

				{
					__XLS_print_formatted_footer_line(GetSpelledSumTaxable(), bt.GetSumTaxable());

					__xls_row_counter++;
				}

				{
					__XLS_print_formatted_footer_line(GetSpelledSumTax(), bt.GetSumTax());

					__xls_row_counter++;
				}

				{
					auto		temp = c_float(GetBTSumExpenses(index));
					__XLS_print_formatted_footer_line(GetSpelledSumExpense(), temp);

					__xls_row_counter++;
				}

				{
					auto		temp = c_float(GetBTMarkup(index));

					// --- if markup defined
					if(temp.Get())
					{
						__XLS_print_formatted_footer_line(GetSpelledMarkup(), temp);
					}

					__xls_row_counter++;
				}

				{
					auto		temp = c_float(GetBTPaymentNoVAT(index));
					__XLS_print_formatted_footer_line(GetSpelledTotalPaymentNoVAT(), temp);

					__xls_row_counter++;
				}

				{
					auto		temp = c_float(GetBTVAT(index));
					__XLS_print_formatted_footer_line(GetSpelledVAT(), temp);

					__xls_row_counter++;
				}

				{
					auto		temp = c_float(GetBTPaymentAndVAT(index));
					__XLS_print_formatted_footer_line(GetSpelledTotalPayment(), temp);

					__xls_row_counter++;
				}

				__xls_row_counter++;
				// --- approvers block
				{
					__xls_sheet->writeStr(__xls_row_counter, 1, multibyte_to_wide(GetSpelledApprovers()).c_str());
					__xls_sheet->writeStr(__xls_row_counter, 6, multibyte_to_wide(bt.GetApprovers()).c_str());

					__xls_row_counter++;
				}


				__xls_row_counter++;
				__xls_row_counter++;

				// --- signature block
				{
					if(GetSignRole1().length()) __xls_sheet->writeStr(__xls_row_counter, 1, multibyte_to_wide(GetSignatureTitle1()).c_str());
					if(GetSignRole2().length()) __xls_sheet->writeStr(__xls_row_counter, 9, multibyte_to_wide(GetSignatureTitle2()).c_str());

					__xls_row_counter++;
					__xls_row_counter++;
				}

				{
					if(GetSignRole1().length())
					{
											__xls_sheet->writeStr(__xls_row_counter, 1, spelled_signature.c_str());
											__XLS_DrawUnderline(2, 6);
					}
					if(GetSignRole2().length())
					{
											__xls_sheet->writeStr(__xls_row_counter, 9, spelled_signature.c_str());
											__XLS_DrawUnderline(10, 13);
					}

					__xls_row_counter++;
					__xls_row_counter++;
				}

				// --- initials block
				{
					if(GetSignRole1().length())
					{
											__xls_sheet->writeStr(__xls_row_counter,  1, spelled_initials.c_str());
											__xls_sheet->writeStr(__xls_row_counter,  3, multibyte_to_wide(bt.GetInitials1()).c_str());
											__XLS_DrawUnderline(2, 6);
					}
					if(GetSignRole2().length())
					{
											__xls_sheet->writeStr(__xls_row_counter,  9, spelled_initials.c_str());
											__xls_sheet->writeStr(__xls_row_counter, 11, multibyte_to_wide(bt.GetInitials2()).c_str());
											__XLS_DrawUnderline(10, 13);
					}

					__xls_row_counter++;
					__xls_row_counter++;
				}

				// --- position block
				{
					if(GetSignRole1().length())
					{
											__xls_sheet->writeStr(__xls_row_counter,  1, spelled_position.c_str());
											__xls_sheet->writeStr(__xls_row_counter,  3, multibyte_to_wide(bt.GetPosition1()).c_str());
											__XLS_DrawUnderline(2, 6);
					}
					if(GetSignRole2().length())
					{
											__xls_sheet->writeStr(__xls_row_counter,  9, spelled_position.c_str());
											__xls_sheet->writeStr(__xls_row_counter, 11, multibyte_to_wide(bt.GetPosition2()).c_str());
											__XLS_DrawUnderline(10, 13);
					}

					__xls_row_counter++;
					__xls_row_counter++;
				}

				// --- date block
				{
					if(GetSignRole1().length()) 
					{
											__xls_sheet->writeStr(__xls_row_counter, 1, spelled_date.c_str());
											__XLS_DrawUnderline(2, 6);
					}
					if(GetSignRole2().length()) 
					{
											__xls_sheet->writeStr(__xls_row_counter, 9, spelled_date.c_str());
											__XLS_DrawUnderline(10, 13);
					}

					__xls_row_counter++;
					__xls_row_counter++;
				}
			}

			if(__xls_book->save(multibyte_to_wide(GetFilename()).c_str())) {}
			else
			{
				MESSAGE_ERROR("", "", "can't save " + filename + ".xls")
			}
			__xls_book->release();
		} 
	}
	else
	{
		error_message = "vars obj is null";
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

// --- PDF part
auto	C_Print_BT::__HPDF_DrawHeader() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledTitle()), 0, 100, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE + 5, false)).length())
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
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledPSoW()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledPurchaseOrder()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledProjectID()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if(GetSpelledLocation().length())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledLocation()), 0, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
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
		error_message = gettext("hpdf: fail to print title");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTable() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			pdf_obj.AddColumn(10);
			pdf_obj.AddColumn(45);
			pdf_obj.AddColumn(15);
			pdf_obj.AddColumn(15);
			pdf_obj.AddColumn(15);

			if((error_message = pdf_obj.__HPDF_StartTable()).length())
			{
				MESSAGE_ERROR("", "", "fail to start table");
			}
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(0, utf8_to_cp1251(GetTitleDate()), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title index line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(GetTitleExpense()), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title description line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(2, utf8_to_cp1251(GetTitleSumRur()), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title quantity line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(3, utf8_to_cp1251(GetTitleSumCurrency()), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title items line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(4, utf8_to_cp1251(GetTitleRateExchange()), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title price line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveTableLineDown(1)).length())
			{ MESSAGE_ERROR("", "", "fail to move table line down"); }
		}

		if(error_message.empty())
		{
			auto			bt_expenses = bt.GetExpenseLines();

			for(auto &bt_expense: bt_expenses)
			{
				auto	max_lines = pdf_obj.__HPDF_GetNumberOfLinesInTable(1, utf8_to_cp1251(bt_expense.description), NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE);

				if(error_message.empty())
				{
					if((error_message = pdf_obj.__HPDF_PrintTextTableCell(0, utf8_to_cp1251(GetSpellingFormattedDate(bt_expense.date, "%d.%m.%Y")), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
					{ MESSAGE_ERROR("", "", "fail to write table date"); }
				}
				if(error_message.empty())
				{
					if((error_message = pdf_obj.__HPDF_PrintTextTableCell(1, utf8_to_cp1251(bt_expense.description), HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
					{ MESSAGE_ERROR("", "", "fail to write table description"); }
				}
				if(error_message.empty())
				{
					if((error_message = pdf_obj.__HPDF_PrintTextTableCell(2, utf8_to_cp1251(bt_expense.price_domestic), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
					{ MESSAGE_ERROR("", "", "fail to write table price_domestic"); }
				}
				if(c_float(bt_expense.price_foreign).Get())
				{
					if(error_message.empty())
					{
						if((error_message = pdf_obj.__HPDF_PrintTextTableCell(3, utf8_to_cp1251(bt_expense.price_foreign), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
						{ MESSAGE_ERROR("", "", "fail to write table price_foreign"); }
					}
					if(error_message.empty())
					{
						if((error_message = pdf_obj.__HPDF_PrintTextTableCell(4, utf8_to_cp1251(bt_expense.currency_nominal + TIMECARD_ENTRY_TITLE_SEPARATOR + bt_expense.currency_value), HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
						{ MESSAGE_ERROR("", "", "fail to write table currency_nominal"); }
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", "no need to print price_foreign and rate exchange");
				}
				if(error_message.empty())
				{
					if((error_message = pdf_obj.__HPDF_MoveTableLineDown(max_lines)).length())
					{ MESSAGE_ERROR("", "", "fail to move table line down " + to_string(max_lines) + " line(s)"); }
				}

				if(error_message.length()) break;
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

auto	C_Print_BT::__HPDF_DrawFooter() -> string
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
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledSumNonTaxable(), bt.GetSumNonTaxable())).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}
		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledSumTaxable(), bt.GetSumTaxable())).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}
		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledSumTax(), bt.GetSumTax())).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}
		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledSumExpense(), GetBTSumExpenses(index))).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}

		{
			auto		temp = c_float(GetBTMarkup(index));

			if(temp)
			{
				if(error_message.empty())
				{
					if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledMarkup(), temp)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
				}
			}
		}

		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledTotalPaymentNoVAT(), GetBTPaymentNoVAT(index))).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}
		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledVAT(), GetBTVAT(index))).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}
		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawFooter_SingleLine(GetSpelledTotalPayment(), GetBTPaymentAndVAT(index))).empty()) {}
			else { MESSAGE_ERROR("", "", "fail returned from __HPDF_DrawFooter_SingleLine"); }
		}

		if(error_message.empty())
		{
			if((error_message = __HPDF_PrintApprovers()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print pdf approvers"); }
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

auto C_Print_BT::__HPDF_PrintApprovers() -> string
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
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledApprovers()), 0, 25, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(bt.GetApprovers()), 25, 100, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
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

auto C_Print_BT::__HPDF_PrintSignature() -> string
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
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureTitle1()), 0, 20, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureTitle2()), 60, 60+20, HPDF_TALIGN_LEFT, BOLD_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		// --- signature
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledSignature()), 0, 40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledSignature()), 60, 60+40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(10, 40)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(70, 100)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}

		// --- initials
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledInitials()), 0, 40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledInitials()), 60, 60+40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(bt.GetInitials1()), 10, 40, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			// --- don't move to the next line because text could be empty
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(bt.GetInitials2()), 70, 100, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

// TODO: clean-up initials
/*		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledInitials("1")), 10, 40, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledInitials("2")), 70, 100, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
*/
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(10, 40)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(70, 100)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}

		// --- position
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledPosition()), 0, 40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledPosition()), 60, 60+40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(bt.GetPosition1()), 10, 40, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(bt.GetPosition2()), 70, 100, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

// TODO: clean-up position
/*
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledPosition("1")), 10, 40, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledPosition("2")), 70, 100, HPDF_TALIGN_CENTER, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length()) // --- print from variable
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
*/
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(10, 40)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(70, 100)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}

		// --- date
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledDate()), 0, 40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledDate()), 60, 60+40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty() && GetSignRole1().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(10, 40)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty() && GetSignRole2().length())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(70, 100)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
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


auto	C_Print_BT::__HPDF_DrawFooter_SingleLine(string text, c_float number) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(text), 0, 25, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(to_string(number.GetWhole()), 25, 30, HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledRur()), 31, 35, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(to_string(number.GetFraction()), 30, 35, HPDF_TALIGN_RIGHT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSpelledKop()), 36, 40, HPDF_TALIGN_LEFT, NORMAL_FONT, HPDF_TIMECARD_FONT_SIZE, true)).length())
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

auto	C_Print_BT::PrintAsPDF() -> string
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

ostream& operator<<(ostream& os, const C_Print_BT &var)
{
	os << "object C_Print_BT [empty for now]";

	return os;
}

