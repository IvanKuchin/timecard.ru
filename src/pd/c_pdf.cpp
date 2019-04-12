#include "c_pdf.h"

void c_pdf_error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	char	buffer[512];

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

auto	C_PDF::Restart() -> void
{
	__pdf_line = -1;
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
		MESSAGE_ERROR("", "", "hpdf: fail save to file");
		error_message = gettext("hpdf: fail save to file");
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

auto	C_PDF::__HPDF_MoveLineDown(int line_decrement) -> string
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

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_PDF::__HPDF_DrawHorizontalLine(int x, int line_length) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	try
	{
		HPDF_Page_MoveTo (__pdf_page, x, __pdf_line);
		HPDF_Page_LineTo (__pdf_page, x + line_length, __pdf_line);
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




ostream& operator<<(ostream& os, const C_PDF &var)
{
	os << "object C_PDF [empty for now]";

	return os;
}





