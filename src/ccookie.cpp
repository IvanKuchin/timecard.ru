#include "ccookie.h"

CCookie::CCookie()
{
	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}
}

CCookie::CCookie(string n, string v) : name(n), value(v)
{
	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}
}

void	CCookie::SetName(string s) { 
	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	name = s; 

	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}
}
void	CCookie::SetValue(string s) {
	{
		CLog	log;

		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: name = " + GetName() + ", value = " + s);
	}
	value = s;
}
void	CCookie::SetDomain(string s) { domain = s; }
void	CCookie::SetPath(string s) { path = s; }

/*
Header mismatch:
	Expected: Set-Cookie: name=value; expires=Thu, 24-Apr-2014 22:36:43 GMT; Max-Age=5
	Received: Set-Cookie: name=value; expires=Thu, 24-Apr-2014 22:36:43 GMT
*/
void	CCookie::SetExpiration(string s) { expiration = s; }
void	CCookie::SetMaxAge(int s) { maxAge = s; }
void	CCookie::SetSecure(string s) { secure = s; }
void	CCookie::SetNew(bool s) 
{
	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: name = " + GetName());
	}
	isNew = s;
}

string	CCookie::GetName() { return name; }
string	CCookie::GetValue() { return value; }
string	CCookie::GetDomain() { return domain; }
string	CCookie::GetPath() { return path; }
string	CCookie::GetExpiration() { return expiration; }
int		CCookie::GetMaxAge() { return maxAge; }
string	CCookie::GetSecure() { return secure; }
bool	CCookie::GetNew() { return isNew; }


CCookie::~CCookie()
{
	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: destructor for " + GetName());
	}
}

CCookies::CCookies()
{
	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	cookies.reserve(8);

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: size of cookies=" + to_string(cookies.size()) + " object after reservation");
	}

}

void CCookies::Add(string name, string value, string expiration, string domain, string path, string secure, bool newCookie)
{
	CCookie		*c;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "] start (# of cookies are " + to_string(cookies.size()) + " objects)");
	}

	if((cookies.capacity() - cookies.size()) <= 0)
	{
		CLog	log;
		log.Write(ERROR, "Size of cookie vector is overflow. May be reallocating memory.");
	}

	c = new(CCookie);
	if(!c)
	{
		CLog	log;
		log.Write(ERROR, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "] error allocating memory (cookie module)");
		throw CException("error allocating memory (cookie module)");
	}

	c->SetName(name);
	c->SetValue(value);
	c->SetExpiration(expiration);
	// c->SetMaxAge(0);
	c->SetDomain(domain);
	c->SetPath(path);
	c->SetSecure(secure);
	c->SetNew(newCookie);

	cookies.push_back(c);

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "] end (size of cookies=" + to_string(cookies.size()) + " object)");
	}

}

void CCookies::Add(string name, string value, string expiration, int maxAge, string domain, string path, string secure, bool newCookie)
{
	CCookie		*c;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "] start (""name=" + name + " value=" + value + " exp=" + expiration + " maxAge=" + to_string(maxAge) + " domain=" + domain + " path=" + path + " secure=" + secure + " newCookie=" + to_string(newCookie) + ")");
	}

	if((cookies.capacity() - cookies.size()) <= 0)
	{
		CLog	log;
		log.Write(ERROR, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "] Size of cookie vector is overflow. May be reallocating memory. (cookies.capacity()=" + to_string(cookies.capacity()) + " cookies.size()=" + to_string(cookies.size()) + " )");
	}

	c = new(CCookie);
	if(!c)
	{
		CLog	log;
		log.Write(ERROR, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "] error allocating memory (cookie module)");
		throw CException("error allocating memory (cookie module)");
	}

	c->SetName(name);
	c->SetValue(value);
	c->SetExpiration((expiration == "") ? GetTimestampShifted(maxAge) : expiration);
	c->SetMaxAge(maxAge);
	c->SetDomain(domain);
	c->SetPath(path);
	c->SetSecure(secure);
	c->SetNew(newCookie);

	cookies.push_back(c);

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]end (size of cookies=" + to_string(cookies.size()) + " object)");
	}
}

