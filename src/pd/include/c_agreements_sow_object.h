#ifndef __C_AGREEMENTS_SOW_OBJECT__H__
#define __C_AGREEMENTS_SOW_OBJECT__H__

#include <string>

#include "clog.h"

using namespace std;

class C_Agreements_SoW_Object
{
	private:
		string		sow_id;
		string		id;
		string		title;
		string		filename;

	public:
		auto		SetSoWID(const string &param)			{ sow_id = param; }
		auto		SetSoWID(string &&param)				{ sow_id = move(param); }
		auto		SetID(const string &param)				{ id = param; }
		auto		SetID(string &&param)					{ id = move(param); }
		auto		SetTitle(const string &param)			{ title = param; }
		auto		SetTitle(string &&param)				{ title = move(param); }
		auto		SetFilename(const string &param)		{ filename = param; }
		auto		SetFilename(string &&param)				{ filename = move(param); }

		auto		GetSoWID()								{ return sow_id; }
		auto		GetID()									{ return id; }
		auto		GetTitle()								{ return title; }
		auto		GetFilename()							{ return filename; }
};

ostream&	operator<<(ostream& os, const C_Agreements_SoW_Object &);

#endif
