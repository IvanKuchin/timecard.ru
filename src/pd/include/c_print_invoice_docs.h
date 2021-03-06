#ifndef __C_PRINT_INVOICE_DOCS__H__
#define __C_PRINT_INVOICE_DOCS__H__

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

#include "utilities_timecard.h" 
#include "c_price_spelling.h"
#include "c_timecard_to_print.h"
#include "c_invoicing_vars.h"
#include "c_float.h"
#include "c_pdf.h"
#include "clog.h"

using namespace std;

#define LIBXL_DEFAULT_ROW_HEIGHT					12.75

class C_Print_Invoice_Docs_Base
{
	protected:
		CMysql							*db = NULL;
		C_Invoicing_Vars				*vars;

		string							filename = "";
		string							cost_center_id = "";

		string							xl_username = "";
		string							xl_key = "";

		libxl::Sheet					*__sheet = nullptr;
		libxl::Book						*__book = nullptr;
		int								__row_counter = 1;

		C_PDF							pdf_obj;

		int								total_table_items = 0;

		auto			GetSupplierCompanyDetails() -> string;
		auto			GetCustomerCompanyDetails() -> string;
		auto			SpellTotalItemsAndSum()							{ return vars->Get("Sum items") + " " + to_string(total_table_items) + ", " + vars->Get("total amount") + " " + GetTableTotal() + " " + vars->Get("rub."); };
		auto			SpellPrice() -> string;

		auto			__PrintXLSComment() -> string;
		virtual	auto	PrintXLSComment() -> string						= 0;

	public:

		auto			Restart() -> void;
		auto			SetDB(CMysql *param)							{ db = param; };
		auto			SetFilename(const string &param1)				{ filename = param1; };
		auto			SetFilename(string &&param1) 					{ filename = move(param1); };
		auto			SetCostCenterID(const string &param1)			{ cost_center_id = param1; };
		auto			SetCostCenterID(string &&param1) 				{ cost_center_id = move(param1); };
		auto			SetVariableSet(C_Invoicing_Vars *param)			{ vars = param; };

		auto			GetFilename()									{ return filename; }

		auto			xlSetKey(const string &p1, const string &p2)	{ xl_username = p1; xl_key = p2; };
		auto			PrintAsXLS() -> string;
		auto			__XLS_DrawBorder(int left, int top, int right, int bottom) -> string;
		auto			__DrawXLSRowUnderline() -> string;
		auto			__PrintXLSHeaderTable() -> string;
		auto			__PrintXLSSignature() -> string;

		auto			PrintAsPDF() -> string;
		auto			__PrintPDFHeaderTable() -> string;
		auto			__HPDF_DrawHeader() -> string;
		auto			__HPDF_DrawTable() -> string;
		auto			__HPDF_DrawFooter() -> string;
		auto			__HPDF_PrintSignature() -> string;

		virtual auto	PrintXLSHeader() -> string						= 0;
		virtual auto	PrintXLSFooter() -> string						= 0;

		virtual auto	PrintPDFHeader() -> string						= 0;
		virtual auto	PrintPDFFooter() -> string						= 0;

		virtual auto	GetDocumentTitle() -> string					= 0;

		virtual auto	GetSupplierSpelling() -> string					= 0;
		virtual auto	GetSupplierName() -> string						= 0;
		virtual auto	GetSupplierBankName() -> string					= 0;
		virtual auto	GetSupplierBankAccount() -> string				= 0;
		virtual auto	GetSupplierAccount() -> string					= 0;
		virtual auto	GetSupplierBIK() -> string						= 0;
		virtual auto	GetSupplierTIN() -> string						= 0;
		virtual auto	GetSupplierKPP() -> string						= 0;
		virtual auto	GetSupplierOGRN() -> string						= 0;
		virtual auto	GetSupplierLegalZIP() -> string					= 0;
		virtual auto	GetSupplierLegalLocality() -> string			= 0;
		virtual auto	GetSupplierLegalAddress() -> string				= 0;
		virtual auto	GetCustomerSpelling() -> string					= 0;
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

