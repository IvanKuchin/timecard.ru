#include "c_print_bt.h"

static void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	char	buffer[512];

    snprintf(buffer, sizeof(buffer) - 1, "error_no=%04X, detail_no=%d\n", (unsigned int) error_no, (int) detail_no);
    MESSAGE_ERROR("", "", "libhpdf: " +  buffer);
    throw std::exception (); /* throw exception on error */
}

C_Print_BT::C_Print_BT()
{
	MESSAGE_DEBUG("", "", "start");

	__pdf = HPDF_New(error_handler, NULL);

	if(__pdf)
	{
		try
		{
			// __pdf_font_name =  HPDF_LoadType1FontFromFile (__pdf, (PDF_FONT_INSTALL_DIR + "/type1/a010013l.afm").c_str(), (PDF_FONT_INSTALL_DIR + "/type1/a010013l.pfb").c_str());
			__pdf_font_name = HPDF_LoadTTFontFromFile (__pdf, (PDF_FONT_INSTALL_DIR + "/ttf/arial.ttf").c_str(), HPDF_TRUE);
			// HPDF_UseUTFEncodings (__pdf);
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

auto C_Print_BT::SetBT(const C_BT_To_Print &param1) -> void
{
	bt = param1;
	effort_hours = 0;
	__pdf_line = -1;
	effort_hours = 0;
	effort_days = 0;
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

auto	C_Print_BT::GetSpelledProjectID() -> string
{
	return bt.GetProjectNumber().length() ? vars->Get("Project ID") + ": "s + bt.GetProjectNumber() : "";
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
		auto	spelled_sum_taxable			= multibyte_to_wide(GetSpelledSumTaxable());
		auto	spelled_sum_tax				= multibyte_to_wide(GetSpelledSumTax());
		auto	spelled_sum_non_taxable		= multibyte_to_wide(GetSpelledSumNonTaxable());
		auto	spelled_sum_expense			= multibyte_to_wide(GetSpelledSumExpense());
		auto	spelled_markup				= multibyte_to_wide(GetSpelledMarkup());
		auto	spelled_totalpayment		= multibyte_to_wide(GetSpelledTotalPayment());
		auto	spelled_vat					= multibyte_to_wide(GetSpelledVAT());
		auto	spelled_totalpaymentnovat	= multibyte_to_wide(GetSpelledTotalPaymentNoVAT());
		auto	spelled_signature			= multibyte_to_wide(GetSpelledSignature());
		auto	spelled_initials			= multibyte_to_wide(GetSpelledInitials());
		auto	spelled_position			= multibyte_to_wide(GetSpelledPosition());
		auto	spelled_date				= multibyte_to_wide(GetSpelledDate());
		auto	spelled_rur					= multibyte_to_wide(GetSpelledRur());
		auto	spelled_kop					= multibyte_to_wide(GetSpelledKop());
		auto	index						= vars->GetIndexByBTID(bt.GetID());

		libxl::Book* book = xlCreateBook();
		if(book)
		{
			libxl::Sheet* sheet = book->addSheet(L"Sheet1");
			if(sheet)
			{
				auto			format_title = book->addFormat();
				auto			format_weekend = book->addFormat();
				auto			format_table_number_d2 = book->addFormat();
				auto			format_summary_over = book->addFormat();
				auto			format_summary_right = book->addFormat();
				auto			format_number_d2 = book->addFormat();
				auto			font_summary_over = book->addFont();
				auto			font_summary_right = book->addFont();
				auto			start_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(bt.GetDateStart(), "%d %b %G"));
				auto			finish_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(bt.GetDateFinish(), "%d %b %G"));
				auto			bt_expenses = bt.GetExpenseLines();
				auto			row_counter = 1;

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

				// --- print proerties
				sheet->setPaper(libxl::PAPER_A4);
				sheet->setLandscape();
				sheet->setPrintZoom(75);

				// --- set columns width
				sheet->setCol(2, 2, 60);
				sheet->setCol(3, 5, 16);

				row_counter++;
				sheet->writeStr(row_counter, 2, spelled_title.c_str());

				row_counter++;
				row_counter++;
				sheet->writeStr(row_counter, 1, spelled_psow.c_str());

				row_counter++;
				sheet->writeStr(row_counter, 1, spelled_projectid.c_str());

				if(GetSpelledLocation().length())
				{
					row_counter++;
					sheet->writeStr(row_counter, 1, multibyte_to_wide(GetSpelledLocation()).c_str());
				}


				// --- table header
				{
					row_counter++;
					row_counter++;
					sheet->writeStr(row_counter, 1, multibyte_to_wide(GetTitleDate()).c_str()			, format_title);
					sheet->writeStr(row_counter, 2, multibyte_to_wide(GetTitleExpense()).c_str()		, format_title);
					sheet->writeStr(row_counter, 3, multibyte_to_wide(GetTitleSumRur()).c_str()			, format_title);
					sheet->writeStr(row_counter, 4, multibyte_to_wide(GetTitleSumCurrency()).c_str()	, format_title);
					sheet->writeStr(row_counter, 5, multibyte_to_wide(GetTitleRateExchange()).c_str()	, format_title);
				}

				// --- table body
				row_counter++;
				for(auto &bt_expense: bt_expenses)
				{
					wstring		bt_line_date = multibyte_to_wide(bt_expense.date);
					wstring		bt_line_description = multibyte_to_wide(bt_expense.description);
					c_float		bt_line_price_domestic(bt_expense.price_domestic);
					c_float		bt_line_price_foreign(bt_expense.price_foreign);
					wstring		bt_line_price_rate_exch = multibyte_to_wide(bt_expense.currency_nominal + " / " + bt_expense.currency_value);

					sheet->writeStr(row_counter, 1, bt_line_date.c_str(), format_title);
					sheet->writeStr(row_counter, 2, bt_line_description.c_str(), format_title);
					sheet->writeNum(row_counter, 3, bt_line_price_domestic.Get(), format_table_number_d2);

					if(bt_line_price_foreign.Get())
					{
						sheet->writeNum(row_counter, 4, bt_line_price_foreign.Get(), format_table_number_d2);
						sheet->writeStr(row_counter, 5, bt_line_price_rate_exch.c_str(), format_title);
					}
					else
					{
						sheet->writeBlank(row_counter, 4, format_table_number_d2);
						sheet->writeBlank(row_counter, 5, format_title);
					}

					row_counter++;
				}

				// --- table summary
				{
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_sum_non_taxable.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, double(bt.GetSumNonTaxable()), format_number_d2);
					
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_sum_taxable.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, double(bt.GetSumTaxable()), format_number_d2);

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_sum_tax.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, double(bt.GetSumTax()), format_number_d2);

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_sum_expense.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, c_float(GetBTSumExpenses(index)).Get(), format_number_d2);

					row_counter++;
				}

				{
					auto		temp = c_float(GetBTMarkup(index));

					// --- if markup defined
					if(temp.Get())
					{
						sheet->writeStr(row_counter, 1, spelled_markup.c_str());
						// sheet->setMerge(row_counter, row_counter, 3, 5);
						sheet->writeNum(row_counter, 3, temp.GetWhole());
						sheet->writeStr(row_counter, 6, spelled_rur.c_str());
						sheet->writeNum(row_counter, 7, temp.GetFraction());
						sheet->writeStr(row_counter, 8, spelled_kop.c_str());
					}

					row_counter++;
				}

				{
					auto		temp = c_float(GetBTPaymentNoVAT(index));

					sheet->writeStr(row_counter, 1, spelled_totalpaymentnovat.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, temp.GetWhole());
					sheet->writeStr(row_counter, 6, spelled_rur.c_str());
					sheet->writeNum(row_counter, 7, temp.GetFraction());
					sheet->writeStr(row_counter, 8, spelled_kop.c_str());
					
					row_counter++;
				}

				{
					auto		temp = c_float(GetBTVAT(index));

					sheet->writeStr(row_counter, 1, spelled_vat.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, temp.GetWhole());
					sheet->writeStr(row_counter, 6, spelled_rur.c_str());
					sheet->writeNum(row_counter, 7, temp.GetFraction());
					sheet->writeStr(row_counter, 8, spelled_kop.c_str());
					
					row_counter++;
				}

				{
					auto		temp = c_float(GetBTPaymentAndVAT(index));

					sheet->writeStr(row_counter, 1, spelled_totalpayment.c_str());
					// sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, temp.GetWhole());
					sheet->writeStr(row_counter, 6, spelled_rur.c_str());
					sheet->writeNum(row_counter, 7, temp.GetFraction());
					sheet->writeStr(row_counter, 8, spelled_kop.c_str());
					
					row_counter++;
				}

				row_counter++;
				row_counter++;

				{
					sheet->writeStr(row_counter, 1, multibyte_to_wide(bt.GetSignatureTitle1()).c_str());
					sheet->writeStr(row_counter, 4, multibyte_to_wide(bt.GetSignatureTitle2()).c_str());

					row_counter++;
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_signature.c_str());
					sheet->writeStr(row_counter, 4, spelled_signature.c_str());

					row_counter++;
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_initials.c_str());
					sheet->writeStr(row_counter, 4, spelled_initials.c_str());

					row_counter++;
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_position.c_str());
					sheet->writeStr(row_counter, 4, spelled_position.c_str());

					row_counter++;
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, spelled_date.c_str());
					sheet->writeStr(row_counter, 4, spelled_date.c_str());

					row_counter++;
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
		error_message = "vars obj is null";
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

