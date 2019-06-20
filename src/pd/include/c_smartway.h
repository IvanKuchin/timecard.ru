#ifndef __C_SMARTWAY__H__
#define __C_SMARTWAY__H__

#include "chtml.h"
#include "clog.h"
#include "utilities.h"
#include "localy.h"

using namespace std;

class C_Smartway
{
private: 
		CMysql			*db = nullptr;
		CUser			*user = nullptr;

		string			method = "";
		string			json_query = "";

		CHTML			chtml;

		string			GetAuthJSON();

		void			SetMethod(string p) { method = p; };
		string			GetMethod() { return method; };

		void			SetQuery(string p) { json_query = p; };
		string			GetQuery() { return json_query; };

		string			GetPostJSON();
		string			SendRequestToServer();
		
public:
						C_Smartway()	{};
						C_Smartway(CMysql *p1, CUser *p2) : db(p1), user(p2) {};

		string			ping();
};

#endif
