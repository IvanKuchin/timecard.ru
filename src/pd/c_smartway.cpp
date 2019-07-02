#include "c_smartway.h"

string C_Smartway::GetAuthJSON()
{
	return "\"authorization\": {"
			      "\"username\": \"" + SMARTWAY_LOGIN + "\","
			      "\"password\": \"" + SMARTWAY_PASSWORD + "\""
			    "}"s;
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
		error_message = gettext("json_obj reponse error");
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

string C_Smartway::ParseResponse_AirlineSearch()
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(json_obj["result"].IsObject())
	{

	}
	else
	{
		error_message = "Smartway "s + gettext("result is not an array");
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
				if((error_message = ParseResponse_AirlineSearch()).length()) MESSAGE_ERROR("", "", "fail to parse airline.airport_autocomplete response");
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

			error_message = gettext(("json_obj reponse error (code: " + to_string(error_code_from_server) + ", message: " + error_message_from_server + ")").c_str());
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
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user_id.length())
	{
		if(db)
		{
			if(db->Query("SELECT * FROM `users` WHERE `id`=\"" + user_id + "\";"))			
			{
				auto	employee_id			= user_id;
				auto	is_phone_confirmed	= db->Get(0, "is_phone_confirmed");
				auto	phone				= "+" + db->Get(0, "country_code") + db->Get(0, "phone");
				auto	surname				= db->Get(0, "nameLast");
				auto	name				= db->Get(0, "name");
				auto	patronymic			= db->Get(0, "nameMiddle");
				auto	surname_eng			= db->Get(0, "last_name_en");
				auto	name_eng			= db->Get(0, "first_name_en");
				auto	patronymic_eng		= db->Get(0, "middle_name_en");
				auto	email				= db->Get(0, "email");
				auto	citizenship_code	= db->Get(0, "citizenship_code");
				auto	birthday			= db->Get(0, "birthday");
				auto	sex					= db->Get(0, "sex");
				auto	ru_passport_number	= db->Get(0, "passport_series") + " " + db->Get(0, "passport_number");
				auto	ru_passport_due_date= "null"s;
				auto	ru_passport_type	= "domestic_passport"s;
				auto	for_passport_number	= db->Get(0, "foreign_passport_number");
				auto	for_passport_due_date= db->Get(0, "foreign_passport_expiration_date");
				auto	for_passport_type	= "foreign_passport"s;

				if(db->Query("SELECT * FROM `company` WHERE `admin_userID`=\"" + user_id + "\" and `type`=\"subcontractor\";"))			
				{
					auto	company_tin = db->Get(0, "tin");

					company_tin = "0000000000";					

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

					result = 
						      "\"employees\": ["
						        "{"
						          "\"id\": " + quoted(employee_id) + ","
						          "\"mobile_phone\": " + quoted(phone) + ","
						          "\"surname\": " + quoted(surname) + ","
						          "\"name\": " + quoted(name) + ","
						          "\"patronymic\": " + quoted(patronymic) + ","
						          "\"surname_eng\": " + quoted(surname_eng) + ","
						          "\"name_eng\": " + quoted(name_eng) + ","
						          "\"patronymic_eng\": " + quoted(patronymic_eng) + ","
						          "\"email\": " + quoted(email) + ","
						          "\"citizenship_code\": " + quoted(citizenship_code) + ","
						          "\"birthday\": " + quoted(birthday) + ","
						          "\"sex\": " + quoted(sex) + ","
						          "\"bonuses\": [],"
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
						          "],"
						          "\"companies_inn\": [" + quoted(company_tin) + "]"
						        "}"
						      "]";
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
			error_message = gettext("DB is not initialized");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		error_message = gettext("mandatory parameter missed");
		MESSAGE_ERROR("", "", error_message);
	}

	if(error_message.empty())
	{
		SetQuery("{" + result + "}");
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
