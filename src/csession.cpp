#include "csession.h"

CSession::CSession() : db(NULL), cookies(NULL)
{
	struct	timeval	tv;

	{
			CLog	log;
			log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec * tv.tv_usec * 100000);

	sessID = GetRandom(SESSION_LEN);

	MMDB_usage = false;

	{
			CLog	log;
			log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: end");
	}
}

string CSession::GetRandom(int len)
{
	string	result;
	int	i;

	for(i = 0; i < len; i++)
	{
		result += (char)('0' + (int)(rand()/(RAND_MAX + 1.0) * 10));
	}

	return result;
}

void CSession::InitMaxMind()
{
	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}

	MMDB_usage = false;

#ifndef MAXMIND_DISABLE
	if(getenv("REMOTE_ADDR"))
	{
		int		status;

		{
			string	temp = MMDB_fname;
		    status = MMDB_open(temp.c_str(), MMDB_MODE_MMAP, &mmdb);
		}
	    if (status == MMDB_SUCCESS)
	    {
	    	int gai_error, mmdb_error;

			MMDB_usage = true;
			{
				CLog	log;
				log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: MMDB_open(" + MMDB_fname + ") opened succesfully. ");
			}

		    MMDB_result = MMDB_lookup_string(&mmdb, getenv("REMOTE_ADDR"), &gai_error, &mmdb_error);

		    if (0 != gai_error) {
		    	{
					CLog	log;
					log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: MMDB_open(" + MMDB_fname + ") Error from getaddrinfo for " + getenv("REMOTE_ADDR") + ", " + gai_strerror(gai_error) + ". ");
				}

				MMDB_usage = false;
				MMDB_close(&mmdb);
		    }

		    if (MMDB_SUCCESS != mmdb_error) {
		    	{
					CLog	log;
					log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: MMDB_open(" + MMDB_fname + ") Got an error from libmaxminddb: " + MMDB_strerror(mmdb_error));
				}

				MMDB_usage = false;
				MMDB_close(&mmdb);
		    }

		    MMDB_entry_data_list = NULL;
		    if (MMDB_result.found_entry) {
		        int status = MMDB_get_entry_data_list(&MMDB_result.entry, &MMDB_entry_data_list);

		        if (MMDB_SUCCESS != status) {
			    	{
						CLog	log;
						log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: MMDB_open(" + MMDB_fname + ") : Got an error looking up the entry data -" + MMDB_strerror(status));
					}

					MMDB_usage = false;
					MMDB_close(&mmdb);
		        }

		    }


	    }
	    else
	    {
			CLog	log;
			log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: in reading GeoInfo DB MMDB_open(" + MMDB_fname + ") error is: " + MMDB_strerror(status) + ". ");

			MMDB_usage = false;

	        if (MMDB_IO_ERROR == status)
	        {
				CLog	log;
				log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: MMDB_open(" + MMDB_fname + ") returned MMDB_IO_ERROR: " + strerror(status) + ". ");
	        }
	    }

	}
	else
	{
	    {
			CLog	log;
			log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: REMOTE_ADDR is empty, no way to determine remote IP");
		}
	}
#endif

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: finish (MMDB_usage = " + to_string(MMDB_usage) + ")");
	}
}

string CSession::GetID()
{
	return sessID;
}

void CSession::SetID(string id)
{
	sessID = id;
}

string CSession::GetUser()
{
	return user;
}

void CSession::SetUser(string u)
{
	user = u;
}

string CSession::GetIP()
{
	return ip;
}

void CSession::SetIP(string i)
{
	ip = i;
}

string CSession::GetLng()
{
	return lng;
}

void CSession::SetLng(string l)
{
	lng = l;
}

void CSession::SetDB(CMysql *mysql)
{
	db = mysql;
}

int	 CSession::GetExpire()
{
	return expire;
}

void CSession::SetExpire(int i)
{
	expire = i;
}

string CSession::DetectItem(string MMDB_itemName) {
	string		item = "";

#ifndef MAXMIND_DISABLE
	if(MMDB_usage)
	{
        MMDB_entry_data_s       MMDB_entryDataS;

		if(MMDB_get_value(&MMDB_result.entry, &MMDB_entryDataS, MMDB_itemName.c_str(), "names", "ru", NULL) == MMDB_SUCCESS)
		{
			if(MMDB_entryDataS.has_data)
			{
				char    itemName[1024] = {0};

				if(convert_utf8_to_windows1251(MMDB_entryDataS.utf8_string, itemName, MMDB_entryDataS.data_size))
				{
					CLog	log;
					log.Write(DEBUG, "CSession::" + string(__func__) + "(" + MMDB_itemName + ")[" + to_string(__LINE__) + "]: result is " + itemName + ". ");

					item = itemName;
				}
				else
				{
					CLog	log;
					log.Write(ERROR, "CSession::" + string(__func__) + "(" + MMDB_itemName + ")[" + to_string(__LINE__) + "]:ERROR: in converting from UTF8 to CP1251. ");
				}
			}
		}
	}

#endif
	return	item;
}

string CSession::DetectCountry()
{
	string country;

	country = DetectItem("country");

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: auto detection country by IP");
    }

	return country;
}

string CSession::DetectCity()
{
	string city;

	city = DetectItem("city");
	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: auto detection city by IP");
    }

	return city;
}

bool CSession::Save(string u, string i, string l)
{
	SetUser(u);
	SetIP(i);
	SetLng(l);
	// --- try to detect IP address on new session
	InitMaxMind();
	return Save();
}

