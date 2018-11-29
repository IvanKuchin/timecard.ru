#include "c_bt.h"

// --- class C_ExpenseLineTemplate

C_ExpenseLineTemplate::C_ExpenseLineTemplate()
{
}

string	C_ExpenseLineTemplate::CheckValidity(CMysql *db) const
{
	string	error_message = "";

	MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "start");

	if(title.empty())
	{
		error_message = "Необходимо указать название";
		MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "title is required");
	}
	else if((dom_type != "image") && (dom_type != "input"))
	{
		error_message = "Некорректный тип документа";
		MESSAGE_ERROR("C_ExpenseLineTemplate", "", "Incorrect document type (must be either input or image)");
	}
	else if(payment.empty())
	{
		error_message = "Необходимо выбрать форму оплаты";
		MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "payment method doesn't defined");
	}
/*
	else if(bt_expense_template_id.empty())
	{
		error_message = "Не задан идентефикатор расхода";
		MESSAGE_ERROR("C_ExpenseLineTemplate", "", "expense.id doesn't defined");
	}
*/
	else if(db == NULL)
	{
		error_message = "Ошибка БД";
		MESSAGE_ERROR("C_ExpenseLineTemplate", "", "*db doesn't initialized");
	}
	else if
	(
		bt_expense_template_id.length()
		&&
		(bt_expense_template_id != "0")
		&&
		(db->Query("SELECT `id` FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=\"" + bt_expense_template_id + "\" AND `title`=\"" + title + "\";"))
	)
	{
		error_message = "Документ с таким названием уже существует";
		MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "bt_expense_template_line.title already exists with the same bt_expense_template_id(" + bt_expense_template_id + ")");
	}

	MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	C_ExpenseLineTemplate::SaveToDB(CMysql *db)
{
	string		error_message = "";

	MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "start");

	if((bt_expense_template_id.length()) && (bt_expense_template_id != "0"))
	{
		if(db) 										
		{
			if((GetID().empty()) || (GetID() == "0"))
			{
				auto 	temp = db->InsertQuery("INSERT INTO `bt_expense_line_templates` SET "
								"`bt_expense_template_id`=\"" + bt_expense_template_id + "\", "
								"`dom_type`=\"" + dom_type + "\", "
								"`title`=\"" + GetTitle() + "\", "
								"`description`=\"" + description + "\", "
								"`tooltip`=\"" + tooltip + "\", "
								"`payment`=\"" + payment + "\", "
								"`required`=\"" + (required ? "Y" : "N") + "\" "
						);
				if(temp)
				{
					SetID(to_string(temp));
				}
				else
				{
					error_message = "Ошибка БД";
					MESSAGE_ERROR("C_ExpenseLineTemplate", "", "fail to insert into `bt_expense_line_templates` table entry (" + db->GetErrorMessage() + ")");
				}
			}
			else
			{
				db->Query("UPDATE `bt_expense_line_templates` SET "
								"`bt_expense_template_id`=\"" + bt_expense_template_id + "\", "
								"`dom_type`=\"" + dom_type + "\", "
								"`title`=\"" + GetTitle() + "\", "
								"`description`=\"" + description + "\", "
								"`tooltip`=\"" + tooltip + "\", "
								"`payment`=\"" + payment + "\", "
								"`required`=\"" + (required ? "Y" : "N") + "\" "
								"WHERE `id`=\"" + GetID() + "\";"
						);
				if(db->isError())
				{
					error_message = "Ошибка БД";
					MESSAGE_ERROR("C_ExpenseLineTemplate", "", "fail to update `bt_expense_line_templates` table (" + db->GetErrorMessage() + ")");
				}
			}
		}
		else
		{
			error_message = "Ошибка DB";
			MESSAGE_ERROR("C_ExpenseLineTemplate", "", "DB not initialized");
		}
	}
	else
	{
		error_message = "Не найден идентификатор шаблона";
		MESSAGE_ERROR("C_ExpenseLineTemplate", "", "bt_expense_template_id not initialized or 0");
	}

	MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	C_ExpenseLineTemplate::SetPaymentMethod(bool is_cash, bool is_card)
{
	string	error_message = "";

	MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "start");

	if(is_cash && is_card) { payment = "cash and card"; }
	else if(is_cash) { payment = "cash"; }
	else if(is_card) { payment = "card"; }
	else 
	{
		error_message = "Необходимо выбрать форму оплаты";
		MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "payment method doesn't defined");
	}

	MESSAGE_DEBUG("C_ExpenseLineTemplate", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

C_ExpenseLineTemplate::~C_ExpenseLineTemplate()
{
}


// --- class C_ExpenseTemplate

C_ExpenseTemplate::C_ExpenseTemplate()
{
}

string	C_ExpenseTemplate::CheckValidity(CMysql *db) const
{
	string	error_message = "";

	MESSAGE_DEBUG("C_ExpenseTemplate", "", "start");

	if(GetID().empty())
	{
		error_message = "Необходимо указать идентификатор расхода";
		MESSAGE_DEBUG("C_ExpenseTemplate", "", "title is required");
	}
	if(GetTitle().empty())
	{
		error_message = "Необходимо указать название расхода";
		MESSAGE_DEBUG("C_ExpenseTemplate", "", "title is required");
	}
	else if(GetCompanyID().empty() || (GetCompanyID() == "0"))
	{
		error_message = "Необходимо указать компанию которой будет принадлежать расход";
		MESSAGE_ERROR("C_ExpenseTemplate", "", "company.id is required");
	}
	else if(db == NULL)
	{
		error_message = "Ошибка БД";
		MESSAGE_ERROR("C_ExpenseTemplate", "", "*db doesn't initialized");
	}
	else if(db->Query("SELECT `id` FROM `bt_expense_templates` WHERE `agency_company_id`=\"" + GetCompanyID() + "\" AND `title`=\"" + GetTitle() + "\";"))
	{
		error_message = "Расход с таким названием в компании уже существует";
		MESSAGE_DEBUG("C_ExpenseTemplate", "", "bt_expense_templates.title already exists with the same company_id(" + GetCompanyID() + ")");
	}
	else if((error_message = CheckDuplicateLineTitles()).length())
	{
		MESSAGE_DEBUG("C_ExpenseTemplate", "", "bt_expense_templates.title duplicate titles");
	}
	else
	{
		for(auto &expense_template_line: expense_template_lines)
		{
			error_message = expense_template_line.CheckValidity(db);
			if(error_message.length()) break;
		}
	}

	MESSAGE_DEBUG("C_ExpenseTemplate", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto C_ExpenseTemplate::CheckDuplicateLineTitles() const -> string
{
	auto			error_message = ""s;
	vector<string>	titles;
	
	MESSAGE_DEBUG("C_ExpenseTemplate", "", "start");

	if(expense_template_lines.size() > 1)
	{
		for(auto &expense_template_line: expense_template_lines)
		{
			titles.push_back(expense_template_line.GetTitle());
		}

		sort(titles.begin(), titles.end());

		for(auto it = titles.begin() + 1; it != titles.end(); ++it)
		{
			if(*it == *(it-1)) error_message = "Название докуметов не должно повторяться";
		}
	}
	else
	{
		MESSAGE_DEBUG("C_ExpenseTemplate", "", "expense_template_lines < 2, elements are unique");
	}


	MESSAGE_DEBUG("C_ExpenseTemplate", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_ExpenseTemplate::SaveToDB(CMysql *db) -> string
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("C_ExpenseTemplate", "", "start");

	if(db) 										
	{
		if((GetID().empty()) || (GetID() == "0"))
		{
			auto 	temp = db->InsertQuery("INSERT INTO `bt_expense_templates` SET "
							"`title`=\"" + GetTitle() + "\", "
							"`agency_company_id`=\"" + GetCompanyID() + "\", "
							"`agency_comment`=\"" + GetDescription() + "\", "
							"`eventTimestamp`=UNIX_TIMESTAMP() ;"
					);
			if(temp)
			{
				SetID(to_string(temp));
			}
			else
			{
				error_message = "Ошибка БД";
				MESSAGE_ERROR("C_ExpenseTemplate", "", "fail to insert into `bt_expense_templates` table entry (" + db->GetErrorMessage() + ")");
			}
		}
		else
		{
			db->Query("UPDATE `bt_expense_templates` SET "
							"`title`=\"" + GetTitle() + "\", "
							"`agency_company_id`=\"" + GetCompanyID() + "\", "
							"`agency_comment`=\"" + GetDescription() + "\", "
							"`eventTimestamp`=UNIX_TIMESTAMP() "
							"WHERE `id`=\"" + GetID() + "\";"
					);
			if(db->isError())
			{
				error_message = "Ошибка БД";
				MESSAGE_ERROR("C_ExpenseTemplate", "", "fail to update `bt_expense_templates` table (" + db->GetErrorMessage() + ")");
			}
		}

		if(error_message.empty())
		{
			for(auto &expense_template_line: expense_template_lines)
			{
				expense_template_line.SetBTExpenseTemplateID(GetID());
				error_message = expense_template_line.SaveToDB(db);
				if(error_message.length())
				{
					MESSAGE_ERROR("C_ExpenseTemplate", "", "fail to save to `bt_expense_line_templates` table entry");
					break;
				}
			}
		}
	}
	else
	{
		error_message = "Ошибка DB";
		MESSAGE_ERROR("C_ExpenseTemplate", "", "DB not initialized");
	}

	MESSAGE_DEBUG("C_ExpenseTemplate", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	C_ExpenseTemplate::AddLine(C_ExpenseLineTemplate expense_template_line)
{
	string	error_message = "";

	MESSAGE_DEBUG("C_ExpenseTemplate", "", "start");

	expense_template_lines.push_back(expense_template_line);

	MESSAGE_DEBUG("C_ExpenseTemplate", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

C_ExpenseTemplate::~C_ExpenseTemplate()
{
}



// ---  class C_ExpenseLine

C_ExpenseLine::C_ExpenseLine()
{
}

string	C_ExpenseLine::CheckValidity(C_ExpenseLineTemplate exp_line_template, CMysql *db) const
{
	string	result = "";

	MESSAGE_DEBUG("C_ExpenseLine", "", "start");

	if(exp_line_template.required)
	{
		if(GetValue().length())
		{
			// --- all is well
		}
		else
		{
			if(exp_line_template.dom_type == "image")
			{
				if(GetID().length() && (GetID() != "0"))
				{
					if(db)
					{
						if(db->Query("SELECT `value` FROM `bt_expense_lines` WHERE `id`=\"" + GetID() + "\";"))
						{
							string	db_value = db->Get(0, "value");
							if(db_value.length())
							{
								// --- all is well. Keep DB image for this expense_line
							}
							else
							{
								result = "Обязательное фото(" + exp_line_template.title + ") не загружено";
								MESSAGE_ERROR("C_ExpenseLine", "", "required image expense_line_template.id(" + exp_line_template.id + ") didn't upload in expense_line (id/random: " + GetID() + "/" + GetRandom() + ")");
							}
						}
						else
						{
							result = "Обязательное фото(" + exp_line_template.title + ") не найдено в БД";
							MESSAGE_ERROR("C_ExpenseLine", "", "required image expense_line_template.id(" + exp_line_template.id + ") not found in DB in expense_line (id/random: " + GetID() + "/" + GetRandom() + ")");
						}
					}
					else
					{
						result = "Ошибка DB";
						MESSAGE_ERROR("C_ExpenseLine", "", "DB not initialized in expense (id/random: " + GetID() + "/" + GetRandom() + ")");
					}
				}
				else
				{
					result = "Обязательное фото(" + exp_line_template.title + ") не загружено";
					MESSAGE_ERROR("C_ExpenseLine", "", "required image expense_line_template.id(" + exp_line_template.id + ") didn't upload in expense_line (id/random: " + GetID() + "/" + GetRandom() + ")");
				}
			}
			else
			{
				result = "Обязательное поле ввода(" + exp_line_template.title + ") не заполнено";
				MESSAGE_ERROR("C_ExpenseLine", "", "missed required input field expense_line_template.id(" + exp_line_template.id + ") in expense_line (id/random: " + GetID() + "/" + GetRandom() + ")");
			}
		}
	}

	MESSAGE_DEBUG("C_ExpenseLine", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	C_ExpenseLine::RemoveOldImage(CMysql *db)
{
	string	error_message = ""; // --- assign smth to error_message only if further actions have to be aborted

	MESSAGE_DEBUG("C_ExpenseLine", "", "start");

	if(db) 										
	{
		if(GetID().length() && (GetID() != "0"))
		{

			int affected = db->Query(
				"SELECT `bt_expense_lines`.`value` as `value`, `bt_expense_line_templates`.`dom_type` as `dom_type` FROM `bt_expense_lines` "
				"INNER JOIN `bt_expense_line_templates` on `bt_expense_lines`.`bt_expense_line_template_id`=`bt_expense_line_templates`.`id` "
				"WHERE `bt_expense_lines`.`id`=\"" + GetID() + "\";"
				);
			for(int i = 0; i < affected; ++i)
			{
				string value = db->Get(i, "value");
				string dom_type = db->Get(i, "dom_type");

				if((dom_type == "image") && (value.length()))
				{
					MESSAGE_DEBUG("C_ExpenseLine", "", "remove image (" + IMAGE_EXPENSELINES_DIRECTORY + "/" + value + ")");
					unlink((IMAGE_EXPENSELINES_DIRECTORY + "/" + value).c_str());
				}
			}
		}
		else
		{
			MESSAGE_ERROR("C_ExpenseLine", "", "expense_line.id doesn't defined");
		}
	}
	else
	{
		MESSAGE_ERROR("C_ExpenseLine", "", "DB not initialized");
	}


	MESSAGE_DEBUG("C_ExpenseLine", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

string	C_ExpenseLine::SaveToDB(string bt_expense_id, CMysql *db)
{
	string		error_message = "";

	MESSAGE_DEBUG("C_ExpenseLine", "", "start");

	if(db) 										
	{
		if(GetID().length() && (GetID() != "0"))
		{
			
			if(GetValue().length()) error_message = RemoveOldImage(db);
			if(error_message.empty())
			{
				string	update_query = "";
				update_query =		"UPDATE `bt_expense_lines` SET "
									"`bt_expense_id`=\"" + bt_expense_id + "\", "
									"`bt_expense_line_template_id`=\"" + GetExpenseLineTemplateID() + "\", "
									"`comment`=\"" + GetComment() + "\", ";
				if(GetValue().length())
					update_query +=	"`value`=\"" + GetValue() + "\", ";
				update_query +=		"`eventTimestamp`=UNIX_TIMESTAMP() "
									"WHERE `id`=\"" + GetID() + "\";";

				db->Query(update_query);
				if(db->isError())
				{
					error_message = "Ошибка БД";
					MESSAGE_ERROR("C_ExpenseLine", "", "fail to update `bt_expense_lines` table (" + db->GetErrorMessage() + ")");
				}
			}
			else
			{
				MESSAGE_DEBUG("C_ExpenseLine", "", "fail to remove old image")
			}
		}
		else
		{
			long int 	temp = db->InsertQuery("INSERT INTO `bt_expense_lines` SET "
							"`bt_expense_id`=\"" + bt_expense_id + "\", "
							"`bt_expense_line_template_id`=\"" + GetExpenseLineTemplateID() + "\", "
							"`comment`=\"" + GetComment() + "\", "
							"`value`=\"" + GetValue() + "\", "
							"`eventTimestamp`=UNIX_TIMESTAMP() ;"
					);
			if(temp)
			{
				SetID(to_string(temp));
			}
			else
			{
				error_message = "Ошибка БД";
				MESSAGE_ERROR("C_ExpenseLine", "", "fail to insert into `bt_expense_lines` table entry (" + db->GetErrorMessage() + ")");
			}
		}
	}
	else
	{
		error_message = "Ошибка DB";
		MESSAGE_ERROR("C_ExpenseLine", "", "DB not initialized");
	}

	MESSAGE_DEBUG("C_ExpenseLine", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

bool C_ExpenseLine::RemoveUnsavedLinesImages(CMysql *db) const
{
	bool		result = false;

	MESSAGE_DEBUG("C_ExpenseLine", "", "start");

	if(GetValue().length())
	{
		if(isSavedInDB())
		{
			// --- expense_line saved in DB. Don't remove it.
			MESSAGE_DEBUG("C_ExpenseLine", "", "expense_line.id(" + GetID() + ") in DB, don't remove.");
			result = true;
		}
		else
		{
			// --- unsaved expense_line 
			if(GetExpenseLineTemplateID().length())
			{
				if(db->Query("SELECT `dom_type` FROM `bt_expense_line_templates` WHERE `id`=\"" + GetExpenseLineTemplateID() + "\";"))
				{
					string	dom_type = db->Get(0, "dom_type");

					if(dom_type == "image")
					{
						MESSAGE_DEBUG("C_ExpenseLine", "", "remove (" + IMAGE_EXPENSELINES_DIRECTORY + "/" + GetValue() + ") image");
						unlink((IMAGE_EXPENSELINES_DIRECTORY + "/" + GetValue()).c_str());
						result = true;
					}
					else
					{
						MESSAGE_DEBUG("C_ExpenseLine", "", "expense_line_template_id(" + GetExpenseLineTemplateID() + ") dom_type(" + dom_type + ") not an image, don't delete file");
						result = true;
					}
				}
				else
				{
					MESSAGE_ERROR("C_ExpenseLine", "", "expense_line_template_id(" + GetExpenseLineTemplateID() + ") not found in bt_expense_line_templates table");
				}
			}
			else
			{
				MESSAGE_ERROR("C_ExpenseLine", "", "expense_line_template_id not defined in expense_line.random(" + GetRandom() + ")");
			}
		}
	}
	else
	{
		result = true;
		MESSAGE_DEBUG("C_ExpenseLine", "", "expense_line(id/random: " + GetID() + "/" + GetRandom() + ") value is empty");
	}

	MESSAGE_DEBUG("C_ExpenseLine", "", "finish (result length is " + to_string(result) + ")");

	return result;
}

C_ExpenseLine::~C_ExpenseLine()
{
}



// --- class C_Expense

string	C_Expense::CheckExpenseLine(C_ExpenseLineTemplate exp_line_template, CMysql *db) const
{
	string	result = "";

	MESSAGE_DEBUG("C_Expense", "", "start");

	if(db)
	{
		bool	is_found = false;

		for(auto &expense_line: expense_lines)
		{
			if(expense_line.GetExpenseLineTemplateID() == exp_line_template.id)
			{
				is_found = true;
				result = expense_line.CheckValidity(exp_line_template, db);
			}
		}

		if(is_found)
		{
		}
		else
		{
			result = "Необходимый документ не найден";
			MESSAGE_ERROR("C_Expense", "", "Mandatory document not found, it should match expense_line_template.id(" + exp_line_template.id + ") in expense(id/random: " + GetID() + "/" + GetRandom() + ")");
		}
	}
	else
	{
		result = "Ошибка DB";
		MESSAGE_ERROR("C_Expense", "", "DB not initialized in expense (id/random: " + GetID() + "/" + GetRandom() + ")");
	}

	MESSAGE_DEBUG("C_Expense", "", "finish (result.length = " + to_string(result.length()) + ")");

	return result;
}


string	C_Expense::CheckAppropriateExpenseLines(CMysql *db) const
{
	string	result = "";

	MESSAGE_DEBUG("C_Expense", "", "start");

	if(db)
	{
		if(GetExpenseTemplateID().length())
		{
			vector<C_ExpenseLineTemplate>	expense_line_templates;
			int								affected = db->Query("SELECT * FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=\"" + GetExpenseTemplateID() + "\";");

			for(int i = 0; i < affected; ++i)
			{
				C_ExpenseLineTemplate	item;

				item.id 				=  db->Get(i, "id");
				item.bt_expense_template_id =  db->Get(i, "bt_expense_template_id");
				item.dom_type 			=  db->Get(i, "dom_type");
				item.title 				=  db->Get(i, "title");
				item.description 		=  db->Get(i, "description");
				item.tooltip 			=  db->Get(i, "tooltip");
				item.payment 			=  db->Get(i, "payment");
				item.required 			= (string(db->Get(i, "required")) == "Y");

				expense_line_templates.push_back(item);
			}

			for(auto &expense_line_template: expense_line_templates)
			{
				if(expense_line_template.required && (expense_line_template.payment.find(GetPaymentType()) != string::npos))
				{
					result = CheckExpenseLine(expense_line_template, db);
				}

				if(result.length())
				{
					MESSAGE_DEBUG("C_Expense", "", "ExpenseLineTemplate.id(" + expense_line_template.id + ") check failed on expense (id/random: " + GetID() + "/" + GetRandom() + ")");
					break;
				}

				expense_line_template.matched = true;
			}

			if(result.empty())
			{
				for(auto &expense_line_template: expense_line_templates)
				{
					if(expense_line_template.required && !expense_line_template.matched)
					{
						result = "Не все обязательные документы загружены";
						MESSAGE_DEBUG("C_Expense", "", "mandatory expense_line_template.id(" + expense_line_template.id + ") failed on expense (id/random: " + GetID() + "/" + GetRandom() + ")");

						// --- if place break then we don't get all the information about missing expense_lines
						// break;
					}
				}
			}
		}
		else
		{
			result = "Не найден шаблон расхода";
			MESSAGE_ERROR("C_Expense", "", "ExpenseTemplateID is empty on expense (id/random: " + GetID() + "/" + GetRandom() + ")");
		}
	}
	else
	{
		result = "Ошибка DB";
		MESSAGE_ERROR("C_Expense", "", "DB not initialized in expense (id/random: " + GetID() + "/" + GetRandom() + ")");
	}

	MESSAGE_DEBUG("C_Expense", "", "finish (result.length = " + to_string(result.length()) + ")");

	return result;
}

string	C_Expense::CheckValidity(CMysql *db) const
{
	string	result = "";
	regex	date_regex("^[[:digit:]]{1,2}\\/[[:digit:]]{1,2}\\/[[:digit:]]{2,4}$");

	MESSAGE_DEBUG("C_Expense", "", "start");

	if(db == NULL) 										
	{
		result = "Ошибка DB";
		MESSAGE_ERROR("C_Expense", "", "DB not initialized in expense (id/random: " + GetID() + "/" + GetRandom() + ")");
	}
	else if(!regex_match(GetDate(), date_regex))
	{
		result = "некорректная дата расхода";
		MESSAGE_DEBUG("C_Expense", "", "wrong expense date(" + GetDate() + ")  in expense (id/random: " + GetID() + "/" + GetRandom() + ")");
	}
	else if(GetPriceDomestic() <= 0)
	{
		result = "не указана цена в руб.";
		MESSAGE_DEBUG("C_Expense", "", "domestic price " + string(GetPriceDomestic()) + " incorrect in expense (id/random: " + GetID() + "/" + GetRandom() + "");
	}
	else if(GetPriceForeign() && (GetCurrencyName().length() == 0))
	{
		result = "не указано наименование валюты";
		MESSAGE_DEBUG("C_Expense", "", "currency name is empty in expense (id/random: " + GetID() + "/" + GetRandom() + "");
	}
	else if(GetPriceForeign() && (GetCurrencyNominal() == 0))
	{
		result = "не указан номинал валюты";
		MESSAGE_DEBUG("C_Expense", "", "currency nominal is 0 in expense (id/random: " + GetID() + "/" + GetRandom() + "");
	}
	else if(GetPriceForeign() && (GetPriceDomestic() != (GetPriceForeign() * GetCurrencyValue() / GetCurrencyNominal())))
	{
		result = "сумма в рублях не соответствует сумме в валюте";
		MESSAGE_DEBUG("C_Expense", "", "price in rub.(" + string(GetPriceDomestic()) + ") not equal to foregn price(cost = price / nominal / value is " + string(GetPriceForeign() * GetCurrencyValue() / GetCurrencyNominal()) + " = " + string(GetPriceForeign()) + " / " + to_string(GetCurrencyNominal()) + " / " + string(GetCurrencyValue()) + ") in expense (id/random: " + GetID() + "/" + GetRandom() + ")");
	}
	else
	{
		result = CheckAppropriateExpenseLines(db);
	}

	MESSAGE_DEBUG("C_Expense", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	C_Expense::SaveToDB(string bt_id, CMysql *db)
{
	string		error_message = "";

	MESSAGE_DEBUG("C_Expense", "", "start");

	if(db) 										
	{
		if(GetID().length() && (GetID() != "0"))
		{
			db->Query("UPDATE `bt_expenses` SET "
							"`bt_id`=\"" + bt_id + "\", "
							"`bt_expense_template_id`=\"" + GetExpenseTemplateID() + "\", "
							"`comment`=\"" + GetComment() + "\", "
							"`date`=STR_TO_DATE(\"" + GetDate() + "\", '%d/%m/%Y'), "
							"`payment_type`=\"" + string(GetPaymentType()) + "\", "
							"`price_foreign`=\"" + string(GetPriceForeign()) + "\", "
							"`price_domestic`=\"" + string(GetPriceDomestic()) + "\", "
							"`currency_nominal`=\"" + to_string(GetCurrencyNominal()) + "\", "
							"`currency_name`=\"" + GetCurrencyName() + "\", "
							"`currency_value`=\"" + string(GetCurrencyValue()) + "\", "
							"`eventTimestamp`=UNIX_TIMESTAMP() "
							"WHERE `id`=\"" + GetID() + "\";"
					);
			if(db->isError())
			{
				error_message = "Ошибка БД";
				MESSAGE_ERROR("C_Expense", "", "fail to update `bt_expenses` table (" + db->GetErrorMessage() + ")");
			}
		}
		else
		{
			long int 	temp = db->InsertQuery("INSERT INTO `bt_expenses` SET "
							"`bt_id`=\"" + bt_id + "\", "
							"`bt_expense_template_id`=\"" + GetExpenseTemplateID() + "\", "
							"`comment`=\"" + GetComment() + "\", "
							"`date`=STR_TO_DATE(\"" + GetDate() + "\", '%d/%m/%Y'), "
							"`payment_type`=\"" + string(GetPaymentType()) + "\", "
							"`price_foreign`=\"" + string(GetPriceForeign()) + "\", "
							"`price_domestic`=\"" + string(GetPriceDomestic()) + "\", "
							"`currency_nominal`=\"" + to_string(GetCurrencyNominal()) + "\", "
							"`currency_name`=\"" + GetCurrencyName() + "\", "
							"`currency_value`=\"" + string(GetCurrencyValue()) + "\", "
							"`eventTimestamp`=UNIX_TIMESTAMP() ;"
					);
			if(temp)
			{
				SetID(to_string(temp));
			}
			else
			{
				error_message = "Ошибка БД";
				MESSAGE_ERROR("C_Expense", "", "fail to insert into `bt_expenses` table entry (" + db->GetErrorMessage() + ")");
			}
		}

		if(error_message.empty())
		{
			for(auto &expense_line: expense_lines)
			{
				error_message = expense_line.SaveToDB(GetID(), db);

				if(error_message.length())
				{
					MESSAGE_ERROR("C_Expense", "", "fail to save to `bt_expense_lines` table entry");
					break;
				}
			}
		}
	}
	else
	{
		error_message = "Ошибка DB";
		MESSAGE_ERROR("C_Expense", "", "DB not initialized");
	}

	MESSAGE_DEBUG("C_Expense", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

bool C_Expense::RemoveUnsavedLinesImages(CMysql *db) const
{
	bool		result = true;

	MESSAGE_DEBUG("C_Expense", "", "start");

	for(auto &expense_line: expense_lines)
	{
		if(!expense_line.RemoveUnsavedLinesImages(db)) result = false;
	}
	
	MESSAGE_DEBUG("C_Expense", "", "finish (result length is " + to_string(result) + ")");

	return result;
}

C_Expense::~C_Expense()
{
}



// --- class C_BT

C_BT::C_BT()
{
}

string C_BT::GetStatus()
{
	MESSAGE_DEBUG("C_BT", "", "start");
	
	if(status.empty())
	{
		if(GetID().empty())
		{
			MESSAGE_DEBUG("C_BT", "", "new bt, no status yet")
		}
		else
		{
			if(db)
			{
				if(db->Query("SELECT `status` FROM `bt` WHERE `id`=\"" + GetID() + "\";"))
				{
					SetStatus(db->Get(0, "status"));
				}
				else
				{
					MESSAGE_ERROR("C_BT", "", "bt.is(" + GetID() + ") not found in DB");
				}
			}
			else
			{
				MESSAGE_ERROR("C_BT", "", "db not initialized");
			}
		}
	}

	MESSAGE_DEBUG("C_BT", "", "end(result = " + status + ")");

	return status;
}

bool C_BT::AssignExpenseLineByParentRandom(C_ExpenseLine expense_line)
{

	bool	result = false;

	MESSAGE_DEBUG("C_BT", "", "start");

	if(expense_line.GetParentRandom().length())
	{
		for(auto &expense: expenses)
		{
			if(expense.GetRandom() == expense_line.GetParentRandom())
			{
				expense.AddExpenseLine(expense_line);
				MESSAGE_DEBUG("C_BT", "", "expense_line(" + expense_line.GetRandom() + ") assigned to expense(" + expense.GetRandom() + ")");
				result = true;
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "parent_random expense_line(id/random - " + expense_line.GetID() + "/" + expense_line.GetRandom() + ") is empty");
	}

	MESSAGE_DEBUG("C_BT", "", "finish (result is " + to_string(result) + ")");

	return result;
}

bool C_BT::isSubcontractorAssignedToSow() const
{
	bool	result = false;

	MESSAGE_DEBUG("C_BT", "", "start");

	if(!db) 
	{		
		MESSAGE_ERROR("C_BT", "", "DB not initialized");
	}
	else if(GetUserID().length() == 0)
	{
		MESSAGE_ERROR("C_BT", "", "user.id not initialized");
	}
	else if(GetSowID().length() == 0)
	{
		MESSAGE_DEBUG("C_BT", "", "sow.id is empty");
	}
	else if(db->Query("SELECT `id` FROM `contracts_sow` WHERE `id`=\"" + GetSowID() + "\" AND `subcontractor_company_id` IN (SELECT `id` FROM `company` WHERE `admin_userID`=\"" + GetUserID() + "\");"))
	{
		result = true;
	}	

	MESSAGE_DEBUG("C_BT", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string	C_BT::CheckValidity() const
{
	string	result = "";
	regex	date_regex("^[[:digit:]]{1,2}\\/[[:digit:]]{1,2}\\/[[:digit:]]{2,4}$");

	MESSAGE_DEBUG("C_BT", "", "start");

	if(db == NULL) 										
	{
		result = "Ошибка DB";
		MESSAGE_ERROR("C_BT", "", "DB not initialized");
	}
	else if(GetUserID().length() == 0)
	{
		result = "не указан user.id";
		MESSAGE_ERROR("C_BT", "", "user.id not initialized");
	}
	else if(GetSowID().length() == 0)
	{
		result = "не указан sow";
		MESSAGE_DEBUG("C_BT", "", "sow.id is empty");
	}
	else if(GetPurpose().length() == 0)
	{
		result = "не указана цель поездки";
		MESSAGE_DEBUG("C_BT", "", "purpose is empty");
	}
	else if(GetDestination().length() == 0)
	{
		result = "пункт назначения пустой";
		MESSAGE_DEBUG("C_BT", "", "destination is empty");
	}
	else if(!regex_match(GetStartDate(), date_regex))
	{
		result = "некорректная дата отправления";
		MESSAGE_DEBUG("C_BT", "", "start_date is empty");
	}
	else if(!regex_match(GetEndDate(), date_regex))
	{
		result = "некорректная дата возврата";
		MESSAGE_DEBUG("C_BT", "", "end_date is empty");
	}
	else if(!isSubcontractorAssignedToSow())
	{
		result = "этот договор заключен не с вами";
		MESSAGE_DEBUG("C_BT", "", "subcontractor doesn't belong to sow");
	}
	else
	{
		for(auto &expense: expenses)
		{
			result = expense.CheckValidity(db);
			if(result.length()) break;
		}
	}

	MESSAGE_DEBUG("C_BT", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	C_BT::isValidToUpdate()
{
	string		error_message = "";

	MESSAGE_DEBUG("C_BT", "", "start");

	if((GetStatus() == "submit") || (GetStatus() == "approved"))
	{
		error_message = "Невозможно изменить данные";
	}
	
	MESSAGE_DEBUG("C_BT", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	C_BT::SaveToDB()
{
	string		error_message = "";

	MESSAGE_DEBUG("C_BT", "", "start");

	if(db) 										
	{
		if(GetID().length() && (GetID() != "0"))
		{
			error_message = isValidToUpdate();
			if(error_message.empty())
			{
				db->Query("UPDATE `bt` SET "
								"`contract_sow_id`=\"" + GetSowID() + "\", "
								"`date_start`=STR_TO_DATE(\"" + GetStartDate() + "\", '%d/%m/%Y'), "
								"`date_end`=STR_TO_DATE(\"" + GetEndDate() + "\", '%d/%m/%Y'), "
								"`place`=\"" + GetDestination() + "\", "
								"`purpose`=\"" + GetPurpose() + "\", "
								+ (GetAction() == "submit" ? "`status`=\"submit\",`submit_date`=UNIX_TIMESTAMP(), " : "") +
								"`eventTimestamp`=UNIX_TIMESTAMP() "
								"WHERE `id`=\"" + GetID() + "\";"
						);
				if(db->isError())
				{
					error_message = "Ошибка БД";
					MESSAGE_ERROR("C_BT", "", "fail to update `bt` table (" + db->GetErrorMessage() + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("C_BT", "", "not allowed to update bt.id(" + GetID() + ")");
			}
		}
		else
		{
			long int 	temp = db->InsertQuery("INSERT INTO `bt` SET "
							"`contract_sow_id`=\"" + GetSowID() + "\", "
							"`date_start`=STR_TO_DATE(\"" + GetStartDate() + "\", '%d/%m/%Y'), "
							"`date_end`=STR_TO_DATE(\"" + GetEndDate() + "\", '%d/%m/%Y'), "
							"`place`=\"" + GetDestination() + "\", "
							"`purpose`=\"" + GetPurpose() + "\", "
							+ (GetAction() == "submit" ? "`status`=\"submit\",`submit_date`=UNIX_TIMESTAMP(), " : "") +
							"`eventTimestamp`=UNIX_TIMESTAMP() "
							";"
					);
			if(temp)
			{
				SetID(to_string(temp));
			}
			else
			{
				error_message = "Ошибка БД";
				MESSAGE_ERROR("C_BT", "", "fail to insert into `bt` table entry (" + db->GetErrorMessage() + ")");
			}
		}

		if(error_message.empty())
		{
			for(auto &expense: expenses)
			{
				error_message = expense.SaveToDB(GetID(), db);

				if(error_message.length())
				{
					MESSAGE_ERROR("C_BT", "", "fail to save to `bt_expense` table");
					break;
				}
			}
		}

		// --- submit BT, if needed
		if(error_message.empty() && (GetAction() == "submit"))
		{
			if(GetID().length() && GetSowID().length())
			{
				// --- auto_approve
				// --- UNIX_TIMESTAMP() + 1 needed to make visibility that approve been done after submission
				db->Query("INSERT INTO `bt_approvals` (`bt_id`, `approver_id`, `decision`, `comment`, `eventTimestamp`) "
								"SELECT \"" + GetID() + "\", `id`, \"approved\", \"auto-approve\",UNIX_TIMESTAMP()+1 FROM `bt_approvers` WHERE `contract_sow_id`=\"" + GetSowID() + "\" AND `auto_approve`=\"Y\"");
			}
			else
			{
				// doesn't assign anything to error_message to allow process to continue
				MESSAGE_ERROR("C_BT", "", "fail to auto_approve due to bt.id(" + GetID() + ") is empty");
			}

			if(SubmitBT(GetID(), db, GetUser()))
			{
			}
			else
			{
				error_message = "Неудалось отправить командировку на проверку.";
				MESSAGE_ERROR("C_BT", "", "fail to submit bt.id(" + GetID() + ") for approvals");
			}
		}
	}
	else
	{
		error_message = "Ошибка DB";
		MESSAGE_ERROR("C_BT", "", "DB not initialized");
	}

	MESSAGE_DEBUG("C_BT", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

bool C_BT::RemoveUnsavedLinesImages() const
{
	bool		result = true;

	MESSAGE_DEBUG("C_BT", "", "start");

	for(auto &expense: expenses)
	{
		if(!expense.RemoveUnsavedLinesImages(db)) result = false;
	}
	
	MESSAGE_DEBUG("C_BT", "", "finish (result length is " + to_string(result) + ")");

	return result;
}


C_BT::~C_BT()
{
}

ostream& operator<<(ostream& os, const C_ExpenseLine &expense_line)
{
	os << "\"id\":\"" + expense_line.GetID() + "\",\"expense_item_doc_id\":\"" + expense_line.GetExpenseLineTemplateID() + "\",\"value\":\"" + expense_line.GetValue() + "\",\"comment\":\"" + expense_line.GetComment() + "\",\"random\":\"" + expense_line.GetRandom() + "\"";
	return os;
}

ostream& operator<<(ostream& os, const C_Expense &expense)
{
	int		i = 0;

	os << "\"id\":\"" + expense.GetID() + "\",\"expenseitemid\":\"" + expense.GetExpenseTemplateID() + "\",\"paymenttype\":\"" + expense.GetPaymentType() + "\",\"date\":\"" + expense.GetDate() + "\",\"pricedomestic\":\"" + to_string(expense.GetPriceDomestic()) + "\",\"priceforeign\":\"" + to_string(expense.GetPriceForeign()) + "\",\"currencynominal\":\"" + to_string(expense.GetCurrencyNominal()) + "\",\"currencyvalue\":\"" + to_string(expense.GetCurrencyValue()) + "\",\"currencyname\":\"" + expense.GetCurrencyName() + "\",\"comment\":\"" + expense.GetComment() + "\",\"random\":\"" + expense.GetRandom() + "\"";
	os << ",\"lines\":[";
	i = 0;
	for(auto &expense_line: expense.GetExpenseLines())
	{
		if(i++) os << ",";
		os << "{" << expense_line << "}";
	}
	os << "]";
	return os;
}

ostream& operator<<(ostream& os, const C_BT &bt)
{
	int		i = 0;

	os << "\"ID\":\"" + bt.GetID() + "\", \"contract_sow_id\":\"" + bt.GetSowID() + "\", \"UserID\":\"" + bt.GetUserID() + "\", \"Destination\":\"" + bt.GetDestination() + "\", \"StartDate\":\"" + bt.GetStartDate() + "\", \"EndDate\":\"" + bt.GetEndDate() + "\", \"Purpose\":\"" + bt.GetPurpose() + "\", \"action\":\"" + bt.GetAction() + "\"";
	os << ",\"expenses\":[";
	i = 0;
	for(auto &expense: bt.GetExpenses())
	{
		if(i++) os << ",";
		os << "{" << expense << "}";
	}
	os << "]";
	return os;
}

