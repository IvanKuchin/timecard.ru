#include "agency.h"

static string isAgencyEmployeeAllowedToChangeSoW(string sow_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(user->GetType() == "agency")
		{
			if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\" AND `agency_company_id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_sow`=\"Y\");"))
			{
				// --- everything is good
			}
			else
			{
				MESSAGE_DEBUG("", "", "user(" + user->GetID() + ") have not rights to change SoW(" + sow_id + ")");
				error_message = "Вы не можете менять данные договора";
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") is not an agency employee");
			error_message = "Информация доступна только для агенства";
		}
	}
	else
	{
		error_message = "Неизвестный номер договора";
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

static string isAgencyEmployeeAllowedToChangeAgencyData(CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(user && db)
	{
		if(user->GetType() == "agency")
		{
			if(db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\" AND `allowed_change_agency_data`=\"Y\";"))
			{
				// --- everything is good
			}
			else
			{
				MESSAGE_DEBUG("", "", "user(" + user->GetID() + ") have no rights to change agency data");
				error_message = "Вы не можете менять данные договора";
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user(" + user->GetID() + ") is not an agency employee");
			error_message = "Информация доступна только для агенства";
		}
	}
	else
	{
		error_message = "Инициализации данных";
		MESSAGE_ERROR("", "", "db or user is NULL");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

static struct tm GetFirstDayReportedOnAssignmentID(string sow_id, string task_id, CMysql *db)
{
	struct tm	result;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
			"SELECT `row`, `timecards`.`period_start` as `period_start` FROM `timecard_lines`"
			"INNER JOIN  `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id`"
			"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`contract_sow_id` = \"" + sow_id + "\" "
			"ORDER BY `timecards`.`period_start` ASC LIMIT 0, 1"
		))
	{
		int			first_reporting_day, last_reporting_day;
		string		row = db->Get(0, "row");

		result = GetTMObject(db->Get(0, "period_start"));
		tie(first_reporting_day, last_reporting_day) = FirstAndLastReportingDaysFromTimereport(SplitTimeentry(row));
		result.tm_mday += first_reporting_day;

	}
	else
	{
		result = GetTMObject("3333-01-01");

		MESSAGE_DEBUG("", "", "no time reported on sow_id/task_id(" + sow_id + "/" + task_id + "), assign fake date further in future");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + PrintTime(result, RU_DATE_FORMAT) + ")");


	return result;	
}

static struct tm GetLastDayReportedOnAssignmentID(string sow_id, string task_id, CMysql *db)
{
	struct tm	result;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
			"SELECT `row`, `timecards`.`period_end` as `period_end` FROM `timecard_lines`"
			"INNER JOIN  `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id`"
			"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`contract_sow_id` = \"" + sow_id + "\" "
			"ORDER BY `timecards`.`period_end` DESC LIMIT 0, 1"
		))
	{
		int			first_reporting_day, last_reporting_day;
		string		row = db->Get(0, "row");

		result = GetTMObject(db->Get(0, "period_end"));
		tie(first_reporting_day, last_reporting_day) = FirstAndLastReportingDaysFromTimereport(SplitTimeentry(row));
		result.tm_mday -= last_reporting_day;
	}
	else
	{
		result = GetTMObject("1900-01-01");

		MESSAGE_DEBUG("", "", "no time reported on sow_id/task_id(" + sow_id + "/" + task_id + "), assign fake date further in future");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + PrintTime(result, RU_DATE_FORMAT) + ")");


	return result;	
}

