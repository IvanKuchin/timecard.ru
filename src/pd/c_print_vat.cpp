#include "c_print_vat.h"


auto	C_Print_VAT_Base::GetSupplierFullAddress() -> string
{
	return GetSupplierLegalZIP() + " " + GetSupplierLegalLocality() + ", " + GetSupplierLegalAddress();
}

auto	C_Print_VAT_Base::GetCustomerFullAddress() -> string
{
	return GetCustomerLegalZIP() + " " + GetCustomerLegalLocality() + ", " + GetCustomerLegalAddress();
}

auto	C_Print_VAT_Base::SpellPrice() -> string
{
	MESSAGE_DEBUG("", "", "start");

	C_Price_Spelling	price(stod_noexcept(GetTableTotal()));
	auto				result = vars->Get("Total payment") + ": " + price.Spelling();

	MESSAGE_DEBUG("", "", "finish");

	return result;
}


// --- XLS part
auto C_Print_VAT_Base::__PrintXLSHeader() -> string
{
	MESSAGE_DEBUG("", "", "start");
	auto	error_message = ""s;

	auto	font_big					= __book->addFont();
	auto	font_small 					= __book->addFont();
	auto	font_mid 					= __book->addFont();

	auto	format_center				= __book->addFormat();
	auto	format_big					= __book->addFormat();
	auto	format_mid_center			= __book->addFormat();
	auto	format_mid_left				= __book->addFormat();
	auto	format_mid_left_underline	= __book->addFormat();
	auto	format_small_left_underline	= __book->addFormat();
	auto	format_top_right			= __book->addFormat();
	auto	format_border_bottom		= __book->addFormat();


	font_small				->setSize(6);
	font_mid				->setSize(8);
	font_big				->setSize(12);
	font_big				->setBold();
	format_big				->setFont(font_big);

	format_top_right		->setFont(font_small);
	format_top_right		->setWrap(false);
	format_top_right		->setAlignV(libxl::ALIGNV_TOP);
	format_top_right		->setAlignH(libxl::ALIGNH_RIGHT);
	format_mid_center		->setFont(font_mid);
	format_mid_center		->setAlignH(libxl::ALIGNH_CENTER);
	format_mid_left			->setFont(font_mid);
	format_mid_left			->setAlignH(libxl::ALIGNH_LEFT);
	format_small_left_underline->setFont(font_small);
	format_small_left_underline->setAlignH(libxl::ALIGNH_LEFT);
	format_small_left_underline->setBorderBottom(libxl::BORDERSTYLE_THIN);
	format_mid_left_underline->setFont(font_mid);
	format_mid_left_underline->setAlignH(libxl::ALIGNH_LEFT);
	format_mid_left_underline->setBorderBottom(libxl::BORDERSTYLE_THIN);
	format_center			->setAlignH(libxl::ALIGNH_CENTER);
	format_border_bottom	->setBorderBottom(libxl::BORDERSTYLE_THIN);
	format_border_bottom	->setAlignH(libxl::ALIGNH_CENTER);


	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("VAT doc")).c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide(vars->Get("invoice_agreement")).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(1)").c_str(), format_mid_center);
	MergeApplyFormat(8, 15, format_top_right);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("vat_header_1") + vars->Get("vat_header_2") + vars->Get("vat_header_3") + vars->Get("vat_header_4")).c_str(), format_top_right);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("vat_correction_1")).c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide(vars->Get("vat_correction_2")).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(1a)").c_str(), format_mid_center);
	MergeApplyFormat(8, 15, format_top_right);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("vat_header_5")).c_str(), format_top_right);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("Seller") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide(GetSupplierName()).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(2)").c_str(), format_mid_center);

	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("Buyer") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_2_VALUE_MERGE_START, XL_HEADER_COL_2_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_VALUE, multibyte_to_wide(GetCustomerName()).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_SEQUENCE, multibyte_to_wide("(6)").c_str(), format_mid_center);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("Address") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_small_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide(GetSupplierFullAddress()).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(2a)").c_str(), format_mid_center);

	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("Address") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_2_VALUE_MERGE_START, XL_HEADER_COL_2_VALUE_MERGE_END, format_small_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_VALUE, multibyte_to_wide(GetCustomerFullAddress()).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_SEQUENCE, multibyte_to_wide("(6a)").c_str(), format_mid_center);


	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("vat_header_TIN_KPP_seller") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide(GetSupplierTIN() + " / " + GetSupplierKPP()).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(2б)").c_str(), format_mid_center);

	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("vat_header_TIN_KPP_buyer") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_2_VALUE_MERGE_START, XL_HEADER_COL_2_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_VALUE, multibyte_to_wide(GetCustomerTIN() + " / " + GetCustomerKPP()).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_SEQUENCE, multibyte_to_wide("(6б)").c_str(), format_mid_center);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("vat_shipper_address") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide("").c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(3)").c_str(), format_mid_center);

	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("VAT comment1")).c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_2_VALUE_MERGE_START, XL_HEADER_COL_2_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_VALUE, multibyte_to_wide(vars->Get("VAT comment2")).c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_SEQUENCE, multibyte_to_wide("(7)").c_str(), format_mid_center);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("vat_consignee_address") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide("").c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(4)").c_str(), format_mid_center);

	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("VAT comment3")).c_str(), format_mid_left);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("vat_doc_accompany") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide("").c_str());
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(5)").c_str(), format_mid_center);

	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_TITLE, multibyte_to_wide(vars->Get("VAT comment4")).c_str(), format_mid_left);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_2_SEQUENCE, multibyte_to_wide("(8)").c_str(), format_mid_center);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_TITLE, multibyte_to_wide(vars->Get("vat_shipping_doc") + ": ").c_str(), format_mid_left);
	MergeApplyFormat(XL_HEADER_COL_1_VALUE_MERGE_START,  XL_HEADER_COL_1_VALUE_MERGE_END, format_mid_left_underline);
	__sheet->writeStr(__row_counter, XL_HEADER_COL_1_SEQUENCE, multibyte_to_wide("(5a)").c_str(), format_mid_center);

	{
		auto	number_of_payment_orders = 0;
		for(number_of_payment_orders = 1; isTableRowExists(number_of_payment_orders); ++number_of_payment_orders) {};


		__sheet->writeStr(__row_counter, XL_HEADER_COL_1_VALUE, multibyte_to_wide(vars->Get("VAT title - Payment order number") + " 1-" + to_string(number_of_payment_orders) + " " + vars->Get("invoice_agreement")).c_str(), format_mid_left_underline);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Print_VAT_Base::__PrintXLSTable() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto			error_message				= ""s;
	auto			font_small					= __book->addFont();
	auto			format_table_left			= __book->addFormat();
	auto			format_small_border			= __book->addFormat();
	auto			format_table_right			= __book->addFormat();
	auto			format_table_center			= __book->addFormat();
	auto			format_small_border_left	= __book->addFormat();

	font_small->setSize(6);

	format_table_right->setWrap(true);
	format_table_right->setAlignV(libxl::ALIGNV_TOP);
	format_table_right->setAlignH(libxl::ALIGNH_RIGHT);
	format_table_right->setBorder(libxl::BORDERSTYLE_THIN);

	format_small_border->setFont(font_small);
	format_small_border->setBorder(libxl::BORDERSTYLE_THIN);
	format_small_border->setAlignH(libxl::ALIGNH_CENTER);
	format_small_border->setWrap(true);

	format_table_left->setWrap(true);
	format_table_left->setAlignV(libxl::ALIGNV_TOP);
	format_table_left->setAlignH(libxl::ALIGNH_LEFT);
	format_table_left->setBorder(libxl::BORDERSTYLE_THIN);

	format_table_center->setWrap(true);
	format_table_center->setAlignV(libxl::ALIGNV_TOP);
	format_table_center->setAlignH(libxl::ALIGNH_CENTER);
	format_table_center->setBorder(libxl::BORDERSTYLE_THIN);

	format_small_border_left->setFont(font_small);
	format_small_border_left->setBorder(libxl::BORDERSTYLE_THIN);
	format_small_border_left->setAlignH(libxl::ALIGNH_LEFT);
	format_small_border_left->setWrap(true);

	// --- table header
	++__row_counter;
	__sheet->setRow(__row_counter,		6);
	++__row_counter;

	__sheet->setRow(__row_counter,		16);
	__sheet->setRow(__row_counter + 1,	25);

	// --- assign style
	for(auto i = 0; i <= 14; ++i)
	{
		__sheet->writeStr(__row_counter,  i, L"", format_table_left); // --- assign style
		__sheet->writeStr(__row_counter+1,  i, L"", format_table_left); // --- assign style
	}

	// --- merge cells vertically, but 2, 2a, 10, 10a
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_1, XL_TABLE_COL_1);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_1a, XL_TABLE_COL_1a);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_1b, XL_TABLE_COL_1b);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_3, XL_TABLE_COL_3);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_4, XL_TABLE_COL_4);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_5, XL_TABLE_COL_5);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_6, XL_TABLE_COL_6);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_7, XL_TABLE_COL_7);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_8, XL_TABLE_COL_8);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_9, XL_TABLE_COL_9);
	__sheet->setMerge(__row_counter, __row_counter + 1, XL_TABLE_COL_11, XL_TABLE_COL_11);
	// --- merge cells horizontally
	__sheet->setMerge(__row_counter, __row_counter, XL_TABLE_COL_2, XL_TABLE_COL_2a);
	__sheet->setMerge(__row_counter + 1, __row_counter + 1, XL_TABLE_COL_2, 4);
	__sheet->setMerge(__row_counter, __row_counter, XL_TABLE_COL_10, XL_TABLE_COL_10a);

	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_1, multibyte_to_wide(vars->Get("VAT title - Payment order number")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_1a, multibyte_to_wide(vars->Get("VAT title - Good name")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_1b, multibyte_to_wide(vars->Get("VAT title - Good code")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,      XL_TABLE_COL_2, multibyte_to_wide(vars->Get("VAT title - Measure unit")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter + 1,  XL_TABLE_COL_2, multibyte_to_wide(vars->Get("VAT title - Measure unit (code)")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter + 1,  XL_TABLE_COL_2a, multibyte_to_wide(vars->Get("VAT title - Measure unit (name)")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_3, multibyte_to_wide(vars->Get("VAT title - Quantity")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_4, multibyte_to_wide(vars->Get("VAT title - Price per unit")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_5, multibyte_to_wide(vars->Get("VAT title - Price wo tax")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_6, multibyte_to_wide(vars->Get("VAT title - Excise")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_7, multibyte_to_wide(vars->Get("Tax rate")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,	  XL_TABLE_COL_8, multibyte_to_wide(vars->Get("VAT title - Tax amount")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,      XL_TABLE_COL_9, multibyte_to_wide(vars->Get("VAT title - Price w tax")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,      XL_TABLE_COL_10, multibyte_to_wide(vars->Get("VAT title - Country")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter + 1,  XL_TABLE_COL_10, multibyte_to_wide(vars->Get("VAT title - Country code")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter + 1,  XL_TABLE_COL_10a, multibyte_to_wide(vars->Get("VAT title - Country name")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter,      XL_TABLE_COL_11, multibyte_to_wide(vars->Get("VAT title - Custom number")).c_str(), format_small_border);
	__sheet->writeStr(__row_counter + 1,  XL_TABLE_COL_11, L"", format_small_border); // --- assign style
	++__row_counter;
	++__row_counter;
	__sheet->setRow(__row_counter,		10);
	__sheet->setMerge(__row_counter, __row_counter, 3, 4);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_1, 	L"1",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_1a, 	L"1a",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_1b, 	L"1б",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_2, 	L"2",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_2a, 	L"2a",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_3, 	L"3",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_4, 	L"4",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_5, 	L"5",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_6, 	L"6",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_7,	L"7",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_8,	L"8",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_9,	L"9",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_10,	L"10",	format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_10a,	L"10a", format_small_border);
	__sheet->writeStr(__row_counter, XL_TABLE_COL_11,	L"11",	format_small_border);

	// --- table body
	for(auto i = 1; isTableRowExists(i); ++i)
	{
		auto	description = GetTableRowDescription(i);

		++__row_counter;

		__sheet->writeStr(__row_counter, XL_TABLE_COL_1, multibyte_to_wide(to_string(i)).c_str(), format_small_border);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_1a, multibyte_to_wide(description).c_str(), format_small_border_left);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_1b, L"-", format_table_center);
		__sheet->writeNum(__row_counter, XL_TABLE_COL_2, 796, format_table_center);
		__sheet->writeStr(__row_counter,  4, L"", format_table_center); // --- assign style
		__sheet->setMerge(__row_counter, __row_counter, 3, 4);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_2a, multibyte_to_wide(GetTableRowItem(i)).c_str(), format_table_center);
		__sheet->writeNum(__row_counter, XL_TABLE_COL_3, stod_noexcept(GetTableRowQuantity(i)), format_table_center);
		__sheet->writeNum(__row_counter, XL_TABLE_COL_4, stod_noexcept(GetTableRowPrice(i)), format_small_border);
		__sheet->writeNum(__row_counter, XL_TABLE_COL_5, stod_noexcept(GetTableRowPrice(i)), format_table_right);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_6, multibyte_to_wide(vars->Get("no excise")).c_str(), format_small_border);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_7, multibyte_to_wide(GetSupplierVATSpellingShort()).c_str(), format_small_border);
		__sheet->writeNum(__row_counter, XL_TABLE_COL_8, stod_noexcept(GetTableRowVAT(i)), format_table_right);
		__sheet->writeNum(__row_counter, XL_TABLE_COL_9, stod_noexcept(GetTableRowTotal(i)), format_table_right);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_10, L"-", format_table_center);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_10a, L"-", format_table_center);
		__sheet->writeStr(__row_counter, XL_TABLE_COL_11, L"-", format_table_center);

		++total_table_items;
	}

	++__row_counter;
	MergeApplyFormat(0, 7, format_table_left);
	__sheet->writeStr(__row_counter,  1, multibyte_to_wide(vars->Get("Total payment") + ":").c_str(), format_table_left);

	__sheet->writeNum(__row_counter,  8, stod_noexcept(GetTableSum()), format_table_right);
	__sheet->writeStr(__row_counter,  9, L"", format_table_left); // --- assign style
	__sheet->writeStr(__row_counter, 10, L"", format_table_left); // --- assign style
	__sheet->setMerge(__row_counter, __row_counter, 9, 10);
	__sheet->writeStr(__row_counter,  9, L"X", format_table_center);
	__sheet->writeNum(__row_counter, 11, stod_noexcept(GetTableVAT()), format_table_right);
	__sheet->writeNum(__row_counter, 12, stod_noexcept(GetTableTotal()), format_table_right);

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Print_VAT_Base::__PrintXLSSignature() -> string
{
	MESSAGE_DEBUG("", "", "start");
	auto	error_message = ""s;

	auto	font_bold				= __book->addFont();
	auto	font_small				= __book->addFont();
	auto	format_bold				= __book->addFormat();
	auto	format_bold_right		= __book->addFormat();
	auto	format_right			= __book->addFormat();
	auto	format_left_small		= __book->addFormat();
	auto	format_center_small		= __book->addFormat();
	auto	format_underline		= __book->addFormat();
	auto	format_wrap_small_up	= __book->addFormat();
	auto	format_underline_center_small	= __book->addFormat();

	font_small->setSize(8);
	font_bold->setBold();

	format_bold->setFont(font_bold);
	format_right->setAlignH(libxl::ALIGNH_RIGHT);
	format_bold_right->setFont(font_bold);
	format_bold_right->setAlignH(libxl::ALIGNH_RIGHT);
	format_left_small->setFont(font_small);
	format_left_small->setAlignH(libxl::ALIGNH_LEFT);
	format_center_small->setFont(font_small);
	format_center_small->setAlignH(libxl::ALIGNH_CENTER);
	format_underline->setFont(font_small);
	format_underline->setBorderBottom(libxl::BORDERSTYLE_THIN);
	format_underline_center_small->setFont(font_small);
	format_underline_center_small->setAlignH(libxl::ALIGNH_CENTER);
	format_underline_center_small->setBorderBottom(libxl::BORDERSTYLE_THIN);
	format_wrap_small_up->setWrap(true);
	format_wrap_small_up->setFont(font_small);
	format_wrap_small_up->setAlignV(libxl::ALIGNV_TOP);
	format_wrap_small_up->setAlignH(libxl::ALIGNH_CENTER);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_1, multibyte_to_wide(GetSignatureInfo1()).c_str(), format_left_small);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_2, multibyte_to_wide(GetSignatureInfo2()).c_str(), format_left_small);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_1, multibyte_to_wide(vars->Get("VAT sign - Authorized person")).c_str(), format_left_small);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_2, multibyte_to_wide(vars->Get("VAT sign - Authorized person")).c_str(), format_left_small);
	MergeApplyFormat(XL_SIGNATURE_SIGN_1_START,  XL_SIGNATURE_SIGN_1_END, format_underline);
	MergeApplyFormat(XL_SIGNATURE_NAME_1_START,  XL_SIGNATURE_NAME_1_END, format_underline);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_NAME_1_START, multibyte_to_wide(GetSignatureName1()).c_str(), format_underline_center_small);

	__sheet->writeStr(__row_counter, XL_SIGNATURE_SIGN_2_START, L"", format_underline_center_small);
	MergeApplyFormat(XL_SIGNATURE_NAME_2_START, XL_SIGNATURE_NAME_2_END, format_underline);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_NAME_2_START, multibyte_to_wide(GetSignatureName2()).c_str(), format_underline_center_small);

	++__row_counter;
	MergeApplyFormat(XL_SIGNATURE_SIGN_1_START,  XL_SIGNATURE_SIGN_1_END, nullptr);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_SIGN_1_START, multibyte_to_wide("(" + vars->Get("Signature") + ")").c_str(), format_center_small);
	MergeApplyFormat(XL_SIGNATURE_NAME_1_START,  XL_SIGNATURE_NAME_1_END, nullptr);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_NAME_1_START, multibyte_to_wide("(" + vars->Get("initials") + ")").c_str(), format_center_small);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_SIGN_2_START, multibyte_to_wide("(" + vars->Get("Signature") + ")").c_str(), format_center_small);
	MergeApplyFormat(XL_SIGNATURE_NAME_2_START, XL_SIGNATURE_NAME_2_END, nullptr);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_NAME_2_START, multibyte_to_wide("(" + vars->Get("initials") + ")").c_str(), format_center_small);

	++__row_counter;
	__sheet->setRow(__row_counter, 18);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_1, multibyte_to_wide(vars->Get("VAT sign - Entrepreneur")).c_str(), format_left_small);

	++__row_counter;
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_1, multibyte_to_wide(vars->Get("VAT sign - Authorized person")).c_str(), format_left_small);
	MergeApplyFormat(XL_SIGNATURE_SIGN_1_START,  XL_SIGNATURE_SIGN_1_END, format_underline);
	MergeApplyFormat(XL_SIGNATURE_NAME_1_START,  XL_SIGNATURE_NAME_1_END, format_underline);
	MergeApplyFormat( 8, 15, format_underline);

	++__row_counter;
	__sheet->setRow(__row_counter, 25);
	MergeApplyFormat(XL_SIGNATURE_SIGN_1_START,  XL_SIGNATURE_SIGN_1_END, nullptr);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_SIGN_1_START, multibyte_to_wide("(" + vars->Get("Signature") + ")").c_str(), format_wrap_small_up);
	MergeApplyFormat(XL_SIGNATURE_NAME_1_START,  XL_SIGNATURE_NAME_1_END, nullptr);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_NAME_1_START, multibyte_to_wide("(" + vars->Get("initials") + ")").c_str(), format_wrap_small_up);
	MergeApplyFormat( 8, 15, format_wrap_small_up);
	__sheet->writeStr(__row_counter, XL_SIGNATURE_TITLE_2, multibyte_to_wide(vars->Get("VAT sign - Entrepreneur registration")).c_str(), format_wrap_small_up);



	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::MergeApplyFormat(int start_col, int end_col, libxl::Format* fmt) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	for(auto i = start_col; i <= end_col; ++i)
	{
		__sheet->writeStr(__row_counter, i, L"", fmt);
	}
	__sheet->setMerge(__row_counter, __row_counter, start_col, end_col);

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::PrintAsXLS() -> string
{
	auto	error_message = ""s;
	auto	f_name = GetFilename(); 

	MESSAGE_DEBUG("", "", "start");

	// locale cp1251_locale("ru_RU.UTF-8");
	// std::locale::global(cp1251_locale);

	if(vars)
	{
		// --- after instantiation libxl-object locale switch over to wide-byte string representation
		// --- gettext is not working properly with multibyte strings, therefore all gettext constants
		// --- must be defined before libxl-instantiation

		__book = xlCreateBook();
		if(__book)
		{
			__book->setKey(multibyte_to_wide(xl_username).c_str(), multibyte_to_wide(xl_key).c_str());

			__sheet = __book->addSheet(L"Sheet1");
			if(__sheet)
			{
				total_table_items = 0;

				__row_counter = 0;

				// --- print properties
				__sheet->setPaper(libxl::PAPER_A4);
				__sheet->setLandscape(true);
				__sheet->setPrintZoom(97);

				// --- set columns width
				__sheet->setCol(0, 0, 5);
				__sheet->setCol(1, 1, 25);
				__sheet->setCol(2, 2, 5);
				__sheet->setCol(3, 3, 6);
				__sheet->setCol(4, 4, 0.67);
				__sheet->setCol(5, 5, 8);
				__sheet->setCol(6, 6, 6);
				__sheet->setCol(7, 7, 6);
				__sheet->setCol(8, 8, 11);
				__sheet->setCol(9, 9, 5);
				__sheet->setCol(10, 10, 6);
				__sheet->setCol(11, 11, 11);
				__sheet->setCol(12, 12, 11);
				__sheet->setCol(13, 13, 6);
				__sheet->setCol(14, 14, 6);
				__sheet->setCol(15, 15, 7);

				PrintXLSHeader();

				__PrintXLSTable();

				++__row_counter;
				__PrintXLSSignature();

			}

			if(__book->save(multibyte_to_wide(GetFilename()).c_str())) {}
			else
			{
				MESSAGE_ERROR("", "", "can't save " + filename + ".xls")
			}
			__book->release();
		} 
	}
	else
	{
		error_message = gettext("fail to initialize variable set");
		MESSAGE_ERROR("", "", "set variables before generating documents. Invoke SetVariableSet before printing.");
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}




// --- PDF part
auto	C_Print_VAT_Base::__HPDF_DrawHeader() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = PrintPDFHeader()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print header"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT doc")), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("invoice_agreement")), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(1)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_header_1") + vars->Get("vat_header_2") + vars->Get("vat_header_3") + vars->Get("vat_header_4")), 0, 100, HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size - 2, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_correction_1")), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_correction_2")), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(1a)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_header_5")), 0, 100, HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size - 2, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Seller") + ": "), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierName()), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(2)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Buyer") + ": "), PDF_HEADER_COL_2_TITLE_START, PDF_HEADER_COL_2_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetCustomerName()), PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(6)"), PDF_HEADER_SEQUENCE_2_START, PDF_HEADER_SEQUENCE_2_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Address") + ": "), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierFullAddress()), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(2a)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("Address") + ": "), PDF_HEADER_COL_2_TITLE_START, PDF_HEADER_COL_2_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetCustomerFullAddress()), PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(6a)"), PDF_HEADER_SEQUENCE_2_START, PDF_HEADER_SEQUENCE_2_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_header_TIN_KPP_seller") + ": "), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSupplierTIN() + " / " + GetSupplierKPP()), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(2б)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_header_TIN_KPP_buyer") + ": "), PDF_HEADER_COL_2_TITLE_START, PDF_HEADER_COL_2_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetCustomerTIN() + " / " + GetCustomerKPP()), PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(6б)"), PDF_HEADER_SEQUENCE_2_START, PDF_HEADER_SEQUENCE_2_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_shipper_address")), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(""), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(3)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT comment1") + ": "), PDF_HEADER_COL_2_TITLE_START, PDF_HEADER_COL_2_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT comment2") + ": "), PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(7)"), PDF_HEADER_SEQUENCE_2_START, PDF_HEADER_SEQUENCE_2_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_consignee_address")), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(""), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(4)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT comment3")), PDF_HEADER_COL_2_TITLE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(""), PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(" "), PDF_HEADER_SEQUENCE_2_START, PDF_HEADER_SEQUENCE_2_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_doc_accompany")), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(""), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(5)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT comment4")), PDF_HEADER_COL_2_TITLE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(""), PDF_HEADER_COL_2_VALUE_START, PDF_HEADER_COL_2_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(8)"), PDF_HEADER_SEQUENCE_2_START, PDF_HEADER_SEQUENCE_2_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("vat_shipping_doc") + ": "), PDF_HEADER_COL_1_TITLE_START, PDF_HEADER_COL_1_TITLE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		{
			auto	number_of_payment_orders = 0;
			for(number_of_payment_orders = 1; isTableRowExists(number_of_payment_orders); ++number_of_payment_orders) {};

			if(error_message.empty())
			{
				if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT title - Payment order number") + " 1-" + to_string(number_of_payment_orders) + " " + vars->Get("invoice_agreement")), PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
			}
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(5a)"), PDF_HEADER_SEQUENCE_1_START, PDF_HEADER_SEQUENCE_1_END, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(PDF_HEADER_COL_1_VALUE_START, PDF_HEADER_COL_1_VALUE_END)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print title");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawTable_Header() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1, utf8_to_cp1251(vars->Get("VAT title - Payment order number")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title index line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1a, utf8_to_cp1251(vars->Get("VAT title - Good name")), HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size - 2, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title index line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1b, utf8_to_cp1251(vars->Get("VAT title - Good code")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title description line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2, " \n \n \n " + utf8_to_cp1251(vars->Get("VAT title - Measure unit (code)")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title measure unit line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2a, " \n \n \n " + utf8_to_cp1251(vars->Get("VAT title - Measure unit (name)")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title measure unit line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_3, utf8_to_cp1251(vars->Get("VAT title - Quantity")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title quantity line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_4, utf8_to_cp1251(vars->Get("VAT title - Price per unit")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title price per unit"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_5, utf8_to_cp1251(vars->Get("VAT title - Price wo tax")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title VAT title - Price wo tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_6, utf8_to_cp1251(vars->Get("VAT title - Excise")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title excise line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_7, utf8_to_cp1251(vars->Get("Tax rate")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title tax rate line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_8, utf8_to_cp1251(vars->Get("VAT title - Tax amount")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title tax amount line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_9, utf8_to_cp1251(vars->Get("VAT title - Price w tax")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10, " \n \n \n " + utf8_to_cp1251(vars->Get("VAT title - Country code")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title Country code"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10a, " \n \n \n " + utf8_to_cp1251(vars->Get("VAT title - Country name")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 3, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title Country name"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_11, utf8_to_cp1251(vars->Get("VAT title - Custom number")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveTableLineDown(3)).length())
			{ MESSAGE_ERROR("", "", "fail to move table line down"); }
		}

		// --- all these tricks to merge cells
		// --- VERY IMPORTANT to ensure that there is no page break here. 
		// --- pega break doesn't handle on move up
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_StopTable()).length())
			{ MESSAGE_ERROR("", "", "fail to stop table"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown(-3)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_2a)).length())
			{ MESSAGE_ERROR("", "", "fail to remove table separator"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2, PDF_TABLE_COL_2a, utf8_to_cp1251(vars->Get("VAT title - Measure unit")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_10a)).length())
			{ MESSAGE_ERROR("", "", "fail to remove table separator"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10, PDF_TABLE_COL_10a, utf8_to_cp1251(vars->Get("VAT title - Country")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_9, utf8_to_cp1251(vars->Get("")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown(1)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(28, 39)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(85, 95)).length())
			{ MESSAGE_ERROR("", "", "fail to write table title total line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown(2)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_StartTable()).length())
			{ MESSAGE_ERROR("", "", "fail to start table"); }
		}


		// --- numeration
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1, utf8_to_cp1251("1"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1a, utf8_to_cp1251("1a"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1b, utf8_to_cp1251("1б"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2, utf8_to_cp1251("2"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2a, utf8_to_cp1251("2a"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_3, utf8_to_cp1251("3"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_4, utf8_to_cp1251("4"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_5, utf8_to_cp1251("5"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_6, utf8_to_cp1251("6"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_7, utf8_to_cp1251("7"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_8, utf8_to_cp1251("8"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_9, utf8_to_cp1251("9"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10, utf8_to_cp1251("10"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10a, utf8_to_cp1251("10a"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_11, utf8_to_cp1251("11"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "fail to print text to table cell"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveTableLineDown(1)).length())
			{ MESSAGE_ERROR("", "", "fail to move table line down"); }
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print table");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawTable_Footer() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1, PDF_TABLE_COL_1a, utf8_to_cp1251(vars->Get("Total payment") + ":"), HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer description line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_5, utf8_to_cp1251(GetTableSum()), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_pre_tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_6, PDF_TABLE_COL_7, utf8_to_cp1251("X"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer excise"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_8, utf8_to_cp1251(GetTableVAT()), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_9, utf8_to_cp1251(GetTableTotal()), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, true)).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_post_tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_StopTable()).length())
			{ MESSAGE_ERROR("", "", "fail to stop table"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown(-1)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_1a)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_1b)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_2)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_2a)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_3)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_4)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_RemoveTableSeparator(PDF_TABLE_COL_7)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown(1)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(1, 91)).length())
			{ MESSAGE_ERROR("", "", "fail to move line down"); }
		}

	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print table");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawTable_Body() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			total_table_items = 0;
			for(auto i = 1; isTableRowExists(i); ++i)
			{
				if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1, utf8_to_cp1251(to_string(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
				{
					auto	max_lines = pdf_obj.__HPDF_GetNumberOfLinesInTable(PDF_TABLE_COL_1a, utf8_to_cp1251(GetTableRowDescription(i))	, NORMAL_FONT, __pdf_font_size - 2);


					if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1a, utf8_to_cp1251(GetTableRowDescription(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).empty())
					{
						if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_1b, utf8_to_cp1251("-"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
						{
							if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2, utf8_to_cp1251("796"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
							{
								if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_2a, utf8_to_cp1251(GetTableRowItem(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
								{
									if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_3, utf8_to_cp1251(GetTableRowQuantity(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
									{
										if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_4, utf8_to_cp1251(GetTableRowPrice(i)), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 2, false)).empty())
										{
											if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_5, utf8_to_cp1251(GetTableRowPrice(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).empty())
											{
												if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_6, utf8_to_cp1251(vars->Get("no excise")), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 4, false)).empty())
												{
													if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_7, utf8_to_cp1251(GetSupplierVATSpellingShort()), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).empty())
													{
														if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_8, utf8_to_cp1251(GetTableRowVAT(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).empty())
														{
															if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_9, utf8_to_cp1251(GetTableRowTotal(i)), HPDF_TALIGN_RIGHT, NORMAL_FONT, __pdf_font_size, false)).empty())
															{
																if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10, utf8_to_cp1251("-"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
																{
																	if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_10a, utf8_to_cp1251("-"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
																	{
																		if((error_message = pdf_obj.__HPDF_PrintTextTableCell(PDF_TABLE_COL_11, utf8_to_cp1251("-"), HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).empty())
																		{
																			if((error_message = pdf_obj.__HPDF_MoveTableLineDown(max_lines)).empty())
																			{

																			}
																			else
																			{
																				MESSAGE_ERROR("", "", "fail to move table line down " + to_string(max_lines) + " line(s)");
																			}
																		}
																		else
																		{
																			MESSAGE_ERROR("", "", "fail to write table text (" + to_string(i) + ") line");
																		}
																	}
																	else
																	{
																		MESSAGE_ERROR("", "", "fail to write table text (" + to_string(i) + ") line");
																	}
																}
																else
																{
																	MESSAGE_ERROR("", "", "fail to write table text (" + to_string(i) + ") line");
																}
															}
															else
															{
																MESSAGE_ERROR("", "", "fail to write table total (" + to_string(i) + ") line");
															}
														}
														else
														{
															MESSAGE_ERROR("", "", "fail to write table tax (" + to_string(i) + ") line");
														}
													}
													else
													{
														MESSAGE_ERROR("", "", "fail to write vat percentage (" + to_string(i) + ") line");
													}
												}
												else
												{
													MESSAGE_ERROR("", "", "fail to write table excise (" + to_string(i) + ") line");
												}
											}
											else
											{
												MESSAGE_ERROR("", "", "fail to write table price (" + to_string(i) + ") line");
											}
										}
										else
										{
											MESSAGE_ERROR("", "", "fail to write table price per unit (" + to_string(i) + ") line");
										}
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to write table quantity (" + to_string(i) + ") line");
									}
								}
								else
								{
									MESSAGE_ERROR("", "", "fail to write table item (" + to_string(i) + ") line");
								}
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to write table measure unit (" + to_string(i) + ") line");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "fail to write table description (" + to_string(i) + ") line");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to write table item tittle (" + to_string(i) + ") line");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to write table index seq (" + to_string(i) + ") line");
				}

				if(error_message.length()) break;

				++total_table_items;
			}
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print table");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawTable() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		if(error_message.empty())
		{
			pdf_obj.AddColumn(4);
			pdf_obj.AddColumn(20);
			pdf_obj.AddColumn(4);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(6);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(9);
			pdf_obj.AddColumn(4);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(9);
			pdf_obj.AddColumn(9);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(5);
			pdf_obj.AddColumn(5);

			if((error_message = pdf_obj.__HPDF_StartTable()).length())
			{
				MESSAGE_ERROR("", "", "fail to start table");
			}
		}

		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawTable_Header()).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_pre_tax line"); }
		}

		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawTable_Body()).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_pre_tax line"); }
		}
		if(error_message.empty())
		{
			if((error_message = __HPDF_DrawTable_Footer()).length())
			{ MESSAGE_ERROR("", "", "fail to write table footer sum_pre_tax line"); }
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print table");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::__HPDF_DrawFooter() -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	try
	{
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureInfo1()), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureInfo2()), 50, 50+20, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT sign - Authorized person")), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT sign - Authorized person")), 50, 50+20, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureName1()), 30, 45, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(GetSignatureName2()), 80, 95, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(21, 30)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(31, 45)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(71, 80)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(81, 95)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(" + vars->Get("Signature") + ")"), 20, 30, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(" + vars->Get("initials") + ")"), 30, 45, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(" + vars->Get("Signature") + ")"), 70, 80, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(" + vars->Get("initials") + ")"), 80, 95, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT sign - Entrepreneur")), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT sign - Authorized person")), 0, 20, HPDF_TALIGN_LEFT, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(21, 30)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(31, 45)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}

		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_DrawHorizontalLine(50, 95)).length())
				{ MESSAGE_ERROR("", "", "hpdf: fail to draw horizontal line"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(" + vars->Get("Signature") + ")"), 20, 30, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251("(" + vars->Get("initials") + ")"), 30, 45, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}
		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_PrintTextRect(utf8_to_cp1251(vars->Get("VAT sign - Entrepreneur registration")), 50, 95, HPDF_TALIGN_CENTER, NORMAL_FONT, __pdf_font_size - 1, false)).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
		}


		if(error_message.empty())
		{
			if((error_message = pdf_obj.__HPDF_MoveLineDown()).length())
			{ MESSAGE_ERROR("", "", "hpdf: fail to move line down"); }
		}
	}
	catch(...)
	{
		error_message = gettext("hpdf: fail to print title");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Print_VAT_Base::PrintAsPDF() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	pdf_obj.SetFilename(GetFilename());
	pdf_obj.SetOrientation(HPDF_PAGE_LANDSCAPE);
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_init()).length())			{ MESSAGE_ERROR("", "", "fail to initialize hpdf library"); }
	}
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_SetDocProps()).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to set dc props"); }
	}
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_MoveLineDown(0)).length())	{ MESSAGE_ERROR("", "", "hpdf: fail to move pointer down"); }
	}
	if(error_message.empty())
	{
		if((error_message = __HPDF_DrawHeader()).length())				{ MESSAGE_ERROR("", "", "hpdf: fail to draw timecard title"); }
	}
	if(error_message.empty())
	{
		if((error_message = __HPDF_DrawTable()).length())				{ MESSAGE_ERROR("", "", "hpdf: fail to print table"); }
	}
	if(error_message.empty())
	{
		if((error_message = __HPDF_DrawFooter()).length())				{ MESSAGE_ERROR("", "", "hpdf: fail to print text"); }
	}
	if(error_message.empty())
	{
		if((error_message = pdf_obj.__HPDF_SaveToFile()).length())		{ MESSAGE_ERROR("", "", "hpdf: fail to save to file"); }
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

ostream& operator<<(ostream& os, const C_Print_VAT_Base &var)
{
	os << "object C_Print_VAT_Base [empty for now]";

	return os;
}



