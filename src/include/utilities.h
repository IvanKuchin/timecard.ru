#ifndef __UTILITIES__H__
#define __UTILITIES__H__

#include <fstream>  	//--- used for CopyFile function
#include <map>
#include <vector>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <regex>
#include <cctype> 		//--- used for tolower
#include <sys/stat.h>
#include <execinfo.h>   // --- backtrace defined here
#include <signal.h>     // --- signal intercaeption
#include <iomanip>		// --- setprecision(2) in Float2StringWithPrecision
#include <openssl/sha.h>
#include <Magick++.h>

#include "c_float.h"
#include "cfiles.h"
#include "cmysql.h"
#include "cuser.h"
#include "cexception.h"
#include "clog.h"
#include "localy.h"

using namespace std;

auto			crash_handler(int sig) -> void;
auto			GetSHA512(const string &src) -> string;
auto	      	rtrim(string& str) -> string;
auto	      	ltrim(string& str) -> string;
auto	      	trim(string& str) -> string;
auto	      	quoted(string src) -> string;
auto	  		toLower(string src) -> string;
auto	      	GetRandom(int len) -> string;
auto	      	DeleteHTML(string src, bool removeBR = true) -> string;
auto	      	RemoveQuotas(string src) -> string;
auto	      	RemoveSpecialSymbols(string src) -> string;
auto	      	RemoveSpecialHTMLSymbols(string src) -> string;
auto	      	ReplaceDoubleQuoteToQuote(string src) -> string;
auto	      	ReplaceCRtoHTML(string src) -> string;
auto	      	CleanUPText(const string messageBody, bool removeBR = true) -> string;
auto	      	RemoveAllNonAlphabetSymbols(string src) -> string;
auto	      	ConvertTextToHTML(const string messageBody) -> string;
auto	 		CheckHTTPParam_Text(string srcText) -> string;
auto 			CheckHTTPParam_Number(string srcText) -> string;
auto	 		CheckHTTPParam_Date(string srcText) -> string;
auto	 		CheckHTTPParam_Float(string srcText) -> string;
auto	 		CheckHTTPParam_Email(string srcText) -> string;
auto	 		CheckHTTPParam_Timeentry(string srcText) -> string;
auto	      	CheckIfFurtherThanNow(string occupationStart_cp1251) -> string;
auto			GetDefaultActionFromUserType(string role, CMysql *) -> string;
auto	      	GetSecondsSinceY2k() -> double;
auto	      	GetLocalFormattedTimestamp() -> string;
auto	      	GetTimeDifferenceFromNow(const string timeAgo) -> double;
auto	      	GetMinutesDeclension(const int value) -> string;
auto	      	GetHoursDeclension(const int value) -> string;
auto	      	GetDaysDeclension(const int value) -> string;
auto	      	GetMonthsDeclension(const int value) -> string;
auto	      	GetYearsDeclension(const int value) -> string;
auto	      	GetHumanReadableTimeDifferenceFromNow (const string timeAgo) -> string;
auto	      	SymbolReplace(const string where, const string src, const string dst) -> string;
auto	      	SymbolReplace_KeepDigitsOnly(const string where) -> string;
auto	        qw(const string src, vector<string> &dst) -> int;
vector<string>	split(const string& s, const char& c);
auto			join(const vector<string>& vec, string separator = ",") -> string;
auto			FilterCP1251Symbols(const string &src) -> string;
auto      		UniqueUserIDInUserIDLine(string userIDLine) -> string; //-> decltype(static_cast<string>("123"))
auto      		AutodetectSexByName(string name, CMysql *) -> string;
auto			GetPasswordNounsList(CMysql *) -> string;
auto			GetPasswordAdjectivesList(CMysql *) -> string;
auto			GetPasswordCharacteristicsList(CMysql *) -> string;

