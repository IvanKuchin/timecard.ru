#include "c_print_timecard.h"

auto	C_Print_Timecard::PrintAsXLS() -> string
{
	auto	error_message = ""s;
	auto	f_name = GetFilename(); 

	MESSAGE_DEBUG("", "", "start");

	// locale cp1251_locale("ru_RU.UTF-8");
	// std::locale::global(cp1251_locale);

	libxl::Book* book = xlCreateBook();
	if(book)
	{
		libxl::Sheet* sheet = book->addSheet(L"Sheet1");
		if(sheet)
		{
			auto		start_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateStart(), "%d %b %G"));
			auto		finish_spelling_date = multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateFinish(), "%d %b %G"));
			auto		title = L"Отчет об оказанных услугах за период с " + start_spelling_date + L" по " + finish_spelling_date;
			auto		psow_title = L"ТЗ №" + multibyte_to_wide(timecard.GetAgreementNumber()) + L" от " + multibyte_to_wide(GetSpellingFormattedDate(timecard.GetDateSign(), "%d %b %G"));
			auto		project_title = L""s;
			auto		timecard_lines = timecard.GetTimecardLines();
			auto		i = 6;

			if(timecard.GetProjectNumber().length())
				project_title = L"Идентификационный номер проекта: " + multibyte_to_wide(timecard.GetProjectNumber());

			sheet->writeStr(2, 2, title.c_str());
			sheet->writeStr(3, 1, psow_title.c_str());
			sheet->writeStr(4, 1, project_title.c_str());

			// --- 5-th row is dates

			for(auto &timcard_line: timecard_lines)
			{
				wstring		timcard_line_title = multibyte_to_wide(timcard_line.customer) + L" / " + multibyte_to_wide(timcard_line.project) + L" / " + multibyte_to_wide(timcard_line.task);
				wstring		timcard_line_hours = multibyte_to_wide(timcard_line.hours);

				sheet->writeStr(i, 1, timcard_line_title.c_str());
				sheet->writeStr(i, 2, timcard_line_hours.c_str());

				MESSAGE_DEBUG("", "", "--- "s + to_string(i));
				i++;
			}
/*
			sheet->writeStr(2, 1, L"ID");
			sheet->writeStr(3, 1, L"DateStart");
			sheet->writeStr(4, 1, L"DateFinish");
			sheet->writeStr(5, 1, L"AgreementNumber");
			sheet->writeStr(6, 1, L"Дата подписания");
			sheet->writeStr(7, 1, L"SignatureTitle1");
			sheet->writeStr(8, 1, L"SignatureTitle2");
			sheet->writeStr(9, 1, L"ProjectNumber");
			sheet->writeStr(10, 1, L"Dayrate");

			sheet->writeStr(2, 3, timecard.GetID().c_str());
			sheet->writeStr(3, 3, timecard.GetDteStart().c_str());
			sheet->writeStr(4, 3, timecard.GetDateFinish().c_str());
			sheet->writeStr(5, 3, timecard.GetAgreementNumber().c_str());
			sheet->writeStr(6, 3, timecard.GetDateSign().c_str());
			sheet->writeStr(7, 3, timecard.GetSignatureTitle1().c_str());
			sheet->writeStr(8, 3, timecard.GetSignatureTitle2().c_str());
			sheet->writeStr(9, 3, timecard.GetProjectNumber().c_str());
			sheet->writeStr(10, 3, string(timecard.GetDayrate()).c_str());
*/

			
			// libxl::Format* dateFormat = book->addFormat();
			// dateFormat->setNumFormat(libxl::NUMFORMAT_DATE);
			// sheet->writeNum(4, 1, book->datePack(2008, 4, 22), dateFormat);
			
			// sheet->setCol(1, 1, 12);
		}

		if(book->save(multibyte_to_wide(GetFilename()).c_str())) {}
		else
		{
			MESSAGE_ERROR("", "", "can't save " + filename + ".xls")
		}
		book->release();
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

