#include "c_cache_obj.h"

auto c_cache_obj::Get(string db_query, CMysql *db, CUser *user, string (*func)(string, CMysql *, CUser *)) -> string
{
	auto	result = cache[db_query];

	MESSAGE_DEBUG("", "", "start (" + db_query + ")");

	if(result.length())
	{
		cache_hits++;
	}
	else
	{
		if(db)
		{
			if(user)
			{
				cache[db_query] = func(db_query, db, user);
				result = cache[db_query];
				cache_misses++;
			}
			else
			{
				MESSAGE_ERROR("", "", "user is not initialized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db not initialized");
		}
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

c_cache_obj::~c_cache_obj()
{
	MESSAGE_DEBUG("", "", "cache hits = " + to_string(cache_hits) + ", cache misses = " + to_string(cache_misses) + (cache_hits + cache_misses ? ", cache performance = " + to_string(cache_hits * 100 / (cache_hits + cache_misses)) + "%" : ""));
}