auto      		GetChatMessagesInJSONFormat(string dbQuery, CMysql *) -> string;
auto      		GetUserListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto      		GetCompanyListInJSONFormat(string dbQuery, CMysql *, CUser *, bool quickSearch = true, bool includeEmployedUsersList = false) -> string;
auto      		GetGroupListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto 			GetBookListInJSONFormat(string dbQuery, CMysql *, bool includeReaders = false) -> string;
auto 			GetComplainListInJSONFormat(string dbQuery, CMysql *, bool includeReaders = false) -> string;
auto 			GetCertificationListInJSONFormat(string dbQuery, CMysql *, bool includeDevoted = false) -> string;
auto 			GetCourseListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false) -> string;
auto 			GetLanguageListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false) -> string;
auto 			GetSkillListInJSONFormat(string dbQuery, CMysql *) -> string;
auto 			GetUniversityListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false) -> string;
auto 			GetSchoolListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false) -> string;
auto 			GetNewsFeedInJSONFormat(string whereStatement, int currPage, int newsOnSinglePage, CUser *, CMysql *) -> string;
auto      		GetUnreadChatMessagesInJSONFormat(CUser *, CMysql *) -> string;
auto			GetGiftListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto 			GetGiftToGiveListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto			GetEventListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto			GetEventHostsListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto			GetEventGuestsListInJSONFormat(string dbQuery, CMysql *, CUser *) -> string;
auto			GetMySQLDateInJSONFormat(string dateString) -> string;
auto      		GetMessageImageList(string imageSetID, CMysql *) -> string;
auto      		GetMessageLikesUsersList(string messageID, CUser *, CMysql *) -> string;
auto 			GetCertificationLikesUsersList(string usersCertificationID, CUser *, CMysql *) -> string;
auto 			GetCourseLikesUsersList(string usersCourseID, CUser *, CMysql *) -> string;
auto 			GetUniversityDegreeLikesUsersList(string messageID, CUser *, CMysql *) -> string;
auto 			GetCompanyLikesUsersList(string usersCompanyID, CUser *, CMysql *) -> string;
auto 			GetLanguageLikesUsersList(string usersLanguageID, CUser *, CMysql *) -> string;
auto 			GetBookLikesUsersList(string usersBookID, CUser *, CMysql *) -> string;
auto 			GetBookRatingList(string bookID, CMysql *) -> string;
auto 			GetCourseRatingList(string courseID, CMysql *) -> string;
auto 			GetBookRatingUsersList(string bookID, CUser *, CMysql *) -> string;
auto      		GetMessageCommentsCount(string messageID, CMysql *) -> string;
auto 			GetCompanyCommentsCount(string messageID, CMysql *) -> string;
auto 			GetLanguageCommentsCount(string messageID, CMysql *) -> string;
auto 			GetBookCommentsCount(string messageID, CMysql *) -> string;
auto 			GetCertificateCommentsCount(string messageID, CMysql *) -> string;
auto 			GetUniversityDegreeCommentsCount(string messageID, CMysql *) -> string;
auto      		GetMessageSpam(string messageID, CMysql *) -> string;
auto      		GetMessageSpamUser(string messageID, string userID, CMysql *) -> string;
auto			GetLanguageIDByTitle(string title, CMysql *) -> string;
auto			GetSkillIDByTitle(string title, CMysql *) -> string;
auto			GetCompanyPositionIDByTitle(string title, CMysql *) -> string;
auto 			GetCandidatesListAppliedToVacancyInJSONFormat(string dbQuery, CMysql *) -> string;
auto 			GetOpenVacanciesInJSONFormat(string companyID, CMysql *, CUser * = NULL) -> string;
auto			GetGeoLocalityIDByCityAndRegion(string regionName, string cityName, CMysql *) -> string;
auto        	AllowMessageInNewsFeed(CUser *me, const string messageOwnerID, const string messageAccessRights, vector<string> *messageFriendList) -> bool;
auto        	isPersistenceRateLimited(string REMOTE_ADDR, CMysql *) -> bool;
auto        	isFileExists(const std::string& name) -> bool;
auto			isFilenameImage(string	filename) -> bool;
auto			isFilenameVideo(string	filename) -> bool;
auto        	CopyFile(const string src, const string dst) -> void;
auto      		GetCompanyDuplicates(CMysql *) -> string;
auto      		GetPicturesWithEmptySet(CMysql *) -> string;
auto      		GetPicturesWithUnknownMessage(CMysql *) -> string;
auto      		GetPicturesWithUnknownUser(CMysql *) -> string;
auto      		GetRecommendationAdverse(CMysql *) -> string;
auto      		GetUserAvatarByUserID(string userID, CMysql *) -> string;
auto      		GetUserNotificationSpecificDataByType(unsigned long typeID, unsigned long actionID, CMysql *, CUser *) -> string;
auto      		GetUserNotificationInJSONFormat(string sqlRequest, CMysql *, CUser *) -> string;
auto        	RemoveMessageImages(string sqlWhereStatement, CMysql *) -> void;
auto    		RemoveBookCover(string sqlWhereStatement, CMysql *) -> void;
auto    		RemoveSpecifiedCover(string itemID, string itemType, CMysql *) -> bool;
auto        	CheckUserEmailExisting(string userNameToCheck, CMysql *) -> bool;
vector<string>	GetUserTagsFromText(string srcMessage);
auto			RedirStdoutToFile(string fname) -> bool;
auto			RedirStderrToFile(string fname) -> bool;
auto 			AmIMessageOwner(string messageID, CUser *, CMysql *) -> bool;
pair<string, string> GetMessageOwner(string messageID, CUser *, CMysql *);
auto			GetUserSubscriptionsInJSONFormat(string sqlQuery, CMysql *) -> string;
auto			SubscribeToCompany(string companyID, CUser *, CMysql *) -> string;
auto			UnsubscribeFromCompany(string companyID, CUser *, CMysql *) -> string;
auto			SubscribeToGroup(string groupID, CUser *, CMysql *) -> string;
auto			UnsubscribeFromGroup(string groupID, CUser *, CMysql *) -> string;
auto 			isBotIP(string ip) -> bool;
auto 			isAdverseWordsHere(string text, CMysql *) -> bool;
auto			GetCompanyPositionsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetSiteThemesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
struct tm		GetTMObject(string date);
auto			operator <(const struct tm &tm_1, const struct tm &tm_2) -> bool;
auto			operator <=(const struct tm &tm_1, const struct tm &tm_2) -> bool;
auto			operator >(const struct tm &tm_1, const struct tm &tm_2) -> bool;
auto			operator >=(const struct tm &tm_1, const struct tm &tm_2) -> bool;
auto			operator ==(const struct tm &tm_1, const struct tm &tm_2) -> bool;
auto			PrintDate(const struct tm &_tm) -> string;
auto			PrintDateTime(const struct tm &_tm) -> string;
auto			PrintTime(const struct tm &_tm, string format) -> string;
auto			GetZipInJSONFormat(string zip_id, CMysql *, CUser *) -> string;
auto			GetBankInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;




