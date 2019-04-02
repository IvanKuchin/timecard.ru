#include "c_invoicing_vars.h"

auto	C_Invoicing_Vars::AssignVariableFromDB(const string &var_name, const string &sql_query, bool isMandatory)
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

auto	C_Invoicing_Vars::AssignVariableValue(const string &var_name, const string &value, bool isMandatory)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start (variable: " + var_name + ")");

	if(vars.Get(var_name).empty())
	{
		vars.Add(var_name, ConvertHTMLToText(value));
	}
	else
	{
		MESSAGE_DEBUG("", "", "variable (" + var_name + ") already defined");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Invoicing_Vars::GenerateVariableSet() -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(error_message.empty()) error_message = AssignVariableValue("Bank Identifier", gettext("Bank Identifier"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Account", gettext("Account"), true);
			if(error_message.empty()) error_message = AssignVariableValue("Destination bank", gettext("Destination bank"), true);
			
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

			{
				auto	affected = db->Query("SELECT * FROM `cost_center_custom_fields` WHERE `cost_center_id`=\"" + vars.Get("cost_center_id") + "\";");

				for(auto i = 0; i < affected; ++i)
				{
					if(error_message.empty()) error_message = AssignVariableValue(db->Get(i, "var_name"), db->Get(i, "value"), true);
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




ostream& operator<<(ostream& os, const C_Invoicing_Vars &var)
{
	os << "object C_Invoicing_Vars [empty for now]";

	return os;
}