bool CSession::Save()
{
	ostringstream	ost;

	if(!db)
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: connect to database in CSession module");

		throw CExceptionHTML("error db");
	}

	if(GetUser().empty())
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: user name must be set in session::Save");

		throw CExceptionHTML("session error");
	}
	if(GetID().empty())
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: id must be set in session::Save");

		throw CExceptionHTML("session error");
	}
	if(GetIP().empty())
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: ip must be set in session::Save");

		throw CExceptionHTML("session error");
	}
	if(GetLng().empty())
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: lng must be set in session::Save");

		throw CExceptionHTML("session error");
	}

	if(db->Query("INSERT INTO `sessions` (`id`, `user`, `country_auto`, `city_auto`, `lng`, `ip`, `time`,`expire` ) VALUES ('" + GetID() + "', '" + GetUser() + "', '" + DetectCountry() + "', '" + DetectCity() + "', '" + GetLng() + "', '" + GetIP() + "', NOW(), '" + to_string(SESSION_LEN * 60) + "')") != 0) {
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: in insert SQL-query");

		return false;
	}

	return true;
}



bool CSession::Load(string id)
{
	ostringstream	ost;
	string			currIP;
	bool			result = true;

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: start");
	}

	if(id.find(",") != string::npos)
	{
		string		ip = (getenv("REMOTE_ADDR") ? getenv("REMOTE_ADDR") : "");

		if(isBotIP(ip))
		{
			CLog	log;
			log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: wrong cookie behavior from search engine bot [" + ip + "]");
		}
		else
		{
			CLog	log;
			log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: \"sessid\" got the wrong value (" + id + "). (probably bot, check IP in whois DB)");
		}
	}

	if(isExist(id))
	{
		// --- DB should not return NULL value
		// --- all fields must have "not null"-attribute
		SetID(id);
		SetUser(db->Get(0, "user"));
		SetIP(db->Get(0, "ip"));
		SetLng(db->Get(0, "lng"));
		SetExpire(stoi(db->Get(0, "expire")));

		{
			CLog	log;
			log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: session loaded for user [" + GetUser() + "]");
		}
	}
	else
	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: there is no session in DB, session hasn't been loaded from DB");

		result = false;
	}


	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: end (result: " + (result ? "true" : "false") + ")");
	}

	return result;
}

bool CSession::CheckConsistency() {
	bool		result = true;

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: start");
	}
	// --- Check IP address changing
	if(getenv("REMOTE_ADDR"))
	{
		string	currIP = getenv("REMOTE_ADDR");
		string	oldIP = GetIP();

		if(currIP != oldIP)
		{
			if(UpdateIP(currIP))
			{
				CLog	log;
				log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: user (" + GetUser() + ") IP was changed (" + oldIP + " -> " + currIP + ") during the session.");
			}
			else
			{
				CLog	log;
				log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: fail to update IP (" + oldIP + " -> " + currIP + ")");
			}
		}
	}
	else
	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: environment variable 'REMOTE_ADDR' doesn't defined");
	}

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]: end (result: " + (result ? "true" : "false") + ")");
	}
	return result;
}

bool CSession::Update() {

	if(GetUser() == "")
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: session [" + GetID() + "], user is empty");

		return false;
	}

	if(GetID() == "")
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: sessionID is empty");

		return false;
	}

	if(db->Query("UPDATE `sessions` SET `time`=NOW() WHERE `id`=\"" + GetID() + "\";")) 
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: session [" + GetID() + "] there are more than one session with the same session ID.");

		return false;
	}

	return true;
}

bool CSession::UpdateIP(string newIP) {
	ostringstream	ost;

	if(GetUser() == "")
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: session [" + GetID() + "], user is empty");

		return false;
	}

	if(GetID() == "")
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: session ID is empty");

		return false;
	}

	// --- try to detect IP address on session with changed IP-address
	// --- call stack here
	// --- CCgi::SessID_CheckConsistency
	// --- CSession::CheckConsistency
	// --- CSession::UpdateDB (here)
	InitMaxMind();

	if(db->Query("UPDATE `sessions` SET `ip`=\"" + newIP + "\", `country_auto`=\"" + DetectCountry() + "\", `city_auto`=\"" + DetectCity() + "\" WHERE `id`=\"" + GetID() + "\";"))
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: session [" + GetID() + "] there are more than one session with the same session ID.");

		return false;
	}
	SetIP(newIP);

	return true;
}

bool CSession::isExist(string id)
{
	ostringstream	ost;
	string		currIP;

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: start");
	}

	if(!db)
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]:ERROR:ERROR: connect to database in CSession::Load module");

		throw CExceptionHTML("error db");
	}
	if(id.empty())
	{
		CLog	log;
		log.Write(ERROR, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]:ERROR: id must be set in session::Load");

		throw CExceptionHTML("activator error");
	}

	db->Query("delete from `sessions` where `time`<=(NOW()-interval `expire` second) and `expire`>'0'");
	if(db->Query("select * from `sessions` where `id`=\"" + id + "\";"))
	{
	}
	else
	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: end (return: false)");

		return false;
	}

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(" + id + ")[" + to_string(__LINE__) + "]: end (return: true)");
	}

	return true;
}

CSession::~CSession()
{
	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(): start");
	}

#ifndef MAXMIND_DISABLE

	if(MMDB_usage) {
		MMDB_close(&mmdb);
	}

#endif

	{
		CLog	log;
		log.Write(DEBUG, "CSession::" + string(__func__) + "(): end");
	}
}