// --- timecard functions
auto			GetTimecardCustomersInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardProjectsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardTasksInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardTaskAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetTimecardsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended = false) -> string;
auto			GetSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string;
auto			GetSOWInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_tasks = true, bool include_bt = false) -> string;
auto			GetTimecardApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			SubmitTimecard(string timecard_id, CMysql *, CUser *) -> bool;
auto			SubmitBT(string bt_id, CMysql *, CUser *) -> bool;
auto			GetNumberOfTimecardsInPendingState(CMysql *, CUser *) -> string;
auto			GetNumberOfBTInPendingState(CMysql *, CUser *) -> string;
auto 			SummarizeTimereports(string timereport1, string timereport2) -> string;
vector<string> 	SplitTimeentry(const string& s, const char c = ',');
auto			isTimecardEntryEmpty(string	timereports) -> bool;
auto			isUserAssignedToSoW(string user_id, string sow_id, CMysql *) -> bool;
auto 			isSoWAllowedToCreateTask(string sow_id, CMysql *) -> bool;
auto			GetTaskIDFromAgency(string customer, string project, string task, string agency_id, CMysql *) -> string;
auto			GetTaskIDFromSOW(string customer, string project, string task, string sow_id, CMysql *) -> string;
auto			CreateTaskBelongsToAgency(string customer, string project, string task, string agency_id, CMysql *) -> string;
auto			GetTimecardID(string sow_id, string period_start, string period_end, CMysql *) -> string;
auto			GetTimecardStatus(string timecard_id, CMysql *) -> string;
auto			GetTimecardLineID(string timecard_id, string task_id, CMysql *) -> string;
auto			GetAgencyID(string sow_id, CMysql *) -> string;
auto			GetAgencyID(CUser *, CMysql *) -> string;
auto			GetObjectsSOW_Reusable_InJSONFormat(string object, string filter, CMysql *, CUser *) -> string;
auto			GetBTExpenseTemplateAssignmentToSoW(string bt_expense_template_id, string sow_id, CMysql *) -> string;
auto			GetTaskAssignmentID(string customer, string project, string task, string sow_id, CMysql *) -> string;
auto			CreateTaskAssignment(string task_id, string sow_id, string assignment_start, string assignment_end, CMysql *, CUser *) -> string;
auto 			CreateBTExpenseTemplateAssignmentToSoW(string new_bt_expense_template_id, string sow_id, CMysql *, CUser *) -> string;
auto			GetCostCentersInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCostCentersAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetApproversInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_sow) -> string;
auto			GetSpelledTimecardCustomerByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardProjectByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardTaskByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardTaskAssignmentByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateLineByID(string id, CMysql *) -> string;
auto			GetSpelledCostCenterByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseAssignmentByID(string id, CMysql *) -> string;
auto			GetSpelledUserNameByID(string id, CMysql *) -> string;
auto			GetSpelledEmployeeByID(string id, CMysql *) -> string;
auto			GetSpelledTimecardApproverNameByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseApproverNameByID(string id, CMysql *) -> string;
auto			GetSpelledSoWByID(string sow_id, CMysql *) -> string;
auto			GetSpelledZIPByID(string id, CMysql *) -> string;
auto			GetSpelledBankByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateByLineID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseTemplateByAssignmentID(string id, CMysql *) -> string;
auto			isValidToReportTime(string timecard_id, string task_id, string timereport, CMysql *, CUser *) -> string;
auto			isTaskIDValidToRemove(string task_id, CMysql *) -> string;
auto			isProjectIDValidToRemove(string task_id, CMysql *) -> bool;
auto			isCustomerIDValidToRemove(string task_id, CMysql *) -> bool;
auto			isExpenseTemplateIDValidToRemove(string bt_expense_template_id, CMysql *) -> string;
auto			isExpenseTemplateLineIDValidToRemove(string bt_expense_template_line_id, CMysql *) -> string;
auto			isEmployeeIDValidToRemove(string employee_id, CMysql *) -> string;
pair<string, string> GetCustomerIDProjectIDByTaskID(string task_id, CMysql *);
pair<int, int>	FirstAndLastReportingDaysFromTimereport(const vector<string> &timereport);


