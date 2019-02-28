#include "c_print_timecard.h"

static void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	char	buffer[512];

    snprintf(buffer, sizeof(buffer) - 1, "error_no=%04X, detail_no=%d\n", (unsigned int) error_no, (int) detail_no);
    MESSAGE_ERROR("", "", "libhpdf: " +  buffer);
    throw std::exception (); /* throw exception on error */
}

C_Print_Timecard::C_Print_Timecard()
{
	MESSAGE_DEBUG("", "", "start");

	__pdf = HPDF_New(error_handler, NULL);

	if(__pdf)
	{
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to initialize hpdf library");
	}

	MESSAGE_DEBUG("", "", "start");
}

auto	C_Print_Timecard::isDaySummaryStruct_Filled() -> bool
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(day_summary.size()) result = true;

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

auto	C_Print_Timecard::AssignValuesToDaySummaryStruct() -> bool
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(timecard.GetDateStart().length() && timecard.GetDateFinish().length())
	{
		auto			timecard_lines = timecard.GetTimecardLines();

		if(timecard_lines.size())
		{
			auto	period_start = GetTMObject(timecard.GetDateStart());
			auto	period_finish = GetTMObject(timecard.GetDateFinish());

			if(period_start <= period_finish)
			{
				auto	today = period_start;
				while(today <= period_finish)
				{
					Day_Summary_Struct	single_day;

					mktime(&today);

					if((today.tm_wday == 6) || (today.tm_wday == 0)) single_day.is_weekend = true;
					else single_day.is_weekend = false;

					single_day.date = to_string(today.tm_mday);
					single_day.efforts = 0;

					day_summary.push_back(single_day);

					++today.tm_mday;
				}

				for(auto &timcard_line: timecard_lines)
				{
					auto		time_entries = SplitTimeentry(timcard_line.hours);

					if(time_entries.size())
					{
						auto	column_counter = 0;
						for(auto &time_entry : time_entries)
						{
							if(time_entry != "0")
							{
								c_float		tmp(time_entry);

								day_summary[column_counter].efforts = day_summary[column_counter].efforts + tmp;
								effort_hours = effort_hours + tmp;
							}
							++column_counter;
						}

						effort_days = effort_hours / c_float(8);
						effort_cost = timecard.GetDayrate() * GetEffortDays();
						effort_cost_vat = GetEffortCost() * c_float(VAT_PERCENTAGE) / c_float(100);
						total_payment = GetEffortCost() + GetEffortCostVAT();

					}
					else
					{
						MESSAGE_ERROR("", "", "time report(" + timcard_line.hours + ") have no entries. This flow expect to have time entries reported.");
					}
				}

				result = true;
			}
			else
			{
				MESSAGE_ERROR("", "", "period_start(" + timecard.GetDateStart() + ") must come earlier than period_finish(" + timecard.GetDateFinish() + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "timecard have no lines reported. Before using Print methods, populate lines.")
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard date start(" + timecard.GetDateStart() + ") or finish(" + timecard.GetDateFinish() + ") didn't populated");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_Timecard::GetSpelledTitle() -> string
{
	auto			start_spelling_date = GetSpellingFormattedDate(timecard.GetDateStart(), "%d %b %G");
	auto			finish_spelling_date = GetSpellingFormattedDate(timecard.GetDateFinish(), "%d %b %G");

	return "Отчет об оказанных услугах за период с " + start_spelling_date + " по " + finish_spelling_date;
}


auto	C_Print_Timecard::PrintAsXLS() -> string
{
	auto	error_message = ""s;
	auto	f_name = GetFilename(); 
	auto	good2go = false;

	MESSAGE_DEBUG("", "", "start");

	// locale cp1251_locale("ru_RU.UTF-8");
	// std::locale::global(cp1251_locale);

	if(isDaySummaryStruct_Filled())
	{
		// --- good to go
		good2go = true;
	}
	else
	{
		if(AssignValuesToDaySummaryStruct())
		{
			// --- good to go
			good2go = true;
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to fill in day_summary structure");
		}
	}

	if(good2go)
	{
		libxl::Book* book = xlCreateBook();
		if(book)
		{
			libxl::Sheet* sheet = book->addSheet(L"Sheet1");
			if(sheet)
			{
				auto			format_title = book->addFormat();
				auto			format_weekend = book->addFormat();
				auto			format_weekday = book->addFormat();
				auto			format_summary_over = book->addFormat();
				auto			format_summary_right = book->addFormat();
				auto			format_number_d2 = book->addFormat();
				auto			font_summary_over = book->addFont();
				auto			font_summary_right = book->addFont();
				auto			start_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateStart(), "%d %b %G"));
				auto			finish_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateFinish(), "%d %b %G"));
				auto			title = multibyte_to_wide(GetSpelledTitle());
				auto			psow_title = L"ТЗ №" + multibyte_to_wide(timecard.GetAgreementNumber()) + L" от " + multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateSign(), "%d %b %G"));
				auto			project_title = L""s;
				auto			timecard_lines = timecard.GetTimecardLines();
				auto			row_counter = 6;
				auto			column_counter = 0;

				format_title->setBorder(libxl::BORDERSTYLE_THIN);

				format_weekend->setFillPattern(libxl::FILLPATTERN_SOLID);
				format_weekend->setPatternForegroundColor(libxl::COLOR_GRAY25);
				format_weekend->setBorder(libxl::BORDERSTYLE_THIN);
				format_weekend->setNumFormat(libxl::NUMFORMAT_GENERAL);

				format_weekday->setBorder(libxl::BORDERSTYLE_THIN);
				format_weekday->setNumFormat(libxl::NUMFORMAT_GENERAL);

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

				if(timecard.GetProjectNumber().length())
					project_title = L"Идентификационный номер проекта: " + multibyte_to_wide(timecard.GetProjectNumber());

				// --- print proerties
				sheet->setPaper(libxl::PAPER_A4);
				sheet->setLandscape();
				sheet->setPrintZoom(50);

				// --- set columns width
				sheet->setCol(1, 1, 45);
				sheet->setCol(2, 31 + 2, 5);

				sheet->writeStr(2, 2, title.c_str());
				sheet->writeStr(3, 1, psow_title.c_str());
				sheet->writeStr(4, 1, project_title.c_str());

				// --- table header
				column_counter = 0;
				for(auto &single_day: day_summary)
				{
					sheet->writeBlank(5, column_counter + 2, format_title);

					sheet->writeNum(5, column_counter + 2, stod(single_day.date), single_day.is_weekend ? format_weekend : format_weekday);

					++column_counter;
				}

				// --- table body
				for(auto &timcard_line: timecard_lines)
				{
					wstring		timcard_line_title = multibyte_to_wide(timcard_line.customer) + L" / " + multibyte_to_wide(timcard_line.project) + L" / " + multibyte_to_wide(timcard_line.task);
					wstring		timcard_line_hours = multibyte_to_wide(timcard_line.hours);
					auto		time_entries = SplitTimeentry(timcard_line.hours);

					sheet->writeStr(row_counter, 1, timcard_line_title.c_str(), format_title);

					if(time_entries.size())
					{
						column_counter = 0;
						for(auto &time_entry : time_entries)
						{
							auto	hours = stod_noexcept(time_entry);

							sheet->writeBlank(row_counter, column_counter + 2, day_summary[column_counter].is_weekend ? format_weekend : format_weekday);

							if(hours)
								sheet->writeNum(row_counter, column_counter + 2, hours);

							++column_counter;
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "time report(" + timcard_line.hours + ") have no entries. This flow expect to have time entries reported.");
					}

					row_counter++;
				}

				// --- table summary
				sheet->writeStr(row_counter, 1, L"Итого:", format_title);
				column_counter = 0;
				for(auto &single_day: day_summary)
				{
					auto	hours = stod_noexcept(single_day.efforts);

					sheet->writeBlank(row_counter, column_counter + 2, single_day.is_weekend ? format_weekend : format_weekday);

					if(hours)
					{
						if(hours == 8)
							sheet->writeBlank(row_counter, column_counter + 2, format_summary_right);
						if(hours > 8)
							sheet->writeBlank(row_counter, column_counter + 2, format_summary_over);

						sheet->writeNum(row_counter, column_counter + 2, hours);
					}

					++column_counter;
				}
				row_counter++;

				{
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"Количество часов оказания услуг:");
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, double(GetEffortHours()), format_number_d2);

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"Количество дней оказания услуг:");
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, double(GetEffortDays()), format_number_d2);
					
					row_counter++;
				}

				{
					auto		temp = timecard.GetDayrate();

					sheet->writeStr(row_counter, 1, L"Ежедневный тариф: ");
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, temp.GetWhole());
					sheet->writeStr(row_counter, 6, L"руб.");
					sheet->writeNum(row_counter, 7, temp.GetFraction());
					sheet->writeStr(row_counter, 8, L"коп.");

					row_counter++;
				}

				{
					auto		temp = GetEffortCost();

					sheet->writeStr(row_counter, 1, L"Стоимость услуг по настоящему отчету составляет");
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, temp.GetWhole());
					sheet->writeStr(row_counter, 6, L"руб.");
					sheet->writeNum(row_counter, 7, temp.GetFraction());
					sheet->writeStr(row_counter, 8, L"коп.");
					
					row_counter++;
				}

				{
					auto	vat = GetEffortCostVAT();

					sheet->writeStr(row_counter, 1, L"НДС:");
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, vat.GetWhole());
					sheet->writeStr(row_counter, 6, L"руб.");
					sheet->writeNum(row_counter, 7, vat.GetFraction());
					sheet->writeStr(row_counter, 8, L"коп.");
					
					row_counter++;
				}

				{
					auto	temp = GetTotalPayment();

					sheet->writeStr(row_counter, 1, (L"Итого за период с "s + start_spelling_date + L" по " + finish_spelling_date + L": ").c_str());
					sheet->setMerge(row_counter, row_counter, 3, 5);
					sheet->writeNum(row_counter, 3, temp.GetWhole());
					sheet->writeStr(row_counter, 6, L"руб.");
					sheet->writeNum(row_counter, 7, temp.GetFraction());
					sheet->writeStr(row_counter, 8, L"коп.");
					
					row_counter++;
				}

				{
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, multibyte_to_wide(timecard.GetSignatureTitle1()).c_str());
					sheet->writeStr(row_counter, 3, multibyte_to_wide(timecard.GetSignatureTitle2()).c_str());

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"By/ Подпись:_________________________________");
					sheet->writeStr(row_counter, 3, L"By/ Подпись:_________________________________");

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"Name/ Ф. И. О.:_______________________________");
					sheet->writeStr(row_counter, 3, L"Name/ Ф. И. О.:_______________________________");

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"Title/ Должность:______________________________");
					sheet->writeStr(row_counter, 3, L"Title/ Должность:______________________________");

					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"Date/ Дата:___________________________________");
					sheet->writeStr(row_counter, 3, L"Date/ Дата:___________________________________");

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
		MESSAGE_ERROR("", "", "print to XLS not valid");
		error_message = gettext("print to XLS not valid");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}


