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

	MESSAGE_DEBUG("", "", "start (variable: " + var_name + ")");

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

	if(error_message.empty()) error_message = AssignVariableValue("Bank Identifier", gettext("Bank Identifier"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Account", gettext("Account"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Destination bank", gettext("Destination bank"), true);
	if(error_message.empty()) error_message = AssignVariableValue("TIN", gettext("TIN"), true);
	if(error_message.empty()) error_message = AssignVariableValue("KPP", gettext("KPP"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Recipient", gettext("Recipient"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Invoice", gettext("Invoice"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Act", gettext("Act"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Tax", gettext("Tax"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum expense", gettext("Sum expense"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT", gettext("VAT"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT doc", gettext("VAT doc"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT comment1", gettext("VAT comment1"), true);
	if(error_message.empty()) error_message = AssignVariableValue("agreement from", gettext("agreement from"), true);
	if(error_message.empty()) error_message = AssignVariableValue("agreement declensioned", gettext("agreement declensioned"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Technical Requirement agreement short", gettext("Technical Requirement agreement short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("from", gettext("from"), true);
	if(error_message.empty()) error_message = AssignVariableValue("up to", gettext("up to"), true);
	if(error_message.empty()) error_message = AssignVariableValue("in scope", gettext("in scope"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Provider", gettext("Provider"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Customer", gettext("Customer"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Seller", gettext("Seller"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Buyer", gettext("Buyer"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Supplier", gettext("Supplier"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Goods", gettext("Goods"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Expense", gettext("Expense"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Taxable sum", gettext("Taxable sum"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Non-taxable sum", gettext("Non-taxable sum"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Basis", gettext("Basis"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Price", gettext("Price"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Dayrate", gettext("Dayrate"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Markup", gettext("Markup"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total", gettext("Total"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total (tax free)", gettext("Total (tax free)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Excise", gettext("Excise"), true);
	if(error_message.empty()) error_message = AssignVariableValue("no excise", gettext("no excise"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Tax rate", gettext("Tax rate"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Location", gettext("Location"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Tax amount", gettext("Tax amount"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Quantity short", gettext("Quantity short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Items short", gettext("Items short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("item short", gettext("item short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("VAT short", gettext("VAT short"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total payment", gettext("Total payment"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum items", gettext("Sum items"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum (rur)", gettext("Sum (rur)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Sum (currency)", gettext("Sum (currency)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Rate exchange", gettext("Rate exchange"), true);
	if(error_message.empty()) error_message = AssignVariableValue("total amount", gettext("total amount"), true);
	if(error_message.empty()) error_message = AssignVariableValue("rur.", gettext("rur."), true);
	if(error_message.empty()) error_message = AssignVariableValue("Director", gettext("Director"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Accountant", gettext("Accountant"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Measure unit (code)", gettext("Measure unit (code)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Measure unit (name)", gettext("Measure unit (name)"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total hours on duty", gettext("Total hours on duty"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total days on duty", gettext("Total days on duty"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Total payment in reported timecard", gettext("Total payment in reported timecard"), true);
	if(error_message.empty()) error_message = AssignVariableValue("w/o", gettext("w/o"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Signature", gettext("Signature"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Initials", gettext("Initials"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Title", gettext("Title"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Date", gettext("Date"), true);
	if(error_message.empty()) error_message = AssignVariableValue("rur.", gettext("rur."), true);
	if(error_message.empty()) error_message = AssignVariableValue("kop.", gettext("kop."), true);
	if(error_message.empty()) error_message = AssignVariableValue("Remote service report over period", gettext("Remote service report over period"), true);
	if(error_message.empty()) error_message = AssignVariableValue("PSoW", gettext("PSoW"), true);
	if(error_message.empty()) error_message = AssignVariableValue("Project ID", gettext("Project ID"), true);

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
						vars.Add("cost_center_act_number", db->Get(0, "act_number"));
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
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
						vars.Add("agency_link", 				ConvertHTMLToText(db->Get(0, "link")));
						vars.Add("agency_admin_userID", 		ConvertHTMLToText(db->Get(0, "admin_userID")));
						vars.Add("agency_isConfirmed", 			ConvertHTMLToText(db->Get(0, "isConfirmed")));
						vars.Add("agency_webSite", 				ConvertHTMLToText(db->Get(0, "webSite")));
						vars.Add("agency_description", 			ConvertHTMLToText(db->Get(0, "description")));
						vars.Add("agency_isBlocked", 			ConvertHTMLToText(db->Get(0, "isBlocked")));
						vars.Add("agency_logo_folder", 			ConvertHTMLToText(db->Get(0, "logo_folder")));
						vars.Add("agency_logo_filename", 		ConvertHTMLToText(db->Get(0, "logo_filename")));
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
				auto	affected = db->Query("SELECT * FROM `company_custom_fields` WHERE `company_id`=\"" + __agency_company_id + "\";");

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
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SoW_Index_VarSet(string sql_query, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(db->Query(sql_query))
			{
				auto	sow_id = db->Get(0, "id");
				auto	sow_number = db->Get(0, "number");
				auto	sow_date = db->Get(0, "sign_date");
				auto	act_number = db->Get(0, "act_number");
				auto	subcontractor_company_id = db->Get(0, "subcontractor_company_id");
				auto	subcontractor_dayrate = db->Get(0, "day_rate");

				if(sow_id.length())
				{
					if((error_message = AssignVariableValue("sow_id_" + index, sow_id, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("id is empty");
					MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) id is empty");
				}

				if(subcontractor_dayrate.length())
				{
					if((error_message = AssignVariableValue("subcontractor_dayrate_" + index, subcontractor_dayrate, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("number is empty");
					MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) dayrate is empty");
				}

				if(sow_number.length())
				{
					if((error_message = AssignVariableValue("sow_number_" + index, sow_number, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("number is empty");
					MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) number is empty");
				}

				if(sow_date.length())
				{
					if((error_message = AssignVariableValue("sow_sign_date_" + index, sow_date, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("date is empty");
					MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) date is empty");
				}

				if(act_number.length())
				{
					if((error_message = AssignVariableValue("subcontractor_act_number_" + index, act_number, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("act number is empty");
					MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) act number is empty");
				}

				if(subcontractor_company_id.length())
				{
					if((error_message = AssignVariableValue("subcontractor_company_id_" + index, subcontractor_company_id, true)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail to assign variable"); }
				}
				else
				{
					error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("subc company.id is empty");
					MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) subc company.id is empty");
				}

				// --- scoping
				{
					C_Date_Spelling	date_obj;

					date_obj.SetTMObj(GetTMObject(vars.Get("sow_sign_date_" + index)));

					if(error_message.empty()) error_message = AssignVariableValue("sow_agreement_" + index, "№" + vars.Get("sow_number_" + index) + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
				}
			}
			else
			{
				error_message = gettext("SQL syntax issue");
				MESSAGE_ERROR("", "", "SQL syntax issue");
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
								error_message = gettext("SQL syntax issue");
								MESSAGE_ERROR("", "", "SQL syntax issue");
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
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "SQL syntax issue");
						}
					}
				}
				else
				{
					error_message = gettext("Agreement") + " "s + Get("sow_agreement_" + index) + " " + gettext("missed occupancy title");
					MESSAGE_ERROR("", "", error_message);
				};
			}

			// --- subcontractor sow custom fields
			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + vars.Get("sow_id_" + index) + "\";");

				for(auto j = 0; j < affected; ++j)
				{
					if(error_message.empty()) error_message = AssignVariableValue(db->Get(j, "var_name") + "_" + index, db->Get(j, "value"), true);
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::CostCenterPayment_Index_VarSet(c_float cost_center_price, string index) -> string
{
	auto		error_message = ""s;
	c_float		cost_center_vat;
	c_float		cost_center_total_payment;

	MESSAGE_DEBUG("", "", "start");

	if(Get("agency_vat") == "Y") cost_center_vat = cost_center_price * c_float(VAT_PERCENTAGE) / c_float(100);
	cost_center_total_payment = cost_center_price + cost_center_vat;

	if(error_message.empty()) error_message = AssignVariableValue("cost_center_price_" + index, string(cost_center_price), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_vat_" + index, string(cost_center_vat), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_total_" + index, string(cost_center_total_payment), true);


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SubcontractorPayment_Index_VarSet(c_float subcontractor_price, string index) -> string
{
	auto		error_message = ""s;
	c_float		subcontractor_vat;
	c_float		subcontractor_total_payment;

	MESSAGE_DEBUG("", "", "start (subcontractor_price: " + string(subcontractor_price) + ")");

	if(Get("subcontractor_company_vat_" + index) == "Y") subcontractor_vat = subcontractor_price * c_float(VAT_PERCENTAGE) / c_float(100);
	subcontractor_total_payment = subcontractor_price + subcontractor_vat;

	if(error_message.empty()) error_message = AssignVariableValue("timecard_price_" + index, subcontractor_price        .PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("timecard_vat_"   + index, subcontractor_vat          .PrintPriceTag(), true);
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
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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

				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_short_name_" + index, db->Get(0, "name"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_name_" + index, db->Get(0, "name"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_bank_id_" + index, db->Get(0, "bank_id"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_tin_" + index, db->Get(0, "tin"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_account_" + index, db->Get(0, "account"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_ogrn_" + index, db->Get(0, "ogrn"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_kpp_" + index, db->Get(0, "kpp"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_" + index, subcontractor_vat, true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_boolean_" + index, subcontractor_vat == "Y" ? "true" : "false", true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_spelling_" + index, subcontractor_vat == "N" ? gettext("no VAT") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%", true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_spelling_short_" + index, subcontractor_vat == "N" ? gettext("no VAT short") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%", true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_spelling_1C_" + index, subcontractor_vat == "N" ? gettext("noVAT") : gettext("VAT") + to_string(VAT_PERCENTAGE), true);

				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_mailing_geo_zip_id_" + index, db->Get(0, "mailing_geo_zip_id"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_legal_geo_zip_id_" + index, db->Get(0, "legal_geo_zip_id"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_mailing_address_" + index, db->Get(0, "mailing_address"), true);
				if(error_message.empty()) error_message = AssignVariableValue("subcontractor_legal_address_" + index, db->Get(0, "legal_address"), true);
			}
			else
			{
				error_message = gettext("SQL syntax issue");
				MESSAGE_ERROR("", "", error_message);
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
					error_message = gettext("SQL syntax issue");
					MESSAGE_ERROR("", "", error_message);
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::TableRowDecsriptions_Index_VarSet(string local_remote_service_description, string index) -> string
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
						Get("psow_contract_" + index + "_Department_spelling") +
						"."
						" " + Get("agency_vat_spelling") + ".";

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

auto	C_Invoicing_Vars::DocumentSubmissionDate_1C_Index_VarSet(string date_inside_month, string index) -> string
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	date_inside_month_obj = GetTMObject(date_inside_month);

	// --- build last day of given month
	date_inside_month_obj.tm_mon += 1;
	date_inside_month_obj.tm_mday = -1;

	if(error_message.empty()) error_message = AssignVariableValue("LastDayOfMonth_Timestamp_1CFormat_" + index, GetSpellingFormattedDate(date_inside_month, "%FT%T"), true);
	if(error_message.empty()) error_message = AssignVariableValue("LastDayOfMonth_Date_1CFormat_" + index, GetSpellingFormattedDate(date_inside_month, "%F"), true);
	if(error_message.empty()) error_message = AssignVariableValue("LastDayOfMonth_Timestamp_1CFormat", Get("LastDayOfMonth_Timestamp_1CFormat_" + index), true);


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::SubcontractorsTotal_VarSet(string sum, string tax, string total) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	// --- not used anywhere
	if(error_message.empty()) error_message = AssignVariableValue("subcontractors_sum_amount", sum, true);
	if(error_message.empty()) error_message = AssignVariableValue("subcontractors_vat_amount", tax, true);
	if(error_message.empty()) error_message = AssignVariableValue("subcontractors_total_payment", total, true);

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::CostCenterTotal_VarSet(c_float cost_center_sum) -> string
{
	auto	error_message = ""s;
	c_float	cost_center_vat(0);

	MESSAGE_DEBUG("", "", "start");

	if(Get("agency_vat") == "Y")
		cost_center_vat = cost_center_sum * c_float(VAT_PERCENTAGE) / c_float(100);

	if(error_message.empty()) error_message = AssignVariableValue("cost_center_sum_amount", cost_center_sum.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_vat_amount", cost_center_vat.PrintPriceTag(), true);
	if(error_message.empty()) error_message = AssignVariableValue("cost_center_total_payment", (cost_center_sum + cost_center_vat).PrintPriceTag(), true);

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
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto	C_Invoicing_Vars::Workperiod_vs_PSoWperiod_Index_VarSet(struct tm workperiod_start, struct tm workperiod_finish, string index) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	// --- define dates resided inside PSoW
	auto				psow_period_start		= GetTMObject(Get("psow_contract_start_date_" + index));
	auto				psow_period_finish		= GetTMObject(Get("psow_contract_end_date_" + index));
	auto				psow_sign_date			= GetTMObject(Get("psow_contract_sign_date_" + index));

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
	else
	{
		error_message = Workperiod_Index_VarSet(
													psow_period_start  > workperiod_start  ? psow_period_start  : workperiod_start,
													psow_period_finish < workperiod_finish ? psow_period_finish : workperiod_finish,
													index
												);
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
					date_obj.SetTMObj(GetTMObject(timecard_obj_list[0].GetDateFinish()));
				}
				else if(bt_obj_list.size())
				{
					date_obj.SetTMObj(GetTMObject(bt_obj_list[0].GetDateFinish()));
				}
				else
				{
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
					if((error_message = AssignVariableValue("comment_1C", gettext("Local service delivery") + " "s + date_obj.GetFormatted("%B %Y"), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
			}

		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
			c_float	cost_center_sum;


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
				if((error_message = AgreementNumberSpelling_VarSet(Get("cost_center_act_number"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
			}

			if(error_message.empty())
			{
				if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";", true);

				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }
			}

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &timecard: timecard_obj_list)
				{
					c_float		timecard_days = timecard.GetTotalHours() / c_float(8.0);

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
						if((error_message = SoW_Index_VarSet("SELECT `id`,`number`,`sign_date`,`act_number`,`subcontractor_company_id`,`day_rate` FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + vars.Get("timecard_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
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
						if((error_message = Workperiod_vs_PSoWperiod_Index_VarSet(GetTMObject(timecard.GetDateStart()), GetTMObject(timecard.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_vs_PSoWperiod_Index_VarSet"); }
					}

					// --- subcontractor company data
					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
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
						if((error_message = TableRowDecsriptions_Index_VarSet(Get("subcontractor_position_local_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDecsriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;
					cost_center_sum = cost_center_sum + c_float(Get("cost_center_price_" + to_string(i)));

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum.PrintPriceTag(), subcontractors_vat.PrintPriceTag(), (subcontractors_sum + subcontractors_vat).PrintPriceTag())).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}

				// --- cost_center calculations scoping
				if(error_message.empty())
				{
					if((error_message = CostCenterTotal_VarSet(cost_center_sum)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from CostCenterTotalVarSet"); }
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
					if(error_message.empty()) error_message = AssignVariableFromDB("sow_act_number", "SELECT `act_number` FROM `contracts_sow` WHERE `id`=\"" + timecard_obj_list[0].GetSoWID() + "\";", true);
				}
				else
				{
					error_message = gettext("no timecard found");
					MESSAGE_ERROR("", "", "timecard_obj_list is empty.");
				}

				if((error_message = AgreementNumberSpelling_VarSet(Get("sow_act_number"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
			}

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &timecard: timecard_obj_list)
				{
					c_float		timecard_days = timecard.GetTotalHours() / c_float(8.0);

					++i;

					if(error_message.empty()) error_message = AssignVariableValue("timecard_id_" + to_string(i), timecard.GetID(), true);
					if(error_message.empty())
					{
						if((error_message = Common_Index_VarSet(to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Common_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = SoW_Index_VarSet("SELECT `id`,`number`,`sign_date`,`act_number`,`subcontractor_company_id`,`day_rate` FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + vars.Get("timecard_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_Index_VarSet(GetTMObject(timecard.GetDateStart()), GetTMObject(timecard.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_Index_VarSet"); }
					}

					// --- subcontractor company data
					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}
					if(error_message.empty())
					{
						if((error_message = SubcontractorAddress_Index_VarSet("1")).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
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
						if((error_message = TableRowDecsriptions_Index_VarSet(Get("subcontractor_position_local_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDecsriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum.PrintPriceTag(), subcontractors_vat.PrintPriceTag(), (subcontractors_sum + subcontractors_vat).PrintPriceTag())).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
			c_float	cost_center_sum;

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
				if((error_message = AgreementNumberSpelling_VarSet(Get("cost_center_act_number"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgreementNumberSpelling_VarSet"); }
			}

			if(error_message.empty())
			{
				if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id", "SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";", true);

				if((error_message = Agency_VarSet(Get("agency_company_id"))).empty()) {}
				else { MESSAGE_ERROR("", "", "fail returned from AgencyVarSet"); }
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
						if((error_message = SoW_Index_VarSet("SELECT `id`,`number`,`sign_date`,`act_number`,`subcontractor_company_id`,`day_rate` FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + vars.Get("bt_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						auto	psow_id = GetPSoWIDByBTIDAndCostCenterID(vars.Get("bt_id_" + to_string(i)), vars.Get("cost_center_id"), db, user);

						if((error_message = PSoW_Index_VarSet(psow_id, to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from PSoW_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_vs_PSoWperiod_Index_VarSet(GetTMObject(bt.GetDateStart()), GetTMObject(bt.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_vs_PSoWperiod_Index_VarSet"); }
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

					if(error_message.empty())
					{
						if((error_message = TableRowDecsriptions_Index_VarSet(Get("subcontractor_position_remote_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDecsriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;
					cost_center_sum = cost_center_sum + c_float(Get("cost_center_price_" + to_string(i)));;

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum.PrintPriceTag(), subcontractors_vat.PrintPriceTag(), (subcontractors_sum + subcontractors_vat).PrintPriceTag())).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}

				// --- cost_center calculations scoping
				if(error_message.empty())
				{
					if((error_message = CostCenterTotal_VarSet(cost_center_sum)).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from CostCenterTotalVarSet"); }
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
					if(error_message.empty()) error_message = AssignVariableFromDB("sow_act_number", "SELECT `act_number` FROM `contracts_sow` WHERE `id`=\"" + bt_obj_list[0].GetSoWID() + "\";", true);
				}
				else
				{
					error_message = gettext("no bt found");
					MESSAGE_ERROR("", "", "bt_obj_list is empty.");
				}

				if((error_message = AgreementNumberSpelling_VarSet(Get("sow_act_number"))).empty()) {}
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
						if((error_message = SoW_Index_VarSet("SELECT `id`,`number`,`sign_date`,`act_number`,`subcontractor_company_id`,`day_rate` FROM `contracts_sow` WHERE `id`=("
																"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + vars.Get("bt_id_" + to_string(i)) + "\""
															");", to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from SoW_Index_VarSet"); }
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						if((error_message = Workperiod_Index_VarSet(GetTMObject(bt.GetDateStart()), GetTMObject(bt.GetDateFinish()), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Workperiod_Index_VarSet"); }
					}

					if(error_message.empty())
					{
						if((error_message = Subcontractor_Index_VarSet(Get("subcontractor_company_id_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
					}
					if(error_message.empty())
					{
						if((error_message = SubcontractorAddress_Index_VarSet("1")).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
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
						if((error_message = TableRowDecsriptions_Index_VarSet(Get("subcontractor_position_remote_service_description_" + to_string(i)), to_string(i))).empty()) {}
						else { MESSAGE_ERROR("", "", "fail returned from TableRowDecsriptions_Index_VarSet"); }
					}

					subcontractors_vat = subcontractors_vat + c_float(Get("timecard_vat_" + to_string(i)));
					subcontractors_sum = subcontractors_sum + c_float(Get("timecard_price_" + to_string(i)));;

					// --- loop closure
					if(error_message.length()) break;
				}

				// --- timecard calculations scoping
				if(error_message.empty())
				{
					if((error_message = SubcontractorsTotal_VarSet(subcontractors_sum.PrintPriceTag(), subcontractors_vat.PrintPriceTag(), (subcontractors_sum + subcontractors_vat).PrintPriceTag())).empty()) {}
					else { MESSAGE_ERROR("", "", "fail returned from SubcontractorsTotalVarSet"); }
				}
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
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
				if((error_message = SoW_Index_VarSet("SELECT `id`,`number`,`sign_date`,`act_number`,`subcontractor_company_id`,`day_rate` FROM `contracts_sow` WHERE `id`=\"" + GetSoWID() + "\";", "")).empty()) {}
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
				else { MESSAGE_ERROR("", "", "fail returned from Subcontractor_Index_VarSet"); }
			}

		}
		else
		{
			MESSAGE_ERROR("", "", "db is not initialized");
			error_message = gettext("db is not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is not initialized");
		error_message = gettext("user is not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return	error_message;
}

auto C_Invoicing_Vars::Get(const string &name) -> string
{
	return vars.Get(name);
}


// --- helper funcitons
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


ostream& operator<<(ostream& os, const C_Invoicing_Vars &var)
{
	os << "object C_Invoicing_Vars [empty for now]";

	return os;
}

