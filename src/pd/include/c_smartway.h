#ifndef __C_SMARTWAY__H__
#define __C_SMARTWAY__H__

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include "c_config.h"
#include "chtml.h"
#include "clog.h"
#include "utilities.h"
#include "localy.h"

using namespace std;
using namespace rapidjson;

#define	SMARTWAY_URL								"https://api.test.dev.smartway.today/v2"s

struct C_Flight_Route
{
		string			from;
		string			to;
		string			date;
};

class C_Smartway_Employees
{
	private:
		CMysql			*db = nullptr;
		CUser			*user = nullptr;

		string			method					= "";
		string			passport_type			= "";

		string			employee_id				= "";
		string			is_phone_confirmed		= "";
		string			phone					= "";
		string			surname					= "";
		string			name					= "";
		string			patronymic				= "";
		string			surname_eng				= "";
		string			name_eng				= "";
		string			patronymic_eng			= "";
		string			email					= "";
		string			citizenship_code		= "";
		string			birthday				= "";
		string			sex						= "";
		string			ru_passport_number		= "";
		string			ru_passport_due_date	= "";
		string			ru_passport_type		= "";
		string			for_passport_number		= "";
		string			for_passport_due_date	= "";
		string			for_passport_type		= "";
		string			company_tin				= "";

		string			error_message			= "";

		string			CheckValidity();

		string			GetCommonPart_JSON();
		string			GetAllBonuses_JSON();
		string			GetBonuses_JSON();
		string			GetCompaniesINN_JSON();
		string			GetDocuments_JSON();
		string			GetPassport_JSON();
		string			GetAirlineBonuses_JSON();

	public:
		void			SetDB(CMysql *param)	 				{ db = param; }
		void			SetUser(CUser *param)	 				{ user = param; }

		void			SetMethod(const string &p)				{ method = p; };
		string			GetMethod()	const						{ return method; };

		void			SetPassportType(const string &p)		{ passport_type = p; };

		string			GetErrorMessage() const					{ return error_message; };

		// --- this is the only useful method
		string			GetEmployeesJSON(const string &sql_query);
};

class C_Smartway
{
	private: 
		CMysql			*db = nullptr;
		CUser			*user = nullptr;

		string			sow_id							= "";
		string			method							= "";
		string			json_query						= "";
		string			json_response					= "";
		string			employee_id						= "";
		string			airline_booking_result_id		= "";
		string			airline_result_status			= "";
		string			airline_result_error			= "";
		string			airline_result_trip_id			= "";
		string			voucher_file					= "";
		string			purchased_service_id			= "";
		string			purchased_service_amount		= "";
		string			purchased_service_checkin		= "";
		string			purchased_service_checkout		= "";
		string			purchased_service_book_date		= "";
		string			purchased_service_destination	= "";


		C_Smartway_Employees	employees;
		rapidjson::Document		json_obj;

		CHTML			chtml;

		string			GetAuthJSON();

		void			SetMethod(string p) { method = p; };
		string			GetMethod() { return method; };

		void			SetQuery(string p) { json_query = p; };
		string			GetQuery() { return json_query; };
		string			BuildEmployeesSaveQuery(string user_id);
		string			GetEmployeesAirlineBookQuery(const string &user_id, const string &passport_type) const;

		string			GetPostJSON();
		string			SendRequestToServer();

		string			ParseResponse();
		string			ParseResponse_Ping();
		string			ParseResponse_EmployeesSave();
		string			ParseResponse_AirportAutocomplete();
		string			ParseResponse_AirlineSearch();
		string			ParseResponse_AirlineBook();
		string			ParseResponse_AirlineResult();
		string			ParseResponse_TripItemVoucher();
		string			ParseResponse_TripInfo();

		int				GetDirectionLimit(const vector<C_Flight_Route> &flight_routes);
		string			ApplyFilter_ByDirection(const vector<C_Flight_Route> &flight_routes);
		string			ApplyFilters(const vector<C_Flight_Route> &flight_routes);

		void			SetEmployeeID(string p) { employee_id = p; };

		bool			isUserEnrolled(string user_id);
		string			SaveTempFile(const string &content, const string &extension);

		string			SaveVoucher(const string &content, const string &extension);

	public:
						C_Smartway()	{};
						C_Smartway(CMysql *p1, CUser *p2) : db(p1), user(p2) { employees.SetDB(db); employees.SetUser(user);  };

		void			SetSoW(const string &param) { sow_id = param; };
		void			SetSoW(string &&param) { sow_id = move(param); };
		string			GetSoW() {return sow_id; };

		string			ping();
		string			employees_create(string userID);
		string			employees_save(string userID);
		string			airline_search(const vector<C_Flight_Route> &, string cabin_class = "Econom", bool direct = false, string baggage = "all", unsigned int travelers = 1);
		string			airline_book(const string &passport_type, const string &search_id, const string &trip_id, const string &fare_id);
		string			airline_result(const string &id);
		string			trip_info(const string &trip_id);
		string			trip_item_voucher(const string &trip_id);

		string			airport_autocomplete(string query);
		string			GetAirportAutocompleteJSON() const;
		string			GetFlightsJSON() const;
		auto			GetAirlineBookingResultID() const		{ return airline_booking_result_id; };
		auto			GetAirlineResultTripID() const			{ return airline_result_trip_id; };
		auto			GetAirlineResultStatus() const			{ return airline_result_status; };
		auto			GetAirlineResultError() const			{ return airline_result_error; };
		auto			GetVoucherFile() const					{ return voucher_file; };
		auto			GetPurchasedServiceID() const			{ return purchased_service_id; };
		auto			GetPurchasedServiceAmount() const		{ return purchased_service_amount; };
		auto			GetPurchasedServiceCheckin() const		{ return purchased_service_checkin; };
		auto			GetPurchasedServiceCheckout() const		{ return purchased_service_checkout; };
		auto			GetPurchasedServiceBookDate() const		{ return purchased_service_book_date; };
		auto			GetPurchasedServiceDestination() const	{ return purchased_service_destination; };

		string			GetEmployeeID()	{ return employee_id; };
};



#endif
