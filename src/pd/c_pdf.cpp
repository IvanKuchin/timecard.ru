#include "c_pdf.h"

void c_pdf_error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	char	buffer[512];   /* Flawfinder: ignore */

    snprintf(buffer, sizeof(buffer) - 1, "error_no=%04X, detail_no=%d\n", (unsigned int) error_no, (int) detail_no);
    MESSAGE_ERROR("", "", "libhpdf: " +  buffer);
    throw std::exception (); /* throw exception on error */
}

C_PDF::C_PDF()
{
	MESSAGE_DEBUG("", "", "start");

	__pdf = HPDF_New(c_pdf_error_handler, NULL);

	if(__pdf)
	{
		try
		{
			__pdf_font_name = HPDF_LoadTTFontFromFile (__pdf, (PDF_FONT_INSTALL_DIR + "/ttf/arial.ttf").c_str(), HPDF_TRUE);
			__pdf_font_bold_name = HPDF_LoadTTFontFromFile (__pdf, (PDF_FONT_INSTALL_DIR + "/ttf/arialbd.ttf").c_str(), HPDF_TRUE);
		}
		catch(...)
		{
			MESSAGE_ERROR("", "", "HPDF thrown exception");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "fail to initialize hpdf library");
	}

	MESSAGE_DEBUG("", "", "start");
}

auto	C_PDF::Restart() -> void
{
	__pdf_line = -1;
}


