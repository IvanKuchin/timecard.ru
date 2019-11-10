#ifndef __C_PRINT_VAT__H__
#define __C_PRINT_VAT__H__

#include <string>
#include <vector>
#include <locale>
#include <algorithm>    // std::max

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "codecvt/mbwcvt.hpp"
#pragma GCC diagnostic pop 

#define _UNICODE
#include "libxl.h"
#undef _UNICODE

// #include "hpdf.h"

#include "utilities_timecard.h" 
#include "c_price_spelling.h"
#include "c_timecard_to_print.h"
#include "c_invoicing_vars.h"
#include "c_float.h"
#include "c_pdf.h"
#include "clog.h"

using namespace std;

class C_Print_VAT_Base
{
	protected:
		CMysql							*db = NULL;
		C_Invoicing_Vars				*vars;

		string							filename = "";
		string							cost_center_id = "";

		libxl::Sheet					*__sheet = nullptr;
		libxl::Book						*__book = nullptr;
		int								__row_counter = 1;

		C_PDF							pdf_obj;
		int								__pdf_font_size = HPDF_TIMECARD_FONT_SIZE;

		int								total_table_items = 0;

		auto			__PrintXLSHeader() -> string;
		auto			__PrintXLSTable() -> string;
		auto			__PrintXLSSignature() -> string;

		auto			GetSupplierFullAddress() -> string;
		auto			GetCustomerFullAddress() -> string;
		auto			SpellTotalItemsAndSum()							{ return vars->Get("Sum items") + " " + to_string(total_table_items) + ", " + vars->Get("total amount") + " " + GetTableTotal() + " " + vars->Get("rub."); };
		auto			SpellPrice() -> string;
		auto			MergeApplyFormat(int start_col, int end_col, libxl::Format* fmt) -> string;

		auto			__HPDF_DrawTable_Header() -> string;
		auto			__HPDF_DrawTable_Body() -> string;
		auto			__HPDF_DrawTable_Footer() -> string;
		auto			__HPDF_DrawHeader() -> string;
		auto			__HPDF_DrawTable() -> string;
		auto			__HPDF_DrawFooter() -> string;
	public:

		auto			Restart() -> void;
		auto			SetDB(CMysql *param)							{ db = param; };
		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto			SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };
		auto			SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto			GetFilename()									{ return filename; }

		auto			PrintAsXLS() -> string;
		auto			PrintAsPDF() -> string;

		virtual auto	PrintXLSHeader() -> string						= 0;
		virtual auto	PrintXLSFooter() -> string						= 0;

		virtual auto	PrintPDFHeader() -> string						= 0;
		virtual auto	PrintPDFFooter() -> string						= 0;

		virtual auto	GetDocumentTitle() -> string					= 0;

		virtual auto	GetSupplierName() -> string						= 0;
		virtual auto	GetSupplierBankName() -> string					= 0;
		virtual auto	GetSupplierBankAccount() -> string				= 0;
		virtual auto	GetSupplierAccount() -> string					= 0;
		virtual auto	GetSupplierBIK() -> string						= 0;
		virtual auto	GetSupplierTIN() -> string						= 0;
		virtual auto	GetSupplierVAT() -> string						= 0;
		virtual auto	GetSupplierVATSpelling() -> string				= 0;
		virtual auto	GetSupplierVATSpellingShort() -> string			= 0;
		virtual auto	GetSupplierKPP() -> string						= 0;
		virtual auto	GetSupplierOGRN() -> string						= 0;
		virtual auto	GetSupplierLegalZIP() -> string					= 0;
		virtual auto	GetSupplierLegalLocality() -> string			= 0;
		virtual auto	GetSupplierLegalAddress() -> string				= 0;
		virtual auto	GetCustomerName() -> string						= 0;
		virtual auto	GetCustomerBankName() -> string					= 0;
		virtual auto	GetCustomerBankAccount() -> string				= 0;
		virtual auto	GetCustomerAccount() -> string					= 0;
		virtual auto	GetCustomerBIK() -> string						= 0;
		virtual auto	GetCustomerTIN() -> string						= 0;
		virtual auto	GetCustomerKPP() -> string						= 0;
		virtual auto	GetCustomerOGRN() -> string						= 0;
		virtual auto	GetCustomerLegalZIP() -> string					= 0;
		virtual auto	GetCustomerLegalLocality() -> string			= 0;
		virtual auto	GetCustomerLegalAddress() -> string				= 0;
		virtual	auto	isTableRowExists(int i) -> bool					= 0;
		virtual	auto	GetTableRowDescription(int i) -> string			= 0;
		virtual	auto	GetTableRowIndex(int i) -> string				= 0;
		virtual	auto	GetTableRowQuantity(int i) -> string			= 0;
		virtual	auto	GetTableRowItem(int i) -> string				= 0;
		virtual	auto	GetTableRowPrice(int i) -> string				= 0;
		virtual	auto	GetTableRowVAT(int i) -> string					= 0;
		virtual	auto	GetTableRowTotal(int i) -> string				= 0;
		virtual	auto	GetTableSum() -> string							= 0;
		virtual	auto	GetTableVAT() -> string							= 0;
		virtual	auto	GetTableTotal() -> string						= 0;
		virtual	auto	GetSignatureTitle1() -> string					= 0;
		virtual	auto	GetSignatureTitle2() -> string					= 0;
		virtual	auto	GetSignatureName1() -> string					= 0;
		virtual	auto	GetSignatureName2() -> string					= 0;
		virtual	auto	GetSignatureInfo1() -> string					= 0;
		virtual	auto	GetSignatureInfo2() -> string					= 0;
};

