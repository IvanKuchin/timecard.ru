#ifndef __C_SMARTWAY__H__
#define __C_SMARTWAY__H__

#include <rapidjson/document.h>

#include "chtml.h"
#include "clog.h"
#include "utilities.h"
#include "localy.h"

using namespace std;
using namespace rapidjson;

class C_Smartway
{
private: 
		CMysql			*db = nullptr;
		CUser			*user = nullptr;

		string			method = "";
		string			json_query = "";
		string			json_response = "";
		string			employee_id = "";

		rapidjson::Document	json_obj;

		CHTML			chtml;

		string			GetAuthJSON();

		void			SetMethod(string p) { method = p; };
		string			GetMethod() { return method; };

		void			SetQuery(string p) { json_query = p; };
		string			GetQuery() { return json_query; };
		string			BuildEmployeesSaveQuery(string user_id);

		string			GetPostJSON();
		string			SendRequestToServer();

		string			ParseResponse();
		string			ParseResponse_Ping();
		string			ParseResponse_EmployeesSave();

		void			SetEmployeeID(string p) { employee_id = p; };

		bool			isUserEnrolled(string user_id);

public:
						C_Smartway()	{};
						C_Smartway(CMysql *p1, CUser *p2) : db(p1), user(p2) {};

		string			ping();
		string			employees_create(string userID);
		string			employees_save(string userID);

		string			GetEmployeeID()	{ return employee_id; };
};

#endif
