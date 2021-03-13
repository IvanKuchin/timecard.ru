#include "c_smartway.h"

string C_Smartway_Employees::CheckValidity()
{
	MESSAGE_DEBUG("", "", "start"); 

	error_message = "";

	// --- validity check
	if(is_phone_confirmed == "N")
	{
		error_message = gettext("phone is not confirmed");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(surname.empty())
	{
		error_message = gettext("last name is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(name.empty())
	{
		error_message = gettext("name is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(surname_eng.empty())
	{
		error_message = gettext("foreign last name is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(name_eng.empty())
	{
		error_message = gettext("foreign name is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(email.empty())
	{
		error_message = gettext("email is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(citizenship_code.empty())
	{
		error_message = gettext("citizenship code is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(birthday.empty())
	{
		error_message = gettext("birthday is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(sex.empty())
	{
		error_message = gettext("sex is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(ru_passport_number.empty())
	{
		error_message = gettext("ru passport number is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(ru_passport_type.empty())
	{
		error_message = gettext("ru passport type is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(ru_passport_due_date.empty())
	{
		error_message = gettext("ru passport due date is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(for_passport_number.empty())
	{
		error_message = gettext("foreign passport number is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(for_passport_type.empty())
	{
		error_message = gettext("foreign passport type is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(for_passport_due_date.empty())
	{
		error_message = gettext("foreign passport due date is empty");
		MESSAGE_ERROR("", "", error_message);
	}
	else if(company_tin.empty())
	{
		error_message = gettext("company tin is empty");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish"); 

	return error_message;
}

string C_Smartway_Employees::GetCommonPart_JSON()
{
	MESSAGE_DEBUG("", "", "start"); 

	auto	result = 
				"\"id\": " + quoted(employee_id) + ","
				"\"surname\": " + quoted(surname) + ","
				"\"name\": " + quoted(name) + ","
				"\"patronymic\": " + quoted(patronymic) + ","
				"\"email\": " + quoted(email) + ","
				"\"birthday\": " + quoted(birthday) + ","
				"\"sex\": " + quoted(sex) + "";

	MESSAGE_DEBUG("", "", "finish"); 

	return result;
}

string C_Smartway_Employees::GetAllBonuses_JSON()
{
	MESSAGE_DEBUG("", "", "start"); 

	auto	bonuses = ""s;
	auto	affected = db->Query("SELECT * FROM `user_bonuses_avia_view` WHERE `user_id`=\"" + user->GetID() + "\";");

	for(auto i = 0; i < affected; ++i)
	{
		if(bonuses.length()) bonuses += ",";
		bonuses +=	"{"s + 
						"\"code\":" + quoted(db->Get(i, "code")) + "," +
						"\"number\":" + quoted(db->Get(i, "number")) + "," +
						"\"type\":" + quoted("air"s) + "" +
					"}";
	}

	affected = db->Query("SELECT * FROM `user_bonuses_railroad_view` WHERE `user_id`=\"" + user->GetID() + "\";");
	for(auto i = 0; i < affected; ++i)
	{
		if(bonuses.length()) bonuses += ",";
		bonuses +=	"{"s + 
						"\"code\":" + quoted(db->Get(i, "code")) + "," +
						"\"number\":" + quoted(db->Get(i, "number")) + "," +
						"\"type\":" + quoted("train"s) + "" +
					"}";
	}


	MESSAGE_DEBUG("", "", "finish"); 

	return "\"bonuses\":[" + bonuses + "]";
}

string C_Smartway_Employees::GetCompaniesINN_JSON()
{
	return "\"companies_inn\": [" + company_tin + "]";
}

string C_Smartway_Employees::GetDocuments_JSON()
{
	MESSAGE_DEBUG("", "", "start"); 

	auto	result = 
					  "\"documents\": ["
					    "{"
					      "\"number\": " + quoted(ru_passport_number) + ","
					      "\"due_date\": " + quoted(ru_passport_due_date) + ","
					      "\"type\": " + quoted(ru_passport_type) + ""
					    "},"
					    "{"
					      "\"number\": " + quoted(for_passport_number) + ","
					      "\"due_date\": " + quoted(for_passport_due_date) + ","
					      "\"type\": " + quoted(for_passport_type) + ""
					    "}"
					  "]";

	MESSAGE_DEBUG("", "", "finish"); 

	return result;
}

string C_Smartway_Employees::GetPassport_JSON()
{
	MESSAGE_DEBUG("", "", "start"); 
	auto	result = ""s;

	if(passport_type == "domestic_passport")
	{

		result = 
					  "\"passports\": ["
					    "{"
					      "\"number\": " + quoted(ru_passport_number) + ","
					      "\"due_date\": " + quoted(ru_passport_due_date) + ","
					      "\"type\": \"DomesticPassport\","
					      "\"selected\": true"
					    "}]";
	}
	else if(passport_type == "foreign_passport")
	{
		result = 
					    "\"passports\": [{"
					      "\"number\": " + quoted(for_passport_number) + ","
					      "\"due_date\": " + quoted(for_passport_due_date) + ","
					      "\"type\": \"ForeignPassport\","
					      "\"selected\": true"
					    "}]";
	}
	else
	{
		MESSAGE_ERROR("", "", "passport_type(" + passport_type + ") is unknown");
	}

	MESSAGE_DEBUG("", "", "finish"); 

	return result;
}

string C_Smartway_Employees::GetAirlineBonuses_JSON()
{
	MESSAGE_DEBUG("", "", "start"); 

	auto	result = "\"airline_bonuses\": ["s;
	auto	affected = db->Query("SELECT * FROM `user_bonuses_avia_view` WHERE `user_id`=" + quoted(employee_id) + ";");

	for(auto i = 0; i < affected; ++i)
	{
		if(i) result += ",";
		result +=
		    "{"
		      "\"iata\": " + quoted(db->Get(i, "code")) + ","
		      "\"number\": " + quoted(db->Get(i, "number")) + ""
		    "}";
	}
	result += "]";

	MESSAGE_DEBUG("", "", "finish"); 

	return result;
}

string C_Smartway_Employees::GetEmployeesJSON(const string &sql_query)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start"); 

	if(sql_query.length())
	{
		if(db)
		{
			if(db->Query(sql_query))			
			{
				auto	affected = 0;

				employee_id			= db->Get(0, "id");
				is_phone_confirmed	= db->Get(0, "is_phone_confirmed");
				phone				= "+" + db->Get(0, "country_code") + db->Get(0, "phone");
				surname				= db->Get(0, "nameLast");
				name				= db->Get(0, "name");
				patronymic			= db->Get(0, "nameMiddle");
				surname_eng			= db->Get(0, "last_name_en");
				name_eng			= db->Get(0, "first_name_en");
				patronymic_eng		= db->Get(0, "middle_name_en");
				email				= db->Get(0, "email");
				citizenship_code	= db->Get(0, "citizenship_code");
				birthday			= db->Get(0, "birthday");
				sex					= db->Get(0, "sex");
				ru_passport_number	= db->Get(0, "passport_series") + " " + db->Get(0, "passport_number");
				ru_passport_due_date= "null"s;
				ru_passport_type	= "domestic_passport"s;
				for_passport_number	= db->Get(0, "foreign_passport_number");
				for_passport_due_date= db->Get(0, "foreign_passport_expiration_date");
				for_passport_type	= "foreign_passport"s;

				affected = db->Query("SELECT `tin` FROM `company` WHERE `id` IN (SELECT `agency_company_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=(SELECT `id` FROM `company` WHERE `admin_userID`=" + quoted(employee_id) + "));");
				if(affected)
				{
					for(auto i = 0; i < affected; ++i)
					{
						if(company_tin.length()) company_tin += ",";
						company_tin += quoted(db->Get(i, 0));
					}

					if(CheckValidity().empty())
					{
						if(GetMethod() == "employees.save")
						{
							result = 
							        "{" +
										GetCommonPart_JSON() + "," + 
										"\"mobile_phone\": " + quoted(phone) + "," +
										"\"surname_eng\": " + quoted(surname_eng) + ","
										"\"name_eng\": " + quoted(name_eng) + ","
										"\"patronymic_eng\": " + quoted(patronymic_eng) + "," +
										"\"citizenship_code\": " + quoted(citizenship_code) + "," +
										GetAllBonuses_JSON() + "," +
										GetDocuments_JSON() + "," +
							          	GetCompaniesINN_JSON() +
							        "}";

						}
						else if(GetMethod() == "airline.book")
						{
							result = 
							        "{" +
										GetCommonPart_JSON() + "," + 
										"\"phone\": " + quoted(phone) + "," +
										"\"surname_e\": " + quoted(surname_eng) + ","
										"\"name_e\": " + quoted(name_eng) + ","
										"\"patronymic_e\": " + quoted(patronymic_eng) + "," +
										"\"citizenship\": " + quoted(citizenship_code) + "," +
										GetAirlineBonuses_JSON() + "," +
										GetPassport_JSON() + "," +
							          	GetCompaniesINN_JSON() +
							        "}";

						}
						else
						{
							error_message = gettext("unknown smartway method");
							MESSAGE_ERROR("", "", error_message + "(" + GetMethod() + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", GetErrorMessage());
					}
				}
				else
				{
					error_message = gettext("company not found");
					MESSAGE_ERROR("", "", error_message);
				}
			}
			else
			{
				error_message = gettext("user not found");
				MESSAGE_ERROR("", "", error_message);
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
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}


	MESSAGE_DEBUG("", "", "finish (" + (result.length() ? "success" : "fail") + ")");

	return result;
}







// --- C_Smartway

string C_Smartway::GetAuthJSON()
{
	MESSAGE_DEBUG("", "", "start");

    c_config    config;
    auto        auth_map        	= config.Read({"SMARTWAY_LOGIN", "SMARTWAY_PASSWORD"});
    auto        is_auth_valid   	= (auth_map["SMARTWAY_LOGIN"].length() > 0);
    auto        __SMARTWAY_LOGIN    = (is_auth_valid ? auth_map["SMARTWAY_LOGIN"] : "");
    auto        __SMARTWAY_PASSWORD = (is_auth_valid ? auth_map["SMARTWAY_PASSWORD"] : "");

	auto		result				=  "\"authorization\": {"
									      "\"username\": \"" + __SMARTWAY_LOGIN + "\","
									      "\"password\": \"" + __SMARTWAY_PASSWORD + "\""
									    "}"s;

	if(is_auth_valid) {}
	else
	{
		MESSAGE_ERROR("", "", gettext("service credentials not found"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string	C_Smartway::GetPostJSON()
{
	auto	result = ""s;

	if(GetMethod().length())
	{
		result = 
		"{"
		  "\"jsonrpc\": \"2.0\","
		  "\"id\": 1,"
		  "\"method\": \"" + GetMethod() + "\","
		  "\"params\": "
		  	"{" + GetAuthJSON() + ","
		    "\"query\": " + GetQuery() + ""
		  "}"
		"}";
	}
	else
	{
		MESSAGE_ERROR("", "", "method is not defined");
	}

	return result;
}

string	C_Smartway::SendRequestToServer()
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	post_json = GetPostJSON();

	if(post_json.length())
	{
		chtml.SetParsingDisable();
		chtml.SetPostJSON(post_json);
		chtml.PerformRequest(SMARTWAY_URL);
		// chtml.PerformRequest("http://127.0.0.1:8080");
		json_response = chtml.GetContent();

		MESSAGE_DEBUG("", "", "response (" + json_response + ")")
	}
	else
	{
		MESSAGE_ERROR("", "", "there are no data to post to remote server");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_Ping()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj.HasMember("result"))
	{
		if(json_obj["result"].IsNull()) {}
		else
		{
			error_message = gettext("result is not null");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("json_obj response error");
		MESSAGE_ERROR("", "", error_message + ", missed result member");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_EmployeesSave()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

		if(json_obj["result"].IsArray())
		{
			auto result_id = ""s;
			auto result_success = false;

			// --- continue from here
			if(json_obj["result"][0].HasMember("id"))
			{
				if(json_obj["result"][0].HasMember("success"))
				{
					if(json_obj["result"][0]["id"].IsString())
					{
						result_id = json_obj["result"][0]["id"].GetString();
						if(json_obj["result"][0]["success"].IsBool())
						{
							result_success = json_obj["result"][0]["success"].GetBool();

							if(result_success)
							{
								SetEmployeeID(result_id);
							}
							else
							{
								error_message = "Smartway "s + "response.success "s + gettext("is not true");
								MESSAGE_ERROR("", "", error_message);
							}
						}
						else
						{
							error_message = "Smartway "s + "response.success "s + gettext("is not a boolean");
							MESSAGE_ERROR("", "", error_message)
						}
					}
					else
					{
						error_message = "Smartway "s + "response.id "s + gettext("is not a string");
						MESSAGE_ERROR("", "", error_message)
					}
				}
				else
				{
					error_message = "Smartway "s + gettext("response failed to parse");
					MESSAGE_ERROR("", "", error_message + "(member 'id' is missed)")
				}
			}
			else
			{
				error_message = "Smartway "s + gettext("response failed to parse");
				MESSAGE_ERROR("", "", error_message + "(member 'id' is missed)")
			}
		}
		else
		{
			error_message = "Smartway "s + gettext("result is not an array");
			MESSAGE_ERROR("", "", error_message);
		}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

int C_Smartway::GetDirectionLimit(const vector<C_Flight_Route> &flight_routes)
{
	MESSAGE_DEBUG("", "", "start");

	auto		error_message = ""s;
	auto		limit = 0;

	if(sow_id.length())
	{
		if(flight_routes.size())
		{
			if(db->Query(
						"SELECT `limit` FROM `airfare_limits_by_direction` WHERE "s + 
							"(`from`=(SELECT `airport_country_id` FROM `airports` WHERE `airport_code`=\"" + flight_routes[0].from + "\") AND `to`=(SELECT `airport_country_id` FROM `airports` WHERE `airport_code`=\"" + flight_routes[0].to + "\")) " + 
							"OR "
							"(`to`=(SELECT `airport_country_id` FROM `airports` WHERE `airport_code`=\"" + flight_routes[0].from + "\") AND `from`=(SELECT `airport_country_id` FROM `airports` WHERE `airport_code`=\"" + flight_routes[0].to + "\")) " + 
							"AND "
							"(`agency_company_id`=(SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=" + quoted(sow_id) + "))"
				))
			{
				limit = stoi(db->Get(0, 0));
			}
			else
			{
				MESSAGE_DEBUG("", "", "no limitation configured on this direction");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "flight_routes is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty. Call SetSoW(xxxx) prior to GetDirectionLimit execution");
	}

	MESSAGE_DEBUG("", "", "finish");

	return limit;
}

string C_Smartway::ApplyFilter_ByDirection(const vector<C_Flight_Route> &flight_routes)
{
	MESSAGE_DEBUG("", "", "start");

	auto		error_message = ""s;
	auto		direction_limit = GetDirectionLimit(flight_routes);

	if(direction_limit)
	{
		if(json_obj["result"].HasMember("trips") && json_obj["result"]["trips"].IsArray())
		{
			auto &trips = json_obj["result"]["trips"];

			for (SizeType i = 0; i < trips.Size(); ++i) // Uses SizeType instead of size_t
			{
				auto	&trip = trips[i];

				if(trip.HasMember("price") && trip["price"].IsNumber())
				{
					auto	trip_price = trip["price"].GetDouble();

					if(trip_price > direction_limit)
					{
						MESSAGE_DEBUG("", "", "trip price " + to_string(trip_price) + " is over the limit(" + to_string(direction_limit) + ")");

						trips.Erase(trips.Begin() + i--);
					}
					else
					{
						if(trip.HasMember("fares") && trip["fares"].IsArray())
						{
							auto	&fares = trip["fares"];
							for (SizeType j = 0; j < fares.Size(); j++) // Uses SizeType instead of size_t
							{
								auto	&fare = fares[j];

								if(fare.HasMember("price") && fare["price"].IsNumber())
								{
									auto	fare_price = fare["price"].GetDouble();

									if(fare_price > direction_limit)
									{
										MESSAGE_DEBUG("", "", "fare price " + to_string(fare_price) + " is over the limit(" + to_string(direction_limit) + ")");
										fares.Erase(fares.Begin() + j--);
									}
								}
								else
								{
									error_message = gettext("json_obj response error");
									MESSAGE_ERROR("", "", error_message + "(price is not a number or missed)");
								}
							}

						}
						else
						{
							error_message = gettext("json_obj response error");
							MESSAGE_ERROR("", "", error_message + "(fares is not an array or missed)");
						}
					}
				}
				else
				{
					error_message = gettext("json_obj response error");
					MESSAGE_ERROR("", "", error_message + "(price is not a number or missed)");
				}


			}
		}
		else
		{
			error_message = gettext("json_obj response error");
			MESSAGE_ERROR("", "", error_message + "(trips is not an array or missed)");
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "no direction limit");
	}


	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ApplyFilters(const vector<C_Flight_Route> &flight_routes)
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if((error_message = ApplyFilter_ByDirection(flight_routes)).empty())
	{

	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_AirlineSearch()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsObject())
	{
		// --- good to go
	}
	else
	{
		error_message = "Smartway "s + gettext("result is not an array");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_AirlineResult()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsObject())
	{
		if(json_obj["result"]["status"].IsString())
		{
			airline_result_status = json_obj["result"]["status"].GetString();

			if(GetAirlineResultStatus() == "done")
			{
				if(json_obj["result"]["trip_id"].IsNumber())
				{
					airline_result_trip_id = to_string(json_obj["result"]["trip_id"].GetInt());
				}
				else
				{
					error_message = "Smartway trip_id "s + gettext("is not a number");
					MESSAGE_ERROR("", "", error_message);
				}
			}
			else if(GetAirlineResultStatus() == "pending")
			{
				airline_result_trip_id = "";
			}
			else if(GetAirlineResultStatus() == "failed")
			{
				if(json_obj["result"]["error"].IsString())
				{
					airline_result_error = json_obj["result"]["error"].GetString();
				}
				else
				{
					error_message = "Smartway error "s + gettext("is not a string");
					MESSAGE_ERROR("", "", error_message);
				}
			}
			else
			{
				error_message = "Smartway "s + gettext("unknown method");
				MESSAGE_ERROR("", "", error_message);
			}

		}
		else
		{
			error_message = "Smartway status "s + gettext("is not a string");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = "Smartway "s + gettext("result is not a object");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_TripInfo()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsObject())
	{
		if(json_obj["result"].HasMember("items") && json_obj["result"]["items"].IsArray())
		{
			auto	&item = json_obj["result"]["items"][0];

			if(item.HasMember("service_type") && item["service_type"].IsString())
			{
				if(item.HasMember("id") && item["id"].IsNumber())
				{
					if(item["service_type"].GetString() == "Air"s)
					{
						purchased_service_id = to_string(item["id"].GetInt());

						// --- more optional fields
						if(item.HasMember("amount") && item["amount"].IsNumber())
						{
							purchased_service_amount = to_string((long int)item["amount"].GetDouble());
						}
						else
						{
							error_message = "Smartway amount "s + gettext("is not a number");
							MESSAGE_ERROR("", "", error_message);
						}

						if(item.HasMember("checkin") && item["checkin"].IsString())
						{
							purchased_service_checkin = item["checkin"].GetString();
						}
						else
						{
							error_message = "Smartway checkin "s + gettext("is not a string");
							MESSAGE_ERROR("", "", error_message);
						}

						if(item.HasMember("checkout") && item["checkout"].IsString())
						{
							purchased_service_checkout = item["checkout"].GetString();
						}
						else
						{
							error_message = "Smartway checkout "s + gettext("is not a string");
							MESSAGE_ERROR("", "", error_message);
						}

						if(json_obj["result"].HasMember("book_date") && json_obj["result"]["book_date"].IsString())
						{
							purchased_service_book_date = json_obj["result"]["book_date"].GetString();
						}
						else
						{
							error_message = "Smartway book_date "s + gettext("is not a string");
							MESSAGE_ERROR("", "", error_message);
						}

						if(json_obj["result"].HasMember("name") && json_obj["result"]["name"].IsString())
						{
							purchased_service_destination = json_obj["result"]["name"].GetString();
						}
						else
						{
							error_message = "Smartway name "s + gettext("is not a string");
							MESSAGE_ERROR("", "", error_message);
						}


					}
					else
					{
						error_message = "Smartway service_type "s + gettext("is not an air reservation");
						MESSAGE_ERROR("", "", error_message);
					}
				}
				else
				{
					error_message = "Smartway id "s + gettext("is not a number");
					MESSAGE_ERROR("", "", error_message);
				}
			}
			else
			{
				error_message = "Smartway service_type "s + gettext("is not a string");
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			error_message = "Smartway items "s + gettext("is not an array");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = "Smartway "s + gettext("result is not an object");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::SaveTempFile(const string &content, const string &extension)
{
	auto	result = ""s;
	auto	fname = ""s;

	MESSAGE_DEBUG("", "", "start");

	do
	{
		fname = GetRandom(10) + extension;
		result = TEMP_DIRECTORY_PREFIX + fname;
	} while(isFileExists(result));

	ofstream ofs (result, std::ofstream::out);
	ofs << content;
	ofs.close();

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string C_Smartway::SaveVoucher(const string &content, const string &extension)
{
	auto	fname = ""s;
	auto	voucher_folder = ""s;
	auto	voucher_file = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(content.length())
	{
		do
		{
			voucher_folder = to_string( (int)(rand()/(RAND_MAX + 1.0) * SMARTWAY_VOUCHERS_NUMBER_OF_FOLDERS) + 1);
			voucher_file = GetRandom(15) + extension;
			fname = voucher_folder + "/" + voucher_file;

		} while(isFileExists(SMARTWAY_VOUCHERS_DIRECTORY + fname));

		ofstream ofs (SMARTWAY_VOUCHERS_DIRECTORY + fname, std::ofstream::out);
		ofs << content;
		ofs.close();
	}
	else
	{
		MESSAGE_DEBUG("", "", "file content is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + fname + ")");

	return fname;
}

string C_Smartway::ParseResponse_TripItemVoucher()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsObject())
	{
		if(json_obj["result"].HasMember("content"))
		{
			if(json_obj["result"]["content"].IsString())
			{
				auto	base64_encoded_string = json_obj["result"]["content"].GetString();
				auto	base64_encoded_file = SaveTempFile(base64_encoded_string, ".enc64");

				auto	base64_decoded_string = base64_decode(base64_encoded_string);
				auto	base64_decoded_file = SaveTempFile(base64_decoded_string, ".pdf");

				if(base64_decoded_string.length())
				{
					voucher_file = SaveVoucher(base64_decoded_string, ".pdf");
				}
				else
				{
					error_message = gettext("voucher content is empty");
					MESSAGE_ERROR("", "", error_message);
				}
			}
			else
			{
				error_message = "Smartway content "s + gettext("is not a string");
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			error_message = "Smartway content "s + gettext("member has missed");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = "Smartway "s + gettext("result is not a string");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_AirlineBook()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsString())
	{
		airline_booking_result_id = json_obj["result"].GetString();
	}
	else
	{
		error_message = "Smartway "s + gettext("result is not a string");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse_AirportAutocomplete()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsArray())
	{
		auto result_id = ""s;

		for(auto i = 0u; i < json_obj["result"].Size(); ++i)
		{
			// --- continue from here
			if(json_obj["result"][i].HasMember("code"))
			{
				if(json_obj["result"][i].HasMember("country"))
				{
					if(json_obj["result"][i]["country"].HasMember("name_eng"))
					{
						if(json_obj["result"][i].HasMember("name_eng"))
						{
						}
						else
						{
							error_message = "Smartway "s + "response.id "s + gettext("response failed to parse");
							MESSAGE_ERROR("", "", error_message + "(member 'name_eng' is missed)")
							break;
						}
					}
					else
					{
						error_message = "Smartway "s + "response.id "s + gettext("response failed to parse");
						MESSAGE_ERROR("", "", error_message + "(member 'name_eng' is missed)")
						break;
					}
				}
				else
				{
					error_message = "Smartway "s + gettext("response failed to parse");
					MESSAGE_ERROR("", "", error_message + "(member 'country' is missed)");
					break;
				}
			}
			else
			{
				error_message = "Smartway "s + gettext("response failed to parse");
				MESSAGE_ERROR("", "", error_message + "(member 'code' is missed)");
				break;
			}
		}
	}
	else
	{
		error_message = "Smartway "s + gettext("result is not an array");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

string C_Smartway::ParseResponse()
{
	MESSAGE_DEBUG("", "", "start");

	auto		error_message = ""s;

	json_obj.Parse(json_response.c_str());

	if(json_obj.IsObject())
	{
		if(json_obj.HasMember("result"))
		{
			if(GetMethod() == "ping")
			{
				if((error_message = ParseResponse_Ping()).length()) MESSAGE_ERROR("", "", "fail to parse ping response");
			}
			else if(GetMethod() == "employees.save")
			{
				if((error_message = ParseResponse_EmployeesSave()).length()) MESSAGE_ERROR("", "", "fail to parse employees.save response");
			}
			else if(GetMethod() == "airline.airport_autocomplete")
			{
				if((error_message = ParseResponse_AirportAutocomplete()).length()) MESSAGE_ERROR("", "", "fail to parse airline.airport_autocomplete response");
			}
			else if(GetMethod() == "airline.search")
			{
				if((error_message = ParseResponse_AirlineSearch()).length()) MESSAGE_ERROR("", "", "fail to parse airline.search response");
			}
			else if(GetMethod() == "airline.book")
			{
				if((error_message = ParseResponse_AirlineBook()).length()) MESSAGE_ERROR("", "", "fail to parse airline.book response");
			}
			else if(GetMethod() == "airline.result")
			{
				if((error_message = ParseResponse_AirlineResult()).length()) MESSAGE_ERROR("", "", "fail to parse airline.book response");
			}
			else if(GetMethod() == "trip.info")
			{
				if((error_message = ParseResponse_TripInfo()).length()) MESSAGE_ERROR("", "", "fail to parse trip.info response");
			}
			else if(GetMethod() == "trip.item_voucher")
			{
				if((error_message = ParseResponse_TripItemVoucher()).length()) MESSAGE_ERROR("", "", "fail to parse trip.item_voucher response");
			}
			else
			{
				error_message = gettext("unknown smartway method");
				MESSAGE_ERROR("", "", error_message + " (" + GetMethod() + ")");
			}
		}
		else
		{
			auto error_message_from_server = ""s;
			auto error_code_from_server = 0;

			if(json_obj.HasMember("error"))
			{
				if(json_obj["error"].HasMember("message"))
					error_message_from_server = json_obj["error"]["message"].GetString();
				else
				{
					MESSAGE_ERROR("", "", "response from server have no error.message member");
				}
				if(json_obj["error"].HasMember("code"))
					error_code_from_server = json_obj["error"]["code"].GetInt();
				else
				{
					MESSAGE_ERROR("", "", "response from server have no error.code member");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "response from server have no error member");
			}

			error_message = gettext("json_obj response error") + " ("s + gettext("code") + ": " + to_string(error_code_from_server) + ", " + gettext("message") + ": " + error_message_from_server + ")";
			MESSAGE_ERROR("", "", error_message + ", missed result member");
		}
	}
	else
	{
		error_message = gettext("root item is not an object");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

bool C_Smartway::isUserEnrolled(string user_id)
{
	auto	result = false;
	auto	smartway_user_id = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `smartway_employee_id` FROM `users` WHERE `id`=\"" + user_id + "\";"))
	{
		if((smartway_user_id = db->Get(0, 0)).length())
		{
			result = true;
			MESSAGE_DEBUG("", "", "user_id(" + user_id + ") already enrolled in smartway employee.id(" + smartway_user_id + ")");
		}
		else
		{
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not found");
	}

	MESSAGE_DEBUG("", "", "finish (" + (result ? "success" : "fail") + ")");

	return result;
}

string C_Smartway::BuildEmployeesSaveQuery(string user_id)
{
	auto	error_message = ""s;
	auto	json = ""s;

	MESSAGE_DEBUG("", "", "start");

	employees.SetMethod("employees.save");
	json = employees.GetEmployeesJSON("SELECT * FROM `users` WHERE `id`=\"" + user_id + "\" AND `type`=\"subcontractor\";");

	if(json.length())
	{
		SetQuery("{\"employees\":[" + json + "]}");
	}
	else
	{
		error_message = employees.GetErrorMessage();
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string C_Smartway::employees_create(string user_id)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(isUserEnrolled(user_id))
	{
		error_message = gettext("user already enrolled");
		MESSAGE_ERROR("", "", error_message);
	}
	else
	{
		error_message = employees_save(user_id);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string C_Smartway::employees_save(string user_id)
{
	auto	error_message = ""s;
	auto	employee_save_query = ""s;

	MESSAGE_DEBUG("", "", "start");


	SetMethod("employees.save");
	if((error_message = BuildEmployeesSaveQuery(user_id)).empty())
	{
		if((error_message = SendRequestToServer()).empty())
		{
			if((error_message = ParseResponse()).empty())
			{

			}
			else
			{
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string C_Smartway::ping()
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	SetMethod("ping");
	SetQuery("{}");

	if((error_message = SendRequestToServer()).empty())
	{
		if((error_message = ParseResponse()).empty())
		{

		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string C_Smartway::airport_autocomplete(string query)
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	SetMethod("airline.airport_autocomplete");
	SetQuery(quoted(query));

	if((error_message = SendRequestToServer()).empty())
	{
		if((error_message = ParseResponse()).empty())
		{

		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string C_Smartway::GetAirportAutocompleteJSON() const
{
	auto	result = ""s;
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	for(SizeType i = 0; i < json_obj["result"].Size(); ++i)
	{
		// --- continue from here
		if(json_obj["result"][i].HasMember("code") && json_obj["result"][i]["code"].IsString())
		{
				if(json_obj["result"][i].HasMember("country") && json_obj["result"][i]["country"].IsObject())
				{
						if(json_obj["result"][i]["country"].HasMember("name_eng") && json_obj["result"][i]["country"]["name_eng"].IsString())
						{
							if(json_obj["result"][i].HasMember("name_eng") && json_obj["result"][i]["name_eng"].IsString())
							{
								if(result.length()) result += ",";
								result = result + "{"
												 + "\"id\":\"" + json_obj["result"][i]["code"].GetString() + "\","
												 + "\"label\":\"" + json_obj["result"][i]["name_eng"].GetString() + " (" + json_obj["result"][i]["country"]["name_eng"].GetString() + ")\"" +
											"}";
							}
							else
							{
								error_message = "Smartway "s + "response.id "s + gettext("response failed to parse");
								MESSAGE_ERROR("", "", error_message + "(member 'name_eng' is missed or not a string)")
								break;
							}
						}
						else
						{
							error_message = "Smartway "s + "response.id "s + gettext("response failed to parse");
							MESSAGE_ERROR("", "", error_message + "(member 'name_eng' is missed or not a string)")
							break;
						}
				}
				else
				{
					error_message = "Smartway "s + gettext("response failed to parse");
					MESSAGE_ERROR("", "", error_message + "(member 'country' is missed or not a string)");
					break;
				}
		}
		else
		{
			error_message = "Smartway "s + gettext("response failed to parse");
			MESSAGE_ERROR("", "", error_message + "(member 'code' is missed or not a string)");
			break;
		}
	}

	MESSAGE_DEBUG("", "", "finish (" + (result.length() ? "success" : "fail") + ")");

	return result;
}

string C_Smartway::GetFlightsJSON() const
{
	auto	result = ""s;
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");


	if(json_obj.HasMember("result") && json_obj["result"].IsObject())
	{
		StringBuffer			sb;
		Writer<StringBuffer>	writer(sb);

		json_obj["result"].Accept(writer);

		result = sb.GetString();
	}
	else
	{
		error_message = "Smartway "s + "response.id "s + gettext("response failed to parse");
		MESSAGE_ERROR("", "", error_message + "(result is missed or doesn't exists)");
	}

	MESSAGE_DEBUG("", "", "finish (" + (result.length() ? "success" : "fail") + ")");

	return result;
}

string	C_Smartway::airline_search(const vector<C_Flight_Route> &flight_routes, string cabin_class, bool direct, string baggage, unsigned int travelers)
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;

	auto	temp_query = 
	"{"
      "\"class\": " + quoted(cabin_class) + ","
      "\"direct\": " + (direct ? "true" : "false") + ","
      "\"baggage\": " + quoted(baggage) + ","
      "\"travelers\": " + to_string(travelers) + ","
      "\"routes\": [";

	for(auto i = 0u; i < flight_routes.size(); ++i)
	{
		if(i) temp_query += ",";
		temp_query +=
        "{"
			"\"departure\": " + quoted(flight_routes[i].from) + ","
			"\"arrival\": " + quoted(flight_routes[i].to) + ","
			"\"date\": " + quoted(GetSpellingFormattedDate(flight_routes[i].date, "%F")) + ""
        "}";
	}

	temp_query +=
      "]"
    "}";

	SetMethod("airline.search");
	SetQuery(temp_query);

	if((error_message = SendRequestToServer()).empty())
	{
		if((error_message = ParseResponse()).empty())
		{
			if((error_message = ApplyFilters(flight_routes)).empty())
			{

			}
			else
			{
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string	C_Smartway::airline_book(const string &passport_type, const string &search_id, const string &trip_id, const string &fare_id)
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	temp_query = ""s;

	if(sow_id.length())
	{
		employees.SetPassportType(passport_type);
		employees.SetMethod("airline.book");
		temp_query = 
						"{"
							"\"item\":{"
								"\"search_id\": " + quoted(search_id) + ","
								"\"trip_id\": " + quoted(trip_id) + 
								(fare_id == "null" ? "" : ",\"fare_id\": " + quoted(fare_id) + "") +
						    "},"
						    "\"employees\":[" + employees.GetEmployeesJSON("SELECT * FROM `users` WHERE `id`=("
					    														"SELECT `admin_userID` FROM `company` WHERE `id`=("
								    													"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=" + quoted(sow_id) + ""
					    														")"
					    													") AND `type`=\"subcontractor\";") + "]"
						"}";

		SetMethod("airline.book");
		SetQuery(temp_query);

		if((error_message = SendRequestToServer()).empty())
		{
			if((error_message = ParseResponse()).empty())
			{
			}
			else
			{
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", "sow_id is empty. Call SetSoW(xxxx) prior to GetDirectionLimit execution");
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string	C_Smartway::airline_result(const string &id)
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	temp_query = ""s;

	SetMethod("airline.result");
	SetQuery("\"" + id + "\"");

	if((error_message = SendRequestToServer()).empty())
	{
		if((error_message = ParseResponse()).empty())
		{
		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string	C_Smartway::trip_info(const string &trip_id)
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	temp_query = ""s;

	SetMethod("trip.info");
	SetQuery(trip_id);

	if((error_message = SendRequestToServer()).empty())
	{
		if((error_message = ParseResponse()).empty())
		{
		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}

string	C_Smartway::trip_item_voucher(const string &trip_id)
{
	MESSAGE_DEBUG("", "", "start");

	auto	error_message = ""s;
	auto	temp_query = ""s;


	if((error_message = trip_info(trip_id)).empty())
	{
		SetMethod("trip.item_voucher");
		SetQuery(GetPurchasedServiceID());

		if((error_message = SendRequestToServer()).empty())
		{
			if((error_message = ParseResponse()).empty())
			{
			}
			else
			{
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to get purchased_service_id");
	}

	MESSAGE_DEBUG("", "", "finish (" + (error_message.empty() ? "success" : "fail") + ")");

	return error_message;
}