auto	C_PDF::AddColumn(double new_width) -> string
{
	auto	error_message = ""s;
	auto	sum = 0.0;

	MESSAGE_DEBUG("", "", "start (add column " + to_string(new_width) + "%)");

	for(auto width: grid_widths) sum += width;

	if((sum + new_width) <= 100)
	{
		grid_widths.push_back(new_width);
	}
	else
	{
		error_message = gettext("table is too wide");
		MESSAGE_ERROR("", "", "adding " + to_string(new_width) + " to table width(" + to_string(sum) + ") makes it wider than 100%");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_PDF::__HPDF_StartTable() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	__pdf_table_top = __pdf_line;

	if((error_message = __HPDF_DrawHorizontalLine(0, __HPDF_GetTableWidthInPercent())).empty())
	{
/*		if((error_message = __HPDF_MoveTableLineDown()).empty())
		{
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to move table line down");
		}
*/
	}
	else
	{
		MESSAGE_ERROR("", "", "hpdf: fail to draw timecard horizontal line");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_PDF::__HPDF_MoveTableLineDown(int number_of_lines) -> string
{
	auto	error_message = ""s;
	auto	line_decrement = number_of_lines * __pdf_font_height + HPDF_TIMECARD_TABLE_SPACING;

	MESSAGE_DEBUG("", "", "start (move down " + to_string(number_of_lines) + " line(s), " + to_string(line_decrement) + " px.)");

	if((__pdf_line - line_decrement) < HPDF_FIELD_BOTTOM)
	{
		MESSAGE_DEBUG("", "", "change page in pdf");

		if((error_message = __HPDF_StopTable()).empty())
		{
			if((error_message = __HPDF_MoveLineDownByPixels(line_decrement)).empty())
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
		if((error_message = __HPDF_MoveLineDownByPixels(line_decrement)).empty())
		{
		}
		else
		{
			MESSAGE_ERROR("", "", "hpdf: fail to stop table");
		}
	}

	__HPDF_DrawHorizontalLine(0, __HPDF_GetTableWidthInPercent());

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_PDF::__HPDF_PrintTextTableCell(unsigned int index_col, string text, HPDF_TextAlignment text_align, Font_Type font_type, HPDF_REAL font_size, bool move_line_down) -> string
{
	return __HPDF_PrintTextTableCell(index_col, index_col, text, text_align, font_type, font_size, move_line_down);
}

auto	C_PDF::__HPDF_PrintTextTableCell(unsigned int index_col_start, unsigned int index_col_finish, string text, HPDF_TextAlignment text_align, Font_Type font_type, HPDF_REAL font_size, bool move_line_down) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (column: " + to_string(index_col_start) + ", text.length(" + to_string(text.length()) + "))");

	if(index_col_start <= grid_widths.size())
	{
		auto	number_of_lines = __HPDF_GetNumberOfLinesInTable(index_col_start, text, font_type, font_size);
		auto	line_decrement = number_of_lines * __pdf_font_height + HPDF_TIMECARD_TABLE_SPACING;

		if((__pdf_line - line_decrement) < HPDF_FIELD_BOTTOM)
		{
			if((error_message = __HPDF_MoveTableLineDown(number_of_lines)).empty())
			{
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to move table line down");
			}
		}


		if(error_message.empty())	
		{
			auto	left_shift_percent = 0.0;
			auto	right_shift_percent = 0.0;

			for(auto i = 0u; i < index_col_start; ++i)
			{
				left_shift_percent += grid_widths[i];
			}

			right_shift_percent = left_shift_percent;
			for(auto i = index_col_start; i <= index_col_finish; ++i)
			{
				right_shift_percent += grid_widths[i];
			}

			// --- shrink text square, so text will be spaced from vertical table lines
			// --- with narrow cells (<= 5%) don't shrink print area
			if(left_shift_percent + 6 <= right_shift_percent)
			{
				left_shift_percent += 1;
				right_shift_percent -= 1;
			}

			if((error_message = __HPDF_PrintTextRect(text, left_shift_percent, right_shift_percent, text_align, font_type, font_size, false)).empty())
			{
				if(move_line_down)
				{
					if((error_message = __HPDF_MoveTableLineDown(number_of_lines)).empty())
					{
						__HPDF_DrawHorizontalLine(0, __HPDF_GetTableWidthInPercent());
					}
					else
					{
						MESSAGE_ERROR("", "", "hpdf: fail to stop table");
					}
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "hpdf: fail to stop table");
			}
		}

	}
	else
	{
		error_message = gettext("col index out of range") + "( "s + to_string(index_col_start) + " of " + to_string(grid_widths.size()) +")";
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_PDF::__HPDF_StopTable() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (top/bottom: " + to_string(__pdf_table_top) + "/" + to_string(__pdf_line) + ")");

	__pdf_table_bottom = __pdf_line;

	if(__pdf_table_top >= __pdf_table_bottom)
	{
		// --- Draw vertical lines
		// ---- vertical line: table left side
		if((error_message = __HPDF_DrawVerticalLine(0)).empty())
		{
			auto	current_offset = 0.0;
			for(auto width: grid_widths)
			{
				current_offset += width;
				if((error_message = __HPDF_DrawVerticalLine(current_offset)).length())
				{
					MESSAGE_ERROR("", "", "fail to draw timecard vertical line");
					break;
				}
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

// --- PDF part
auto	C_PDF::__HPDF_SaveToFile() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_SaveToFile(__pdf, GetFilename().c_str());
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail save to file");
		MESSAGE_ERROR("", "", "hpdf: fail save to file (" + GetFilename() + ")");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_PDF::__HPDF_init() -> string
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

auto	C_PDF::__HPDF_SetDocProps() -> string
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

		grid_widths.clear();
		__pdf_line			= -1;
	    __pdf_font			= HPDF_GetFont(__pdf, __pdf_font_name.c_str(), "CP1251");
	    __pdf_font_bold 	= HPDF_GetFont(__pdf, __pdf_font_bold_name.c_str(), "CP1251");

	    __pdf_font_descent	= HPDF_Font_GetDescent(__pdf_font) * HPDF_TIMECARD_FONT_SIZE / 1000;
	    __pdf_font_ascent	= HPDF_Font_GetAscent(__pdf_font) * HPDF_TIMECARD_FONT_SIZE / 1000;
	    __pdf_font_xheight	= HPDF_Font_GetXHeight(__pdf_font) * HPDF_TIMECARD_FONT_SIZE / 1000;

	    if(__pdf_font_descent && __pdf_font_ascent && __pdf_font_xheight)
	    {
		    MESSAGE_DEBUG("", "", "hpdf: font ascent is " + to_string(__pdf_font_ascent));
		    MESSAGE_DEBUG("", "", "hpdf: font xheight is " + to_string(__pdf_font_xheight));
		    MESSAGE_DEBUG("", "", "hpdf: font descent is " + to_string(__pdf_font_descent));

		    __pdf_font_height = __pdf_font_descent + __pdf_font_ascent + __pdf_font_xheight;
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
		MESSAGE_ERROR("", "", "HPDF thrown exception");		
	}


	MESSAGE_DEBUG("", "", "end (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;	
}

auto	C_PDF::__HPDF_MoveLineDownByPixels(int pixel_lines) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (" + to_string(pixel_lines) + ")");

	__pdf_line -= pixel_lines;

	if(__pdf_line < HPDF_FIELD_BOTTOM)
	{
		MESSAGE_DEBUG("", "", "new page adding");

		try
		{
			__pdf_page = HPDF_AddPage(__pdf);
		    HPDF_Page_SetSize(__pdf_page, HPDF_PAGE_SIZE_A4, __pdf_orientation);
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

	MESSAGE_DEBUG("", "", "finish (curr line is " + to_string(__pdf_line) + ")");

	return error_message;
}

auto	C_PDF::__HPDF_DrawVerticalLine(double percentage) -> string
{
	MESSAGE_DEBUG("", "", "start (at " + to_string(percentage) + "%, top/bottom " + to_string(__pdf_table_top) + "/" + to_string(__pdf_table_bottom) + ")");

	auto	error_message = ""s;

	try
	{
		HPDF_Page_MoveTo (__pdf_page, __HPDF_GetPixelOffsetByPercentage(percentage), __pdf_table_top);
		HPDF_Page_LineTo (__pdf_page, __HPDF_GetPixelOffsetByPercentage(percentage), __pdf_table_bottom);
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

// --- remove table separator at index(col_idx) one line height
auto	C_PDF::__HPDF_RemoveTableSeparator(unsigned int col_idx, unsigned int num_lines) -> string const
{
	MESSAGE_DEBUG("", "", "start (column index " + to_string(__HPDF_GetPixelOffsetByPercentage(col_idx)) + ", top " + to_string(__pdf_line) + ")");

	auto	error_message = ""s;
	auto	percentage = 0.0;

	for(auto i = 0u; i < col_idx; ++i) percentage += grid_widths[i];

	try
	{
		if(HPDF_Page_SetGrayStroke(__pdf_page, 1) != HPDF_OK) { MESSAGE_ERROR("", "", "fail to set stroke color"); }
		HPDF_Page_MoveTo (__pdf_page, __HPDF_GetPixelOffsetByPercentage(percentage), __pdf_line);
		HPDF_Page_LineTo (__pdf_page, __HPDF_GetPixelOffsetByPercentage(percentage), __pdf_line - num_lines * __pdf_font_height);
		// HPDF_Page_LineTo (__pdf_page, __HPDF_GetPixelOffsetByPercentage(percentage), __pdf_line - __pdf_font_height);
		HPDF_Page_Stroke (__pdf_page);
		if(HPDF_Page_SetGrayStroke(__pdf_page, 0) != HPDF_OK) { MESSAGE_ERROR("", "", "fail to set stroke color"); }
	}
	catch(...)
	{
		MESSAGE_ERROR("", "", "hpdf: fail to remove column separator");
		error_message = gettext("hpdf: fail to remove column separator");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_PDF::__HPDF_DrawHorizontalLine(double left_shift_percent, double right_shift_percent) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto		error_message	= ""s;
	HPDF_REAL	left_offset		= __HPDF_GetPixelOffsetByPercentage(left_shift_percent);
	HPDF_REAL	right_offset	= __HPDF_GetPixelOffsetByPercentage(right_shift_percent);

	try
	{
		HPDF_Page_MoveTo (__pdf_page, left_offset, __pdf_line);
		HPDF_Page_LineTo (__pdf_page, right_offset, __pdf_line);
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

auto	C_PDF::__HPDF_PrintText(string text, int x) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_Page_BeginText (__pdf_page);
		HPDF_Page_SetFontAndSize (__pdf_page, __pdf_font, HPDF_TIMECARD_FONT_SIZE);
		HPDF_Page_TextRect (__pdf_page, x, __pdf_line + __pdf_font_height, __pdf_page_width - HPDF_FIELD_RIGHT, __pdf_line, text.c_str(), HPDF_TALIGN_LEFT, NULL);
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

auto	C_PDF::__HPDF_GetPixelSizeByPercentage(double percents) -> double
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = percents * double(__pdf_page_width - HPDF_FIELD_RIGHT - HPDF_FIELD_LEFT) / 100.0;

	MESSAGE_DEBUG("", "", "finish (width " + to_string(percents) + "% = " + to_string(result) + " px.)");

	return result;
}

auto	C_PDF::__HPDF_GetPixelOffsetByPercentage(double percents) -> double
{
	MESSAGE_DEBUG("", "", "start");

	auto	result = percents * double(__pdf_page_width - HPDF_FIELD_RIGHT - HPDF_FIELD_LEFT) / 100.0 + HPDF_FIELD_RIGHT;

	MESSAGE_DEBUG("", "", "finish (offset " + to_string(percents) + "% = " + to_string(result) + " px.)");

	return result;
}

auto	C_PDF::__HPDF_GetNumberOfLines(string text, double left_shift_percent, double right_shift_percent, Font_Type font_type, HPDF_REAL font_size) -> int
{
	MESSAGE_DEBUG("", "", "start (" + to_string(left_shift_percent) + " <-> " + to_string(right_shift_percent) + ", text length is " + to_string(text.length()) + ")");

	HPDF_Font	current_font			= font_type == BOLD_FONT ? __pdf_font_bold : __pdf_font;
	auto		text_width_pdf_struct	= HPDF_Font_TextWidth(current_font, (HPDF_BYTE *)(text).c_str(), text.length());
	auto		text_width				= text_width_pdf_struct.width * font_size / 1000.0;
	auto		cell_width				= __HPDF_GetPixelSizeByPercentage(abs(right_shift_percent - left_shift_percent));
	auto		number_of_lines			= ceil(text_width / cell_width);

	if(number_of_lines)
	{
		// --- add 10% to calculated width due to line break may move whole word to the next line
		number_of_lines			= ceil(text_width * 1.1 / cell_width);
	}

	for(auto &symbol: text) if(symbol == '\n') ++ number_of_lines;
		
	MESSAGE_DEBUG("", "", "finish (text height is " + to_string(number_of_lines) + " lines = text_width(" + to_string(text_width_pdf_struct.width * font_size / 1000.0) + ") / cell_width(" + to_string(cell_width) + "))");

	return number_of_lines;
}

auto	C_PDF::__HPDF_GetNumberOfLinesInTable(unsigned int index_col, string text, Font_Type font_type, HPDF_REAL font_size) -> int
{
	MESSAGE_DEBUG("", "", "start");

	auto		number_of_lines			= 0;

	if(index_col <= grid_widths.size())
	{
		auto	left_shift_percent = 0.0;
		auto	right_shift_percent = 0.0;

		for(auto i = 0u; i < index_col; ++i)
		{
			left_shift_percent += grid_widths[i];
		}
		right_shift_percent = left_shift_percent + grid_widths[index_col];

		number_of_lines = __HPDF_GetNumberOfLines(text, left_shift_percent + 1, right_shift_percent - 1, font_type, font_size);
	}
	else
	{
		MESSAGE_ERROR("", "", gettext("col index out of range") + "( "s + to_string(index_col) + " of " + to_string(grid_widths.size()) +")");
	}

	MESSAGE_DEBUG("", "", "finish (text height is " + to_string(number_of_lines) + " lines)");

	return number_of_lines;
}

auto	C_PDF::__HPDF_PrintTextRect(string text, double left_shift_percent, double right_shift_percent, HPDF_TextAlignment text_align, Font_Type font_type, HPDF_REAL font_size, bool move_line_down) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_Font	current_font			= font_type == BOLD_FONT ? __pdf_font_bold : __pdf_font;
		auto		number_of_lines			= __HPDF_GetNumberOfLines(text, left_shift_percent, right_shift_percent, font_type, font_size);

		HPDF_REAL	left_offset				= __HPDF_GetPixelOffsetByPercentage(left_shift_percent);
		HPDF_REAL	right_offset			= __HPDF_GetPixelOffsetByPercentage(right_shift_percent);
		HPDF_REAL	top_offset				= __pdf_line;
		HPDF_REAL	bottom_offset			= __pdf_line - __pdf_font_height * number_of_lines;
		HPDF_UINT	number_of_symbols_fit_to_area;


		HPDF_Page_BeginText		(__pdf_page);
		HPDF_Page_SetTextLeading(__pdf_page, font_size);
		HPDF_Page_SetFontAndSize(__pdf_page, current_font, font_size);
		auto hpdf_result = HPDF_Page_TextRect(__pdf_page, left_offset, top_offset, right_offset, bottom_offset, text.c_str(), text_align, &number_of_symbols_fit_to_area);
		HPDF_Page_EndText		(__pdf_page);

		if(hpdf_result == HPDF_PAGE_INSUFFICIENT_SPACE)
		{
			MESSAGE_ERROR("", "", "not whole text fit to the area (" + to_string(number_of_symbols_fit_to_area) + " of " + to_string(text.length()) + ")");
		}

		if(move_line_down)
		{
			for(auto i = 0; i < number_of_lines; ++i)
			{
				error_message = __HPDF_MoveLineDown();
				if(error_message.length())
				{
					MESSAGE_ERROR("", "", error_message);
					break;
				}
			}
		}

	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print text");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;	
}

auto	C_PDF::__HPDF_DrawRect(double left_shift_percent, double top_shift_line, double right_shift_percent, double bottom_shift_line) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (" + to_string(left_shift_percent) + "%, " + to_string(top_shift_line) + " line, " + to_string(right_shift_percent) + "%, " + to_string(bottom_shift_line) + " line)");

	try
	{
		auto	left_coord = __HPDF_GetPixelOffsetByPercentage(left_shift_percent);
		auto	right_coord = __HPDF_GetPixelOffsetByPercentage(right_shift_percent);
		auto	top_coord = __pdf_line - top_shift_line * __pdf_font_height;
		auto	bottom_coord = __pdf_line - bottom_shift_line * __pdf_font_height - __pdf_font_height;

	    HPDF_Page_SetLineWidth (__pdf_page, 1);
		if(HPDF_Page_Rectangle(__pdf_page, left_coord, bottom_coord, right_coord - left_coord, top_coord - bottom_coord) == HPDF_OK) {}
		else
		{
			error_message = gettext("hpdf: fail to draw rectangle");
			MESSAGE_ERROR("", "", error_message);
		}
	    HPDF_Page_Stroke (__pdf_page);
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to draw rectangle");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;	
}


ostream& operator<<(ostream& os, const C_PDF &var)
{
	os << "object C_PDF [empty for now]";

	return os;
}
