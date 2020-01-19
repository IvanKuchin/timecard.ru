#ifndef __C_TC_BT_SUBMIT__H__
#define __C_TC_BT_SUBMIT__H__

#include "cuser.h"
#include "cmysql.h"
#include "clog.h"
#include "localy.h"

using namespace std;

class C_TC_BT_Submit
{
	private: 

		struct ApproveClass
		{
			string	id;
			string	timecard_id;
			string	approver_id;
			string	decision;
			string	comment;
			string	eventTimestamp;
		};

		struct ApproverClass
		{
			string	id;
			string	approver_user_id;
			string	contract_psow_id;
			string	approver_order;
			string	auto_approve;
		};

		CMysql					*db = nullptr;
		CUser					*user = nullptr;

		string					type = "";
		string					id = "";

		vector<ApproveClass>	approvals;
		vector<ApproverClass>	approvers;

		bool					completely_approved = false;

		bool					isOrderedApprovals();
		string					FetchInitialData();
		string					FetchApprovals();
		string					FetchApprovers();
		bool					isItFirstSubmit(); 
		string					CraftApprovalList();
		bool					isItBeenRejected();
		string					RemovePendingApprovals();
		string					UpdateItToNewState(string new_state);
		string					Autoapprove();
		string					GetNextApproverID();
		string					GetCurrentApproverID();
		string					GetLastApproverID();
		bool					isItCompletelyApproved();
		bool					ApproverIDHasAutoApprove(string approver_id);



	public:
								C_TC_BT_Submit()											{};
								C_TC_BT_Submit(CMysql *p1, CUser *p2) : db(p1), user(p2)	{};

		auto					isCompletelyApproved()										{ return completely_approved; };
		string					Submit(const string tc_bt_type_param, const string &id_param);

};



#endif
