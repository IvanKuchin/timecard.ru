#include "wkhtmltox_wrapper.h"

/* Print out loading progress information */
static void wkhtmltox_progress_changed(wkhtmltopdf_converter * c, int p) {
    MESSAGE_DEBUG("", "", "progress " + to_string(p) + "%");
}

/* Print loading phase information */
static void wkhtmltox_phase_changed(wkhtmltopdf_converter * c) {
    int phase = wkhtmltopdf_current_phase(c);

    MESSAGE_DEBUG("", "", "phase: " + wkhtmltopdf_phase_description(c, phase));
}

/* Print a message to stderr when an error occurs */
static void wkhtmltox_error(wkhtmltopdf_converter * c, const char * msg) {
    MESSAGE_ERROR("", "", msg);
}

/* Print a message to stderr when a warning is issued */
static void wkhtmltox_warning(wkhtmltopdf_converter * c, const char * msg) {
    MESSAGE_ERROR("", "", msg);
}


auto	wkhtmltox_wrapper::Convert() -> string
{
	auto			error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(isFileExists(GetSrc()))
	{
		if(isFileExists(GetDst()))
		{
			MESSAGE_DEBUG("", "", "pdf file is about to render already exists, remove existsing file.");
			unlink(GetDst().c_str());
		}

		{
			wkhtmltopdf_global_settings	*gs = nullptr;
			wkhtmltopdf_object_settings	*os = nullptr;
			wkhtmltopdf_converter 		*c = nullptr;

			/*
			 * Create a global settings object used to store options that are not
			 * related to input objects, note that control of this object is parsed to
			 * the converter later, which is then responsible for freeing it
			 */
			gs = wkhtmltopdf_create_global_settings();
			/* We want the result to be storred in the file called test.pdf */
			wkhtmltopdf_set_global_setting(gs, "out", GetDst().c_str());

			// wkhtmltopdf_set_global_setting(gs, "web.defaultEncoding", "utf-8");
			/*
			 * Create a input object settings object that is used to store settings
			 * related to a input object, note again that control of this object is parsed to
			 * the converter later, which is then responsible for freeing it
			 */
			os = wkhtmltopdf_create_object_settings();
			/* We want to convert to convert the qstring documentation page */
			wkhtmltopdf_set_object_setting(os, "page", GetSrc().c_str());

			/* Create the actual converter object used to convert the pages */
			c = wkhtmltopdf_create_converter(gs);

			/* Call the progress_changed function when progress changes */
			wkhtmltopdf_set_progress_changed_callback(c, wkhtmltox_progress_changed);

			/* Call the phase _changed function when the phase changes */
			wkhtmltopdf_set_phase_changed_callback(c, wkhtmltox_phase_changed);

			/* Call the error function when an error occurs */
			wkhtmltopdf_set_error_callback(c, wkhtmltox_error);

			/* Call the warning function when a warning is issued */
			wkhtmltopdf_set_warning_callback(c, wkhtmltox_warning);

			/*
			 * Add the the settings object describing the qstring documentation page
			 * to the list of pages to convert. Objects are converted in the order in which
			 * they are added
			 */
			wkhtmltopdf_add_object(c, os, NULL);

			/* Perform the actual conversion */
			if (!wkhtmltopdf_convert(c))
			{
				error_message = gettext("html to pdf conversation failed");
				MESSAGE_ERROR("", "", error_message);
			}

			/* Destroy the converter object since we are done with it */
			wkhtmltopdf_destroy_converter(c);

			// --- remove template file
			// unlink((GetFilename() + ".html").c_str());
		}
	}
	else
	{
		error_message = gettext("temporary file doesn't exists");
		MESSAGE_ERROR("", "", error_message);
	}

	MESSAGE_DEBUG("", "", "finish (error_message.length() = " + to_string(error_message.length()) + ")");

	return error_message;
}

ostream& operator<<(ostream& os, const wkhtmltox_wrapper &var)
{
	os << "object wkhtmltox_wrapper [empty for now]";

	return os;
}



