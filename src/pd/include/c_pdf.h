#ifndef __C_PDF__H__
#define __C_PDF__H__

#include <vector>
#include <string>
#include <locale>
#include <cmath>

#include "hpdf.h"

#include "localy_pd.h"
#include "c_pdf_const.h"
#include "clog.h"

using namespace std;


void c_pdf_error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data);

enum Font_Type {BOLD_FONT, NORMAL_FONT};

class C_PDF
{
	private:
		string			filename = "";
		vector<double>	grid_widths = {};

		auto			__HPDF_MoveLineDownByPixels(int pixel_lines) -> string;
		auto			__HPDF_GetPixelSizeByPercentage(double percents) -> double;
		auto			__HPDF_GetPixelOffsetByPercentage(double percents) -> double;
		auto			__HPDF_GetNumberOfLines(string text, double left_shift_percent, double right_shift_percent, Font_Type font_type, HPDF_REAL font_size) -> int;
		auto			__HPDF_GetTableWidthInPercent() { auto sum = 0.0; for(auto temp: grid_widths) sum += temp; return sum; };
		auto			__HPDF_DrawVerticalLine(double) -> string;

	public:
		HPDF_Doc		__pdf;
		HPDF_PageDirection		__pdf_orientation = HPDF_PAGE_LANDSCAPE;
		HPDF_Page		__pdf_page;
		HPDF_REAL		__pdf_page_width = 0;
		HPDF_REAL		__pdf_page_height = 0;
		HPDF_Font		__pdf_font, __pdf_font_bold;
		string			__pdf_font_name;
		string			__pdf_font_bold_name;
		HPDF_INT		__pdf_font_descent = 0;
		HPDF_INT		__pdf_font_ascent = 0;
		HPDF_INT		__pdf_font_height = 0;
		HPDF_INT		__pdf_font_xheight = 0;
		HPDF_INT		__pdf_table_line_height = 0;
		int				__pdf_line = -1;
		int				__pdf_table_top = -1;
		int				__pdf_table_bottom = -1;

						C_PDF();

		auto			Restart() -> void;
		auto			AddColumn(double param) -> string;

		auto			__HPDF_init() -> string;
		auto			__HPDF_SetDocProps() -> string;
		auto			__HPDF_SaveToFile() -> string;
		auto			__HPDF_DrawHorizontalLine(double, double) -> string;

		auto			__HPDF_PrintText(string text, int x) -> string;
		auto			__HPDF_MoveLineDown(int num_lines = 1)			{ return __HPDF_MoveLineDownByPixels(num_lines * __pdf_font_height); };
		auto			__HPDF_PrintTextRect(string text, double left_shift_percent, double right_shift_percent, HPDF_TextAlignment align, Font_Type font_type, HPDF_REAL font_size, bool move_line_down) -> string;
		auto			__HPDF_DrawRect(double left_shift_percent, double top_shift_line, double right_shift_percent, double bottom_shift_line) -> string;

		auto			__HPDF_StartTable() -> string;
		auto			__HPDF_GetNumberOfLinesInTable(unsigned int index_col, string text, Font_Type font_type, HPDF_REAL font_size) -> int;
		auto			__HPDF_PrintTextTableCell(unsigned int index_col, string text, HPDF_TextAlignment text_align, Font_Type font_type, HPDF_REAL font_size, bool move_line_down) -> string;
		auto			__HPDF_PrintTextTableCell(unsigned int index_col_start, unsigned int index_col_finish, string text, HPDF_TextAlignment text_align, Font_Type font_type, HPDF_REAL font_size, bool move_line_down) -> string;

		auto			__HPDF_RemoveTableSeparator(unsigned int col_idx, unsigned int num_lines = 1) -> string const;
		auto			__HPDF_MoveTableLineDown(int ) -> string;
		auto			__HPDF_MoveTableLineDown()						{ return __HPDF_MoveTableLineDown(__pdf_table_line_height); };
		auto			__HPDF_StopTable() -> string;


		auto			SetOrientation(HPDF_PageDirection param)		{ __pdf_orientation = param; };
		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			GetFilename()									{ return filename; }

						~C_PDF()										{ if(__pdf) { HPDF_Free(__pdf); }; };
};

ostream&	operator<<(ostream& os, const C_PDF &);

#endif
