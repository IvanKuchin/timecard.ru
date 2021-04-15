#ifndef __UTILITIES_SUBMIT__H__
#define __UTILITIES_SUBMIT__H__

#include <string>
#include "cmysql.h"
#include "clog.h"
#include "cuser.h"
#include "c_invoice_service_subc_to_agency.h"
#include "c_invoice_bt_subc_to_agency.h"

auto			SubmitTimecard(string timecard_id, c_config *, CMysql *, CUser *) -> bool;
auto			SubmitBT(string bt_id, c_config *, CMysql *, CUser *) -> bool;
auto			ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, c_config *, CMysql *, CUser *) -> string;

#endif