		virtual	auto	GetTableMainTitleSpelling() -> string			= 0;
		virtual	auto	isTableRowExists(int i) -> bool					= 0;
		virtual	auto	GetTableRowDescription(int i) -> string			= 0;
		virtual	auto	GetTableRowIndex(int i) -> string				= 0;
		virtual	auto	GetTableRowQuantity(int i) -> string			= 0;
		virtual	auto	GetTableRowItem(int i) -> string				= 0;
		virtual	auto	GetTableRowPrice(int i) -> string				= 0;
		virtual	auto	GetTableRowTotal(int i) -> string				= 0;
		virtual	auto	GetTableSum() -> string							= 0;
		virtual	auto	GetTableVAT() -> string							= 0;
		virtual	auto	GetTableTotal() -> string						= 0;

		virtual	auto	GetTotalPaymentSpelling() -> string				= 0;
		virtual	auto	GetFooterComment() -> string					= 0;

		virtual	auto	GetSignatureTitle1() -> string					= 0;
		virtual	auto	GetSignatureTitle2() -> string					= 0;
		virtual	auto	GetSignatureName1() -> string					= 0;
		virtual	auto	GetSignatureName2() -> string					= 0;
		virtual	auto	GetSignatureInfo1() -> string					= 0;
		virtual	auto	GetSignatureInfo2() -> string					= 0;

						// ~C_Print_Invoice_Docs_Base()						{ if(__pdf) { HPDF_Free(__pdf); }; };
};

ostream&	operator<<(ostream& os, const C_Print_Invoice_Docs_Base &);

class C_Print_Invoice_Agency : public C_Print_Invoice_Docs_Base
{
	protected:
		auto			PrintXLSComment() -> string						{ return ""s; }

	public:

		auto			PrintXLSHeader() -> string						{ return __PrintXLSHeaderTable(); };
		auto			PrintXLSFooter() -> string						{ return ""s; };

		auto			PrintPDFHeader() -> string						{ return __PrintPDFHeaderTable(); };
		auto			PrintPDFFooter() -> string						{ return ""s; };

		auto			GetDocumentTitle() -> string					{ return vars->Get("Invoice") + " " + vars->Get("invoice_agreement"); };
		auto			GetSupplierSpelling() -> string					{ return vars->Get("Provider") + " (" + vars->Get("Implementor") + ")"; };
		auto			GetSupplierName() -> string						{ return vars->Get("agency_name"); };
		auto			GetSupplierBankName() -> string					{ return vars->Get("agency_bank_title"); };
		auto			GetSupplierBankAccount() -> string				{ return vars->Get("agency_bank_account"); };
		auto			GetSupplierAccount() -> string					{ return vars->Get("agency_account"); };
		auto			GetSupplierBIK() -> string						{ return vars->Get("agency_bank_bik"); };
		auto			GetSupplierTIN() -> string						{ return vars->Get("agency_tin"); };
		auto			GetSupplierKPP() -> string						{ return vars->Get("agency_kpp"); };
		auto			GetSupplierOGRN() -> string						{ return vars->Get("agency_ogrn"); };
		auto			GetSupplierLegalZIP() -> string					{ return vars->Get("agency_legal_geo_zip"); };
		auto			GetSupplierLegalLocality() -> string			{ return vars->Get("agency_legal_locality_title"); };
		auto			GetSupplierLegalAddress() -> string				{ return vars->Get("agency_legal_address"); };
		auto			GetSupplierMailingZIP() -> string				{ return vars->Get("agency_mailing_geo_zip"); };
		auto			GetSupplierMailingLocality() -> string			{ return vars->Get("agency_mailing_locality_title"); };
		auto			GetSupplierMailingAddress() -> string			{ return vars->Get("agency_mailing_address"); };
		auto			GetCustomerSpelling() -> string					{ return vars->Get("Buyer") + " (" + vars->Get("Customer") + ")"; };
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

