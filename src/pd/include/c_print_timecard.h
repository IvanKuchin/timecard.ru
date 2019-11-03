#ifndef __C_PRINT_TIMECARD__H__
#define __C_PRINT_TIMECARD__H__

#include <string>
#include <vector>
#include <locale>

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "codecvt/mbwcvt.hpp"
#pragma GCC diagnostic pop 

#define _UNICODE
#include "libxl.h"
#undef _UNICODE

#include "hpdf.h"

#include "utilities_timecard.h" 
#include "c_invoicing_vars.h"
#include "c_timecard_to_print.h"
#include "c_float.h"
#include "clog.h"

using namespace std;

class C_Print_Timecard
{

	struct	Day_Summary_Struct
	{
		string		date;
		bool		is_weekend;
		c_float		efforts = 0;
	};

	private:
		// CMysql						*db = NULL;
		// CUser						*user = NULL;

		string							filename = "";

		C_Timecard_To_Print				timecard;
		C_Invoicing_Vars				*vars = nullptr;

		vector<Day_Summary_Struct>		day_summary = {};
		c_float							effort_hours = 0;
		c_float							effort_days = 0;
		c_float							effort_cost = 0;
		c_float							effort_cost_vat = 0;
		c_float							total_payment = 0;

		HPDF_Doc						__pdf;
		HPDF_Page						__pdf_page;
		HPDF_REAL						__pdf_page_width = 0;
		HPDF_REAL						__pdf_page_height = 0;
		HPDF_Font						__pdf_font;
		string							__pdf_font_name = "Helvetica";
		HPDF_INT						__pdf_font_descent = 0;
		HPDF_INT						__pdf_font_ascent = 0;
		HPDF_INT						__pdf_font_height = 0;
		HPDF_INT						__pdf_font_xheight = 0;
		HPDF_INT						__pdf_table_line_height = 0;
		int								__pdf_line = -1;
		int								__pdf_table_top = -1;
		int								__pdf_table_bottom = -1;

		auto		isDaySummaryStruct_Filled() -> bool;
		auto		AssignValuesToDaySummaryStruct() -> bool;
		auto		GetEffortHours()								{ return effort_hours; };
		auto		GetEffortDays()									{ return effort_days; };
		auto		GetEffortCost()									{ return effort_cost; };
		auto		GetEffortCostVAT()								{ return effort_cost_vat; };
		auto		GetNumberOfDaysInTimecard() -> int;

		auto		GetSpelledTitle() -> string;
		auto		GetSpelledPSoW() -> string;
		auto		GetSpelledProjectID()							{ return string(timecard.GetProjectNumber().length() ? gettext("Project ID") + ": "s + timecard.GetProjectNumber() : ""); };
		auto		GetSpelledTotalHours()							{ return string(gettext("Total hours on duty") + ": "s); };
		auto		GetSpelledTotalDays()							{ return string(gettext("Total days on duty") + ": "s); };
		auto		GetSpelledDayrate()								{ return string(gettext("Dayrate") + ": "s); };
		auto		GetSpelledTotalPayment()						{ return string(gettext("Total payment in reported timecard") + ": "s); };
		auto		GetSpelledVAT()									{ return string(gettext("VAT") + ": "s); };
		auto		GetSpelledTotalPaymentNoVAT()					{ return string(gettext("Total payment in reported timecard") + " "s + gettext("w/o") + " " + gettext("VAT") + ": "); };
		auto		GetSpelledSignature()							{ return string(gettext("Signature") + ": ________________________________"s); };
		auto		GetSpelledInitials()							{ return string(gettext("Initials") + ": ___________________________________"s); };
		// auto		GetSpelledInitials(string idx)	-> string;
		auto		GetSpelledPosition()							{ return string(gettext("Title") + ": _______________________________"s); };
		// auto		GetSpelledPosition(string idx)	-> string;
		auto		GetSpelledDate()								{ return string(gettext("Date") + ": ____________________________________"s); };
		auto		GetSpelledRur()									{ return string(gettext("rur.")); };
		auto		GetSpelledKop()									{ return string(gettext("kop.")); };
		auto		GetSpelledApprovers()							{ return string(gettext("Timecard approvers")); };

		auto		__HPDF_init() -> string;
		auto		__HPDF_SetDocProps() -> string;
		auto		__HPDF_MoveLineDown() -> string					{ return __HPDF_MoveLineDown(__pdf_font_height); };
		auto		__HPDF_MoveLineDown(int line_increment) -> string;
		auto		__HPDF_MoveTableLineDown() -> string			{ return __HPDF_MoveTableLineDown(__pdf_table_line_height); };
		auto		__HPDF_MoveTableLineDown(int line_increment) -> string;
		auto		__HPDF_SaveToFile() -> string;
		auto		__HPDF_PrintText(string text, double x, HPDF_TextAlignment) -> string;
		auto		__HPDF_PrintText(string text, double x1, double x2, HPDF_TextAlignment) -> string;
		auto		__HPDF_DrawTimecardHorizontalLine() -> string;
		auto		__HPDF_DrawTimecardTitle() -> string;
		auto		__HPDF_DrawTimecardTable() -> string;
		auto		__HPDF_DrawTimecardTableHeader() -> string;
		auto		__HPDF_DrawTimecardTableBody() -> string;
		auto		__HPDF_DrawTimecardTableFooter() -> string;
		auto		__HPDF_DrawTimecardFooter() -> string;
		auto		__HPDF_TableGetXOffset() -> HPDF_REAL;
		auto		__HPDF_TableGetTitleWidth() -> HPDF_REAL;
		auto		__HPDF_TableDaysGetXOffset() -> HPDF_REAL;
		auto		__HPDF_StartTable() -> string;
		auto		__HPDF_StopTable() -> string;
		auto		__HPDF_GetTimecardTableXByPercentage(double percent) -> double;
		auto		__HPDF_DrawTimecardVerticalLine(double x) -> string;
		auto		__HPDF_PaintDay(int day_number, double red, double green, double blue, int number_of_lines = 1) -> string;



	public:
					C_Print_Timecard();
					
		auto		GetTotalPayment()								{ return total_payment; };

		auto		SetTimecard(const C_Timecard_To_Print &param1) -> void;
		auto		SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto		SetFilename(const string &param1)				{ filename = param1; };
		auto		SetFilename(string &&param1) 					{ filename = move(param1); };

		auto		GetFilename()									{ return filename; }

		auto		PrintAsXLS() -> string;
		auto		PrintAsPDF() -> string;

					~C_Print_Timecard()								{ if(__pdf) { HPDF_Free(__pdf); }; };
};

ostream&	operator<<(ostream& os, const C_Print_Timecard &);

#endif