bool CCookies::IsExist(string name)
{
	bool	result;
	vector<CCookie *>::iterator	im;

	result = false;
	for(im = cookies.begin(); im < cookies.end(); ++im)
	{
		if((*im)->GetName() == name)
		{
			result = true;
			break;
		}
	}

	return result;
}

// --- input: deltaTimeStamp in seconds
// --- return: string with shifted timestamp
string CCookies::GetTimestampShifted(int deltaTimeStamp)
{
	time_t		t;
	char		utc_str[100];
	struct tm	*utc_tm;
	string		result = "";

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: start (deltaTimeStamp = " + to_string(deltaTimeStamp) + ")");
	}

	t = time(NULL) + deltaTimeStamp;
	utc_tm = gmtime(&t);
	if(utc_tm)
	{
		if(strftime(utc_str, sizeof(utc_str), "%a, %02d %b %Y %T %Z", utc_tm))
		{
			result = utc_str;
		} 
		else
		{
			CLog	log;
			log.Write(ERROR, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: in running strftime)(" + utc_str + ", " + to_string(sizeof(utc_str)) + ", '', utc_tm) buffer size is not enough to convert date to GMT");
		}
	}
	else
	{
		CLog	log;
		log.Write(ERROR, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: in running gmtime(&t)  (possible problem with cookie expiration in IE)");
	}


	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: return (result = " + result + ")");
	}

	return result;
}

// --- shift timestamp and update cookies
// --- name - cookie name
// --- deltaTimeStamp - delta time in minutes
bool CCookies::UpdateTS(string name, int deltaTimeStamp)
{
	string						value, path, timestamp, domain, secure;
	vector<CCookie *>::iterator	im;
	bool						result = true;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: start (name = " + name + ", deltaTimeStamp = " + to_string(deltaTimeStamp) + ")");
	}

	if(IsExist(name)) {
		value = Get(name);
		timestamp = GetTimestampShifted(deltaTimeStamp * 60);
		path = "/";
		domain = "";
		secure = "";

		for(im = cookies.begin(); im < cookies.end(); ++im)
		{
			if((*im)->GetName() == name)
			{
				(*im)->SetValue(value);
				(*im)->SetDomain(domain);
				(*im)->SetPath(path);
				(*im)->SetSecure(secure);
				(*im)->SetNew(true);
				if(deltaTimeStamp)
				{
					(*im)->SetExpiration(timestamp);
					(*im)->SetMaxAge(deltaTimeStamp * 60);
				}
				else
				{
					(*im)->SetExpiration("");
				}
				break;
			}
		}

	}
	else 
	{
		{
			CLog	log;
			log.Write(ERROR, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: unable to find cookie (name = " + name + ")");
		}

		result = false;
	} // if(IsExist(name)) 

	{
		CLog	log;
		log.Write(DEBUG, "CCookie::" + string(__func__) + "[" + to_string(__LINE__) + "]: finish (" + to_string(result) + ")");
	}

	return result;
}

void CCookies::Modify(string name, string value, string expiration, string domain, string path, string secure)
{
	vector<CCookie *>::iterator	im;

	for(im = cookies.begin(); im < cookies.end(); ++im)
	{
		if((*im)->GetName() == name)
		{
			(*im)->SetValue(value);
			(*im)->SetExpiration(expiration);
			(*im)->SetDomain(domain);
			(*im)->SetPath(path);
			(*im)->SetSecure(secure);
			(*im)->SetNew(true);
			break;
		}
	}
}

void CCookies::Delete(string name, string domain, string path, string secure)
{
// TODO delete Jun 1
/*
	time_t		t = time(NULL) - 3600 * 24 * 30 * 12;
	char		date[100];

	memset(date, 0, sizeof(date));
	strftime(date, sizeof(date) - 2, "%a, %d %b %Y %X %Z", localtime(&t));
*/
	for(auto im = cookies.begin(); im < cookies.end(); ++im)
	{
		if((*im)->GetName() == name)
		{
			(*im)->SetValue((*im)->GetValue());
			(*im)->SetExpiration(GetTimestampShifted(-3600 * 24));
			(*im)->SetPath(path);
			(*im)->SetMaxAge(0);
			(*im)->SetNew(TRUE);
//			break;
		}
	}
}

