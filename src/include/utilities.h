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

void			crash_handler(int sig);
string			GetSHA512(string src);
string      	rtrim(string& str);
string      	ltrim(string& str);
string      	trim(string& str);
string      	quoted(string src);
string  		toLower(string src);
string      	GetRandom(int len);
string      	DeleteHTML(string src, bool removeBR = true);
string      	RemoveQuotas(string src);
string      	RemoveSpecialSymbols(string src);
string      	RemoveSpecialHTMLSymbols(string src);
string      	ReplaceDoubleQuoteToQuote(string src);
string      	ReplaceCRtoHTML(string src);
string      	CleanUPText(const string messageBody, bool removeBR = true);
string      	RemoveAllNonAlphabetSymbols(string src);
string      	ConvertTextToHTML(const string messageBody);
string 			CheckHTTPParam_Text(string srcText);
auto 			CheckHTTPParam_Number(string srcText) -> string;
string 			CheckHTTPParam_Date(string srcText);
string 			CheckHTTPParam_Float(string srcText);
string 			CheckHTTPParam_Email(string srcText);
string 			CheckHTTPParam_Timeentry(string srcText);
string      	CheckIfFurtherThanNow(string occupationStart_cp1251);
string			GetDefaultActionFromUserType(string role, CMysql *);
double      	GetSecondsSinceY2k();
string      	GetLocalFormattedTimestamp();
double      	GetTimeDifferenceFromNow(const string timeAgo);
string      	GetMinutesDeclension(const int value);
string      	GetHoursDeclension(const int value);
string      	GetDaysDeclension(const int value);
string      	GetMonthsDeclension(const int value);
string      	GetYearsDeclension(const int value);
string      	GetHumanReadableTimeDifferenceFromNow (const string timeAgo);
string      	SymbolReplace(const string where, const string src, const string dst);
string      	SymbolReplace_KeepDigitsOnly(const string where);
int         	qw(const string src, vector<string> &dst);
vector<string>	split(const string& s, const char& c);
string			join(const vector<string>& vec, string separator = ",");
string			FilterCP1251Symbols(const string &src);
string      	UniqueUserIDInUserIDLine(string userIDLine); //-> decltype(static_cast<string>("123"))
string      	AutodetectSexByName(string name, CMysql *);
string			GetPasswordNounsList(CMysql *);
string			GetPasswordAdjectivesList(CMysql *);
string			GetPasswordCharacteristicsList(CMysql *);

