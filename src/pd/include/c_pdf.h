#ifndef __C_PDF__H__
#define __C_PDF__H__

#include <string>
#include <locale>
#include <cmath>

#include "hpdf.h"

#include "clog.h"

using namespace std;


void c_pdf_error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data);

class C_PDF
{
	private:
		string							filename = "";
	public:
		HPDF_Doc						__pdf;
		HPDF_PageDirection				__pdf_orientation = HPDF_PAGE_LANDSCAPE;
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

						C_PDF();

		auto			Restart() -> void;

		auto			__HPDF_init() -> string;
		auto			__HPDF_SetDocProps() -> string;
		auto			__HPDF_SaveToFile() -> string;
		auto			__HPDF_DrawHorizontalLine(int x, int line_length) -> string;
		auto			__HPDF_PrintText(string text, int x) -> string;
		auto			__HPDF_MoveLineDown() -> string					{ return __HPDF_MoveLineDown(__pdf_font_height); };
		auto			__HPDF_MoveLineDown(int line_increment) -> string;

		auto			SetOrientation(HPDF_PageDirection param)		{ __pdf_orientation = param; };
		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			GetFilename()									{ return filename; }

						~C_PDF()										{ if(__pdf) { HPDF_Free(__pdf); }; };
};

ostream&	operator<<(ostream& os, const C_PDF &);

#endif
