#ifndef __C_CONFIG__H__
#define __C_CONFIG__H__

#include <iostream>
#include <fstream>
#include <regex>
#include <tuple>
#include <vector>
#include <map>

#include "clog.h"

using namespace std;

class c_config
{
	protected:
		string					config_file		=	""s;
		tuple<string, string>	ExtractSingleValue(const string &line, const vector<string> &params);

	public:
								c_config(const string &file) : config_file {file} {};
		void					SetFile(const string &param) { config_file = param; };
		map<string, string>		Read(const vector<string> &params);
};

ostream&	operator<<(ostream& os, const c_config &);

#endif