auto			GetBTApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTExpenseLineTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetBTsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended) -> string;
auto			GetBTExpensesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto			GetCurrencyRatesInJSONFormat(string sqlQuery, CMysql *, CUser *) -> string;
auto 			isUserAllowedAccessToBT(string bt_id, CMysql *, CUser *) -> string;
// string			isAgencyEmployeeAllowedToChangeSoW(string sow_id, CMysql *, CUser *);
// string			isAgencyEmployeeAllowedToChangeAgencyData(CMysql *, CUser *);
// string			isActionEntityBelongsToSoW(string action, string is, string sow_id, CMysql *, CUser *);
// string			isActionEntityBelongsToAgency(string action, string id, string agency_id, CMysql *, CUser *);
auto			NotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *, CUser *) -> bool;
auto			GetDBValueByAction(string action, string id, string sow_id, CMysql *, CUser *) -> string;
// string			CheckNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *);
auto			SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;
auto			GetAgencyEmployeesInJSONFormat(string sql_query, CMysql *, CUser *) -> string;
auto			GetAgencyObjectInJSONFormat(string agency_id, bool include_tasks, bool include_bt, CMysql *, CUser *) -> string;
auto			GetInfoToReturnByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *) -> string;

// --- function set for image upload/removal
auto 			GetSpecificData_GetNumberOfFolders(string itemType) -> int;
auto 			GetSpecificData_GetMaxFileSize(string itemType) -> int;
auto		 	GetSpecificData_GetMaxWidth(string itemType) -> unsigned int;
auto		 	GetSpecificData_GetMaxHeight(string itemType) -> unsigned int;
auto 			GetSpecificData_GetBaseDirectory(string itemType) -> string;
auto			GetSpecificData_GetFinalFileExtenstion(string itemType) -> string;
auto 			GetSpecificData_SelectQueryItemByID(string itemID, string itemType) -> string;
auto 			GetSpecificData_UpdateQueryItemByID(string itemID, string itemType, string folderID, string fileName) -> string;
auto 			GetSpecificData_GetDBCoverPhotoFolderString(string itemType) -> string;
auto 			GetSpecificData_GetDBCoverPhotoFilenameString(string itemType) -> string;
auto 			GetSpecificData_GetDataTypeByItemType(const string &itemType) -> string;
auto 			GetSpecificData_AllowedToChange(string itemID, string itemType, CMysql *, CUser *) -> bool;
auto			SaveImageFileFromHandler(string f_name, string f_type, CFiles *files) -> string;
auto 			isCBCurrencyRate(string date, string currency_name, string currency_nominal, string currency_value, CMysql *) -> string;

