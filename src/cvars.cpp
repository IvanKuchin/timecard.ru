#include "cvars.h"

CVars::CVars()
{
}

bool CVars::Redefine(string name, string value)
{
    bool        bResult = false;

    if(name.length() == 0)
    {
        CLog log;
        log.Write(ERROR, "CVars::" + string(__func__) + "(" + name + ")[" + to_string(__LINE__) + "]:ERROR: can't add variable with NULL name");
        return bResult;
    }

    if(find(name) != end())
    {
        CLog    log;
        log.Write(DEBUG, "CVars::" + string(__func__) + "(" + name + ")[" + to_string(__LINE__) + "]: variable [" + name + "(" + value + ")] already exist with other value. Redefine it.");

    	Delete(name);
    }

    CVars::iterator     lb = lower_bound(name);
    insert(lb, value_type(name, value));
    bResult = true;

    return bResult;
}

bool CVars::Add(string name, string value)
{
    bool	bResult = false;

    if(name.length() == 0)
    {
    	CLog log;
    	log.Write(ERROR, "CVars::" + string(__func__) + "(" + name + ")[" + to_string(__LINE__) + "]:ERROR:can't add variable with NULL name");
    	return bResult;
    }
    
    if(find(name) != end())
    {
    	CLog	log;
    	log.Write(DEBUG, "CVars::" + string(__func__) + "(" + name + ")[" + to_string(__LINE__) + "]: variable [" + name + "(" + value + ")] already exist with other value");
    	return bResult;
    }
    
    CVars::iterator	lb = lower_bound(name);
    insert(lb, value_type(name, value));
    bResult = true;
    
    return bResult;
}

bool CVars::Delete(string name)
{
    bool	bResult = false;
    
    try
    {
	CVars::iterator	itr = find(name);
	
	if(itr != end())
	    erase(itr);
	bResult = true;
    }
    catch(...)
    {
	bResult = false;
    }
    return bResult;
}

string CVars::Get(string name)
{
    string sResult;

    try
    {
        CVars::iterator itr = find(name);
        if(itr == end())
            sResult = "";
        else
            sResult = itr->second;
    }
    catch(...)
    {
        sResult = "";
    }
    
    return sResult;
}

string  CVars::GetNameByRegex(regex r)
{
    string  result = "";

    for (auto it=begin(); it!=end(); ++it)
    {
        smatch  m;

        if(regex_search(it->first, m, r))
        {
            result = it->first;
            break;
        }
    }

    return result;
}

//returned val
// true: error
// false: success
bool CVars::Set(string name, string val)
{
    bool	result = false;
    
    if(name.length() == 0)
    {
	CLog log;
	log.Write(ERROR, "try to delete empty variable");
	return result;
    }
    
    erase(name);
    CVars::iterator	lb = lower_bound(name);
    insert(lb, value_type(name, val));
    result = false;

    return result;    
}

int CVars::Count()
{
    return size();
}

CVars::~CVars()
{
}
