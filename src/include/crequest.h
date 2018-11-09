#ifndef __CREQUEST__H__
#define	__CREQUEST__H__

using namespace std;

#include "cfiles.h"
#include "cvars.h"
#include "clog.h"
#include "localy.h"
#include "utilities.h"

class CURLMethod;
class CPut;
class CGet;
class CRequest;

class CURLMethod
{
    protected:
	int					paramCount;
    public:
						CURLMethod();
	virtual	int			CalculateVars() = 0;
	virtual	int			ParamCount() = 0;
	virtual	char*		ParamName(int i) = 0;
	virtual	char*		ParamValue(int i) = 0;
	virtual	int			ParamValueSize(int i) = 0;
	virtual	bool		isFileName(int i) = 0;
	virtual	string		GetFileName(int i) = 0;
	virtual				~CURLMethod();
};

class CGet : public CURLMethod
{
		char		*queryString;
    public:
					CGet();
		int			CalculateVars();
		int			ParamCount();
		char*		ParamName(int i);
		char*		ParamValue(int i);
		int			ParamValueSize(int i);
		bool		isFileName(int i) { return false; };
		string		GetFileName(int i) { return(""); };
					~CGet();
};

class CPost : public CURLMethod
{
	private:
		char		*queryString;
		int			contentLength;
		string		boundaryMarker;
		char*		Binary_strstr(const char* where, const char* what, unsigned int c);

		bool		isContentMultipart();
		string		GetBoundaryMarker();


    public:
					CPost();
		int			CalculateVars();
		int			ParamCount();
		char*		ParamName(int i);
		char*		ParamValue(int i);
		int			ParamValueSize(int i);
		bool		isFileName(int i);
		string		GetFileName(int i);
					~CPost();
};

class CRequest
{
	CURLMethod	*url;
	CVars		*vars;
	CFiles		*files;
	char		*requestURI;
	
    public:
	    		CRequest();

	void		RegisterURLVariables(CVars *v, CFiles *f);

	//Encode "%2B%3F%AA%AA%90+world" to "privet world"
	//In: string
	//Out: string
	string		WebString(string str);
			
				~CRequest();
};

#endif