// --- UTF8 encoding/decoding
auto         	convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n) -> int;
auto			utf8_to_cp1251(const string &) -> string;
auto 			convert_cp1251_to_utf8(const char *in, char *out, int size) -> bool;

// --- base64 encoding/decoding
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
typedef unsigned char BYTE;

auto 			is_base64(BYTE c) -> bool;
auto 			base64_encode(BYTE const* buf, unsigned int bufLen)  -> string;
auto 			base64_decode(std::string const& encoded_string)  -> string;


struct ExifInfo 
{
	string  DateTime;
	string  GPSAltitude;
	string  GPSLatitude;
	string  GPSLongitude;
	string  GPSSpeed;
	string  Model;
	string  Authors;
	string  ApertureValue;
	string  BrightnessValue;
	string  ColorSpace;
	string  ComponentsConfiguration;
	string  Compression;
	string  DateTimeDigitized;
	string  DateTimeOriginal;
	string  ExifImageLength;
	string  ExifImageWidth;
	string  ExifOffset;
	string  ExifVersion;
	string  ExposureBiasValue;
	string  ExposureMode;
	string  ExposureProgram;
	string  ExposureTime;
	string  Flash;
	string  FlashPixVersion;
	string  FNumber;
	string  FocalLength;
	string  FocalLengthIn35mmFilm;
	string  GPSDateStamp;
	string  GPSDestBearing;
	string  GPSDestBearingRef;
	string  GPSImgDirection;
	string  GPSImgDirectionRef;
	string  GPSInfo;
	string  GPSTimeStamp;
	string  ISOSpeedRatings;
	string  JPEGInterchangeFormat;
	string  JPEGInterchangeFormatLength;
	string  Make;
	string  MeteringMode;
	string  Orientation;
	string  ResolutionUnit;
	string  SceneCaptureType;
	string  SceneType;
	string  SensingMethod;
	string  ShutterSpeedValue;
	string  Software;
	string  SubjectArea;
	string  SubSecTimeDigitized;
	string  SubSecTimeOriginal;
	string  WhiteBalance;
	string  XResolution;
	string  YCbCrPositioning;
	string  YResolution;
};

auto		ParseGPSLongitude(const string longitudeStr) -> string;
auto		ParseGPSLatitude(const string latitudeStr) -> string;
auto		ParseGPSAltitude(const string altitudeStr) -> string;
auto		ParseGPSSpeed(const string speedStr) -> string;

#endif
