#ifndef __UTILITIES_COMMON__H__
#define __UTILITIES_COMMON__H__

#include "ccgi.h"
#include "cmysql.h"
#include "cuser.h"

using namespace std;

bool 		RegisterInitialVariables(CCgi *, CMysql *, CUser *);
string 		GenerateSession(string action, CCgi *, CMysql *, CUser *);
string 		LogoutIfGuest(string action, CCgi *, CMysql *, CUser *);

#endif
