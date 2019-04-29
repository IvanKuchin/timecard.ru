#include "c_print_1c_subcontractor.h"


auto	C_Print_1C_Subcontractor_Base::RenderTemplate() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start (template: " + GetTemplateBody_Filename() + ")");

	// --- build table for service invoice
	if(error_message.empty())
	{
		if(GetTemplateBody_Filename().length())
		{
			auto	table_content = ""s;

			for(auto i = 1; isTableRowExists(i); ++i)
			{
				CCgi		render_obj;
				CVars		vars_obj = vars->GetVars();

				vars_obj.SetIndex(to_string(i));
				render_obj.SetVars(vars_obj);

				if(render_obj.SetTemplateFile(GetTemplateBody_Filename()))
				{
					table_content += render_obj.RenderTemplate();

					{
						auto	empty_var_list = render_obj.GetEmptyVarList();
						auto	empty_var_list_str = ""s;

						// --- generate error_message
						if(empty_var_list.size())
						{

							for(auto &var_name: empty_var_list)
							{
								if(empty_var_list_str.length()) empty_var_list_str += ", ";
								empty_var_list_str += var_name;
							}

							error_message = gettext("1C generation failed") + "("s + vars->Get("subcontractor_company_name_" + to_string(i)) + "). " + gettext("empty vars list") + ": "s + empty_var_list_str;
							MESSAGE_ERROR("", "", "following variable must be defined on contract (" + vars->Get("subcontractor_company_name_" + to_string(i)) + "): " + empty_var_list_str);
						}

					}
				}
				else
				{
					error_message = gettext("1c template not found");
					MESSAGE_ERROR("", "", error_message);
				}

				if(error_message.length()) break;
			}

			vars->AssignVariableValue("Subcontractor_Documents_1C", table_content, true);
		}
		else
		{
			MESSAGE_DEBUG("", "", "1c filename is empty");
		}
	}

	// --- build service invoice
	if(error_message.empty())
	{
		if(GetTemplate_Filename().length())
		{
			CCgi		render_obj;
			CVars		vars_obj = vars->GetVars();

			render_obj.SetVars(vars_obj);

			if(render_obj.SetTemplateFile(GetTemplate_Filename()))
			{
				content = render_obj.RenderTemplate();
			}
			else
			{
				error_message = gettext("1c template not found");
				MESSAGE_ERROR("", "", error_message);
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "1c filename is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Print_1C_Subcontractor_Base::SaveFile() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(content.length())
	{
		if(GetFilename().length())
		{
			ofstream ofs;
			ofs.open (GetFilename(), std::ofstream::out | std::ofstream::app);

			ofs << content;

			ofs.close();
		}
		else
		{
			error_message = gettext("1c filename is empty");
			MESSAGE_ERROR("", "", error_message);
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "content is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Print_1C_Subcontractor_Payment::Print() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty())
	{
		if((error_message = RenderTemplate()).length())
		{ MESSAGE_ERROR("", "", "fail to render template 1c subcontractor payment file"); }
	}

	if(error_message.empty())
	{
		if((error_message = SaveFile()).length())
		{ MESSAGE_ERROR("", "", "fail to save 1c subcontractor payment file"); }
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Print_1C_Subcontractor_Payment_Order::Print() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty())
	{
		if((error_message = RenderTemplate()).length())
		{ MESSAGE_ERROR("", "", "fail to render template 1c subcontractor payment order file"); }
	}

	if(error_message.empty())
	{
		if((error_message = SaveFile()).length())
		{ MESSAGE_ERROR("", "", "fail to save 1c subcontractor payment order file"); }
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

ostream& operator<<(ostream& os, const C_Print_1C_Subcontractor_Base &var)
{
	os << "object C_Print_1C_Subcontractor_Base [empty for now]";

	return os;
}