string      	GetChatMessagesInJSONFormat(string dbQuery, CMysql *);
string      	GetUserListInJSONFormat(string dbQuery, CMysql *, CUser *);
string      	GetCompanyListInJSONFormat(string dbQuery, CMysql *, CUser *, bool quickSearch = true, bool includeEmployedUsersList = false);
string      	GetGroupListInJSONFormat(string dbQuery, CMysql *, CUser *);
string 			GetBookListInJSONFormat(string dbQuery, CMysql *, bool includeReaders = false);
string 			GetComplainListInJSONFormat(string dbQuery, CMysql *, bool includeReaders = false);
string 			GetCertificationListInJSONFormat(string dbQuery, CMysql *, bool includeDevoted = false);
string 			GetCourseListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false);
string 			GetLanguageListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false);
string 			GetSkillListInJSONFormat(string dbQuery, CMysql *);
string 			GetUniversityListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false);
string 			GetSchoolListInJSONFormat(string dbQuery, CMysql *, bool includeStudents = false);
string 			GetNewsFeedInJSONFormat(string whereStatement, int currPage, int newsOnSinglePage, CUser *, CMysql *);
string      	GetUnreadChatMessagesInJSONFormat(CUser *, CMysql *);
string			GetGiftListInJSONFormat(string dbQuery, CMysql *, CUser *);
string 			GetGiftToGiveListInJSONFormat(string dbQuery, CMysql *, CUser *);
string			GetEventListInJSONFormat(string dbQuery, CMysql *, CUser *);
string			GetEventHostsListInJSONFormat(string dbQuery, CMysql *, CUser *);
string			GetEventGuestsListInJSONFormat(string dbQuery, CMysql *, CUser *);
string			GetMySQLDateInJSONFormat(string dateString);
string      	GetMessageImageList(string imageSetID, CMysql *);
string      	GetMessageLikesUsersList(string messageID, CUser *, CMysql *);
string 			GetCertificationLikesUsersList(string usersCertificationID, CUser *, CMysql *);
string 			GetCourseLikesUsersList(string usersCourseID, CUser *, CMysql *);
string 			GetUniversityDegreeLikesUsersList(string messageID, CUser *, CMysql *);
string 			GetCompanyLikesUsersList(string usersCompanyID, CUser *, CMysql *);
string 			GetLanguageLikesUsersList(string usersLanguageID, CUser *, CMysql *);
string 			GetBookLikesUsersList(string usersBookID, CUser *, CMysql *);
string 			GetBookRatingList(string bookID, CMysql *);
string 			GetCourseRatingList(string courseID, CMysql *);
string 			GetBookRatingUsersList(string bookID, CUser *, CMysql *);
string      	GetMessageCommentsCount(string messageID, CMysql *);
string 			GetCompanyCommentsCount(string messageID, CMysql *);
string 			GetLanguageCommentsCount(string messageID, CMysql *);
string 			GetBookCommentsCount(string messageID, CMysql *);
string 			GetCertificateCommentsCount(string messageID, CMysql *);
string 			GetUniversityDegreeCommentsCount(string messageID, CMysql *);
string      	GetMessageSpam(string messageID, CMysql *);
string      	GetMessageSpamUser(string messageID, string userID, CMysql *);
string			GetLanguageIDByTitle(string title, CMysql *);
string			GetSkillIDByTitle(string title, CMysql *);
string			GetCompanyPositionIDByTitle(string title, CMysql *);
string 			GetCandidatesListAppliedToVacancyInJSONFormat(string dbQuery, CMysql *);
string 			GetOpenVacanciesInJSONFormat(string companyID, CMysql *, CUser * = NULL);
string			GetGeoLocalityIDByCityAndRegion(string regionName, string cityName, CMysql *);
bool        	AllowMessageInNewsFeed(CUser *me, const string messageOwnerID, const string messageAccessRights, vector<string> *messageFriendList);
bool        	isPersistenceRateLimited(string REMOTE_ADDR, CMysql *);
bool        	isFileExists(const std::string& name);
bool			isFilenameImage(string	filename);
bool			isFilenameVideo(string	filename);
void        	CopyFile(const string src, const string dst);
string      	GetCompanyDuplicates(CMysql *);
string      	GetPicturesWithEmptySet(CMysql *);
string      	GetPicturesWithUnknownMessage(CMysql *);
string      	GetPicturesWithUnknownUser(CMysql *);
string      	GetRecommendationAdverse(CMysql *);
string      	GetUserAvatarByUserID(string userID, CMysql *);
string      	GetUserNotificationSpecificDataByType(unsigned long typeID, unsigned long actionID, CMysql *, CUser *);
string      	GetUserNotificationInJSONFormat(string sqlRequest, CMysql *, CUser *);
void        	RemoveMessageImages(string sqlWhereStatement, CMysql *);
void    		RemoveBookCover(string sqlWhereStatement, CMysql *);
bool    		RemoveSpecifiedCover(string itemID, string itemType, CMysql *);
bool        	CheckUserEmailExisting(string userNameToCheck, CMysql *);
vector<string>	GetUserTagsFromText(string srcMessage);
bool			RedirStdoutToFile(string fname);
bool			RedirStderrToFile(string fname);
bool 			AmIMessageOwner(string messageID, CUser *, CMysql *);
pair<string, string> GetMessageOwner(string messageID, CUser *, CMysql *);
string			GetUserSubscriptionsInJSONFormat(string sqlQuery, CMysql *);
string			SubscribeToCompany(string companyID, CUser *, CMysql *);
string			UnsubscribeFromCompany(string companyID, CUser *, CMysql *);
string			SubscribeToGroup(string groupID, CUser *, CMysql *);
string			UnsubscribeFromGroup(string groupID, CUser *, CMysql *);
bool 			isBotIP(string ip);
bool 			isAdverseWordsHere(string text, CMysql *);
string			GetSiteThemesInJSONFormat(string sqlQuery, CMysql *, CUser *);
struct tm		GetTMObject(string date);
bool			operator <(const struct tm &tm_1, const struct tm &tm_2);
bool			operator <=(const struct tm &tm_1, const struct tm &tm_2);
bool			operator >(const struct tm &tm_1, const struct tm &tm_2);
bool			operator >=(const struct tm &tm_1, const struct tm &tm_2);
bool			operator ==(const struct tm &tm_1, const struct tm &tm_2);
string			PrintDate(const struct tm &_tm);
string			PrintDateTime(const struct tm &_tm);
string			PrintTime(const struct tm &_tm, string format);
string			GetZipInJSONFormat(string zip_id, CMysql *, CUser *);
string			GetBankInJSONFormat(string sqlQuery, CMysql *, CUser *);