/*
auto	C_Print_BT::__HPDF_init() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(__pdf)
	{
		MESSAGE_DEBUG("", "", "hpdf object created");
	}
	else
	{
		MESSAGE_DEBUG("", "", "hpdf object has not been created");
		error_message = gettext("fail to initialize hpdf library");
	}

	MESSAGE_DEBUG("", "", "end (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Print_BT::__HPDF_SetDocProps() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");
	try
	{
		HPDF_NewDoc (__pdf);;

		HPDF_SetCompressionMode (__pdf, HPDF_COMP_ALL);

		HPDF_SetPageMode (__pdf, HPDF_PAGE_MODE_USE_OUTLINE);

		__pdf_line = -1;
	    __pdf_font = HPDF_GetFont(__pdf, __pdf_font_name.c_str(), "CP1251");

	    __pdf_font_descent = HPDF_Font_GetDescent(__pdf_font) * HPDF_TIMECARD_FONT_SIZE / 1000;
	    __pdf_font_ascent = HPDF_Font_GetAscent(__pdf_font) * HPDF_TIMECARD_FONT_SIZE / 1000;
	    __pdf_font_xheight = HPDF_Font_GetXHeight(__pdf_font) * HPDF_TIMECARD_FONT_SIZE / 1000;

	    if(__pdf_font_descent && __pdf_font_ascent && __pdf_font_xheight)
	    {
		    MESSAGE_DEBUG("", "", "hpdf: font ascent is " + to_string(__pdf_font_ascent));
		    MESSAGE_DEBUG("", "", "hpdf: font xheight is " + to_string(__pdf_font_xheight));
		    MESSAGE_DEBUG("", "", "hpdf: font descent is " + to_string(__pdf_font_descent));

		    __pdf_font_height = abs(__pdf_font_descent) + abs(__pdf_font_ascent) + abs(__pdf_font_xheight);
	    }
	    else
	    {
	    	auto	__rect = HPDF_Font_GetBBox(__pdf_font);

		    MESSAGE_DEBUG("", "", "hpdf: font bbox left " + to_string(__rect.left));
		    MESSAGE_DEBUG("", "", "hpdf: font bbox top " + to_string(__rect.top));
		    MESSAGE_DEBUG("", "", "hpdf: font bbox right " + to_string(__rect.right));
		    MESSAGE_DEBUG("", "", "hpdf: font bbox bottom " + to_string(__rect.bottom));
		    
		    __pdf_font_height = (abs(__rect.top) - abs(__rect.bottom)) * HPDF_TIMECARD_FONT_SIZE / 1000;
	    }

	    // __pdf_font_height += 2;
	    __pdf_table_line_height = __pdf_font_height + HPDF_TIMECARD_TABLE_SPACING;

	    MESSAGE_DEBUG("", "", "hpdf: font height is " + to_string(__pdf_font_height));
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "HPDF trown exception");		
	}


	MESSAGE_DEBUG("", "", "end (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;	
}

auto	C_Print_BT::__HPDF_MoveLineDown(int line_decrement) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	__pdf_line -= line_decrement;

	if(__pdf_line < HPDF_FIELD_BOTTOM)
	{
		MESSAGE_DEBUG("", "", "new page adding");

		try
		{
			__pdf_page = HPDF_AddPage(__pdf);
		    HPDF_Page_SetSize(__pdf_page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);
			__pdf_line = HPDF_Page_GetHeight(__pdf_page);
			__pdf_line -= HPDF_FIELD_TOP;

			__pdf_page_width = HPDF_Page_GetWidth(__pdf_page);
			__pdf_page_height = HPDF_Page_GetHeight(__pdf_page);

			MESSAGE_DEBUG("", "", "new page size width = " + to_string(__pdf_page_width) + ", height = " + to_string(__pdf_page_height));
		}
		catch(...)
		{
			MESSAGE_ERROR("", "", "hpdf: fail to add page");
			error_message = gettext("hpdf: fail to add page");
		}

	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_StartTable() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	__pdf_table_top = __pdf_line;

	if((error_message = __HPDF_DrawTimecardHorizontalLine()).empty())
	{
		if((error_message = __HPDF_MoveTableLineDown()).empty())
		{
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to move table line down");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw timecard horizontal line");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_GetTimecardTableXByPercentage(double percent) -> double
{
	MESSAGE_DEBUG("", "", "start");
	MESSAGE_DEBUG("", "", "HPDF_TIMECARD_DAY_WIDTH_PERCENT = "s + to_string(HPDF_TIMECARD_DAY_WIDTH_PERCENT));

	auto	workpane_width = __pdf_page_width - HPDF_FIELD_RIGHT - HPDF_FIELD_LEFT;
	auto	result = percent / 100 * workpane_width + HPDF_FIELD_LEFT;
	
	MESSAGE_DEBUG("", "", "finish(" + to_string(percent) + "% = " + to_string(result) + " px.)");

	return result;
}


auto	C_Print_BT::__HPDF_StopTable() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	__pdf_table_bottom = __pdf_line;

	if(__pdf_table_top > __pdf_table_bottom)
	{
		// --- Draw vertical lines
		// ---- vertical line: table left side
		if((error_message = __HPDF_DrawTimecardVerticalLine(HPDF_FIELD_LEFT)).empty())
		{
			auto	number_of_days = GetNumberOfDaysInTimecard();

			if(number_of_days)
			{
				for(auto i = 0; i <= number_of_days; ++i)
				{
					// ---- vertical lines: days
					if((error_message = __HPDF_DrawTimecardVerticalLine(__HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + HPDF_TIMECARD_DAY_WIDTH_PERCENT * i))).length())
					{
						MESSAGE_ERROR("", "", "fail to draw timecard vertical line");
						break;
					}
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: timecard have no days in it (must be either 7 or month)");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to draw timecard vertical line");
		}
		// ---- vertical line: table right side

	}
	else
	{
		MESSAGE_ERROR("", "", "hpdf: table top(" + to_string(__pdf_table_top) + ") must be higher than bottom(" + to_string(__pdf_table_bottom) + ")");
		error_message = gettext("hpdf: table top must be higher than bottom");
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_MoveTableLineDown(int line_decrement) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");


	if((__pdf_line - line_decrement) < HPDF_FIELD_BOTTOM)
	{
		MESSAGE_DEBUG("", "", "change page in pdf");

		if((error_message = __HPDF_StopTable()).empty())
		{
			if((error_message = __HPDF_MoveLineDown(line_decrement)).empty())
			{
				if((error_message = __HPDF_StartTable()).empty())
				{

				}
				else
				{
					MESSAGE_ERROR("", "", "hpdf: fail to start table");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to move line down");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to stop table");
		}
	}
	else
	{
		if((error_message = __HPDF_MoveLineDown(line_decrement)).empty())
		{
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to stop table");
		}
	}

	__HPDF_DrawTimecardHorizontalLine();

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_PrintText(string text, double x, HPDF_TextAlignment text_align = HPDF_TALIGN_LEFT) -> string
{
	return __HPDF_PrintText(text, x, __pdf_page_width - HPDF_FIELD_RIGHT, text_align);
}

auto	C_Print_BT::__HPDF_PrintText(string text, double x1, double x2, HPDF_TextAlignment text_align = HPDF_TALIGN_LEFT) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_Page_BeginText (__pdf_page);
		HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
		HPDF_Page_TextRect (__pdf_page, x1, __pdf_line + __pdf_font_height, x2, __pdf_line, (utf8_to_cp1251(text)).c_str(), text_align, NULL);
		HPDF_Page_EndText (__pdf_page);
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to print timecard title");
		error_message = gettext("hpdf: fail to print text");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}


auto	C_Print_BT::__HPDF_TableGetXOffset() -> HPDF_REAL
{
	HPDF_REAL	result = 0;

	MESSAGE_DEBUG("", "", "start");

	result = HPDF_FIELD_LEFT;

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_BT::__HPDF_TableGetTitleWidth() -> HPDF_REAL
{
	HPDF_REAL	result = 0;

	MESSAGE_DEBUG("", "", "start");

	result = (__pdf_page_width - HPDF_FIELD_LEFT - HPDF_FIELD_RIGHT) * HPDF_TIMECARD_TITLE_WIDTH_PERCENT / 100;

	MESSAGE_DEBUG("", "", "finish");

	return result;
}
auto	C_Print_BT::__HPDF_TableDaysGetXOffset() -> HPDF_REAL
{
	HPDF_REAL	result = 0;

	MESSAGE_DEBUG("", "", "start");

	result = HPDF_FIELD_LEFT + __HPDF_TableGetTitleWidth();

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_BT::GetNumberOfDaysInTimecard() -> int
{
	auto	result = 0;
	auto	bt_expenses = bt.GetExpenseLines();

	MESSAGE_DEBUG("", "", "start");

	if(bt_expenses.size())
	{
		result = SplitTimeentry(bt_expenses[0].hours).size();
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard object have no time_entries");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto	C_Print_BT::__HPDF_DrawTimecardHorizontalLine() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	line_length = __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + HPDF_TIMECARD_DAY_WIDTH_PERCENT * GetNumberOfDaysInTimecard());

	try
	{
		HPDF_Page_MoveTo (__pdf_page, HPDF_FIELD_LEFT, __pdf_line);
		HPDF_Page_LineTo (__pdf_page, line_length, __pdf_line);
		HPDF_Page_Stroke (__pdf_page);
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw the line");
		error_message = gettext("hpdf: fail to draw the line");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardVerticalLine(double x) -> string
{
	MESSAGE_DEBUG("", "", "start (x=" + to_string(x) + ")");

	auto	error_message = ""s;

	try
	{
		HPDF_Page_MoveTo (__pdf_page, x, __pdf_table_top);
		HPDF_Page_LineTo (__pdf_page, x, __pdf_table_bottom);
		HPDF_Page_Stroke (__pdf_page);
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw the line");
		error_message = gettext("hpdf: fail to draw the line");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_PaintDay(int day_number, double red, double green, double blue, int number_of_lines) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		auto	rect_left = __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + HPDF_TIMECARD_DAY_WIDTH_PERCENT * day_number);
		auto	rect_width = __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + HPDF_TIMECARD_DAY_WIDTH_PERCENT * (day_number + 1)) - rect_left;
		auto	rect_bottom = __pdf_line;
		auto	rect_height = __pdf_table_line_height * number_of_lines;

		HPDF_Page_SetRGBFill(__pdf_page, red, green, blue);
		HPDF_Page_Rectangle (__pdf_page,
							rect_left, rect_bottom, 
							rect_width, rect_height
							);
		// HPDF_Page_Fill(__pdf_page);
		HPDF_Page_FillStroke(__pdf_page);
		HPDF_Page_SetRGBFill(__pdf_page, 0, 0, 0);
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw timecard table header");
		error_message = gettext("hpdf: fail to draw timecard table header");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardTitle() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		if((error_message = __HPDF_MoveLineDown()).empty())
		{
			HPDF_Page_BeginText (__pdf_page);
			HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
			HPDF_Page_TextRect (__pdf_page, __HPDF_TableDaysGetXOffset(), __pdf_line + __pdf_font_height, __pdf_page_width - HPDF_FIELD_RIGHT, __pdf_line, (utf8_to_cp1251(GetSpelledTitle())).c_str(), HPDF_TALIGN_LEFT, NULL);
			HPDF_Page_EndText (__pdf_page);

			if((error_message = __HPDF_MoveLineDown()).empty())
			{
				HPDF_Page_BeginText (__pdf_page);
				HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
				HPDF_Page_TextRect (__pdf_page, __HPDF_TableGetXOffset(), __pdf_line + __pdf_font_height, __pdf_page_width - __HPDF_TableGetXOffset() - HPDF_FIELD_RIGHT, __pdf_line, (utf8_to_cp1251(GetSpelledPSoW())).c_str(), HPDF_TALIGN_LEFT, NULL);
				HPDF_Page_EndText (__pdf_page);

				if((error_message = __HPDF_MoveLineDown()).empty())
				{
					HPDF_Page_BeginText (__pdf_page);
					HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
					HPDF_Page_TextRect (__pdf_page, __HPDF_TableGetXOffset(), __pdf_line + __pdf_font_height, __pdf_page_width - __HPDF_TableGetXOffset() - HPDF_FIELD_RIGHT, __pdf_line, (utf8_to_cp1251(GetSpelledProjectID())).c_str(), HPDF_TALIGN_LEFT, NULL);
					HPDF_Page_EndText (__pdf_page);
				}
				else
				{
					MESSAGE_ERROR("", "", "hpdf: fail to move line down");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to move line down");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to move line down");
		}

	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to print timecard title");
		error_message = gettext("hpdf: fail to print timecard title");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardTableHeader() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{

			auto	number_of_days = GetNumberOfDaysInTimecard();

			if(number_of_days)
			{
				auto		percentage = HPDF_TIMECARD_TITLE_WIDTH_PERCENT;

				for(auto i = 0; i < number_of_days; ++i)
				{
					HPDF_Page_BeginText (__pdf_page);
					HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
					HPDF_Page_TextRect (__pdf_page, 
										__HPDF_GetTimecardTableXByPercentage(percentage), 
										__pdf_line + __pdf_table_line_height, 
										__HPDF_GetTimecardTableXByPercentage(percentage + HPDF_TIMECARD_DAY_WIDTH_PERCENT), 
										__pdf_line, 
										"", 
										HPDF_TALIGN_CENTER, 
										NULL);
					HPDF_Page_EndText (__pdf_page);

					percentage += HPDF_TIMECARD_DAY_WIDTH_PERCENT;
				}
				if((error_message = __HPDF_DrawTimecardHorizontalLine()).empty())
				{
				}
				else
				{
					MESSAGE_ERROR("", "", "hpdf: fail to draw table horizontal line");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: timecard have no days in it (must be either 7 or month)");
			}
		
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw timecard table header");
		error_message = gettext("hpdf: fail to draw timecard table header");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardTableBody() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		auto	number_of_days = GetNumberOfDaysInTimecard();

		if(number_of_days)
		{
			auto	bt_expenses = bt.GetExpenseLines();

			for(auto &bt_expense : bt_expenses)
			{
				auto	time_entries			= SplitTimeentry(bt_expense.price_domestic);
				auto	bt_line_description		= bt_expense.description + TIMECARD_ENTRY_TITLE_SEPARATOR + bt_expense.project + TIMECARD_ENTRY_TITLE_SEPARATOR + bt_expense.task;
				auto	text_width_pdf_struct	= HPDF_Font_TextWidth(__pdf_font, (HPDF_BYTE *)utf8_to_cp1251(bt_line_description).c_str(), utf8_to_cp1251(bt_line_description).length());
				auto	text_width				= text_width_pdf_struct.width * HPDF_TIMECARD_FONT_SIZE / 1000.0;
				auto	title_cell_width		= __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT);
				auto	number_of_lines			= ceil(HPDF_TIMECARD_FONT_MULTIPLIER * text_width / title_cell_width);

				if(number_of_lines) {}
				else
				{
					MESSAGE_ERROR("", "", "number of lines reuired to write cu / proj / task is 0 (probably string is empty)");
					number_of_lines = 1;
				}

				MESSAGE_DEBUG("", "", "text width is " + to_string(text_width) + ", reserved width " + to_string(title_cell_width) + ", text will fit to " + to_string(number_of_lines) + " number of lines");

				if((error_message = __HPDF_MoveTableLineDown(__pdf_table_line_height * number_of_lines)).length())
				{
					MESSAGE_DEBUG("", "", "hpdf: fail to move line down");
					break;
				}

				HPDF_Page_BeginText (__pdf_page);
				HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
				HPDF_Page_TextRect (__pdf_page, 
									__HPDF_GetTimecardTableXByPercentage(0) + 1, 
									__pdf_line + __pdf_table_line_height * number_of_lines - 1, 
									__HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT) - 1, 
									__pdf_line + 1, 
									utf8_to_cp1251(bt_line_description).c_str(), 
									HPDF_TALIGN_LEFT, 
									NULL);
				HPDF_Page_EndText (__pdf_page);



				if(time_entries.size())
				{
						auto	percentage = HPDF_TIMECARD_TITLE_WIDTH_PERCENT;

						for(auto i = 0; i < number_of_days; ++i)
						{
							auto	time_entry = time_entries[i];

							if(time_entry.length() && (time_entry != "0"))
							{
								HPDF_Page_BeginText (__pdf_page);
								HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE - 3);
								HPDF_Page_TextRect (__pdf_page, 
													__HPDF_GetTimecardTableXByPercentage(percentage), 
													__pdf_line + __pdf_table_line_height * number_of_lines - 2, 
													__HPDF_GetTimecardTableXByPercentage(percentage + HPDF_TIMECARD_DAY_WIDTH_PERCENT), 
													__pdf_line, 
													(time_entries[i]).c_str(), 
													HPDF_TALIGN_CENTER, 
													NULL);
								HPDF_Page_EndText (__pdf_page);
							}

							percentage += HPDF_TIMECARD_DAY_WIDTH_PERCENT;
						}
				}
				else
				{
					MESSAGE_ERROR("", "", "time report(" + bt_expense.price_domestic + ") have no entries. This flow expect to have time entries reported.");
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: timecard have no days in it (must be either 7 or month)");
		}

	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw timecard table body");
		error_message = gettext("hpdf: fail to draw timecard table body");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardTableFooter() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
			auto	number_of_days = GetNumberOfDaysInTimecard();

			if(number_of_days)
			{
				auto		percentage = HPDF_TIMECARD_TITLE_WIDTH_PERCENT;

				if((error_message = __HPDF_MoveTableLineDown()).length())
				{
					MESSAGE_DEBUG("", "", "hpdf: fail to move line down");
				}

				for(auto i = 0; i < number_of_days; ++i)
				{
					percentage += HPDF_TIMECARD_DAY_WIDTH_PERCENT;
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: timecard have no days in it (must be either 7 or month)");
			}
		
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw timecard table header");
		error_message = gettext("hpdf: fail to draw timecard table header");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardTable() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if((error_message = __HPDF_StartTable()).empty())
		{
			if((error_message = __HPDF_DrawTimecardTableHeader()).empty())
			{
				if((error_message = __HPDF_DrawTimecardTableBody()).empty())
				{
					if((error_message = __HPDF_DrawTimecardTableFooter()).empty())
					{
						if((error_message = __HPDF_StopTable()).empty())
						{
						}
						else
						{
							MESSAGE_ERROR("", "", "hpdf: fail to stop timecard table");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "hpdf: fail to draw table footer");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "hpdf: fail to table body");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to draw table header");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to start timecard table");
		}
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to print timecard title");
		error_message = gettext("hpdf: fail to print timecard title");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_DrawTimecardFooter() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	MESSAGE_DEBUG("", "", "dayrate " + to_string(bt.GetDayrate()) + " kop. " + to_string(bt.GetDayrate().GetFraction()) + ", long kop. " + to_string(long(bt.GetDayrate().GetFraction())));

	try
	{
		if((error_message = __HPDF_MoveLineDown()).length())																																													{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledTotalHours(), HPDF_FIELD_LEFT)).length())																																			{ MESSAGE_ERROR("", "", "hpdf: fail to print total hours title"); }
		else if((error_message = __HPDF_PrintText(string(GetEffortHours())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 2.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())																					{ MESSAGE_ERROR("", "", "hpdf: fail to print total hours"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledTotalDays(), HPDF_FIELD_LEFT)).length())																																			{ MESSAGE_ERROR("", "", "hpdf: fail to print total days title"); }
		else if((error_message = __HPDF_PrintText(string(GetEffortDays())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 2.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())																					{ MESSAGE_ERROR("", "", "hpdf: fail to print total days"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledDayrate(), HPDF_FIELD_LEFT)).length())																																				{ MESSAGE_ERROR("", "", "hpdf: fail to print dayrate title"); }
		else if((error_message = __HPDF_PrintText(to_string(long(bt.GetDayrate().GetWhole()))			, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 2.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())												{ MESSAGE_ERROR("", "", "hpdf: fail to print dayrate whole"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledRur())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 3.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print dayrate rur"); }
		else if((error_message = __HPDF_PrintText(to_string(long(bt.GetDayrate().GetFraction()))		, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 4.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT),  __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 5.5 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print dayrate fraction"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledKop())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 6.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print dayrate kop"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledTotalPaymentNoVAT(), HPDF_FIELD_LEFT)).length())																																	{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment title"); }
		else if((error_message = __HPDF_PrintText(to_string(long(GetEffortCost().GetWhole()))				, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 2.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())												{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment whole"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledRur())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 3.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment rur"); }
		else if((error_message = __HPDF_PrintText(to_string(long(GetEffortCost().GetFraction()))			, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 4.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT),  __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 5.5 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment fraction"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledKop())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 6.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment kop"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledVAT(), HPDF_FIELD_LEFT)).length())																																					{ MESSAGE_ERROR("", "", "hpdf: fail to print vat title"); }
		else if((error_message = __HPDF_PrintText(to_string(long(GetEffortCostVAT().GetWhole()))			, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 2.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())												{ MESSAGE_ERROR("", "", "hpdf: fail to print vat payment whole"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledRur())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 3.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print vat payment rur"); }
		else if((error_message = __HPDF_PrintText(to_string(long(GetEffortCostVAT().GetFraction()))			, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 4.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT),  __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 5.5 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print vat payment fraction"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledKop())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 6.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print vat payment kop"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledTotalPayment(), HPDF_FIELD_LEFT)).length())																																			{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		else if((error_message = __HPDF_PrintText(to_string(long(GetTotalPayment().GetWhole()))				, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 2.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())												{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat whole"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledRur())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 3.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat rur"); }
		else if((error_message = __HPDF_PrintText(to_string(long(GetTotalPayment().GetFraction()))			, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 4.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT),  __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 5.5 * HPDF_TIMECARD_DAY_WIDTH_PERCENT), HPDF_TALIGN_RIGHT)).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat fraction"); }
		else if((error_message = __HPDF_PrintText(string(GetSpelledKop())									, __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 6.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat kop"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(bt.GetSignatureTitle1(), HPDF_FIELD_LEFT)).length())																																	{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		else if((error_message = __HPDF_PrintText(bt.GetSignatureTitle2(), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 10.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())									{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		// else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledSignature(), HPDF_FIELD_LEFT)).length())																																			{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		else if((error_message = __HPDF_PrintText(GetSpelledSignature(), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 10.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())											{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		// else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledInitials(), HPDF_FIELD_LEFT)).length())																																				{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		else if((error_message = __HPDF_PrintText(GetSpelledInitials(), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 10.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())											{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		// else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledPosition(), HPDF_FIELD_LEFT)).length())																																				{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		else if((error_message = __HPDF_PrintText(GetSpelledPosition(), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 10.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())											{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		// else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_MoveLineDown()).length())																																												{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		else if((error_message = __HPDF_PrintText(GetSpelledDate(), HPDF_FIELD_LEFT)).length())																																					{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
		else if((error_message = __HPDF_PrintText(GetSpelledDate(), __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + 10.0 * HPDF_TIMECARD_DAY_WIDTH_PERCENT))).length())												{ MESSAGE_ERROR("", "", "hpdf: fail to print total payment w/o vat title"); }
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to print timecard footer");
		error_message = gettext("hpdf: fail to print timecard footer");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_BT::__HPDF_SaveToFile() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_SaveToFile(__pdf, GetFilename().c_str());
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail save to file");
		error_message = gettext("hpdf: fail save to file");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}


auto	C_Print_BT::PrintAsPDF() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if((error_message = __HPDF_init()).empty())
	{
		if((error_message = __HPDF_SetDocProps()).empty())
		{
			if((error_message = __HPDF_MoveLineDown(0)).empty())
			{
				if((error_message = __HPDF_DrawTimecardTitle()).empty())
				{
					if((error_message = __HPDF_DrawTimecardTable()).empty())
					{
						if((error_message = __HPDF_DrawTimecardFooter()).empty())
						{
							if((error_message = __HPDF_SaveToFile()).empty())
							{

							}
							else
							{
								MESSAGE_ERROR("", "", "hpdf: fail to save to file");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "hpdf: fail to print text");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "hpdf: fail to print table");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "hpdf: fail to draw timecard title");
				}

			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to move pointer down");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to set dc props");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to initialize hpdf library");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}
*/

ostream& operator<<(ostream& os, const C_Print_BT &var)
{
	os << "object C_Print_BT [empty for now]";

	return os;
}

