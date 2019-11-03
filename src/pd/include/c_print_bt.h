#ifndef __C_PRINT_BT__H__
#define __C_PRINT_BT__H__

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

#include "utilities_timecard.h" 
#include "c_invoicing_vars.h"
#include "c_bt_to_print.h"
#include "c_float.h"
#include "c_pdf.h"
#include "clog.h"

using namespace std;

class C_Print_BT
{

	protected:
		// CMysql						*db = NULL;
		// CUser						*user = NULL;

		C_Invoicing_Vars				*vars = nullptr;

		string							filename = "";
		string							index = ""; // --- bt index inside global bt list, needed to get access to specific bt.invoicing_vars

		C_BT_To_Print					bt;

		c_float							effort_hours = 0;
		c_float							effort_days = 0;
		c_float							effort_cost = 0;
		c_float							effort_cost_vat = 0;
		c_float							total_payment = 0;

		libxl::Book						*__xls_book = nullptr;
		libxl::Sheet					*__xls_sheet = nullptr;
		int								__xls_row_counter = -1;

		C_PDF							pdf_obj;
/*
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
*/
		auto		GetEffortHours()								{ return effort_hours; };
		auto		GetEffortDays()									{ return effort_days; };
		auto		GetEffortCost()									{ return effort_cost; };
		auto		GetEffortCostVAT()								{ return effort_cost_vat; };
		auto		GetNumberOfDaysInTimecard() -> int;

		auto		GetSpelledTitle() -> string;
		auto		GetSpelledPSoW() -> string;
		auto		GetSpelledProjectID() -> string;
		auto		GetSpelledLocation()							{ return bt.GetLocation().length() ? vars->Get("Location") + ": "s + bt.GetLocation() : ""; };
		auto		GetSpelledSumNonTaxable()						{ return vars->Get("Non-taxable sum") + ": "s; };
		auto		GetSpelledSumTaxable()							{ return vars->Get("Taxable sum") + ": "s; };
		auto		GetSpelledSumTax()								{ return vars->Get("Tax") + " "s + c_float(BUSINESS_TRIP_TAX_PERCENTAGE).PrintPriceTag() + "%" + ": "s; };
		auto		GetSpelledSumExpense()							{ return vars->Get("Sum expense") + ": "s; };
		auto		GetSpelledMarkup()								{ return vars->Get("Markup") + ": "s; };
		auto		GetSpelledTotalPayment()						{ return vars->Get("Total payment in reported timecard") + ": "s; };
		auto		GetSpelledVAT()									{ return vars->Get("VAT") + ": "s; };
		auto		GetSpelledTotalPaymentNoVAT()					{ return vars->Get("Total payment in reported timecard") + " "s + vars->Get("w/o") + " " + vars->Get("VAT") + ": "; };
		auto		GetSpelledSignature()							{ return vars->Get("Signature") + ":"s; };
		auto		GetSpelledInitials()							{ return vars->Get("Initials") + ":"s; };
		// auto		GetSpelledInitials(string idx) -> string;
		auto		GetSpelledPosition()							{ return vars->Get("Title") + ":"s; };
		// auto		GetSpelledPosition(string idx) -> string;
		auto		GetSpelledDate()								{ return vars->Get("Date") + ":"s; };
		auto		GetSpelledRur()									{ return vars->Get("rur."); };
		auto		GetSpelledKop()									{ return vars->Get("kop."); };
		auto		GetSpelledApprovers()							{ return vars->Get("BT approvers") + ":"s; };
		auto		GetTitleDate()									{ return vars->Get("Date"); };
		auto		GetTitleExpense()								{ return vars->Get("Expense"); };
		auto		GetTitleSumRur()								{ return vars->Get("Sum (rur)"); };
		auto		GetTitleSumCurrency()							{ return vars->Get("Sum (currency)"); };
		auto		GetTitleRateExchange()							{ return vars->Get("Rate exchange"); };
		auto		isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };

		auto		__XLS_print_formatted_footer_line(string title, c_float price) -> void;
		auto		__XLS_DrawBorder(int left, int top, int right, int bottom) -> string;
		auto		__XLS_DrawUnderline(int left, int right) -> string;

		auto		__HPDF_DrawHeader() -> string;
		auto		__HPDF_DrawTable() -> string;
		auto		__HPDF_DrawFooter() -> string;
		auto		__HPDF_DrawFooter_SingleLine(string text, c_float number) -> string;
		auto 		__HPDF_PrintApprovers() -> string;
		auto 		__HPDF_PrintSignature() -> string;

		auto		GetBTSumExpenses(string index)					{ return vars->Get("timecard_price_" + index); };

		virtual auto	GetBTMarkup(string index)		-> string	= 0;
		virtual auto	GetBTPaymentNoVAT(string index) -> string	= 0;
		virtual auto	GetBTVAT(string index) 			-> string	= 0;
		virtual auto	GetBTPaymentAndVAT(string index)-> string	= 0;
		virtual auto	GetSignatureTitle1()			-> string	= 0;
		virtual auto	GetSignatureTitle2()			-> string	= 0;

	public:
					C_Print_BT()									{};
					
		auto		GetTotalPayment()								{ return total_payment; };

		auto		SetBT(const C_BT_To_Print &param1) -> void;
		auto		SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto		SetFilename(const string &param1)				{ filename = param1; };
		auto		SetFilename(string &&param1) 					{ filename = move(param1); };

		auto		GetFilename()									{ return filename; }

		auto		PrintAsXLS() -> string;
		auto		PrintAsPDF() -> string;

					~C_Print_BT()									{};
};

class C_Print_BT_To_CC : public C_Print_BT
{
	protected:
		auto		GetBTMarkup(string index)			-> string	{ return vars->Get("cost_center_markup_" + index); };
		auto		GetBTPaymentNoVAT(string index)		-> string	{ return vars->Get("cost_center_price_" + index); };
		auto		GetBTVAT(string index)				-> string	{ return vars->Get("cost_center_vat_" + index); };
		auto		GetBTPaymentAndVAT(string index)	-> string	{ return vars->Get("cost_center_total_" + index); };
		auto		GetSignatureTitle1()				-> string	{ return bt.GetSignatureTitle1(); };
		auto		GetSignatureTitle2()				-> string	{ return bt.GetSignatureTitle2(); };
};

class C_Print_BT_To_Subc : public C_Print_BT
{
	protected:
		auto		GetBTMarkup(string index)			-> string	{ return "0"; };
		auto		GetBTPaymentNoVAT(string index)		-> string	{ return vars->Get("timecard_price_" + index); };
		auto		GetBTVAT(string index)				-> string	{ return vars->Get("timecard_vat_" + index); };
		auto		GetBTPaymentAndVAT(string index)	-> string	{ return vars->Get("timecard_total_" + index); };
		auto		GetSignatureTitle1()				-> string	{ return bt.GetSignatureTitle1(); };
		auto		GetSignatureTitle2()				-> string	{ return bt.GetSignatureTitle2(); };
};

ostream&	operator<<(ostream& os, const C_Print_BT &);

#endif
