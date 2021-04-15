#ifndef __C_PDF_CONST__H__
#define __C_PDF_CONST__H__


#define	HPDF_FIELD_TOP								40
#define	HPDF_FIELD_BOTTOM							40
#define	HPDF_FIELD_RIGHT							40
#define	HPDF_FIELD_LEFT								40
#define	HPDF_TIMECARD_TITLE_WIDTH_PERCENT			35.0
#define	HPDF_TIMECARD_DAYS_WIDTH_PERCENT			(100 - HPDF_TIMECARD_TITLE_WIDTH_PERCENT)
#define	HPDF_TIMECARD_DAY_WIDTH_PERCENT				HPDF_TIMECARD_DAYS_WIDTH_PERCENT / 31.0		// --- important to have double here
#define	HPDF_TIMECARD_FONT_SIZE						8
#define	HPDF_TIMECARD_TABLE_FONT_SIZE				HPDF_TIMECARD_FONT_SIZE - 1
#define	HPDF_TIMECARD_TABLE_SPACING					1
#define	HPDF_TIMECARD_FONT_MULTIPLIER				1.2		// --- font_width calculated by HPDF_Font_TextWidth or HPDF_Page_TextWidth return incorrect width wich is lower than actual space taken by the string. To compensate it space utilization should be corrected with this multiplier.


#endif