// --- timecard functions
string			GetTimecardCustomersInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetTimecardProjectsInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetTimecardTasksInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetTimecardTaskAssignmentInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetBTExpenseAssignmentInJSONFormat(string sqlQuery, CMysql *db, CUser *user);
string			GetTimecardsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended = false);
string			GetSOWInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_tasks = true, bool include_bt = false);
string			GetTimecardApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *);
bool			SubmitTimecard(string timecard_id, CMysql *, CUser *);
bool			SubmitBT(string bt_id, CMysql *, CUser *);
string			GetNumberOfTimecardsInPendingState(CMysql *, CUser *);
string			GetNumberOfBTInPendingState(CMysql *, CUser *);
string 			SummarizeTimereports(string timereport1, string timereport2);
vector<string> 	SplitTimeentry(const string& s, const char c = ',');
bool			isTimecardEntryEmpty(string	timereports);
bool			isUserAssignedToSoW(string user_id, string sow_id, CMysql *);
bool 			isSoWAllowedToCreateTask(string sow_id, CMysql *);
string			GetTaskIDFromAgency(string customer, string project, string task, string agency_id, CMysql *);
string			GetTaskIDFromSOW(string customer, string project, string task, string sow_id, CMysql *);
string			CreateTaskBelongsToAgency(string customer, string project, string task, string agency_id, CMysql *);
string			GetTimecardID(string sow_id, string period_start, string period_end, CMysql *);
string			GetTimecardStatus(string timecard_id, CMysql *);
string			GetTimecardLineID(string timecard_id, string task_id, CMysql *);
string			GetAgencyID(string sow_id, CMysql *);
string			GetAgencyID(CUser *, CMysql *);
string			GetObjectsSOW_Reusable_InJSONFormat(string object, string filter, CMysql *, CUser *);
auto			GetBTExpenseTemplateAssignmentToSoW(string bt_expense_template_id, string sow_id, CMysql *db) -> string;
string			GetTaskAssignmentID(string customer, string project, string task, string sow_id, CMysql *);
string			CreateTaskAssignment(string task_id, string sow_id, string assignment_start, string assignment_end, CMysql *, CUser *);
auto 			CreateBTExpenseTemplateAssignmentToSoW(string new_bt_expense_template_id, string sow_id, CMysql *db, CUser *user) -> string;
string			GetApproversInJSONFormat(string sqlQuery, CMysql *, CUser *, bool include_sow);
string			GetSpelledTimecardCustomerByID(string id, CMysql *);
string			GetSpelledTimecardProjectByID(string id, CMysql *);
string			GetSpelledTimecardTaskByID(string id, CMysql *);
string			GetSpelledTimecardTaskAssignmentByID(string id, CMysql *);
auto			GetSpelledBTExpenseTemplateLineByID(string id, CMysql *) -> string;
auto			GetSpelledBTExpenseAssignmentByID(string id, CMysql *) -> string;
string			GetSpelledUserNameByID(string id, CMysql *);
string			GetSpelledEmployeeByID(string id, CMysql *);
string			GetSpelledTimecardApproverNameByID(string id, CMysql *);
string			GetSpelledBTExpenseApproverNameByID(string id, CMysql *);
string			GetSpelledSoWByID(string sow_id, CMysql *);
string			GetSpelledZIPByID(string id, CMysql *);
string			GetSpelledBankByID(string id, CMysql *);
string			GetSpelledBTExpenseTemplateByID(string id, CMysql *);
string			GetSpelledBTExpenseTemplateByLineID(string id, CMysql *);
auto			GetSpelledBTExpenseTemplateByAssignmentID(string id, CMysql *db) -> string;
string			isValidToReportTime(string timecard_id, string task_id, string timereport, CMysql *, CUser *);
bool			isTaskIDValidToRemove(string task_id, CMysql *);
bool			isProjectIDValidToRemove(string task_id, CMysql *);
bool			isCustomerIDValidToRemove(string task_id, CMysql *);
auto			isExpenseTemplateIDValidToRemove(string bt_expense_template_id, CMysql *) -> string;
auto			isExpenseTemplateLineIDValidToRemove(string bt_expense_template_line_id, CMysql *) -> string;
string			isEmployeeIDValidToRemove(string employee_id, CMysql *);
pair<string, string> GetCustomerIDProjectIDByTaskID(string task_id, CMysql *);
pair<int, int>	FirstAndLastReportingDaysFromTimereport(const vector<string> &timereport);