auto	C_Print_Timecard::__HPDF_init() -> string
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

auto	C_Print_Timecard::__HPDF_SetDocProps() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");
	try
	{
		HPDF_NewDoc (__pdf);;

		/* set compression mode */
		HPDF_SetCompressionMode (__pdf, HPDF_COMP_ALL);

		/* set page mode to use outlines. */
		HPDF_SetPageMode (__pdf, HPDF_PAGE_MODE_USE_OUTLINE);

		__pdf_line = -1;
		__pdf_font_name =  HPDF_LoadType1FontFromFile (__pdf, (PDF_FONT_INSTALL_DIR + "/type1/a010013l.afm").c_str(), (PDF_FONT_INSTALL_DIR + "/type1/a010013l.pfb").c_str());
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

	    __pdf_font_height += 5;
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

auto	C_Print_Timecard::__HPDF_MoveLineDown(int line_decrement) -> string
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

auto	C_Print_Timecard::__HPDF_StartTable() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	__pdf_table_top = __pdf_line;

	if((error_message = __HPDF_DrawTimecardHorizontalLine()).empty())
	{
		if((error_message = __HPDF_MoveTableLineDown()).empty())
		{
/*
			if((error_message = __HPDF_DrawTimecardHorizontalLine()).empty())
			{
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to stop timecard table");
			}
*/
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

auto	C_Print_Timecard::__HPDF_GetTimecardTableXByPercentage(double percent) -> double
{
	MESSAGE_DEBUG("", "", "start");
	MESSAGE_DEBUG("", "", "HPDF_TIMECARD_DAY_WIDTH_PERCENT = "s + to_string(HPDF_TIMECARD_DAY_WIDTH_PERCENT));

	auto	workpane_width = __pdf_page_width - HPDF_FIELD_RIGHT - HPDF_FIELD_LEFT;
	auto	result = percent / 100 * workpane_width + HPDF_FIELD_LEFT;
	
	MESSAGE_DEBUG("", "", "finish(" + to_string(percent) + "% = " + to_string(result) + " px.)");

	return result;
}


auto	C_Print_Timecard::__HPDF_StopTable() -> string
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

auto	C_Print_Timecard::__HPDF_MoveTableLineDown(int line_decrement) -> string
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

auto	C_Print_Timecard::__HPDF_DrawHorizontalLine() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_Page_MoveTo (__pdf_page, 0, __pdf_line);
		HPDF_Page_LineTo (__pdf_page, 200, __pdf_line);
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

auto	C_Print_Timecard::__HPDF_PrintText(string text, int x, int y) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_REAL	width;

		HPDF_Page_BeginText (__pdf_page);
		HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, 20);
		HPDF_Page_MoveTextPos (__pdf_page, x, y);
		HPDF_Page_ShowText (__pdf_page, (text + " " + GetFilename()).c_str());
		HPDF_Page_MoveTextPos (__pdf_page, 0, - 50);
		width = HPDF_Page_TextWidth(__pdf_page, (text + " " + GetFilename()).c_str());
		HPDF_Page_EndText (__pdf_page);
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to print text");
		error_message = gettext("hpdf: fail to print text");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}


auto	C_Print_Timecard::__HPDF_TableGetXOffset() -> HPDF_REAL
{
	HPDF_REAL	result = 0;

	MESSAGE_DEBUG("", "", "start");

	result = HPDF_FIELD_LEFT;

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_Timecard::__HPDF_TableGetTitleWidth() -> HPDF_REAL
{
	HPDF_REAL	result = 0;

	MESSAGE_DEBUG("", "", "start");

	result = (__pdf_page_width - HPDF_FIELD_LEFT - HPDF_FIELD_RIGHT) * HPDF_TIMECARD_TITLE_WIDTH_PERCENT / 100;

	MESSAGE_DEBUG("", "", "finish");

	return result;
}
auto	C_Print_Timecard::__HPDF_TableDaysGetXOffset() -> HPDF_REAL
{
	HPDF_REAL	result = 0;

	MESSAGE_DEBUG("", "", "start");

	result = HPDF_FIELD_LEFT + __HPDF_TableGetTitleWidth();

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	C_Print_Timecard::__HPDF_DrawTimecardTitle() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		if((error_message = __HPDF_MoveLineDown()).empty())
		{
			HPDF_Page_BeginText (__pdf_page);
			HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
			HPDF_Page_TextRect (__pdf_page, __HPDF_TableDaysGetXOffset(), __pdf_line + __pdf_font_height, __pdf_page_width - __HPDF_TableDaysGetXOffset() - HPDF_FIELD_RIGHT, __pdf_line, (utf8_to_cp1251(GetSpelledTitle())).c_str(), HPDF_TALIGN_LEFT, NULL);
			HPDF_Page_EndText (__pdf_page);

			HPDF_Page_Rectangle (__pdf_page, __HPDF_TableDaysGetXOffset(), __pdf_line, __pdf_page_width - __HPDF_TableDaysGetXOffset() - HPDF_FIELD_RIGHT, __pdf_font_height);
			MESSAGE_DEBUG("", "", "msg ---- "s + GetSpelledTitle());
			MESSAGE_DEBUG("", "", "msg ---- "s + utf8_to_cp1251(GetSpelledTitle()));

			HPDF_Page_Stroke (__pdf_page);
			
			if((error_message = __HPDF_MoveLineDown()).empty())
			{
				HPDF_Page_BeginText (__pdf_page);
				HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
				HPDF_Page_TextRect (__pdf_page, __HPDF_TableGetXOffset(), __pdf_line + __pdf_font_height, __pdf_page_width - __HPDF_TableGetXOffset() - HPDF_FIELD_RIGHT, __pdf_line, ("--------[second line]-------"s).c_str(), HPDF_TALIGN_LEFT, NULL);
				HPDF_Page_EndText (__pdf_page);

				if((error_message = __HPDF_MoveLineDown()).empty())
				{
					HPDF_Page_BeginText (__pdf_page);
					HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
					HPDF_Page_TextRect (__pdf_page, __HPDF_TableGetXOffset(), __pdf_line + __pdf_font_height, __pdf_page_width - __HPDF_TableGetXOffset() - HPDF_FIELD_RIGHT, __pdf_line, ("--------[3-rd line]q-G-h-g-l-|-||||||-"s).c_str(), HPDF_TALIGN_LEFT, NULL);
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

auto	C_Print_Timecard::GetNumberOfDaysInTimecard() -> int
{
	auto	result = 0;
	auto	timecard_lines = timecard.GetTimecardLines();

	MESSAGE_DEBUG("", "", "start");

	if(timecard_lines.size())
	{
		result = SplitTimeentry(timecard_lines[0].hours).size();
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard object have no time_entries");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

auto	C_Print_Timecard::__HPDF_DrawTimecardHorizontalLine() -> string
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

auto	C_Print_Timecard::__HPDF_DrawTimecardVerticalLine(double x) -> string
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

auto	C_Print_Timecard::__HPDF_PaintDay(int day_number, double red, double green, double blue) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		auto	rect_left = __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + HPDF_TIMECARD_DAY_WIDTH_PERCENT * day_number);
		auto	rect_width = __HPDF_GetTimecardTableXByPercentage(HPDF_TIMECARD_TITLE_WIDTH_PERCENT + HPDF_TIMECARD_DAY_WIDTH_PERCENT * (day_number + 1)) - rect_left;
		auto	rect_bottom = __pdf_line;
		auto	rect_height = __pdf_table_line_height;

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

auto	C_Print_Timecard::__HPDF_DrawTimecardTableHeader() -> string
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
					if(day_summary[i].is_weekend) __HPDF_PaintDay(i, 0.8, 0.8, 0.8);

					HPDF_Page_BeginText (__pdf_page);
					HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
					HPDF_Page_TextRect (__pdf_page, 
										__HPDF_GetTimecardTableXByPercentage(percentage), 
										__pdf_line + __pdf_table_line_height, 
										__HPDF_GetTimecardTableXByPercentage(percentage + HPDF_TIMECARD_DAY_WIDTH_PERCENT), 
										__pdf_line, 
										(day_summary[i].date).c_str(), 
										HPDF_TALIGN_CENTER, 
										NULL);
					HPDF_Page_EndText (__pdf_page);

					percentage += HPDF_TIMECARD_DAY_WIDTH_PERCENT;
				}
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

auto	C_Print_Timecard::__HPDF_DrawTimecardTableBody() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		auto	number_of_days = GetNumberOfDaysInTimecard();

		if(number_of_days)
		{
			auto	timecard_lines = timecard.GetTimecardLines();

			for(auto &timcard_line : timecard_lines)
			{
				auto		time_entries = SplitTimeentry(timcard_line.hours);

				if(time_entries.size())
				{
						auto	percentage = HPDF_TIMECARD_TITLE_WIDTH_PERCENT;

						for(auto i = 0; i < number_of_days; ++i)
						{
							auto	time_entry = time_entries[i];

							if(day_summary[i].is_weekend) __HPDF_PaintDay(i, 0.8, 0.8, 0.8);

							if(time_entry.length() && (time_entry != "0"))
							{
								HPDF_Page_BeginText (__pdf_page);
								HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE - 3);
								HPDF_Page_TextRect (__pdf_page, 
													__HPDF_GetTimecardTableXByPercentage(percentage), 
													__pdf_line + __pdf_table_line_height - 1, 
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
					MESSAGE_ERROR("", "", "time report(" + timcard_line.hours + ") have no entries. This flow expect to have time entries reported.");
				}

				if((error_message = __HPDF_MoveTableLineDown()).length())
				{
					MESSAGE_DEBUG("", "", "hpdf: fail to move line down");
					break;
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

auto	C_Print_Timecard::__HPDF_DrawTimecardTableFooter() -> string
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
					if(day_summary[i].is_weekend) __HPDF_PaintDay(i, 0.8, 0.8, 0.8);
					if(day_summary[i].efforts)
					{
						if(day_summary[i].efforts == 8)
						{
							__HPDF_PaintDay(i, 0.4, 1, 0.4);
							HPDF_Page_SetRGBFill(__pdf_page, 0.2, 0.4, 0.2);
						}
						if(day_summary[i].efforts > 8)
						{
							__HPDF_PaintDay(i, 1, 0.4, 0.4);
							HPDF_Page_SetRGBFill(__pdf_page, 0.4, 0.2, 0.2);
						}

						HPDF_Page_BeginText (__pdf_page);
						HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE - 3);
						HPDF_Page_TextRect (__pdf_page, 
											__HPDF_GetTimecardTableXByPercentage(percentage), 
											__pdf_line + __pdf_table_line_height - 1, 
											__HPDF_GetTimecardTableXByPercentage(percentage + HPDF_TIMECARD_DAY_WIDTH_PERCENT), 
											__pdf_line, 
											(string(day_summary[i].efforts)).c_str(), 
											HPDF_TALIGN_CENTER, 
											NULL);
						HPDF_Page_EndText (__pdf_page);

						HPDF_Page_SetRGBFill(__pdf_page, 0, 0, 0);
					}

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

auto	C_Print_Timecard::__HPDF_DrawTimecardTable() -> string
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

auto	C_Print_Timecard::__HPDF_SaveToFile() -> string
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


auto	C_Print_Timecard::PrintAsPDF() -> string
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
						if((error_message = __HPDF_PrintText("My text " + GetRandom(10), 100, 100)).empty())
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


ostream& operator<<(ostream& os, const C_Print_Timecard &var)
{
	os << "object C_Print_Timecard [empty for now]";

	return os;
}

