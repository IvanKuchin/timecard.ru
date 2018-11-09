#include "agency.h"

int main(void)
{
	CStatistics		appStat;  // --- CStatistics must be firts statement to measure end2end param's
	CCgi			indexPage(EXTERNAL_TEMPLATE);
	CUser			user;
	string			action, partnerID;
	CMysql			db;
	struct timeval	tv;

	{
		MESSAGE_DEBUG("", action, __FILE__);
	}

	signal(SIGSEGV, crash_handler);

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	try
	{

		indexPage.ParseURL();
		indexPage.AddCookie("lng", "ru", "", "", "/");

		if(!indexPage.SetTemplate("index.htmlt"))
		{
			MESSAGE_ERROR("", action, "template file was missing");
			throw CException("Template file was missing");
		}

		if(db.Connect(DB_NAME, DB_LOGIN, DB_PASSWORD) < 0)
		{
			MESSAGE_ERROR("", action, "Can not connect to mysql database");
			throw CExceptionHTML("MySql connection");
		}

		indexPage.SetDB(&db);

#ifndef MYSQL_3
		db.Query("set names cp1251");
#endif

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
			action = GenerateSession(action, &indexPage, &db, &user);
			action = LogoutIfGuest(action, &indexPage, &db, &user);

		}
	// ------------ end generate common parts

		{
			MESSAGE_DEBUG("", "", "pre-condition if(action == \"" + action + "\")");
		}


		if(action == "AJAX_getGeoRegionName")
		{
			string			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
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
				error_message = "Некорректный параметры";
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
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
				error_message = "Некорректный параметры";
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
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
				error_message = "Некорректный параметры";
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getPositionAutocompleteList")
		{
			string			position = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("position"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(position.length())
			{
				int	affected = db.Query("SELECT `id`, `title` FROM `company_position` WHERE `title` LIKE \"%" + position + "%\" LIMIT 0, 20;");
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
					error_message = "Должность не найдена";
					MESSAGE_DEBUG("", "", "position not found");
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_submitNewGeoZip")
		{
			string			country = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("country"));
			string			region = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("region"));
			string			locality = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("locality"));
			string			zip = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("zip"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(country.length() && region.length() && locality.length() && zip.length())
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
				error_message = "Некорректный параметры";
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getBankInfoByBIK")
		{
			string			bik = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("bik"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(bik.length())
			{
				ostResult << "{\"result\":\"success\","
						  << "\"banks\":[" << GetBankInJSONFormat("SELECT * FROM `banks` WHERE `bik`=\"" + bik + "\";", &db, &user) << "]"
						  << "}";
			}
			else
			{
				error_message = "Некорректный параметры";
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
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

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}


		{
			MESSAGE_DEBUG("", "", "post-condition if(action == \"" + action + "\")");
		}

		indexPage.OutTemplate();

	}
	catch(CExceptionHTML &c)
	{

		c.SetLanguage(indexPage.GetLanguage());
		c.SetDB(&db);
		MESSAGE_DEBUG("", action, "catch CExceptionHTML: DEBUG exception reason: [" + c.GetReason() + "]");

		if(!indexPage.SetTemplate(c.GetTemplate()))
		{
			return(-1);
		}
		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(0);
	}
	catch(CException &c)
	{
		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}
		{
			MESSAGE_ERROR("", action, "catch CException: exception: ERROR  " + c.GetReason());
		}

		indexPage.RegisterVariable("content", c.GetReason());
		indexPage.OutTemplate();
		return(-1);
	}
	catch(exception& e)
	{
		{
			MESSAGE_ERROR("", action, "catch(exception& e): catch standard exception: ERROR  " + e.what());
		}

		if(!indexPage.SetTemplateFile("templates/error.htmlt"))
		{
			return(-1);
		}
		indexPage.RegisterVariable("content", e.what());
		indexPage.OutTemplate();
		return(-1);
	}

	return(0);
}
