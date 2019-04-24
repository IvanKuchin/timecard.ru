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

auto	C_Invoicing_Vars::GenerateServiceVariableSet() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			c_float	timecards_sum, timecards_vat;
			c_float	cost_center_sum;

			if(error_message.empty()) error_message = AssignVariableValue("Bank Identifier", gettext("Bank Identifier"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Account", gettext("Account"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Destination bank", gettext("Destination bank"), true);
			if(error_message.empty()) error_message = AssignVariableValue("TIN", gettext("TIN"), true);
			if(error_message.empty()) error_message = AssignVariableValue("KPP", gettext("KPP"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Recipient", gettext("Recipient"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Invoice", gettext("Invoice"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Act", gettext("Act"), true);
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
			if(error_message.empty()) error_message = AssignVariableValue("Basis", gettext("Basis"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Price", gettext("Price"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Total", gettext("Total"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Total (tax free)", gettext("Total (tax free)"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Excise", gettext("Excise"), true);
			if(error_message.empty()) error_message = AssignVariableValue("no excise", gettext("no excise"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Tax rate", gettext("Tax rate"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Tax amount", gettext("Tax amount"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Quantity short", gettext("Quantity short"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Items short", gettext("Items short"), true);
			if(error_message.empty()) error_message = AssignVariableValue("item short", gettext("item short"), true);
			if(error_message.empty()) error_message = AssignVariableValue("VAT short", gettext("VAT short"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Total payment", gettext("Total payment"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Sum items", gettext("Sum items"), true);
			if(error_message.empty()) error_message = AssignVariableValue("total amount", gettext("total amount"), true);
			if(error_message.empty()) error_message = AssignVariableValue("rur.", gettext("rur."), true);
			if(error_message.empty()) error_message = AssignVariableValue("Director", gettext("Director"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Accountant", gettext("Accountant"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Measure unit (code)", gettext("Measure unit (code)"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Measure unit (name)", gettext("Measure unit (name)"), true);

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
				else
				{
					time (&rawtime);
					timeinfo = localtime (&rawtime);

					date_obj.SetTMObj(*timeinfo);
				}

				// strftime(buffer, sizeof(buffer), "%B %Y", timeinfo);


				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("invoice_agreement", "№" + Get("cost_center_act_number") + " " + gettext("agreement from") + " " + date_obj.Spell(), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
				if(error_message.empty())
				{
					if((error_message = AssignVariableValue("comment_1C", gettext("Local service delivery") + " "s + date_obj.GetFormatted("%B %Y"), true)).length())
						{ MESSAGE_ERROR("", "", "fail to assign variable value"); }
				}
			}

            if(error_message.empty()) error_message = AssignVariableFromDB("cost_center_company_id"                  , "SELECT `company_id` FROM `cost_centers` WHERE `id`=\"" + Get("cost_center_id") + "\";", true);
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

			if(error_message.empty()) error_message = AssignVariableFromDB("agency_company_id"					, "SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";", true);
			if(error_message.empty())
			{
				if(vars.Get("agency_company_id").length())
				{
					MESSAGE_DEBUG("", "", "build agency company id");

					if(db->Query("SELECT * FROM `company` WHERE `id`=\"" + vars.Get("agency_company_id") + "\";"))
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

			// --- agency company custom fields 
			if(error_message.empty())
			{
				auto	affected = db->Query("SELECT * FROM `company_custom_fields` WHERE `company_id`=\"" + vars.Get("agency_company_id") + "\";");

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

			// --- define timecards variables
			if(error_message.empty())
			{
				auto	i = 0;
				for(auto &timecard: timecard_obj_list)
				{
					c_float		timecard_days;
					c_float		timecard_price;
					c_float		timecard_vat;
					c_float		cost_center_price;
					c_float		subcontractor_dayrate(0);


					++i;

					// --- subcontractor VAT
					if(error_message.empty())
					{
						auto	subcontractor_vat = cache.Get(
											"SELECT `vat` FROM `company` WHERE `id`=("
												"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=("
													"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard.GetID() + "\""
												")"
											");", db, user, [](string sql_query, CMysql *db, CUser *)
																{
																	return (db->Query(sql_query) ? db->Get(0, 0) : ""s);
																});
						if(subcontractor_vat.length())
						{
							error_message = AssignVariableValue("timecard_company_vat_" + to_string(i), subcontractor_vat, true);
							error_message = AssignVariableValue("timecard_company_vat_boolean_" + to_string(i), subcontractor_vat == "Y" ? "true" : "false", true);
							error_message = AssignVariableValue("timecard_company_vat_spelling_" + to_string(i), subcontractor_vat == "N" ? gettext("no VAT") : gettext("VAT") + " "s + to_string(VAT_PERCENTAGE) + "%", true);
							error_message = AssignVariableValue("timecard_company_vat_spelling_1C_" + to_string(i), subcontractor_vat == "N" ? gettext("noVAT") : gettext("VAT") + to_string(VAT_PERCENTAGE), true);
						}
						else
						{
							error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("vat") + " " + gettext("is empty");
							MESSAGE_ERROR("", "", error_message);
						}

					}

					timecard_days = timecard.GetTotalHours() / c_float(8.0);

					if(error_message.empty()) error_message = AssignVariableValue("timecard_id_" + to_string(i), timecard.GetID(), true);
					if(error_message.empty()) error_message = AssignVariableValue("timecard_index_" + to_string(i), to_string(i), true);
					if(error_message.empty()) error_message = AssignVariableValue("timecard_quantity_" + to_string(i), "1", true);
					if(error_message.empty()) error_message = AssignVariableValue("timecard_item_" + to_string(i), Get("item short"), true);
					if(error_message.empty()) error_message = AssignVariableValue("vat_rate_" + to_string(i), to_string(int(VAT_PERCENTAGE)), true);
					if(error_message.empty()) error_message = AssignVariableValue("LastDayOfTimecard_Timestamp_1CFormat_" + to_string(i), GetSpellingFormattedDate(timecard.GetDateFinish(), "%FT%T"), true);
					if(error_message.empty()) error_message = AssignVariableValue("LastDayOfTimecard_Date_1CFormat_" + to_string(i), GetSpellingFormattedDate(timecard.GetDateFinish(), "%F"), true);
					if(error_message.empty()) error_message = AssignVariableValue("LastDayOfTimecard_Timestamp_1CFormat", GetSpellingFormattedDate(timecard.GetDateFinish(), "%FT%T"), true);

					if(error_message.empty())
					{
						if(db->Query(	
										"SELECT `id`,`number`,`sign_date`,`act_number`,`subcontractor_company_id`,`day_rate` FROM `contracts_sow` WHERE `id`=("
											"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + vars.Get("timecard_id_" + to_string(i)) + "\""
										");"))
						{
							auto	sow_id = db->Get(0, "id");
							auto	sow_number = db->Get(0, "number");
							auto	sow_date = db->Get(0, "sign_date");
							auto	act_number = db->Get(0, "act_number");
							auto	subcontractor_company_id = db->Get(0, "subcontractor_company_id");

							subcontractor_dayrate.Set(db->Get(0, "day_rate"));

							if(sow_id.length())
							{
								if((error_message = AssignVariableValue("sow_id_" + to_string(i), sow_id, true)).empty()) {}
								else { MESSAGE_ERROR("", "", "fail to assign variable"); }
							}
							else
							{
								error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("id is empty");
								MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) id is empty");
							}

							if(subcontractor_dayrate.Get())
							{
								if((error_message = AssignVariableValue("timecard_subcontractor_dayrate_" + to_string(i), subcontractor_dayrate, true)).empty()) {}
								else { MESSAGE_ERROR("", "", "fail to assign variable"); }
							}
							else
							{
								error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("number is empty");
								MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) dayrate is empty");
							}

							if(sow_number.length())
							{
								if((error_message = AssignVariableValue("timecard_sow_number_" + to_string(i), sow_number, true)).empty()) {}
								else { MESSAGE_ERROR("", "", "fail to assign variable"); }
							}
							else
							{
								error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("number is empty");
								MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) number is empty");
							}

							if(sow_date.length())
							{
								if((error_message = AssignVariableValue("timecard_sow_sign_date_" + to_string(i), sow_date, true)).empty()) {}
								else { MESSAGE_ERROR("", "", "fail to assign variable"); }
							}
							else
							{
								error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("date is empty");
								MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) date is empty");
							}

							if(act_number.length())
							{
								if((error_message = AssignVariableValue("subcontractor_act_number_" + to_string(i), act_number, true)).empty()) {}
								else { MESSAGE_ERROR("", "", "fail to assign variable"); }
							}
							else
							{
								error_message = gettext("Agreement") + " "s + db->Get(0, "number") + " " + gettext("agreement from") + " " + db->Get(0, "sign_date") + " " + gettext("act number is empty");
								MESSAGE_ERROR("", "", "Agreement( " + db->Get(0, "number") + " " + gettext("agreement from") + db->Get(0, "sign_date") + " ) act number is empty");
							}

							if(subcontractor_company_id.length())
							{
								if((error_message = AssignVariableValue("subcontractor_company_id_" + to_string(i), subcontractor_company_id, true)).empty()) {}
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

								date_obj.SetTMObj(GetTMObject(vars.Get("timecard_sow_sign_date_" + to_string(i))));

								if(error_message.empty()) error_message = AssignVariableValue("sow_agreement_" + to_string(i), "№" + vars.Get("timecard_sow_number_" + to_string(i)) + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
							}
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", "SQL syntax issue");
						}
					}

					if(error_message.empty())
						error_message = AssignVariableValue("timecard_sow_agreement_" + to_string(i), Get("timecard_sow_number_" + to_string(i)) + " " + gettext("agreement from") + " " + Get("timecard_sow_sign_date_" + to_string(i)), true);

					if(error_message.empty())
					{
						auto	company_position_id = cache.Get(	
																	"SELECT `company_position_id` FROM `contracts_sow` WHERE `id`=("
																		"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard.GetID() + "\""
																	");", db, user, [](string sql_query, CMysql *db, CUser *)
																					{
																						return (db->Query(sql_query) ? db->Get(0, 0) : ""s);
																					});

						if(company_position_id.length())
						{
							auto	service_description = cache.Get(	"SELECT `local_service_description` FROM `company_position` WHERE `id`=\"" + company_position_id + "\";", db, user,
																					[](string sql_query, CMysql *db, CUser *)
																					{
																						return (db->Query(sql_query) ? db->Get(0, 0) : ""s);
																					});

							if(service_description.length())
							{
								error_message = AssignVariableValue("timecard_company_position_id_" + to_string(i), company_position_id, true);
								error_message = AssignVariableValue("timecard_local_service_description_" + to_string(i), service_description, true);
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
							if(db->Query(	
											"SELECT `number`,`sign_date` FROM `contracts_sow` WHERE `id`=("
												"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + vars.Get("timecard_index_" + to_string(i)) + "\""
											");"))
							{
								error_message = gettext("Agreement") + " "s + Get("timecard_sow_agreement_" + to_string(i)) + " " + gettext("missed occupancy title");
								MESSAGE_ERROR("", "", error_message);
							}
							else
							{
								error_message = gettext("SQL syntax issue");
								MESSAGE_ERROR("", "", "SQL syntax issue");
							}
						};
					}

					// --- get psow_id
					if(error_message.empty())
					{
						auto	psow_id = GetPSoWIDByTimecardIDAndCostCenterID(vars.Get("timecard_id_" + to_string(i)), vars.Get("cost_center_id"), db, user);

						if(psow_id.length())
						{
							if(error_message.empty()) error_message = AssignVariableValue("timecard_psow_id_" + to_string(i), psow_id, true);

							if(db->Query("SELECT * FROM `contracts_psow` WHERE `id`=\"" + vars.Get("timecard_psow_id_" + to_string(i)) + "\";"))
							{
								C_Date_Spelling		date_obj;

								if(error_message.empty())
								{
									if(db->Get(0, "number").length())
									{
										error_message = AssignVariableValue("timecard_psow_contract_number_" + to_string(i), db->Get(0, "number"), true);
									}
									else
									{
										error_message = "PSOW "s + gettext("number") + " " + gettext("is empty") + "(" + Get("timecard_sow_agreement_" + to_string(i)) + ")";
										MESSAGE_ERROR("", "", error_message);
									}
								}
								if(error_message.empty())
								{
									if(db->Get(0, "sign_date").length())
									{
										error_message = AssignVariableValue("timecard_psow_contract_sign_date_" + to_string(i), db->Get(0, "sign_date"), true);
									}
									else
									{
										error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("sign date") + " " + gettext("is empty");
										MESSAGE_ERROR("", "", error_message);
									}
								}
								if(error_message.empty())
								{
									if(db->Get(0, "start_date").length())
									{
										error_message = AssignVariableValue("timecard_psow_contract_start_date_" + to_string(i), db->Get(0, "start_date"), true);
									}
									else
									{
										error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("start date") + " " + gettext("is empty");
										MESSAGE_ERROR("", "", error_message);
									}
								}
								if(error_message.empty())
								{
									if(db->Get(0, "end_date").length())
									{
										error_message = AssignVariableValue("timecard_psow_contract_end_date_" + to_string(i), db->Get(0, "end_date"), true);
									}
									else
									{
										error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("end date") + " " + gettext("is empty");
										MESSAGE_ERROR("", "", error_message);
									}
								}
								if(error_message.empty())
								{
									if(db->Get(0, "day_rate").length())
									{
										c_float		cost_center_day_rate(db->Get(0, "day_rate"));
										c_float		cost_center_vat;
										c_float		cost_center_total_payment;


										cost_center_price = cost_center_day_rate * timecard_days;
										if(Get("agency_vat") == "Y") cost_center_vat = cost_center_price * c_float(VAT_PERCENTAGE) / c_float(100);
										cost_center_total_payment = cost_center_price + cost_center_vat;



										if(error_message.empty()) error_message = AssignVariableValue("cost_center_price_" + to_string(i), string(cost_center_price), true);
										if(error_message.empty()) error_message = AssignVariableValue("cost_center_vat_" + to_string(i), string(cost_center_vat), true);
										if(error_message.empty()) error_message = AssignVariableValue("cost_center_total_" + to_string(i), string(cost_center_total_payment), true);

										if(error_message.empty()) error_message = AssignVariableValue("contract_psow_day_rate_" + to_string(i), db->Get(0, "day_rate"), true);
									}
									else
									{
										error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("day rate") + " " + gettext("is empty");
										MESSAGE_ERROR("", "", error_message);
									}
								}

								date_obj.SetTMObj(GetTMObject(vars.Get("timecard_psow_contract_sign_date_" + to_string(i))));

								if(error_message.empty()) error_message = AssignVariableValue("psow_agreement_" + to_string(i), "№" + vars.Get("timecard_psow_contract_number_" + to_string(i)) + " " + gettext("agreement from") + " " + date_obj.Spell(), true);
							}
							else
							{

							}
						}
						else
						{
							error_message = gettext("Agreement") + " "s + Get("timecard_sow_agreement_" + to_string(i)) + " " + gettext("missed PSoW number");
							MESSAGE_ERROR("", "", error_message);
						}
					}

					// --- get report start date and finish date
					if(error_message.empty())
					{
						C_Date_Spelling		report_period_start;
						C_Date_Spelling		report_period_finish;
						auto				timecard_period_start = GetTMObject(timecard.GetDateStart());
						auto				timecard_period_finish = GetTMObject(timecard.GetDateFinish());
						auto				psow_period_start = GetTMObject(Get("timecard_psow_contract_start_date_" + to_string(i)));
						auto				psow_period_finish = GetTMObject(Get("timecard_psow_contract_end_date_" + to_string(i)));
						auto				psow_sign_date = GetTMObject(Get("timecard_psow_contract_sign_date_" + to_string(i)));

						if(psow_period_start <= GetTMObject("2000-01-01"))
						{
							error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("start date") + " " + gettext("must be in 20-th century");
							MESSAGE_ERROR("", "", error_message);
						}
						else if(psow_period_finish <= GetTMObject("2000-01-01"))
						{
							error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("end date") + " " + gettext("must be in 20-th century");
							MESSAGE_ERROR("", "", error_message);
						}
						else if(psow_sign_date <= GetTMObject("2000-01-01"))
						{
							error_message = "PSOW("s + Get("timecard_psow_contract_number_" + to_string(i)) + ") " + gettext("sign date") + " " + gettext("must be in 20-th century");
							MESSAGE_ERROR("", "", error_message);
						}
						else
						{
							report_period_start	.SetTMObj(psow_period_start  > timecard_period_start  ? psow_period_start  : timecard_period_start);
							report_period_finish.SetTMObj(psow_period_finish < timecard_period_finish ? psow_period_finish : timecard_period_finish);

							error_message = AssignVariableValue("timecard_date_start" + to_string(i), report_period_start.GetFormatted("%d.%m.%Y"), true);
							error_message = AssignVariableValue("timecard_date_finish" + to_string(i), report_period_finish.GetFormatted("%d.%m.%Y"), true);
						}

					}

					// --- cost center custom fields 
					if(error_message.empty())
					{
						auto	affected = db->Query("SELECT * FROM `contract_psow_custom_fields` WHERE `contract_psow_id`=\"" + vars.Get("timecard_psow_id_" + to_string(i)) + "\";");

						for(auto j = 0; j < affected; ++j)
						{
							if(error_message.empty()) error_message = AssignVariableValue("timecard_contract_psow_" + to_string(i) + "_" + db->Get(j, "var_name"), db->Get(j, "value"), true);
						}
					}

					// --- subcontractor sow custom fields
					if(error_message.empty())
					{
						auto	affected = db->Query("SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + vars.Get("sow_id_" + to_string(i)) + "\";");

						for(auto j = 0; j < affected; ++j)
						{
							if(error_message.empty()) error_message = AssignVariableValue(db->Get(j, "var_name") + "_" + to_string(i), db->Get(j, "value"), true);
						}
					}

					// --- subcontractor company data
					if(error_message.empty())
					{
						auto	affected = db->Query("SELECT * FROM `company` WHERE `id`=\"" + vars.Get("subcontractor_company_id_" + to_string(i)) + "\";");

						if(affected)
						{
							c_float		timecard_total_payment;

							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_short_name_" + to_string(i), db->Get(0, "name"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_name_" + to_string(i), db->Get(0, "name"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_bank_id_" + to_string(i), db->Get(0, "bank_id"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_tin_" + to_string(i), db->Get(0, "tin"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_account_" + to_string(i), db->Get(0, "account"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_ogrn_" + to_string(i), db->Get(0, "ogrn"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_kpp_" + to_string(i), db->Get(0, "kpp"), true);
							if(error_message.empty()) error_message = AssignVariableValue("subcontractor_company_vat_" + to_string(i), db->Get(0, "vat"), true);

							timecard_price = subcontractor_dayrate * timecard_days;
							if(Get("subcontractor_company_vat_" + to_string(i)) == "Y") timecard_vat = timecard_price * c_float(VAT_PERCENTAGE) / c_float(100);
							timecard_total_payment = timecard_price + timecard_vat;

							if(error_message.empty()) error_message = AssignVariableValue("timecard_price_" + to_string(i), string(timecard_price), true);
							if(error_message.empty()) error_message = AssignVariableValue("timecard_vat_" + to_string(i), string(timecard_vat), true);
							if(error_message.empty()) error_message = AssignVariableValue("timecard_total_" + to_string(i), string(timecard_total_payment), true);

						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", error_message);
						}
					}



					if(error_message.empty())
					{
						auto	affected = db->Query("SELECT * FROM `banks` WHERE `id`=\"" + vars.Get("subcontractor_company_bank_id_" + to_string(i)) + "\";");

						if(affected)
						{
							error_message = AssignVariableValue("subcontractor_bank_title_" + to_string(i), db->Get(0, "title"), true);
							error_message = AssignVariableValue("subcontractor_bank_bik_" + to_string(i), db->Get(0, "bik"), true);
							error_message = AssignVariableValue("subcontractor_bank_account_" + to_string(i), db->Get(0, "account"), true);
						}
						else
						{
							error_message = gettext("SQL syntax issue");
							MESSAGE_ERROR("", "", error_message);
						}
					}

					if(error_message.empty())
					{						
						error_message = AssignVariableValue("timecard_contract_psow_" + to_string(i) + "_Department_spelling", 
											Get("timecard_contract_psow_" + to_string(i) + "_Department").length()
											? " ("s + gettext("department") + " " + Get("timecard_contract_psow_" + to_string(i) + "_Department") + ")"
											: "", true);
						if(error_message.length())
						{ MESSAGE_ERROR("", "", "fail to assign variable"); }
					}

					if(error_message.empty())
					{
						error_message = AssignVariableValue("cost_center_table_row_description_" + to_string(i),
															Get("timecard_local_service_description_" + to_string(i)) + " " + 
															Get("from") + " " + Get("timecard_date_start" + to_string(i)) + " " + Get("up to") + " " + Get("timecard_date_finish" + to_string(i)) + " " + 
															Get("in scope") + " " + Get("Technical Requirement agreement short") + " " + Get("psow_agreement_" + to_string(i)) +
															(Get("timecard_contract_psow_" + to_string(i) + "_Department_spelling").length() ? " "s + Get("timecard_contract_psow_" + to_string(i) + "_Department_spelling") : ""s) +
															"."
															" " + Get("agency_vat_spelling") + "."
															,true);

						if(error_message.length())
						{ MESSAGE_ERROR("", "", "fail to assign variable"); }
					}

					if(error_message.empty())
					{
						error_message = AssignVariableValue("subcontractor_table_row_description_" + to_string(i),
															Get("timecard_local_service_description_" + to_string(i)) + " " + 
															Get("from") + " " + Get("timecard_date_start" + to_string(i)) + " " + Get("up to") + " " + Get("timecard_date_finish" + to_string(i)) + " " + 
															Get("in scope") + " " + Get("agreement declensioned") + " " + Get("sow_agreement_" + to_string(i)) + "."
															" " + Get("timecard_company_vat_spelling_" + to_string(i)) + "."
															,true);

						if(error_message.length())
						{ MESSAGE_ERROR("", "", "fail to assign variable"); }
					}

					timecards_vat = timecards_vat + timecard_vat;
					timecards_sum = timecards_sum + timecard_price;
					cost_center_sum = cost_center_sum + cost_center_price;

					// --- loop closure
					if(error_message.length()) break;
				}

// CONTINUE FROM HERE
// --- think about VAT calculation from subcontractors to agency
				// --- timecard calculations scoping
				{
					if(error_message.empty()) error_message = AssignVariableValue("timecards_sum_amount", string(timecards_sum), true);
					if(error_message.empty()) error_message = AssignVariableValue("timecards_vat_amount", string(timecards_vat), true);
					if(error_message.empty()) error_message = AssignVariableValue("timecards_total_payment", string(timecards_sum + timecards_vat), true);
				}

				// --- cost_center calculations scoping
				{
					c_float	cost_center_vat(0);

					if(Get("agency_vat") == "Y")
						cost_center_vat = cost_center_sum * c_float(VAT_PERCENTAGE) / c_float(100);

					if(error_message.empty()) error_message = AssignVariableValue("cost_center_sum_amount", string(cost_center_sum), true);
					if(error_message.empty()) error_message = AssignVariableValue("cost_center_vat_amount", string(cost_center_vat), true);
					if(error_message.empty()) error_message = AssignVariableValue("cost_center_total_payment", string(cost_center_sum + cost_center_vat), true);
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


ostream& operator<<(ostream& os, const C_Invoicing_Vars &var)
{
	os << "object C_Invoicing_Vars [empty for now]";

	return os;
}

