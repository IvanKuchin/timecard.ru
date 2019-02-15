#include "c_print_timecard.h"

auto	C_Print_Timecard::isDaySummaryStruct_Filled() -> bool
{
	auto	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(day_summary.size()) result = true;

	MESSAGE_DEBUG("", "", "finish");

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
								total_efforts = total_efforts + tmp;
							}
							++column_counter;
						}

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
				auto			font_summary_over = book->addFont();
				auto			font_summary_right = book->addFont();
				auto			start_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateStart(), "%d %b %G"));
				auto			finish_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateFinish(), "%d %b %G"));
				auto			title = L"Отчет об оказанных услугах за период с " + start_spelling_date + L" по " + finish_spelling_date;
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
					sheet->writeNum(row_counter, 3, double(total_efforts));
					
					row_counter++;
				}

				{
					c_float		days_efforts = total_efforts / c_float(8);

					sheet->writeStr(row_counter, 1, L"Количество дней оказания услуг:");
					sheet->writeNum(row_counter, 3, double(days_efforts));
					
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, (L"Итого за период с "s + start_spelling_date + L" по " + finish_spelling_date + L": ").c_str());
					
					row_counter++;
				}

				{
					sheet->writeStr(row_counter, 1, L"Стоимость услуг по настоящему отчету составляет:");
					
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
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Print_Timecard::PrintAsPDF() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");


	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}


ostream& operator<<(ostream& os, const C_Print_Timecard &var)
{
	os << "object C_Print_Timecard [empty for now]";

	return os;
}

