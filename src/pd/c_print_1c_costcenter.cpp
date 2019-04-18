#include "c_print_1c_costcenter.h"


auto	C_Print_1C_CostCenter::RenderTemplate() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	// --- build table for service invoice
	if(error_message.empty())
	{
		if(vars->Get("1C_template_invoice_to_cc_service_table_row_full_path").length())
		{
			auto	table_content = ""s;

			for(auto i = 1; isTableRowExists(i); ++i)
			{
				CCgi		render_obj;
				CVars		vars_obj = vars->GetVars();

				vars_obj.SetIndex(to_string(i));
				render_obj.SetVars(vars_obj);

				if(render_obj.SetTemplateFile(vars->Get("1C_template_invoice_to_cc_service_table_row_full_path")))
				{
					table_content += render_obj.RenderTemplate();
				}
				else
				{
					error_message = gettext("1c template not found");
					MESSAGE_ERROR("", "", error_message);
				}

				if(error_message.length()) break;
			}

			vars->AssignVariableValue("Table_Service_Invoice_To_CostCenter_1C", table_content, true);
		}
		else
		{
			MESSAGE_DEBUG("", "", "1c filename is empty");
		}
	}

	// --- build service invoice
	if(error_message.empty())
	{
		if(vars->Get("1C_template_invoice_to_cc_service_full_path").length())
		{
			CCgi		render_obj;
			CVars		vars_obj = vars->GetVars();

			render_obj.SetVars(vars_obj);

			if(render_obj.SetTemplateFile(vars->Get("1C_template_invoice_to_cc_service_full_path")))
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

auto	C_Print_1C_CostCenter::SaveFile() -> string
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

auto	C_Print_1C_CostCenter_Service::Print() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(error_message.empty())
	{
		if((error_message = RenderTemplate()).length())
		{ MESSAGE_ERROR("", "", "fail to render template 1c cost center service file"); }
	}

	if(error_message.empty())
	{
		if((error_message = SaveFile()).length())
		{ MESSAGE_ERROR("", "", "fail to save 1c cost center service file"); }
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

auto	C_Print_1C_CostCenter_BT::Print() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");



	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

ostream& operator<<(ostream& os, const C_Print_1C_CostCenter &var)
{
	os << "object C_Print_1C_CostCenter [empty for now]";

	return os;
}



