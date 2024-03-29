#include "ajax_anyrole.h"

int main(void)
{
	CStatistics		appStat;  // --- CStatistics must be first statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	c_config		config(CONFIG_DIR);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	{
		MESSAGE_DEBUG("", action, __FILE__);
	}

	signal(SIGSEGV, crash_handler);

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);  /* Flawfinder: ignore */

	try
	{

		indexPage.ParseURL();

		if(!indexPage.SetProdTemplate("index.htmlt"))
		{
			MESSAGE_ERROR("", action, "template file was missing");
			throw CException("Template file was missing");
		}

		if(db.Connect(&config) < 0)
		{
			MESSAGE_ERROR("", action, "Can not connect to mysql database");
			throw CException("MySql connection");
		}

		indexPage.SetDB(&db);


		action = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("action"));
		{
			MESSAGE_DEBUG("", "", "action taken from HTTP is " + action);
		}

		// --- generate common parts
		{
			// --- it has to be run before session, because session relay upon Apache environment variable
			if(RegisterInitialVariables(&indexPage, &db, &user))
			{
			}
			else
			{
				MESSAGE_ERROR("", "", "RegisterInitialVariables failed, throwing exception");
				throw CExceptionHTML("environment variable error");
			}

			//------- Generate session
			action = GenerateSession(action, &config, &indexPage, &db, &user);
			action = LogoutIfGuest(action, &config, &indexPage, &db, &user);

		}
	// ------------ end generate common parts

		{
			MESSAGE_DEBUG("", "", "pre-condition if(action == \"" + action + "\")");
		}

		if((action.length() > 10) && (action.compare(action.length() - 9, 9, "_template") == 0))
		{
			ostringstream	ost;
			string			strPageToGet, strFriendsOnSinglePage;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			{
				string		template_name = action.substr(0, action.length() - 9) + ".htmlt";

				if(!indexPage.SetProdTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getCompanyInfo")
		{
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			tin = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("tin"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			company_obj = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(id.length())
			{
				company_obj = GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + id + "\";", &db, &user);

				if(company_obj.length())
				{
				}
				else
				{
					error_message = gettext("Company not found");
					MESSAGE_DEBUG("", "", "company.id(" + id + ") not found");
				}	
					
			}
			else if(tin.length())
			{
				company_obj = GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `tin`=\"" + tin + "\";", &db, &user);

				if(company_obj.length())
				{
				}
				else
				{
					error_message = gettext("Company not found");
					MESSAGE_DEBUG("", "", "company.tin(" + tin + ") not found");
				}	
					
			}
			else
			{
				error_message = "Некорректный номер kompanii";
				MESSAGE_DEBUG("", "", "fail to get company.id(" + id + ")");
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\",\"companies\":[" + company_obj + "]}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}
		if(action == "AJAX_isCompanyExists")
		{
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			tin = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("tin"));
			auto			company_type = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("company_type"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(id.length())
			{
				if(db.Query("SELECT `id` FROM `company` WHERE `id`=\"" + id + "\""s + (company_type.length() ? " AND `type`=\"" + company_type + "\"" : "") + ";"))
				{
					success_message = ",\"isExists\":\"yes\"";
				}
				else
				{
					success_message = ",\"isExists\":\"no\"";
				}
			}
			else if(tin.length())
			{
				if(db.Query("SELECT `id` FROM `company` WHERE `tin`=\"" + tin + "\""s + (company_type.length() ? " AND `type`=\"" + company_type + "\"" : "") + ";"))
				{
					success_message = ",\"isExists\":\"yes\"";
				}
				else
				{
					success_message = ",\"isExists\":\"no\"";
				}					
			}
			else
			{
				error_message = "Некорректный номер kompanii";
				MESSAGE_DEBUG("", "", "fail to get company.id(" + id + ")");
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"" + success_message + "}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getGeoRegionName")
		{
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(id.length())
			{
				if(db.Query("SELECT `title` FROM `geo_region` WHERE `id`=\"" + id + "\";"))
					ostResult << "{\"result\":\"success\",\"geo_region\":{\"id\":\"" + id + "\",\"name\":\"" + db.Get(0, "title") + "\"}}";
				else
				{
					error_message = "Регион не найден";
					MESSAGE_DEBUG("", "", "region_id(" + id + ") not found");
				}	
					
			}
			else
			{
				error_message = "Некорректный номер региона";
				MESSAGE_DEBUG("", "", "fail to get region_id(" + id + ")");
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getGeoRegionAndLocalityNames")
		{
			string			country = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("country"));
			string			zip = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("zip"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(country.length() && zip.length())
			{
				if(db.Query(
					"SELECT `id` FROM `geo_zip` WHERE `zip`=\"" + zip + "\" and `geo_locality_id` IN ("
						"SELECT `id` FROM `geo_locality` WHERE `geo_region_id` IN ("
							"SELECT `id` FROM `geo_region` WHERE `geo_country_id` IN ("
								"SELECT `id` FROM `geo_country` WHERE `title`=\"" + country + "\""
							")"
						")"
					");"
					))
				{
					ostResult << "{\"result\":\"success\","
							  << "\"geo_zip\":[" << GetZipInJSONFormat(db.Get(0, "id"), &db, &user) << "]"
							  << "}";
				}
				else
				{
					error_message = "Индекс не найден";
					MESSAGE_DEBUG("", "", "country and zip not found");
				}	
					
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_DEBUG("", "", "fail to get country name or zip code");
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}
 
		if(action == "AJAX_getRegionAutocompleteList")
		{
			string			country = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("country"));
			string			region = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("region"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(region.length())
			{
				int	affected = db.Query(
					"SELECT `id`, `title` FROM `geo_region` WHERE `title` LIKE \"%" + region + "%\" AND `geo_country_id` IN ("
						"SELECT `id` FROM `geo_country` WHERE `title` LIKE \"%" + country + "%\""
					") LIMIT 0, 20;"
					);
				if(affected)
				{
					ostResult << "{\"result\":\"success\","
							  << "\"autocomplete_list\":[";
					for(int i = 0; i < affected; ++i)
					{
						if(i) ostResult << ",";
						ostResult << "{\"id\":\"" << db.Get(i, "id") << "\","
								  << "\"label\":\"" << db.Get(i, "title") << "\"}";
					}
					ostResult << "]}";
				}
				else
				{
					error_message = "Регион не найден";
					MESSAGE_DEBUG("", "", "country and region pair not found");
				}	
					
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_DEBUG("", "", "fail to get country name or region code");
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getLocalityAutocompleteList")
		{
			string			country = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("country"));
			string			region = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("region"));
			string			locality = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("locality"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(locality.length())
			{
				int	affected = db.Query(
					"SELECT `id`, `title` FROM `geo_locality` WHERE `title` LIKE \"%" + locality + "%\" AND `geo_region_id` IN ("
						"SELECT `id` FROM `geo_region` WHERE `title` LIKE \"%" + region + "%\" AND `geo_country_id` IN ("
							"SELECT `id` FROM `geo_country` WHERE `title` LIKE \"%" + country + "%\""
						")"
					") LIMIT 0, 20;"
					);
				if(affected)
				{
					ostResult << "{\"result\":\"success\","
							  << "\"autocomplete_list\":[";
					for(int i = 0; i < affected; ++i)
					{
						if(i) ostResult << ",";
						ostResult << "{\"id\":\"" << db.Get(i, "id") << "\","
								  << "\"label\":\"" << db.Get(i, "title") << "\"}";
					}
					ostResult << "]}";
				}
				else
				{
					error_message = "Город не найден";
					MESSAGE_DEBUG("", "", "country, region and locality not found");
				}	
					
			}
			else
			{
				error_message = gettext("parameters incorrect");
				MESSAGE_DEBUG("", "", "fail to get country name or region code");
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getLocalityAutocompleteList_2")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			locality = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;

			if(locality.length())
			{
				auto	affected = db.Query("SELECT * FROM `geo_locality_view` WHERE `geo_locality_title` LIKE \"%" + locality + "%\" LIMIT 0, 20;");

				for(int i = 0; i < affected; ++i)
				{
					if(i) success_message += ",";
					success_message += "{"
											"\"id\":\"" + db.Get(i, "geo_locality_id") + "\","
											"\"country_id\":\"" + db.Get(i, "geo_country_id") + "\","
					  						"\"label\":\"" + db.Get(i, "geo_locality_title") + ", " + db.Get(i, "geo_region_title") + ", " + db.Get(i, "geo_country_title") + "\""
				  						"}";
				}				
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", "", error_message);
			}

			indexPage.RegisterVariableForce("result", "[" + success_message + "]");
			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getPositionAutocompleteList")
		{
			auto			position = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("position"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(position.length())
			{
				int	affected = db.Query("SELECT `id`, `title` FROM `company_position` WHERE `title` LIKE \"%" + position + "%\" LIMIT 0, 20;");
				if(affected)
				{
					success_message = ",\"autocomplete_list\":[";
					for(int i = 0; i < affected; ++i)
					{
						if(i) success_message += ",";
						success_message += "{\"id\":\"" + db.Get(i, "id") + "\","
								  + "\"label\":\"" + db.Get(i, "title") + "\"}";
					}
					success_message += "]";
				}
				else
				{
					error_message = "Должность не найдена";
					MESSAGE_DEBUG("", "", "position not found");
				}	
					
			}
			else
			{
				MESSAGE_DEBUG("", "", "mandatory parameter missed");
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"" + success_message + "}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");

				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getAgencyAutocompleteList")
		{
			string			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			string			template_name = "json_response.htmlt";
			string			success_message = "";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(name.length())
			{
				int	affected = db.Query("SELECT `id`, `name` FROM `company` WHERE `name` LIKE \"%" + name + "%\" AND `type`=\"agency\" LIMIT 0, 20;");
				if(affected)
				{
					for(int i = 0; i < affected; ++i)
					{
						if(i) success_message += ",";
						success_message += "{\"id\":\"" + db.Get(i, "id") + "\",\"label\":\"" + RemoveQuotas(ConvertHTMLToText(db.Get(i, "name"))) + "\"}";
					}
				}
				else
				{
					error_message = gettext("agency id not found");
					MESSAGE_DEBUG("", "", error_message);
				}	
					
			}
			else
			{
				error_message = gettext("agency id not found");
				MESSAGE_DEBUG("", "", error_message);
			}

			if(error_message.empty())
			{
				ostResult << "[" << success_message << "]";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "[]";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getAviaBonusAutocompleteList")
		{
			string			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			string			success_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(name.length())
			{
				int	affected = db.Query("SELECT * FROM `airlines` WHERE `code` LIKE \"%" + name + "%\" OR `description_rus` LIKE \"%" + name + "%\" OR `description_eng` LIKE \"%" + name + "%\" LIMIT 0, 20;");
				if(affected)
				{
					for(int i = 0; i < affected; ++i)
					{
						if(i) success_message += ",";
						success_message += "{\"id\":\"" + db.Get(i, "id") + "\","
								  + "\"label\":\"" + db.Get(i, "description_rus") + TIMECARD_ENTRY_TITLE_SEPARATOR + db.Get(i, "description_eng") + TIMECARD_ENTRY_TITLE_SEPARATOR + db.Get(i, "code") + "\"}";
					}
				}
				else
				{
					error_message = gettext("airline not found");
					MESSAGE_DEBUG("", "", error_message);
				}	
					
			}
			else
			{
				MESSAGE_DEBUG("", "", "name is empty");
			}

			if(error_message.empty())
			{
				ostResult << "[" << success_message << "]";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "[]";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getSmartwayAirportAutocompleteList")
		{
			string			query = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			string			success_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(query.length())
			{
				C_Smartway		smartway(&config, &db, &user);

				error_message = smartway.airport_autocomplete(query);
				if(error_message.empty())
				{
					success_message = smartway.GetAirportAutocompleteJSON();
				}
				else
				{
					MESSAGE_ERROR("", "", "fail to save employee");
				}

			}
			else
			{
				MESSAGE_DEBUG("", "", "query is empty");
			}

			indexPage.RegisterVariableForce("result", "[" + success_message + "]");

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getAirportAutocompleteList")
		{
			string			query = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			string			success_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(query.length())
			{
				int	affected = db.Query("SELECT * FROM `airports_view` WHERE (`city_name` LIKE \"%" + query + "%\") OR (`airport_code` LIKE \"%" + query + "%\") OR (`airport_name` LIKE \"%" + query + "%\") OR (`country_name` LIKE \"%" + query + "%\") LIMIT 0, 20;");

				for(int i = 0; i < affected; ++i)
				{
					if(i) success_message += ",";
					success_message += "{\"id\":\"" + db.Get(i, "airport_code") + "\","
							  + "\"label\":\"" + db.Get(i, "airport_name") + " (" + db.Get(i, "city_name") + TIMECARD_ENTRY_TITLE_SEPARATOR + db.Get(i, "country_name") + ")\"}";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "query is empty");
			}

			indexPage.RegisterVariableForce("result", "[" + success_message + "]");

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getAirportCountryAutocompleteList")
		{
			auto			query = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(query.length())
			{
				int	affected = db.Query("SELECT * FROM `airport_countries` WHERE (`title` LIKE \"%" + query + "%\") LIMIT 0, 20;");

				for(int i = 0; i < affected; ++i)
				{
					if(i) success_message += ",";
					
					success_message += "{\"id\":\"" + db.Get(i, "id") + "\","
									  + "\"label\":\"" + db.Get(i, "title") + "\"}";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "query is empty");
			}

			indexPage.RegisterVariableForce("result", "[" + success_message + "]");

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getCountryAutocompleteList")
		{
			auto			query = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("term"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			success_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(query.length())
			{
				int	affected = db.Query("SELECT * FROM `geo_country` WHERE (`title` LIKE \"%" + query + "%\") LIMIT 0, 20;");

				for(int i = 0; i < affected; ++i)
				{
					if(i) success_message += ",";
					
					success_message += "{\"id\":\"" + db.Get(i, "id") + "\","
									  + "\"label\":\"" + db.Get(i, "title") + "\"}";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "query is empty");
			}

			indexPage.RegisterVariableForce("result", "[" + success_message + "]");

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_submitNewGeoZip")
		{
			auto			country = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("country"));
			auto			region = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("region"));
			auto			locality = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("locality"));
			auto			zip = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("zip"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			ostringstream	ostResult;

			ostResult.str("");

			if(zip.length())
			{
				if(country.length() && region.length() && locality.length())
				{
					if(db.Query("SELECT `id` FROM `geo_zip` WHERE `zip`=\"" + zip + "\";") == 0)
					{
						string	country_id = "";

						if(db.Query("SELECT `id` FROM `geo_country` WHERE `title`=\"" + country + "\";")) country_id = db.Get(0, "id");
						else
						{
							long int temp = db.InsertQuery("INSERT INTO `geo_country` (`title`) VALUES (\"" + country + "\");");
							if(temp)
							{
								country_id = to_string(temp);
								MESSAGE_ERROR("", action, "new country.id(" + country_id + ") added");
							}
							else
							{
								MESSAGE_ERROR("", action, "fail to add new country");
							}
						}

						if(country_id.length())
						{
							string	region_id = "";

							if(db.Query("SELECT `id` FROM `geo_region` WHERE `title`=\"" + region + "\";")) region_id = db.Get(0, "id");
							else
							{
								long int temp = db.InsertQuery("INSERT INTO `geo_region` (`geo_country_id`,`title`) VALUES (\"" + country_id + "\",\"" + region + "\");");

								if(temp)
								{
									region_id = to_string(temp);
									MESSAGE_ERROR("", action, "new region.id(" + region_id + ") added");
								}
								else
								{
									MESSAGE_ERROR("", action, "fail to add new region");
								}
							}

							if(region_id.length())
							{
								string	locality_id = "";

								if(db.Query("SELECT `id` FROM `geo_locality` WHERE `title`=\"" + locality + "\";")) locality_id = db.Get(0, "id");
								else
								{
									long int temp = db.InsertQuery("INSERT INTO `geo_locality` (`geo_region_id`,`title`) VALUES (\"" + region_id + "\",\"" + locality + "\");");

									if(temp)
									{
										locality_id = to_string(temp);
										MESSAGE_ERROR("", action, "new locality.id(" + locality_id + ") added");
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to add new locality");
									}
								}

								if(locality_id.length())
								{
									string	zip_id = "";
									long int temp = db.InsertQuery("INSERT INTO `geo_zip` (`geo_locality_id`,`zip`) VALUES (\"" + locality_id + "\",\"" + zip + "\");");

									if(temp)
									{
										zip_id = to_string(temp);
										MESSAGE_ERROR("", action, "new zip.id(" + zip_id + ") added");
									}
									else
									{
										MESSAGE_ERROR("", action, "fail to add new zip");
									}

									if(zip_id.length())
									{
										ostResult << "{\"result\":\"success\","
												  << "\"geo_zip\":[" << GetZipInJSONFormat(zip_id, &db, &user) << "]"
												  << "}";
									}
									else
									{
										error_message = "Не удалось добавить индекс";
										MESSAGE_ERROR("", action, "fail to add new zip");
									}
								}
								else
								{
									error_message = "Не удалось добавить город";
									MESSAGE_ERROR("", action, "fail to add new locality");
								}
							}
							else
							{
								error_message = "Не удалось добавить регион";
								MESSAGE_ERROR("", action, "fail to add new region");
							}

						}
						else
						{
							error_message = "Не удалось добавить страну";
							MESSAGE_ERROR("", action, "fail to add new country");
						}

					}
					else
					{
						error_message = "Индекс уже существует в БД";
						MESSAGE_DEBUG("", "", "zip(" + zip + ") already exists in DB");
					}	
						
				}
				else
				{
					error_message = gettext("parameters incorrect");
					MESSAGE_DEBUG("", "", "mandatory parameter missed");
				}
			}
			else
			{
				error_message = gettext("zip code must be a number");
				MESSAGE_ERROR("", action, error_message)
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getBankInfoByBIK")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			bik = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("bik"));
			auto			success_message = ""s;
			auto			error_message = ""s;

			if(bik.length())
			{
				auto temp_message = GetBankInJSONFormat("SELECT * FROM `banks` WHERE `bik`=\"" + bik + "\";", &db, &user);

				if(temp_message.length())
				{
					success_message = "\"banks\":[" + temp_message + "]";
				}
				else
				{
					if((error_message = isUserAllowedToCreateBIK(&user, &db)).empty())
					{
						// --- user allowed to create new BIK
						success_message = "\"banks\":[]";
					}
					else
					{
						MESSAGE_DEBUG("", action, error_message);
					}
				}
			}
			else
			{
				error_message = gettext("bank_id is empty");
				MESSAGE_DEBUG("", "", error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_submitNewBank")
		{
			string			account = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("account"));
			string			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("name"));
			string			bik = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("bik"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(account.length() && name.length() && bik.length())
			{
				if(db.Query("SELECT `id` FROM `banks` WHERE `bik`=\"" + bik + "\";") == 0)
				{
					{
						{
							{
								string	bank_id = "";
								long int temp = db.InsertQuery("INSERT INTO `banks` (`title`,`bik`,`account`) VALUES (\"" + name + "\",\"" + bik + "\",\"" + account + "\");");

								if(temp)
								{
									bank_id = to_string(temp);
									// --- this is important event needs to be investigated, therefore high severity
									MESSAGE_ERROR("", action, "new bank.id(" + bank_id + ") added");
								}
								else
								{
									MESSAGE_ERROR("", action, "fail to add new bik");
								}

								if(bank_id.length())
								{
									ostResult << "{\"result\":\"success\","
											  << "\"banks\":[" << GetBankInJSONFormat("SELECT * FROM `banks` WHERE `id`=\"" + bank_id + "\";", &db, &user) << "]"
											  << "}";
								}
								else
								{
									error_message = "Не удалось добавить индекс";
									MESSAGE_ERROR("", action, "fail to add new bik");
								}
							}
						}
					}
				}
				else
				{
					error_message = "Банк уже существует в БД";
					MESSAGE_DEBUG("", "", "bik(" + bik + ") already exists in DB");
				}	
					
			}
			else
			{
				error_message = "Некорректные параметры";
				MESSAGE_DEBUG("", "", "mandatory parameter missed");
			}

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_addNewCompany")
		{
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			legal_geo_zip		= CheckHTTPParam_Number	(indexPage.GetVarsHandler()->Get("geo_zip_legal"));
			auto			legal_address		= CheckHTTPParam_Text	(indexPage.GetVarsHandler()->Get("legal_address"));
			auto			mailing_geo_zip		= CheckHTTPParam_Number	(indexPage.GetVarsHandler()->Get("geo_zip_mailing"));
			auto			mailing_address		= CheckHTTPParam_Text	(indexPage.GetVarsHandler()->Get("mailing_address"));
			auto			kpp					= CheckHTTPParam_Text	(indexPage.GetVarsHandler()->Get("company_kpp"));
			auto			template_name = "json_response.htmlt"s;
			auto			error_message = ""s;
			auto			company_obj = ""s;
			C_Company		company(&db, &user);

			ostringstream	ostResult;

			ostResult.str("");

			if(mailing_geo_zip.empty()) mailing_geo_zip = legal_geo_zip;
			if(mailing_address.empty()) mailing_address = legal_address;

			company.SetName				(CheckHTTPParam_Text  (indexPage.GetVarsHandler()->Get("company_title")));
			company.SetType				(CheckHTTPParam_Text  (indexPage.GetVarsHandler()->Get("type")));
			company.SetLegalGeoZip		(legal_geo_zip);
			company.SetLegal_address	(legal_address);
			company.SetMailingGeoZip	(mailing_geo_zip);
			company.SetMailing_address	(mailing_address);
			company.SetBIK				(CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bank_bik")));
			company.SetAccount			(CheckHTTPParam_Text  (indexPage.GetVarsHandler()->Get("company_account")));
			company.SetTIN				(CheckHTTPParam_Text  (indexPage.GetVarsHandler()->Get("company_tin")));
			company.SetVAT				(CheckHTTPParam_Text  (indexPage.GetVarsHandler()->Get("company_vat")));
			company.SetOGRN				(CheckHTTPParam_Text  (indexPage.GetVarsHandler()->Get("company_ogrn")));
			company.SetKPP				(kpp.length() ? kpp : "0");

			if((error_message = company.CheckValidity()).empty())
			{
				if((error_message = company.InsertToDB()).empty())
				{
					if(company.GetType() == "agency")
					{
						auto	company_employee_id = db.InsertQuery(
																"INSERT INTO `company_employees` (`user_id`, `company_id`, `allowed_change_agency_data`,`allowed_change_sow`,`eventTimestamp`) "
																"VALUES "
																" (\"" + user.GetID() + "\", \"" + company.GetID() + "\", \"Y\", \"Y\", UNIX_TIMESTAMP())"
															);
						if(company_employee_id == 0)
						{
							MESSAGE_ERROR("", action, "fail to insert to company_employee table");
						}
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "company InsertToDB failed");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "company CheckValidity failed");
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\",\"company_id\":\"" + company.GetID() + "\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getAbsenceTypesList")
		{
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			ostResult	<< "{\"result\":\"success\","
						<< "\"absence_types\":["
						<< GetAbsenceTypesInJSONFormat("SELECT * FROM `absence_types`;", &db, &user)
						<< "]}";

			if(error_message.empty())
			{
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult.str("");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_submitUnknownLocality")
		{
			MESSAGE_DEBUG("", action, "start");

			auto			success_message = ""s;
			auto			error_message = ""s;

			if(db.InsertQuery("INSERT INTO `geo_locality_unknown` SET `locality`=" + quoted(indexPage.GetVarsHandler()->Get("value")) + ", `eventTimestamp`=NOW();"))
			{
			}
			else
			{
				error_message = gettext("SQL syntax error");
				MESSAGE_ERROR("", action, error_message);
			}

			AJAX_ResponseTemplate(&indexPage, success_message, error_message);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addAviaBonusNumber")
		{
			ostringstream	ostResult;
			auto			success_message = ""s;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				auto			program_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("program_id"));
				auto			number = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("number"));

				if(program_id.length() && number.length())
				{
					if(db.Query("SELECT `id` FROM `user_bonuses_avia` WHERE `user_id`=\"" + user.GetID() + "\" AND `airline_id`=\"" + program_id + "\";"))
					{
						error_message = gettext("already exists");
						MESSAGE_DEBUG("", action, error_message);
					}
					else
					{
						auto new_id = db.InsertQuery("INSERT INTO `user_bonuses_avia` (`user_id`, `airline_id`, `number`, `eventTimestamp`) VALUES "
														"("
															"\"" + user.GetID() + "\","
															"\"" + program_id + "\","
															"\"" + number + "\","
															"UNIX_TIMESTAMP()"
														");");
						if(new_id)
						{
							success_message = ",\"bonuses_airlines\":[" + GetUserBonusesAirlinesInJSONFormat("SELECT * FROM `user_bonuses_avia` WHERE `user_id`=\"" + user.GetID() + "\";", &db, &user) + "]";
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
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"" + success_message + "}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addRailroadBonusNumber")
		{
			ostringstream	ostResult;
			auto			success_message = ""s;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				auto			program_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("program_id"));
				auto			number = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("number"));

				if(program_id.length() && number.length())
				{
					if(db.Query("SELECT `id` FROM `user_bonuses_railroads` WHERE `user_id`=\"" + user.GetID() + "\" AND `railroad_id`=\"" + program_id + "\";"))
					{
						error_message = gettext("already exists");
						MESSAGE_DEBUG("", action, error_message);
					}
					else
					{
						auto new_id = db.InsertQuery("INSERT INTO `user_bonuses_railroads` (`user_id`, `railroad_id`, `number`, `eventTimestamp`) VALUES "
														"("
															"\"" + user.GetID() + "\","
															"\"" + program_id + "\","
															"\"" + number + "\","
															"UNIX_TIMESTAMP()"
														");");
						if(new_id)
						{
							success_message = ",\"bonuses_railroads\":[" + GetUserBonusesRailroadsInJSONFormat("SELECT * FROM `user_bonuses_railroads` WHERE `user_id`=\"" + user.GetID() + "\";", &db, &user) + "]";
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
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"" + success_message + "}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addHotelchainBonusNumber")
		{
			ostringstream	ostResult;
			auto			success_message = ""s;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				auto			program_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("program_id"));
				auto			number = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("number"));

				if(program_id.length() && number.length())
				{
					if(db.Query("SELECT `id` FROM `user_bonuses_hotels` WHERE `user_id`=\"" + user.GetID() + "\" AND `hotel_chain_id`=\"" + program_id + "\";"))
					{
						error_message = gettext("already exists");
						MESSAGE_DEBUG("", action, error_message);
					}
					else
					{
						auto new_id = db.InsertQuery("INSERT INTO `user_bonuses_hotels` (`user_id`, `hotel_chain_id`, `number`, `eventTimestamp`) VALUES "
														"("
															"\"" + user.GetID() + "\","
															"\"" + program_id + "\","
															"\"" + number + "\","
															"UNIX_TIMESTAMP()"
														");");
						if(new_id)
						{
							success_message = ",\"bonuses_hotel_chains\":[" + GetUserBonusesHotelchainsInJSONFormat("SELECT * FROM `user_bonuses_hotels` WHERE `user_id`=\"" + user.GetID() + "\";", &db, &user) + "]";
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
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"" + success_message + "}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteAirlineBonusNumber")
		{

			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

			ostResult.str("");

			if(id.length())
			{
				db.Query("DELETE FROM `user_bonuses_avia` WHERE `user_id`=\"" + user.GetID() + "\" AND `id`=\"" + id + "\";");
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteRailroadBonusNumber")
		{

			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

			ostResult.str("");

			if(id.length())
			{
				db.Query("DELETE FROM `user_bonuses_railroads` WHERE `user_id`=\"" + user.GetID() + "\" AND `id`=\"" + id + "\";");
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteHotelchainBonusNumber")
		{

			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

			ostResult.str("");

			if(id.length())
			{
				db.Query("DELETE FROM `user_bonuses_hotels` WHERE `user_id`=\"" + user.GetID() + "\" AND `id`=\"" + id + "\";");
			}
			else
			{
				error_message = gettext("mandatory parameter missed");
				MESSAGE_DEBUG("", action, error_message);
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_updateRailroadBonusNumber")
		{

			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			value = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			ostResult.str("");

			if(value.length())
			{
				if(id.length())
				{
					db.Query("UPDATE `user_bonuses_railroads` SET `number`=\"" + value + "\" WHERE `user_id`=\"" + user.GetID() + "\" AND `id`=\"" + id + "\";");
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("field is empty");
				MESSAGE_DEBUG("", action, error_message);
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_updateAirlineBonusNumber")
		{

			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			value = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			ostResult.str("");

			if(value.length())
			{
				if(id.length())
				{
					db.Query("UPDATE `user_bonuses_avia` SET `number`=\"" + value + "\" WHERE `user_id`=\"" + user.GetID() + "\" AND `id`=\"" + id + "\";");
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("field is empty");
				MESSAGE_DEBUG("", action, error_message);
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_updateHotelchainBonusNumber")
		{

			MESSAGE_DEBUG("", action, "start");

			ostringstream	ostResult;
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			auto			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			auto			value = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

			ostResult.str("");

			if(value.length())
			{
				if(id.length())
				{
					db.Query("UPDATE `user_bonuses_hotels` SET `number`=\"" + value + "\" WHERE `user_id`=\"" + user.GetID() + "\" AND `id`=\"" + id + "\";");
				}
				else
				{
					error_message = gettext("mandatory parameter missed");
					MESSAGE_DEBUG("", action, error_message);
				}
			}
			else
			{
				error_message = gettext("field is empty");
				MESSAGE_DEBUG("", action, error_message);
			}

			if(error_message.empty())
			{
				ostResult << "{\"result\":\"success\"}";
			}
			else
			{
				MESSAGE_DEBUG("", action, "failed");
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetProdTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);

			MESSAGE_DEBUG("", action, "finish");
		}


		MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");

		indexPage.OutTemplate();

	}
	catch(CExceptionHTML &c)
	{
		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);

		MESSAGE_DEBUG("", action, "catch CExceptionHTML: DEBUG exception reason: [" + c.GetReason() + "]");

		if(!indexPage.SetProdTemplate(c.GetTemplate()))
		{
			MESSAGE_ERROR("", "", "template (" + c.GetTemplate() + ") not found");
			return(-1);
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();

		return(-1);
	}
	catch(CException &c)
	{
		MESSAGE_ERROR("", action, "catch CException: exception: ERROR  " + c.GetReason());

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			MESSAGE_ERROR("", "", "template not found");
			return(-1);
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();

		return(-1);
	}
	catch(exception& e)
	{
		MESSAGE_ERROR("", action, "catch(exception& e): catch standard exception: ERROR  " + e.what());

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			MESSAGE_ERROR("", "", "template not found");
			return(-1);
		}
		
		indexPage.RegisterVariable("content", e.what());
		indexPage.OutTemplate();

		return(-1);
	}

	return(0);
}