string			GetBTApprovalsInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetBTExpenseTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetBTExpenseLineTemplatesInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetBTsInJSONFormat(string sqlQuery, CMysql *, CUser *, bool isExtended);
string			GetBTExpensesInJSONFormat(string sqlQuery, CMysql *, CUser *);
string			GetCurrencyRatesInJSONFormat(string sqlQuery, CMysql *, CUser *);
string 			isUserAllowedAccessToBT(string bt_id, CMysql *, CUser *);
// string			isAgencyEmployeeAllowedToChangeSoW(string sow_id, CMysql *, CUser *);
// string			isAgencyEmployeeAllowedToChangeAgencyData(CMysql *, CUser *);
// string			isActionEntityBelongsToSoW(string action, string is, string sow_id, CMysql *, CUser *);
// string			isActionEntityBelongsToAgency(string action, string id, string agency_id, CMysql *, CUser *);
bool			NotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *, CUser *);
string			GetDBValueByAction(string action, string id, string sow_id, CMysql *, CUser *);
// string			CheckNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *);
string			SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *);
string			ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *);
string			GetAgencyEmployeesInJSONFormat(string sql_query, CMysql *, CUser *);
string			GetAgencyObjectInJSONFormat(string agency_id, bool include_tasks, bool include_bt, CMysql *, CUser *);
string			GetInfoToReturnByAction(string action, string id, string sow_id, string new_value, CMysql *, CUser *);

// --- function set for image upload/removal
int 			GetSpecificData_GetNumberOfFolders(string itemType);
int 			GetSpecificData_GetMaxFileSize(string itemType);
unsigned int 	GetSpecificData_GetMaxWidth(string itemType);
unsigned int 	GetSpecificData_GetMaxHeight(string itemType);
string 			GetSpecificData_GetBaseDirectory(string itemType);
string 			GetSpecificData_SelectQueryItemByID(string itemID, string itemType);
string 			GetSpecificData_UpdateQueryItemByID(string itemID, string itemType, string folderID, string fileName);
string 			GetSpecificData_GetDBCoverPhotoFolderString(string itemType);
string 			GetSpecificData_GetDBCoverPhotoFilenameString(string itemType);
bool 			GetSpecificData_AllowedToChange(string itemID, string itemType, CMysql *, CUser *);
string			SaveImageFileFromHandler(string f_name, string f_type, CFiles *files);
string 			isCBCurrencyRate(string date, string currency_name, string currency_nominal, string currency_value, CMysql *);

// --- UTF8 encoding/decoding
int         	convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n);
bool 			convert_cp1251_to_utf8(const char *in, char *out, int size);

// --- base64 encoding/decoding
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
typedef unsigned char BYTE;

bool 			is_base64(BYTE c);
string 			base64_encode(BYTE const* buf, unsigned int bufLen) ;
string 			base64_decode(std::string const& encoded_string) ;


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

string		ParseGPSLongitude(const string longitudeStr);
string		ParseGPSLatitude(const string latitudeStr);
string		ParseGPSAltitude(const string altitudeStr);
string		ParseGPSSpeed(const string speedStr);

#endif
