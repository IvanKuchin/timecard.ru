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
		HPDF_INT						__pdf_font_descent = 0;
		HPDF_INT						__pdf_font_ascent = 0;
		HPDF_INT						__pdf_font_height = 0;
		HPDF_INT						__pdf_font_xheight = 0;
		int								__pdf_line = -1;

		auto		isDaySummaryStruct_Filled() -> bool;
		auto		AssignValuesToDaySummaryStruct() -> bool;
		auto		GetEffortHours()								{ return effort_hours; };
		auto		GetEffortDays()									{ return effort_days; };
		auto		GetEffortCost()									{ return effort_cost; };
		auto		GetEffortCostVAT()								{ return effort_cost_vat; };
		auto		GetTotalPayment()								{ return total_payment; };

		auto		GetSpelledTitle() -> string;

		auto		__HPDF_init() -> string;
		auto		__HPDF_SetDocProps() -> string;
		auto		__HPDF_MoveLineDown(int line_increment) -> string;
		auto		__HPDF_SaveToFile() -> string;
		auto		__HPDF_PrintText(string text, int x, int y) -> string;
		auto		__HPDF_DrawHorizontalLine() -> string;
		auto		__HPDF_DrawTimecardTitle() -> string;
		auto		__HPDF_TableGetXOffset() -> HPDF_REAL;
		auto		__HPDF_TableGetTitleWidth() -> HPDF_REAL;
		auto		__HPDF_TableDaysGetXOffset() -> HPDF_REAL;

	public:
					C_Print_Timecard();
					// C_Print_Timecard(CMysql *, CUser *) : db(param1), user(param2) {};

		auto		SetTimecard(const C_Timecard_To_Print &param1)	{ timecard = param1; day_summary = {}; __pdf_line = -1; };

		auto		SetFilename(const string &param1)				{ filename = param1; };
		auto		SetFilename(string &&param1) 					{ filename = move(param1); };

		auto		GetFilename()									{ return filename; }

		auto		PrintAsXLS() -> string;
		auto		PrintAsPDF() -> string;

					~C_Print_Timecard()								{ if(__pdf) { HPDF_Free(__pdf); }; };
};

ostream&	operator<<(ostream& os, const C_Print_Timecard &);

#endif