static auto	isActionEntityBelongsToSoW(string action, string id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
		if(id.length())
		{
			if(action.length())
			{

				auto		sql_query = ""s;

				if(action == "AJAX_updateCustomerTitle") 			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN (SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN (SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\"));";
				if(action == "AJAX_updateProjectTitle") 			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN (SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\");";
				if(action == "AJAX_updateTaskTitle") 				sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodStart") 				sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodEnd") 				sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteTimecardApproverFromSoW")	sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `timecard_approvers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteBTExpenseApproverFromSoW")	sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `bt_approvers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSubcontractorCreateTasks")	sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true

				if(action == "AJAX_updateSoWNumber")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWAct")					sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWPosition")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWDayRate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWSignDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWStartDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWEndDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateSoWCustomField")			sql_query = "SELECT `contract_sow_id` AS `sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updatePSoWNumber")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWAct")					sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWPosition")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWDayRate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWSignDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWStartDate")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWEndDate")				sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updatePSoWCustomField")			sql_query = "SELECT `contract_psow_id` AS `sow_id` FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateCostCenterNumber")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterAct")			sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterSignDate")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterStartDate")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterEndDate")		sql_query = "SELECT \"" + sow_id + "\" AS `sow_id`;"; // --- fake request, always true
				if(action == "AJAX_updateCostCenterCustomField")	sql_query = "SELECT `cost_center_id` AS `sow_id` FROM `cost_center_custom_fields` WHERE `id`=\"" + id + "\";";

				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						string	sow_id_from_db = db->Get(0, "sow_id");

						if(sow_id == sow_id_from_db)
						{
							// --- everything is good
						}
						else
						{
							error_message = "Нет доступа";
							MESSAGE_ERROR("", "", "Probably hacker action(" + action + "), because action(" + action + ") entity.id(" + id + ") doesn't belong to sow.id(" + sow_id + ")");
						}
					}
					else
					{
						error_message = "Нет доступа";
						MESSAGE_ERROR("", "", "action(" + action + ") element.id(" + id + ") doesn't belong to sow.id(" + sow_id + ")");
					}
				}
				else
				{
					error_message = "Неизвестное действие";
					MESSAGE_ERROR("", "", "unknown action (" + action + ")");
				}
			}
			else
			{
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Идентификатор поля не задан";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}
	else
	{
		error_message = "Неизвестный договор";
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

static string	CheckNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				if(new_value.length())
				{
					if(action == "AJAX_updatePeriodStart")
					{
						if(sow_id.length())
						{
							string	assignment_id = id;

							// --- find existing period_start and period_end
							if(db->Query("SELECT `period_start`, `period_end`,`timecard_tasks_id` FROM `timecard_task_assignment` WHERE `id`=\"" + assignment_id + "\";"))
							{
								string		task_id = db->Get(0, "timecard_tasks_id");
								struct tm	existing_assignment_start	= GetTMObject(db->Get(0, "period_start"));
								struct tm	existing_assignment_end		= GetTMObject(db->Get(0, "period_end"));
								struct tm	new_assignment_start		= GetTMObject(new_value);

								if(new_assignment_start <= existing_assignment_end)
								{
									if(new_assignment_start <= existing_assignment_start)
									{
										// --- start date can be moved earlier
									}
									else
									{
										struct tm	first_reporting_day	= GetFirstDayReportedOnAssignmentID(sow_id, task_id, db);

										if(new_assignment_start <= first_reporting_day)
										{
											// --- start date can be moved earlier
										}
										else
										{
											error_message = "Часы уже были заполнены на " + PrintTime(first_reporting_day, RU_DATE_FORMAT) + " дату.";
											MESSAGE_DEBUG("", "", "assignment.id(" + assignment_id + ") has first reported day on " + PrintTime(first_reporting_day, RU_DATE_FORMAT) + ". Start can't be later than this date.");
										}
									}
								}
								else
								{
									error_message = "Начало должно быть раньше окончания";
									MESSAGE_DEBUG("", "", "timecard_task_assignment.id(" + id + ") requsted start(" + PrintTime(new_assignment_start, RU_DATE_FORMAT) + ") must be earlier than existing finish(" + PrintTime(existing_assignment_end, RU_DATE_FORMAT) + ").")
								}
							}
							else
							{
								error_message = "Назначение на задачу не найдено в БД";
								MESSAGE_ERROR("", "", "timecard_task_assignment.id(" + id + ") not found")
							}
						}
						else
						{
							error_message = "Неизвестный договор";
							MESSAGE_ERROR("", "", "sow_id is empty");
						}
					}
					else if(action == "AJAX_updatePeriodEnd")
					{
						if(sow_id.length())
						{
							string	assignment_id = id;

							// --- find existing period_start and period_end
							if(db->Query("SELECT `period_start`, `period_end`,`timecard_tasks_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";"))
							{
								string		task_id = db->Get(0, "timecard_tasks_id");
								struct tm	existing_assignment_start	= GetTMObject(db->Get(0, "period_start"));
								struct tm	existing_assignment_end 	= GetTMObject(db->Get(0, "period_end"));
								struct tm	new_assignment_end			= GetTMObject(new_value);

								if(existing_assignment_start <= new_assignment_end)
								{
									if(existing_assignment_end <= new_assignment_end)
									{
										// --- start date can be moved earlier
									}
									else
									{
										struct tm	last_reporting_day	= GetLastDayReportedOnAssignmentID(sow_id, task_id, db);

										if(last_reporting_day <= new_assignment_end)
										{
											// --- start date can be moved earlier
										}
										else
										{
											error_message = "Часы уже были заполнены на " + PrintTime(last_reporting_day, RU_DATE_FORMAT) + " дату.";
											MESSAGE_DEBUG("", "", "assignment.id(" + assignment_id + ") has last reported day on " + PrintTime(last_reporting_day, RU_DATE_FORMAT) + ". Finish can't be earlier than this date.");
										}
									}
								}
								else
								{
									error_message = "Окончание должно быть позднее начала";
									MESSAGE_DEBUG("", "", "timecard_task_assignment.id(" + id + ") requsted finish(" + PrintTime(new_assignment_end, RU_DATE_FORMAT) + ") must be later than existing start(" + PrintTime(existing_assignment_start, RU_DATE_FORMAT) + ").")
								}
							}
							else
							{
								error_message = "Назначение на задачу не найдено в БД";
								MESSAGE_ERROR("", "", "timecard_task_assignment.id(" + id + ") not found");
							}
						}
						else
						{
							error_message = "Неизвестный договор";
							MESSAGE_ERROR("", "", "sow_id is empty");
						}
					}
					else if(action == "AJAX_updateCustomerTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + new_value + "\" AND `agency_company_id` = ( "
								"SELECT `agency_company_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Заказчик " + new_value + " уже существует";
							MESSAGE_DEBUG("", "", "Customer with new_value already exists");
						}
					}
					else if(action == "AJAX_updateProjectTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + new_value + "\" AND `timecard_customers_id` = ( "
								"SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\""
							");"
							))
						{
							error_message = "Проект " + new_value + " уже есть у этого Заказчика";
							MESSAGE_DEBUG("", "", "Project with new_value already assigned to the same Customer");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateTaskTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + new_value + "\" AND `timecard_projects_id` = ( "
								"SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\""
							");"
							))
						{
							error_message = "Задача " + new_value + " уже существует в этом проекте";
							MESSAGE_DEBUG("", "", "Customer with new_value already assigned to the same Customer/Project");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateAgencyEditCapability")
					{
						if(db->Query(
							"SELECT `id` FROM `company` WHERE `admin_userID`=("
								"SELECT `user_id` FROM `company_employees` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Нельзя запретить редактирование компании ее владельцу";
							MESSAGE_DEBUG("", "", "employee.id(" + id + ") is company owner, he should be able edit company details any time");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateSoWEditCapability")			{ /* --- good to go */ }
					else if(action == "AJAX_updateAgencyPosition")				{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWPosition")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyTitle")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyDescription")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyWebSite")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyTIN")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyAccount")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyOGRN")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyKPP")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyLegalAddress")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyMailingAddress")		{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyMailingZipID")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyLegalZipID")			{ /* --- good to go */ }
					else if(action == "AJAX_updateCompanyBankID")				{ /* --- good to go */ }
					else if(action == "AJAX_updateSubcontractorCreateTasks")	{ /* --- good to go */ }
					else if(action == "AJAX_deleteCostCenterFromCustomer")		{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterTitle")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterDescription")		{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWAct")						{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWSignDate")					{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_updateSoWStartDate")
					{
						if(db->Query("SELECT `end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = new_value;
							auto		string_end = db->Get(0, "end_date");
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							if(tm_start <= tm_end)
							{
								if(db->Query("SELECT `period_start`, `period_end` FROM `timecards` where `contract_sow_id`=\"" + sow_id + "\" ORDER BY `period_start` ASC LIMIT 0,1;"))
								{
									auto	string_timecard_report_date = db->Get(0, "period_start");
									auto	tm_timecard_report_date = GetTMObject(string_timecard_report_date);

									if(tm_start <= tm_timecard_report_date)
									{
										// --- good to go
									}
									else
									{
										error_message = utf8_to_cp1251(gettext("Subcontractor reported time on")) + " " + PrintDate(tm_timecard_report_date);
										MESSAGE_DEBUG("", "", "Subcontractor reported time on " + PrintDate(tm_timecard_report_date) + " earlier than SoW start " + PrintDate(tm_start));
									}
								}
								else
								{
									error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
									MESSAGE_ERROR("", "", "issue in SQL-syntax");
								}
							}
							else
							{
								error_message = utf8_to_cp1251(gettext("period start have to precede period end")) + " (" + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateSoWEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = db->Get(0, "start_date");
							auto		string_end = new_value;
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							mktime(&tm_start);
							mktime(&tm_end);

							if(tm_start <= tm_end)
							{
								if(db->Query("SELECT `period_start`, `period_end` FROM `timecards` where `contract_sow_id`=\"" + sow_id + "\" ORDER BY `period_end` DESC LIMIT 0,1;"))
								{
									auto	string_timecard_report_date = db->Get(0, "period_end");
									auto	tm_timecard_report_date = GetTMObject(string_timecard_report_date);

									if(tm_timecard_report_date <= tm_end)
									{
										// --- good to go
									}
									else
									{
										error_message = utf8_to_cp1251(gettext("Subcontractor reported time on")) + " " + PrintDate(tm_timecard_report_date);
										MESSAGE_DEBUG("", "", "Subcontractor reported time on " + PrintDate(tm_timecard_report_date) + " earlier than SoW start " + PrintDate(tm_start));
									}
								}
								else
								{
									error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
									MESSAGE_ERROR("", "", "issue in SQL-syntax");
								}
							}
							else
							{
								error_message = utf8_to_cp1251(gettext("period start have to precede period end")) + " (" + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateSoWDayRate")
					{
						c_float		num(new_value);

						if(string(num) == new_value) { /* --- good to go */ }
						else
						{
							MESSAGE_ERROR("", "", "input DayRate(" + new_value + ") wrongly formatted, need to be " + string(num));
						}
					}
					else if(action == "AJAX_deleteCostCenter")
					{
						if(db->Query("SELECT COUNT(*) AS `counter` FROM `cost_center_assignment` WHERE `cost_center_id`=\"" + id + "\";"))
						{
							auto		counter = stoi(db->Get(0, "counter"));

							if(counter)
							{
								char	buffer[50];
								sprintf(buffer, ngettext("%d customers", "%d customers", counter), counter);

								error_message = utf8_to_cp1251(gettext("cost center assigned")) + " " + utf8_to_cp1251(buffer) + ". " +  utf8_to_cp1251(gettext("removal prohibited"));
								MESSAGE_DEBUG("", "", "cost_center.id(" + id + ") assigned to " + to_string(counter) + " customers. Removal prohibited.");
							}
							else
							{
								// --- good to go
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}					
					else if(action == "AJAX_updateSoWNumber")
					{
						if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `number`=\"" + new_value + "\" AND `id`!=\"" + sow_id + "\" AND `agency_company_id`=(SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");"))
						{
							error_message = utf8_to_cp1251(gettext("SoW number already exists"));
							MESSAGE_DEBUG("", "", "sow.number already exists in agency.id");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_addTimecardApproverToSoW")
					{
						if(db->Query("SELECT `id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + new_value + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
						{
							error_message = "Уже в списке утвердителей";
							MESSAGE_DEBUG("", "", "user.id(" + new_value + ") already in sow.id(" + sow_id + ") approver list");
						}
						else
						{
							// --- good to go
						}
					}					
					else if(action == "AJAX_addCostCenter")
					{
						if(db->Query("SELECT `id` FROM `cost_centers` WHERE `title`=\"" + new_value + "\" AND `agency_company_id`=\"" + id + "\";"))
						{
							error_message = utf8_to_cp1251(gettext("cost center already exists"));
							MESSAGE_DEBUG("", "", "cost_center with the same name already exists in agency.id(" + id + ")");
						}
						else
						{
							// --- good to go
						}
					}
					else if(action == "AJAX_updateCostCenterToCustomer")
					{
						// --- check that assigning cost center belongs to the same agency as timecard_customer
						if(db->Query("SELECT `id` FROM `timecard_customers` WHERE `id`=\"" + id + "\" AND `agency_company_id`=(SELECT `agency_company_id` FROM `cost_centers` WHERE `id`=\"" + new_value + "\");"))
						{
							// --- good to go
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("cost center doesn't belongs to your company"));
							MESSAGE_DEBUG("", "", "cost center doesn't belongs to your company");
						}
					}
					else if(action == "AJAX_addBTExpenseApproverToSoW")
					{
						if(db->Query("SELECT `id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + new_value + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
						{
							error_message = "Уже в списке утвердителей";
							MESSAGE_DEBUG("", "", "user.id(" + new_value + ") already in sow.id(" + sow_id + ") approver list");
						}
						else
						{
							// --- good to go
						}
					}					
					// --- Expense templates
					else if(action == "AJAX_updateExpenseTemplateTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `bt_expense_templates` WHERE `title`=\"" + new_value + "\" AND `agency_company_id` = ( "
								"SELECT `agency_company_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Расход " + new_value + " уже существует";
							MESSAGE_DEBUG("", "", "BT expense template with same title already exists");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateAgencyComment")		{ /* --- good to go */ }
					// --- Expense line templates
					else if(action == "AJAX_updateExpenseTemplateLineTitle")
					{
						if(db->Query(
							"SELECT `id` FROM `bt_expense_line_templates` WHERE `title`=\"" + new_value + "\" AND `bt_expense_template_id` = ( "
								"SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\""
							");"
						))
						{
							error_message = "Документ с таким названием (" + new_value + ") уже есть в расходе";
							MESSAGE_DEBUG("", "", "BT expense template line with same title already exists belongs to the same expense");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateLineDescription") 		{ /* --- good to go */ }
					else if(action == "AJAX_updateExpenseTemplateLineTooltip") 			{ /* --- good to go */ }
					else if(action == "AJAX_updateExpenseTemplateLineDomType") 
					{
						error_message = "Нельзя поменять тип документа";
						MESSAGE_DEBUG("", "", "dom_type can't be changed");
					}
					else if(action == "AJAX_updateExpenseTemplateLinePaymentCash") 
					{
						if(
							(new_value == "false")
							&&
							db->Query("SELECT `id` FROM `bt_expense_line_templates` WHERE `payment`=\"cash\" AND `id`=\"" + id + "\";")
						)
						{
							error_message = "Необходимо оставить какой-нибудь способ оплаты";
							MESSAGE_DEBUG("", "", "Fail to disable single methid of payment");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateLinePaymentCard")
					{
						if(
							(new_value == "false")
							&&
							db->Query("SELECT `id` FROM `bt_expense_line_templates` WHERE `payment`=\"card\" AND `id`=\"" + id + "\";")
						)
						{
							error_message = "Необходимо оставить какой-нибудь способ оплаты";
							MESSAGE_DEBUG("", "", "Fail to disable single methid of payment");
						}
					}
					else if(action == "AJAX_updateExpenseTemplateLineRequired") 		{ /* --- good to go */ }

					// --- PSoW part
					else if(action == "AJAX_updatePSoWPosition")				{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWAct")						{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWSignDate")				{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_updatePSoWStartDate")
					{
						if(db->Query("SELECT `end_date` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = new_value;
							auto		string_end = db->Get(0, "end_date");
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = utf8_to_cp1251(gettext("period start have to precede period end")) + " (" + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updatePSoWEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = db->Get(0, "start_date");
							auto		string_end = new_value;
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							mktime(&tm_start);
							mktime(&tm_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = utf8_to_cp1251(gettext("period start have to precede period end")) + " (" + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updatePSoWDayRate")
					{
						c_float		num(new_value);

						if(string(num) == new_value) { /* --- good to go */ }
						else
						{
							MESSAGE_ERROR("", "", "input DayRate(" + new_value + ") wrongly formatted, need to be " + string(num));
						}
					}
					else if(action == "AJAX_updatePSoWNumber")
					{
						if(db->Query("SELECT `id` FROM `contracts_psow` WHERE `number`=\"" + new_value + "\" AND `id`!=\"" + sow_id + "\" AND `cost_center_id`=(SELECT `cost_center_id` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\");"))
						{
							error_message = utf8_to_cp1251(gettext("PSoW number already exists"));
							MESSAGE_DEBUG("", "", "psow.number already exists in agency.id");
						}
						else
						{
							// --- good to go
						}
					}

					// --- cost_center part
					else if(action == "AJAX_updateCostCenterAct")						{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterSignDate")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterCustomField")				{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterNumber")					{ /* --- good to go */ }
					else if(action == "AJAX_updateCostCenterStartDate")
					{
						if(db->Query("SELECT `end_date` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = new_value;
							auto		string_end = db->Get(0, "end_date");
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = utf8_to_cp1251(gettext("period start have to precede period end")) + " (" + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}
					else if(action == "AJAX_updateCostCenterEndDate")
					{
						if(db->Query("SELECT `start_date` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";"))
						{
							auto		string_start = db->Get(0, "start_date");
							auto		string_end = new_value;
							auto		tm_start = GetTMObject(string_start);
							auto		tm_end = GetTMObject(string_end);

							mktime(&tm_start);
							mktime(&tm_end);

							if(tm_start <= tm_end)
							{
								// --- good to go
							}
							else
							{
								error_message = utf8_to_cp1251(gettext("period start have to precede period end")) + " (" + string_start + " - " +  string_end + ")";
								MESSAGE_DEBUG("", "", "period start have to precede period end (" + string_start + " - " + string_end + ")");
							}
						}
						else
						{
							error_message = utf8_to_cp1251(gettext("SQL syntax issue"));
							MESSAGE_ERROR("", "", "issue in SQL-syntax");
						}
					}

 					else
					{
						MESSAGE_ERROR("", "", "unknown action(" + action + "), no validity check performed.");
					}

				}
				else
				{
					error_message = "Новое значение не может быть пустым";
					MESSAGE_ERROR("", "", "new_value is empty");
				}
			}
			else
			{
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Неизвестный идентификатор поля";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

static string	isActionEntityBelongsToAgency(string action, string id, string agency_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(agency_id.length())
	{
		if(id.length())
		{
			if(action.length())
			{
				string		sql_query = "";

				if(action == "AJAX_updateCustomerTitle") 				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateProjectTitle") 				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateTaskTitle") 					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=(SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\"));";

				if(action == "AJAX_updateExpenseTemplateTitle")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateAgencyComment")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCash")	sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCard")	sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";

				if(action == "AJAX_updateCostCenterToCustomer")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteCostCenterFromCustomer")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_deleteCostCenter")						sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterTitle")					sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterDescription")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `cost_centers` WHERE `id`=\"" + id + "\";";


				if(action == "AJAX_deleteTask")							sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `timecard_customers` WHERE `id`=(SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id`=(SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\"));";
				if(action == "AJAX_deleteExpenseTemplate")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_addExpenseTemplateLine")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteExpenseTemplateLine")			sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_addBTExpenseTemplateAssignment")		sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_getCompanyInfoBySoWID")				sql_query = "SELECT `agency_company_id` AS `agency_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_deleteEmployee")						sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyPosition")				sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyEditCapability")			sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSoWEditCapability")			sql_query = "SELECT `company_id` AS `agency_id` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTitle")					sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyDescription")			sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyWebSite")				sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyTIN")					sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyAccount")				sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyOGRN")					sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyKPP")					sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyLegalAddress")			sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyMailingAddress")		sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyMailingZipID")			sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyLegalZipID")			sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency
				if(action == "AJAX_updateCompanyBankID")				sql_query = "SELECT \"" + agency_id + "\" AS `agency_id`;"; // --- entity always belongs to agency

				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						string	agency_id_from_db = db->Get(0, "agency_id");

						if(agency_id == agency_id_from_db)
						{
							// --- everything is good
						}
						else
						{
							error_message = "Нет доступа";
							MESSAGE_ERROR("", "", "Probably hacker action(" + action + "), because action(" + action + ") entity.id(" + id + ") doesn't belong to agency.id(" + agency_id + ")");
						}
					}
					else
					{
						error_message = "Нет доступа";
						MESSAGE_ERROR("", "", "action(" + action + ") element.id(" + id + ") doesn't belong to agency.id(" + agency_id + ")");
					}
				}
				else
				{
					error_message = "Неизвестное действие";
					MESSAGE_ERROR("", "", "unknown action (" + action + ")");
				}
			}
			else
			{
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Идентификатор поля не задан";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}
	else
	{
		error_message = "Неизвестное агенство";
		MESSAGE_ERROR("", "", "agency_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

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

		/*
			ostringstream		query, ost1, ost2;
			string				partNum;
			string				content;
			// Menu				m;

			indexPage.RegisterVariableForce("rand", GetRandom(10));
			indexPage.RegisterVariableForce("random", GetRandom(10));
			indexPage.RegisterVariableForce("style", "style.css");

			// --- Generate session
			{
				string			lng, sessidHTTP;
				ostringstream	ost;


				sessidHTTP = indexPage.SessID_Get_FromHTTP();
				if(sessidHTTP.length() < 5)
				{
					{
						MESSAGE_DEBUG("", action, "session cookie is not exist, generating new session.");
					}
					sessidHTTP = indexPage.SessID_Create_HTTP_DB();
					if(sessidHTTP.length() < 5)
					{
						MESSAGE_ERROR("", action, "error in generating session ID");
						throw CExceptionHTML("session can't be created");
					}
				}
				else {
					if(indexPage.SessID_Load_FromDB(sessidHTTP))
					{
						if(indexPage.SessID_CheckConsistency())
						{
							if(indexPage.SessID_Update_HTTP_DB())
							{
								indexPage.RegisterVariableForce("loginUser", "");
								indexPage.RegisterVariableForce("loginUserID", "");

								user.SetDB(&db);
								if(indexPage.SessID_Get_UserFromDB() != "Guest")
								{
									// --- place 2change user from user.email to user.id
									if(user.GetFromDBbyEmail(indexPage.SessID_Get_UserFromDB()))
									{
										ostringstream	ost1;
										string			avatarPath;

										indexPage.RegisterVariableForce("loginUser", indexPage.SessID_Get_UserFromDB());
										indexPage.RegisterVariableForce("loginUserID", user.GetID());
										indexPage.RegisterVariableForce("myLogin", user.GetLogin());
										indexPage.RegisterVariableForce("myFirstName", user.GetName());
										indexPage.RegisterVariableForce("myLastName", user.GetNameLast());

										// --- Get user avatars
										ost1.str("");
										ost1 << "select * from `users_avatars` where `userid`='" << user.GetID() << "' and `isActive`='1';";
										avatarPath = "empty";
										if(db.Query(ost1.str()))
										{
											ost1.str("");
											ost1 << "/images/avatars/avatars" << db.Get(0, "folder") << "/" << db.Get(0, "filename");
											avatarPath = ost1.str();
										}
										indexPage.RegisterVariableForce("myUserAvatar", avatarPath);

										{
											MESSAGE_DEBUG("", "", "user [" + user.GetLogin() + "] logged in");
										}
									}
									else
									{
										// --- enforce to close session
										action = "logout";

										{
											MESSAGE_DEBUG("", "", "user [" + indexPage.SessID_Get_UserFromDB() + "] session exists on client device, but not in the DB. Change the [action = logout]");
										}

									}
								}
								else
								{
									{
										MESSAGE_DEBUG("", action, "user [" + user.GetLogin() + "] surfing");
									}

									if(user.GetFromDBbyLogin(user.GetLogin()))
									{
										indexPage.RegisterVariableForce("loginUser", user.GetLogin());
										indexPage.RegisterVariableForce("loginUserID", user.GetID());
										indexPage.RegisterVariableForce("myLogin", user.GetLogin());
										indexPage.RegisterVariableForce("myFirstName", user.GetName());
										indexPage.RegisterVariableForce("myLastName", user.GetNameLast());
									}
									else
									{
										// --- enforce to close session
										action = "logout";

										{
											MESSAGE_DEBUG("", action, "user [" + indexPage.SessID_Get_UserFromDB() + "] session exists on client device, but not in the DB. Change the [action = logout].");
										}

									}

								}


							}
							else
							{
								MESSAGE_ERROR("", action, "error update session in HTTP or DB failed");
							}
						}
						else {
							MESSAGE_ERROR("", action, "error session consistency check failed");
						}
					}
					else
					{
						ostringstream	ost;

						{
							MESSAGE_DEBUG("", action, "cookie session and DB session is not equal. Need to recreate session");
						}

						ost.str("");
						ost << "/?rand=" << GetRandom(10);

						if(!indexPage.Cookie_Expire()) {
							MESSAGE_ERROR("", action, "error in session expiration");
						} // --- if(!indexPage.Cookie_Expire())
						indexPage.Redirect(ost.str());
					} // --- if(indexPage.SessID_Load_FromDB(sessidHTTP))
				} // --- if(sessidHTTP.length() < 5)
			}
	//------- End generate session


	//------- Register html META-tags
			if(db.Query("select `value` from `settings_default` where `setting`=\"keywords_head\"") > 0)
				indexPage.RegisterVariable("keywords_head", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting keywords_head from settings_default");
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"title_head\"") > 0)
				indexPage.RegisterVariable("title_head", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting title_head from settings_default");
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"description_head\"") > 0)
				indexPage.RegisterVariable("description_head", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting description_head from settings_default");
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"NewsOnSinglePage\"") > 0)
				indexPage.RegisterVariable("NewsOnSinglePage", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting NewsOnSinglePage from settings_default");

				indexPage.RegisterVariable("NewsOnSinglePage", to_string(NEWS_ON_SINGLE_PAGE));
			}

			if(db.Query("select `value` from `settings_default` where `setting`=\"FriendsOnSinglePage\"") > 0)
				indexPage.RegisterVariable("FriendsOnSinglePage", db.Get(0, "value"));
			else
			{
				MESSAGE_ERROR("", action, "error getting FriendsOnSinglePage from settings_default");

				indexPage.RegisterVariable("FriendsOnSinglePage", to_string(FRIENDS_ON_SINGLE_PAGE));
			}

			if(action.empty() and user.GetLogin().length() and (user.GetLogin() != "Guest"))
			{
				action = GetDefaultActionFromUserType(user.GetType(), &db);

				{
					MESSAGE_DEBUG("", action, "META-registration: 'logged-in user' action has been overriden to [" + action + "]");
				}

			}
			else if(!action.length())
			{
				action = GUEST_USER_DEFAULT_ACTION;

				{
					MESSAGE_DEBUG("", action, "META-registration: 'guest user' action has been overriden to [" + action + "]");
				} // --- log block
			} // --- default action
	//------- End register html META-tags
		*/
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

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}

			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getApprovalsList")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			object = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("object"));;
				string			filter = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("filter"));;

				if((object == "timecard") || (object == "bt"))
				{
					if((user.GetType() == "agency") || (user.GetType() == "approver"))
					{
						string	temp = GetObjectsSOW_Reusable_InJSONFormat(object, filter, &db, &user);

						if(temp.length())
						{
							ostResult	<< "{"
										<< "\"result\":\"success\","
										<< temp
										<< "}";
						}
						else
						{
							MESSAGE_ERROR("", action, "error get reusable data");
							ostResult << "{\"result\":\"error\",\"description\":\"Ошибка получения данных\"}";
						}

					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") neither approver, nor agency");
						ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны согласовывать\"}";
					}
				}
				else
				{
					ostResult << "{\"result\":\"error\",\"description\":\"Неизвестный тип обьекта\"}";
					MESSAGE_ERROR("", action, "unknown object type (" + object + ")");
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getTimecardEntry")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			timecard_id = "";

				timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));

				if(timecard_id.length())
				{
					if(user.GetType() == "agency")
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN (SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\"))"))
						{
							ostResult << "{"
											"\"result\":\"success\","
											"\"timecards\":[" << GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";", &db, &user, true) << "]"
										"}";
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
					else if(user.GetType() == "approver")
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\" AND `contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
						{
							ostResult << "{"
											"\"result\":\"success\","
											"\"timecards\":[" << GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";", &db, &user, true) << "]"
										"}";
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an approver(" + user.GetType() + ")");
						ostResult << "{\"result\":\"error\",\"description\":\"Вам не назначена роль согласования\"}";
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "parameter timecard_id is empty");
					ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
				}


				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_approveTimecard")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			timecard_id = "";
				string			comment = "";

				timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(timecard_id.length())
				{
					{
						// --- REPEAT #2 
	/*					if(db.Query("SELECT `id` FROM `timecards` WHERE "
										"`id`=\"" + timecard_id + "\" "
										"AND "
										"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
	*/
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
										"`approver_user_id`=\"" + user.GetID() + "\" "
										"AND "
										"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
										";"))
						{
							string		approver_id = db.Get(0, "id");

							if(db.Query("SELECT `status`, `submit_date` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								string	timecard_state = db.Get(0, "status");
								string	timecard_submit_date = db.Get(0, "submit_date");

								if(timecard_state == "submit")
								{
									{
										// --- did approver approved after user submission ?
										if(db.Query("SELECT `eventTimestamp` FROM `timecard_approvals` WHERE `approver_id`=\"" + approver_id + "\" AND `timecard_id`=\"" + timecard_id + "\" AND `eventTimestamp`>\"" + timecard_submit_date + "\";"))
										{
											// --- this approver already approved this bt
											ostResult << "{\"result\":\"error\",\"description\":\"Вы уже подтвердили\"}";
											MESSAGE_ERROR("", action, "approver.id(" + approver_id + ") already approved timecard.id(" + timecard_id + ") at timestamp(" + db.Get(0, "eventTimestamp") + ")");
										}
										else
										{
											long int	temp;

											temp = db.InsertQuery(	"INSERT INTO `timecard_approvals` SET "
																	"`timecard_id`=\"" + timecard_id + "\","
																	"`approver_id`=\"" + approver_id + "\","
																	"`decision`=\"approved\","
																	"`comment`=\"" + comment + "\","
																	"`eventTimestamp`=UNIX_TIMESTAMP();"
																);

											if(temp)
											{
												if(SubmitTimecard(timecard_id, &db, &user))
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("timecard", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending timecards");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка подтверждения таймкарты\"}";
													MESSAGE_ERROR("", action, "fail to submit timecard_id(" + timecard_id + "): ");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
												MESSAGE_ERROR("", action, "fail to insert to timecards table with timecard_id(" + timecard_id + ")");
											}
										}
										
									}
	/*								else
									{
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an timecard.id(" + timecard_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать таймкарту у данного договора\"}";
									}
	*/
								}
								else
								{
									MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") have to be in submit state to be approved/rejected, but it is in \"" + timecard_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"таймкарта не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about timecard.id(" + timecard_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter timecard_id is empty");
					ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
				}


				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_rejectTimecard")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			timecard_id = "";
				string			comment = "";

				timecard_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("timecard_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(timecard_id.length())
				{
					if(comment.empty())
					{
						MESSAGE_ERROR("", action, "reject reason is empty");
						ostResult << "{\"result\":\"error\",\"description\":\"Необходимо обосновать отказ\"}";
					}
					else
					{
						// --- check ability to see this timecard
						if(db.Query("SELECT `id` FROM `timecards` WHERE "
										"`id`=\"" + timecard_id + "\" "
										"AND "
										"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user.GetID() + "\")"))
						{

							if(db.Query("SELECT `status` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
							{
								string	timecard_state = db.Get(0, "status");

								if(timecard_state == "submit")
								{

									if(db.Query("SELECT `id` FROM `timecard_approvers` WHERE "
													"`approver_user_id`=\"" + user.GetID() + "\" "
													"AND "
													"`contract_sow_id`=( SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
													";"))
									{
										string		approver_id = db.Get(0, "id");
										long int	temp;

										temp = db.InsertQuery(	"INSERT INTO `timecard_approvals` SET "
																"`timecard_id`=\"" + timecard_id + "\","
																"`approver_id`=\"" + approver_id + "\","
																"`decision`=\"rejected\","
																"`comment`=\"" + comment + "\","
																"`eventTimestamp`=UNIX_TIMESTAMP();"
															);

										if(temp)
										{
											db.Query("UPDATE `timecards` SET `status`=\"rejected\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
											if(!db.isError())
											{
												string	success_description = GetObjectsSOW_Reusable_InJSONFormat("timecard", "submit", &db, &user);

												if(success_description.length())
												{
													ostResult	<< "{"
																<< "\"result\":\"success\","
																<< success_description
																<< "}";
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
													MESSAGE_ERROR("", action, "error getting info about pending timecards");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
												MESSAGE_ERROR("", action, "fail to update timecard_id(" + timecard_id + "): " + db.GetErrorMessage());
											}
										}
										else
										{
											ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
											MESSAGE_ERROR("", action, "fail to insert to timecards table with timecard_id(" + timecard_id + ")");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an timecard.id(" + timecard_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать таймкарту у данного договора\"}";
									}

								}
								else
								{
									MESSAGE_ERROR("", action, "timecard.id(" + timecard_id + ") have to be in submit state to be approved/rejected, but it is in \"" + timecard_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"таймкарта не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about timecard.id(" + timecard_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this timecard");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter timecard_id is empty");
					ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
				}


				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_rejectBT")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			bt_id = "";
				string			comment = "";

				bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(bt_id.length())
				{
					if(comment.empty())
					{
						MESSAGE_ERROR("", action, "reject reason is empty");
						ostResult << "{\"result\":\"error\",\"description\":\"Необходимо обосновать отказ\"}";
					}
					else
					{
						if((user.GetType() == "approver") || (user.GetType() == "agency"))
						{
							// --- check ability to see this bt
							string	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
							if(error_string.empty())
							{

								if(db.Query("SELECT `status` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
								{
									string	bt_state = db.Get(0, "status");

									if(bt_state == "submit")
									{

										if(db.Query("SELECT `id` FROM `bt_approvers` WHERE "
														"`approver_user_id`=\"" + user.GetID() + "\" "
														"AND "
														"`contract_sow_id`=( SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
														";"))
										{
											string		approver_id = db.Get(0, "id");
											long int	temp;

											temp = db.InsertQuery(	"INSERT INTO `bt_approvals` SET "
																	"`bt_id`=\"" + bt_id + "\","
																	"`approver_id`=\"" + approver_id + "\","
																	"`decision`=\"rejected\","
																	"`comment`=\"" + comment + "\","
																	"`eventTimestamp`=UNIX_TIMESTAMP();"
																);

											if(temp)
											{
												db.Query("UPDATE `bt` SET `status`=\"rejected\", `eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + bt_id + "\";");
												if(!db.isError())
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending bt");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
													MESSAGE_ERROR("", action, "fail to update bt_id(" + bt_id + "): " + db.GetErrorMessage());
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
												MESSAGE_ERROR("", action, "fail to insert to bt table with bt_id(" + bt_id + ")");
											}
										}
										else
										{
											MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
											ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать командировку у данного договора\"}";
										}

									}
									else
									{
										MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
										ostResult << "{\"result\":\"error\",\"description\":\"командировка не требует подтверждения\"}";
									}
								}
								else
								{
									MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
									ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
								ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой командировке\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
					ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
				}


				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_approveBT")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			bt_id = "";
				string			comment = "";

				bt_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("bt_id"));
				comment = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("comment"));

				if(bt_id.length())
				{
					if((user.GetType() == "approver") || (user.GetType() == "agency"))
					{
						// --- check ability to see this bt
						string	error_string = isUserAllowedAccessToBT(bt_id, &db, &user);
						if(error_string.empty())
						{

							if(db.Query("SELECT `status`, `submit_date` FROM `bt` WHERE `id`=\"" + bt_id + "\";"))
							{
								string	bt_state = db.Get(0, "status");
								string	bt_submit_date = db.Get(0, "submit_date");

								if(bt_state == "submit")
								{

									// --- find approver_id for user and sow
									if(db.Query("SELECT `id` FROM `bt_approvers` WHERE "
													"`approver_user_id`=\"" + user.GetID() + "\" "
													"AND "
													"`contract_sow_id`=( SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
													";"))
									{
										string		approver_id = db.Get(0, "id");

										// --- did approver approved after user submission ?
										if(db.Query("SELECT `eventTimestamp` FROM `bt_approvals` WHERE `approver_id`=\"" + approver_id + "\" AND `bt_id`=\"" + bt_id + "\" AND `eventTimestamp`>\"" + bt_submit_date + "\";"))
										{
											// --- this approver already approved this bt
											ostResult << "{\"result\":\"error\",\"description\":\"Вы уже подтвердили\"}";
											MESSAGE_ERROR("", action, "approver.id(" + approver_id + ") already approved bt.id(" + bt_id + ") at timestamp(" + db.Get(0, "eventTimestamp") + ")");
										}
										else
										{
											long int	temp;

											temp = db.InsertQuery(	"INSERT INTO `bt_approvals` SET "
																	"`bt_id`=\"" + bt_id + "\","
																	"`approver_id`=\"" + approver_id + "\","
																	"`decision`=\"approved\","
																	"`comment`=\"" + comment + "\","
																	"`eventTimestamp`=UNIX_TIMESTAMP();"
																);

											if(temp)
											{
												if(SubmitBT(bt_id, &db, &user))
												{
													string	success_description = GetObjectsSOW_Reusable_InJSONFormat("bt", "submit", &db, &user);

													if(success_description.length())
													{
														ostResult	<< "{"
																	<< "\"result\":\"success\","
																	<< success_description
																	<< "}";
													}
													else
													{
														ostResult << "{\"result\":\"error\",\"description\":\"ошибка БД\"}";
														MESSAGE_ERROR("", action, "error getting info about pending bt");
													}
												}
												else
												{
													ostResult << "{\"result\":\"error\",\"description\":\"ошибка подтверждения таймкарты\"}";
													MESSAGE_ERROR("", action, "fail to submit bt_id(" + bt_id + "): ");
												}
											}
											else
											{
												ostResult << "{\"result\":\"error\",\"description\":\"ошибка обновления списка апруверов\"}";
												MESSAGE_ERROR("", action, "fail to insert to bt table with bt_id(" + bt_id + ")");
											}
										}

									}
									else
									{
										MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") not an bt.id(" + bt_id + ") approver");
										ostResult << "{\"result\":\"error\",\"description\":\"Вы не должны подтверждать таймкарту у данного договора\"}";
									}

								}
								else
								{
									MESSAGE_ERROR("", action, "bt.id(" + bt_id + ") have to be in submit state to be approved/rejected, but it is in \"" + bt_state + "\" state");
									ostResult << "{\"result\":\"error\",\"description\":\"таймкарта не требует подтверждения\"}";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "issue get info about bt.id(" + bt_id + ") from DB");
								ostResult << "{\"result\":\"error\",\"description\":\"Ошибка ДБ\"}";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't allow to see this bt");
							ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is " + user.GetType() + ", who can't approve/reject");
						ostResult << "{\"result\":\"error\",\"description\":\"У Вас нет доступа к этой таймкарте\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "parameter bt_id is empty");
					ostResult << "{\"result\":\"error\",\"description\":\"Некорректые параметры\"}";
				}


				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				} // if(!indexPage.SetTemplate("my_network.htmlt"))
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getDashboardPendingData")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			{
				MESSAGE_DEBUG("", action, "start");
			}

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_pending_timecards\":" << GetNumberOfTimecardsInPendingState(&db, &user) << ","
										"\"number_of_pending_bt\":" << GetNumberOfBTInPendingState(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}


			{
				MESSAGE_DEBUG("", action, "finish");
			}
		}

		if(action == "AJAX_getDashboardSubmitTimecardsThisMonth")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_submit_timecards\":" << GetNumberOfApprovedTimecardsThisMonth(&db, &user) << ","
										"\"total_number_of_sow\":" << GetNumberOfSoWActiveThisMonth(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getDashboardSubmitTimecardsLastMonth")
		{
			string			strPageToGet, strFriendsOnSinglePage;
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string		template_name = "json_response.htmlt";
				int			affected = db.Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\";");

				if(affected)
				{
					string		company_id= db.Get(0, "company_id");
					bool		successFlag = true;
					string		pending_timecards = "";

					if(successFlag)
					{
						ostResult << "{"
										"\"result\":\"success\","
										"\"number_of_submit_timecards\":" << GetNumberOfApprovedTimecardsLastMonth(&db, &user) << ","
										"\"total_number_of_sow\":" << GetNumberOfSoWActiveLastMonth(&db, &user) << ""
									"}";
					}
					else
					{
						MESSAGE_ERROR("", action, "dashboard data not gathered completely");
						ostResult << "{\"result\":\"error\",\"description\":\"Ошибка построения панели управления\"}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") doesn't owns company");
					ostResult << "{\"result\":\"error\",\"description\":\"Вы не работаете ни в каком агенстве\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name))
				{
					MESSAGE_ERROR("", action, "can't find template " + template_name);
				}
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getSoWList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				bool			include_tasks 			= indexPage.GetVarsHandler()->Get("include_tasks") == "true";
				bool			include_bt 				= indexPage.GetVarsHandler()->Get("include_bt") == "true";
				bool			include_cost_centers 	= indexPage.GetVarsHandler()->Get("include_cost_centers") == "true";
				string			sow_id 					= indexPage.GetVarsHandler()->Get("sow_id");

				if(sow_id.length())	sow_id = CheckHTTPParam_Number(sow_id);

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
					{
						string		agency_id = db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"sow\":[" << GetSOWInJSONFormat(
												"SELECT * FROM `contracts_sow` WHERE "
													+ (sow_id.length() ? string("`id`=\"" + sow_id + "\" AND ") : "") +
													"`agency_company_id`=\"" + agency_id + "\" "
												";", &db, &user, include_tasks, include_bt, include_cost_centers) << "]";

						if(include_tasks)
							ostResult <<	","
											"\"task_assignments\":[" << GetTimecardTaskAssignmentInJSONFormat(
													"SELECT * FROM `timecard_task_assignment` WHERE "
														+ (sow_id.length() ? string("`contract_sow_id`=\"" + sow_id + "\" AND ") : "") +
														"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
															"`agency_company_id`=\"" + agency_id + "\" "
													");", &db, &user) << "]";
						if(include_bt)
							ostResult <<	","
											"\"bt_expense_assignments\":[" << GetBTExpenseAssignmentInJSONFormat(
													"SELECT * FROM `bt_sow_assignment` WHERE "
														+ (sow_id.length() ? string("`sow_id`=\"" + sow_id + "\" AND ") : "") +
														"`sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE "
															"`agency_company_id`=\"" + agency_id + "\" "
													");", &db, &user) << "]";
						ostResult << "}";
					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an agency employee");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getCostCenterList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				if(user.GetType() == "agency")
				{
					{
						string		agency_id = db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"cost_centers\":[" << GetCostCentersInJSONFormat(
												"SELECT * FROM `cost_centers` WHERE `agency_company_id`=("
													"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
														"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\""
													")"
												");", &db, &user) << "]";
						ostResult << "}";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getApprovedTimecardList")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");

			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			cost_center_id = indexPage.GetVarsHandler()->Get("cost_center_id");

				if(user.GetType() == "agency")
				{
					if(isCostCenterBelongsToAgency(cost_center_id, &db, &user))
					{
						string		agency_id = db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"timecards\":[" << GetTimecardsInJSONFormat(
												"SELECT * FROM `timecards` WHERE `status`=\"approved\" AND `contract_sow_id` IN ("
													"SELECT `contract_sow_id` FROM `contracts_psow` WHERE `cost_center_id`=\"" + cost_center_id + "\""
												");", &db, &user) << "]";
						ostResult << "}";
					}
					else
					{
						MESSAGE_ERROR("", action, "cost_center.id(" + cost_center_id + ") doesn't belongs to agance user(" + user.GetID() + ") employeed");
						error_message = "Информация доступна только для агенства";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getBTExpenseTemplates")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			sow_id 			= indexPage.GetVarsHandler()->Get("sow_id");

				if(sow_id.length())	sow_id = CheckHTTPParam_Number(sow_id);

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
					{
						string		agency_id = db.Get(0, "id");

						ostResult << "{"
										"\"result\":\"success\","
										"\"bt_expense_templates\":[" << GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `agency_company_id`=\"" + agency_id + "\";", &db, &user) << "]";
						ostResult << "}";
					}
					else
					{
						error_message = "Агенство не найдено";
						MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an agency employee");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(
			(action == "AJAX_updateSoWNumber")					||
			(action == "AJAX_updateSoWAct")						||
			(action == "AJAX_updateSoWPosition")				||
			(action == "AJAX_updateSoWDayRate")					||
			(action == "AJAX_updateSoWSignDate")				||
			(action == "AJAX_updateSoWStartDate")				||
			(action == "AJAX_updateSoWEndDate")					||
			(action == "AJAX_updateSoWCustomField")				||

			(action == "AJAX_updatePSoWNumber")					||
			(action == "AJAX_updatePSoWAct")					||
			(action == "AJAX_updatePSoWPosition")				||
			(action == "AJAX_updatePSoWDayRate")				||
			(action == "AJAX_updatePSoWSignDate")				||
			(action == "AJAX_updatePSoWStartDate")				||
			(action == "AJAX_updatePSoWEndDate")				||
			(action == "AJAX_updatePSoWCustomField")			||

			(action == "AJAX_updateCostCenterNumber")			||
			(action == "AJAX_updateCostCenterAct")				||
			(action == "AJAX_updateCostCenterSignDate")			||
			(action == "AJAX_updateCostCenterStartDate")		||
			(action == "AJAX_updateCostCenterEndDate")			||
			(action == "AJAX_updateCostCenterCustomField")		||

			(action == "AJAX_updatePeriodStart")				||
			(action == "AJAX_updatePeriodEnd")					||
			(action == "AJAX_updateSubcontractorCreateTasks")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
				string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(new_value.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						error_message = isActionEntityBelongsToSoW(action, id, sow_id, &db, &user);
						if(error_message.empty())
						{
							error_message = CheckNewValueByAction(action, id, sow_id, new_value, &db, &user);
							if(error_message.empty())
							{
								if(action.find("update"))
								{
									string		existing_value = GetDBValueByAction(action, id, sow_id, &db, &user);

									error_message = SetNewValueByAction(action, id, sow_id, new_value, &db, &user);
									if(error_message.empty())
									{
										ostResult << "{\"result\":\"success\"}";

										if(NotifySoWContractPartiesAboutChanges(action, id, sow_id, existing_value, new_value, &db, &user))
										{
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/id/value = " + action + "/" + sow_id + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
									}
								}
								else if(action.find("insert"))
								{
									
								}
								else if(action.find("delete"))
								{

								}
								else
								{
									MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
								}

							}
							else
							{
								MESSAGE_DEBUG("", action, "new value failed to pass validity check");
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belong to sow.id(" + sow_id + ")");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
					}
				}
				else
				{
					error_message = "Поле не должно быть пустым";
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new value in sow.id(" + sow_id + ")");
				}

				if(error_message.empty())
				{
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(
			(action == "AJAX_updateCustomerTitle")					||
			(action == "AJAX_updateProjectTitle")					||
			(action == "AJAX_updateTaskTitle")						|| 
			(action == "AJAX_updateCompanyTitle")					||
			(action == "AJAX_updateCompanyDescription")				||
			(action == "AJAX_updateCompanyWebSite")					||
			(action == "AJAX_updateCompanyTIN")						||
			(action == "AJAX_updateCompanyAccount")					||
			(action == "AJAX_updateCompanyOGRN")					||
			(action == "AJAX_updateCompanyKPP")						||
			(action == "AJAX_updateCompanyLegalAddress")			||
			(action == "AJAX_updateCompanyMailingAddress")			||
			(action == "AJAX_updateAgencyPosition")					||
			(action == "AJAX_updateAgencyEditCapability")			||
			(action == "AJAX_updateSoWEditCapability")				||
			(action == "AJAX_updateCompanyMailingZipID")			||
			(action == "AJAX_updateCompanyLegalZipID")				||
			(action == "AJAX_updateCompanyBankID")					||
			(action == "AJAX_updateExpenseTemplateTitle")			||
			(action == "AJAX_updateExpenseTemplateAgencyComment")	||
			(action == "AJAX_updateExpenseTemplateLineTitle")		||
			(action == "AJAX_updateExpenseTemplateLineDescription")	||	
			(action == "AJAX_updateExpenseTemplateLineTooltip")		||
			(action == "AJAX_updateExpenseTemplateLineDomType")		||
			(action == "AJAX_updateExpenseTemplateLinePaymentCash")	||	
			(action == "AJAX_updateExpenseTemplateLinePaymentCard")	||	
			(action == "AJAX_updateExpenseTemplateLineRequired")	||
			(action == "AJAX_updateCostCenterToCustomer")			||
			(action == "AJAX_deleteCostCenterFromCustomer")			||
			(action == "AJAX_deleteCostCenter")						||
			(action == "AJAX_updateCostCenterTitle")				||
			(action == "AJAX_updateCostCenterDescription")
		)
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			id				= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
				string			new_value		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("value"));

				if(new_value.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);
						if(agency_id.length())
						{
							if(action == "AJAX_updateCompanyTitle") 		{	id = agency_id; }
							if(action == "AJAX_updateCompanyDescription")	{	id = agency_id; }
							if(action == "AJAX_updateCompanyWebSite")		{	id = agency_id; }
							if(action == "AJAX_updateCompanyTIN")			{	id = agency_id; }
							if(action == "AJAX_updateCompanyAccount")		{	id = agency_id; }
							if(action == "AJAX_updateCompanyOGRN")			{	id = agency_id; }
							if(action == "AJAX_updateCompanyKPP")			{	id = agency_id; }
							if(action == "AJAX_updateCompanyLegalAddress") 	{	id = agency_id; }
							if(action == "AJAX_updateCompanyMailingAddress"){	id = agency_id; }
							if(action == "AJAX_updateCompanyMailingZipID") 	{	new_value = id; id = agency_id; }
							if(action == "AJAX_updateCompanyLegalZipID") 	{	new_value = id; id = agency_id; }
							if(action == "AJAX_updateCompanyBankID") 		{	new_value = id; id = agency_id; }

							error_message = isActionEntityBelongsToAgency(action, id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = CheckNewValueByAction(action, id, "", new_value, &db, &user);
								if(error_message.empty())
								{
									if((action.find("update") != string::npos))
									{
										string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

										error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
										if(error_message.empty())
										{
											// --- good finish

											if(NotifySoWContractPartiesAboutChanges(action, id, "", existing_value, new_value, &db, &user))
											{
											}
											else
											{
												MESSAGE_DEBUG("", action, "fail to notify agency about changes");
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
										}
									}
									else if(action.find("insert") != string::npos)
									{
										
									}
									else if(action.find("delete") != string::npos)
									{
										string		existing_value = GetDBValueByAction(action, id, "", &db, &user);

										if(NotifySoWContractPartiesAboutChanges(action, id, "", existing_value, new_value, &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", action, "fail to notify agency about changes");
										}

										error_message = SetNewValueByAction(action, id, "", new_value, &db, &user);
										if(error_message.empty())
										{
											// --- good finish
										}
										else
										{
											MESSAGE_DEBUG("", action, "unable to set new value (action/id/value = " + action + "/" + id + "/[" + FilterCP1251Symbols(new_value) + "])");
										}
									}
									else
									{
										MESSAGE_ERROR("", action, "unsupported action type(" + action + ")");
									}

								}
								else
								{
									MESSAGE_DEBUG("", action, "new value failed to pass validity check");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							error_message = "Агенство не найдено";
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					error_message = "Поле не должно быть пустым";
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") didn't set new_value");
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

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addTaskAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			customer 		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer"));
				string			project			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project"));
				string			task			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task"));
				string			period_start	= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("period_start"));
				string			period_end		= CheckHTTPParam_Date(indexPage.GetVarsHandler()->Get("period_end"));
				string			sow_id 			= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(customer.length() && project.length() && task.length() && period_start.length() && period_end.length() && sow_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						if(db.Query("SELECT `agency_company_id`,`start_date`,`end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							string agency_id				= db.Get(0, "agency_company_id");
							struct tm sow_start				= GetTMObject(db.Get(0, "start_date"));
							struct tm sow_end				= GetTMObject(db.Get(0, "end_date"));
							struct tm new_assignemt_start	= GetTMObject(period_start);
							struct tm new_assignemt_end		= GetTMObject(period_end);

							if(difftime(mktime(&new_assignemt_start), mktime(&sow_start)) >= 0)
							{
								if(difftime(mktime(&sow_end), mktime(&new_assignemt_end)) >= 0)
								{
									if(difftime(mktime(&new_assignemt_end), mktime(&new_assignemt_start)) >= 0)
									{
										string	task_assignment_id = GetTaskAssignmentID(customer, project, task, sow_id, &db);

										if(task_assignment_id.empty())
										{
											string	assignment_start = to_string(1900 + new_assignemt_start.tm_year) + "-" + to_string(1 + new_assignemt_start.tm_mon) + "-" + to_string(new_assignemt_start.tm_mday);
											string	assignment_end = to_string(1900 + new_assignemt_end.tm_year) + "-" + to_string(1 + new_assignemt_end.tm_mon) + "-" + to_string(new_assignemt_end.tm_mday);
											string	task_id = GetTaskIDFromAgency(customer, project, task, agency_id, &db);
											bool	notify_about_task_creation = false;

											if(task_id.empty())
											{
												task_id = CreateTaskBelongsToAgency(customer, project, task, agency_id, &db);
												notify_about_task_creation = true;
											}
											else
											{
												MESSAGE_DEBUG("", action, "Customer/Project/Task already exists for this agency.id(" + agency_id + ")");
											}

											task_assignment_id = CreateTaskAssignment(task_id, sow_id, assignment_start, assignment_end, &db, &user);

											if(task_assignment_id.length())
											{
												ostResult << "{\"result\":\"success\"}";

												if(notify_about_task_creation)
												{
													if(NotifySoWContractPartiesAboutChanges("AJAX_addTask", task_id, sow_id, "", task, &db, &user))
													{
													}
													else
													{
														MESSAGE_ERROR("", "", "fail to notify SoW parties");
													}
												}
												else
												{
													MESSAGE_DEBUG("", "", "no notification about task creation");
												}
												if(NotifySoWContractPartiesAboutChanges(action, task_assignment_id, sow_id, "", customer + " / " + project + " / " + task + " ( с " + assignment_start + " по " + assignment_end + ")", &db, &user))
												{
												}
												else
												{
													MESSAGE_ERROR("", "", "fail to notify SoW parties");
												}
											}
											else
											{
												MESSAGE_DEBUG("", action, "fail to create assignment sow.id(" + sow_id + ") task.id(" + task_id + ")");
												error_message = "Неудалось создать назначение";
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "assignment Customer/Project/Task already exists for this sow.id(" + sow_id + ")");
											error_message = "Такое назначение уже существует";
										}
										
									}
									else
									{
										MESSAGE_DEBUG("", action, "new assignment finish(" + period_end + ")  earlier than start(" + period_start + ")");
										error_message = "Начало назначения должно быть раньше, чем окончание";
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "new assignment end " + period_end + " later than sow.id(" + sow_id + ") end");
									error_message = "Новое назначение не должно заканчиваться позднее контракта";
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "new assignment start " + period_start + " earlier than sow.id(" + sow_id + ") start");
								error_message = "Новое назначение не должно начинаться раньше контракта";
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "sow.id" + sow_id + " not found");
							error_message = "Контракт не найден";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Некоторые параметры не заданы";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addBTExpenseTemplateAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			bt_expense_templates_param	= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("bt_expense_templates"));
				string			sow_id 						= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(split(bt_expense_templates_param, ',').size() && sow_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
					if(error_message.empty())
					{
						if(db.Query("SELECT `agency_company_id`,`start_date`,`end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
						{
							string agency_id				= db.Get(0, "agency_company_id");

							error_message = isActionEntityBelongsToAgency(action, sow_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								vector<string>	new_bt_expense_template_id_list = split(bt_expense_templates_param, ',');
								
								for(auto new_bt_expense_template_id: new_bt_expense_template_id_list)
								{
									string	bt_expense_template_sow_assignment_id = GetBTExpenseTemplateAssignmentToSoW(new_bt_expense_template_id, sow_id, &db);

									if(bt_expense_template_sow_assignment_id.empty())
									{
										bt_expense_template_sow_assignment_id = CreateBTExpenseTemplateAssignmentToSoW(new_bt_expense_template_id, sow_id, &db, &user);

										if(bt_expense_template_sow_assignment_id.length())
										{
											if(NotifySoWContractPartiesAboutChanges(action, bt_expense_template_sow_assignment_id, sow_id, "", "", &db, &user))
											{
											}
											else
											{
												MESSAGE_ERROR("", "", "fail to notify SoW parties");
											}
										}
										else
										{
											MESSAGE_DEBUG("", action, "fail to create assignment sow.id(" + sow_id + ") new_bt_expense_template.id(" + new_bt_expense_template_id + ")");
											error_message = "Неудалось создать назначение";
											break;
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "assignment BTExpenseTemplate(" + new_bt_expense_template_id + ") already assigned to sow.id(" + sow_id + ")");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "sow.id" + sow_id + " not found");
							error_message = "Контракт не найден";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Некоторые параметры не заданы";
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
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

			MESSAGE_DEBUG("", action, "finish");
		}





		if(action == "AJAX_addTask")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			customer 		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("customer"));
				string			project			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("project"));
				string			task			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("task"));

				if(customer.length() && project.length() && task.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							string	task_id = GetTaskIDFromAgency(customer, project, task, agency_id, &db);

							if(task_id.empty())
							{
								task_id = CreateTaskBelongsToAgency(customer, project, task, agency_id, &db);

								ostResult << "{\"result\":\"success\"}";

								if(NotifySoWContractPartiesAboutChanges(action, task_id, "", "", task, &db, &user))
								{
								}
								else
								{
									MESSAGE_ERROR("", "", "fail to notify agency");
								}
							}
							else
							{
								error_message = "Задача (" + task + ") уже существует у заказчика " + customer + " в проекте " + project;
								MESSAGE_DEBUG("", action, "Customer/Project/Task already exists for this agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Некоторые параметры не заданы";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addExpenseTemplate")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				auto			http_params = indexPage.GetVarsHandler();
				auto			expense_random_name = ""s;
				regex			regex1("bt_expense_template_line_random_");

				C_ExpenseTemplate	expense_template;

				expense_template.SetID			(CheckHTTPParam_Text(http_params->Get("bt_expense_template_id")));
				expense_template.SetTitle		(CheckHTTPParam_Text(http_params->Get("bt_expense_template_title")));
				expense_template.SetDescription	(CheckHTTPParam_Text(http_params->Get("bt_expense_template_description")));

				expense_random_name = http_params->GetNameByRegex(regex1);
				while(expense_random_name.length() && error_message.empty())
				{
					string expense_template_line_random = CheckHTTPParam_Number(http_params->Get(expense_random_name));

					if(expense_template_line_random.length())
					{
						C_ExpenseLineTemplate	expense_line_template;

						expense_line_template.title				= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_title_" + expense_template_line_random));
						expense_line_template.tooltip			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_tooltip_" + expense_template_line_random));
						expense_line_template.description		= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_description_" + expense_template_line_random));
						expense_line_template.dom_type			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_dom_type_" + expense_template_line_random));
						expense_line_template.required			= (http_params->Get("bt_expense_template_line_is_required_" + expense_template_line_random) == "true");
						expense_line_template.SetPaymentMethod	(
																	http_params->Get("bt_expense_template_line_is_cash_" + expense_template_line_random) == "true",
																	http_params->Get("bt_expense_template_line_is_card_" + expense_template_line_random) == "true"
																);

						expense_template.AddLine(expense_line_template);
					}
					else
					{
						error_message = "некорректный параметер random у документа";
						MESSAGE_ERROR("", action, "Can't convert expense_random(" + http_params->Get(expense_random_name) + ") to number")
					}

					http_params->Delete(expense_random_name);
					expense_random_name = http_params->GetNameByRegex(regex1);
				}

				if(error_message.empty())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							expense_template.SetCompanyID(agency_id);

							error_message = expense_template.CheckValidity(&db);
							if(error_message.empty())
							{
								error_message = expense_template.SaveToDB(&db);
								if(error_message.empty())
								{
									if(NotifySoWContractPartiesAboutChanges(action, expense_template.GetID(), "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail to check bt_expense_template consistency");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "consistensy check failed");
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addExpenseTemplateLine")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			new_expense_tempate_line_obj = ""s;
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;
				auto			bt_expense_template_id = ""s;

				auto			http_params = indexPage.GetVarsHandler();
				auto			expense_random_name = ""s;
				regex			regex1("bt_expense_template_line_random_");

				C_ExpenseLineTemplate	expense_line_template;

				bt_expense_template_id = CheckHTTPParam_Text(http_params->Get("bt_expense_template_id"));

				expense_random_name = http_params->GetNameByRegex(regex1);
				while(expense_random_name.length() && error_message.empty())
				{
					string expense_template_line_random = CheckHTTPParam_Number(http_params->Get(expense_random_name));

					if(expense_template_line_random.length())
					{

						expense_line_template.title				= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_title_" + expense_template_line_random));
						expense_line_template.tooltip			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_tooltip_" + expense_template_line_random));
						expense_line_template.description		= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_description_" + expense_template_line_random));
						expense_line_template.dom_type			= CheckHTTPParam_Text(http_params->Get("bt_expense_template_line_dom_type_" + expense_template_line_random));
						expense_line_template.required			= (http_params->Get("bt_expense_template_line_is_required_" + expense_template_line_random) == "true");
						expense_line_template.SetPaymentMethod	(
																	http_params->Get("bt_expense_template_line_is_cash_" + expense_template_line_random) == "true",
																	http_params->Get("bt_expense_template_line_is_card_" + expense_template_line_random) == "true"
																);
					}
					else
					{
						error_message = "некорректный параметер random у документа";
						MESSAGE_ERROR("", action, "Can't convert expense_random(" + http_params->Get(expense_random_name) + ") to number")
					}

					http_params->Delete(expense_random_name);
					expense_random_name = http_params->GetNameByRegex(regex1);
				}


				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, bt_expense_template_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								expense_line_template.SetBTExpenseTemplateID(bt_expense_template_id);
								error_message = expense_line_template.CheckValidity(&db);
								if(error_message.empty())
								{
									error_message = expense_line_template.SaveToDB(&db);
									if(error_message.empty())
									{
										new_expense_tempate_line_obj = GetBTExpenseLineTemplatesInJSONFormat("SELECT * FROM `bt_expense_line_templates` WHERE `id`=\"" + expense_line_template.GetID() + "\";", &db, &user);

										if(NotifySoWContractPartiesAboutChanges(action, expense_line_template.GetID(), "", "", "", &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", "", "fail to notify agency");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "fail to check bt_expense_template consistency");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\",\"bt_expense_line_template\":" + new_expense_tempate_line_obj + "}";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteTaskAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			timecard_task_assignment_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(timecard_task_assignment_id.length())
				{
					if(db.Query("SELECT `timecard_tasks_id`,`contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + timecard_task_assignment_id + "\";"))
					{
						string	sow_id = db.Get(0, "contract_sow_id");
						string	task_id = db.Get(0, "timecard_tasks_id");

						error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
						if(error_message.empty())
						{
							if(db.Query("SELECT COUNT(*) AS `counter` FROM `timecard_lines` WHERE `timecard_task_id`=\"" + task_id + "\" AND `timecard_id` IN ( "
											"SELECT `id` FROM `timecards` WHERE `contract_sow_id`=\"" + sow_id + "\" "
										");"
							))
							{
								string	counter = db.Get(0, "counter");

								if(counter == "0")
								{
									if(NotifySoWContractPartiesAboutChanges(action, timecard_task_assignment_id, sow_id, "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify SoW parties");
									}

									// --- important that assignment removal come after notification
									// --- such as notification use information about assignment 
									db.Query("DELETE FROM `timecard_task_assignment` WHERE `id`=\"" + timecard_task_assignment_id + "\";");
									ostResult << "{\"result\":\"success\"}";
								}
								else
								{
									MESSAGE_DEBUG("", action, counter + " instances reported on timecard_task.id(" + task_id + ")");
									error_message = "Нельзя удалять, поскольку субконтрактор отчитался на это назначение " + counter + " раз(а)";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "can't define # of reported lines");
								error_message = "Ошибка БД";
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "timecard_task_assignment.id(" + timecard_task_assignment_id + ") not found");
						error_message = "Назначение не найдено";
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteBTExpenseAssignment")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			bt_expense_assignment_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(bt_expense_assignment_id.length())
				{
					if(db.Query("SELECT `bt_expense_template_id`,`sow_id` FROM `bt_sow_assignment` WHERE `id`=\"" + bt_expense_assignment_id + "\";"))
					{
						string	sow_id = db.Get(0, "sow_id");
						string	bt_expense_template_id = db.Get(0, "bt_expense_template_id");

						error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
						if(error_message.empty())
						{
							if(db.Query("SELECT COUNT(*) AS `counter` FROM `bt_expenses` WHERE `bt_expense_template_id`=\"" + bt_expense_template_id + "\" AND `bt_id` IN ( "
											"SELECT `id` FROM `bt` WHERE `contract_sow_id`=\"" + sow_id + "\" "
										");"
							))
							{
								string	counter = db.Get(0, "counter");

								if(counter == "0")
								{
									if(NotifySoWContractPartiesAboutChanges(action, bt_expense_assignment_id, sow_id, "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify SoW parties");
									}

									// --- important that assignment removal comes after notification
									// --- such as notification use information about assignment 
									db.Query("DELETE FROM `bt_sow_assignment` WHERE `id`=\"" + bt_expense_assignment_id + "\";");
									ostResult << "{\"result\":\"success\"}";
								}
								else
								{
									MESSAGE_DEBUG("", action, counter + " instances reported on bt_expenses.id(" + bt_expense_template_id + ")");
									error_message = "Нельзя удалять, поскольку субконтрактор использовал этот расход " + counter + " раз(а)";
								}
							}
							else
							{
								MESSAGE_ERROR("", action, "can't define # of reported lines");
								error_message = "Ошибка БД";
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "bt_sow_assignment.id(" + bt_expense_assignment_id + ") not found");
						error_message = "Назначение не найдено";
					}

				}
				else
				{
					MESSAGE_ERROR("", action, "one of mandatory parameters missed");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteTask")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			task_id		= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(task_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, task_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isTaskIDValidToRemove(task_id, &db);
								if(error_message.empty())
								{
									string		removal_sql_query = "";
									string		project_id, customer_id;

									tie(customer_id, project_id) = GetCustomerIDProjectIDByTaskID(task_id, &db);

									if(NotifySoWContractPartiesAboutChanges("AJAX_deleteTask", task_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}
									db.Query("DELETE FROM `timecard_tasks` WHERE `id`=\"" + task_id + "\";");
									ostResult << "{\"result\":\"success\"}";

									if(isProjectIDValidToRemove(project_id, &db))
									{
										if(NotifySoWContractPartiesAboutChanges("AJAX_deleteProject", project_id, "", "", "", &db, &user))
										{
										}
										else
										{
											MESSAGE_ERROR("", "", "fail to notify agency");
										}
										db.Query("DELETE FROM `timecard_projects` WHERE `id`=\"" + project_id + "\";");

										if(isCustomerIDValidToRemove(customer_id, &db))
										{
											if(NotifySoWContractPartiesAboutChanges("AJAX_deleteCustomer", customer_id, "", "", "", &db, &user))
											{
											}
											else
											{
												MESSAGE_ERROR("", "", "fail to notify agency");
											}
											db.Query("DELETE FROM `timecard_customers` WHERE `id`=\"" + customer_id + "\";");
										}
										else
										{
											MESSAGE_DEBUG("", action, "customer_id(" + customer_id + ") has another projects assigned. Not valid for removal.");
										}
									}
									else
									{
										MESSAGE_DEBUG("", action, "project_id(" + project_id + ") has another tasks assigned. Not valid for removal.");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "can't remove task_id(" + task_id + ")");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "task_id missed");
					error_message = "Не указан номер задачи на удаление";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteExpenseTemplate")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			expense_template_id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(expense_template_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, expense_template_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isExpenseTemplateIDValidToRemove(expense_template_id, &db);
								if(error_message.empty())
								{
									string		removal_sql_query = "";

									if(NotifySoWContractPartiesAboutChanges("AJAX_deleteExpenseTemplate", expense_template_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}
									db.Query("DELETE FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=\"" + expense_template_id + "\";");
									db.Query("DELETE FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + expense_template_id + "\";");
									db.Query("DELETE FROM `bt_expense_templates` WHERE `id`=\"" + expense_template_id + "\";");
									ostResult << "{\"result\":\"success\"}";
								}
								else
								{
									MESSAGE_DEBUG("", action, "subcontractors reported on expense_template_id(" + expense_template_id + "), can't remove it");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "expense_template_id missed");
					error_message = "Не указан номер расхода на удаление";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteExpenseTemplateLine")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			expense_template_line_id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(expense_template_line_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, expense_template_line_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isExpenseTemplateLineIDValidToRemove(expense_template_line_id, &db);
								if(error_message.empty())
								{
									string		removal_sql_query = "";

									if(NotifySoWContractPartiesAboutChanges("AJAX_deleteExpenseTemplateLine", expense_template_line_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}
									
									db.Query("DELETE FROM `bt_expense_line_templates` WHERE `id`=\"" + expense_template_line_id + "\";");
								}
								else
								{
									MESSAGE_DEBUG("", action, "expense_template_line_id(" + expense_template_line_id + ") presents in expences, can't remove it");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "expense_template_line_id missed");
					error_message = "Не указан номер расхода на удаление";
				}

				if(error_message.empty())
				{
					ostResult << "{\"result\":\"success\"}";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_addCostCenter")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				auto			template_name = "json_response.htmlt"s;
				auto			error_message = ""s;

				auto			title			= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("title"));
				auto			description		= CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("description"));
				auto			cost_center_id = 0l;

				if(title.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = CheckNewValueByAction(action, agency_id, "", title, &db, &user);
							if(error_message.empty())
							{
								cost_center_id = db.InsertQuery("INSERT INTO `cost_centers` (`title`, `description`, `agency_company_id`, `assignee_user_id`, `eventTimestamp`) VALUES (\"" + title + "\", \"" + description + "\", \"" + agency_id + "\", \"" + user.GetID() + "\", UNIX_TIMESTAMP());");
								if(cost_center_id)
								{
									if(NotifySoWContractPartiesAboutChanges(action, to_string(cost_center_id), "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail saving bt_expense_template to DB");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = gettext("agency not found");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "mandatory parameter missed");
					error_message = gettext("mandatory parameter missed");
				}

				ostResult.str("");
				if(error_message.empty())
				{
					ostResult	<< "{"
								<< "\"result\":\"success\","
								<< "\"cost_centers\":[" << GetCostCentersInJSONFormat("SELECT * FROM `cost_centers` WHERE `id`=\"" + to_string(cost_center_id) + "\";", &db, &user) << "]"
								<< "}";
				}
				else
				{
					MESSAGE_DEBUG("", action, "failed");
					ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
				}

				indexPage.RegisterVariableForce("result", ostResult.str());

				if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
			}

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getAgencyInfo")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			agency_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				bool			include_bt = indexPage.GetVarsHandler()->Get("include_bt") == "true";
				bool			include_tasks = indexPage.GetVarsHandler()->Get("include_tasks") == "true";

				string			agency_obj = GetAgencyObjectInJSONFormat(agency_id, include_tasks, include_bt, &db, &user);

				if(agency_obj.length())
				{
					// --- get short info
					ostResult << "{\"result\":\"success\","
									"\"agencies\":[" + agency_obj + "]"
									"}";
				}
				else
				{
					error_message = "Агенство не найдено";
					MESSAGE_DEBUG("", action, "agency(" + agency_id + ") not found");
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_deleteEmployee")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";

				string			employee_id	= CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));

				if(employee_id.length())
				{
					error_message = isAgencyEmployeeAllowedToChangeAgencyData(&db, &user);
					if(error_message.empty())
					{
						string	agency_id = GetAgencyID(&user, &db);

						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, employee_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								error_message = isEmployeeIDValidToRemove(employee_id, &db);
								if(error_message.empty())
								{
									if(NotifySoWContractPartiesAboutChanges(action, employee_id, "", "", "", &db, &user))
									{
									}
									else
									{
										MESSAGE_ERROR("", "", "fail to notify agency");
									}

									if(db.Query("SELECT `user_id` FROM `company_employees` WHERE `id`=\"" + employee_id + "\";"))
									{
										string		user_id = db.Get(0, "user_id");

										db.Query(
											"DELETE FROM `company_employees` WHERE `id`=\"" + employee_id + "\";"
											"UPDATE `users` SET `type`=\"no role\" WHERE `id`=\"" + user_id + "\";"
											);
										ostResult << "{\"result\":\"success\"}";
									}
									else
									{
										error_message = "Пользователь не найден в БД";
										MESSAGE_DEBUG("", action, "fail to get user.id by employee_id(" + employee_id + ")");
									}
								}
								else
								{
									MESSAGE_DEBUG("", action, "employee_id(" + employee_id + ") can't be fired");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", action, "agency.id not found by user.id(" + user.GetID() + ") employment");
							error_message = "Агенство не найдено";
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change agency data");
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "employee_id missed");
					error_message = "Не указан номер задачи на удаление";
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

			MESSAGE_DEBUG("", action, "finish");
		}

		if(action == "AJAX_getApproversAutocompleteList")
		{
			string			name = CheckHTTPParam_Text(indexPage.GetVarsHandler()->Get("name"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));
			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(name.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					int	affected = db.Query("SELECT `id`, `name`, `nameLast` FROM `users` WHERE ((`type`=\"approver\") OR (`type`=\"agency\")) AND ((`name` LIKE \"%" + name + "%\") OR (`nameLast` LIKE \"%" + name + "%\")) LIMIT 0, 20;");
					if(affected)
					{
						ostResult << "{\"result\":\"success\","
								  << "\"autocomplete_list\":[";
						for(int i = 0; i < affected; ++i)
						{
							if(i) ostResult << ",";
							ostResult << "{\"id\":\"" << db.Get(i, "id") << "\","
									  << "\"label\":\"" << db.Get(i, "name") << " " << db.Get(i, "nameLast") << "\"}";
						}
						ostResult << "]}";
					}
					else
					{
						ostResult << "{\"result\":\"success\",\"autocomplete_list\":[]}";
						MESSAGE_DEBUG("", "", "no users found");
					}	
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
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

		if(	action == "AJAX_addTimecardApproverToSoW" ||
			action == "AJAX_addBTExpenseApproverToSoW"
		)
		{
			string			new_value = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("new_value"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(new_value.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					string	approver_id = "";

					error_message = CheckNewValueByAction(action, "fake_id", sow_id, new_value, &db, &user);
					if(error_message.empty())
					{
						error_message = SetNewValueByAction(action, "fake_id", sow_id, new_value, &db, &user);
						if(error_message.empty())
						{
							string info_to_return = GetInfoToReturnByAction(action, "fake_id", sow_id, new_value, &db, &user);

							ostResult << "{\"result\":\"success\"";
							if(info_to_return.length()) ostResult << "," << info_to_return;
							ostResult << "}";

							if(NotifySoWContractPartiesAboutChanges(action, "fake_id", sow_id, "fake_existing_value", new_value, &db, &user))
							{
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "unable to set new value (action/sow_id/value = " + action + "/" + sow_id + "/[" + FilterCP1251Symbols(new_value) + "])");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "new value failed to pass validity check");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
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
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(	action == "AJAX_deleteTimecardApproverFromSoW" ||
			action == "AJAX_deleteBTExpenseApproverFromSoW"
		)
		{
			string			id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("id"));
			string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

			string			template_name = "json_response.htmlt";
			string			error_message = "";
			ostringstream	ostResult;

			ostResult.str("");

			if(id.length() && sow_id.length())
			{
				error_message = isAgencyEmployeeAllowedToChangeSoW(sow_id, &db, &user);
				if(error_message.empty())
				{
					error_message = isActionEntityBelongsToSoW(action, id, sow_id, &db, &user);
					if(error_message.empty())
					{
						// --- notify before actual removal
						if(NotifySoWContractPartiesAboutChanges(action, id, sow_id, "fake_existing_value", "fake_new_value", &db, &user))
						{
						}
						else
						{
							MESSAGE_DEBUG("", action, "fail to notify sow.id(" + sow_id + ") parties about changes");
						}

						error_message = SetNewValueByAction(action, id, sow_id, "fake_new_value", &db, &user);
						if(error_message.empty())
						{
							error_message = ResubmitEntitiesByAction(action, id, sow_id, "fake_new_value", &db, &user);
							if(error_message.empty())
							{
								string info_to_return = GetInfoToReturnByAction(action, id, sow_id, "fake_new_value", &db, &user);

								ostResult << "{\"result\":\"success\"";
								if(info_to_return.length()) ostResult << "," << info_to_return;
								ostResult << "}";
							}
							else
							{
								MESSAGE_DEBUG("", action, "fail to resubmit entities by action");
							}
						}
						else
						{
							MESSAGE_DEBUG("", action, "unable to delete value (action/sow_id/id = " + action + "/" + sow_id + "/" + id + ")");
						}
					}
					else
					{
						MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belong to sow.id(" + sow_id + ")");
					}
				}
				else
				{
					MESSAGE_DEBUG("", action, "user.id(" + user.GetID() + ") doesn't allowed to change sow.id(" + sow_id + ")");
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
				ostResult << "{\"result\":\"error\",\"description\":\"" + error_message + "\"}";
			}

			indexPage.RegisterVariableForce("result", ostResult.str());

			if(!indexPage.SetTemplate(template_name)) MESSAGE_ERROR("", action, "can't find template " + template_name);
		}

		if(action == "AJAX_getCompanyInfoBySoWID")
		{
			ostringstream	ostResult;

			MESSAGE_DEBUG("", action, "start");

			ostResult.str("");
			{
				string			template_name = "json_response.htmlt";
				string			error_message = "";
				string			sow_id = CheckHTTPParam_Number(indexPage.GetVarsHandler()->Get("sow_id"));

				if(user.GetType() == "agency")
				{
					if(db.Query("SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=(SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user.GetID() + "\");"))
					{
						string	agency_id = db.Get(0, "id");
						if(agency_id.length())
						{
							error_message = isActionEntityBelongsToAgency(action, sow_id, agency_id, &db, &user);
							if(error_message.empty())
							{
								string			company_obj = GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=(SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");", &db, &user);

								if(company_obj.length())
								{
									// --- get short info
									ostResult << "{\"result\":\"success\","
													"\"companies\":[" + company_obj + "]"
													"}";
								}
								else
								{
									error_message = "Агенство не найдено";
									MESSAGE_DEBUG("", action, "sow_id(" + sow_id + ") not found");
								}
							}
							else
							{
								MESSAGE_DEBUG("", action, "action entity id(" + user.GetID() + ") doesn't belongs to agency.id(" + agency_id + ")");
							}
						}
						else
						{
							error_message = "Агенство не найдено";
							MESSAGE_ERROR("", action, "user.id(" + user.GetID() + ") is not an agency employee");
						}
					}
					else
					{
						MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
						error_message = "Информация доступна только для агенства";
					}
				}
				else
				{
					MESSAGE_ERROR("", action, "user(" + user.GetID() + ") is not an agency employee");
					error_message = "Информация доступна только для агенства";
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

			MESSAGE_DEBUG("", action, "finish");
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