ostream&	operator<<(ostream& os, const C_Print_VAT_Base &);

class C_Print_VAT_Agency : public C_Print_VAT_Base
{
	public:
		auto			PrintXLSHeader() -> string						{ return __PrintXLSHeader(); };
		auto			PrintXLSFooter() -> string						{ return ""s; };

		auto			PrintPDFHeader() -> string						{ return ""s; };
		auto			PrintPDFFooter() -> string						{ return ""s; };

		auto			GetDocumentTitle() -> string					{ return vars->Get("VAT doc") + " " + vars->Get("invoice_agreement"); };
		auto			GetSupplierName() -> string						{ return vars->Get("agency_name"); };
		auto			GetSupplierBankName() -> string					{ return vars->Get("agency_bank_title"); };
		auto			GetSupplierBankAccount() -> string				{ return vars->Get("agency_bank_account"); };
		auto			GetSupplierAccount() -> string					{ return vars->Get("agency_account"); };
		auto			GetSupplierBIK() -> string						{ return vars->Get("agency_bank_bik"); };
		auto			GetSupplierTIN() -> string						{ return vars->Get("agency_tin"); };
		auto			GetSupplierVAT() -> string						{ return vars->Get("agency_vat"); };
		auto			GetSupplierVATSpelling() -> string				{ return vars->Get("agency_vat_spelling"); };
		auto			GetSupplierVATSpellingShort() -> string			{ return vars->Get("agency_vat_spelling_short"); };
		auto			GetSupplierKPP() -> string						{ return vars->Get("agency_kpp"); };
		auto			GetSupplierOGRN() -> string						{ return vars->Get("agency_ogrn"); };
		auto			GetSupplierLegalZIP() -> string					{ return vars->Get("agency_legal_geo_zip"); };
		auto			GetSupplierLegalLocality() -> string			{ return vars->Get("agency_legal_locality_title"); };
		auto			GetSupplierLegalAddress() -> string				{ return vars->Get("agency_legal_address"); };
		auto			GetSupplierMailingZIP() -> string				{ return vars->Get("agency_mailing_geo_zip"); };
		auto			GetSupplierMailingLocality() -> string			{ return vars->Get("agency_mailing_locality_title"); };
		auto			GetSupplierMailingAddress() -> string			{ return vars->Get("agency_mailing_address"); };
		auto			GetCustomerName() -> string						{ return vars->Get("cost_center_name"); };
		auto			GetCustomerBankName() -> string					{ return vars->Get("cost_center_bank_title"); };
		auto			GetCustomerBankAccount() -> string				{ return vars->Get("cost_center_bank_account"); };
		auto			GetCustomerAccount() -> string					{ return vars->Get("cost_center_account"); };
		auto			GetCustomerBIK() -> string						{ return vars->Get("cost_center_bank_bik"); };
		auto			GetCustomerTIN() -> string						{ return vars->Get("cost_center_tin"); };
		auto			GetCustomerKPP() -> string						{ return vars->Get("cost_center_kpp"); };
		auto			GetCustomerOGRN() -> string						{ return vars->Get("cost_center_ogrn"); };
		auto			GetCustomerLegalZIP() -> string					{ return vars->Get("cost_center_legal_geo_zip"); };
		auto			GetCustomerLegalLocality() -> string			{ return vars->Get("cost_center_legal_locality_title"); };
		auto			GetCustomerLegalAddress() -> string				{ return vars->Get("cost_center_legal_address"); };
		auto			GetCustomerMailingZIP() -> string				{ return vars->Get("cost_center_mailing_geo_zip"); };
		auto			GetCustomerMailingLocality() -> string			{ return vars->Get("cost_center_mailing_locality_title"); };
		auto			GetCustomerMailingAddress() -> string			{ return vars->Get("cost_center_mailing_address"); };

		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };
		auto			GetTableRowDescription(int i) -> string			{ return vars->Get("cost_center_table_row_description_" + to_string(i)); };

		auto			GetTableRowIndex(int i) -> string				{ return vars->Get("index_" + to_string(i)); };
		auto			GetTableRowQuantity(int i) -> string			{ return vars->Get("quantity_" + to_string(i)); };
		auto			GetTableRowItem(int i) -> string				{ return vars->Get("item_" + to_string(i)); };
		auto			GetTableRowPrice(int i) -> string				{ return c_float(vars->Get("cost_center_price_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowVAT(int i) -> string					{ return c_float(vars->Get("cost_center_vat_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowTotal(int i) -> string				{ return c_float(vars->Get("cost_center_total_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableSum() -> string							{ return c_float(vars->Get("cost_center_sum_amount")).PrintPriceTag(); };
		auto			GetTableVAT() -> string							{ return c_float(vars->Get("cost_center_vat_amount")).PrintPriceTag(); };
		auto			GetTableTotal() -> string						{ return c_float(vars->Get("cost_center_total_payment")).PrintPriceTag(); };

		auto			GetSignatureTitle1() -> string					{ return vars->Get("Director"); };
		auto			GetSignatureTitle2() -> string					{ return vars->Get("Accountant"); };
		auto			GetSignatureName1() -> string					{ return vars->Get("invoice_signature_name1"); };
		auto			GetSignatureName2() -> string					{ return vars->Get("invoice_signature_name2"); };
		auto			GetSignatureInfo1() -> string					{ return vars->Get("invoice_signature_info1"); };
		auto			GetSignatureInfo2() -> string					{ return vars->Get("invoice_signature_info2"); };
};

class C_Print_VAT_Subc : public C_Print_VAT_Base
{
	public:
		auto			PrintXLSHeader() -> string						{ return __PrintXLSHeader(); };
		auto			PrintXLSFooter() -> string						{ return ""s; };

		auto			PrintPDFHeader() -> string						{ return ""s; };
		auto			PrintPDFFooter() -> string						{ return ""s; };

		auto			GetDocumentTitle() -> string					{ return vars->Get("VAT doc") + " " + vars->Get("invoice_agreement"); };
		auto			GetSupplierName() -> string						{ return vars->Get("subcontractor_company_name_1"); };
		auto			GetSupplierBankName() -> string					{ return vars->Get("subcontractor_bank_title_1"); };
		auto			GetSupplierBankAccount() -> string				{ return vars->Get("subcontractor_bank_account_1"); };
		auto			GetSupplierAccount() -> string					{ return vars->Get("subcontractor_company_account_1"); };
		auto			GetSupplierBIK() -> string						{ return vars->Get("subcontractor_bank_bik_1"); };
		auto			GetSupplierTIN() -> string						{ return vars->Get("subcontractor_company_tin_1"); };

		auto			GetSupplierVAT() -> string						{ return vars->Get("subcontractor_company_vat_1"); };
		auto			GetSupplierVATSpelling() -> string				{ return vars->Get("subcontractor_company_vat_spelling_1"); };
		auto			GetSupplierVATSpellingShort() -> string			{ return vars->Get("subcontractor_company_vat_spelling_short_1"); };

		auto			GetSupplierKPP() -> string						{ return vars->Get("subcontractor_company_kpp_1"); };
		auto			GetSupplierOGRN() -> string						{ return vars->Get("subcontractor_company_ogrn_1"); };
		auto			GetSupplierLegalZIP() -> string					{ return vars->Get("subcontractor_legal_geo_zip_1"); };
		auto			GetSupplierLegalLocality() -> string			{ return vars->Get("subcontractor_legal_locality_title_1"); };
		auto			GetSupplierLegalAddress() -> string				{ return vars->Get("subcontractor_legal_address_1"); };
		auto			GetSupplierMailingZIP() -> string				{ return vars->Get("subcontractor_mailing_geo_zip_1"); };
		auto			GetSupplierMailingLocality() -> string			{ return vars->Get("subcontractor_mailing_locality_title_1"); };
		auto			GetSupplierMailingAddress() -> string			{ return vars->Get("subcontractor_mailing_address_1"); };
		auto			GetCustomerName() -> string						{ return vars->Get("agency_name"); };
		auto			GetCustomerBankName() -> string					{ return vars->Get("agency_bank_title"); };
		auto			GetCustomerBankAccount() -> string				{ return vars->Get("agency_bank_account"); };
		auto			GetCustomerAccount() -> string					{ return vars->Get("agency_account"); };
		auto			GetCustomerBIK() -> string						{ return vars->Get("agency_bank_bik"); };
		auto			GetCustomerTIN() -> string						{ return vars->Get("agency_tin"); };
		auto			GetCustomerKPP() -> string						{ return vars->Get("agency_kpp"); };
		auto			GetCustomerOGRN() -> string						{ return vars->Get("agency_ogrn"); };
		auto			GetCustomerLegalZIP() -> string					{ return vars->Get("agency_legal_geo_zip"); };
		auto			GetCustomerLegalLocality() -> string			{ return vars->Get("agency_legal_locality_title"); };
		auto			GetCustomerLegalAddress() -> string				{ return vars->Get("agency_legal_address"); };
		auto			GetCustomerMailingZIP() -> string				{ return vars->Get("agency_mailing_geo_zip"); };
		auto			GetCustomerMailingLocality() -> string			{ return vars->Get("agency_mailing_locality_title"); };
		auto			GetCustomerMailingAddress() -> string			{ return vars->Get("agency_mailing_address"); };

		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };
		auto			GetTableRowDescription(int i) -> string			{ return vars->Get("subcontractor_table_row_description_" + to_string(i)); };
		auto			GetTableRowIndex(int i) -> string				{ return vars->Get("index_" + to_string(i)); };
		auto			GetTableRowQuantity(int i) -> string			{ return vars->Get("quantity_" + to_string(i)); };
		auto			GetTableRowItem(int i) -> string				{ return vars->Get("item_" + to_string(i)); };
		auto			GetTableRowPrice(int i) -> string				{ return c_float(vars->Get("timecard_price_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowVAT(int i) -> string					{ return c_float(vars->Get("timecard_vat_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowTotal(int i) -> string				{ return c_float(vars->Get("timecard_total_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableSum() -> string							{ return c_float(vars->Get("subcontractors_sum_amount")).PrintPriceTag(); };
		auto			GetTableVAT() -> string							{ return c_float(vars->Get("subcontractors_vat_amount")).PrintPriceTag(); };
		auto			GetTableTotal() -> string						{ return c_float(vars->Get("subcontractors_total_payment")).PrintPriceTag(); };

		auto			GetSignatureTitle1() -> string					{ return vars->Get("Director"); };
		auto			GetSignatureTitle2() -> string					{ return vars->Get("Accountant"); };
		auto			GetSignatureName1() -> string					{ return vars->Get("subc2agency_vat_signature1"); };
		auto			GetSignatureName2() -> string					{ return vars->Get("subc2agency_vat_signature2"); };
		auto			GetSignatureInfo1() -> string					{ return vars->Get("subc2agency_vat_position1"); };
		auto			GetSignatureInfo2() -> string					{ return vars->Get("subc2agency_vat_position2"); };
};

#endif