string CCookies::Get(string name)
{
	string result;
	vector<CCookie *>::iterator	im;

	result = "";
	for(im = cookies.begin(); im < cookies.end(); ++im)
	{
		if((*im)->GetName() == name)
		{
			result = (*im)->GetValue();
			break;
		}
	}


	return result;
}

string CCookies::GetAll()
{
	string 						result;
	vector<CCookie *>::iterator	im;
	ostringstream				ost;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	result = "";
	for(im = cookies.begin(); im < cookies.end(); ++im)
	{
		if((*im)->GetNew())// does this cookie needs to be sendback to server ?
		{
			string	temp;

			temp = "Set-Cookie: ";
			temp += (*im)->GetName();
			temp += "=";
			temp += (*im)->GetValue();
			if((*im)->GetExpiration().length() > 0)
			{
				temp += "; ";
				temp += "expires=";
				temp += (*im)->GetExpiration();
			}
			if((*im)->GetMaxAge() >= 0)
			{
				ost.str("");
				ost << "; max-age=" << (*im)->GetMaxAge();
				temp += ost.str();
			}
			if((*im)->GetPath().length() > 0)
			{
				temp += "; ";
				temp += "path=";
				temp += (*im)->GetPath();
			}
			temp += "\n";

			result += temp;

			{
				CLog	log;
				log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: add cookie: " + temp);
			}
		}
	}
	
	if(result.length() > 1)	result.erase(result.length() - 1);

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: finish (result len = " + to_string(result.length()) + ")");
	}
	return result;
}

void CCookies::RegisterCookieVariables(CVars *v)
{
	vector<CCookie *>::iterator	im;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: start ");
	}

	for(im = cookies.begin(); im < cookies.end(); ++im)
		if((*im)->GetName().length())
			v->Add((*im)->GetName(), (*im)->GetValue());
		else
		{
			{
				CLog	log;
				log.Write(ERROR, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: cookie name is empty (" + (*im)->GetName() + " = " + (*im)->GetValue() + ")");
			}
		}

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: finish ");
	}
}

void CCookies::ParseString(string str)
{
	string				result, expr, cookName, cookValue;
	string::size_type	exprPos, eqPos;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "(" +  str + ")[" + to_string(__LINE__) + "]: start ");
	}

	result = str;
	exprPos = result.find(";");

	while((exprPos != string::npos) || (result.length() > 2))
	{
		if(exprPos != string::npos)
		{
			expr = result.substr(0, exprPos);
			result = result.erase(0, exprPos + 1);
		}
		else
		{
			expr = result;
			result.erase(0, result.length());
		}

		eqPos = expr.find("=");
		if(eqPos == string::npos)
		{
			exprPos = result.find(";");
			continue;
		}

		cookName = expr.substr(0, eqPos);
		cookValue = expr.substr(eqPos + 1, expr.length() - eqPos - 1);
		trim(cookName);
		trim(cookValue);

		{
			CLog	log;
			log.Write(DEBUG, "CCookies::" + string(__func__) + "(" +  str + ")[" + to_string(__LINE__) + "]: adding cookie: " + cookName + " = " + cookValue);
		}

		if(cookName.length())
			Add(cookName, cookValue, "", "", "", "", FALSE);
		else
		{
			{
				CLog	log;
				log.Write(ERROR, "CCookies::" + string(__func__) + "(" +  str + ")[" + to_string(__LINE__) + "]:ERROR: cookie parsing end-up with empty name() and value(" + cookValue + ")");
			}

		}

		exprPos = result.find(";");
	}

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: end ");
	}
}

CCookies::~CCookies()
{
	vector<CCookie *>::iterator	iv;

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	for(iv = cookies.begin(); iv < cookies.end(); ++iv)
	{
		delete(*iv);
	}

	{
		CLog	log;
		log.Write(DEBUG, "CCookies::" + string(__func__) + "[" + to_string(__LINE__) + "]: finish");
	}
}