		auto			GetTableMainTitleSpelling() -> string			{ return vars->Get("invoice_table_header"); };
		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };
		auto			GetTableRowDescription(int i) -> string			{ return vars->Get("cost_center_table_row_description_" + to_string(i)); };
		auto			GetTableRowIndex(int i) -> string				{ return vars->Get("index_" + to_string(i)); };
		auto			GetTableRowQuantity(int i) -> string			{ return vars->Get("quantity_" + to_string(i)); };
		auto			GetTableRowItem(int i) -> string				{ return vars->Get("item_" + to_string(i)); };
		auto			GetTableRowPrice(int i) -> string				{ return c_float(vars->Get("cost_center_price_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowTotal(int i) -> string				{ return c_float(vars->Get("cost_center_total_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableSum() -> string							{ return c_float(vars->Get("cost_center_sum_amount")).PrintPriceTag(); };
		auto			GetTableVAT() -> string							{ return c_float(vars->Get("cost_center_vat_amount")).PrintPriceTag(); };
		auto			GetTableTotal() -> string						{ return c_float(vars->Get("cost_center_total_payment")).PrintPriceTag(); };

		auto			GetTotalPaymentSpelling() -> string				{ return vars->Get("Total payment") + ": "; };
		auto			GetFooterComment() -> string					{ return ""s; };

		auto			GetSignatureTitle1() -> string					{ return vars->Get("invoice_role1"); };
		auto			GetSignatureTitle2() -> string					{ return vars->Get("invoice_role2"); };
		auto			GetSignatureName1() -> string					{ return vars->Get("agency_name"); };
		auto			GetSignatureName2() -> string					{ return vars->Get("agency_name"); };
		auto			GetSignatureInfo1() -> string					{ return vars->Get("invoice_signature_info1") + " " + vars->Get("invoice_signature_name1"); };
		auto			GetSignatureInfo2() -> string					{ return vars->Get("invoice_signature_info2") + " " + vars->Get("invoice_signature_name2"); };
};

class C_Print_Invoice_Subc : public C_Print_Invoice_Docs_Base
{
	protected:
		auto			PrintXLSComment() -> string						{ return ""s; }

	public:

		auto			PrintXLSHeader() -> string						{ return __PrintXLSHeaderTable(); };
		auto			PrintXLSFooter() -> string						{ return ""s; };

		auto			PrintPDFHeader() -> string						{ return __PrintPDFHeaderTable(); };
		auto			PrintPDFFooter() -> string						{ return ""s; };

		auto			GetDocumentTitle() -> string					{ return vars->Get("Invoice") + " " + vars->Get("invoice_agreement"); };
		auto			GetSupplierSpelling() -> string					{ return vars->Get("Provider") + " (" + vars->Get("Implementor") + ")"; };
		auto			GetSupplierName() -> string						{ return vars->Get("subcontractor_company_name_1"); };
		auto			GetSupplierBankName() -> string					{ return vars->Get("subcontractor_bank_title_1"); };
		auto			GetSupplierBankAccount() -> string				{ return vars->Get("subcontractor_bank_account_1"); };
		auto			GetSupplierAccount() -> string					{ return vars->Get("subcontractor_company_account_1"); };
		auto			GetSupplierBIK() -> string						{ return vars->Get("subcontractor_bank_bik_1"); };
		auto			GetSupplierTIN() -> string						{ return vars->Get("subcontractor_company_tin_1"); };
		auto			GetSupplierKPP() -> string						{ return vars->Get("subcontractor_company_kpp_1"); };
		auto			GetSupplierOGRN() -> string						{ return vars->Get("subcontractor_company_ogrn_1"); };
		auto			GetSupplierLegalZIP() -> string					{ return vars->Get("subcontractor_legal_geo_zip_1"); };
		auto			GetSupplierLegalLocality() -> string			{ return vars->Get("subcontractor_legal_locality_title_1"); };
		auto			GetSupplierLegalAddress() -> string				{ return vars->Get("subcontractor_legal_address_1"); };
		auto			GetSupplierMailingZIP() -> string				{ return vars->Get("subcontractor_mailing_geo_zip_1"); };
		auto			GetSupplierMailingLocality() -> string			{ return vars->Get("subcontractor_mailing_locality_title_1"); };
		auto			GetSupplierMailingAddress() -> string			{ return vars->Get("subcontractor_mailing_address_1"); };
		auto			GetCustomerSpelling() -> string					{ return vars->Get("Buyer") + " (" + vars->Get("Customer") + ")"; };
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

		auto			GetTableMainTitleSpelling() -> string			{ return vars->Get("invoice_table_header"); };
		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };
		auto			GetTableRowDescription(int i) -> string			{ return vars->Get("subcontractor_table_row_description_" + to_string(i)); };
		auto			GetTableRowIndex(int i) -> string				{ return vars->Get("index_" + to_string(i)); };
		auto			GetTableRowQuantity(int i) -> string			{ return vars->Get("quantity_" + to_string(i)); };
		auto			GetTableRowItem(int i) -> string				{ return vars->Get("item_" + to_string(i)); };
		auto			GetTableRowPrice(int i) -> string				{ return c_float(vars->Get("timecard_price_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowTotal(int i) -> string				{ return c_float(vars->Get("timecard_total_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableSum() -> string							{ return c_float(vars->Get("subcontractors_sum_amount")).PrintPriceTag(); };
		auto			GetTableVAT() -> string							{ return c_float(vars->Get("subcontractors_vat_amount")).PrintPriceTag(); };
		auto			GetTableTotal() -> string						{ return c_float(vars->Get("subcontractors_total_payment")).PrintPriceTag(); };

		auto			GetTotalPaymentSpelling() -> string				{ return vars->Get("Total payment") + ": "; };
		auto			GetFooterComment() -> string					{ return ""s; };

		auto			GetSignatureName1() -> string					{ return vars->Get("subcontractor_company_name_1"); };
		auto			GetSignatureName2() -> string					{ return vars->Get("subcontractor_company_name_1"); };
		auto			GetSignatureTitle1() -> string
						{
							return	vars->Get("subc2agency_invoice_role1_1").length()
									? vars->Get("subc2agency_invoice_role1_1")
									: vars->Get("subc2agency_invoice_role1");
						};
		auto			GetSignatureTitle2() -> string
						{
							return	vars->Get("subc2agency_invoice_role2_1").length()
									? vars->Get("subc2agency_invoice_role2_1")
									: vars->Get("subc2agency_invoice_role2");
						};
		auto			GetSignatureInfo1() -> string					
						{
							return	vars->Get("subc2agency_invoice_position1_1").length() + vars->Get("subc2agency_invoice_signature1_1").length()
									? vars->Get("subc2agency_invoice_position1_1") + " " + vars->Get("subc2agency_invoice_signature1_1")
									: vars->Get("subc2agency_invoice_position1") + " " + vars->Get("subc2agency_invoice_signature1");
						};
		auto			GetSignatureInfo2() -> string					
						{
							return	vars->Get("subc2agency_invoice_position2_1").length() + vars->Get("subc2agency_invoice_signature2_1").length()
									? vars->Get("subc2agency_invoice_position2_1") + " " + vars->Get("subc2agency_invoice_signature2_1")
									: vars->Get("subc2agency_invoice_position2") + " " + vars->Get("subc2agency_invoice_signature2");
						};
};

class C_Print_Act_Agency : public C_Print_Invoice_Docs_Base
{
	protected:
		auto			PrintXLSComment() -> string						{ return __PrintXLSComment(); }

	public:

		auto			PrintXLSHeader() -> string						{ return ""s; };
		auto			PrintXLSFooter() -> string						{ return ""s; };

		auto			PrintPDFHeader() -> string						{ return ""s; };
		auto			PrintPDFFooter() -> string						{ return ""s; };

		auto			GetDocumentTitle() -> string					{ return vars->Get("Act") + " " + vars->Get("invoice_agreement"); };
		auto			GetSupplierSpelling() -> string					{ return vars->Get("Implementor"); };
		auto			GetSupplierName() -> string						{ return vars->Get("agency_name"); };
		auto			GetSupplierBankName() -> string					{ return vars->Get("agency_bank_title"); };
		auto			GetSupplierBankAccount() -> string				{ return vars->Get("agency_bank_account"); };
		auto			GetSupplierAccount() -> string					{ return vars->Get("agency_account"); };
		auto			GetSupplierBIK() -> string						{ return vars->Get("agency_bank_bik"); };
		auto			GetSupplierTIN() -> string						{ return vars->Get("agency_tin"); };
		auto			GetSupplierKPP() -> string						{ return vars->Get("agency_kpp"); };
		auto			GetSupplierOGRN() -> string						{ return vars->Get("agency_ogrn"); };
		auto			GetSupplierLegalZIP() -> string					{ return vars->Get("agency_legal_geo_zip"); };
		auto			GetSupplierLegalLocality() -> string			{ return vars->Get("agency_legal_locality_title"); };
		auto			GetSupplierLegalAddress() -> string				{ return vars->Get("agency_legal_address"); };
		auto			GetSupplierMailingZIP() -> string				{ return vars->Get("agency_mailing_geo_zip"); };
		auto			GetSupplierMailingLocality() -> string			{ return vars->Get("agency_mailing_locality_title"); };
		auto			GetSupplierMailingAddress() -> string			{ return vars->Get("agency_mailing_address"); };
		auto			GetCustomerSpelling() -> string					{ return vars->Get("Customer"); };
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

		auto			GetTableMainTitleSpelling() -> string			{ return vars->Get("act_table_header"); };
		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };
		auto			GetTableRowDescription(int i) -> string			{ return vars->Get("cost_center_table_row_description_" + to_string(i)); };
		auto			GetTableRowIndex(int i) -> string				{ return vars->Get("index_" + to_string(i)); };
		auto			GetTableRowQuantity(int i) -> string			{ return vars->Get("quantity_" + to_string(i)); };
		auto			GetTableRowItem(int i) -> string				{ return vars->Get("item_" + to_string(i)); };
		auto			GetTableRowPrice(int i) -> string				{ return c_float(vars->Get("cost_center_price_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowTotal(int i) -> string				{ return c_float(vars->Get("cost_center_total_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableSum() -> string							{ return c_float(vars->Get("cost_center_sum_amount")).PrintPriceTag(); };
		auto			GetTableVAT() -> string							{ return c_float(vars->Get("cost_center_vat_amount")).PrintPriceTag(); };
		auto			GetTableTotal() -> string						{ return c_float(vars->Get("cost_center_total_payment")).PrintPriceTag(); };

		auto			GetTotalPaymentSpelling() -> string				{ return ""; };
		auto			GetFooterComment() -> string					{ return vars->Get("act_footnote"); };

		auto			GetSignatureTitle1() -> string					{ return vars->Get("act_role1"); };
		auto			GetSignatureTitle2() -> string					{ return vars->Get("act_role2"); };
		auto			GetSignatureName1() -> string					{ return vars->Get("agency_name"); };
		auto			GetSignatureName2() -> string					{ return vars->Get("cost_center_name"); };
		auto			GetSignatureInfo1() -> string					{ return vars->Get("act_signature_info1") + " " + vars->Get("act_signature_name1"); };
		auto			GetSignatureInfo2() -> string					{ return vars->Get("act_signature_info2") + " " + vars->Get("act_signature_name2"); };
};

class C_Print_Act_Subc : public C_Print_Invoice_Docs_Base
{
	protected:
		auto			PrintXLSComment() -> string						{ return __PrintXLSComment(); }

	public:

		auto			PrintXLSHeader() -> string						{ return ""s; };
		auto			PrintXLSFooter() -> string						{ return ""s; };

		auto			PrintPDFHeader() -> string						{ return ""s; };
		auto			PrintPDFFooter() -> string						{ return ""s; };

		auto			GetDocumentTitle() -> string					{ return vars->Get("Act") + " " + vars->Get("invoice_agreement"); };
		auto			GetSupplierSpelling() -> string					{ return vars->Get("Implementor"); };
		auto			GetSupplierName() -> string						{ return vars->Get("subcontractor_company_name_1"); };
		auto			GetSupplierBankName() -> string					{ return vars->Get("subcontractor_bank_title_1"); };
		auto			GetSupplierBankAccount() -> string				{ return vars->Get("subcontractor_bank_account_1"); };
		auto			GetSupplierAccount() -> string					{ return vars->Get("subcontractor_company_account_1"); };
		auto			GetSupplierBIK() -> string						{ return vars->Get("subcontractor_bank_bik_1"); };
		auto			GetSupplierTIN() -> string						{ return vars->Get("subcontractor_company_tin_1"); };
		auto			GetSupplierKPP() -> string						{ return vars->Get("subcontractor_company_kpp_1"); };
		auto			GetSupplierOGRN() -> string						{ return vars->Get("subcontractor_company_ogrn_1"); };
		auto			GetSupplierLegalZIP() -> string					{ return vars->Get("subcontractor_legal_geo_zip_1"); };
		auto			GetSupplierLegalLocality() -> string			{ return vars->Get("subcontractor_legal_locality_title_1"); };
		auto			GetSupplierLegalAddress() -> string				{ return vars->Get("subcontractor_legal_address_1"); };
		auto			GetSupplierMailingZIP() -> string				{ return vars->Get("subcontractor_mailing_geo_zip_1"); };
		auto			GetSupplierMailingLocality() -> string			{ return vars->Get("subcontractor_mailing_locality_title_1"); };
		auto			GetSupplierMailingAddress() -> string			{ return vars->Get("subcontractor_mailing_address_1"); };
		auto			GetCustomerSpelling() -> string					{ return vars->Get("Customer"); };
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

		auto			GetTableMainTitleSpelling() -> string			{ return vars->Get("act_table_header"); };
		auto			isTableRowExists(int i) -> bool					{ return vars->Get("index_" + to_string(i)).length(); };
		auto			GetTableRowDescription(int i) -> string			{ return vars->Get("subcontractor_table_row_description_" + to_string(i)); };
		auto			GetTableRowIndex(int i) -> string				{ return vars->Get("index_" + to_string(i)); };
		auto			GetTableRowQuantity(int i) -> string			{ return vars->Get("quantity_" + to_string(i)); };
		auto			GetTableRowItem(int i) -> string				{ return vars->Get("item_" + to_string(i)); };
		auto			GetTableRowPrice(int i) -> string				{ return c_float(vars->Get("timecard_price_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableRowTotal(int i) -> string				{ return c_float(vars->Get("timecard_total_" + to_string(i))).PrintPriceTag(); };
		auto			GetTableSum() -> string							{ return c_float(vars->Get("subcontractors_sum_amount")).PrintPriceTag(); };
		auto			GetTableVAT() -> string							{ return c_float(vars->Get("subcontractors_vat_amount")).PrintPriceTag(); };
		auto			GetTableTotal() -> string						{ return c_float(vars->Get("subcontractors_total_payment")).PrintPriceTag(); };

		auto			GetTotalPaymentSpelling() -> string				{ return ""; };
		auto			GetFooterComment() -> string					{ return vars->Get("act_footnote"); };

		auto			GetSignatureTitle1() -> string					{ return vars->Get("subc2agency_act_role1"); };
		auto			GetSignatureTitle2() -> string					{ return vars->Get("subc2agency_act_role2"); };
		auto			GetSignatureName1() -> string					{ return vars->Get("subcontractor_company_name_1"); };
		auto			GetSignatureName2() -> string					{ return vars->Get("agency_name"); };
		auto			GetSignatureInfo1() -> string					
						{ 
							// --- SoW specific variables override agency variables
							return vars->Get("subc2agency_act_position1_1").length() + vars->Get("subc2agency_act_signature_name1_1").length()
									? vars->Get("subc2agency_act_position1_1") + " " + vars->Get("subc2agency_act_signature_name1_1")
									: vars->Get("subc2agency_act_position1") + " " + vars->Get("subc2agency_act_signature_name1");
						};
		auto			GetSignatureInfo2() -> string					
						{
							// --- SoW specific variables override agency variables
							return  vars->Get("subc2agency_act_position2_1").length() + vars->Get("subc2agency_act_signature_name2_1").length()
									? vars->Get("subc2agency_act_position2_1") + " " + vars->Get("subc2agency_act_signature_name2_1") 
									: vars->Get("subc2agency_act_position2") + " " + vars->Get("subc2agency_act_signature_name2"); 
						};
};


#endif
