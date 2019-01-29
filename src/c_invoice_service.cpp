#include "c_invoice_service.h"

static void error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
}

C_Invoice_Service::C_Invoice_Service() {}

C_Invoice_Service::C_Invoice_Service(CMysql *param1, CUser *param2) : db(param1), user(param2) {}

auto C_Invoice_Service::GenerateDocumentArchive() -> string
{
	auto		error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(CreateTempDirectory())
	{

	}
	else
	{
		MESSAGE_ERROR("", "", "fail to create temp directory");
		error_message = utf8_to_cp1251(gettext("fail to create temp directory"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

auto C_Invoice_Service::CreateTempDirectory() -> bool
{
	auto result = false;

	do
	{
		temp_dir = TEMP_DIRECTORY_PREFIX + GetRandom(15);
	} while(isDirExists(temp_dir));

	if(CreateDir(temp_dir))
	{
		result = true;
	}
	else
	{
		MESSAGE_ERROR("", "", "fail to create " + temp_dir);
	}

	return result;
}

auto C_Invoice_Service::GeneratePDF() -> string
{
	HPDF_Doc  pdf;

	pdf = HPDF_NewEx (error_handler, NULL, NULL, 0, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
    }
    HPDF_Free (pdf);

    return "ok";
}

auto C_Invoice_Service::GenerateXL() -> string
{
	using namespace libxl;

	Book*			book = xlCreateBook();
	string			s1 = "qwe";

    if (book) {
    	book->release();
    }

    return "ok";
}

C_Invoice_Service::~C_Invoice_Service()
{
	if(temp_dir.length())
	{
		RmDirRecursive(temp_dir.c_str());
	}
	else
	{
		MESSAGE_DEBUG("", "", "temp_dir is not defined, no need to remove it");
	}
}

ostream& operator<<(ostream& os, const C_Invoice_Service &var)
{
	os << "object C_Invoice_Service [empty for now]";

	return os;
}

