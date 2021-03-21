#include "c_invoicing_vars.h"

auto	C_Invoicing_Vars::AssignVariableFromDB(const string &var_name, const string &sql_query, bool isMandatory) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (variable: " + var_name + ")");

	if(db)
	{
		if(vars.Get(var_name).empty())
		{
			if(db->Query(sql_query))
			{
				vars.Add(var_name, ConvertHTMLToText(db->Get(0, 0)));
			}
			else
			{
				if(isMandatory)
				{
					MESSAGE_ERROR("", "", var_name + " " + "variable not found");
					error_message = var_name + " " + gettext("variable not found");
				}
				else
				{
					MESSAGE_DEBUG("", "", var_name + " " + "variable not found");
				}
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "variable (" + var_name + ") already defined");
		}
	}
	else
	{
		error_message = gettext("db is not initialized");
		MESSAGE_ERROR("", "", "db is not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Invoicing_Vars::AssignVariableValue(const string &var_name, const string &value, bool isMandatory) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (variable: " + var_name + ", value = " + value + ")");

	if(vars.Get(var_name).length())
	{
		MESSAGE_DEBUG("", "", "variable (" + var_name + ") already defined");
	}
	vars.Set(var_name, ConvertHTMLToText(value));

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Invoicing_Vars::FillStaticDictionary() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty()) error_message = AssignVariableValue("Bank Identifier",		gettext("Bank Identifier"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Account",				gettext("Account"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Destination bank",		gettext("Destination bank"), true);
	if(error_message.empty()) error_message = AssignVariableValue("TIN",					gettext("TIN"), true);
	if(error_message.empty()) error_message = AssignVariableValue("KPP",					gettext("KPP"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Address",				gettext("Address"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Recipient",				gettext("Recipient"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Invoice",				gettext("Invoice"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Act",					gettext("Act"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Tax",					gettext("Tax"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum expense",			gettext("Sum expense"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT",					gettext("VAT"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT doc",				gettext("VAT doc"), true);
	if(error_message.empty()) error_message = AssignVariableValue("agreement from",			gettext("agreement from"), true);
	if(error_message.empty()) error_message = AssignVariableValue("agreement declensioned",	gettext("agreement declensioned"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Technical Requirement agreement short", gettext("Technical Requirement agreement short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("from",					gettext("from"), true);
	if(error_message.empty()) error_message = AssignVariableValue("up to",					gettext("up to"), true);
	if(error_message.empty()) error_message = AssignVariableValue("in scope",				gettext("in scope"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Provider",				gettext("Provider"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Implementor",			gettext("Implementor"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Customer",				gettext("Customer"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Seller",					gettext("Seller"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Buyer",					gettext("Buyer"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Supplier",				gettext("Supplier"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Goods",					gettext("Goods"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Expense",				gettext("Expense"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Taxable sum",			gettext("Taxable sum"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Non-taxable sum",		gettext("Non-taxable sum"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Basis",					gettext("Basis"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Price",					gettext("Price"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Dayrate",				gettext("Dayrate"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Markup",					gettext("Markup"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total",					gettext("Total"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total (tax free)",		gettext("Total (tax free)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("no excise",				gettext("no excise"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Tax rate",				gettext("Tax rate"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Location",				gettext("Location"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Tax amount",				gettext("Tax amount"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Quantity short",			gettext("Quantity short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Items short",			gettext("Items short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("item short",				gettext("item short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT short",				gettext("VAT short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT sum",				gettext("VAT sum"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total payment",			gettext("Total payment"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum items",				gettext("Sum items"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum (rur)",				gettext("Sum (rur)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum (currency)",			gettext("Sum (currency)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Rate exchange",			gettext("Rate exchange"), true);
	if(error_message.empty()) error_message = AssignVariableValue("total amount",			gettext("total amount"), true);
	if(error_message.empty()) error_message = AssignVariableValue("rur.",					gettext("rur."), true);
	if(error_message.empty()) error_message = AssignVariableValue("Director",				gettext("Director"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Accountant",				gettext("Accountant"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total hours on duty",	gettext("Total hours on duty"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total days on duty",		gettext("Total days on duty"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total payment in reported timecard", gettext("Total payment in reported timecard"), true);
	if(error_message.empty()) error_message = AssignVariableValue("w/o",					gettext("w/o"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Signature", 				gettext("Signature"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Initials", 				gettext("Initials"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Title", 					gettext("Title"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Date", 					gettext("Date"), true);
	if(error_message.empty()) error_message = AssignVariableValue("rur.", 					gettext("rur."), true);
	if(error_message.empty()) error_message = AssignVariableValue("kop.", 					gettext("kop."), true);
	if(error_message.empty()) error_message = AssignVariableValue("Remote service report over period", gettext("Remote service report over period"), true);
	if(error_message.empty()) error_message = AssignVariableValue("PSoW", 					gettext("PSoW"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Project ID", 			gettext("Project ID"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Timecard approvers", 	gettext("Timecard approvers"), true);
	if(error_message.empty()) error_message = AssignVariableValue("BT approvers", 			gettext("BT approvers"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_1", 			gettext("vat_header_1"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_2", 			gettext("vat_header_2"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_3", 			gettext("vat_header_3"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_4", 			gettext("vat_header_4"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_5", 			gettext("vat_header_5"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_correction", 		gettext("vat_correction"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_TIN_KPP_seller", 	gettext("vat_header_TIN_KPP_seller"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_header_TIN_KPP_buyer", 	gettext("vat_header_TIN_KPP_buyer"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_shipper_address",	gettext("vat_shipper_address"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_consignee_address",	gettext("vat_consignee_address"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_doc_accompany",		gettext("vat_doc_accompany"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Good name",				gettext("VAT title - Good name"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Good code",				gettext("VAT title - Good code"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Measure unit",			gettext("VAT title - Measure unit"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Measure unit (code)",	gettext("VAT title - Measure unit (code)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Measure unit (name)",	gettext("VAT title - Measure unit (name)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Quantity",				gettext("VAT title - Quantity"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Price per unit",			gettext("VAT title - Price per unit"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Price wo tax",			gettext("VAT title - Price wo tax"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Excise",					gettext("VAT title - Excise"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Tax amount",				gettext("VAT title - Tax amount"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Price w tax",			gettext("VAT title - Price w tax"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Country",				gettext("VAT title - Country"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Country code",			gettext("VAT title - Country code"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Country name",			gettext("VAT title - Country name"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT title - Custom number",			gettext("VAT title - Custom number"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT comment1",						gettext("VAT comment1"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT comment2",						gettext("VAT comment2"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT comment3",						gettext("VAT comment3"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT sign - CxO",						gettext("VAT sign - CxO"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT sign - Accountant",				gettext("VAT sign - Accountant"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT sign - Authorized person",		gettext("VAT sign - Authorized person"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT sign - Entrepreneur",			gettext("VAT sign - Entrepreneur"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT sign - Entrepreneur registration",gettext("VAT sign - Entrepreneur registration"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT rate",				to_string(VAT_PERCENTAGE) + "%", true);

	if(error_message.empty()) error_message = AssignVariableValue("invoice_table_header",	gettext("invoice_table_header"), true);
	if(error_message.empty()) error_message = AssignVariableValue("act_table_header",		gettext("act_table_header"), true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::CostCenter_VarSet() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(error_message.empty()) error_message = AssignVariableValue("cost_center_id", cost_center_id, true);
			if(error_message.empty())
			{
				if(vars.Get("cost_center_id").length())
				{
					MESSAGE_DEBUG("", "", "build cost_center id");

					if(db->Query("SELECT * FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";"))
					{
						// --- take same results 
						vars.Add("cost_center_title", ConvertHTMLToText(db->Get(0, "title")));
						vars.Add("cost_center_agreement_start_date", db->Get(0, "start_date"));
						vars.Add("cost_center_agreement_end_date", db->Get(0, "end_date"));
						vars.Add("cost_center_agreement_number", db->Get(0, "number"));
						vars.Add("cost_center_agreement_sign_date", db->Get(0, "sign_date"));
					}
					else
					{
						MESSAGE_ERROR("", "", "cost_center id not found");
						error_message = gettext("cost_center id not found");
					}
				}
			}

			// --- cost_center number
			if(error_message.empty())
			{
				C_Date_Spelling		date_obj(GetTMObject(Get("cost_center_agreement_sign_date")));

				error_message = AssignVariableValue("cost_center_agreement", "№" + Get("cost_center_agreement_number") + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
			}

            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_company_id", "SELECT `company_id` FROM `cost_centers` WHERE `id`=\"" + Get("cost_center_id") + "\";", true);
            if(error_message.empty())
            {
                if(vars.Get("cost_center_company_id").length())
                {
                    MESSAGE_DEBUG("", "", "build cost_center company id");

                    if(db->Query("SELECT * FROM `company` WHERE `id`=\"" + vars.Get("cost_center_company_id") + "\";"))
                    {
                        vars.Add("cost_center_type",                 ConvertHTMLToText(db->Get(0, "type")));
                        vars.Add("cost_center_name",                 ConvertHTMLToText(db->Get(0, "name")));
                        vars.Add("cost_center_legal_geo_zip_id",     ConvertHTMLToText(db->Get(0, "legal_geo_zip_id")));
                        vars.Add("cost_center_legal_address",        ConvertHTMLToText(db->Get(0, "legal_address")));
                        vars.Add("cost_center_mailing_geo_zip_id",   ConvertHTMLToText(db->Get(0, "mailing_geo_zip_id")));
                        vars.Add("cost_center_mailing_address",      ConvertHTMLToText(db->Get(0, "mailing_address")));
                        vars.Add("cost_center_tin",                  ConvertHTMLToText(db->Get(0, "tin")));
                        vars.Add("cost_center_bank_id",              ConvertHTMLToText(db->Get(0, "bank_id")));
                        vars.Add("cost_center_account",              ConvertHTMLToText(db->Get(0, "account")));
                        vars.Add("cost_center_kpp",                  ConvertHTMLToText(db->Get(0, "kpp")));
                        vars.Add("cost_center_ogrn",                 ConvertHTMLToText(db->Get(0, "ogrn")));
                        vars.Add("cost_center_link",                 ConvertHTMLToText(db->Get(0, "link")));
                        vars.Add("cost_center_admin_userID",         ConvertHTMLToText(db->Get(0, "admin_userID")));
                        vars.Add("cost_center_isConfirmed",          ConvertHTMLToText(db->Get(0, "isConfirmed")));
                        vars.Add("cost_center_webSite",              ConvertHTMLToText(db->Get(0, "webSite")));
                        vars.Add("cost_center_description",          ConvertHTMLToText(db->Get(0, "description")));
                        vars.Add("cost_center_isBlocked",            ConvertHTMLToText(db->Get(0, "isBlocked")));
                        vars.Add("cost_center_logo_folder",          ConvertHTMLToText(db->Get(0, "logo_folder")));
                        vars.Add("cost_center_logo_filename",        ConvertHTMLToText(db->Get(0, "logo_filename")));
                    }
                    else
                    {
                        MESSAGE_ERROR("", "", "cost_center company id not found");
                        error_message = gettext("cost_center company id not found");
                    }
                }
            }
			if(error_message.empty())
			{
				if(vars.Get("cost_center_bank_id").length())
				{
					MESSAGE_DEBUG("", "", "build cost_center bank id");

					if(db->Query("SELECT * FROM `banks` WHERE `id`=\"" + vars.Get("cost_center_bank_id") + "\";"))
					{
						vars.Add("cost_center_bank_id",			ConvertHTMLToText(db->Get(0, "id")));
						vars.Add("cost_center_bank_title",		ConvertHTMLToText(db->Get(0, "title")));
						vars.Add("cost_center_bank_bik",        ConvertHTMLToText(db->Get(0, "bik")));
						vars.Add("cost_center_bank_account",	ConvertHTMLToText(db->Get(0, "account")));
						vars.Add("cost_center_bank_geo_zip_id", ConvertHTMLToText(db->Get(0, "geo_zip_id")));
						vars.Add("cost_center_bank_address",	ConvertHTMLToText(db->Get(0, "address")));
						vars.Add("cost_center_bank_phone",		ConvertHTMLToText(db->Get(0, "phone")));
						vars.Add("cost_center_bank_okato",		ConvertHTMLToText(db->Get(0, "okato")));
						vars.Add("cost_center_bank_okpo",		ConvertHTMLToText(db->Get(0, "okpo")));
						vars.Add("cost_center_bank_regnum",		ConvertHTMLToText(db->Get(0, "regnum")));
						vars.Add("cost_center_bank_srok",		ConvertHTMLToText(db->Get(0, "srok")));
					}
					else
					{
						MESSAGE_ERROR("", "", "cost_center bank id not found");
						error_message = gettext("cost_center bank id not found");
					}
				}
			}
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_legal_geo_zip"               , "SELECT `zip` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_legal_geo_zip_id") + "\";", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_legal_locality_title"        , "SELECT `title` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_legal_geo_zip_id") + "\");", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_legal_region_title"          , "SELECT `title` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_legal_geo_zip_id") + "\"));", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_legal_country_title"         , "SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_legal_geo_zip_id") + "\")));", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_mailing_geo_zip"             , "SELECT `zip` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_mailing_geo_zip_id") + "\";", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_mailing_locality_title"      , "SELECT `title` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_mailing_geo_zip_id") + "\");", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_mailing_region_title"        , "SELECT `title` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_mailing_geo_zip_id") + "\"));", true);
            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_mailing_country_title"       , "SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("cost_center_mailing_geo_zip_id") + "\")));", true);


			if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_title"					, "SELECT `title` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_agreement_start_date"	, "SELECT `start_date` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_agreement_end_date"		, "SELECT `end_date` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_agreement_number"		, "SELECT `number` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_agreement_sign_date"	, "SELECT `sign_date` FROM `cost_centers` WHERE `id`=\"" + vars.Get("cost_center_id") + "\";", true);

			// --- cost center custom fields 
			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `cost_center_custom_fields` WHERE `cost_center_id`=\"" + vars.Get("cost_center_id") + "\";");

				for(auto i = 0; i < affected; ++i)
				{
					if(error_message.empty()) error_message = AssignVariableValue(db->Get(i, "var_name"), db->Get(i, "value"), true);
				}

				if(Get("1C_template_invoice_to_cc_service").length())
					AssignVariableValue("1C_template_invoice_to_cc_service_full_path", TEMPLATE_CC_DIRECTORY + Get("1C_template_invoice_to_cc_service"), true);
				if(Get("1C_template_invoice_to_cc_service_table_row").length())
					AssignVariableValue("1C_template_invoice_to_cc_service_table_row_full_path", TEMPLATE_CC_DIRECTORY + Get("1C_template_invoice_to_cc_service_table_row"), true);
				
				if(Get("1C_template_invoice_to_cc_bt").length())
					AssignVariableValue("1C_template_invoice_to_cc_bt_full_path", TEMPLATE_CC_DIRECTORY + Get("1C_template_invoice_to_cc_bt"), true);
				if(Get("1C_template_invoice_to_cc_bt_table_row").length())
					AssignVariableValue("1C_template_invoice_to_cc_bt_table_row_full_path", TEMPLATE_CC_DIRECTORY + Get("1C_template_invoice_to_cc_bt_table_row"), true);
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::Agency_VarSet(string __agency_company_id) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");


	if(user)
	{
		if(db)
		{
			if(error_message.empty())
			{
				if(__agency_company_id.length())
				{
					MESSAGE_DEBUG("", "", "build agency company id");

					if(db->Query("SELECT * FROM `company` WHERE `id`=\"" + __agency_company_id + "\";"))
					{
						vars.Add("agency_type", 				ConvertHTMLToText(db->Get(0, "type")));
						vars.Add("agency_name", 				ConvertHTMLToText(db->Get(0, "name")));
						vars.Add("agency_legal_geo_zip_id", 	ConvertHTMLToText(db->Get(0, "legal_geo_zip_id")));
						vars.Add("agency_legal_address", 		ConvertHTMLToText(db->Get(0, "legal_address")));
						vars.Add("agency_mailing_geo_zip_id", 	ConvertHTMLToText(db->Get(0, "mailing_geo_zip_id")));
						vars.Add("agency_mailing_address", 		ConvertHTMLToText(db->Get(0, "mailing_address")));
						vars.Add("agency_tin", 					ConvertHTMLToText(db->Get(0, "tin")));
						vars.Add("agency_bank_id", 				ConvertHTMLToText(db->Get(0, "bank_id")));
						vars.Add("agency_account", 				ConvertHTMLToText(db->Get(0, "account")));
						vars.Add("agency_kpp", 					ConvertHTMLToText(db->Get(0, "kpp")));
						vars.Add("agency_ogrn", 				ConvertHTMLToText(db->Get(0, "ogrn")));
						vars.Add("agency_vat",	 				ConvertHTMLToText(db->Get(0, "vat")));
						vars.Add("agency_vat_spelling",			Get("agency_vat") == "N" ? gettext("no VAT") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%");
						vars.Add("agency_vat_spelling_short",	Get("agency_vat") == "N" ? gettext("no VAT short") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%");
						vars.Add("agency_vat_spelling_1C",		Get("agency_vat") == "N" ? gettext("noVAT") : gettext("VAT") + to_string(VAT_PERCENTAGE));
						vars.Add("agency_vat_calculation_type",	ConvertHTMLToText(db->Get(0, "vat_calculation_type")));
						vars.Add("agency_link", 				ConvertHTMLToText(db->Get(0, "link")));
						vars.Add("agency_admin_userID", 		ConvertHTMLToText(db->Get(0, "admin_userID")));
						vars.Add("agency_isConfirmed", 			ConvertHTMLToText(db->Get(0, "isConfirmed")));
						vars.Add("agency_webSite", 				ConvertHTMLToText(db->Get(0, "webSite")));
						vars.Add("agency_description", 			ConvertHTMLToText(db->Get(0, "description")));
						vars.Add("agency_isBlocked", 			ConvertHTMLToText(db->Get(0, "isBlocked")));
						vars.Add("agency_logo_folder", 			ConvertHTMLToText(db->Get(0, "logo_folder")));
						vars.Add("agency_logo_filename", 		ConvertHTMLToText(db->Get(0, "logo_filename")));
						vars.Add("cost_center_act_number", 		GetActFullNumberByCompanyID(__agency_company_id));
					}
					else
					{
						MESSAGE_ERROR("", "", "agency company id not found");
						error_message = gettext("agency company id not found");
					}
				}
			}
			if(error_message.empty())
			{
				if(vars.Get("agency_bank_id").length())
				{
					MESSAGE_DEBUG("", "", "build agency bank id");

					if(db->Query("SELECT * FROM `banks` WHERE `id`=\"" + vars.Get("agency_bank_id") + "\";"))
					{
						vars.Add("agency_bank_id",				ConvertHTMLToText(db->Get(0, "id")));
						vars.Add("agency_bank_title",			ConvertHTMLToText(db->Get(0, "title")));
						vars.Add("agency_bank_bik",	        	ConvertHTMLToText(db->Get(0, "bik")));
						vars.Add("agency_bank_account",			ConvertHTMLToText(db->Get(0, "account")));
						vars.Add("agency_bank_geo_zip_id",		ConvertHTMLToText(db->Get(0, "geo_zip_id")));
						vars.Add("agency_bank_address",			ConvertHTMLToText(db->Get(0, "address")));
						vars.Add("agency_bank_phone",			ConvertHTMLToText(db->Get(0, "phone")));
						vars.Add("agency_bank_okato",			ConvertHTMLToText(db->Get(0, "okato")));
						vars.Add("agency_bank_okpo",			ConvertHTMLToText(db->Get(0, "okpo")));
						vars.Add("agency_bank_regnum",			ConvertHTMLToText(db->Get(0, "regnum")));
						vars.Add("agency_bank_srok",			ConvertHTMLToText(db->Get(0, "srok")));
					}
					else
					{
						MESSAGE_ERROR("", "", "agency bank id not found");
						error_message = gettext("agency bank id not found");
					}
				}
			}
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_legal_geo_zip"				, "SELECT `zip` FROM `geo_zip` WHERE `id`=\"" + Get("agency_legal_geo_zip_id") + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_legal_locality_title"		, "SELECT `title` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("agency_legal_geo_zip_id") + "\");", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_legal_region_title"			, "SELECT `title` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("agency_legal_geo_zip_id") + "\"));", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_legal_country_title"			, "SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("agency_legal_geo_zip_id") + "\")));", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_mailing_geo_zip"				, "SELECT `zip` FROM `geo_zip` WHERE `id`=\"" + Get("agency_mailing_geo_zip_id") + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_mailing_locality_title"		, "SELECT `title` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("agency_mailing_geo_zip_id") + "\");", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_mailing_region_title"		, "SELECT `title` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("agency_mailing_geo_zip_id") + "\"));", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("agency_mailing_country_title"		, "SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("agency_mailing_geo_zip_id") + "\")));", true);

			// --- agency company custom fields 
			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `company_custom_fields` WHERE "
												"`company_id`=\"" + __agency_company_id + "\";");

				for(auto i = 0; i < affected; ++i)
				{
					if(error_message.empty()) error_message = AssignVariableValue(db->Get(i, "var_name"), db->Get(i, "value"), true);
				}

				if(Get("1C_template_payment_to_subcontractor").length())
					AssignVariableValue("1C_template_payment_to_subcontractor_full_path", TEMPLATE_COMPANY_DIRECTORY + Get("1C_template_payment_to_subcontractor"), true);
				if(Get("1C_template_payment_to_subcontractor_body").length())
					AssignVariableValue("1C_template_payment_to_subcontractor_body_full_path", TEMPLATE_COMPANY_DIRECTORY + Get("1C_template_payment_to_subcontractor_body"), true);
				
				if(Get("1C_template_payment_order_to_subcontractor").length())
					AssignVariableValue("1C_template_payment_order_to_subcontractor_full_path", TEMPLATE_COMPANY_DIRECTORY + Get("1C_template_payment_order_to_subcontractor"), true);
				if(Get("1C_template_payment_order_to_subcontractor_body").length())
					AssignVariableValue("1C_template_payment_order_to_subcontractor_body_full_path", TEMPLATE_COMPANY_DIRECTORY + Get("1C_template_payment_order_to_subcontractor_body"), true);
				
			}

		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SoW_Index_VarSet(string sql_query, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start(, index = " + index + ")");

	if(user)
	{
		if(db)
		{
			if(db->Query(sql_query))
			{
				auto	sow_id = db->Get(0, "id");
				auto	sow_number = db->Get(0, "number");
				auto	sow_date = db->Get(0, "sign_date");
				auto	sow_start_date = db->Get(0, "start_date");
				auto	sow_end_date = db->Get(0, "end_date");
				auto	subcontractor_company_id = db->Get(0, "subcontractor_company_id");
				auto	subcontractor_dayrate = db->Get(0, "day_rate");
				auto	sow_working_hours_per_day = db->Get(0, "working_hours_per_day");
				auto	sow_payment_period_service = db->Get(0, "payment_period_service");
				auto	sow_payment_period_bt = db->Get(0, "payment_period_bt");

// TODO: cleanup on March 23-rd
/*
				if(db->Query("SELECT * FROM `company` WHERE `id`=\"" + subcontractor_company_id + "\";"))
				{
					auto	act_number = db->Get(0, "act_number_prefix") + db->Get(0, "act_number") + db->Get(0, "act_number_postfix");

					if((error_message = AssignVariableValue("subcontractor_act_number_" + index, act_number, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("act number is empty");
					MESSAGE_ERROR("", "", error_message);
				}
*/
				if(sow_id.length())
				{
					if((error_message = AssignVariableValue("sow_id_" + index, sow_id, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("id is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(sow_number.length())
				{
					if((error_message = AssignVariableValue("sow_number_" + index, sow_number, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("number is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(sow_payment_period_service.length())
				{
					if((error_message = AssignVariableValue("sow_payment_period_service_" + index, sow_payment_period_service, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("service payment period is empty");
					MESSAGE_ERROR("", "", error_message);
				}
				if(sow_payment_period_service.length())
				{
					C_Price_Spelling	price_spelling;

					if((error_message = AssignVariableValue("sow_payment_period_service_spelling_" + index, price_spelling.SpellNumber(stol(sow_payment_period_service), PLURAL_MALE_GENDER), true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("service payment period is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(sow_payment_period_bt.length())
				{
					if((error_message = AssignVariableValue("sow_payment_period_bt_" + index, sow_payment_period_bt, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("bt payment period is empty");
					MESSAGE_ERROR("", "", error_message);
				}
				if(sow_payment_period_bt.length())
				{
					C_Price_Spelling	price_spelling;

					if((error_message = AssignVariableValue("sow_payment_period_bt_spelling_" + index, price_spelling.SpellNumber(stol(sow_payment_period_bt), PLURAL_MALE_GENDER), true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("bt payment period is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(subcontractor_dayrate.length())
				{
					if((error_message = AssignVariableValue("subcontractor_dayrate_" + index, subcontractor_dayrate, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("dayrate is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(subcontractor_dayrate.length())
				{
					C_Price_Spelling	price_spelling(stod_noexcept(subcontractor_dayrate));

					if((error_message = AssignVariableValue("subcontractor_dayrate_spelling_" + index, price_spelling.Spelling(), true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("dayrate is empty");
					MESSAGE_ERROR("", "", error_message);
				}


				if(stod_noexcept(sow_working_hours_per_day) != 0)
				{
					if((error_message = AssignVariableValue("sow_working_hours_per_day_" + index, sow_working_hours_per_day, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("working hours per day not set");
					MESSAGE_ERROR("", "", error_message);
				}

				if(sow_date.length())
				{
					if((error_message = AssignVariableValue("sow_sign_date_" + index, sow_date, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("date is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(sow_start_date.length())
				{
					if((error_message = AssignVariableValue("sow_start_date_" + index, sow_start_date, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("start date is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(sow_end_date.length())
				{
					if((error_message = AssignVariableValue("sow_end_date_" + index, sow_end_date, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("end date is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				if(subcontractor_company_id.length())
				{
					if((error_message = AssignVariableValue("subcontractor_company_id_" + index, subcontractor_company_id, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + sow_number + " " + gettext("agreement from") + " " + sow_date + " " + gettext("subc company.id is empty");
					MESSAGE_ERROR("", "", error_message);
				}

				// --- scoping
				{
					C_Date_Spelling	date_obj;

					date_obj.SetTMObj(GetTMObject(vars.Get("sow_sign_date_" + index)));

					if(error_message.empty()) error_message = AssignVariableValue("sow_sign_date_spelling_" + index, date_obj.Spell(), true);
					if(error_message.empty()) error_message = AssignVariableValue("sow_agreement_" + index, "№" + vars.Get("sow_number_" + index) + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
				}
				{
					C_Date_Spelling	date_obj;

					date_obj.SetTMObj(GetTMObject(vars.Get("sow_start_date_" + index)));

					if(error_message.empty()) error_message = AssignVariableValue("sow_start_date_spelling_" + index, date_obj.Spell(), true);
				}
				{
					C_Date_Spelling	date_obj;

					date_obj.SetTMObj(GetTMObject(vars.Get("sow_end_date_" + index)));

					if(error_message.empty()) error_message = AssignVariableValue("sow_end_date_spelling_" + index, date_obj.Spell(), true);
				}
				// --- calculate contract duration in days
				{
					auto	start_sow		= GetTMObject(vars.Get("sow_start_date_" + index));
					auto	end_sow			= GetTMObject(vars.Get("sow_end_date_" + index));
					int		number_of_days	= (mktime(&end_sow) - mktime(&start_sow)) / (3600 * 24) + 1;

					if(error_message.empty()) error_message = AssignVariableValue("sow_duration_in_days_" + index, to_string(number_of_days), true);
				}
			}
			else
			{
				error_message = gettext("SQL syntax error");
				MESSAGE_ERROR("", "", "SQL syntax error");
			}

			if(error_message.empty())
			{
				auto	company_position_id = cache.Get("SELECT `company_position_id` FROM `contracts_sow` WHERE `id`=\"" + Get("sow_id_" + index) + "\";", db, user, 
																			[](string sql_query, CMysql *db, CUser *)
																			{
																				return (db->Query(sql_query) ? db->Get(0, 0) : ""s);
																			});

				if(company_position_id.length())
				{
					auto	local_service_description = cache.Get(	"SELECT `local_service_description` FROM `company_position` WHERE `id`=\"" + company_position_id + "\";", db, user,
																			[](string sql_query, CMysql *db, CUser *)
																			{
																				return (db->Query(sql_query) ? db->Get(0, 0) : ""s);
																			});

					if(local_service_description.length())
					{
						auto	remote_service_description = cache.Get(	"SELECT `remote_service_description` FROM `company_position` WHERE `id`=\"" + company_position_id + "\";", db, user,
																				[](string sql_query, CMysql *db, CUser *)
																				{
																					return (db->Query(sql_query) ? db->Get(0, 0) : ""s);
																				});

						if(remote_service_description.length())
						{
							error_message = AssignVariableValue("subcontractor_position_id_" + index, company_position_id, true);
							error_message = AssignVariableValue("subcontractor_position_local_service_description_" + index, local_service_description, true);
							error_message = AssignVariableValue("subcontractor_position_remote_service_description_" + index, remote_service_description, true);
						}
						else
						{
							if(db->Query("SELECT `title` FROM `company_position` WHERE `id`=\"" + company_position_id + "\";"))
							{
								error_message = gettext("Occupancy") + " "s + db->Get(0, 0) + " " + gettext("missed service description");
								MESSAGE_ERROR("", "", "Occupancy " + db->Get(0, 0) + " missed remote service description");
							}
							else
							{
								error_message = gettext("SQL syntax error");
								MESSAGE_ERROR("", "", "SQL syntax error");
							}
						}
					}
					else
					{
						if(db->Query("SELECT `title` FROM `company_position` WHERE `id`=\"" + company_position_id + "\";"))
						{
							error_message = gettext("Occupancy") + " "s + db->Get(0, 0) + " " + gettext("missed service description");
							MESSAGE_ERROR("", "", "Occupancy " + db->Get(0, 0) + " missed service description");
						}
						else
						{
							error_message = gettext("SQL syntax error");
							MESSAGE_ERROR("", "", "SQL syntax error");
						}
					}
				}
				else
				{
					error_message = gettext("Agreement") + " "s + Get("sow_agreement_" + index) + " " + gettext("missed occupancy title");
					MESSAGE_ERROR("", "", error_message);
				};
			}

/*
			// --- subcontractor sow custom fields
			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + vars.Get("sow_id_" + index) + "\";");

				for(auto j = 0; j < affected; ++j)
				{
					if(error_message.empty()) error_message = AssignVariableValue(db->Get(j, "var_name") + "_" + index, db->Get(j, "value"), true);
				}
			}
*/
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SoW_Custom_Fields(string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			// --- subcontractor sow custom fields
			auto	affected = db->Query("SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + vars.Get("sow_id_" + index) + "\";");

			for(auto j = 0; j < affected; ++j)
			{
				if(error_message.empty()) error_message = AssignVariableValue(db->Get(j, "var_name") + "_" + index, db->Get(j, "value"), true);
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::PSoW_Index_VarSet(string psow_id, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(psow_id.length())
			{
				if(error_message.empty()) error_message = AssignVariableValue("psow_id_" + index, psow_id, true);

				if(db->Query("SELECT * FROM `contracts_psow` WHERE `id`=\"" + vars.Get("psow_id_" + index) + "\";"))
				{
					C_Date_Spelling		date_obj;

					if(error_message.empty())
					{
						if(db->Get(0, "number").length())
						{
							error_message = AssignVariableValue("psow_contract_number_" + index, db->Get(0, "number"), true);
						}
						else
						{
							error_message = "PSOW "s + gettext("number") + " " + gettext("is empty") + "(" + Get("sow_agreement_" + index) + ")";
							MESSAGE_ERROR("", "", error_message);
						}
					}
					if(error_message.empty())
					{
						if(db->Get(0, "sign_date").length())
						{
							error_message = AssignVariableValue("psow_contract_sign_date_" + index, db->Get(0, "sign_date"), true);
						}
						else
						{
							error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("sign date") + " " + gettext("is empty");
							MESSAGE_ERROR("", "", error_message);
						}
					}
					if(error_message.empty())
					{
						if(db->Get(0, "start_date").length())
						{
							error_message = AssignVariableValue("psow_contract_start_date_" + index, db->Get(0, "start_date"), true);
						}
						else
						{
							error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("start date") + " " + gettext("is empty");
							MESSAGE_ERROR("", "", error_message);
						}
					}
					if(error_message.empty())
					{
						if(db->Get(0, "end_date").length())
						{
							error_message = AssignVariableValue("psow_contract_end_date_" + index, db->Get(0, "end_date"), true);
						}
						else
						{
							error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("end date") + " " + gettext("is empty");
							MESSAGE_ERROR("", "", error_message);
						}
					}
					if(error_message.empty())
					{
						if(db->Get(0, "day_rate").length())
						{
							error_message = AssignVariableValue("psow_contract_day_rate_" + index, db->Get(0, "day_rate"), true);
						}
						else
						{
							error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("day rate") + " " + gettext("is empty");
							MESSAGE_ERROR("", "", error_message);
						}
					}

					date_obj.SetTMObj(GetTMObject(vars.Get("psow_contract_sign_date_" + index)));

					if(error_message.empty()) error_message = AssignVariableValue("psow_agreement_" + index, "№" + vars.Get("psow_contract_number_" + index) + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
				}
				else
				{
					error_message = gettext("PSoW") + "("s + vars.Get("psow_id_" + index) + ")" + gettext("not found");
					MESSAGE_ERROR("", "", error_message);
				}
			}
			else
			{
				error_message = gettext("Agreement") + " "s + Get("sow_agreement_" + index) + " " + gettext("missed PSoW number");
				MESSAGE_ERROR("", "", error_message);
			}

			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `contract_psow_custom_fields` WHERE `contract_psow_id`=\"" + vars.Get("psow_id_" + index) + "\";");

				for(auto j = 0; j < affected; ++j)
				{
					if(error_message.empty()) error_message = AssignVariableValue("psow_contract_" + index + "_" + db->Get(j, "var_name"), db->Get(j, "value"), true);
				}
			}

			// --- Department spelling (taken from custom_vars)
			if(error_message.empty())
			{						
				error_message = AssignVariableValue("psow_contract_" + index + "_Department_spelling", 
									Get("psow_contract_" + index + "_Department").length()
									? " ("s + gettext("department") + " " + Get("psow_contract_" + index + "_Department") + ")"
									: "", true);
				if(error_message.length())
				{ MESSAGE_ERROR("", "", "fail to assign variable"); }
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::CostCenterPayment_Index_VarSet(c_float cost_center_price, string index) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto						error_message = ""s;
	
	// --- having "enforced rounding" here VERY IMPORTANT !!!. 
	// --- VAT must be calculated from rounded base-price, otherwise it is easy to loose dime
	// --- more details could be found in SR 438840
	c_float						cost_center_price_local = GetBaseCostFrom(cost_center_price);
	c_float						cost_center_vat;
	c_float						cost_center_total_payment;


	if(Get("agency_vat") == "Y") cost_center_vat = GetVATFrom(cost_center_price_local);
	cost_center_total_payment = GetTotalPaymentFrom(cost_center_price_local, cost_center_vat);

	if(error_message.empty()) error_message = AssignVariableValue("cost_center_price_" + index, string(cost_center_price_local), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_vat_" + index,	string(cost_center_vat), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_total_" + index, string(cost_center_total_payment), true);


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SubcontractorPayment_Index_VarSet(c_float subcontractor_price, string index) -> string
{
	MESSAGE_DEBUG("", "", "start (subcontractor_price: " + string(subcontractor_price) + " (will be rounded))");

	auto						error_message = ""s;
	c_float						subcontractor_price_local = GetBaseCostFrom(subcontractor_price);
	c_float						subcontractor_vat;
	c_float						subcontractor_total_payment;

	if(Get("subcontractor_company_vat_" + index) == "Y") subcontractor_vat = GetVATFrom(subcontractor_price_local);
	subcontractor_total_payment = GetTotalPaymentFrom(subcontractor_price_local, subcontractor_vat);

	if(error_message.empty()) error_message = AssignVariableValue("timecard_price_" + index, subcontractor_price_local	.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("timecard_vat_"   + index, subcontractor_vat			.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("timecard_total_" + index, subcontractor_total_payment.PrintPriceTag(), true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SubcontractorAddress_Index_VarSet(string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (index: " + index + ")");

	if(user)
	{
		if(db)
		{
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_legal_geo_zip_"			+ index , "SELECT `zip` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_legal_geo_zip_id_" + index) + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_legal_locality_title_"	+ index , "SELECT `title` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_legal_geo_zip_id_" + index) + "\");", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_legal_region_title_"		+ index , "SELECT `title` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_legal_geo_zip_id_" + index) + "\"));", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_legal_country_title_"		+ index , "SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_legal_geo_zip_id_" + index) + "\")));", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_mailing_geo_zip_"			+ index , "SELECT `zip` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_mailing_geo_zip_id_" + index) + "\";", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_mailing_locality_title_"	+ index , "SELECT `title` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_mailing_geo_zip_id_" + index) + "\");", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_mailing_region_title_"	+ index , "SELECT `title` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_mailing_geo_zip_id_" + index) + "\"));", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_mailing_country_title_"	+ index , "SELECT `title` FROM `geo_country` WHERE `id`=(SELECT `geo_country_id` FROM `geo_region` WHERE `id`=(SELECT `geo_region_id` FROM `geo_locality` WHERE `id`=(SELECT `geo_locality_id` FROM `geo_zip` WHERE `id`=\"" + Get("subcontractor_mailing_geo_zip_id_" + index) + "\")));", true);

			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_user_name_"				+ index , "SELECT `name` FROM `users` WHERE `id`=(SELECT `admin_userID` FROM `company` WHERE `id`=\"" + Get("subcontractor_company_id_" + index) + "\");", true);
			if(error_message.empty()) error_message = AssignVariableFromDB("subcontractor_user_name_last_"			+ index , "SELECT `nameLast` FROM `users` WHERE `id`=(SELECT `admin_userID` FROM `company` WHERE `id`=\"" + Get("subcontractor_company_id_" + index) + "\");", true);
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::Subcontractor_Index_VarSet(string subcontractor_company_id, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(db->Query("SELECT * FROM `company` WHERE `id`=\"" + subcontractor_company_id + "\";"))
			{
				auto	subcontractor_vat = db->Get(0, "vat");

				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_account_" + index, db->Get(0, "account"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_short_name_" + index, db->Get(0, "name"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_name_" + index, db->Get(0, "name"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_bank_id_" + index, db->Get(0, "bank_id"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_tin_" + index, db->Get(0, "tin"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_ogrn_" + index, db->Get(0, "ogrn"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_kpp_" + index, db->Get(0, "kpp"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_" + index, subcontractor_vat, true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_boolean_" + index, subcontractor_vat == "Y" ? "true" : "false", true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_spelling_" + index, subcontractor_vat == "N" ? gettext("no VAT") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%", true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_spelling_short_" + index, subcontractor_vat == "N" ? gettext("no VAT short") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%", true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_spelling_1C_" + index, subcontractor_vat == "N" ? gettext("noVAT") : gettext("VAT") + to_string(VAT_PERCENTAGE), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_calculation_type_" + index, db->Get(0, "vat_calculation_type"), true);

				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_mailing_geo_zip_id_" + index, db->Get(0, "mailing_geo_zip_id"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_legal_geo_zip_id_" + index, db->Get(0, "legal_geo_zip_id"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_mailing_address_" + index, db->Get(0, "mailing_address"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_legal_address_" + index, db->Get(0, "legal_address"), true);
			}
			else
			{
				error_message = gettext("SQL syntax error");
				MESSAGE_ERROR("", "", error_message);
			}

			if(error_message.empty())
			{
				auto act_full_number = GetActFullNumberByCompanyID(subcontractor_company_id);

				if(act_full_number.length())
				{
					error_message = AssignVariableValue("subcontractor_act_number_" + index, act_full_number, true);
				}
				else
				{
					error_message = gettext("act number is empty");
					MESSAGE_ERROR("", "", error_message);
				}
			}

			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `banks` WHERE `id`=\"" + vars.Get("subcontractor_company_bank_id_" + index) + "\";");

				if(affected)
				{
					if(error_message.empty()) error_message = AssignVariableValue("subcontractor_bank_title_" + index, db->Get(0, "title"), true);
					if(error_message.empty()) error_message = AssignVariableValue("subcontractor_bank_bik_" + index, db->Get(0, "bik"), true);
					if(error_message.empty()) error_message = AssignVariableValue("subcontractor_bank_account_" + index, db->Get(0, "account"), true);
				}
				else
				{
					error_message = gettext("SQL syntax error");
					MESSAGE_ERROR("", "", error_message);
				}
			}

			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `users` WHERE `id`=(SELECT `admin_userID` FROM `company` WHERE `id`=" + quoted(subcontractor_company_id) + ");");

				if(affected)
				{
					if(error_message.empty()) error_message = AssignVariableValue("subcontractor_email_" + index, db->Get(0, "email"), true);
					if(error_message.empty()) error_message = AssignVariableValue("subcontractor_phone_" + index, db->Get(0, "is_phone_confirmed") == "Y" ? "+" + db->Get(0, "country_code") + db->Get(0, "phone") : "("s + gettext("no phone entered") + ")", true);
				}
				else
				{
					error_message = gettext("SQL syntax error");
					MESSAGE_ERROR("", "", error_message);
				}
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::TableRowDescriptions_Index_VarSet(string local_remote_service_description, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty())
	{
		auto	temp = 	local_remote_service_description + " ";

		if(Get("subcontractor_work_period_short_spelling_" + index).length())
			temp +=		gettext("za") + " "s + Get("subcontractor_work_period_short_spelling_" + index) + " ";
		else
			temp +=		Get("from") + " " + Get("subcontractor_work_period_start" + index) + " " + Get("up to") + " " + Get("subcontractor_work_period_finish" + index) + " ";

		temp +=			Get("in scope") + " " + Get("Technical Requirement agreement short") + " " + Get("psow_agreement_" + index) +
						(Get("psow_contract_" + index + "_purchase_order").length() ? " (" + Get("psow_contract_" + index + "_purchase_order") + ")" : "") +
						Get("psow_contract_" + index + "_Department_spelling") +
						".";
						// --- VAT not required in row description 
						// " " + Get("agency_vat_spelling") + ".";

		error_message = AssignVariableValue("cost_center_table_row_description_" + index, temp, true);

		if(error_message.length())
		{ MESSAGE_ERROR("", "", "fail to assign variable"); }
	}

	if(error_message.empty())
	{
		auto	temp =	local_remote_service_description + " ";

		if(Get("subcontractor_work_period_short_spelling_" + index).length())
			temp +=		gettext("za") + " "s + Get("subcontractor_work_period_short_spelling_" + index) + " ";
		else
			temp +=		Get("from") + " " + Get("subcontractor_work_period_start" + index) + " " + Get("up to") + " " + Get("subcontractor_work_period_finish" + index) + " ";

		temp +=			Get("in scope") + " " + Get("agreement declensioned") + " " + Get("sow_agreement_" + index) + "."
						" " + Get("subcontractor_company_vat_spelling_" + index) + ".";

		error_message = AssignVariableValue("subcontractor_table_row_description_" + index, temp ,true);

		if(error_message.length())
		{ MESSAGE_ERROR("", "", "fail to assign variable"); }
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::Common_Index_VarSet(string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty()) error_message = AssignVariableValue("index_" + index, index, true);
	if(error_message.empty()) error_message = AssignVariableValue("quantity_" + index, "1", true);
	if(error_message.empty()) error_message = AssignVariableValue("item_" + index, Get("item short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("vat_rate_" + index, to_string(int(VAT_PERCENTAGE)), true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::DocumentSubmissionDate_1C_Index_VarSet(const struct tm &date_inside_month_obj, string index) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto		error_message = ""s;
/*
	struct tm	first_day_of_month, last_day_of_month;

	tie(first_day_of_month, last_day_of_month) = GetFirstAndLastMonthDaysByDate(date_inside_month_obj);

	// --- former implementation:
	// ---  *) subc_payment and subc_payment_order - will take last month of current item (BT or TC)
	// ---  *) cost_center - will take latest item (BT or TC) in <vector> and take the date

	if(error_message.empty()) error_message = AssignVariableValue("LastDayOfMonth_Date_1CFormat_" + index, GetSpellingFormattedDate(last_day_of_month, "%F"), true);
*/
	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::DocumentSubmissionDate_1C_Index_VarSet(string date_inside_month, string index) -> string
{
	return DocumentSubmissionDate_1C_Index_VarSet(GetTMObject(date_inside_month), index);
}

auto	C_Invoicing_Vars::SubcontractorsTotal_VarSet(c_float sum, c_float vat_sum_by_row) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	c_float	sum_local = GetBaseCostFrom(sum);
	c_float	vat_local(0);
	c_float total_local;

	// --- index "1" set in stone, due to only "one" subcontractor can report service/bt
	// --- if this calculations important "per subcontractor", index must be re-engineered
	if(Get("subcontractor_company_vat_calculation_type_1") == "percentage")
		vat_local = GetVATFrom(sum);
	else
		vat_local = vat_sum_by_row; // --- if variable is not defined , this value will be "by default" 

	total_local = GetTotalPaymentFrom(sum, vat_local);

	// --- not used anywhere
	if(error_message.empty()) error_message = AssignVariableValue("subcontractors_sum_amount",		sum_local.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("subcontractors_vat_amount",		vat_local.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("subcontractors_total_payment",	total_local.PrintPriceTag(), true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::CostCenterTotal_VarSet(c_float sum, c_float vat_sum_by_row) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	c_float	sum_local = GetBaseCostFrom(sum);
	c_float	vat_local(0);
	c_float total_local;

	if(Get("agency_vat") == "Y")
	{
		if(Get("agency_vat_calculation_type") == "percentage")
			vat_local = GetVATFrom(sum);
		else
			vat_local = vat_sum_by_row;
	}

	total_local = GetTotalPaymentFrom(sum, vat_local);

	if(error_message.empty()) error_message = AssignVariableValue("cost_center_sum_amount",		sum_local.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_vat_amount",		vat_local.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_total_payment",	total_local.PrintPriceTag(), true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}


auto	C_Invoicing_Vars::CurrentTimestamp_1C_VarSet() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");


	if(user)
	{
		if(db)
		{
			// --- current timestamp
			if(error_message.empty())
			{
				time_t 				rawtime;
				struct tm * 		timeinfo;
				C_Date_Spelling		date_obj;
				// char				buffer[128];

				time (&rawtime);
				timeinfo = localtime (&rawtime);

				// strftime(buffer, sizeof(buffer), "%FT%T", timeinfo);
				date_obj.SetTMObj(*timeinfo);

				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("Timestamp_1CFormat", date_obj.GetFormatted("%FT%T"), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::Workperiod_vs_SoWPeriod_vs_PSoWperiod_Index_VarSet(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	// --- define dates resided inside PSoW
	auto				psow_period_start		= GetTMObject(Get("psow_contract_start_date_" + index));
	auto				psow_period_finish		= GetTMObject(Get("psow_contract_end_date_" + index));
	auto				psow_sign_date			= GetTMObject(Get("psow_contract_sign_date_" + index));
	auto				sow_period_start		= GetTMObject(Get("sow_start_date_" + index));
	auto				sow_period_finish		= GetTMObject(Get("sow_end_date_" + index));
	auto				sow_sign_date			= GetTMObject(Get("sow_sign_date_" + index));

	if(psow_period_start <= GetTMObject("2000-01-01"))
	{
		error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("start date") + " " + gettext("must be in 20-th century");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(psow_period_finish <= GetTMObject("2000-01-01"))
	{
		error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("end date") + " " + gettext("must be in 20-th century");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(psow_sign_date <= GetTMObject("2000-01-01"))
	{
		error_message = "PSOW("s + Get("psow_contract_number_" + index) + ") " + gettext("sign date") + " " + gettext("must be in 20-th century");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(sow_period_start <= GetTMObject("2000-01-01"))
	{
		error_message = "SOW("s + Get("sow_contract_number_" + index) + ") " + gettext("start date") + " " + gettext("must be in 20-th century");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(sow_period_finish <= GetTMObject("2000-01-01"))
	{
		error_message = "SOW("s + Get("sow_contract_number_" + index) + ") " + gettext("end date") + " " + gettext("must be in 20-th century");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(sow_sign_date <= GetTMObject("2000-01-01"))
	{
		error_message = "SOW("s + Get("sow_contract_number_" + index) + ") " + gettext("sign date") + " " + gettext("must be in 20-th century");
		MESSAGE_ERROR("", "", error_message);
	}
	else
	{
		error_message = Workperiod_Index_VarSet(
													max(max(psow_period_start,  sow_period_start),  workperiod_start),
													min(min(psow_period_finish, sow_period_finish), workperiod_finish),
													index
												);
/*
		error_message = Workperiod_Index_VarSet(
													psow_period_start  > workperiod_start  ? psow_period_start  : workperiod_start,
													psow_period_finish < workperiod_finish ? psow_period_finish : workperiod_finish,
													index
												);
*/
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::ShortenWorkPeriodSpelling(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(workperiod_start.tm_mon == workperiod_finish.tm_mon)
	{
		// --- try to shorten to month
		if(workperiod_start.tm_mday == 1)
		{
			struct tm	last_day_of_month = workperiod_finish;

			last_day_of_month.tm_mon += 1;
			last_day_of_month.tm_mday = 0;
			mktime(&last_day_of_month);

			if(workperiod_finish == last_day_of_month)
			{
				C_Date_Spelling		temp;

				error_message = AssignVariableValue("subcontractor_work_period_short_spelling_" + index, temp.SpellMonth(workperiod_start.tm_mon + 1), true);
			}
			else
			{
				MESSAGE_DEBUG("", "", "workperiod(" + GetSpellingFormattedDate(workperiod_start, "%D") + " - " + GetSpellingFormattedDate(workperiod_finish, "%D") + ") finishes not at last month day");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "workperiod(" + GetSpellingFormattedDate(workperiod_start, "%D") + " - " + GetSpellingFormattedDate(workperiod_finish, "%D") + ") starts not at 1-st");
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "workperiod(" + GetSpellingFormattedDate(workperiod_start, "%D") + " - " + GetSpellingFormattedDate(workperiod_finish, "%D") + ") starts and finishes in different months");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto	C_Invoicing_Vars::Workperiod_Index_VarSet(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	// --- define dates resided inside PSoW
	C_Date_Spelling		report_period_start;
	C_Date_Spelling		report_period_finish;

	report_period_start	.SetTMObj(workperiod_start);
	report_period_finish.SetTMObj(workperiod_finish);

	if(ShortenWorkPeriodSpelling(workperiod_start, workperiod_finish, index).length())
	{
		MESSAGE_DEBUG("", "", "period shortening didn't work");
	}

	error_message = AssignVariableValue("subcontractor_work_period_start" + index, report_period_start.GetFormatted("%d.%m.%Y"), true);
	error_message = AssignVariableValue("subcontractor_work_period_finish" + index, report_period_finish.GetFormatted("%d.%m.%Y"), true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}


auto	C_Invoicing_Vars::AgreementNumberSpelling_VarSet(string agreement_number) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");


	if(user)
	{
		if(db)
		{
			// --- invoice number
			if(error_message.empty())
			{
				time_t 				rawtime;
				struct tm * 		timeinfo;
				C_Date_Spelling		date_obj;
				// char				buffer[128];

				if(timecard_obj_list.size())
				{
					// --- note that tc_objs are not in sorted order.
					// --- last in <vector> not necessary will be latest by date
					// --- if TCs need to be sorted, then have to sort it explicitly
					auto		last_idx = timecard_obj_list.size() - 1;

					date_obj.SetTMObj(GetTMObject(timecard_obj_list[last_idx].GetDateFinish()));
				}
				else if(bt_obj_list.size())
				{
					// --- note that bt_objs are not in sorted order.
					// --- last in <vector> not necessary will be latest by date
					// --- if BTs need to be sorted, then have to sort it explicitly
					auto		last_idx = bt_obj_list.size() - 1;
					struct tm	start_of_mon, end_of_mon;

					tie(start_of_mon, end_of_mon) = GetFirstAndLastMonthDaysByDate(GetTMObject(bt_obj_list[last_idx].GetDateFinish()));
					date_obj.SetTMObj(end_of_mon);
				}
				else
				{
					MESSAGE_ERROR("", "", "while poke around the code, I didn't understand how to get to this block. If this error_message will appear, then need to investigate 'why workflow get you here'. This is not an error, just out of curiosity investigate this branch.");

					time (&rawtime);
					timeinfo = localtime (&rawtime);

					date_obj.SetTMObj(*timeinfo);
				}

				// strftime(buffer, sizeof(buffer), "%B %Y", timeinfo);


				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("invoice_agreement", "№" + agreement_number + " " + Get("agreement from") + " " + date_obj.Spell(), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("Invoicing_Timestamp_1CFormat", date_obj.GetFormatted("%FT%T"), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("Invoicing_Date_1CFormat", date_obj.GetFormatted("%F"), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("comment_1C_suffix", date_obj.GetFormatted("%B %Y"), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
			}

		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::GenerateServiceVariableSet_AgencyToCC() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			c_float	subcontractors_sum, subcontractors_vat;
			c_float	cost_center_sum, cost_center_vat;


			if(error_message.empty())
			{
				if((error_message = FillStaticDictionary()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from static dictionary"); }
			}

			if(error_message.empty())
			{
				if((error_message = CostCenter_VarSet()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from cost center var set"); }
			}

			if(error_message.empty())
			{
				if((error_message = CurrentTimestamp_1C_VarSet()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from 1C current timestamp"); }
			}

			// --- cost_center number
			if(error_message.empty())
			{
				C_Date_Spelling		date_obj(GetTMObject(Get("cost_center_agreement_sign_date")));

				error_message = AssignVariableValue("cost_center_agreement", "№" + Get("cost_center_agreement_number") + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
			}

			if(error_message.empty())
			{
				if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";", true);

				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }

			}

			if(error_message.empty())
			{
				if((error_message = AgreementNumberSpelling_VarSet(Get("cost_center_act_number"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
			}

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &timecard: timecard_obj_list)
				{
					c_float		timecard_days;

					++i;

					if(error_message.empty()) error_message = AssignVariableValue("timecard_id_" + to_string(i), timecard.GetID(), true);
					if(error_message.empty())
					{
						if((error_message = Common_Index_VarSet(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Common_Index_VarSet"); }
					}
					if(error_message.empty())
					{
						if((error_message = DocumentSubmissionDate_1C_Index_VarSet(timecard.GetDateFinish(), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from DocumentSubmissionDate_1C_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SoW_Index_VarSet("SELECT * FROM `contracts_sow` WHERE `id`=(" + Get_SoWIDByTimecardID_sqlquery(vars.Get("timecard_id_" + to_string(i))) + ");", to_string(i))).empty())
						{
							timecard_days = timecard.GetTotalHours() / c_float(Get("sow_working_hours_per_day_" + to_string(i)));
						}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					// --- subcontractor company data
					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						auto	psow_id = GetPSoWIDByTimecardIDAndCostCenterID(vars.Get("timecard_id_" + to_string(i)), vars.Get("cost_center_id"), db, user);

						if((error_message = PSoW_Index_VarSet(psow_id, to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from PSoW_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = CostCenterPayment_Index_VarSet(c_float(Get("psow_contract_day_rate_" + to_string(i))) * timecard_days, to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from CostCenterPayment_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_vs_SoWPeriod_vs_PSoWperiod_Index_VarSet(GetTMObject(timecard.GetDateStart()), GetTMObject(timecard.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_vs_SoWPeriod_vs_PSoWperiod_Index_VarSet"); }
					}

					// --- subcontractor payment
					if(error_message.empty())
					{
						c_float		timecard_price = c_float(Get("subcontractor_dayrate_" + to_string(i))) * timecard_days;

						if((error_message = SubcontractorPayment_Index_VarSet(timecard_price, to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SubcontractorPayment_Index_VarSet"); }
					}

					// --- custom fields comes after Subc_index_varset due to bank details could be overloaded
					if(error_message.empty())
					{
						if((error_message = SoW_Custom_Fields(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Custom_Fields"); }
					}

					// --- table row description
					if(error_message.empty())
					{
						if((error_message = TableRowDescriptions_Index_VarSet(Get("subcontractor_position_local_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDescriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;
					cost_center_sum = cost_center_sum + c_float(Get("cost_center_price_" + to_string(i)));
					cost_center_vat = cost_center_vat + c_float(Get("cost_center_vat_" + to_string(i)));

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum, subcontractors_vat)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}

				// --- cost_center calculations scoping
				if(error_message.empty())
				{
					if((error_message = CostCenterTotal_VarSet(cost_center_sum, cost_center_vat)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from CostCenterTotalVarSet"); }
				}
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::GenerateServiceVariableSet_SubcToAgency() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			c_float	subcontractors_sum, subcontractors_vat;

			if(error_message.empty())
			{
				if((error_message = FillStaticDictionary()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from static dictionary"); }
			}

			if(error_message.empty())
			{
				if(timecard_obj_list.size())
				{
					if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + timecard_obj_list[0].GetSoWID() + "\";", true);
				}
				else
				{
					error_message = gettext("no timecard found");
					MESSAGE_ERROR("", "", "timecard_obj_list is empty.");
				}

				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }
			}

			if(error_message.empty())
			{
				if(timecard_obj_list.size())
				{
					auto	company_id = GetValueFromDB(Get_SubcCompanyIDBySoWID_sqlquery(timecard_obj_list[0].GetSoWID()), db);

					if((error_message = AgreementNumberSpelling_VarSet(GetActFullNumberByCompanyID(company_id))).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
				}
				else
				{
					error_message = gettext("no timecard found");
					MESSAGE_ERROR("", "", "timecard_obj_list is empty.");
				}

			}

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &timecard: timecard_obj_list)
				{
					c_float		timecard_days;

					++i;

					if(error_message.empty()) error_message = AssignVariableValue("timecard_id_" + to_string(i), timecard.GetID(), true);
					if(error_message.empty())
					{
						if((error_message = Common_Index_VarSet(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Common_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SoW_Index_VarSet("SELECT * FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + vars.Get("timecard_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) 
						{
							timecard_days = timecard.GetTotalHours() / c_float(Get("sow_working_hours_per_day_" + to_string(i)));
						}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					// --- subcontractor company data
					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_Index_VarSet(GetTMObject(timecard.GetDateStart()), GetTMObject(timecard.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SubcontractorAddress_Index_VarSet("1")).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SubcontractorAddress_Index_VarSet"); }
					}

					// --- custom fields comes after Subc_index_varset due to bank details could be overloaded
					if(error_message.empty())
					{
						if((error_message = SoW_Custom_Fields(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Custom_Fields"); }
					}

					// --- subcontractor payment
					if(error_message.empty())
					{
						c_float		timecard_price = c_float(Get("subcontractor_dayrate_" + to_string(i))) * timecard_days;

						if((error_message = SubcontractorPayment_Index_VarSet(timecard_price, to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SubcontractorPayment_Index_VarSet"); }
					}

					// --- table row description
					if(error_message.empty())
					{
						if((error_message = TableRowDescriptions_Index_VarSet(Get("subcontractor_position_local_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDescriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum, subcontractors_vat)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::GenerateBTVariableSet_AgencyToCC() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			c_float	subcontractors_sum, subcontractors_vat;
			c_float	cost_center_sum, cost_center_vat;

			if(error_message.empty())
			{
				if((error_message = FillStaticDictionary()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from static dictionary"); }
			}

			if(error_message.empty())
			{
				if((error_message = CostCenter_VarSet()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from cost center var set"); }
			}

			if(error_message.empty())
			{
				if((error_message = CurrentTimestamp_1C_VarSet()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from 1C current timestamp"); }
			}

			// --- cost_center number
			if(error_message.empty())
			{
				C_Date_Spelling		date_obj(GetTMObject(Get("cost_center_agreement_sign_date")));

				error_message = AssignVariableValue("cost_center_agreement", "№" + Get("cost_center_agreement_number") + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
			}

			if(error_message.empty())
			{
				if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";", true);

				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }
			}

			if(error_message.empty())
			{
				if((error_message = AgreementNumberSpelling_VarSet(Get("cost_center_act_number"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
			}

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &bt: bt_obj_list)
				{
					++i;

					if(error_message.empty()) error_message = AssignVariableValue("bt_id_" + to_string(i), bt.GetID(), true);
					if(error_message.empty())
					{
						if((error_message = Common_Index_VarSet(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Common_Index_VarSet"); }
					}
					if(error_message.empty())
					{
						if((error_message = DocumentSubmissionDate_1C_Index_VarSet(bt.GetDateFinish(), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from DocumentSubmissionDate_1C_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SoW_Index_VarSet("SELECT * FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + vars.Get("bt_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						auto	psow_id = GetPSoWIDByBTIDAndCostCenterID(vars.Get("bt_id_" + to_string(i)), vars.Get("cost_center_id"), db, user);

						if((error_message = PSoW_Index_VarSet(psow_id, to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from PSoW_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_vs_SoWPeriod_vs_PSoWperiod_Index_VarSet(GetTMObject(bt.GetDateStart()), GetTMObject(bt.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_vs_SoWPeriod_vs_PSoWperiod_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if(error_message.empty()) error_message = AssignVariableValue("cost_center_markup_" + to_string(i), bt.GetMarkup().PrintPriceTag(), true);

						if((error_message = CostCenterPayment_Index_VarSet((bt.GetSumTaxable() + bt.GetSumTax() + bt.GetSumNonTaxable()) + bt.GetMarkup(), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from CostCenterPayment_Index_VarSet"); }
					}
					if(error_message.empty())
					{
						if((error_message = SubcontractorPayment_Index_VarSet(bt.GetSumTaxable() + bt.GetSumTax() + bt.GetSumNonTaxable(), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SubcontractorPayment_Index_VarSet"); }
					}

					// --- custom fields comes after Subc_index_varset due to bank details could be overloaded
					if(error_message.empty())
					{
						if((error_message = SoW_Custom_Fields(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Custom_Fields"); }
					}

					if(error_message.empty())
					{
						if((error_message = TableRowDescriptions_Index_VarSet(Get("subcontractor_position_remote_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDescriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;
					cost_center_vat = cost_center_vat + c_float(Get("cost_center_vat_" + to_string(i)));;
					cost_center_sum = cost_center_sum + c_float(Get("cost_center_price_" + to_string(i)));;

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum, subcontractors_vat)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}

				// --- cost_center calculations scoping
				if(error_message.empty())
				{
					if((error_message = CostCenterTotal_VarSet(cost_center_sum, cost_center_vat)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from CostCenterTotalVarSet"); }
				}
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::GenerateBTVariableSet_SubcToAgency() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			c_float	subcontractors_sum, subcontractors_vat;

			if(error_message.empty())
			{
				if((error_message = FillStaticDictionary()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from static dictionary"); }
			}

			if(error_message.empty())
			{
				if(bt_obj_list.size())
				{
					if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + bt_obj_list[0].GetSoWID() + "\";", true);
				}
				else
				{
					error_message = gettext("no bt found");
					MESSAGE_ERROR("", "", "bt_obj_list is empty.");
				}


				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }

			}

			if(error_message.empty())
			{
				if(bt_obj_list.size())
				{
					auto	company_id = GetValueFromDB(Get_SubcCompanyIDBySoWID_sqlquery(bt_obj_list[0].GetSoWID()), db);

					if((error_message = AgreementNumberSpelling_VarSet(GetActFullNumberByCompanyID(company_id))).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
				}
				else
				{
					error_message = gettext("no bt found");
					MESSAGE_ERROR("", "", "bt_obj_list is empty.");
				}

			}

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &bt: bt_obj_list)
				{
					++i;

					if(error_message.empty()) error_message = AssignVariableValue("bt_id_" + to_string(i), bt.GetID(), true);
					if(error_message.empty())
					{
						if((error_message = Common_Index_VarSet(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Common_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SoW_Index_VarSet("SELECT * FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + vars.Get("bt_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_Index_VarSet(GetTMObject(bt.GetDateStart()), GetTMObject(bt.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SubcontractorAddress_Index_VarSet("1")).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SubcontractorAddress_Index_VarSet"); }
					}

					// --- custom fields comes after Subc_index_varset due to bank details could be overloaded
					if(error_message.empty())
					{
						if((error_message = SoW_Custom_Fields(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Custom_Fields"); }
					}

					// --- subcontractor payment
					if(error_message.empty())
					{
						if((error_message = SubcontractorPayment_Index_VarSet(bt.GetSumTaxable() + bt.GetSumTax() + bt.GetSumNonTaxable(), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SubcontractorPayment_Index_VarSet"); }
					}

					// --- table row description
					if(error_message.empty())
					{
						if((error_message = TableRowDescriptions_Index_VarSet(Get("subcontractor_position_remote_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDescriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum, subcontractors_vat)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}
			}
		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::GenerateSoWVariableSet() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(error_message.empty())
			{
				if((error_message = FillStaticDictionary()).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from static dictionary"); }
			}

			if(error_message.empty())
			{
				if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";", true);

				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }
			}

			if(error_message.empty())
			{
				if((error_message = SoW_Index_VarSet("SELECT * FROM `contracts_sow` WHERE `id`=\"" + GetSoWID() + "\";", "")).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
			}

			// --- subcontractor company data
			if(error_message.empty())
			{
				if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_"), "")).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
			}
			if(error_message.empty())
			{
				if((error_message = SubcontractorAddress_Index_VarSet("")).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from SubcontractorAddress_Index_VarSet"); }
			}

			// --- custom fields comes after Subc_index_varset due to bank details could be overloaded
			if(error_message.empty())
			{
				if((error_message = SoW_Custom_Fields("")).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from SoW_Custom_Fields"); }
			}

		}
		else
		{
			error_message = gettext("db is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("user is not initialized");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto C_Invoicing_Vars::Get(const string &name) -> string
{
	return vars.Get(name);
}


// --- helper functions
auto C_Invoicing_Vars::GetIndexByTimecardID(string timecard_id) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start (timecard_id = " + timecard_id + ")");

	for(auto i = 1; Get("timecard_id_" + to_string(i)).length(); ++i)
	{
		if(Get("timecard_id_" + to_string(i)) == timecard_id)
		{
			result = to_string(i);
			break;
		}
	}

	MESSAGE_DEBUG("", "", "finish (index is " + result + ")");

	return result;
}

auto C_Invoicing_Vars::GetIndexByBTID(string bt_id) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start (bt_id = " + bt_id + ")");

	for(auto i = 1; Get("bt_id_" + to_string(i)).length(); ++i)
	{
		if(Get("bt_id_" + to_string(i)) == bt_id)
		{
			result = to_string(i);
			break;
		}
	}

	MESSAGE_DEBUG("", "", "finish (index is " + result + ")");

	return result;
}

auto C_Invoicing_Vars::GetActFullNumberByCompanyID(string id) -> string
{
	MESSAGE_DEBUG("", "", "start (id = " + id + ")");

	auto	act_full_number	= GetActNumberByCompanyID(id, db, user);
	auto	result = get<0>(act_full_number) + to_string(get<1>(act_full_number)) + get<2>(act_full_number);

	MESSAGE_DEBUG("", "", "finish (" + result + ")");

	return result;
}


ostream& operator<<(ostream& os, const C_Invoicing_Vars &var)
{
	os << "object C_Invoicing_Vars [empty for now]";

	return os;
}

