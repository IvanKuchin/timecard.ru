#include "utilities_timecard.h"

// --- begin of news feed and notifications

string GetNewsFeedInJSONFormat(string whereStatement, int currPage, int newsOnSinglePage, CUser *user, CMysql *db)
{
		struct  ItemClass
		{
			string  action_category_title;
			string  action_category_title_female;
			string  action_category_title_male;
			string  action_types_title;
			string  action_types_title_female;
			string  action_types_title_male;
			string  feed_actionId;
			string  feed_actionTypeId;
			string  feed_eventTimestamp;
			string  feed_id;
			string  src_id;
			string  src_type;
			string  src_name;
			string  src_nameLast;
			string  src_sex;
			string  dst_id;
			string  dst_type;
			string  dst_name;
			string  dst_nameLast;
			string  dst_sex;
		};
		vector<ItemClass>   itemsList;

		ostringstream	  ostResult;
		int			  affected;
		vector<string>	vectorFriendList;

		ostResult.str("");
		ostResult << "SELECT `users_friends`.`friendID` \
				from `users_friends` \
				left join `users` on `users`.`id`=`users_friends`.`friendID` \
				where `users_friends`.`userID`='" << user->GetID() << "' and `users_friends`.`state`='confirmed' and `users`.`isactivated`='Y' and `users`.`isblocked`='N';";

		affected = db->Query(ostResult.str());
		for(int i = 0; i < affected; i++)
		{
			vectorFriendList.push_back(db->Get(i, "friendID"));
		}

		ostResult.str("");
		ostResult << "SELECT "
			<< "`feed`.`id` as `feed_id`, `feed`.`eventTimestamp` as `feed_eventTimestamp`, `feed`.`actionId` as `feed_actionId` , `feed`.`actionTypeId` as `feed_actionTypeId`, `feed`.`srcType` as `feed_srcType`, `feed`.`userId` as `feed_srcID`, `feed`.`dstType` as `feed_dstType`, `feed`.`dstID` as `feed_dstID`, "
			<< "`action_types`.`title` as `action_types_title`, "
			<< "`action_types`.`title_male` as `action_types_title_male`, "
			<< "`action_types`.`title_female` as `action_types_title_female`, "
			<< "`action_category`.`title` as `action_category_title`, "
			<< "`action_category`.`title_male` as `action_category_title_male`, "
			<< "`action_category`.`title_female` as `action_category_title_female` "
			<< "FROM `feed` "
			<< "INNER JOIN  `action_types`  ON `feed`.`actionTypeId`=`action_types`.`id` "
			<< "INNER JOIN  `action_category`   ON `action_types`.`categoryID`=`action_category`.`id` "
			<< "WHERE (" << whereStatement << ") and `action_types`.`isShowFeed`='1' "
			<< "ORDER BY  `feed`.`eventTimestamp` DESC LIMIT " << currPage * newsOnSinglePage << " , " << newsOnSinglePage;

		affected = db->Query(ostResult.str());
		for(int i = 0; i < affected; i++)
		{
			ItemClass   item;

			item.action_category_title = db->Get(i, "action_category_title");
			item.action_category_title_female = db->Get(i, "action_category_title_female");
			item.action_category_title_male = db->Get(i, "action_category_title_male");
			item.action_types_title = db->Get(i, "action_types_title");
			item.action_types_title_female = db->Get(i, "action_types_title_female");
			item.action_types_title_male = db->Get(i, "action_types_title_male");
			item.feed_actionId = db->Get(i, "feed_actionId");
			item.feed_actionTypeId = db->Get(i, "feed_actionTypeId");
			item.feed_eventTimestamp = db->Get(i, "feed_eventTimestamp");
			item.feed_id = db->Get(i, "feed_id");
			item.src_id = db->Get(i, "feed_srcID");
			item.src_type = db->Get(i, "feed_srcType");
			item.dst_id = db->Get(i, "feed_dstID");
			item.dst_type = db->Get(i, "feed_dstType");

			itemsList.push_back(item);
		}

		ostResult.str("");
		for(int i = 0; i < affected; i++)
		{
			ostringstream   ost1;
			string		  srcAvatarPath;
			string		  dstAvatarPath;
			string		  feedID = itemsList[i].feed_id;
			string		  feedActionTypeId = itemsList[i].feed_actionTypeId;
			string		  feedActionId = itemsList[i].feed_actionId;
			string		  feedMessageOwner = itemsList[i].src_id;
			string		  feedMessageTimestamp = itemsList[i].feed_eventTimestamp;
			string		  messageSrcObject = "";
			string		  messageDstObject = "";

			// --- avatar for srcObj
			if(itemsList[i].src_type == "user")
			{
				if(db->Query("SELECT `id`, `name`, `nameLast`, `sex` FROM `users` WHERE `id`=\"" + itemsList[i].src_id + "\";"))
				{
					messageSrcObject = "\"type\":\"" + itemsList[i].src_type + "\",\"id\":\"" + itemsList[i].src_id + "\",\"name\":\"" + db->Get(0, "name") + "\",\"nameLast\":\"" + db->Get(0, "nameLast") + "\",\"sex\":\"" + db->Get(0, "sex") + "\",\"link\":\"\"";

					if(db->Query("SELECT * FROM `users_avatars` WHERE `userid`='" + itemsList[i].src_id + "' and `isActive`='1';"))
						srcAvatarPath = "/images/avatars/avatars" + string(db->Get(0, "folder")) + "/" + string(db->Get(0, "filename"));
					else
						srcAvatarPath = "empty";

					messageSrcObject = messageSrcObject + ",\"avatar\":\"" + srcAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: user.id [" + itemsList[i].src_id + "] not found");
				}
			}
			else if(itemsList[i].src_type == "company")
			{
				if(db->Query("SELECT `id`, `type`, `name`, `link`, `logo_folder`, `logo_filename` FROM `company` WHERE `id`=\"" + itemsList[i].src_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageSrcObject = "\"type\":\"" + itemsList[i].src_type + "\",\"id\":\"" + itemsList[i].src_id + "\",\"companyType\":\"" + string(db->Get(0, "type")) + "\",\"name\":\"" + string(db->Get(0, "name")) + "\",\"nameLast\":\"\",\"sex\":\"F\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						srcAvatarPath = "/images/companies/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						srcAvatarPath = "empty";

					messageSrcObject = messageSrcObject + ",\"avatar\":\"" + srcAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].src_id + "] not found");
				}
			}
			else if(itemsList[i].src_type == "group")
			{
				if(db->Query("SELECT * FROM `groups` WHERE `id`=\"" + itemsList[i].src_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageSrcObject = "\"type\":\"" + itemsList[i].src_type + "\",\"id\":\"" + itemsList[i].src_id + "\",\"name\":\"" + db->Get(0, "title") + "\",\"nameLast\":\"\",\"sex\":\"F\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						srcAvatarPath = "/images/groups/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						srcAvatarPath = "empty";

					messageSrcObject = messageSrcObject + ",\"avatar\":\"" + srcAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].src_id + "] not found");
				}
			}
			else
			{
				CLog	log;
				log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: unknown srcType [" + itemsList[i].src_type + "]");
			}

			// --- avatar for dstObj
			if(itemsList[i].dst_type == "group")
			{
				if(db->Query("SELECT * FROM `groups` WHERE `id`=\"" + itemsList[i].dst_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageDstObject = "\"type\":\"" + itemsList[i].dst_type + "\",\"id\":\"" + itemsList[i].dst_id + "\",\"name\":\"" + db->Get(0, "title") + "\",\"nameLast\":\"\",\"sex\":\"F\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						dstAvatarPath = "/images/groups/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						dstAvatarPath = "empty";

					messageDstObject = messageDstObject + ",\"avatar\":\"" + dstAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].dst_id + "] not found");
				}
			}
			// --- avatar for dstObj
			if(itemsList[i].dst_type == "event")
			{
				if(db->Query("SELECT * FROM `events` WHERE `id`=\"" + itemsList[i].dst_id + "\";"))
				{
					string  logo_filename = db->Get(0, "logo_filename");

					messageDstObject = "\"type\":\"" + itemsList[i].dst_type + "\",\"id\":\"" + itemsList[i].dst_id + "\",\"name\":\"" + db->Get(0, "title") + "\",\"nameLast\":\"\",\"sex\":\"\",\"link\":\"" + db->Get(0, "link") + "\"";

					if(logo_filename.length())
						dstAvatarPath = "/images/events/" + string(db->Get(0, "logo_folder")) + "/" + string(db->Get(0, "logo_filename"));
					else
						dstAvatarPath = "empty";

					messageDstObject = messageDstObject + ",\"avatar\":\"" + dstAvatarPath + "\"";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: company.id [" + itemsList[i].dst_id + "] not found");
				}
			}

			if(feedActionTypeId == "11")
			{
				// --- 11 - message

				if(db->Query("SELECT * FROM `feed_message` WHERE `id`='" + feedActionId + "';"))
				{
					string  messageId = db->Get(0, "id");
					string  messageTitle = db->Get(0, "title");
					string  messageLink = db->Get(0, "link");
					string  messageMessage = db->Get(0, "message");
					string  messageImageSetID = db->Get(0, "imageSetID");
					string  messageAccessRights = db->Get(0, "access");
					string  messageImageList =			GetMessageImageList(messageImageSetID, db);
					string  messageParamLikesUserList =  GetMessageLikesUsersList(messageId, user, db);
					string  messageParamCommentsCount =  GetMessageCommentsCount(messageId, db);
					string  messageParamSpam =			GetMessageSpam(messageId, db);
					string  messageParamSpamMe =			GetMessageSpamUser(messageId, user->GetID(), db);

					if(AllowMessageInNewsFeed(user, feedMessageOwner, messageAccessRights, &vectorFriendList))
					{
						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"dstObj\":{"			 << messageDstObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female		<< "\",";
						ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId				   << "\",";

						ostResult << "\"messageId\":\""		<< messageId							 << "\",";
						ostResult << "\"messageTitle\":\""	  << messageTitle						  << "\",";
						ostResult << "\"messageLink\":\""		<< messageLink						   << "\",";
						ostResult << "\"messageMessage\":\""	  << messageMessage						<< "\",";
						ostResult << "\"messageImageSetID\":\""   << messageImageSetID					  << "\",";
						ostResult << "\"messageImageList\":["	<< messageImageList						 << "],";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList			  << "],";
						ostResult << "\"messageCommentsCount\":\""<< messageParamCommentsCount			  << "\",";
						ostResult << "\"messageSpam\":\""		<< messageParamSpam						 << "\",";
						ostResult << "\"messageSpamMe\":\""	<< messageParamSpamMe					   << "\",";
						ostResult << "\"messageAccessRights\":\"" << messageAccessRights					  << "\",";

						ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp										<< "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";

						ostResult << "}";

						// if(i < (affected - 1)) ostResult << ",";
					} // --- Message Access Rights onot allow to post it into feed
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: can't get message [" + feedActionId + "] FROM feed_message");
				} // --- Message in news feed not found
			}
			else if((feedActionTypeId == "14") || (feedActionTypeId == "15") || (feedActionTypeId == "16"))
			{
				// --- 14 friendship established
				// --- 15 friendship broken
				// --- 16 friendship request sent

				string  friendID = feedActionId;

				ost1.str("");
				ost1 << "SELECT `users`.`name` as `users_name`, `users`.`nameLast` as `users_nameLast` FROM `users` WHERE `id`=\"" << friendID << "\" and `isblocked`='N';";
				if(db->Query(ost1.str()))
				{
					string  friendAvatar = "empty";
					string  friendName;
					string  friendNameLast;
					string  friendCompanyName;
					string  friendCompanyID;
					string  friendUsersCompanyPositionTitle;

					friendName = db->Get(0, "users_name");
					friendNameLast = db->Get(0, "users_nameLast");

					ost1.str("");
					ost1 << "SELECT * FROM `users_avatars` WHERE `userid`='" << friendID << "' and `isActive`='1';";
					if(db->Query(ost1.str()))
					{
						ost1.str("");
						ost1 << "/images/avatars/avatars" << db->Get(0, "folder") << "/" << db->Get(0, "filename");
						friendAvatar = ost1.str();
					}

					ost1.str("");
					ost1 << "SELECT `company_position`.`title` as `users_company_position_title`,  \
							`company`.`id` as `company_id`, `company`.`name` as `company_name` \
							FROM `users_company` \
							LEFT JOIN  `company_position` ON `company_position`.`id`=`users_company`.`position_title_id` \
							LEFT JOIN  `company`				ON `company`.`id`=`users_company`.`company_id` \
							WHERE `users_company`.`user_id`=\"" << friendID << "\" and `users_company`.`current_company`='1' \
							ORDER BY  `users_company`.`occupation_start` DESC ";
					if(db->Query(ost1.str()))
					{
						friendCompanyName = db->Get(0, "company_name");
						friendCompanyID = db->Get(0, "company_id");
						friendUsersCompanyPositionTitle = db->Get(0, "users_company_position_title");
					}
					else
					{
						MESSAGE_DEBUG("", "", "can't get information [" + itemsList[i].feed_actionId + "] about his/her employment");
					} // --- Message in news feed not found

					{
						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";
						ostResult << "\"avatar\":\""			  << srcAvatarPath									<< "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId				   << "\",";

						ostResult << "\"friendAvatar\":\""	  << friendAvatar								  << "\",";
						ostResult << "\"friendID\":\""		  << friendID									  << "\",";
						ostResult << "\"friendName\":\""		  << friendName									<< "\",";
						ostResult << "\"friendNameLast\":\""	  << friendNameLast								<< "\",";
						ostResult << "\"friendCompanyID\":\""	<< friendCompanyID							   << "\",";
						ostResult << "\"friendCompanyName\":\""   << friendCompanyName							  << "\",";
						ostResult << "\"friendUsersCompanyPositionTitle\":\"" << friendUsersCompanyPositionTitle	  << "\",";

						ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp										<< "\",";
						// ostResult << "\"eventTimestampDelta\":\""  << GetHumanReadableTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)   << "\"";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";
						ostResult << "}";

						// if(i < (affected - 1)) ostResult << ",";
					} // --- Message Access Rights onot allow to post it into feed
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: user [", friendID, "] not found or blocked");
				}
			}
			else if((feedActionTypeId == "41"))
			{
				// --- 41 skill added

				string  users_skillID = feedActionId;

				ost1.str("");
				ost1 << "SELECT * FROM `users_skill` WHERE `id`=\"" << users_skillID << "\";";
				if(db->Query(ost1.str()))
				{
					string  skillID = db->Get(0, "skill_id");

					ost1.str("");
					ost1 << "SELECT * FROM `skill` WHERE `id`=\"" << skillID << "\";";
					if(db->Query(ost1.str()))
					{
						string	skillTitle = db->Get(0, "title");

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";
						ostResult << "\"avatar\":\""			  << srcAvatarPath									<< "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId				   << "\",";

						ostResult << "\"skillID\":\""			<< skillID									   << "\",";
						ostResult << "\"skillTitle\":\""		  << skillTitle									<< "\",";

						ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp							<< "\",";
						// ostResult << "\"eventTimestampDelta\":\""  << GetHumanReadableTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)   << "\"";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";
						ostResult << "}";

					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: skill_id [", skillID, "] not found");
					}

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: users_skill_id [", users_skillID, "] not found");
				}
			}
			else if((feedActionTypeId == "54") || (feedActionTypeId == "53"))
			{
				// --- book read
				string  usersBooksID = itemsList[i].feed_actionId;
				string  bookID = "";
				string  readerUserID = "";
				string  bookReadTimestamp = "";

				if(db->Query("SELECT * FROM `users_books` WHERE `id`=\"" + feedActionId + "\";"))
				{
					bookID = db->Get(0, "bookID");
					readerUserID = db->Get(0, "userID");
					bookReadTimestamp = db->Get(0, "bookReadTimestamp");


					if(bookID.length() && db->Query("SELECT * FROM `book` WHERE `id`=\"" + bookID + "\";"))
					{
						string  bookTitle = db->Get(0, "title");
						string  bookAuthorID = db->Get(0, "authorID");
						string  bookISBN10 = db->Get(0, "isbn10");
						string  bookISBN13 = db->Get(0, "isbn13");
						string  bookCoverPhotoFolder = db->Get(0, "coverPhotoFolder");
						string  bookCoverPhotoFilename = db->Get(0, "coverPhotoFilename");

						if(bookAuthorID.length() && db->Query("SELECT * FROM `book_author` WHERE `id`=\"" + bookAuthorID + "\";"))
						{
							string  bookAuthorName = db->Get(0, "name");
							string  bookReadersRatingList =	 GetBookRatingList(bookID, db);
							string  messageParamLikesUserList = GetBookLikesUsersList(usersBooksID, user, db);
							string  messageParamCommentsCount = GetBookCommentsCount(bookID, db);
							string  bookMyRating = "0";

							if(db->Query("SELECT * FROM `users_books` WHERE `userID`=\"" + user->GetID() + "\" AND `bookID`=\"" + bookID + "\";"))
								bookMyRating = db->Get(0, "rating");

							if(ostResult.str().length() > 10) ostResult << ",";

							ostResult << "{";

							ostResult << "\"bookID\":\""			<< bookID << "\",";
							ostResult << "\"bookTitle\":\""	   << bookTitle << "\",";
							ostResult << "\"bookAuthorID\":\""	  << bookAuthorID << "\",";
							ostResult << "\"bookAuthorName\":\""	<< bookAuthorName << "\",";
							ostResult << "\"bookISBN10\":\""		<< bookISBN10 << "\",";
							ostResult << "\"bookISBN13\":\""		<< bookISBN13 << "\",";
							ostResult << "\"bookMyRating\":\""		<< bookMyRating << "\",";
							ostResult << "\"bookCoverPhotoFolder\":\""<< bookCoverPhotoFolder << "\",";
							ostResult << "\"bookCoverPhotoFilename\":\""<< bookCoverPhotoFilename << "\",";
							ostResult << "\"bookReadTimestamp\":\""   << bookReadTimestamp << "\",";
							ostResult << "\"bookReadersRatingList\":["<< bookReadersRatingList << "],";
							ostResult << "\"bookCommentsCount\":\""   << messageParamCommentsCount << "\",";

							ostResult << "\"usersBooksID\":\""	  << usersBooksID << "\",";
							ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

							ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
							ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
							ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
							ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
							ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
							ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
							ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
							ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
							ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
							ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
							ostResult << "\"actionID\":\""		  << itemsList[i].feed_actionId << "\",";
							ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
							ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
							ostResult << "}";
						}
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: books_author_id [" + bookAuthorID + "] not found");
						}

					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: books_id [" + bookID + "] not found");
					}

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_books_id [" + feedActionId + "] not found");
				}
			}
			else if((feedActionTypeId == "64") || (feedActionTypeId == "65"))
			{
				// --- group created / subscribed
				string  groupID = feedActionId;

				if(groupID.length())
				{
					if(db->Query("SELECT * FROM `groups` WHERE `id`=\"" + groupID + "\""))
					{
						string		isBlocked = db->Get(0, "isBlocked");

						if(isBlocked == "N")
						{
							if(ostResult.str().length() > 10) ostResult << ",";

							ostResult << "{";
							ostResult << "\"groups\":["			<< GetGroupListInJSONFormat("SELECT * FROM `groups` WHERE `id`=\"" + groupID + "\" AND `isBlocked`=\"N\";", db, user) << "],";
							ostResult << "\"avatar\":\""		<< srcAvatarPath << "\",";
							ostResult << "\"srcObj\":{"			<< messageSrcObject << "},";
							ostResult << "\"messageOwner\":{"	<< messageSrcObject << "},";
							ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
							ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
							ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
							ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
							ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
							ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
							ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
							ostResult << "\"actionID\":\""		  << itemsList[i].feed_actionId << "\",";
							ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
							ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
							ostResult << "}";
						}
						else
						{
							CLog	log;
							MESSAGE_DEBUG("", "", "`group`.`id` [" + groupID + "] blocked");
						}
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: `group`.`id` [" + groupID + "] not found");
					}
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: `group`.`id` [" + groupID + "] is empty");
				}
			}
			else if(feedActionTypeId == "63")
			{
				// --- group created / subscribed
				string  companyID = feedActionId;

				if(companyID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + companyID + "\" AND `isBlocked`=\"N\";"))
				{
					if(ostResult.str().length() > 10) ostResult << ",";

					ostResult << "{";
					ostResult << "\"companies\":["		<< GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + companyID + "\" AND `isBlocked`=\"N\";", db, user) << "],";
					ostResult << "\"avatar\":\""		<< srcAvatarPath << "\",";
					ostResult << "\"srcObj\":{"			<< messageSrcObject << "},";
					ostResult << "\"messageOwner\":{"	<< messageSrcObject << "},";
					ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
					ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
					ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
					ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
					ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
					ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
					ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
					ostResult << "\"actionID\":\""		  << itemsList[i].feed_actionId << "\",";
					ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
					ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
					ostResult << "}";
				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: books_id [" + companyID + "] not found");
				}
			}
			else if(feedActionTypeId == "22")
			{
				// --- cretificate received

				if(db->Query("SELECT * FROM `users_certifications` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  certificationNumber = "";
					string  certificationTrackID = "";
					string  certifiedUserID = "";
					string  usersCertificationID = "";

					usersCertificationID = db->Get(0, "id");
					certificationNumber = db->Get(0, "certification_number");
					certificationTrackID = db->Get(0, "track_id");
					certifiedUserID = db->Get(0, "user_id");

					if(certificationTrackID.length() && db->Query("SELECT * FROM `certification_tracks` WHERE `id`=\"" + certificationTrackID + "\";"))
					{
						string  certificationVendorID = db->Get(0, "vendor_id");
						string  certificationVendorName = "";
						string  certificationTrackTitle = db->Get(0, "title");
						string  certificationVendorLogoFolder = db->Get(0, "logo_folder");
						string  certificationVendorLogoFilename = db->Get(0, "logo_filename");
						string  messageParamLikesUserList = GetCertificationLikesUsersList(usersCertificationID, user, db);
						string  messageParamCommentsCount = GetCertificateCommentsCount(certificationTrackID, db);

						if(certificationVendorID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + certificationVendorID + "\";"))
							certificationVendorName = db->Get(0, "name");
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: certification vendor [" + certificationVendorID + "] not found");
						}

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"certificationID\":\"" << usersCertificationID << "\",";
						ostResult << "\"certificationTrackID\":\"" << certificationTrackID << "\",";
						ostResult << "\"certificationNumber\":\"" << certificationNumber << "\",";
						ostResult << "\"certificationVendorID\":\"" << certificationVendorID << "\",";
						ostResult << "\"certificationVendorName\":\"" << certificationVendorName << "\",";
						ostResult << "\"certificationTrackTitle\":\"" << certificationTrackTitle << "\",";
						ostResult << "\"certificationVendorLogoFolder\":\""<< certificationVendorLogoFolder << "\",";
						ostResult << "\"certificationVendorLogoFilename\":\""<< certificationVendorLogoFilename << "\",";

						ostResult << "\"certificationCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: certification track [" + certificationTrackID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_certification_id [" + feedActionId + "] not found");
				}

			}
			else if(feedActionTypeId == "23")
			{
				// --- course received

				if(db->Query("SELECT * FROM `users_courses` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  courseTrackID = "";
					string  courseUserID = "";
					string  courseMyRating = "0";
					string  usersCourseID = "";
					string  eventTimestamp = "";

					usersCourseID = db->Get(0, "id");
					courseTrackID = db->Get(0, "track_id");
					courseUserID = db->Get(0, "user_id");
					eventTimestamp = db->Get(0, "eventTimestamp");

					if(courseTrackID.length() && db->Query("SELECT * FROM `certification_tracks` WHERE `id`=\"" + courseTrackID + "\";"))
					{
						string  courseVendorID = db->Get(0, "vendor_id");
						string  courseVendorName = "";
						string  courseTrackTitle = db->Get(0, "title");
						string  courseVendorLogoFolder = db->Get(0, "logo_folder");
						string  courseVendorLogoFilename = db->Get(0, "logo_filename");
						string  courseRatingList =			GetCourseRatingList(courseTrackID, db);
						string  messageParamLikesUserList = GetCourseLikesUsersList(usersCourseID, user, db);
						string  messageParamCommentsCount = GetCertificateCommentsCount(courseTrackID, db);

						if(courseVendorID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + courseVendorID + "\";"))
							courseVendorName = db->Get(0, "name");
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: course vendor [" + courseVendorID + "] not found");
						}

						if(db->Query("SELECT * FROM `users_courses` WHERE `user_id`=\"" + user->GetID() + "\" AND `track_id`=\"" + courseTrackID + "\";"))
							courseMyRating = db->Get(0, "rating");

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"courseID\":\""		  << courseTrackID << "\",";
						ostResult << "\"usersCourseID\":\""   << usersCourseID << "\",";
						ostResult << "\"courseTrackID\":\""   << courseTrackID << "\",";
						ostResult << "\"courseVendorID\":\""	<< courseVendorID << "\",";
						ostResult << "\"courseVendorName\":\""  << courseVendorName << "\",";
						ostResult << "\"courseTrackTitle\":\""  << courseTrackTitle << "\",";
						ostResult << "\"courseVendorLogoFolder\":\""<< courseVendorLogoFolder << "\",";
						ostResult << "\"courseVendorLogoFilename\":\""<< courseVendorLogoFilename << "\",";
						ostResult << "\"courseMyRating\":\""	<< courseMyRating << "\",";
						ostResult << "\"courseRatingList\":["  << courseRatingList << "],";
						ostResult << "\"courseEventTimestamp\":\""<< eventTimestamp << "\",";

						ostResult << "\"courseCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: course track [" + courseTrackID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_course_id [" + feedActionId + "] not found");
				}

			}
			else if(feedActionTypeId == "39")
			{
				// --- university degree received

				if(db->Query("SELECT * FROM `users_university` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  scienceDegreeTitle = "";
					string  scienceDegreeUniversityID = "";
					string  scienceDegreeStart = "";
					string  scienceDegreeFinish = "";
					string  degreedUserID = "";
					string  usersUniversityDegreeID = "";

					usersUniversityDegreeID = db->Get(0, "id");
					degreedUserID = db->Get(0, "user_id");
					scienceDegreeUniversityID = db->Get(0, "university_id");
					scienceDegreeTitle = db->Get(0, "degree");
					scienceDegreeStart = db->Get(0, "occupation_start");
					scienceDegreeFinish = db->Get(0, "occupation_finish");

					if(scienceDegreeUniversityID.length() && db->Query("SELECT * FROM `university` WHERE `id`=\"" + scienceDegreeUniversityID + "\";"))
					{
						string  scienceDegreeUniversityID = db->Get(0, "id");
						string  scienceDegreeUniversityTitle = db->Get(0, "title");
						string  scienceDegreeUniversityRegionID = db->Get(0, "geo_region_id");
						string  scienceDegreeUniversityRegionTitle = "";
						string  scienceDegreeUniversityCountryID = "";
						string  scienceDegreeUniversityCountryTitle = "";
						string  scienceDegreeUniversityLogoFolder = db->Get(0, "logo_folder");
						string  scienceDegreeUniversityLogoFilename = db->Get(0, "logo_filename");

						if(scienceDegreeUniversityRegionID.length() && db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + scienceDegreeUniversityRegionID + "\";"))
						{
							scienceDegreeUniversityRegionTitle = db->Get(0, "title");
							scienceDegreeUniversityCountryID = db->Get(0, "geo_country_id");
							if(scienceDegreeUniversityCountryID.length() && db->Query("SELECT * FROM `geo_country` WHERE `id`=\"" + scienceDegreeUniversityCountryID + "\";"))
							{
								scienceDegreeUniversityCountryTitle = db->Get(0, "title");
							}
							else
							{
								CLog	log;
								log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: university [" + scienceDegreeUniversityID + "] geo_region [" + scienceDegreeUniversityRegionID + "] geo_country [" + scienceDegreeUniversityCountryID + "] not found");
							}

						}
						else
						{
							CLog	log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]: university [" + scienceDegreeUniversityID + "] geo_region [" + scienceDegreeUniversityRegionID + "] not found");
						}

						{
							string  messageParamLikesUserList = GetUniversityDegreeLikesUsersList(usersUniversityDegreeID, user, db);
							string  messageParamCommentsCount = GetUniversityDegreeCommentsCount(scienceDegreeUniversityID, db);

							if(ostResult.str().length() > 10) ostResult << ",";

							ostResult << "{";

							ostResult << "\"scienceDegreeID\":\"" << usersUniversityDegreeID << "\",";
							ostResult << "\"scienceDegreeTitle\":\"" << scienceDegreeTitle << "\",";
							ostResult << "\"scienceDegreeUniversityTitle\":\"" << scienceDegreeUniversityTitle << "\",";
							ostResult << "\"scienceDegreeUniversityID\":\"" << scienceDegreeUniversityID << "\",";
							ostResult << "\"scienceDegreeUniversityRegionID\":\"" << scienceDegreeUniversityRegionID << "\",";
							ostResult << "\"scienceDegreeUniversityRegionTitle\":\"" << scienceDegreeUniversityRegionTitle << "\",";
							ostResult << "\"scienceDegreeUniversityCountryID\":\"" << scienceDegreeUniversityCountryID << "\",";
							ostResult << "\"scienceDegreeUniversityCountryTitle\":\"" << scienceDegreeUniversityCountryTitle << "\",";
							ostResult << "\"scienceDegreeUniversityLogoFolder\":\""<< scienceDegreeUniversityLogoFolder << "\",";
							ostResult << "\"scienceDegreeUniversityLogoFilename\":\""<< scienceDegreeUniversityLogoFilename << "\",";
							ostResult << "\"scienceDegreeStart\":\""<< scienceDegreeStart << "\",";
							ostResult << "\"scienceDegreeFinish\":\""<< scienceDegreeFinish << "\",";

							ostResult << "\"scienceDegreeCommentsCount\":\"" << messageParamCommentsCount << "\",";
							ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

							ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
							ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
							ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
							ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
							ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
							ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
							ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
							ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
							ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
							ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
							ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
							ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
							ostResult << "}";
						}

					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: university ID [" + scienceDegreeUniversityID + "] not found");
					}

				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_university_id [" + feedActionId + "] not found");
				}
			}
			else if(feedActionTypeId == "40")
			{
				// --- language improved

				if(db->Query("SELECT * FROM `users_language` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  usersLanguageID = "";
					string  languageID = "";
					string  languageUserID = "";
					string  languageLevel = "";

					usersLanguageID = db->Get(0, "id");
					languageID = db->Get(0, "language_id");
					languageUserID = db->Get(0, "user_id");
					languageLevel = db->Get(0, "level");

					if(languageID.length() && db->Query("SELECT * FROM `language` WHERE `id`=\"" + languageID + "\";"))
					{
						string  languageTitle = db->Get(0, "title");
						string  languageLogoFolder = db->Get(0, "logo_folder");
						string  languageLogoFilename = db->Get(0, "logo_filename");
						string  messageParamLikesUserList = GetLanguageLikesUsersList(usersLanguageID, user, db);
						string  messageParamCommentsCount = GetLanguageCommentsCount(languageID, db);

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"languageID\":\""		  << languageID << "\",";
						ostResult << "\"usersLanguageID\":\""   << usersLanguageID << "\",";
						ostResult << "\"languageTitle\":\""   << languageTitle << "\",";
						ostResult << "\"languageLogoFolder\":\""<< languageLogoFolder << "\",";
						ostResult << "\"languageLogoFilename\":\""<< languageLogoFilename << "\",";
						ostResult << "\"languageLevel\":\""  << languageLevel << "\",";

						ostResult << "\"languageCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: language[" + languageID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_language_id [" + feedActionId + "] not found");
				}

			}
			else if(feedActionTypeId == "1")
			{
				// --- change employment

				if(db->Query("SELECT * FROM `users_company` WHERE `id`=\"" + feedActionId + "\";"))
				{
					string  usersCompanyID = "";
					string  companyID = "";
					string  companyUserID = "";
					string  companyPositionTitleID = "";

					usersCompanyID = db->Get(0, "id");
					companyID = db->Get(0, "company_id");
					companyUserID = db->Get(0, "user_id");
					companyPositionTitleID = db->Get(0, "position_title_id");

					if(companyID.length() && db->Query("SELECT * FROM `company` WHERE `id`=\"" + companyID + "\";"))
					{
						string  companyTitle = db->Get(0, "name");
						string  companyLogoFolder = db->Get(0, "logo_folder");
						string  companyLogoFilename = db->Get(0, "logo_filename");
						string  companyPositionTitle = "";
						string  messageParamLikesUserList = GetCompanyLikesUsersList(usersCompanyID, user, db);
						string  messageParamCommentsCount = GetCompanyCommentsCount(companyID, db);

						if(companyPositionTitleID.length() && db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + companyPositionTitleID + "\";"))
							companyPositionTitle = db->Get(0, "title");

						if(ostResult.str().length() > 10) ostResult << ",";

						ostResult << "{";

						ostResult << "\"companyID\":\""		<< companyID << "\",";
						ostResult << "\"usersCompanyID\":\""	 << usersCompanyID << "\",";
						ostResult << "\"companyTitle\":\""  << companyTitle << "\",";
						ostResult << "\"companyLogoFolder\":\""<< companyLogoFolder << "\",";
						ostResult << "\"companyLogoFilename\":\""<< companyLogoFilename << "\",";
						ostResult << "\"companyPositionTitleID\":\""	<< companyPositionTitleID << "\",";
						ostResult << "\"companyPositionTitle\":\""	<< companyPositionTitle << "\",";

						ostResult << "\"companyCommentsCount\":\"" << messageParamCommentsCount << "\",";
						ostResult << "\"messageLikesUserList\":[" << messageParamLikesUserList << "],";

						ostResult << "\"avatar\":\""			<< srcAvatarPath << "\",";
						ostResult << "\"srcObj\":{"			 << messageSrcObject << "},";
						ostResult << "\"messageOwner\":{" << messageSrcObject << "},";
						ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
						ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
						ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
						ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
						ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
						ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
						ostResult << "\"actionTypesId\":\""   << itemsList[i].feed_actionTypeId << "\",";
						ostResult << "\"eventTimestamp\":\""	<< itemsList[i].feed_eventTimestamp << "\",";
						ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)) << "\"";
						ostResult << "}";
					}
					else
					{
						CLog	log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: company[" + companyID + "] not found");
					}


				}
				else
				{
					CLog	log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR: users_company_id [" + feedActionId + "] not found");
				}

			}
			else
			{

				if(ostResult.str().length() > 10) ostResult << ",";

				ostResult << "{";
				ostResult << "\"avatar\":\""			  << srcAvatarPath															<< "\",";
				ostResult << "\"srcObj\":{"	<< messageSrcObject << "},";
				ostResult << "\"actionCategoryTitle\":\"" << itemsList[i].action_category_title		   << "\",";
				ostResult << "\"actionCategoryTitleMale\":\"" << itemsList[i].action_category_title_male	<< "\",";
				ostResult << "\"actionCategoryTitleFemale\":\""<< itemsList[i].action_category_title_female   << "\",";
				ostResult << "\"actionTypesTitle\":\""  << itemsList[i].action_types_title			  << "\",";
				ostResult << "\"actionTypesTitleMale\":\""<< itemsList[i].action_types_title_male		 << "\",";
				ostResult << "\"actionTypesTitleFemale\":\""<< itemsList[i].action_types_title_female	 << "\",";
				ostResult << "\"actionTypesId\":\""	<< itemsList[i].feed_actionTypeId										   << "\",";
				ostResult << "\"eventTimestamp\":\""	  << itemsList[i].feed_eventTimestamp										<< "\",";
				// ostResult << "\"eventTimestampDelta\":\""  << GetHumanReadableTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp)   << "\"";
				ostResult << "\"eventTimestampDelta\":\"" << to_string(GetTimeDifferenceFromNow(itemsList[i].feed_eventTimestamp))  << "\"";
				ostResult << "}";

				// if(i < (affected - 1)) ostResult << ",";
			}
		}

		return ostResult.str();
}

// --- Returns company list in JSON format grabbed from DB
// --- Input:   dbQuery 		- SQL format returns users
//			  db	  			- DB connection
//			  user				- current user object, used to define company admin
//			  quickSearch		- owners, founders, openVacancies are not included into result
//			  include_employees	- used in admin _ONLY_ to get company list + # of users in each company
string GetCompanyListInJSONFormat(string dbQuery, CMysql *db, CUser *user, bool quickSearch/* = true*/, bool include_employees/*= false*/)
{
	struct CompanyClass {
		string	id;
		string	type;
		string	name;
		string	legal_geo_zip_id;
		string	legal_address;
		string	mailing_geo_zip_id;
		string	mailing_address;
		string	tin;
		string	bank_id;
		string	account;
		string	kpp;
		string	ogrn;
		string	vat;
		string	link;
		string	foundationDate;
		string	numberOfEmployee;
		string	admin_userID;
		string	webSite;
		string	description;
		string	logo_folder;
		string	logo_filename;
		string	employedUsersList;
	};

	ostringstream				ost, ostFinal;
	string						sessid, lookForKey;
	int							affected;
	vector<CompanyClass>		companiesList;
	int							companyCounter = 0;

	MESSAGE_DEBUG("", "", "start");

	ostFinal.str("");

	if(db)
	{
		if((affected = db->Query(dbQuery)) > 0)
		{
			companyCounter = affected;
			companiesList.reserve(companyCounter);  // --- reserving allows avoid moving vector in memory
													// --- to fit vector into continous memory piece

			for(int i = 0; i < affected; i++)
			{
				CompanyClass	company;

				company.id = db->Get(i, "id");
				company.type = db->Get(i, "type");
				company.name = db->Get(i, "name");
				company.legal_geo_zip_id = db->Get(i, "legal_geo_zip_id");
				company.legal_address = db->Get(i, "legal_address");
				company.mailing_geo_zip_id = db->Get(i, "mailing_geo_zip_id");
				company.mailing_address = db->Get(i, "mailing_address");
				company.tin = db->Get(i, "tin");
				company.bank_id = db->Get(i, "bank_id");
				company.account = db->Get(i, "account");
				company.kpp = db->Get(i, "kpp");
				company.ogrn = db->Get(i, "ogrn");
				company.vat = db->Get(i, "vat");
				company.link = db->Get(i, "link");
				company.admin_userID = db->Get(i, "admin_userID");
				company.foundationDate = db->Get(i, "foundationDate");
				company.numberOfEmployee = db->Get(i, "numberOfEmployee");
				company.webSite = db->Get(i, "webSite");
				company.description = db->Get(i, "description");
				company.logo_folder = db->Get(i, "logo_folder");
				company.logo_filename = db->Get(i, "logo_filename");
				company.employedUsersList = "";
				companiesList.push_back(company);
			}

			for(int i = 0; i < companyCounter; i++)
			{
					if(ostFinal.str().length()) ostFinal << ", ";

					ostFinal << "{";
					ostFinal << "\"id\": \""				<< companiesList[i].id << "\",";
					ostFinal << "\"type\": \""				<< companiesList[i].type << "\", ";
					ostFinal << "\"name\": \""				<< companiesList[i].name << "\", ";
					ostFinal << "\"legal_geo_zip_id\": \""	<< companiesList[i].legal_geo_zip_id << "\", ";
					ostFinal << "\"legal_geo_zip\": ["		<< GetZipInJSONFormat(companiesList[i].legal_geo_zip_id, db, user) << "], ";
					ostFinal << "\"legal_address\": \""		<< companiesList[i].legal_address << "\", ";
					ostFinal << "\"mailing_geo_zip_id\": \""<< companiesList[i].mailing_geo_zip_id << "\", ";
					ostFinal << "\"mailing_geo_zip\": ["	<< GetZipInJSONFormat(companiesList[i].mailing_geo_zip_id, db, user) << "], ";
					ostFinal << "\"mailing_address\": \""	<< companiesList[i].mailing_address << "\", ";
					ostFinal << "\"tin\": \""				<< companiesList[i].tin << "\", ";
					ostFinal << "\"bank_id\": \""			<< companiesList[i].bank_id << "\", ";
					ostFinal << "\"banks\": ["				<< GetBankInJSONFormat("SELECT * FROM `banks` WHERE `id`=\"" + companiesList[i].bank_id + "\";", db, user) << "], ";
					ostFinal << "\"account\": \""			<< companiesList[i].account << "\", ";
					ostFinal << "\"kpp\": \""				<< companiesList[i].kpp << "\", ";
					ostFinal << "\"ogrn\": \""				<< companiesList[i].ogrn << "\", ";
					ostFinal << "\"vat\": \""				<< companiesList[i].vat << "\", ";
					ostFinal << "\"link\": \""				<< companiesList[i].link << "\", ";
					ostFinal << "\"foundationDate\": \""	<< companiesList[i].foundationDate << "\",";
					ostFinal << "\"numberOfEmployee\": \""	<< companiesList[i].numberOfEmployee << "\",";
					ostFinal << "\"webSite\": \""			<< companiesList[i].webSite << "\",";
					ostFinal << "\"description\": \""		<< companiesList[i].description << "\",";
					ostFinal << "\"logo_folder\": \""		<< companiesList[i].logo_folder << "\",";
					ostFinal << "\"logo_filename\": \""	   	<< companiesList[i].logo_filename << "\",";
					ostFinal << "\"owners\": ["			   	<< (include_employees ? GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + companiesList[i].admin_userID + "\";", db, user) : "") << "],";
					ostFinal << "\"employees\": ["			<< (include_employees ? GetAgencyEmployeesInJSONFormat(companiesList[i].id, db, user) : "") << "],";
					ostFinal << "\"custom_fields\":[" 		<< GetCompanyCustomFieldsInJSONFormat("SELECT * FROM `company_custom_fields` WHERE `company_id`=\"" + companiesList[i].id + "\" "
																+	(
																	user 
																		? (user->GetType() == "subcontractor")
																	 		? " AND (`visible_by_subcontractor`=\"Y\" OR `editable_by_subcontractor`=\"Y\") " 
																	 		: ""
																	 	: ""
																	 )
																+ ";", db, user)
																+ "],";
					ostFinal << "\"isMine\": \""			<< (user ? companiesList[i].admin_userID == user->GetID() : false) << "\"";
					ostFinal << "}";
			} // --- for loop through company list
		} // --- if sql-query on company selection success
		else
		{
			MESSAGE_DEBUG("", "", "there are no companies returned by request [" + dbQuery + "]");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (number of companies returned is " + to_string(companyCounter) + ")");

	return ostFinal.str();
}

string  GetUserNotificationSpecificDataByType(unsigned long typeID, unsigned long actionID, CMysql *db, CUser *user)
{
	ostringstream   ostResult, ost;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "(typeID=" + to_string(typeID) + ", actionID=" + to_string(actionID) + ")[" + to_string(__LINE__) + "]: start");
	}


	ostResult.str("");

	// --- comment provided
	if(typeID == 19)
	{
		unsigned long   comment_id = actionID;

		if(db->Query("select * from  `feed_message_comment` where `id`=\"" + to_string(comment_id) + "\";"))
		{
			string  friend_userID = db->Get(0, "userID");
			string  commentTitle = db->Get(0, "comment");
			string  commentTimestamp = db->Get(0, "eventTimestamp");
			string  messageID = db->Get(0, "messageID");
			string  commentType = db->Get(0, "type");

			if(commentType == "message")
			{
				if(db->Query("select * from  `feed_message` where `id`=\"" + messageID + "\";"))
				{
					string  messageTitle = db->Get(0, "title");
					string  messageBody = db->Get(0, "message");
					string  messageImageSetID = db->Get(0, "imageSetID");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");
						string	  imageSetFolder = "";
						string	  imageSetPic = "";
						string	  messageMediaType = "";

						if(messageImageSetID.length() && (messageImageSetID != "0") && (db->Query("select * from `feed_images` where `setID`=\"" + messageImageSetID + "\";")))
						{
							imageSetFolder = db->Get(0, "folder");
							imageSetPic = db->Get(0, "filename");
							messageMediaType = db->Get(0, "mediaType");
						}
						else
						{
							CLog log;
							MESSAGE_DEBUG("", "", "typeID=" + to_string(typeID) + ": there is no media assigned to message");
						}

						ostResult << "\"notificationMessageID\":\"" << messageID << "\",";
						ostResult << "\"notificationMessageTitle\":\"" << messageTitle << "\",";
						ostResult << "\"notificationMessageBody\":\"" << messageBody << "\",";
						ostResult << "\"notificationMessageImageFolder\":\"" << imageSetFolder << "\",";
						ostResult << "\"notificationMessageImageName\":\"" << imageSetPic << "\",";
						ostResult << "\"notificationMessageMediaType\":\"" << messageMediaType << "\",";
						ostResult << "\"notificationCommentType\":\"message\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR finding messageID[" + messageID + "] in feed_message");
				}
			} // --- comment type "message"

			if(commentType == "book")
			{
				string	  bookID = messageID;

				if(db->Query("select * from  `book` where `id`=\"" + bookID + "\";"))
				{
					string  bookTitle = db->Get(0, "title");
					string  bookAuthorID = db->Get(0, "authorID");
					string  bookAuthor;
					string  bookISBN10 = db->Get(0, "isbn10");
					string  bookISBN13 = db->Get(0, "isbn13");
					string  coverPhotoFolder = db->Get(0, "coverPhotoFolder");
					string  coverPhotoFilename = db->Get(0, "coverPhotoFilename");

					if(bookAuthorID.length() && db->Query("select * from `book_author` where `id`=\"" + bookAuthorID + "\";"))
						bookAuthor = db->Get(0, "name");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationBookID\":\"" << bookID << "\",";
						ostResult << "\"notificationBookTitle\":\"" << bookTitle << "\",";
						ostResult << "\"notificationBookAuthorID\":\"" << bookAuthorID << "\",";
						ostResult << "\"notificationBookAuthor\":\"" << bookAuthor << "\",";
						ostResult << "\"notificationBookISBN10\":\"" << bookISBN10 << "\",";
						ostResult << "\"notificationBookISBN13\":\"" << bookISBN13 << "\",";
						ostResult << "\"notificationBookImageFolder\":\"" << coverPhotoFolder << "\",";
						ostResult << "\"notificationBookImageName\":\"" << coverPhotoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR finding book in feed_message");
				}
			} // --- comment type "book"

			if(commentType == "certification")
			{
				string	  trackID = messageID;
				string	  usersCertificationsID = "";
				string	  certificationNumber = "";

				if(db->Query("select * from `users_certifications` where `track_id`=\"" + trackID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					string	  usersCertificationsID = db->Get(0, "id");
					string	  certificationNumber = db->Get(0, "certification_number");
				}
				else
				{
					CLog log;
					MESSAGE_DEBUG("", "", "typeID=" + to_string(typeID) + ": finding users_certifications by (track_id[" + trackID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
				{
					string  certificationTitle = db->Get(0, "title");
					string  vendorID = db->Get(0, "vendor_id");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  companyName = "";

					if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
						companyName = db->Get(0, "name");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersCertificationID\":\"" << usersCertificationsID << "\",";
						ostResult << "\"notificationCertificationTitle\":\"" << certificationTitle << "\",";
						ostResult << "\"notificationCertificationNumber\":\"" << certificationNumber << "\",";
						ostResult << "\"notificationCertificationID\":\"" << trackID << "\",";
						ostResult << "\"notificationCertificationCompanyID\":\"" << vendorID << "\",";
						ostResult << "\"notificationCertificationCompanyName\":\"" << companyName << "\",";
						ostResult << "\"notificationCertificationImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationCertificationImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: certification.id[" + trackID + "] not found");
				}
			} // --- if(likeType == "certification")

			if(commentType == "course")
			{
				string	  trackID = messageID;
				string	  usersCoursesID = "";

				if(db->Query("select * from `users_courses` where `track_id`=\"" + trackID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersCoursesID = db->Get(0, "id");
				}
				else
				{
					CLog log;
					MESSAGE_DEBUG("", "", "typeID=" + to_string(typeID) + ": finding users_courses by (track_id[" + trackID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
				{
					string  courseTitle = db->Get(0, "title");
					string  vendorID = db->Get(0, "vendor_id");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  companyName = "";

					if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
						companyName = db->Get(0, "name");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersCourseID\":\"" << usersCoursesID << "\",";
						ostResult << "\"notificationCourseTitle\":\"" << courseTitle << "\",";
						ostResult << "\"notificationCourseID\":\"" << trackID << "\",";
						ostResult << "\"notificationCourseCompanyID\":\"" << vendorID << "\",";
						ostResult << "\"notificationCourseCompanyName\":\"" << companyName << "\",";
						ostResult << "\"notificationCourseImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationCourseImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: course.id[" + trackID + "] not found");
				}
			} // --- if(likeType == "course")


			if(commentType == "university")
			{
				string	  universityID = messageID;
				string	  usersUniversityDegreeID = "";
				string	  degree = "";
				string	  studyStart = "";
				string	  studyFinish = "";

				if(db->Query("select * from `users_university` where `university_id`=\"" + universityID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersUniversityDegreeID = db->Get(0, "id");
					degree = db->Get(0, "degree");
					studyStart = db->Get(0, "occupation_start");
					studyFinish = db->Get(0, "occupation_finish");
				}
				else
				{
					CLog log;
					MESSAGE_DEBUG("", "", "typeID=" + to_string(typeID) + ": finding users_universitys by (university_id[" + universityID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `university` where `id`=\"" + universityID + "\";"))
				{
					string  universityTitle = db->Get(0, "title");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  universityCountryID = "";
					string  universityCountryName = "";
					string  universityRegionID = db->Get(0, "geo_region_id");
					string  universityRegionName = "";

					if(universityRegionID.length() && db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + universityRegionID + "\";"))
					{
						universityRegionName = db->Get(0, "title");
						universityCountryID = db->Get(0, "geo_country_id");

						if(universityCountryID.length() && db->Query("SELECT * FROM `geo_country` WHERE `id`=\"" + universityCountryID + "\";"))
							universityCountryName = db->Get(0, "title");
					}

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersUniversityID\":\"" << usersUniversityDegreeID << "\",";
						ostResult << "\"notificationUniversityID\":\"" << universityID << "\",";
						ostResult << "\"notificationUniversityTitle\":\"" << universityTitle << "\",";
						ostResult << "\"notificationUniversityDegree\":\"" << degree << "\",";
						ostResult << "\"notificationUniversityStart\":\"" << studyStart << "\",";
						ostResult << "\"notificationUniversityFinish\":\"" << studyFinish << "\",";
						ostResult << "\"notificationUniversityRegionID\":\"" << universityRegionID << "\",";
						ostResult << "\"notificationUniversityRegionName\":\"" << universityRegionName << "\",";
						ostResult << "\"notificationUniversityCountryID\":\"" << universityCountryID << "\",";
						ostResult << "\"notificationUniversityCountryName\":\"" << universityCountryName << "\",";
						ostResult << "\"notificationUniversityImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationUniversityImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: university.id[" + universityID + "] not found");
				}
			} // --- if(commentType == "university")
			if(commentType == "company")
			{
				string	  companyID = messageID;

				string	  usersCompanyID = "";
				string	  positionTitleID = "";
				string	  employmentStart = "";
				string	  employmentFinish = "";

				if(db->Query("select * from `users_company` where `company_id`=\"" + companyID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersCompanyID = db->Get(0, "id");
					positionTitleID = db->Get(0, "position_title_id");
					employmentStart = db->Get(0, "occupation_start");
					employmentFinish = db->Get(0, "occupation_finish");
				}
				else
				{
					CLog log;
					MESSAGE_DEBUG("", "", "typeID=" + to_string(typeID) + ": finding users_companys by (company_id[" + companyID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `company` where `id`=\"" + companyID + "\";"))
				{
					string  companyName = db->Get(0, "name");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");
					string  companyWebSite = db->Get(0, "webSite");
					string  companyFoundationDate = db->Get(0, "foundationDate");
					string  positionTitle = "";

					if(positionTitleID.length() && db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + positionTitleID + "\";"))
						positionTitle = db->Get(0, "title");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersCompanyID\":\"" << usersCompanyID << "\",";
						ostResult << "\"notificationCompanyID\":\"" << companyID << "\",";
						ostResult << "\"notificationCompanyName\":\"" << companyName << "\",";
						ostResult << "\"notificationCompanyPositionTitleID\":\"" << positionTitleID << "\",";
						ostResult << "\"notificationCompanyPositionTitle\":\"" << positionTitle << "\",";
						ostResult << "\"notificationCompanyStart\":\"" << employmentStart << "\",";
						ostResult << "\"notificationCompanyFinish\":\"" << employmentFinish << "\",";
						ostResult << "\"notificationCompanyFoundationDate\":\"" << companyFoundationDate << "\",";
						ostResult << "\"notificationCompanyWebSite\":\"" << companyWebSite << "\",";
						ostResult << "\"notificationCompanyImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationCompanyImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: company.id[" + companyID + "] not found");
				}
			} // --- if(commentType == "company")
			if(commentType == "language")
			{
				string	  languageID = messageID;
				string	  usersLanguageID = "";
				string	  languageLevel = "";

				if(db->Query("select * from `users_language` where `language_id`=\"" + languageID + "\" and `user_id`=\"" + friend_userID + "\";"))
				{
					usersLanguageID = db->Get(0, "id");
					languageLevel = db->Get(0, "level");
				}
				else
				{
					CLog log;
					MESSAGE_DEBUG("", "", "typeID=" + to_string(typeID) + ": finding users_languages by (language_id[" + languageID + "] and user_id[" + (user ? user->GetID() : "NULL") + "]) or (user[" + (user ? "not null" : "null") + "] == NULL)");
				}


				if(db->Query("select * from  `language` where `id`=\"" + languageID + "\";"))
				{
					string  languageTitle = db->Get(0, "title");
					string  logoFolder = db->Get(0, "logo_folder");
					string  logoFilename = db->Get(0, "logo_filename");

					if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
					{
						string	  friend_userName = db->Get(0, "name");
						string	  friend_userNameLast = db->Get(0, "nameLast");
						string	  friend_sex = db->Get(0, "sex");

						ostResult << "\"notificationUsersLanguageID\":\"" << usersLanguageID << "\",";
						ostResult << "\"notificationLanguageID\":\"" << languageID << "\",";
						ostResult << "\"notificationLanguageTitle\":\"" << languageTitle << "\",";
						ostResult << "\"notificationLanguageLevel\":\"" << languageLevel << "\",";
						ostResult << "\"notificationLanguageImageFolder\":\"" << logoFolder << "\",";
						ostResult << "\"notificationLanguageImageName\":\"" << logoFilename << "\",";
						ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
						ostResult << "\"notificationCommentID\":\"" << comment_id << "\",";
						ostResult << "\"notificationCommentTitle\":\"" << commentTitle << "\",";
						ostResult << "\"notificationCommentEventTimestamp\":\"" << commentTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: language.id[" + languageID + "] not found");
				}
			} // --- if(commentType == "language")
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_recommendation");
		}
	}


	// --- skill confirmed
	if(typeID == 43)
	{
		unsigned long   skill_confirmed_id = actionID;

		if(db->Query("select * from `skill_confirmed` where `id`=\"" + to_string(skill_confirmed_id) + "\";"))
		{
			string  users_skill_id = db->Get(0, "users_skill_id");
			string  approver_userID = db->Get(0, "approver_userID");

			ost.str("");
			ost << "select * from `users` where `id`='" << approver_userID << "';";
			if(db->Query(ost.str()))
			{
				string  approver_userName = db->Get(0, "name");
				string  approver_userNameLast = db->Get(0, "nameLast");

				ost.str("");
				ost << "select * from `users_skill` where `id`='" << users_skill_id << "';";
				if(db->Query(ost.str()))
				{
					string  skillID = db->Get(0, "skill_id");

					ost.str("");
					ost << "select * from `skill` where `id`='" << skillID << "';";
					if(db->Query(ost.str()))
					{
						string  skillTitle = db->Get(0, "title");

						ostResult << "\"notificationFriendUserID\":\"" << approver_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << approver_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << approver_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(approver_userID, db) << "\",";
						ostResult << "\"notificationSkillTitle\":\"" << skillTitle << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_skill");
				}

			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill_confirmed");
		}
	}

	// --- skill removed
	if(typeID == 44)
	{
		ost.str("");
		{
			unsigned long   users_skill_id = actionID;

			{
				ost.str("");
				ost << "select * from `users_skill` where `id`='" << users_skill_id << "';";
				if(db->Query(ost.str()))
				{
					string  skillID = db->Get(0, "skill_id");

					ost.str("");
					ost << "select * from `skill` where `id`='" << skillID << "';";
					if(db->Query(ost.str()))
					{
						string  skillTitle = db->Get(0, "title");

						ostResult << "\"notificationSkillTitle\":\"" << skillTitle << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_skill");
				}

			}

		}
	}

	// --- recommendation provided
	if(typeID == 45)
	{
		ost.str("");
		{
			unsigned long   users_recommendation_id = actionID;

			{
				ost.str("");
				ost << "select * from `users_recommendation` where `id`=\"" << users_recommendation_id << "\";";
				if(db->Query(ost.str()))
				{
					string  recommended_userID = db->Get(0, "recommended_userID");
					string  recommending_userID = db->Get(0, "recommending_userID");
					string  title = db->Get(0, "title");
					string  eventTimestamp = db->Get(0, "eventTimestamp");

					ost.str("");
					ost << "select * from `users` where `id`='" << recommending_userID << "';";
					if(db->Query(ost.str()))
					{
						string  recommending_userName = db->Get(0, "name");
						string  recommending_userNameLast = db->Get(0, "nameLast");

						ostResult << "\"notificationFriendUserID\":\"" << recommending_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << recommending_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << recommending_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(recommending_userID, db) << "\",";
						ostResult << "\"notificationRecommendationID\":\"" << users_recommendation_id << "\",";
						ostResult << "\"notificationRecommendationTitle\":\"" << title << "\",";
						ostResult << "\"notificationRecommendationEventTimestamp\":\"" << eventTimestamp << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_recommendation (probably deleted already)");
				}

			}

		}
	}

	// --- 46/47 recommendation deleted by benefit-owner
	if((typeID == 46) || (typeID == 47))
	{
		ost.str("");
		{
			unsigned long   friend_userID = actionID;

			{
				{
					ost.str("");
					ost << "select * from `users` where `id`='" << friend_userID << "';";
					if(db->Query(ost.str()))
					{
						string  friend_userName = db->Get(0, "name");
						string  friend_userNameLast = db->Get(0, "nameLast");

						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(to_string(friend_userID), db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
			}
		}
	}

	// --- recommendation modified
	if(typeID == 48)
	{
		unsigned long   user_recommendation_id = actionID;

		ost.str("");
		ost << "select * from  `users_recommendation` where `id`=\"" << user_recommendation_id << "\";";
		if(db->Query(ost.str()))
		{
			string  recommended_userID = db->Get(0, "recommended_userID");
			string  friend_userID = db->Get(0, "recommending_userID");
			string  recommendationTitle = db->Get(0, "title");
			string  recommendationEventTimestamp = db->Get(0, "eventTimestamp");

			{
				{
					ost.str("");
					ost << "select * from `users` where `id`='" << friend_userID << "';";
					if(db->Query(ost.str()))
					{
						string  friend_userName = db->Get(0, "name");
						string  friend_userNameLast = db->Get(0, "nameLast");

						ostResult << "\"notificationRecommendationID\":\"" << user_recommendation_id << "\",";
						ostResult << "\"notificationRecommendationTitle\":\"" << recommendationTitle << "\",";
						ostResult << "\"notificationRecommendationEventTimestamp\":\"" << recommendationEventTimestamp << "\",";
						ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
						ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
						ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
						ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from skill");
					}

				}
			}
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users_recommendation");
		}
	}

	// --- message liked
	if(typeID == 49)
	{
		unsigned long   feed_message_params_id = actionID;

		if(db->Query("select * from  `feed_message_params` where `id`=\"" + to_string(feed_message_params_id) + "\";"))
		{
			string  friend_userID = db->Get(0, "userID");
			string  messageID = db->Get(0, "messageID");
			string  commentType = db->Get(0, "parameter");

			if((commentType == "like"))
			{
				if(db->Query("select * from  `feed_message` where `id`=\"" + messageID + "\";"))
				{
					string  messageTitle = db->Get(0, "title");
					string  messageBody = db->Get(0, "message");
					string  messageImageSetID = db->Get(0, "imageSetID");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");
							string	  imageSetFolder = "";
							string	  imageSetPic = "";
							string	  messageMediaType = "";

							if((messageImageSetID != "0") && (db->Query("select * from `feed_images` where `setID`=\"" + messageImageSetID + "\";")))
							{
								imageSetFolder = db->Get(0, "folder");
								imageSetPic = db->Get(0, "filename");
								messageMediaType = db->Get(0, "mediaType");
							}
							else
							{
								CLog log;
								MESSAGE_DEBUG("", "", "typeID=49: there is no media assigned to message");
							} // --- imageSet is empty

							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationMessageID\":\"" << messageID << "\",";
							ostResult << "\"notificationMessageTitle\":\"" << messageTitle << "\",";
							ostResult << "\"notificationMessageBody\":\"" << messageBody << "\",";
							ostResult << "\"notificationMessageImageFolder\":\"" << imageSetFolder << "\",";
							ostResult << "\"notificationMessageImageName\":\"" << imageSetPic << "\",";
							ostResult << "\"notificationMessageMediaType\":\"" << messageMediaType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";
						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: selecting from users");
						}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding message in feed_message");
				}
			} // --- if(likeType == "message")


			if(commentType == "likeBook")
			{
				string	  usersBooksID = messageID;

				if(db->Query("select * from `users_books` where `id`=\"" + usersBooksID + "\";"))
				{
					string	  bookID = db->Get(0, "bookID");

					if(db->Query("select * from  `book` where `id`=\"" + bookID + "\";"))
					{
						string  bookTitle = db->Get(0, "title");
						string  bookAuthorID = db->Get(0, "authorID");
						string  bookAuthor;
						string  bookISBN10 = db->Get(0, "isbn10");
						string  bookISBN13 = db->Get(0, "isbn13");
						string  coverPhotoFolder = db->Get(0, "coverPhotoFolder");
						string  coverPhotoFilename = db->Get(0, "coverPhotoFilename");

						if(bookAuthorID.length() && db->Query("select * from `book_author` where `id`=\"" + bookAuthorID + "\";"))
							bookAuthor = db->Get(0, "name");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationBookID\":\"" << bookID << "\",";
							ostResult << "\"notificationBookTitle\":\"" << bookTitle << "\",";
							ostResult << "\"notificationBookAuthorID\":\"" << bookAuthorID << "\",";
							ostResult << "\"notificationBookAuthor\":\"" << bookAuthor << "\",";
							ostResult << "\"notificationBookISBN10\":\"" << bookISBN10 << "\",";
							ostResult << "\"notificationBookISBN13\":\"" << bookISBN13 << "\",";
							ostResult << "\"notificationBookImageFolder\":\"" << coverPhotoFolder << "\",";
							ostResult << "\"notificationBookImageName\":\"" << coverPhotoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: book.id[" + bookID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_books.id[" + usersBooksID + "]");
				}
			} // --- if(likeType == "book")

			if(commentType == "likeCertification")
			{
				string	  usersCertificationsID = messageID;

				if(db->Query("select * from `users_certifications` where `id`=\"" + usersCertificationsID + "\";"))
				{
					string	  trackID = db->Get(0, "track_id");
					string	  certificationNumber = db->Get(0, "certification_number");

					if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
					{
						string  certificationTitle = db->Get(0, "title");
						string  vendorID = db->Get(0, "vendor_id");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");
						string  companyName = "";

						if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
							companyName = db->Get(0, "name");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersCertificationID\":\"" << usersCertificationsID << "\",";
							ostResult << "\"notificationCertificationID\":\"" << trackID << "\",";
							ostResult << "\"notificationCertificationTitle\":\"" << certificationTitle << "\",";
							ostResult << "\"notificationCertificationNumber\":\"" << certificationNumber << "\",";
							ostResult << "\"notificationCertificationCompanyID\":\"" << vendorID << "\",";
							ostResult << "\"notificationCertificationCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCertificationImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationCertificationImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: certification.id[" + trackID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_certifications.id[" + usersCertificationsID + "]");
				}
			} // --- if(likeType == "certification")

			if(commentType == "likeCourse")
			{
				string	  usersCoursesID = messageID;

				if(db->Query("select * from `users_courses` where `id`=\"" + usersCoursesID + "\";"))
				{
					string	  trackID = db->Get(0, "track_id");

					if(db->Query("select * from  `certification_tracks` where `id`=\"" + trackID + "\";"))
					{
						string  courseTitle = db->Get(0, "title");
						string  vendorID = db->Get(0, "vendor_id");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");
						string  companyName = "";

						if(vendorID.length() && db->Query("select * from `company` where `id`=\"" + vendorID + "\";"))
							companyName = db->Get(0, "name");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersCourseID\":\"" << usersCoursesID << "\",";
							ostResult << "\"notificationCourseID\":\"" << trackID << "\",";
							ostResult << "\"notificationCourseTitle\":\"" << courseTitle << "\",";
							ostResult << "\"notificationCourseCompanyID\":\"" << vendorID << "\",";
							ostResult << "\"notificationCourseCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCourseImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationCourseImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: course.id[" + trackID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_courses.id[" + usersCoursesID + "]");
				}
			} // --- if(likeType == "course")
			if(commentType == "likeLanguage")
			{
				string	  usersLanguagesID = messageID;

				if(db->Query("select * from `users_language` where `id`=\"" + usersLanguagesID + "\";"))
				{
					string	  languageID = db->Get(0, "language_id");
					string	  languageLevel = db->Get(0, "level");

					if(db->Query("select * from  `language` where `id`=\"" + languageID + "\";"))
					{
						string  languageTitle = db->Get(0, "title");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersLanguageID\":\"" << usersLanguagesID << "\",";
							ostResult << "\"notificationLanguageID\":\"" << languageID << "\",";
							ostResult << "\"notificationLanguageTitle\":\"" << languageTitle << "\",";
							ostResult << "\"notificationLanguageLevel\":\"" << languageLevel << "\",";
							ostResult << "\"notificationLanguageImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationLanguageImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: language.id[" + languageID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_languages.id[" + usersLanguagesID + "]");
				}
			} // --- if(likeType == "language")

			if(commentType == "likeCompany")
			{
				string	  usersCompanysID = messageID;

				if(db->Query("select * from `users_company` where `id`=\"" + usersCompanysID + "\";"))
				{
					string	  companyID = db->Get(0, "company_id");
					string	  positionTitleID = db->Get(0, "position_title_id");
					string	  positionTitle = "";

					if(db->Query("select * from  `company` where `id`=\"" + companyID + "\";"))
					{
						string  companyName = db->Get(0, "name");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");

						if(positionTitleID.length() && db->Query("SELECT * FROM `company_position` WHERE `id`=\"" + positionTitleID + "\";"))
							positionTitle = db->Get(0, "title");

						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersCompanyID\":\"" << usersCompanysID << "\",";
							ostResult << "\"notificationCompanyID\":\"" << companyID << "\",";
							ostResult << "\"notificationCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCompanyCompanyName\":\"" << companyName << "\",";
							ostResult << "\"notificationCompanyPositionTitleID\":\"" << positionTitleID << "\",";
							ostResult << "\"notificationCompanyPositionTitle\":\"" << positionTitle << "\",";
							ostResult << "\"notificationCompanyImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationCompanyImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: company.id[" + companyID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_companys.id[" + usersCompanysID + "]");
				}
			} // --- if(likeType == "company")

			if(commentType == "likeUniversityDegree")
			{
				string	  usersUniversityDegreeID = messageID;

				if(db->Query("select * from `users_university` where `id`=\"" + usersUniversityDegreeID + "\";"))
				{
					string	  universityID = db->Get(0, "university_id");
					string	  degree = db->Get(0, "degree");
					string	  studyStart = db->Get(0, "occupation_start");
					string	  studyFinish = db->Get(0, "occupation_finish");

					if(db->Query("select * from  `university` where `id`=\"" + universityID + "\";"))
					{
						string  universityTitle = db->Get(0, "title");
						string  logoFolder = db->Get(0, "logo_folder");
						string  logoFilename = db->Get(0, "logo_filename");
						string  universityCountryID = "";
						string  universityCountryName = "";
						string  universityRegionID = db->Get(0, "geo_region_id");
						string  universityRegionName = "";

						if(universityRegionID.length() && db->Query("SELECT * FROM `geo_region` WHERE `id`=\"" + universityRegionID + "\";"))
						{
							universityRegionName = db->Get(0, "title");
							universityCountryID = db->Get(0, "geo_country_id");

							if(universityCountryID.length() && db->Query("SELECT * FROM `geo_country` WHERE `id`=\"" + universityCountryID + "\";"))
								universityCountryName = db->Get(0, "title");
						}


						if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
						{
							string	  friend_userName = db->Get(0, "name");
							string	  friend_userNameLast = db->Get(0, "nameLast");
							string	  friend_sex = db->Get(0, "sex");

							ostResult << "\"notificationUsersUniversityID\":\"" << messageID << "\",";
							ostResult << "\"notificationUniversityID\":\"" << universityID << "\",";
							ostResult << "\"notificationUniversityTitle\":\"" << universityTitle << "\",";
							ostResult << "\"notificationUniversityDegree\":\"" << degree << "\",";
							ostResult << "\"notificationUniversityStart\":\"" << studyStart << "\",";
							ostResult << "\"notificationUniversityFinish\":\"" << studyFinish << "\",";
							ostResult << "\"notificationUniversityRegionID\":\"" << universityRegionID << "\",";
							ostResult << "\"notificationUniversityRegionName\":\"" << universityRegionName << "\",";
							ostResult << "\"notificationUniversityCountryID\":\"" << universityCountryID << "\",";
							ostResult << "\"notificationUniversityCountryName\":\"" << universityCountryName << "\",";
							ostResult << "\"notificationUniversityImageFolder\":\"" << logoFolder << "\",";
							ostResult << "\"notificationUniversityImageName\":\"" << logoFilename << "\",";
							ostResult << "\"notificationCommentType\":\"" << commentType << "\",";
							ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
							ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
							ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
							ostResult << "\"notificationFriendUserSex\":\"" << friend_sex << "\",";
							ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

						}
						else
						{
							CLog log;
							log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
						}

					}
					else
					{
						CLog log;
						log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: university.id[" + universityID + "] not found");
					}
				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: finding users_universitys.id[" + usersUniversityDegreeID + "]");
				}
			} // --- if(commentType == "university")



		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from feed_message_params");
		}
	}

	// --- message disliked
	if(typeID == 50)
	{
		unsigned long   messageID = actionID;

		{

			ost.str("");
			ost << "select * from  `feed_message` where `id`=\"" << messageID << "\";";
			if(db->Query(ost.str()))
			{
				string	  messageTitle = db->Get(0, "title");
				string	  messageBody = db->Get(0, "message");
				string	  messageImageSetID = db->Get(0, "imageSetID");
				string	  imageSetFolder = "";
				string	  imageSetPic = "";
				string	  messageMediaType = "";

				if(db->Query("select * from `feed_images` where `setID`=\"" + messageImageSetID + "\";"))
				{
					imageSetFolder = db->Get(0, "folder");
					imageSetPic = db->Get(0, "filename");
					messageMediaType = db->Get(0, "mediaType");
				}
				else
				{
					CLog log;
					MESSAGE_DEBUG("", "", "typeID=50: there is no media assigned to message");
				}

				ostResult << "\"notificationMessageID\":\"" << messageID << "\",";
				ostResult << "\"notificationMessageTitle\":\"" << messageTitle << "\",";
				ostResult << "\"notificationMessageBody\":\"" << messageBody << "\",";
				ostResult << "\"notificationMessageMediaType\":\"" << messageMediaType << "\",";
				ostResult << "\"notificationMessageImageFolder\":\"" << imageSetFolder << "\",";
				ostResult << "\"notificationMessageImageName\":\"" << imageSetPic << "\"";
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR finding message int feed_message");
			}
		}
	}

	// --- birthday notification
	if(typeID == 58)
	{
		string   birthdayUserID = to_string(actionID);

		if(birthdayUserID.length() && db->Query("select * from `users` where `id`='" + birthdayUserID + "';"))
		{
			string  birthday_userName = db->Get(0, "name");
			string  birthday_userNameLast = db->Get(0, "nameLast");
			string	birthdayDate = db->Get(0, "birthday");


			ostResult << "\"notificationBirthdayDate\":\"" << birthdayDate << "\",";
			ostResult << "\"notificationFriendUserID\":\"" << birthdayUserID << "\",";
			ostResult << "\"notificationFriendUserName\":\"" << birthday_userName << "\",";
			ostResult << "\"notificationFriendUserNameLast\":\"" << birthday_userNameLast << "\",";
			ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(birthdayUserID, db) << "\"";
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + birthdayUserID + "]");
		}
	}

	// --- gift thank notification
	if(typeID == 66)
	{
		string   giftThankID = to_string(actionID);

		if(giftThankID.length() && db->Query("select * from `gift_thanks` where `id`='" + giftThankID + "';"))
		{
			string  friend_userID = db->Get(0, "src_user_id");
			string  dst_user_id = db->Get(0, "dst_user_id");
			string  gift_id = db->Get(0, "gift_id");
			string  comment = db->Get(0, "comment");

			if(db->Query("select * from `users` where `id`=\"" + friend_userID + "\";"))
			{
				string	  friend_userName = db->Get(0, "name");
				string	  friend_userNameLast = db->Get(0, "nameLast");

				ostResult << "\"gifts\":[" << GetGiftListInJSONFormat("SELECT * FROM `gifts` WHERE `id`=\"" + gift_id + "\";", db, user) << "],";
				ostResult << "\"notificationComment\":\"" << comment << "\",";
				ostResult << "\"notificationFriendUserID\":\"" << friend_userID << "\",";
				ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
				ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
				ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friend_userID, db) << "\"";

			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR: users.id[" + friend_userID + "] not found");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from gift_thanks[id = " + giftThankID + "]");
		}
	}

	// --- Vacancy Rejected
	if(typeID == 59)
	{
		string   company_candidates_id = to_string(actionID);

		if(company_candidates_id.length() && db->Query("select * from `company_candidates` where `id`='" + company_candidates_id + "';"))
		{
			string  user_id = db->Get(0, "user_id");
			string  vacancy_id = db->Get(0, "vacancy_id");

			if(vacancy_id.length() && db->Query("select * from `company_vacancy` where `id`='" + vacancy_id + "';"))
			{
				string		company_id = db->Get(0, "company_id");

				ostResult << "\"notificationCompanyCandidatesID\":\"" << company_candidates_id << "\",";
				ostResult << "\"notificationVacancy\":[" << GetOpenVacanciesInJSONFormat("SELECT * FROM `company_vacancy` WHERE `id`=\"" + vacancy_id + "\";", db) << "],";
				ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_vacancy[id = " + vacancy_id + "]");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_candidates[id = " + company_candidates_id + "]");
		}
	}

	// --- Company Posession Request
	if(typeID == 60)
	{
		string   company_posesison_request_id = to_string(actionID);

		if(company_posesison_request_id.length() && db->Query("select * from `company_posession_request` where `id`='" + company_posesison_request_id + "';"))
		{
			string  friendUserID = db->Get(0, "requester_user_id");
			string  company_id = db->Get(0, "requested_company_id");
			string	description = db->Get(0, "description");
			string	status = db->Get(0, "status");
			string	eventTimestamp = db->Get(0, "eventTimestamp");

			if(company_id.length() && db->Query("select * from `company` where `id`='" + company_id + "';"))
			{

				if(friendUserID.length() && db->Query("select * from `users` where `id`='" + friendUserID + "';"))
				{
					string  friend_userName = db->Get(0, "name");
					string  friend_userNameLast = db->Get(0, "nameLast");

					ostResult << "\"notificationCompanyPosessionRequestID\":\"" << company_posesison_request_id << "\",";
					ostResult << "\"notificationDescription\":\"" << description << "\",";
					ostResult << "\"notificationFriendUserID\":\"" << friendUserID << "\",";
					ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
					ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
					ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friendUserID, db) << "\",";
					ostResult << "\"notificationRequestedCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "],";
					ostResult << "\"notificationPosessionStatus\":\"" << status << "\",";
					ostResult << "\"notificationEventTimestamp\":\"" << eventTimestamp << "\"";

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + friendUserID + "]");
				}
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_vacancy[id = " + company_id + "]");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_posession_request[id = " + company_posesison_request_id + "]");
		}
	}

	// --- company posession approved / rejected
	if((typeID == 61) || (typeID == 62))
	{
		string   company_id = to_string(actionID);

		if(company_id.length())
		{
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from company_candidates[id = " + company_id + "]");
		}
	}

	// --- 67 - Event host added
	// --- 68 - Event guest added
	// --- 69 - Guest accepted
	if((typeID == 67) || (typeID == 68) || (typeID == 69))
	{
		string		event_host_id = to_string(actionID);
		string		host_guest_sql;

		if(typeID == 67)
			host_guest_sql = "SELECT * FROM `event_hosts` WHERE `id`=\"" + event_host_id + "\"";
		else
			host_guest_sql = "SELECT * FROM `event_guests` WHERE `id`=\"" + event_host_id + "\"";

		if(event_host_id.length() && db->Query(host_guest_sql))
		{
			string  event_id = db->Get(0, "event_id");
			string	eventTimestamp = db->Get(0, "eventTimestamp");
			string  friendUserID = "";

			if(typeID == 69) friendUserID = db->Get(0, "user_id");

			if(event_id.length() && db->Query("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";"))
			{
				if(!friendUserID.length()) friendUserID = db->Get(0, "owner_id");

				if(friendUserID.length() && db->Query("select * from `users` where `id`='" + friendUserID + "';"))
				{
					string  friend_userName = db->Get(0, "name");
					string  friend_userNameLast = db->Get(0, "nameLast");
					string  friend_userSex = db->Get(0, "sex");

					ostResult << "\"notificationFriendUserID\":\"" << friendUserID << "\",";
					ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
					ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
					ostResult << "\"notificationFriendUserSex\":\"" << friend_userSex << "\",";
					ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friendUserID, db) << "\",";
					ostResult << "\"notificationEvent\":[" << GetEventListInJSONFormat("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";", db, NULL) << "]";
					// ostResult << "\"notificationEventTimestamp\":\"" << eventTimestamp << "\"";

				}
				else
				{
					CLog log;
					log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + friendUserID + "]");
				}
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from event[id = " + event_id + "]");
			}

		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from event_hosts/event_guests[id = " + event_host_id + "]");
		}
	}

	// --- 70 - 1 day notification about start
	if(typeID == 70)
	{
		string		event_id = to_string(actionID);

		if(event_id.length() && db->Query("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";"))
		{
			string	eventTimestamp = db->Get(0, "startTimestamp");
			string  friendUserID = db->Get(0, "owner_id");

			if(friendUserID.length() && db->Query("select * from `users` where `id`='" + friendUserID + "';"))
			{
				string  friend_userName = db->Get(0, "name");
				string  friend_userNameLast = db->Get(0, "nameLast");

				ostResult << "\"notificationFriendUserID\":\"" << friendUserID << "\",";
				ostResult << "\"notificationFriendUserName\":\"" << friend_userName << "\",";
				ostResult << "\"notificationFriendUserNameLast\":\"" << friend_userNameLast << "\",";
				ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(friendUserID, db) << "\",";
				ostResult << "\"notificationEvent\":[" << GetEventListInJSONFormat("SELECT * FROM `events` WHERE `id`=\"" + event_id + "\";", db, NULL) << "]";
			}
			else
			{
				CLog log;
				log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from users[userID = " + friendUserID + "]");
			}
		}
		else
		{
			CLog log;
			log.Write(ERROR, string(__func__) + "[" + to_string(__LINE__) + "]:ERROR:typeID=" + to_string(typeID) + ": ERROR selecting from event[id = " + event_id + "]");
		}

	}

	if(typeID == NOTIFICATION_GENERAL_FROM_USER)
	{
		string	from_company_id = to_string(actionID);

		if(from_company_id.length())
		{
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + from_company_id + "\";", db, NULL) << "]";
		}
/*
		string	from_user_id = to_string(actionID);
		
		if(from_user_id.length() && db->Query("select * from `users` where `id`='" + from_user_id + "';"))
		{
			ostResult << "\"notificationFriendUserID\":\"" << from_user_id << "\",";
			ostResult << "\"notificationFriendUserName\":\"" << db->Get(0, "name") << "\",";
			ostResult << "\"notificationFriendUserNameLast\":\"" << db->Get(0, "nameLast") << "\",";
			ostResult << "\"notificationFriendUserAvatar\":\"" << GetUserAvatarByUserID(from_user_id, db) << "\"";
		}
*/
	}

	if(typeID == NOTIFICATION_GENERAL_FROM_COMPANY)
	{
		string	from_company_id = to_string(actionID);

		if(from_company_id.length())
		{
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + from_company_id + "\";", db, NULL) << "]";
		}
	}

	if(typeID == NOTIFICATION_CHANGE_CUSTOMER_NAME)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id` IN ("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
							"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\""
						")"
					")"
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardCustomersInJSONFormat("SELECT * FROM `timecard_customers` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by customer.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_CHANGE_PROJECT_NAME)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id` IN ("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\""
					")"
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardProjectsInJSONFormat("SELECT * FROM `timecard_projects` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by project.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_CHANGE_TASK_NAME)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + id + "\""
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardTasksInJSONFormat("SELECT * FROM `timecard_tasks` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by task.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_CHANGE_TASK_DURATION)
	{
		string		id = to_string(actionID);

		if(id.length() && db->Query(
				"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=("
					"SELECT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\""
				")"
		))
		{
			string	company_id = db->Get(0, "agency_company_id");

			ostResult << "\"item\":[" << GetTimecardTaskAssignmentInJSONFormat("SELECT * FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + company_id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": fail to define SoW by task.id(" + id + ")");
		}
	}

	if(typeID == NOTIFICATION_AGENGY_INITIATED_SOW)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			ostResult << "\"item\":[" << GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=("
																							"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\""
																						");", db, NULL) << "],";
			ostResult << "\"users\":[" << GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + user->GetID() + "\";", db, user) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": sow_id is empty");
		}
	}

	if(typeID == NOTIFICATION_SUBCONTRACTOR_SIGNED_SOW)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			ostResult << "\"item\":[" << GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=("
																							"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\""
																						");", db, NULL) << "],";
			ostResult << "\"users\":[" << GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + user->GetID() + "\";", db, user) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": sow_id is empty");
		}
	}

	if(typeID == NOTIFICATION_AGENGY_ABOUT_SUBC_REGISTRATION)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			ostResult << "\"item\":[],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + id + "\";", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": subcontractor company_id is empty");
		}
	}

	if(typeID == NOTIFICATION_AGENGY_GENERATED_AGREEMENTS)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			ostResult << "\"item\":[" << GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + id + "\";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=("
																							"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + id + "\""
																						");", db, NULL) << "],";
			ostResult << "\"users\":[" << GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + user->GetID() + "\";", db, user) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": sow_id is empty");
		}
	}



	{
		CLog	log;
		ostringstream   ost;

		ost.str("");
		ost << __func__ << "(typeID=" << typeID << ", actionID=" << actionID << ")[" << __LINE__ << "]: end (return strlen=" << ostResult.str().length() << ")";
		log.Write(DEBUG, ost.str());
	}

	return  ostResult.str();
}

string  GetUserNotificationInJSONFormat(string sqlRequest, CMysql *db, CUser *user)
{
	int			 affected;
	ostringstream   ostUserNotifications;

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: start");
	}

	ostUserNotifications.str("");
	ostUserNotifications << "[";

	affected = db->Query(sqlRequest);
	if(affected)
	{
		class DBUserNotification
		{
			public:
				string	  notificationID;
				string	  notificationStatus;
				string	  notification_title;
				string	  feed_eventTimestamp;
				string	  feed_actionId;
				string	  feed_actionTypeId;
				string	  action_types_title;
				string	  action_types_title_male;
				string	  action_types_title_female;
				string	  user_id;
				string	  user_name;
				string	  user_nameLast;
				string	  user_sex;
				string	  user_email;
				string	  action_category_title;
				string	  action_category_title_male;
				string	  action_category_title_female;
				string	  action_category_id;
		};

		vector<DBUserNotification>  dbResult;

		for(int i = 0; i < affected; ++i)
		{
			DBUserNotification	  tmpObj;

			tmpObj.notificationID = db->Get(i, "users_notification_id");
			tmpObj.notificationStatus = db->Get(i, "users_notification_notificationStatus");
			tmpObj.notification_title = db->Get(i, "users_notification_title");
			tmpObj.feed_actionTypeId = db->Get(i, "feed_actionTypeId");
			tmpObj.action_types_title = db->Get(i, "action_types_title");
			tmpObj.action_types_title_male = db->Get(i, "action_types_title_male");
			tmpObj.action_types_title_female = db->Get(i, "action_types_title_female");
			tmpObj.action_category_title = db->Get(i, "action_category_title");
			tmpObj.action_category_title_male = db->Get(i, "action_category_title_male");
			tmpObj.action_category_title_female = db->Get(i, "action_category_title_female");
			tmpObj.feed_eventTimestamp = db->Get(i, "feed_eventTimestamp");
			tmpObj.user_id = db->Get(i, "user_id");
			tmpObj.user_name = db->Get(i, "user_name");
			tmpObj.user_nameLast = db->Get(i, "user_nameLast");
			tmpObj.user_sex = db->Get(i, "user_sex");
			tmpObj.user_email = db->Get(i, "user_email");
			tmpObj.action_category_id = db->Get(i, "action_category_id");
			tmpObj.feed_actionId = db->Get(i, "feed_actionId");

			dbResult.push_back(tmpObj);
		}


		for(auto &it: dbResult)
		{
			string	  userNotificationEnrichment = "";

			if(ostUserNotifications.str().length() > 20) ostUserNotifications << ",";
			ostUserNotifications << "{";

			// --- common part
			ostUserNotifications << "\"notificationID\":\"" << it.notificationID << "\",";
			ostUserNotifications << "\"notificationTypeID\":\"" << it.feed_actionTypeId << "\",";
			ostUserNotifications << "\"notificationTypeTitle\":\"" << it.action_types_title << "\",";
			ostUserNotifications << "\"notificationTypeTitleMale\":\"" << it.action_types_title_male << "\",";
			ostUserNotifications << "\"notificationTypeTitleFemale\":\"" << it.action_types_title_female << "\",";
			ostUserNotifications << "\"notificationCategoryID\":\"" << it.action_category_id << "\",";
			ostUserNotifications << "\"notificationCategoryTitle\":\"" << it.action_category_title << "\",";
			ostUserNotifications << "\"notificationCategoryTitleMale\":\"" << it.action_category_title_male << "\",";
			ostUserNotifications << "\"notificationCategoryTitleFemale\":\"" << it.action_category_title_female << "\",";
			ostUserNotifications << "\"notificationEventTimestamp\":\"" << it.feed_eventTimestamp << "\",";
			ostUserNotifications << "\"notificationOwnerUserID\":\"" << it.user_id << "\",";
			ostUserNotifications << "\"notificationOwnerUserName\":\"" << it.user_name << "\",";
			ostUserNotifications << "\"notificationOwnerUserNameLast\":\"" << it.user_nameLast << "\",";
			ostUserNotifications << "\"notificationOwnerUserSex\":\"" << it.user_sex << "\",";
			ostUserNotifications << "\"notificationActionID\":\"" << it.feed_actionId << "\",";
			ostUserNotifications << "\"notificationTitle\":\"" << it.notification_title << "\",";
			ostUserNotifications << "\"notificationStatus\":\"" << it.notificationStatus << "\"";

			userNotificationEnrichment = GetUserNotificationSpecificDataByType(atol(it.feed_actionTypeId.c_str()), atol(it.feed_actionId.c_str()), db, user);
			if(userNotificationEnrichment.length()) ostUserNotifications << "," << userNotificationEnrichment;

			ostUserNotifications << "}";
		}
	}
	ostUserNotifications << "]";

	{
		CLog	log;
		log.Write(DEBUG, string(__func__) + "[" + to_string(__LINE__) +  "]: finish");
	}

	return ostUserNotifications.str();
}

// --- end of news feed and notifications

string CheckHTTPParam_Timeentry(const string &srcText)
{
	string			result = "";
	string			timeentry_string;
	vector<string>	timeentry_vec;

	MESSAGE_DEBUG("", "", "start param(" + srcText + ")");

	timeentry_string = CheckHTTPParam_Text(srcText);
	timeentry_vec = SplitTimeentry(timeentry_string);
	for(unsigned int i = 0; i < timeentry_vec.size(); ++i)
	{
		if(i) result += ",";
		result += (timeentry_vec.at(i) == "0" ? "" : timeentry_vec.at(i));
	}

	{
		MESSAGE_DEBUG("", "", "end ( result length = " + result + ")");
	}

	MESSAGE_DEBUG("", "", "finish param(" + result + ")");

	return	result;
}

// --- difference with "split" is that empty element treat as 0 instead of skip it
vector<string> SplitTimeentry(const string& s, const char c)
{

	string buff{""};
	vector<string> v;

	MESSAGE_DEBUG("", "", "start");

	for(auto n:s)
	{
		if(n == c)
		{
			c_float	num(buff);

			v.push_back(string(num));
			buff = "";
		}
		else
			buff+=n;
	}
	{
		c_float	num(buff);
		v.push_back(string(num));
	}

	MESSAGE_DEBUG("", "", "finish (result.size = " + to_string(v.size()) + ")");

	return v;
}

string	GetTimecardCustomersInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	title;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"title\":\"" + item.title + "\",";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_ERROR("", "", "customer not found (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardProjectsInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_customers_id;
		string	title;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_customers_id = db->Get(i, "timecard_customers_id");
			item.title = db->Get(i, "title");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"title\":\"" + item.title + "\",";
			result +=	"\"customers\":[" + GetTimecardCustomersInJSONFormat("SELECT * FROM `timecard_customers` WHERE `id`=\"" + item.timecard_customers_id + "\";", db, user) + "],";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_ERROR("", "", "project not found (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardTasksInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_projects_id;
		string	title;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_projects_id = db->Get(i, "timecard_projects_id");
			item.title = db->Get(i, "title");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"title\":\"" + item.title + "\",";
			result +=	"\"projects\":[" + GetTimecardProjectsInJSONFormat("SELECT * FROM `timecard_projects` WHERE `id`=\"" + item.timecard_projects_id + "\";", db, user) + "],";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no tasks returned by SQL-query(" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardTaskAssignmentInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_tasks_id;
		string	assignee_user_id;
		string	period_start;
		string	period_end;
		string	hour_limit;
		string	contract_sow_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_tasks_id = db->Get(i, "timecard_tasks_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.period_start = db->Get(i, "period_start");
			item.period_end = db->Get(i, "period_end");
			item.hour_limit = db->Get(i, "hour_limit");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"timecard_tasks_id\":\"" + item.timecard_tasks_id + "\",";
			result +=	"\"assignee_user\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.assignee_user_id + "\" AND `isblocked`=\"N\";", db, NULL) + "],";
			result +=	"\"period_start\":\"" + item.period_start + "\",";
			result +=	"\"period_end\":\"" + item.period_end + "\",";
			result +=	"\"hour_limit\":\"" + item.hour_limit + "\",";
			result +=	"\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "tasks are not assigned (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTExpenseAssignmentInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	auto	result = ""s;
	int		affected;

	struct ItemClass
	{
		string	id;
		string	sow_id;
		string	bt_expense_template_id;
		string	assignee_user_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_expense_template_id = db->Get(i, "bt_expense_template_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.sow_id = db->Get(i, "sow_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"bt_expense_template_id\":\"" + item.bt_expense_template_id + "\",";
			result +=	"\"assignee_user\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.assignee_user_id + "\" AND `isblocked`=\"N\";", db, NULL) + "],";
			result +=	"\"sow_id\":\"" + item.sow_id + "\",";
			result +=	"\"eventTimestamp\":\"" + item.eventTimestamp + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "bt_expenses are not assigned (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetTimecardLinesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected;

	struct ItemClass
	{
		string	id;
		string	timecard_task_id;
		string	row;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_task_id = db->Get(i, "timecard_task_id");
			item.row = db->Get(i, "row");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";
			result +=	"\"id\":\"" + item.id + "\",";
			result +=	"\"tasks\":[" + GetTimecardTasksInJSONFormat("SELECT * FROM `timecard_tasks` WHERE `id`=\"" + item.timecard_task_id + "\";", db, user) + "],";
			result +=	"\"row\":\"" + item.row + "\"";
			result += 	"}";
		}

	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "timecard entry not found (" + sqlQuery + ")");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
/*
string	GetSOWTimecardApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	type;
		string	rate;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.approver_user_id = db->Get(i, "approver_user_id");
			item.type = db->Get(i, "type");
			item.rate = db->Get(i, "rate");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"approver_user_id\":\"" + item.approver_user_id + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"rate\":\"" + item.rate + "\",";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.approver_user_id + "\";", db, user) + "]";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
*/
string	GetTimecardApprovalsInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	timecard_id;
		string	approver_id;
		string	decision;
		string	comment;
		string	eventTimestamp;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.timecard_id = db->Get(i, "timecard_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"timecard_id\":\"" + item.timecard_id + "\",";
			result += "\"approver_id\":\"" + item.approver_id + "\",";
			result += "\"decision\":\"" + item.decision + "\",";
			result += "\"comment\":\"" + item.comment + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTApprovalsInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	bt_id;
		string	approver_id;
		string	decision;
		string	comment;
		string	eventTimestamp;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_id = db->Get(i, "bt_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"bt_id\":\"" + item.bt_id + "\",";
			result += "\"approver_id\":\"" + item.approver_id + "\",";
			result += "\"decision\":\"" + item.decision + "\",";
			result += "\"comment\":\"" + item.comment + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") there are no bt approvals");
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
/*
string	GetTimecardApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	contract_sow_id;
		string	auto_approve;
		string	type;
		string	rate;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.approver_user_id = db->Get(i, "approver_user_id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.auto_approve = db->Get(i, "auto_approve");
			item.type = db->Get(i, "type");
			item.rate = db->Get(i, "rate");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"approver_user_id\":\"" + item.approver_user_id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user) + "],";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.approver_user_id + "\";", db, user) + "],";
			result += "\"auto_approve\":\"" + item.auto_approve + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"rate\":\"" + item.rate + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "approvers list is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}
*/
string	GetApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool include_sow)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	contract_sow_id;
		string	auto_approve;
		string	type;
		string	rate;
	};

	int						affected;
	string					result;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.approver_user_id = db->Get(i, "approver_user_id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.auto_approve = db->Get(i, "auto_approve");
			item.type = db->Get(i, "type");
			item.rate = db->Get(i, "rate");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"approver_user_id\":\"" + item.approver_user_id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			if(include_sow)
				result += "\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user) + "],";
			result += "\"users\":[" + GetUserListInJSONFormat("SELECT * FROM `users` WHERE `id`=\"" + item.approver_user_id + "\";", db, user) + "],";
			result += "\"auto_approve\":\"" + item.auto_approve + "\",";
			result += "\"type\":\"" + item.type + "\",";
			result += "\"rate\":\"" + item.rate + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "approvers list is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetCostCentersAssignmentInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	cost_center_id;
		string	timecard_customer_id;
		string	assignee_user_id;
		string	eventTimestamp;
	};

	auto					affected = 0;
	auto					result = ""s;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(auto i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.cost_center_id = db->Get(i, "cost_center_id");
			item.timecard_customer_id = db->Get(i, "timecard_customer_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"cost_center_id\":\"" + item.cost_center_id + "\",";
			result += "\"timecard_customer_id\":\"" + item.timecard_customer_id + "\",";
			result += "\"assignee_user_id\":\"" + item.assignee_user_id + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "cost_center list is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetCostCentersInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	struct ItemClass
	{
		string	id;
		string	title;
		string	description;
		string	company_id;
		string	start_date;
		string	end_date;
		string	number;
		string	sign_date;
		string	act_number;
		string	agency_company_id;
		string	assignee_user_id;
		string	eventTimestamp;
	};

	auto					affected = 0;
	auto					result = ""s;
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(auto i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.company_id = db->Get(i, "company_id");
			item.start_date = db->Get(i, "start_date");
			item.end_date = db->Get(i, "end_date");
			item.number = db->Get(i, "number");
			item.sign_date = db->Get(i, "sign_date");
			item.act_number = db->Get(i, "act_number");
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.assignee_user_id = db->Get(i, "assignee_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"description\":\"" + item.description + "\",";
			result += "\"companies\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + item.company_id + "\";", db, user) + "],";
			result += "\"start_date\":\"" + item.start_date + "\",";
			result += "\"end_date\":\"" + item.end_date + "\",";
			result += "\"number\":\"" + item.number + "\",";
			result += "\"sign_date\":\"" + item.sign_date + "\",";
			result += "\"act_number\":\"" + item.act_number + "\",";
			result += "\"agency_company_id\":\"" + item.agency_company_id + "\",";
			result += "\"assignee_user_id\":\"" + item.assignee_user_id + "\",";
			result += "\"custom_fields\":[" + GetCostCenterCustomFieldsInJSONFormat("SELECT * FROM `cost_center_custom_fields` WHERE `cost_center_id`=\"" + item.id + "\" "
						+ (user->GetType() == "subcontractor" ? " AND (`visible_by_subcontractor`=\"Y\" OR `editable_by_subcontractor`=\"Y\") " : "")
						+ ";", db, user)
						+ "],";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "cost_center list is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string	GetTimecardsInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool isExtended)
{
	int		affected;
	string	result;

	struct ItemClass
	{
		string				id;
		string				contract_sow_id;
		string				period_start;
		string				period_end;
		string				status;
		string				submit_date;
		string				approve_date;
		string				eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.period_start = db->Get(i, "period_start");
			item.period_end = db->Get(i, "period_end");
			item.submit_date = db->Get(i, "submit_date");
			item.approve_date = db->Get(i, "approve_date");
			item.status = db->Get(i, "status");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"contract_sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user, false, false, false) + "],";
			result += "\"period_start\":\"" + item.period_start + "\",";
			result += "\"period_end\":\"" + item.period_end + "\",";
			result += "\"submit_date\":\"" + item.submit_date + "\",";
			result += "\"approve_date\":\"" + item.approve_date + "\",";
			result += "\"status\":\"" + item.status + "\",";
			result += "\"lines\":[" + GetTimecardLinesInJSONFormat("SELECT * FROM `timecard_lines` WHERE `timecard_id`=\"" + item.id + "\";", db, user) + "],";
			if(isExtended)
			{
				// result += "\"approvers\":[" + GetSOWTimecardApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.contract_sow_id + "\";", db, user) + "],";
				result += "\"approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.contract_sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
				result += "\"approvals\":[" + GetTimecardApprovalsInJSONFormat("SELECT * FROM `timecard_approvals` WHERE `timecard_id`=\"" + item.id + "\";", db, user) + "],";
			}
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTsInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool isExtended)
{
	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	contract_sow_id;
		string	date_start;
		string	date_end;
		string	customer_id;
		string	place;
		string	purpose;
		string	status;
		string	submit_date;
		string	approve_date;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.date_start = db->Get(i, "date_start");
			item.date_end = db->Get(i, "date_end");
			item.customer_id = db->Get(i, "customer_id");
			item.place = db->Get(i, "place");
			item.purpose = db->Get(i, "purpose");
			item.status = db->Get(i, "status");
			item.submit_date = db->Get(i, "submit_date");
			item.approve_date = db->Get(i, "approve_date");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"date_start\":\"" + item.date_start + "\",";
			result += "\"date_end\":\"" + item.date_end + "\",";
			result += "\"customer_id\":\"" + item.customer_id + "\",";
			result += "\"place\":\"" + item.place + "\",";
			result += "\"purpose\":\"" + item.purpose + "\",";
			result += "\"status\":\"" + item.status + "\",";
			result += "\"submit_date\":\"" + item.submit_date + "\",";
			result += "\"approve_date\":\"" + item.approve_date + "\",";
			if(isExtended)
			{
				result += "\"customers\":[" + GetCustomersInJSONFormat("SELECT * FROM `timecard_customers` WHERE `id`=\"" + item.customer_id + "\";", db, user) + "],";
				result += "\"sow\":[" + GetSOWInJSONFormat("SELECT * FROM `contracts_sow` WHERE `id`=\"" + item.contract_sow_id + "\";", db, user, INCLUDE_TIMECARD_TASKS, INCLUDE_BT_EXPENSE_TEMPLATES) + "],";
				result += "\"expenses\":[" + GetBTExpensesInJSONFormat("SELECT * FROM `bt_expenses` WHERE `bt_id`=\"" + item.id + "\";", db, user) + "],";
				result += "\"approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + item.contract_sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
				result += "\"approvals\":[" + GetBTApprovalsInJSONFormat("SELECT * FROM `bt_approvals` WHERE `bt_id`=\"" + item.id + "\";", db, user) + "],";
			}
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetCustomersInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	int		affected;
	string	result;


	struct ItemClass
	{
		string	id;
		string	title;
		string	agency_company_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";


			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"agency_company_id\":\"" + item.agency_company_id + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard_customer is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}


auto	GetPSoWInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	string	result;

	struct ItemClass
	{
		string	id;
		string	contract_sow_id;
		string	cost_center_id;
		string	company_position_id;
		string	start_date;
		string	end_date;
		string	number;
		string	day_rate;
		string	bt_markup;
		string	bt_markup_type;
		string	act_number;
		string	sign_date;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.cost_center_id = db->Get(i, "cost_center_id");
			item.company_position_id = db->Get(i, "company_position_id");
			item.start_date = db->Get(i, "start_date");
			item.end_date = db->Get(i, "end_date");
			item.number = db->Get(i, "number");
			item.day_rate = db->Get(i, "day_rate");
			item.bt_markup = db->Get(i, "bt_markup");
			item.bt_markup_type = db->Get(i, "bt_markup_type");
			item.act_number = db->Get(i, "act_number");
			item.sign_date = db->Get(i, "sign_date");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"contract_sow_id\":\"" + item.contract_sow_id + "\",";
			result += "\"cost_center_id\":\"" + item.cost_center_id + "\",";
			result += "\"company_position_id\":\"" + item.company_position_id + "\",";
			result += "\"company_positions\":[" + GetCompanyPositionsInJSONFormat("SELECT * FROM `company_position` WHERE `id`=\"" + item.company_position_id + "\";", db, user) + "],";
			result += "\"start_date\":\"" + item.start_date + "\",";
			result += "\"end_date\":\"" + item.end_date + "\",";
			result += "\"number\":\"" + item.number + "\",";
			result += "\"day_rate\":\"" + item.day_rate + "\",";
			result += "\"bt_markup\":\"" + item.bt_markup + "\",";
			result += "\"bt_markup_type\":\"" + item.bt_markup_type + "\",";
			result += "\"act_number\":\"" + item.act_number + "\",";
			result += "\"sign_date\":\"" + item.sign_date + "\",";
			result += "\"custom_fields\":[" + GetPSoWCustomFieldsInJSONFormat("SELECT * FROM `contract_psow_custom_fields` WHERE `contract_psow_id`=\"" + item.id + "\" "
						+ (user->GetType() == "subcontractor" ? " AND (`visible_by_subcontractor`=\"Y\" OR `editable_by_subcontractor`=\"Y\") " : "")
						+ ";", db, user)
						+ "],";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

auto	GetSOWInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool include_tasks, bool include_bt, bool include_cost_centers) -> string
{
	auto	affected = 0;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	subcontractor_company_id;
		string	agency_company_id;
		string	company_position_id;
		string	start_date;
		string	end_date;
		string	number;
		string	sign_date;
		string	timecard_period;
		string	subcontractor_create_tasks;
		string	day_rate;
		string	act_number;
		string	status;
		string	agreement_filename;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start (include_bt = " + to_string(include_bt) + ", include_tasks = " + to_string(include_tasks) + ")");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.subcontractor_company_id = db->Get(i, "subcontractor_company_id");
			item.agency_company_id = db->Get(i, "agency_company_id");
			item.company_position_id = db->Get(i, "company_position_id");
			item.start_date = db->Get(i, "start_date");
			item.end_date = db->Get(i, "end_date");
			item.number = db->Get(i, "number");
			item.sign_date = db->Get(i, "sign_date");
			item.timecard_period = db->Get(i, "timecard_period");
			item.subcontractor_create_tasks = db->Get(i, "subcontractor_create_tasks");
			item.act_number = db->Get(i, "act_number");
			item.status = db->Get(i, "status");
			item.agreement_filename = db->Get(i, "agreement_filename");
			item.day_rate = db->Get(i, "day_rate");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"subcontractor_company_id\":\"" + item.subcontractor_company_id + "\",";
			result += "\"agency_company_id\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `isBlocked`='N' AND `id`=\"" + item.agency_company_id + "\";", db, user) + "],";
			result += "\"company_position_id\":\"" + item.company_position_id + "\",";
			result += "\"company_positions\":[" + GetCompanyPositionsInJSONFormat("SELECT * FROM `company_position` WHERE `id`=\"" + item.company_position_id + "\";", db, user) + "],";
			result += "\"start_date\":\"" + item.start_date + "\",";
			result += "\"end_date\":\"" + item.end_date + "\",";
			result += "\"number\":\"" + item.number + "\",";
			result += "\"sign_date\":\"" + item.sign_date + "\",";
			result += "\"timecard_period\":\"" + item.timecard_period + "\",";

			// --- either BT or timecard tasks (could be changed in future)
			// --- configured to keep MySQL DB load low
			if(include_bt)
			{
				result += "\"bt_expense_templates\":[" + GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `id` IN (SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `sow_id`=\"" + item.id + "\");", db, user) + "],";
			}
			if(include_tasks)
			{
				result += "\"tasks\":[" + GetTimecardTasksInJSONFormat(
					"SELECT * FROM `timecard_tasks` WHERE "
							"`id` IN (SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + item.id + "\" "
									");", db, user) + "],";
			}
			result += "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
			result += "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
			result += "\"subcontractor_create_tasks\":\"" + item.subcontractor_create_tasks + "\",";
			result += "\"act_number\":\"" + item.act_number + "\",";
			result += "\"day_rate\":\"" + (user->GetType() == "agency" || user->GetType() == "subcontractor" ? item.day_rate : "") + "\",";
			result += "\"status\":\"" + item.status + "\",";
			result += "\"agreement_filename\":\"" + item.agreement_filename + "\",";
			result += "\"custom_fields\":[" + GetSoWCustomFieldsInJSONFormat("SELECT * FROM `contract_sow_custom_fields` WHERE `contract_sow_id`=\"" + item.id + "\" "
						+ (user->GetType() == "subcontractor" ? " AND (`visible_by_subcontractor`=\"Y\" OR `editable_by_subcontractor`=\"Y\") " : "")
						+ ";", db, user)
						+ "],";
			result += "\"cost_centers\":[" + (include_cost_centers ? GetCostCentersInJSONFormat(
																		"SELECT * FROM `cost_centers` WHERE `id` IN ("
																			"SELECT `cost_center_id` FROM `cost_center_assignment` WHERE `timecard_customer_id` IN ("
																				"SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id` IN ("
																					"SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id` IN ("
																						"SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + item.id + "\""
																					")"
																				")"
																			")"
																		")", db, user) : "") + "],";
			result += "\"psow\":[" + (include_cost_centers ? GetPSoWInJSONFormat("SELECT * FROM `contracts_psow` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user) : "") + "],";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

auto	GetSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	contract_sow_id;
		string	title;
		string	description;
		string	type;
		string	var_name;
		string	value;
		string	visible_by_subcontractor;
		string	editable_by_subcontractor;
		string	owner_user_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.type = db->Get(i, "type");
			item.var_name = db->Get(i, "var_name");
			item.value = db->Get(i, "value");
			item.visible_by_subcontractor = db->Get(i, "visible_by_subcontractor");
			item.editable_by_subcontractor = db->Get(i, "editable_by_subcontractor");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""							+ item.id + "\",";
			result += "\"contract_sow_id\":\""				+ item.contract_sow_id + "\",";
			result += "\"title\":\""						+ item.title + "\",";
			result += "\"description\":\""					+ item.description + "\",";
			result += "\"type\":\""							+ item.type + "\",";
			result += "\"var_name\":\""						+ item.var_name + "\",";
			result += "\"value\":\""						+ item.value + "\",";
			result += "\"visible_by_subcontractor\":\""		+ item.visible_by_subcontractor + "\",";
			result += "\"editable_by_subcontractor\":\""	+ item.editable_by_subcontractor + "\",";
			result += "\"owner_user_id\":\""				+ item.owner_user_id + "\",";
			result += "\"eventTimestamp\":\""				+ item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt items assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

auto	GetPSoWCustomFieldsInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	contract_psow_id;
		string	title;
		string	description;
		string	type;
		string	var_name;
		string	value;
		string	visible_by_subcontractor;
		string	editable_by_subcontractor;
		string	owner_user_id;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.contract_psow_id = db->Get(i, "contract_psow_id");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.type = db->Get(i, "type");
			item.var_name = db->Get(i, "var_name");
			item.value = db->Get(i, "value");
			item.visible_by_subcontractor = db->Get(i, "visible_by_subcontractor");
			item.editable_by_subcontractor = db->Get(i, "editable_by_subcontractor");
			item.owner_user_id = db->Get(i, "owner_user_id");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""							+ item.id + "\",";
			result += "\"contract_psow_id\":\""				+ item.contract_psow_id + "\",";
			result += "\"title\":\""						+ item.title + "\",";
			result += "\"description\":\""					+ item.description + "\",";
			result += "\"type\":\""							+ item.type + "\",";
			result += "\"var_name\":\""						+ item.var_name + "\",";
			result += "\"value\":\""						+ item.value + "\",";
			result += "\"visible_by_subcontractor\":\""		+ item.visible_by_subcontractor + "\",";
			result += "\"editable_by_subcontractor\":\""	+ item.editable_by_subcontractor + "\",";
			result += "\"owner_user_id\":\""				+ item.owner_user_id + "\",";
			result += "\"eventTimestamp\":\""				+ item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt items assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

auto	GetBTExpenseTemplatesInJSONFormat(string sqlQuery, CMysql *db, CUser *user) -> string
{
	int		affected;
	auto	result = ""s;
	struct ItemClass
	{
		string	id;
		string	title;
		string	taxable;
		string	agency_comment;
		string	is_default;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.title = db->Get(i, "title");
			item.taxable = db->Get(i, "taxable");
			item.agency_comment = db->Get(i, "agency_comment");
			item.is_default = db->Get(i, "is_default");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"taxable\":\"" + item.taxable + "\",";
			result += "\"agency_comment\":\"" + item.agency_comment + "\",";
			result += "\"line_templates\":[" + GetBTExpenseLineTemplatesInJSONFormat("SELECT * FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=\"" + item.id + "\";", db, user) + "],";
			result += "\"is_default\":\"" + item.is_default + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt items assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTExpenseLineTemplatesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	int		affected;
	string	result;
	struct ItemClass
	{
		string	id;
		string	bt_expense_template_id;
		string	dom_type;
		string	title;
		string	description;
		string	tooltip;
		string	payment;
		string	required;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_expense_template_id = db->Get(i, "bt_expense_template_id");
			item.dom_type = db->Get(i, "dom_type");
			item.title = db->Get(i, "title");
			item.description = db->Get(i, "description");
			item.tooltip = db->Get(i, "tooltip");
			item.payment = db->Get(i, "payment");
			item.required = db->Get(i, "required");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"bt_expense_template_id\":\"" + item.bt_expense_template_id + "\",";
			result += "\"dom_type\":\"" + item.dom_type + "\",";
			result += "\"title\":\"" + item.title + "\",";
			result += "\"description\":\"" + item.description + "\",";
			result += "\"tooltip\":\"" + item.tooltip + "\",";
			result += "\"payment\":\"" + item.payment + "\",";
			result += "\"required\":\"" + item.required + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt line templates assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string	GetBTExpenseLinesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	int		affected;
	string	result;
	struct ItemClass
	{
		string	id;
		string	bt_expense_id;
		string	bt_expense_line_template_id;
		string	value;
		string	comment;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.bt_expense_id = db->Get(i, "bt_expense_id");
			item.bt_expense_line_template_id = db->Get(i, "bt_expense_line_template_id");
			item.value = db->Get(i, "value");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"bt_expense_id\":\"" + item.bt_expense_id + "\",";
			result += "\"bt_expense_line_template_id\":\"" + item.bt_expense_line_template_id + "\",";
			result += "\"value\":\"" + item.value + "\",";
			result += "\"comment\":\"" + item.comment + "\",";
			result += "\"eventTimestamp\":\"" + item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt lines assigned");
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish");
	}

	return result;
}

string SummarizeTimereports(string timereport1, string timereport2)
{
	string			result = "";
	vector<string>	timereport1_vector, timereport2_vector, sum;

	MESSAGE_DEBUG("", "", "start");

	timereport1_vector = SplitTimeentry(timereport1, ',');
	timereport2_vector = SplitTimeentry(timereport2, ',');

	if(timereport1_vector.size() == timereport2_vector.size())
	{
		for(unsigned int i = 0; i < timereport1_vector.size(); ++i)
		{
			c_float	term1 = { timereport1_vector.at(i) };
			c_float	term2 = { timereport2_vector.at(i) };
			c_float	sum_result = term1 + term2;

			sum.push_back(sum_result.Get() ? string(sum_result) : "");
		}

		for(unsigned int i = 0; i < sum.size(); ++i)
		{
			if(i) result += ",";
			result += sum.at(i);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "can't summarize: " + timereport1 + ", " + timereport2 + " had different lengths " + to_string(timereport1_vector.size()) + "/" + to_string(timereport2_vector.size()) + " ");
	}

	MESSAGE_DEBUG("", "", "finish (" + timereport1 + " + " + timereport2 + " = " + result + ")");

	return	result;
}

bool isTimecardEntryEmpty(string timereports)
{
	bool	result = true;
	vector<string>	timereport_vector;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	timereport_vector = SplitTimeentry(timereports, ',');
	for(auto &item: timereport_vector)
	{
		c_float	digit(item);

		if(digit.Get())
		{
			result = false;
			break;
		}
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");
	}

	return	result;
}

bool isUserAssignedToSoW(string user_id, string sow_id, CMysql *db)
{
	bool 	result = false;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(user_id.length())
	{
		if(sow_id.length())
		{
			if(db->Query("SELECT `admin_userID` FROM `company` WHERE "
									"`type`=\"subcontractor\" "
									"AND "
									"(`id` IN (SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\"))"
				))
			{
				string sow_user_id = db->Get(0, "admin_userID");

				if(user_id == sow_user_id) result = true;
				else
				{
					MESSAGE_DEBUG("", "", "**user_id (" + user_id + ") doesn't assigned to SoW(" + sow_id + ")");
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "*user_id (" + user_id + ") doesn't assigned to SoW(" + sow_id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "sow_id is empty")
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user_id is empty")
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");
	}

	return result;
}

bool isSoWAllowedToCreateTask(string sow_id, CMysql *db)
{
	bool 	result = false;

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(sow_id.length())
	{
		if(db->Query("SELECT `subcontractor_create_tasks` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
		{
			string task_change_flag = db->Get(0, "subcontractor_create_tasks");

			if(task_change_flag == "Y") result = true;
		}
		else
		{
			MESSAGE_DEBUG("", "", "SoW(" + sow_id + ") not found");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty")
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");
	}

	return result;
}

string GetTaskIDFromAgency(string customer, string project, string task, string agency_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(agency_id.length())
				{
					if(db->Query(
							"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + task + "\" AND `timecard_projects_id` IN ("
							    "SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
							        "SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\" AND `agency_company_id`=\"" + agency_id + "\""
							    ")"
							");"
					))
					{
						result = db->Get(0, "id");
					}
					else
					{
						MESSAGE_DEBUG("", "", "triplet customer/project/task doesn't assigned to agency.id(" + agency_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "agency_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string GetTaskIDFromSOW(string customer, string project, string task, string sow_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(sow_id.length())
				{
					if(db->Query(
							"SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN ("
								"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + task + "\" AND `timecard_projects_id` IN ("
									"SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
										"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\""
									")"
								")"
							") LIMIT 0,1;"
					))
					{
						result = db->Get(0, "timecard_tasks_id");
					}
					else
					{
						MESSAGE_DEBUG("", "", "triplet customer/project/task doesn't assigned to sow.id(" + sow_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "sow_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

auto GetBTExpenseTemplateAssignmentToSoW(string bt_expense_template_id, string sow_id, CMysql *db) -> string
{
	auto 	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(bt_expense_template_id.length())
	{
		if(sow_id.length())
		{
			if(db->Query(
				"SELECT `id` FROM `bt_sow_assignment` WHERE `sow_id`=\"" + sow_id + "\" AND `bt_expense_template_id`=\"" + bt_expense_template_id + "\";"
			))
			{
				result = db->Get(0, "id");
			}
			else
			{
				MESSAGE_DEBUG("", "", "bt_expense_template_id(" + bt_expense_template_id + ") doesn't assigned to sow_id(" + sow_id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "sow_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_template_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetTaskAssignmentID(string customer, string project, string task, string sow_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(sow_id.length())
				{
					if(db->Query(
							"SELECT `id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `timecard_tasks_id` IN ("
								"SELECT `id` FROM `timecard_tasks` WHERE `title`=\"" + task + "\" AND `timecard_projects_id` IN ("
									"SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
										"SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\""
									")"
								")"
							") LIMIT 0,1;"
					))
					{
						result = db->Get(0, "id");
					}
					else
					{
						MESSAGE_DEBUG("", "", "triplet customer/project/task doesn't assigned to sow_id(" + sow_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "sow_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string CreateTaskBelongsToAgency(string customer, string project, string task, string agency_id, CMysql *db)
{
	string	customer_id = "";
	string	project_id = "";
	string	task_id = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(agency_id.length())
				{
					int affected = db->Query(
						"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + agency_id + "\";"
					);
					if(affected)
					{
						string	sow_list = "";

						for(int i = 0; i < affected; ++i)
						{
							if(i) sow_list += ",";
							sow_list += db->Get(i, "id");
						}

						// --- try to define project_id from customers and projects belong to SoW
						if(db->Query(
							    "SELECT `id` FROM `timecard_projects` WHERE `title`=\"" + project + "\" AND `timecard_customers_id` IN ("
							        "SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\" AND `agency_company_id`=\"" + agency_id + "\""
							    ")"
						))
						{
							project_id = db->Get(0, "id");
							customer_id = "fake number";
						}
						else
						{
							// --- try to define customer_id from customers belong to SoW
							if(db->Query(
						        "SELECT `id` FROM `timecard_customers` WHERE `title`=\"" + customer + "\" AND `agency_company_id`=\"" + agency_id + "\""
							))
							{
								customer_id = db->Get(0, "id");
							}
							else
							{
								MESSAGE_DEBUG("", "", "neither customer nor project exists in agency portfolio");
							}
						}

						if(customer_id.empty())
						{
							long int	temp = db->InsertQuery("INSERT INTO `timecard_customers` (`agency_company_id`,`title`,`eventTimestamp`) VALUES (\"" + agency_id + "\", \"" + customer + "\", UNIX_TIMESTAMP());");

							if(temp) { customer_id = to_string(temp); }
							else
							{
								MESSAGE_ERROR("", "", "fail to insert to db");
							}
						}

						if(customer_id.length() && project_id.empty())
						{
							long int	temp = db->InsertQuery("INSERT INTO `timecard_projects` (`timecard_customers_id`,`title`,`eventTimestamp`) VALUES (\"" + customer_id + "\",\"" + project + "\", UNIX_TIMESTAMP());");

							if(temp) { project_id = to_string(temp); }
							else
							{
								MESSAGE_ERROR("", "", "fail to insert to db");
							}
						}

						if(customer_id.length() && project_id.length() && task_id.empty())
						{
							long int	temp = db->InsertQuery("INSERT INTO `timecard_tasks` (`timecard_projects_id`,`title`,`eventTimestamp`) VALUES (\"" + project_id + "\",\"" + task + "\", UNIX_TIMESTAMP());");

							if(temp) { task_id = to_string(temp); }
							else
							{
								MESSAGE_ERROR("", "", "fail to insert to db");
							}
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "fail to define all sow's signed with the same agency.id(" + agency_id + ")");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "agency_id is empty");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "project is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "customer is empty");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + task_id + ")");
	}

	return task_id;
}

string CreateTaskAssignment(string task_id, string sow_id, string assignment_start, string assignment_end, CMysql *db, CUser *user)
{
	string	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(task_id.length() && sow_id.length() && assignment_start.length() && assignment_end.length() && db)
	{
		long int	assignment_id = db->InsertQuery("INSERT INTO `timecard_task_assignment` SET "
													"`timecard_tasks_id`=\"" + task_id + "\","
													"`assignee_user_id`=\"" + user->GetID() + "\","
													"`period_start`=\"" + assignment_start + "\","
													"`period_end`=\"" + assignment_end + "\","
													"`hour_limit`=\"" + "0" + "\","
													"`contract_sow_id`=\"" + sow_id + "\","
													"`eventTimestamp`=UNIX_TIMESTAMP()"
													";");

		if(assignment_id)
		{
			result = to_string(assignment_id);
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to insert to db");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "one of mandatory parameters missed");
	}

	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

auto CreateBTExpenseTemplateAssignmentToSoW(string new_bt_expense_template_id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(new_bt_expense_template_id.length() && sow_id.length() && db)
	{
		long int	assignment_id = db->InsertQuery("INSERT INTO `bt_sow_assignment` SET "
													"`bt_expense_template_id`=\"" + new_bt_expense_template_id + "\","
													"`sow_id`=\"" + sow_id + "\","
													"`assignee_user_id`=\"" + user->GetID() + "\","
													"`eventTimestamp`=UNIX_TIMESTAMP()"
													";");

		if(assignment_id)
		{
			result = to_string(assignment_id);
		}
		else
		{
			MESSAGE_ERROR("", "", "fail to insert to db");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "one of mandatory parameters missed");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetTimecardID(string sow_id, string period_start, string period_end, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(period_start.length() && period_end.length())
	{
		if(sow_id.length())
		{
			if(db->Query("SELECT `id` FROM `timecards` WHERE `contract_sow_id`=\"" + sow_id + "\" AND `period_start`=\"" + period_start + "\" AND `period_end`=\"" + period_end + "\";"))
			{
				result = db->Get(0, "id");
			}
			else
			{
				long int temp;

				MESSAGE_DEBUG("", "", "timecard(sow_id/period_start/period_end = " + sow_id + "/" + period_start + "/" + period_end + ") not found, create new one");

				temp = db->InsertQuery("INSERT INTO `timecards` SET "
												"`contract_sow_id`=\"" + sow_id + "\","
												"`period_start`=\"" + period_start + "\","
												"`period_end`=\"" + period_end + "\","
												"`status`=\"saved\","
												"`eventTimestamp`=UNIX_TIMESTAMP();");

				if(temp) result = to_string(temp);
				else MESSAGE_ERROR("", "", "DB issue: insert request failed");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "sow_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "period not defined (start:" + period_end + " end:" + period_end + ")");
	}


	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string GetTimecardStatus(string timecard_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(timecard_id.length())
	{
			if(db->Query("SELECT `status` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
			{
				result = db->Get(0, "status");
			}
			else
			{
				MESSAGE_ERROR("", "", "timecard_id(" + timecard_id + ") not fount in DB");
			}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_id is empty");
	}


	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}

string GetAgencyID(string sow_id, CMysql *db)
{
	string 	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(sow_id.length())
	{
			if(db->Query("SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
			{
				result = db->Get(0, "agency_company_id");
			}
			else
			{
				MESSAGE_ERROR("", "", "sow_id(" + sow_id + ") not fount in DB");
			}
	}
	else
	{
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

string GetAgencyID(CUser *user, CMysql *db)
{
	string 	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(user && db)
	{
		if(user->GetType() == "agency")
		{
				if(db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";"))
				{
					result = db->Get(0, "company_id");
				}
				else
				{
					MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") isn't an agency employee");
				}
		}
		else
		{
			MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") isn't an agency employee");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user doesn't initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + result + ")");

	return result;
}

auto	GetDBValueByAction(string action, string id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				string		sql_query = "";

				if(action == "AJAX_updateCustomerTitle") 			sql_query = "SELECT `title`				as `value` FROM `timecard_customers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateProjectTitle") 			sql_query = "SELECT `title`				as `value` FROM `timecard_projects` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateTaskTitle") 				sql_query = "SELECT `title`				as `value` FROM `timecard_tasks` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodStart") 				sql_query = "SELECT `period_start`		as `value` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updatePeriodEnd") 				sql_query = "SELECT `period_end`		as `value` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTitle")				sql_query = "SELECT `name`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyDescription")		sql_query = "SELECT `description`		as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyWebSite")			sql_query = "SELECT `webSite`			as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyTIN")				sql_query = "SELECT `tin`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyVAT")				sql_query = "SELECT `vat`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyAccount")			sql_query = "SELECT `account`			as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyOGRN")				sql_query = "SELECT `ogrn`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyKPP")				sql_query = "SELECT `kpp`				as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyLegalAddress")		sql_query = "SELECT `legal_address`		as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyMailingAddress")	sql_query = "SELECT `mailing_address`	as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyMailingZipID")		sql_query = "SELECT `mailing_geo_zip_id`as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyLegalZipID")		sql_query = "SELECT `legal_geo_zip_id`	as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCompanyBankID")			sql_query = "SELECT `bank_id`			as `value` FROM `company` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateAgencyPosition")			sql_query = "SELECT `title`				as `value` FROM `company_position` WHERE `id`=(SELECT `position_id` FROM `company_employees` WHERE `id`=\"" + id + "\");";
				if(action == "AJAX_updateSoWPosition")				sql_query = "SELECT `title`				as `value` FROM `company_position` WHERE `id`=(SELECT `company_position_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\");";
				if(action == "AJAX_updatePSoWPosition")				sql_query = "SELECT `title`				as `value` FROM `company_position` WHERE `id`=(SELECT `company_position_id` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\");";
				if(action == "AJAX_updateAgencyEditCapability")		sql_query = "SELECT `allowed_change_agency_data`	as `value` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSoWEditCapability")		sql_query = "SELECT `allowed_change_sow`			as `value` FROM `company_employees` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateSubcontractorCreateTasks")	sql_query = "SELECT `subcontractor_create_tasks`	as `value` FROM `contracts_sow` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateTitle")		sql_query = "SELECT `title`							as `value` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateTaxable")	sql_query = "SELECT `taxable`						as `value` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateAgencyComment") 	sql_query = "SELECT `agency_comment`		as `value` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "SELECT `title`					as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "SELECT `description`			as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "SELECT `tooltip`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "SELECT `dom_type`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCash")	sql_query = "SELECT `payment`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLinePaymentCard")	sql_query = "SELECT `payment`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "SELECT `required`				as `value` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterToCustomer")				sql_query = "SELECT `cost_center_id`		as `value` FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";";
				if(action == "AJAX_deleteCostCenterFromCustomer")			sql_query = "SELECT `cost_center_id`		as `value` FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";";
				if(action == "AJAX_deleteCostCenter")						sql_query = "SELECT `id`					as `value` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterTitle")					sql_query = "SELECT `title`					as `value` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
				if(action == "AJAX_updateCostCenterDescription")			sql_query = "SELECT `description`			as `value` FROM `cost_centers` WHERE `id`=\"" + id + "\";";
		
				if(action == "AJAX_updateSoWNumber")						sql_query = "SELECT `number`				as `value` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWAct")							sql_query = "SELECT `act_number`			as `value` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWDayRate")						sql_query = "SELECT `day_rate`				as `value` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWSignDate")						sql_query = "SELECT `sign_date`				as `value` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWStartDate")						sql_query = "SELECT `start_date`			as `value` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWEndDate")						sql_query = "SELECT `end_date`				as `value` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateSoWCustomField")					sql_query = "SELECT `value`					as `value` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updatePSoWNumber")						sql_query = "SELECT `number`				as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWAct")							sql_query = "SELECT `act_number`			as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWDayRate")						sql_query = "SELECT `day_rate`				as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWBTMarkup")						sql_query = "SELECT `bt_markup`				as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWBTMarkupType")					sql_query = "SELECT `bt_markup_type`		as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWSignDate")						sql_query = "SELECT `sign_date`				as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWStartDate")					sql_query = "SELECT `start_date`			as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWEndDate")						sql_query = "SELECT `end_date`				as `value` FROM `contracts_psow` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updatePSoWCustomField")					sql_query = "SELECT `value`					as `value` FROM `contract_psow_custom_fields` WHERE `id`=\"" + id + "\";";

				if(action == "AJAX_updateCostCenterNumber")					sql_query = "SELECT `number`				as `value` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateCostCenterAct")					sql_query = "SELECT `act_number`			as `value` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateCostCenterSignDate")				sql_query = "SELECT `sign_date`				as `value` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateCostCenterStartDate")				sql_query = "SELECT `start_date`			as `value` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateCostCenterEndDate")				sql_query = "SELECT `end_date`				as `value` FROM `cost_centers` WHERE `id`=\"" + sow_id + "\";";
				if(action == "AJAX_updateCostCenterCustomField")			sql_query = "SELECT `value`					as `value` FROM `cost_center_custom_fields` WHERE `id`=\"" + id + "\";";

				if(sql_query.length())
				{
					if(db->Query(sql_query))
					{
						result = db->Get(0, "value");
					}
					else
					{
						MESSAGE_DEBUG("", "", "" + action + " with id(" + id + ") returned empty result");
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "unknown action (" + action + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "id is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string GetTimecardLineID(string timecard_id, string task_id, CMysql *db)
{
	string 	result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	if(timecard_id.length())
	{
		if(task_id.length())
		{
			if(db->Query("SELECT `id` FROM `timecard_lines` WHERE `timecard_id`=\"" + timecard_id + "\" AND `timecard_task_id`=\"" + task_id + "\";"))
			{
				result = db->Get(0, "id");
			}
			else
			{
				MESSAGE_DEBUG("", "", "timecard_line(timecard_id / task_id: " + timecard_id + "/" + task_id + ") not fount in DB");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "task_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_id is empty");
	}


	{
		MESSAGE_DEBUG("", "", "finish (result = " + result + ")");
	}

	return result;
}
/*
vector<string> GetTimecardApprovals(string timecard_id, CMysql *db, CUser *user)
{
	vector<string>		result;

	MESSAGE_DEBUG("", "", "start");

	int		affected = db->Query(
				"SELECT `id` FROM `timecard_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
				";");

	// --- get timecard approvals
	for(int i = 0; i < affected; ++i)
	{
		result.push_back(db->Get(i, "id"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

vector<string> GetTimecardApprovers(string timecard_id, CMysql *db, CUser *user)
{
	vector<string>		result;

	MESSAGE_DEBUG("", "", "start");

	int		affected = db->Query(
				"SELECT `id` FROM `timecard_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
				";");

	// --- get timecard approvers
	for(int i = 0; i < affected; ++i)
	{
		result.push_back(db->Get(i, "id"));
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}
*/
bool SubmitTimecard(string timecard_id, CMysql *db, CUser *user)
{
	bool 	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(timecard_id.length())
	{
		struct ApproveClass
		{
			string	id;
			string	timecard_id;
			string	approver_id;
			string	decision;
			string	comment;
			string	eventTimestamp;
		};
		map<string, bool>			approvers;
		vector<ApproveClass>		approves_list;
		int							affected;
		bool						all_approvers_confirm = true;

		affected = db->Query(
				"SELECT `id` FROM `timecard_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\")"
				";");
		for(int i = 0; i < affected; i++)
		{
			approvers[db->Get(i, "id")] = false;
		}

		affected = db->Query(
				"SELECT * FROM `timecard_approvals` WHERE "
					"`eventTimestamp` > (SELECT `eventTimestamp` FROM `timecards` WHERE `id`=\"" + timecard_id + "\") "
					"AND "
					"`timecard_id`=\"" + timecard_id + "\""
				";");
		for(int i = 0; i < affected; i++)
		{
			ApproveClass	item;

			item.id = db->Get(i, "id");
			item.timecard_id = db->Get(i, "timecard_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			approves_list.push_back(item);

			if(approvers.find(item.approver_id) == approvers.end())
			{
				MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") not found in approvers list. Check DB structure !");
			}
			else
			{
				if(item.decision == "approved")
					approvers[item.approver_id] = true;
				else
				{
					MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") did (" + item.decision + ") action on timecard(" + item.timecard_id + "). According to timecard approve workflow, this branch can be used for \"approved\" timecards only. Check timecard workflow !");
				}
			}
		}

		for(auto &approver: approvers)
		{
			if(!approver.second) { all_approvers_confirm = false; break; }
		}

		if(all_approvers_confirm)
		{
			db->Query("UPDATE `timecards` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + timecard_id + "\";");
			if(db->isError())
			{
				MESSAGE_ERROR("", "", "fail to update timecards table with timecard_id(" + timecard_id + ")");
			}
			else
			{
				result = true;
			}
		}
		else
		{
			result = true;
			MESSAGE_DEBUG("", "", "not all approvers approved timecard.id(" + timecard_id + ")");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string GetNumberOfTimecardsInPendingState(CMysql *db, CUser *user)
{
	struct PendingTimecardClass
	{
		string	id;
		string	contract_sow_id;
		string	submit_date;

		PendingTimecardClass(string p1, string p2, string p3) : id(p1), contract_sow_id(p2), submit_date(p3) {};
		PendingTimecardClass() {};
	};
	vector<PendingTimecardClass>		timecards_list;

	int			affected = 0;
	int			pending_timecards = 0;
	string		company_id;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());

		affected = db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";");
		if(affected)
		{
			company_id = db->Get(0, "company_id");

			affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `timecards` WHERE "
										"`status`=\"submit\" "
										"AND "
										"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + company_id + "\");");
			for(int i = 0; i < affected; ++i)
			{
				PendingTimecardClass	item;

				item.id = db->Get(i, "id");
				item.contract_sow_id = db->Get(i, "contract_sow_id");
				item.submit_date = db->Get(i, "submit_date");

				timecards_list.push_back(item);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") doesn't belong to any agency");
		}
	}
	else if(user->GetType() == "approver")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());


		affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `timecards` WHERE "
									"`status`=\"submit\" "
									"AND "
									"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\");");
		for(int i = 0; i < affected; ++i)
		{
			PendingTimecardClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.submit_date = db->Get(i, "submit_date");

			timecards_list.push_back(item);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	for(auto &timecard: timecards_list)
	{
		if(db->Query("SELECT `decision` FROM `timecard_approvals` WHERE "
						"`timecard_id` = \"" + timecard.id + "\" "
						"AND "
						"`eventTimestamp` > \"" + timecard.submit_date + "\" "
						"AND "
						"`approver_id` IN (SELECT `id` FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + timecard.contract_sow_id + "\" AND `approver_user_id`=\"" + user->GetID() + "\")"))
		{
			string		decision = db->Get(0, "decision");

			if(decision == "approved")
			{
				// --- everything ok
			}
			else
			{
				MESSAGE_ERROR("", "", "check approval workflow for contract(" + timecard.contract_sow_id + ") at this time(" + timecard.submit_date + ") timcard.id(" + timecard.id + ") must have only \"approved\" approvals from user.id(" + user->GetID() + ")");
			}
		}
		else
		{
			++pending_timecards;
		}
	}

	MESSAGE_DEBUG("", "", "finish (number of pending_timecards is " + to_string(pending_timecards) + ")");

	return to_string(pending_timecards);
}

string GetNumberOfBTInPendingState(CMysql *db, CUser *user)
{
	struct PendingBTClass
	{
		string	id = "";
		string	contract_sow_id = "";
		string	submit_date = "";

		PendingBTClass(string p1, string p2, string p3) : id(p1), contract_sow_id(p2), submit_date(p3) {};
		PendingBTClass() {};
	};
	vector<PendingBTClass>		bt_list;

	int			affected = 0;
	int			pending_bt = 0;
	string		company_id;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());

		affected = db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";");
		if(affected)
		{
			company_id = db->Get(0, "company_id");

			affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `bt` WHERE "
										"`status`=\"submit\" "
										"AND "
										"`contract_sow_id` IN (SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + company_id + "\");");
			for(int i = 0; i < affected; ++i)
			{
				PendingBTClass	item(db->Get(i, "id"), db->Get(i, "contract_sow_id"), db->Get(i, "submit_date"));

				item.id = db->Get(i, "id");
				item.contract_sow_id = db->Get(i, "contract_sow_id");
				item.submit_date = db->Get(i, "submit_date");

				bt_list.push_back(item);
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") doesn't belong to any agency");
		}
	}
	else if(user->GetType() == "approver")
	{
		MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is " + user->GetType());

		affected = db->Query("SELECT `id`, `contract_sow_id`, `submit_date` FROM `bt` WHERE "
									"`status`=\"submit\" "
									"AND "
									"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\");");
		for(int i = 0; i < affected; ++i)
		{
			PendingBTClass	item;

			item.id = db->Get(i, "id");
			item.contract_sow_id = db->Get(i, "contract_sow_id");
			item.submit_date = db->Get(i, "submit_date");

			bt_list.push_back(item);
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	for(auto &bt: bt_list)
	{
		if(db->Query("SELECT `decision` FROM `bt_approvals` WHERE "
						"`bt_id` = \"" + bt.id + "\" "
						"AND "
						"`eventTimestamp` > \"" + bt.submit_date + "\" "
						"AND "
						"`approver_id` IN (SELECT `id` FROM `bt_approvers` WHERE `contract_sow_id`=\"" + bt.contract_sow_id + "\" AND `approver_user_id`=\"" + user->GetID() + "\")"))
		{
			string		decision = db->Get(0, "decision");

			if(decision == "approved")
			{
				// --- everything ok
			}
			else
			{
				MESSAGE_ERROR("", "", "check approval workflow for contract(" + bt.contract_sow_id + ") at this time(" + bt.submit_date + ") timcard.id(" + bt.id + ") must have only \"approved\" approvals from user.id(" + user->GetID() + ")");
			}
		}
		else
		{
			++pending_bt;
		}
	}

	MESSAGE_DEBUG("", "", "finish (number of pending_bt is " + to_string(pending_bt) + ")");

	return to_string(pending_bt);
}

string	GetObjectsSOW_Reusable_InJSONFormat(string object, string filter, CMysql *db, CUser *user)
{
	string		result = "";

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if((user->GetType() == "agency") || (user->GetType() == "approver"))
			{

				if(object == "timecard")
				{
					string		sql_query_where_statement = "`contract_sow_id` IN ( SELECT `contract_sow_id` FROM `timecard_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\")";

					if(filter == "submit") sql_query_where_statement += " AND `status` = \"submit\" ";

					result += 
						"\"timecards\":[" + GetTimecardsInJSONFormat("SELECT * FROM `timecards` WHERE " + sql_query_where_statement, db, user, true) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`id` IN ( SELECT `contract_sow_id` FROM `timecards` WHERE " + sql_query_where_statement + ") "
								";", db, user) + "]";
				}
				else if(object == "bt")
				{
					string		sql_query_where_statement = "`contract_sow_id` IN ( SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\")";

					if(filter == "submit") sql_query_where_statement += " AND `status` = \"submit\" ";

					result +=
						"\"bt\":[" + GetBTsInJSONFormat("SELECT * FROM `bt` WHERE " + sql_query_where_statement, db, user, true) + "],"
						"\"sow\":[" + GetSOWInJSONFormat(
								"SELECT * FROM `contracts_sow` WHERE "
									"`id` IN ( SELECT `contract_sow_id` FROM `bt` WHERE " + sql_query_where_statement + ") "
								";", db, user) + "]";
				}
				else
				{
					MESSAGE_ERROR("", "", "unknown object type (" + object + ")");
				}

			}
			else
			{
				MESSAGE_ERROR("", "", "unknown user.id(" + user->GetID() + ") type(" + user->GetID() + ")");
			}

		}
		else
		{
			MESSAGE_ERROR("", "", "db is NULL");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user is NULL");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + " symbols)");

	return result;
}

string	GetCurrencyRatesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	date;
		string	char_code;
		string	nominal;
		string	name;
		string	value;
	};
	vector<ItemClass>		itemsList;
	int						affected;
	string					result = "";

	{
		MESSAGE_DEBUG("", "", "start");
	}

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id = db->Get(i, "id");
			item.date = db->Get(i, "date");
			item.char_code = db->Get(i, "char_code");
			item.nominal = db->Get(i, "nominal");
			item.name = db->Get(i, "name");
			item.value = db->Get(i, "value");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\"" + item.id + "\",";
			result += "\"date\":\"" + item.date + "\",";
			result += "\"char_code\":\"" + item.char_code + "\",";
			result += "\"nominal\":\"" + item.nominal + "\",";
			result += "\"name\":\"" + item.name + "\",";
			result += "\"value\":\"" + item.value + "\"";

			result +=	"}";
		}
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + " symbols)");

	return result;
}


bool	SubmitBT(string bt_id, CMysql *db, CUser *user)
{
	bool 	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(bt_id.length())
	{
		struct ApproveClass
		{
			string	id;
			string	bt_id;
			string	approver_id;
			string	decision;
			string	comment;
			string	eventTimestamp;
		};
		map<string, bool>			approvers;
		vector<ApproveClass>		approves_list;
		int							affected;
		bool						all_approvers_confirm = true;

		affected = db->Query(
				"SELECT `id` FROM `bt_approvers` WHERE "
					"`contract_sow_id` IN (SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\")"
				";");
		for(int i = 0; i < affected; i++)
		{
			approvers[db->Get(i, "id")] = false;
		}

		affected = db->Query(
				"SELECT * FROM `bt_approvals` WHERE "
					"`eventTimestamp` > (SELECT `eventTimestamp` FROM `bt` WHERE `id`=\"" + bt_id + "\") "
					"AND "
					"`bt_id`=\"" + bt_id + "\""
				";");
		for(int i = 0; i < affected; i++)
		{
			ApproveClass	item;

			item.id = db->Get(i, "id");
			item.bt_id = db->Get(i, "bt_id");
			item.approver_id = db->Get(i, "approver_id");
			item.decision = db->Get(i, "decision");
			item.comment = db->Get(i, "comment");
			item.eventTimestamp = db->Get(i, "eventTimestamp");

			approves_list.push_back(item);

			if(approvers.find(item.approver_id) == approvers.end())
			{
				MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") not found in approvers list. Check DB structure !");
			}
			else
			{
				if(item.decision == "approved")
					approvers[item.approver_id] = true;
				else
				{
					MESSAGE_ERROR("", "", "approver id(" + item.approver_id + ") did (" + item.decision + ") action on bt(" + item.bt_id + "). According to bt approve workflow, this branch can be used for \"approved\" bt only. Check bt workflow !");
				}
			}
		}

		for(auto &approver: approvers)
		{
			if(!approver.second) { all_approvers_confirm = false; break; }
		}

		if(all_approvers_confirm)
		{
			db->Query("UPDATE `bt` SET `status`=\"approved\", `approve_date`=UNIX_TIMESTAMP() WHERE `id`=\"" + bt_id + "\";");
			if(db->isError())
			{
				MESSAGE_ERROR("", "", "fail to update bt table with bt_id(" + bt_id + ")");
			}
			else
			{
				result = true;
			}
		}
		else
		{
			result = true;
			MESSAGE_DEBUG("", "", "not all approvers approved bt.id(" + bt_id + ")");
		}

	}
	else
	{
		MESSAGE_ERROR("", "", "bt_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result = " + to_string(result) + ")");

	return result;
}

string isCBCurrencyRate(string date, string currency_name, string currency_nominal, string currency_value, CMysql *db)
{
	string	result = "N";

	MESSAGE_DEBUG("", "", "start");

	if(date.length())
	{
		if(currency_name.length())
		{
			if(currency_nominal.length())
			{
				if(currency_value.length())
				{
					if(db->Query("SELECT `id` FROM `currency_rate` WHERE `date`=\"" + date + "\" AND `name`=\"" + currency_name + "\" AND `nominal`=\"" + currency_nominal + "\" AND `value`=\"" + currency_value + "\";"))
					{
						result = "Y";
					}
					else
					{
						MESSAGE_DEBUG("", "", "currency rate(" + currency_nominal + "/" + currency_value + ") on date(" + date + ") doesn't aligned with Centro Bank");
					}
				}
				else
				{
					MESSAGE_DEBUG("", "", "currency_value is empty");
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "currency_nominal is empty");
			}
		}
		else
		{
			MESSAGE_DEBUG("", "", "currency_name is empty");
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "date is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + result + ")");

	return result;
}

string	GetBTExpensesInJSONFormat(string sqlQuery, CMysql *db, CUser *user)
{
	struct ItemClass
	{
		string	id;
		string	bt_id;
		string	bt_expense_template_id;
		string	comment;
		string	date;
		string	payment_type;
		string	price_foreign;
		string	price_domestic;
		string	currency_nominal;
		string	currency_name;
		string	currency_value;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;
	string					result = "";
	int						affected;

	MESSAGE_DEBUG("", "", "start");

	affected = db->Query(sqlQuery);
	if(affected)
	{
		for(int i = 0; i < affected; i++)
		{
			ItemClass	item;

			item.id							= db->Get(i, "id");
			item.bt_id			 			= db->Get(i, "bt_id");
			item.bt_expense_template_id		= db->Get(i, "bt_expense_template_id");
			item.comment					= db->Get(i, "comment");
			item.date						= db->Get(i, "date");
			item.payment_type				= db->Get(i, "payment_type");
			item.price_foreign				= db->Get(i, "price_foreign");
			item.price_domestic				= db->Get(i, "price_domestic");
			item.currency_nominal			= db->Get(i, "currency_nominal");
			item.currency_name				= db->Get(i, "currency_name");
			item.currency_value				= db->Get(i, "currency_value");
			item.eventTimestamp				= db->Get(i, "eventTimestamp");

			itemsList.push_back(item);
		}

		for (const auto& item : itemsList)
		{
			if(result.length()) result += ",";
			result +=	"{";

			result += "\"id\":\""						+ item.id + "\",";
			result += "\"bt_id\":\""					+ item.bt_id + "\",";
			result += "\"bt_expense_template_id\":\""	+ item.bt_expense_template_id + "\",";
			result += "\"bt_expense_templates\":["		+ GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `id`=\"" + item.bt_expense_template_id + "\";", db, user) + "],";
			result += "\"bt_expense_lines\":["			+ GetBTExpenseLinesInJSONFormat("SELECT * FROM `bt_expense_lines` WHERE `bt_expense_id`=\"" + item.id + "\";", db, user) + "],";
			result += "\"comment\":\""					+ item.comment + "\",";
			result += "\"date\":\""						+ item.date + "\",";
			result += "\"payment_type\":\""				+ item.payment_type + "\",";
			result += "\"price_foreign\":\""			+ item.price_foreign + "\",";
			result += "\"price_domestic\":\""			+ item.price_domestic + "\",";
			result += "\"currency_nominal\":\""			+ item.currency_nominal + "\",";
			result += "\"currency_name\":\""			+ item.currency_name + "\",";
			result += "\"currency_value\":\""			+ item.currency_value + "\",";
			result += "\"is_cb_currency_rate\":\""		+ isCBCurrencyRate(item.date, item.currency_name, item.currency_nominal, item.currency_value, db) + "\",";
			result += "\"eventTimestamp\":\""			+ item.eventTimestamp + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") bt have no expenses assigned");
		}
	}

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

auto isCostCenterBelongsToAgency(string cost_center_id, CMysql *db, CUser *user) -> bool
{
	auto result = false;

	MESSAGE_DEBUG("", "", "start");

	if(db && user)
	{
		if(cost_center_id.length())
		{
			if(db->Query("SELECT `id` FROM `cost_centers` WHERE `id`=\"" + cost_center_id + "\" AND `agency_company_id`=("
							"SELECT `id` FROM `company` WHERE `type`=\"agency\" AND `id`=("
								"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
							")"
						");"))
			{
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "cost_center_id(" + cost_center_id + ") doesn't belongs to company user.id(" + user->GetID() + ") employeed");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "cost_center_id is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "db or user not initialized");
	}
	
	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

string isUserAllowedAccessToBT(string bt_id, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(bt_id.length())
	{
		if(user->GetType() == "subcontractor")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `id` FROM `contracts_sow` WHERE `subcontractor_company_id` IN ("
						"SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user->GetID() + "\""
					")"
				")"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = "У Вас нет доступа";
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else if(user->GetType() == "agency")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id` IN ("
						"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
					")"
				")"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = "У Вас нет доступа";
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else if(user->GetType() == "approver")
		{
			if(db->Query(
				"SELECT `id` FROM `bt` WHERE `id`=\"" + bt_id + "\" AND `contract_sow_id` IN ("
					"SELECT `contract_sow_id` FROM `bt_approvers` WHERE `approver_user_id`=\"" + user->GetID() + "\""
					")"
				";"
			))
			{
				// --- everything is good
			}
			else
			{
				error_message = "У Вас нет доступа";
				MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have no access to bt.id(" + bt_id + ")");
			}
		}
		else
		{
			error_message = "Неизвестный тип пользователя";
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
		}
	}
	else
	{
		error_message = "Командировка не найдена";
		MESSAGE_ERROR("", "", "bt_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isExpenseTemplateIDValidToRemove(string bt_expense_template_id, CMysql *db) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(bt_expense_template_id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `bt_expenses` WHERE `bt_expense_template_id`=\"" + bt_expense_template_id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				// --- task.id valid to remve
			}
			else
			{
				error_message = "На этот расход субконтракторы отчитались " + counter + " раз. Поэтому нельзя удалить.";
				MESSAGE_DEBUG("", "", "subcontractors reported " + counter + " times on bt_expense_templates.id(" + bt_expense_template_id + ")");
			}
		}
		else
		{
			error_message = "Ошибка БД.";
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		error_message = "Неопределен идентификатор удаляемого шаблона.";
		MESSAGE_ERROR("", "", "bt_expense_template_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isExpenseTemplateLineIDValidToRemove(string bt_expense_line_template_id, CMysql *db) -> string
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(bt_expense_line_template_id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `bt_expense_line_templates` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + bt_expense_line_template_id + "\");"))
		{
			string	counter = db->Get(0, "counter");
			if(stoi(counter) > 1)
			{
				if(db->Query("SELECT COUNT(*) AS `counter` FROM `bt_expense_lines` WHERE `bt_expense_line_template_id`=\"" + bt_expense_line_template_id + "\";"))
				{
					string	counter = db->Get(0, "counter");
					if(counter == "0")
					{
						// --- task.id valid to remve
					}
					else
					{
						error_message = "Этот документ присутствует в " + counter + " отчетных документах. Нельзя удалить.";
						MESSAGE_DEBUG("", "", "subcontractors reported " + counter + " times on bt_expense_templates.id(" + bt_expense_line_template_id + ")");
					}
				}
				else
				{
					error_message = "Ошибка БД.";
					MESSAGE_ERROR("", "", "fail in sql-query syntax");
				}
			}
			else
			{
				error_message = "Нельзя удалять единственный документ из расхода.";
				MESSAGE_DEBUG("", "", "Can't remove single bt_expense_line_templates.id(" + bt_expense_line_template_id + ") from BT expense");
			}
		}
		else
		{
			error_message = "Ошибка БД.";
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		error_message = "Некорректные параметры.";
		MESSAGE_ERROR("", "", "bt_expense_line_template_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto isTaskIDValidToRemove(string task_id, CMysql *db) -> string
{
	auto	error_message = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(task_id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_lines` WHERE `timecard_task_id`=\"" + task_id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + task_id + "\";"))
				{
					string	counter = db->Get(0, "counter");
					if(counter == "0")
					{
						if(db->Query("SELECT COUNT(*) AS `counter` FROM `cost_center_assignment` WHERE `timecard_customer_id` IN ("
										"SELECT `timecard_customers_id` FROM `timecard_projects` WHERE `id` IN ("
											"SELECT `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + task_id + "\""
										")"
									");"))
						{
							auto	counter = stoi(db->Get(0, "counter"));
							if(counter == 0)
							{
								// --- task.id valid to remve
							}
							else
							{
								MESSAGE_DEBUG("", "", "cost center assigned on task.id(" + task_id + ")");
								error_message = gettext("This task assigned to cost center") + ". "s + gettext("removal prohibited") + ".";
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "fail in sql-query syntax");
							error_message = "Ошибка БД.";
						}
					}
					else
					{
						MESSAGE_DEBUG("", "", counter + "subcontractors assigned on task.id(" + task_id + ")");
						error_message = "Эта задача назначена " + counter + " субконтракторам. Поэтому нельзя удалить.";
					}
				}
				else
				{
					MESSAGE_ERROR("", "", "fail in sql-query syntax");
					error_message = "Ошибка БД.";
				}
			}
			else
			{
				MESSAGE_DEBUG("", "", "subcontractors reported " + counter + " times on task.id(" + task_id + ")");
				error_message = "На эту задачу субконтракторы отчитались. Поэтому нельзя удалить.";
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
			error_message = "Ошибка БД.";
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id is empty");
		error_message = "Задача не найдена";
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string isEmployeeIDValidToRemove(string employee_id, CMysql *db)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(employee_id.length())
	{
		if(db->Query("SELECT `user_id` FROM `company_employees` WHERE `id`=\"" + employee_id + "\";"))
		{
			string	user_id = db->Get(0, "user_id");

			if(db->Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user_id + "\";"))
			{
				error_message = "Нельзя уволить владельца компании";
				MESSAGE_DEBUG("", "", "company_employee.id(" + employee_id + ") is the company.id(" + db->Get(0, "id") + ") owner. He can't be fired.");
			}
			else
			{
				// --- good to go
			}
		}
		else
		{
			error_message = "Сотрудник не найден";
			MESSAGE_DEBUG("", "", "employee.id(" + employee_id + ") not found");
		}
	}
	else
	{
		error_message = "Сотрудник не найден";
		MESSAGE_ERROR("", "", "employee_id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

bool	isProjectIDValidToRemove(string id, CMysql *db)
{
	bool	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				// --- project.id valid to remve
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "there are " + counter + " task(s) belong to project.id(" + id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

bool	isCustomerIDValidToRemove(string id, CMysql *db)
{
	bool	result = false;

	MESSAGE_DEBUG("", "", "start");

	if(id.length())
	{
		if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\";"))
		{
			string	counter = db->Get(0, "counter");
			if(counter == "0")
			{
				// --- customer.id valid to remve
				result = true;
			}
			else
			{
				MESSAGE_DEBUG("", "", "there are " + counter + " project(s) belong to customer.id(" + id + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "fail in sql-query syntax");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;
}

pair<string, string> GetCustomerIDProjectIDByTaskID(string task_id, CMysql *db)
{
	string		project_id = "", customer_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
		"SELECT `timecard_projects_id`,`timecard_customers_id` FROM `timecard_tasks` "
		"INNER JOIN `timecard_projects` ON `timecard_projects`.`id`=`timecard_tasks`.`timecard_projects_id`"
		"WHERE `timecard_tasks`.`id`=\"" + task_id + "\";"
	))
	{
		project_id = db->Get(0, "timecard_projects_id");
		customer_id = db->Get(0, "timecard_customers_id");
	}

	MESSAGE_DEBUG("", "", "finish (cust_id/proj_id=" + customer_id + "/" + project_id + ")");

	return make_pair(customer_id, project_id);
}

/*
static string	isTimeReportedOnTaskIDBeyondPeriod(string task_id, string start, string end, CMysql *db)
{
	string	error_message = "";
	smatch	cm;
	string	start_year, start_month, start_date;
	string	end_year, end_month, end_date;


	MESSAGE_DEBUG("", "", "start");

	if(start.length())
	{
		// --- normalize start, end format
		if(regex_match(start, cm, regex("^([[:digit:]]{1,2})\\/([[:digit:]]{1,2})\\/([[:digit:]]{2,4})$")))
		{
			start_date = cm[1];
			start_month = cm[2];
			start_year = cm[3];
		}
		else if(regex_match(start, cm, regex("^([[:digit:]]{2,4})\\-([[:digit:]]{1,2})\\-([[:digit:]]{1,2})$")))
		{
			start_date = cm[3];
			start_month = cm[2];
			start_year = cm[1];
		}
		else
		{
			error_message = "Некрректный формат даты начала (" + start + ")";
			MESSAGE_ERROR("", "", "incorrect start date(" + start + ") format");
		}

		if(error_message.empty())
		{
			// --- coarse check moving start date
			if(db->Query(	"SELECT `timecard_id` FROM `timecard_lines` "
							"INNER JOIN `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id` "
							"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`period_start`<\"" + start_year + "-" + start_month + "-" + start_date + "\";"
						))
			{
				// --- placeholder for precise check
				
				error_message = "Невозможно сдвинуть дату, поскольку существует таймкарта с отчетным периодом раньше выбранной.";
				MESSAGE_DEBUG("", "", "Can't change period_start to " + start_year + "-" + start_month + "-" + start_date + ", due to exists timecard.id(" + db->Get(0, "timecard_id") + ") with earlier reporting period");
			}
		}

	}
	else
	{
		MESSAGE_DEBUG("", "", "start time is empty, don't checking start");
	}

	if(end.length())
	{
		// --- normalize start, end format
		if(regex_match(end, cm, regex("^([[:digit:]]{1,2})\\/([[:digit:]]{1,2})\\/([[:digit:]]{2,4})$")))
		{
			end_date = cm[1];
			end_month = cm[2];
			end_year = cm[3];
		}
		else if(regex_match(end, cm, regex("^([[:digit:]]{2,4})\\-([[:digit:]]{1,2})\\-([[:digit:]]{1,2})$")))
		{
			end_date = cm[3];
			end_month = cm[2];
			end_year = cm[1];
		}
		else
		{
			error_message = "Некрректный формат даты окончания (" + end + ")";
			MESSAGE_ERROR("", "", "incorrect end date(" + end + ") format");
		}

		if(error_message.empty())
		{
			// --- coarse check moving start date
			if(db->Query(	"SELECT `timecard_id` FROM `timecard_lines` "
							"INNER JOIN `timecards` ON `timecards`.`id`=`timecard_lines`.`timecard_id` "
							"WHERE `timecard_lines`.`timecard_task_id`=\"" + task_id + "\" AND `timecards`.`period_end`>\"" + end_year + "-" + end_month + "-" + end_date + "\";"
						))
			{
				// --- placeholder for precise check
				
				error_message = "Невозможно сдвинуть дату, поскольку существует таймкарта с отчетным периодом позднее выбранной.";
				MESSAGE_DEBUG("", "", "Can't change period_end to " + end_year + "-" + end_month + "-" + end_date + ", due to exists timecard.id(" + db->Get(0, "timecard_id") + ") with later reporting period");
			}
		}
	}
	else
	{
		MESSAGE_DEBUG("", "", "end time is empty, don't checking finish");
	}

	MESSAGE_DEBUG("", "", "finish");

	return error_message;
}

double DatesDifference(string start, string finish)
{
	double		result = 0;
	struct tm	start_tm, finish_tm;

	MESSAGE_DEBUG("", "", "start");

	start_tm = GetTMObject(start);
	finish_tm = GetTMObject(finish);

	MESSAGE_DEBUG("", "", "end (result is " + to_string(result) + ")");

	return difftime(mktime(&start_tm), mktime(&finish_tm));
}
*/

pair<int, int> FirstAndLastReportingDaysFromTimereport(const vector<string> &timereport)
{
	int start = 0, end = 0;

	MESSAGE_DEBUG("", "", "start");

	for(auto i = timereport.begin(); i != timereport.end(); ++i)
	{
		if((*i).length() && ((*i) != "0")) break;
		else ++start;
	}

	for(auto i = timereport.rbegin(); i != timereport.rend(); ++i)
	{
		if((*i).length() && ((*i) != "0")) break;
		else ++end;
	}

	MESSAGE_DEBUG("", "", "end (result = " + to_string(start) + "/" + to_string(end) + ")");

	return make_pair(start, end);
}

string	isValidToReportTime(string timecard_id, string task_id, string timereport, CMysql *db, CUser *user)
{
	string		error_message = "";
	struct tm	sow_start_date;
	struct tm	sow_end_date;
	struct tm	task_start_date;
	struct tm	task_end_date;
	struct tm	timecard_start_date;
	struct tm	timecard_end_date;
	struct tm	reporting_timeframe_start_date;
	struct tm	reporting_timeframe_end_date;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT * FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
	{
		string	sow_id = db->Get(0, "contract_sow_id");

		timecard_start_date = GetTMObject(db->Get(0, "period_start"));
		timecard_end_date = GetTMObject(db->Get(0, "period_end"));

		if(db->Query("SELECT `start_date`, `end_date` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
		{
			sow_start_date = GetTMObject(db->Get(0, "start_date"));
			sow_end_date = GetTMObject(db->Get(0, "end_date"));

			if(db->Query("SELECT `period_start`, `period_end` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + task_id + "\" AND `contract_sow_id`=\"" + sow_id + "\";"))
			{
				vector<string>	timeentry;

				task_start_date = GetTMObject(db->Get(0, "period_start"));
				task_end_date = GetTMObject(db->Get(0, "period_end"));
				timeentry = SplitTimeentry(timereport);

				if(timeentry.size() == round(difftime(mktime(&timecard_end_date), mktime(&timecard_start_date)) / 3600 / 24 + 1))
				{
					int		first_reporting_day, last_reporting_day;

					tie(first_reporting_day, last_reporting_day) = FirstAndLastReportingDaysFromTimereport(timeentry);

					reporting_timeframe_start_date = timecard_start_date;
					reporting_timeframe_end_date = timecard_end_date;

					reporting_timeframe_start_date.tm_mday += first_reporting_day;
					reporting_timeframe_end_date.tm_mday -= last_reporting_day;

					if(reporting_timeframe_start_date <= reporting_timeframe_end_date)
					{
						struct tm available_reporting_start = (task_start_date > sow_start_date ? task_start_date : sow_start_date);
						struct tm available_reporting_end = (task_end_date < sow_end_date ? task_end_date : sow_end_date);

						if(available_reporting_start <= available_reporting_end)
						{
							if(	(available_reporting_start <= reporting_timeframe_start_date) &&
								(available_reporting_end   >= reporting_timeframe_end_date)
								)
							{

							}
							else
							{
								error_message = "Вы не можете отчитываться на задачу. Задача( с " + PrintDate(available_reporting_start) + " по " + PrintDate(available_reporting_end) + "), Ваш отчет( с " + PrintDate(reporting_timeframe_start_date) + " по " + PrintDate(reporting_timeframe_end_date) + ")";
								MESSAGE_ERROR("", "", "Can't report to task_id(" + task_id + "). Available reporting period(" + PrintDate(available_reporting_start) + " - " + PrintDate(available_reporting_end) + "), timereport(" + PrintDate(reporting_timeframe_start_date) + " - " + PrintDate(reporting_timeframe_end_date) + ")");
							}
						}
						else
						{
							error_message = "Задача не активна в данном контракте. Задача( с " + PrintDate(task_start_date) + " по " + PrintDate(task_end_date) + "), догвор( с " + PrintDate(sow_start_date) + " по " + PrintDate(sow_end_date) + ")";
							MESSAGE_ERROR("", "", "task_id(" + task_id + ") not active in sow_id(" + sow_id + "). Task(" + PrintDate(task_start_date) + " - " + PrintDate(task_end_date) + "), sow(" + PrintDate(sow_start_date) + " - " + PrintDate(sow_end_date) + ")");
						}
					}
					else
					{
						error_message = "Начало Вашего отчета(" + PrintDate(reporting_timeframe_start_date) + ") позднее окончания(" + PrintDate(reporting_timeframe_end_date) + ")";
						MESSAGE_ERROR("", "", "reporting start (" + PrintDate(reporting_timeframe_start_date) + ") later than finish(" + PrintDate(reporting_timeframe_end_date) + ")");
					}
				}
				else
				{
					error_message = "Ваш отчет содержит " + to_string(timeentry.size()) + " дней, хотя ожидается " + to_string(round(difftime(mktime(&timecard_end_date), mktime(&timecard_start_date)) / 3600 / 24) + 1);
					MESSAGE_ERROR("", "", "timecard_id(" + timecard_id + ") size(" + to_string(round(difftime(mktime(&timecard_end_date), mktime(&timecard_start_date)) / 3600 / 24) + 1) + ") differ in size from reported timeentry(" + timereport + ") size (" + to_string(timeentry.size()) + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "task_assignment(task_id/sow_id - " + task_id + "/" + sow_id + ") start and end period can't be defined");
				error_message = "Вы не назначены на задачу";
			}

		}
		else
		{
			MESSAGE_ERROR("", "", "sow(" + sow_id + ") start and end period can't be defined");
			error_message = "Не найден договор";
		}
	}
	else
	{
		error_message = "Тамкарты нет в БД";
		MESSAGE_ERROR("", "", "fail to find timecard.id(" + timecard_id + ") in db");
	}


	MESSAGE_DEBUG("", "", "finish (error_message.length is " + to_string(error_message.length()) + ")");

	return error_message;	
}

string	GetInfoToReturnByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(action == "AJAX_addTimecardApproverToSoW")	result = "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "]";
	if(action == "AJAX_addBTExpenseApproverToSoW")	result = "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + sow_id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "]";

	MESSAGE_DEBUG("", "", "finish (result length is " + to_string(result.length()) + ")");

	return result;
}

string	ResubmitEntitiesByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	if(id.length())
	{
		if(action.length())
		{
			if(new_value.length())
			{
					if(action == "AJAX_deleteTimecardApproverFromSoW")
					{
						if(sow_id.length())
						{
							int	affected = db->Query("SELECT `id` FROM `timecards` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";");
							vector<string> timecard_list;

							for(int i = 0; i < affected; ++i)
							{
								timecard_list.push_back(db->Get(i, "id"));
							}

							for (auto &timecard_id: timecard_list)
							{
								if(SubmitTimecard(timecard_id, db, user))
								{

								}
								else
								{
									MESSAGE_ERROR("", "", "fail to re-submit timecard.id(" + timecard_id + ")");
								}
							}
							
						}
						else
						{
							error_message = "Не указан номер договора";
							MESSAGE_ERROR("", "", "sow.id is empty")
						}
					}
					else if(action == "AJAX_deleteBTExpenseApproverFromSoW")
					{
						if(sow_id.length())
						{
							int	affected = db->Query("SELECT `id` FROM `bt` WHERE `status`=\"submit\" AND `contract_sow_id`=\"" + sow_id + "\";");
							vector<string> bt_list;

							for(int i = 0; i < affected; ++i)
							{
								bt_list.push_back(db->Get(i, "id"));
							}

							for (auto &bt_id: bt_list)
							{
								if(SubmitBT(bt_id, db, user))
								{

								}
								else
								{
									MESSAGE_ERROR("", "", "fail to re-submit timecard.id(" + bt_id + ")");
								}
							}
							
						}
						else
						{
							error_message = "Не указан номер договора";
							MESSAGE_ERROR("", "", "sow.id is empty")
						}
					}
					else
					{
						error_message = "Неизвестное действие";
						MESSAGE_ERROR("", "", "unknown action (" + action + ")");
					}

			}
			else
			{
				error_message = "Новое значение некорректно";
				MESSAGE_ERROR("", "", "new_value is empty");
			}
		}
		else
		{
			error_message = "Неизвестное действие";
			MESSAGE_ERROR("", "", "action is empty");
		}
	}
	else
	{
		error_message = "Неизвестный идентификатор поля";
		MESSAGE_ERROR("", "", "id is empty");
	}


	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

auto DeleteEntryByAction(string action, string id, CMysql *db, CUser *user) -> string
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action == "AJAX_deleteTemplateAgreement_company")
			{
				db->Query("DELETE FROM `company_agreement_files` WHERE `id`=\"" + id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to remove from table company_agreement_files");
				}
			}
			else if(action == "AJAX_deleteTemplateAgreement_sow")
			{
				db->Query("DELETE FROM `contract_sow_agreement_files` WHERE `id`=\"" + id + "\";");
				if(db->isError())
				{
					MESSAGE_ERROR("", "", "fail to remove from table company_agreement_files");
				}
			}
			else
			{
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Неизвестный идентификатор поля";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;	
}

string	SetNewValueByAction(string action, string id, string sow_id, string new_value, CMysql *db, CUser *user)
{
	string	error_message = "";

	MESSAGE_DEBUG("", "", "start");

	{
		if(id.length())
		{
			if(action.length())
			{
				if(new_value.length())
				{
					if(error_message.empty())
					{
						string		sql_query = "";

						if(action == "AJAX_updateCustomerTitle") 					sql_query = "UPDATE `timecard_customers`		SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateProjectTitle") 					sql_query = "UPDATE `timecard_projects`			SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateTaskTitle") 						sql_query = "UPDATE `timecard_tasks`			SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateTitle")				sql_query = "UPDATE `bt_expense_templates`		SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateTaxable")			sql_query = "UPDATE `bt_expense_templates`		SET `taxable`						=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateAgencyComment")		sql_query = "UPDATE `bt_expense_templates`		SET `agency_comment`				=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineTitle")			sql_query = "UPDATE `bt_expense_line_templates`	SET `title`							=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineDescription")	sql_query = "UPDATE `bt_expense_line_templates`	SET `description`					=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineTooltip")		sql_query = "UPDATE `bt_expense_line_templates`	SET `tooltip`						=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineDomType")		sql_query = "UPDATE `bt_expense_line_templates`	SET `dom_type`						=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateExpenseTemplateLineRequired")		sql_query = "UPDATE `bt_expense_line_templates`	SET `required`						=\"" + (new_value == "true" ? "Y"s : "N"s) + "\" WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePeriodStart") 						sql_query = "UPDATE `timecard_task_assignment`	SET `period_start`					=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePeriodEnd") 						sql_query = "UPDATE `timecard_task_assignment`	SET `period_end`					=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyTitle")						sql_query = "UPDATE	`company`					SET `name` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyDescription")				sql_query = "UPDATE	`company`					SET `description`					=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyWebSite")					sql_query = "UPDATE	`company`					SET `webSite`						=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyTIN")						sql_query = "UPDATE	`company`					SET `tin` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyVAT")						sql_query = "UPDATE	`company`					SET `vat` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyAccount")					sql_query = "UPDATE	`company`					SET `account`						=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyOGRN")						sql_query = "UPDATE	`company`					SET `ogrn` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyKPP")						sql_query = "UPDATE	`company`					SET `kpp` 							=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyLegalAddress")				sql_query = "UPDATE	`company`					SET `legal_address`					=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyMailingAddress")			sql_query = "UPDATE	`company`					SET `mailing_address`				=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyMailingZipID")				sql_query = "UPDATE	`company`					SET `mailing_geo_zip_id`			=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyLegalZipID")				sql_query = "UPDATE	`company`					SET `legal_geo_zip_id`				=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCompanyBankID")					sql_query = "UPDATE	`company`					SET `bank_id`						=\"" + new_value + "\",`lastActivity`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSoWEditCapability")				sql_query = "UPDATE	`company_employees`			SET `allowed_change_sow`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateAgencyEditCapability")				sql_query = "UPDATE	`company_employees`			SET `allowed_change_agency_data`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSubcontractorCreateTasks")			sql_query = "UPDATE	`contracts_sow`				SET `subcontractor_create_tasks`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_addTimecardApproverToSoW")				sql_query = "INSERT INTO `timecard_approvers` (`approver_user_id`,`contract_sow_id`) VALUES (\"" + new_value + "\", \"" + sow_id + "\");";
						if(action == "AJAX_addBTExpenseApproverToSoW")				sql_query = "INSERT INTO `bt_approvers` (`approver_user_id`,`contract_sow_id`) 		VALUES (\"" + new_value + "\", \"" + sow_id + "\");";
						if(action == "AJAX_deleteCostCenterFromCustomer")			sql_query = "DELETE FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";";
						if(action == "AJAX_deleteCostCenter")						sql_query = "DELETE FROM `cost_centers` WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterTitle")					sql_query = "UPDATE `cost_centers`				SET `title`							=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterDescription")			sql_query = "UPDATE `cost_centers`				SET `description`					=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";

						if(action == "AJAX_updateSoWCustomField")					sql_query = "UPDATE `contract_sow_custom_fields`SET `value`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateSoWNumber")						sql_query = "UPDATE `contracts_sow`				SET `number`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWAct")							sql_query = "UPDATE `contracts_sow`				SET `act_number`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWSignDate")						sql_query = "UPDATE `contracts_sow`				SET `sign_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWStartDate")						sql_query = "UPDATE `contracts_sow`				SET `start_date`	=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWEndDate")						sql_query = "UPDATE `contracts_sow`				SET `end_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateSoWDayRate") {c_float	num(new_value); sql_query = "UPDATE `contracts_sow` 		SET `day_rate`		=\"" + string(num) + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";"; }

						if(action == "AJAX_updatePSoWCustomField")					sql_query = "UPDATE `contract_psow_custom_fields`SET `value`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updatePSoWNumber")						sql_query = "UPDATE `contracts_psow`			SET `number`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWAct")							sql_query = "UPDATE `contracts_psow`			SET `act_number`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWSignDate")						sql_query = "UPDATE `contracts_psow`			SET `sign_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWStartDate")					sql_query = "UPDATE `contracts_psow`			SET `start_date`	=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWEndDate")						sql_query = "UPDATE `contracts_psow`			SET `end_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updatePSoWDayRate")  {c_float num(new_value); sql_query = "UPDATE `contracts_psow` 		SET `day_rate`		=\"" + string(num) + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";"; }
						if(action == "AJAX_updatePSoWBTMarkup") {c_float num(new_value); sql_query = "UPDATE `contracts_psow` 		SET `bt_markup`		=\"" + string(num) + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";"; }
						if(action == "AJAX_updatePSoWBTMarkupType")					sql_query = "UPDATE `contracts_psow` 			SET `bt_markup_type`=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";

						if(action == "AJAX_updateCostCenterCustomField")			sql_query = "UPDATE `cost_center_custom_fields` SET `value`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateCostCenterNumber")					sql_query = "UPDATE `cost_centers`				SET `number`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateCostCenterAct")					sql_query = "UPDATE `cost_centers`				SET `act_number`	=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateCostCenterSignDate")				sql_query = "UPDATE `cost_centers`				SET `sign_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateCostCenterStartDate")				sql_query = "UPDATE `cost_centers`				SET `start_date`	=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
						if(action == "AJAX_updateCostCenterEndDate")				sql_query = "UPDATE `cost_centers`				SET `end_date`		=STR_TO_DATE(\"" + new_value + "\",\"%d/%m/%Y\"),`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";

						if(action == "AJAX_updateTemplateAgreement_company_Title")	sql_query = "UPDATE `company_agreement_files`	SET `title`			=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
						if(action == "AJAX_updateTemplateAgreement_sow_Title")		sql_query = "UPDATE `contract_sow_agreement_files` SET `title`		=\"" + new_value + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";

						// --- expense line template payment part
						if(action == "AJAX_updateExpenseTemplateLinePaymentCash")
						{
							if(db->Query("SELECT `payment` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";"))
							{
								string	curr_payment = db->Get(0, "payment");
								auto	find_position = curr_payment.find("card");
								auto	card = (find_position != string::npos);
								auto	cash = (new_value == "true");
								
								if(cash && card) new_value = "cash and card";
								else if(card) new_value = "card";
								else if(cash) new_value = "cash";
								else
								{
									MESSAGE_ERROR("", "", "fail to set payment method cash/card(" + to_string(cash) + "/" + to_string(card) + ")");
								}
							}
							else
							{
								new_value = "cash and card";
								MESSAGE_ERROR("", "", "fail to take payment value from bt_expense_line_templates table");
							}
							sql_query = "UPDATE `bt_expense_line_templates`	SET `payment`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						}
						if(action == "AJAX_updateExpenseTemplateLinePaymentCard")			
						{
							if(db->Query("SELECT `payment` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";"))
							{
								string	curr_payment = db->Get(0, "payment");
								auto	find_position = curr_payment.find("cash");
								auto	cash = (find_position != string::npos);
								auto	card = (new_value == "true");
								
								if(cash && card) new_value = "cash and card";
								else if(card) new_value = "card";
								else if(cash) new_value = "cash";
								else
								{
									MESSAGE_ERROR("", "", "fail to set payment method cash/card(" + to_string(cash) + "/" + to_string(card) + ")");
								}
							}
							else
							{
								new_value = "cash and card";
								MESSAGE_ERROR("", "", "fail to take payment value from bt_expense_line_templates table");
							}
							sql_query = "UPDATE `bt_expense_line_templates`	SET `payment`		=\"" + new_value + "\" WHERE `id`=\"" + id + "\";";
						}

						if(action == "AJAX_updateAgencyPosition")
						{
							auto	position_id = GetCompanyPositionIDByTitle(new_value, db);

							if(position_id.length())
								sql_query = "UPDATE	`company_employees`	SET `position_id` =\"" + position_id + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + id + "\";";
							else
								MESSAGE_ERROR("", "", "fail to get position.id");
						}
						if(action == "AJAX_updateSoWPosition")
						{
							auto	position_id = GetCompanyPositionIDByTitle(new_value, db);

							if(position_id.length())
								sql_query = "UPDATE	`contracts_sow`	SET `company_position_id` =\"" + position_id + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
							else
								MESSAGE_ERROR("", "", "fail to get position.id");
						}
						if(action == "AJAX_updatePSoWPosition")
						{
							auto	position_id = GetCompanyPositionIDByTitle(new_value, db);

							if(position_id.length())
								sql_query = "UPDATE	`contracts_psow` SET `company_position_id` =\"" + position_id + "\",`eventTimestamp`=UNIX_TIMESTAMP() WHERE `id`=\"" + sow_id + "\";";
							else
								MESSAGE_ERROR("", "", "fail to get position.id");
						}
						if(action == "AJAX_updateCostCenterToCustomer")
						{
							auto	cost_center_assignment_id = ""s;

							if(db->Query("SELECT `id` FROM `cost_center_assignment` WHERE `timecard_customer_id`=\"" + id + "\";"))
							{
								cost_center_assignment_id = db->Get(0, "id");
							}
							else
							{
								long int temp = db->InsertQuery("INSERT INTO `cost_center_assignment` (`cost_center_id`,`timecard_customer_id`,`assignee_user_id`,`eventTimestamp`) VALUES (\"" + new_value + "\",\"" + id + "\",\"" + user->GetID() + "\", UNIX_TIMESTAMP());");
								if(temp)
								{
									cost_center_assignment_id = to_string(temp);
									MESSAGE_DEBUG("", "", "new cost_center_assignment.id(" + cost_center_assignment_id + ") created.");
								}
								else
								{
									cost_center_assignment_id = "0";
									MESSAGE_ERROR("", "", "fail to insert to cost_center_assignment table");
								}
							}

							if(CreatePSoWfromTimecardCustomerIDAndCostCenterID(id, new_value, db, user))
							{
								// --- good to go
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to create PSoW while cost_center.id(" + new_value + ") assigned to timecard_customer.id(" + id + ")")
							}

							sql_query = "UPDATE	`cost_center_assignment` SET `cost_center_id` =\"" + new_value + "\" WHERE `timecard_customer_id`=\"" + id + "\";";
						}

						if(action == "AJAX_deleteTimecardApproverFromSoW")
						{

							db->Query("DELETE FROM `timecard_approvals` WHERE `approver_id`=\"" + id + "\";");
							sql_query =  "DELETE FROM `timecard_approvers` WHERE `id`=\"" + id + "\";";
						}
						if(action == "AJAX_deleteBTExpenseApproverFromSoW")
						{
							db->Query("DELETE FROM `bt_approvals` WHERE `approver_id`=\"" + id + "\";");
							sql_query =  "DELETE FROM `bt_approvers` WHERE `id`=\"" + id + "\";";
						}

						if(sql_query.length())
						{
							db->Query(sql_query);
							if(db->isError())
							{
								error_message = "Ошибка БД";
								MESSAGE_ERROR("", "", "sql_query has failed (" + db->GetErrorMessage() + ")");
							}
							else
							{
								MESSAGE_DEBUG("", "", "" + action + " with id(" + id + ") returned empty result");
							}
						}
						else
						{
							error_message = "Неизвестное действие";
							MESSAGE_ERROR("", "", "unknown action (" + action + ")");
						}
					}
				}
				else
				{
					error_message = "Новое значен некорректно";
					MESSAGE_ERROR("", "", "new_value is empty");
				}
			}
			else
			{
				error_message = "Неизвестное действие";
				MESSAGE_ERROR("", "", "action is empty");
			}
		}
		else
		{
			error_message = "Неизвестный идентификатор поля";
			MESSAGE_ERROR("", "", "id is empty");
		}
	}

	MESSAGE_DEBUG("", "", "finish (error_message length is " + to_string(error_message.length()) + ")");

	return error_message;
}

string	GetSpelledEmployeeByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query(
			"SELECT"
				"`users`.`name` as `user_name`,"
				"`users`.`nameLast` as `user_nameLast`,"
				"`company_position`.`title` as `position_title`"
			"FROM `company_employees`"
			"INNER JOIN `users` ON `users`.`id`=`company_employees`.`user_id`"
			"INNER JOIN `company_position` ON `company_position`.`id`=`company_employees`.`position_id`"
			"WHERE `company_employees`.`id`=\"" + id + "\";"
		))	
	{
		result = string(db->Get(0, "user_name")) + " " + db->Get(0, "user_nameLast") + " (" + db->Get(0, "position_title") + ")";
	}
	else
	{
		MESSAGE_ERROR("", "", "customer_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardCustomerByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `timecard_customers` WHERE `id`=\"" + id + "\";"))	
	{
		result = db->Get(0, "title");
	}
	else
	{
		MESSAGE_ERROR("", "", "customer_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardProjectByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `timecard_customers_id` FROM `timecard_projects` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	customer_id = db->Get(0, "timecard_customers_id");

		result = GetSpelledTimecardCustomerByID(customer_id, db) + " / " + title;
	}
	else
	{
		MESSAGE_ERROR("", "", "project_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardTaskByID(string id, CMysql *db)
{
	string	result = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `timecard_projects_id` FROM `timecard_tasks` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	project_id = db->Get(0, "timecard_projects_id");

		result = GetSpelledTimecardProjectByID(project_id, db) + " / " + title;
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledBTExpenseTemplateByID(string id, CMysql *db)
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `agency_comment` FROM `bt_expense_templates` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	agency_comment = db->Get(0, "agency_comment");

		result = title + " ("s + agency_comment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseTemplateByAssignmentID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `agency_comment` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\");"))
	{
		string 	title = db->Get(0, "title");
		string	agency_comment = db->Get(0, "agency_comment");

		result = title + " ("s + agency_comment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseTemplateByLineID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `agency_comment` FROM `bt_expense_templates` WHERE `id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");"))
	{
		string 	title = db->Get(0, "title");
		string	agency_comment = db->Get(0, "agency_comment");

		result = title + " ("s + agency_comment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledCostCenterByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title` FROM `cost_centers` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");

		result = title;
	}
	else
	{
		MESSAGE_ERROR("", "", "cost_centers.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseTemplateLineByID(string id, CMysql *db) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`, `dom_type`, `description`, `payment` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\";"))
	{
		string 	title = db->Get(0, "title");
		string	description = db->Get(0, "description");
		string	dom_type = db->Get(0, "dom_type");
		string	payment = db->Get(0, "payment");

		result = title + " ("s + description + " / "s + dom_type + " / "s + payment + ")"s;
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_templates.id(" + id + ") not found");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledTimecardTaskAssignmentByID(string id, CMysql *db) -> string
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `period_start`, `period_end`, `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";"))
	{
		struct tm	period_start = GetTMObject(db->Get(0, "period_start"));
		struct tm	period_end = GetTMObject(db->Get(0, "period_end"));
		string		task_id = db->Get(0, "timecard_tasks_id");

		result = GetSpelledTimecardTaskByID(task_id, db) + " c " + PrintTime(period_start, RU_DATE_FORMAT) + " по " + PrintTime(period_end, RU_DATE_FORMAT);
	}
	else
	{
		MESSAGE_ERROR("", "", "task_id couldn't be found from task_assignment");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto	GetSpelledBTExpenseAssignmentByID(string id, CMysql *db) -> string
{
	string	result = "";
	string	bt_expense_template_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";"))
	{
		string		bt_expense_template_id = db->Get(0, "bt_expense_template_id");

		result = GetSpelledBTExpenseTemplateByID(id, db);
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_expense_template_id couldn't be found from task_assignment");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledUserNameByID(string id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `name`, `nameLast` FROM `users` WHERE `id`=\"" + id + "\";"))
	{
		result = string(db->Get(0, "name")) + " " + db->Get(0, "nameLast");
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledTimecardApproverNameByID(string id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `name`, `nameLast` FROM `users` WHERE `id`=( SELECT `approver_user_id` FROM `timecard_approvers` WHERE `id`=\"" + id + "\");"))
	{
		result = string(db->Get(0, "name")) + " " + db->Get(0, "nameLast");
	}
	else
	{
		MESSAGE_ERROR("", "", "timecard_approvers.id(" + id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledBTExpenseApproverNameByID(string id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `name`, `nameLast` FROM `users` WHERE `id`=( SELECT `approver_user_id` FROM `bt_approvers` WHERE `id`=\"" + id + "\");"))
	{
		result = string(db->Get(0, "name")) + " " + db->Get(0, "nameLast");
	}
	else
	{
		MESSAGE_ERROR("", "", "bt_approvers.id(" + id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledSoWCustomFieldNameByID(string custom_field_id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`,`description` FROM `contract_sow_custom_fields` WHERE `id`=\"" + custom_field_id + "\";"))
	{
		auto	description = ""s + db->Get(0, "description");

		result = string(db->Get(0, "title")) + (description.length() ? " / " + description : "");
	}
	else
	{
		MESSAGE_ERROR("", "", "custom_field.id(" + custom_field_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledPSoWCustomFieldNameByID(string custom_field_id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`,`description` FROM `contract_psow_custom_fields` WHERE `id`=\"" + custom_field_id + "\";"))
	{
		auto	description = ""s + db->Get(0, "description");

		result = string(db->Get(0, "title")) + (description.length() ? " / " + description : "");
	}
	else
	{
		MESSAGE_ERROR("", "", "custom_field.id(" + custom_field_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledCostCenterCustomFieldNameByID(string custom_field_id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `title`,`description` FROM `contract_psow_custom_fields` WHERE `id`=\"" + custom_field_id + "\";"))
	{
		auto	description = ""s + db->Get(0, "description");

		result = string(db->Get(0, "title")) + (description.length() ? " / " + description : "");
	}
	else
	{
		MESSAGE_ERROR("", "", "custom_field.id(" + custom_field_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledSoWByID(string sow_id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `sign_date`, `number` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";"))
	{
		result = string(db->Get(0, "number")) + " " + gettext("agreement from") + " " + db->Get(0, "sign_date");
	}
	else
	{
		MESSAGE_ERROR("", "", "contract_sow.id(" + sow_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string	GetSpelledPSoWByID(string psow_id, CMysql *db)
{
	string	result = "";
	string	task_id = "";

	MESSAGE_DEBUG("", "", "start");

	if(db->Query("SELECT `sign_date`, `number` FROM `contracts_psow` WHERE `id`=\"" + psow_id + "\";"))
	{
		result = string(db->Get(0, "number")) + " " + gettext("agreement from") + " " + db->Get(0, "sign_date");
	}
	else
	{
		MESSAGE_ERROR("", "", "contracts_psow.id(" + psow_id + ") not found");
	}

	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

static pair<string, string> GetNotificationDescriptionAndSoWQuery(string action, string id, string sow_id, string existing_value, string new_value, CMysql *db, CUser *user)
{
	string			notification_description, sql_query;

	MESSAGE_DEBUG("", "", "start");

	if(action == "AJAX_updateCustomerTitle") 	
	{
		notification_description = "Данные таймкарты: изменилось название заказчика " + existing_value + " -> " + new_value;
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
							"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id`=\"" + id + "\""
						")"
					");";
	}
	if(action == "AJAX_updateProjectTitle") 	
	{
		notification_description = "Данные таймкарты: изменилось название проекта " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardProjectByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id` IN ("
						"SELECT `id` FROM `timecard_tasks` WHERE `timecard_projects_id`=\"" + id + "\""
					");";
	}
	if(action == "AJAX_updateTaskTitle") 		
	{
		notification_description = "Данные таймкарты: изменилось название задачи " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardTaskByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `timecard_tasks_id`=\"" + id + "\";";
	}
	if(action == "AJAX_addTaskAssignment")
	{
		notification_description = "Данные таймкарты: добавили назначение на задачу " + new_value + ".";
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_updatePeriodStart") 		
	{
		notification_description = "Данные таймкарты: изменилась дата начала работы над задачей " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardTaskAssignmentByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_updatePeriodEnd") 		
	{
		notification_description = "Данные таймкарты: изменилась дата окончания работы над задачей " + existing_value + " -> " + new_value + ". Полная информация: " + GetSpelledTimecardTaskAssignmentByID(id, db);
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_deleteTaskAssignment")
	{
		notification_description = "Данные таймкарты: удалено назначение на задачу " + GetSpelledTimecardTaskAssignmentByID(id, db) + ".";
		sql_query = "SELECT DISTINCT `contract_sow_id` FROM `timecard_task_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_deleteBTExpenseAssignment")
	{
		notification_description = "Данные командировки: удалено возмещение расхода " + GetSpelledBTExpenseTemplateByAssignmentID(id, db) + " из SoW(" + GetSpelledSoWByID(sow_id, db) + ").";
		sql_query = "SELECT DISTINCT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";";
	}
	if(action == "AJAX_addTask")
	{
		notification_description = "Данные таймкарты: добавили новую задачу (" + new_value + "). Полная информация: " + GetSpelledTimecardTaskByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteTask")
	{
		notification_description = "Данные таймкарты: удалили задачу " + GetSpelledTimecardTaskByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteProject")
	{
		notification_description = "Данные таймкарты: удалили проект " + GetSpelledTimecardProjectByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteCustomer")
	{
		notification_description = "Данные таймкарты: удалили заказчика " + GetSpelledTimecardCustomerByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteEmployee")
	{
		notification_description = "Данные компании: уволили " + GetSpelledEmployeeByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCompanyTitle")
	{
		notification_description = "Данные компании: Название компании " + existing_value + " изменилось на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyDescription")
	{
		notification_description = "Данные компании: Описание компании изменилось на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyWebSite")
	{
		notification_description = "Данные компании: Web-сайт компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyTIN")
	{
		notification_description = "Данные компании: ИНН компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyVAT")
	{
		notification_description = "Данные компании: НДС компании изменился " + existing_value + " -> " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyAccount")
	{
		notification_description = "Данные компании: Р/C компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyOGRN")
	{
		notification_description = "Данные компании: ОГРН компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyKPP")
	{
		notification_description = "Данные компании: КПП компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyLegalAddress")
	{
		notification_description = "Данные компании: Юр. адрес компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyMailingAddress")
	{
		notification_description = "Данные компании: Фактический адрес компании " + existing_value + " изменился на " + new_value;
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateAgencyPosition")
	{
		notification_description = gettext("Employee data") + ": "s + GetSpelledEmployeeByID(id, db) + " " + gettext("promouted from") + " " + existing_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateAgencyEditCapability")
	{
		notification_description = "Данные сотрудника: " + GetSpelledEmployeeByID(id, db) + (new_value == "N" ? " не" : "") + " может менять данные агенства";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateSoWEditCapability")
	{
		notification_description = "Данные сотрудника: " + GetSpelledEmployeeByID(id, db) + (new_value == "N" ? " не" : "") + " может менять SoW";
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCompanyMailingZipID")
	{
		notification_description = "Данные компании: Изменился фактический адрес компании с " + GetSpelledZIPByID(existing_value, db) + " на " + GetSpelledZIPByID(new_value, db);
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyLegalZipID")
	{
		notification_description = "Данные компании: Изменился юридический адрес компании с " + GetSpelledZIPByID(existing_value, db) + " на " + GetSpelledZIPByID(new_value, db);
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_updateCompanyBankID")
	{
		notification_description = "Данные компании: Изменился банк компании с " + GetSpelledBankByID(existing_value, db) + " на " + GetSpelledBankByID(new_value, db);
		if(user->GetType() == "agency")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `agency_company_id`=\"" + id + "\";";
		else if(user->GetType() == "subcontractor")
			sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `subcontractor_company_id`=\"" + id + "\";";
		else
		{
			MESSAGE_ERROR("", "", "there is no notification for user type(" + user->GetType() + ")")
		}
	}
	if(action == "AJAX_addTimecardApproverToSoW")
	{
		notification_description = "Данные таймкарты: Добавили нового утвердителя " + GetSpelledUserNameByID(new_value, db) + "  (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_addBTExpenseApproverToSoW")
	{
		notification_description = "Данные командировки: Добавили нового утвердителя " + GetSpelledUserNameByID(new_value, db) + "  (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteTimecardApproverFromSoW")
	{
		notification_description = "Данные таймкарты: Удалили утвердителя таймкарт " + GetSpelledTimecardApproverNameByID(id, db) + "  (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteBTExpenseApproverFromSoW")
	{
		notification_description = "Данные командировки: Удалили утвердителя командировочных расходов " + GetSpelledBTExpenseApproverNameByID(id, db) + " (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSubcontractorCreateTasks")
	{
		notification_description = "Данные таймкарты: Субконтрактор " + (new_value == "N" ? string("не ") : string("")) + "может создавать задачи самостоятельно (SoW " + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_deleteExpenseTemplate")
	{
		notification_description = "Данные командировки: удалили возмещаемый расход " + GetSpelledBTExpenseTemplateByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteExpenseTemplateLine")
	{
		notification_description = "Данные командировки: удалили отчетный документ " + GetSpelledBTExpenseTemplateLineByID(id, db) + " из возмещаемого расхода "  + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addExpenseTemplate")
	{
		notification_description = "Данные командировки: добавили возмещаемый расход "  + GetSpelledBTExpenseTemplateByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addExpenseTemplateLine")
	{
		notification_description = "Данные командировки: добавили отчетный документ " + GetSpelledBTExpenseTemplateLineByID(id, db) + " к возмещаемому расходу "  + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_addCostCenter")
	{
		notification_description = gettext("Agency: cost center added") + " "s + GetSpelledCostCenterByID(id, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterToCustomer")
	{
		notification_description = gettext("Agency: cost center belongs to customer") + " "s + GetSpelledTimecardCustomerByID(id, db) + " "s + gettext("updated") + " "s + (existing_value.length() ? gettext("from") + " "s + GetSpelledCostCenterByID(existing_value, db) : "") + " "s + gettext("to") + " "s + GetSpelledCostCenterByID(new_value, db);
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteCostCenterFromCustomer")
	{
		notification_description = gettext("Agency: cost center belongs to customer") + " "s + GetSpelledTimecardCustomerByID(id, db) + " "s + (existing_value.length() ? " ("s + GetSpelledCostCenterByID(existing_value, db) : "") + ") "s + gettext("removed");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_deleteCostCenter")
	{
		notification_description = gettext("Agency: cost center") + " ("s + GetSpelledCostCenterByID(existing_value, db) + ") "s + gettext("removed");
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterTitle")
	{
		notification_description = gettext("Agency: cost center title updated") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterDescription")
	{
		notification_description = gettext("Agency: cost center")  + " ("s + GetSpelledCostCenterByID(id, db) + ") "  + gettext("description updated") + " "s + gettext("from") + " "s + existing_value + " "s + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}

	if(action == "AJAX_updateSoWNumber")
	{
		notification_description = gettext("SoW: number") + " "s + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWAct")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("updated") + " " + gettext("act number") + " " + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWPosition")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("position changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWDayRate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("dayrate changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWSignDate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("sign date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWStartDate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("start date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWEndDate")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("end date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `id` AS `contract_sow_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\";";
	}
	if(action == "AJAX_updateSoWCustomField")
	{
		notification_description = gettext("SoW") + " ("s + GetSpelledSoWByID(sow_id, db) + "): " + gettext("custom field") + "(" + GetSpelledSoWCustomFieldNameByID(id, db) + ") " + gettext("changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = "SELECT `contract_sow_id` AS `contract_sow_id` FROM `contract_sow_custom_fields` WHERE `id`=\"" + id + "\";";
	}

	if(action == "AJAX_updatePSoWNumber")
	{
		notification_description = gettext("PSoW: number") + " "s + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWAct")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("updated") + " " + gettext("act number") + " " + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWPosition")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("position changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWDayRate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("dayrate changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWBTMarkup")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("bt markup changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWBTMarkupType")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("bt markup type changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWSignDate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("sign date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWStartDate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("start date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWEndDate")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("end date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updatePSoWCustomField")
	{
		notification_description = gettext("PSoW") + " ("s + GetSpelledPSoWByID(sow_id, db) + "): " + gettext("custom field") + "(" + GetSpelledPSoWCustomFieldNameByID(id, db) + ") " + gettext("changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}

	if(action == "AJAX_updateCostCenterNumber")
	{
		notification_description = gettext("Cost Center: number") + " "s + gettext("updated") + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterAct")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(sow_id, db) + "): " + gettext("updated") + " " + gettext("act number") + " " + " " + gettext("from") + " "s + existing_value + " "  + gettext("to") + " "s + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterSignDate")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(sow_id, db) + "): " + gettext("sign date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterStartDate")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(sow_id, db) + "): " + gettext("start date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterEndDate")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(sow_id, db) + "): " + gettext("end date changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}
	if(action == "AJAX_updateCostCenterCustomField")
	{
		notification_description = gettext("Cost Center") + " ("s + GetSpelledCostCenterByID(sow_id, db) + "): " + gettext("custom field") + "(" + GetSpelledCostCenterCustomFieldNameByID(id, db) + ") " + gettext("changed") + " " + gettext("from") + " " + existing_value + " " + gettext("to") + " " + new_value;
		sql_query = ""; // --- don't notify subcontractors, only agency
	}



	if(action == "AJAX_updateExpenseTemplateTitle")
	{
		notification_description = "Данные командировки: изменилось название расхода с " + existing_value + " на " + new_value;
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + id + "\";";
	}
	if(action == "AJAX_updateExpenseTemplateTaxable")
	{
		notification_description = "Данные командировки: изменилась налогооблагаемость расхода с " + existing_value + " на " + new_value;
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + id + "\";";
	}
	if(action == "AJAX_updateExpenseTemplateAgencyComment")
	{
		notification_description = "Данные командировки: изменилось описание расхода (" + GetSpelledBTExpenseTemplateByID(id, db) + ") с " + existing_value + " на " + new_value;
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=\"" + id + "\";";
	}
	if(action == "AJAX_updateExpenseTemplateLineTitle")
	{
		notification_description = "Данные командировки: изменилось название отчетного документа с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineDescription")
	{
		notification_description = "Данные командировки: изменилось описание отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineTooltip")
	{
		notification_description = "Данные командировки: изменилась подсказка у отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineDomType")
	{
		notification_description = "Данные командировки: изменился тип отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLinePaymentCash")
	{
		notification_description = "Данные командировки: изменилась оплата у отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLinePaymentCard")
	{
		notification_description = "Данные командировки: изменилась оплата у отчетного документа (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_updateExpenseTemplateLineRequired")
	{
		notification_description = "Данные командировки: изменилось требование к отчетному документу (" + GetSpelledBTExpenseTemplateLineByID(id, db) + ") с " + existing_value + " на " + new_value + " у расхода " + GetSpelledBTExpenseTemplateByLineID(id, db);
		sql_query = "SELECT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `bt_expense_template_id`=(SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=\"" + id + "\");";
	}
	if(action == "AJAX_addBTExpenseTemplateAssignment")
	{
		notification_description = "Данные командировки: добавили возмещаемый расход (" + GetSpelledBTExpenseTemplateByAssignmentID(id, db) + ") к SoW(" + GetSpelledSoWByID(sow_id, db) + ")";
		sql_query = "SELECT DISTINCT `sow_id` AS `contract_sow_id` FROM `bt_sow_assignment` WHERE `id`=\"" + id + "\";";
	}

	MESSAGE_DEBUG("", "", "finish");

	return make_pair(notification_description, sql_query);
}

auto NotifySoWContractPartiesAboutChanges(string action_type_id, string sow_id, CMysql *db, CUser *user) -> string
{
	auto				error_message = "";

	MESSAGE_DEBUG("", "", "start (action_type_id=" + action_type_id + ", sow_id=" + sow_id + ")");

	if(sow_id.length())
	{
		if(action_type_id.length())
		{
			vector<string>		user_list;
			auto				affected = db->Query("SELECT `user_id` FROM `company_employees` where `company_id`=("
														"SELECT `agency_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\""
													")");
			for(int i = 0; i < affected; ++i)
			{
				user_list.push_back(db->Get(i, 0));
			}

			affected = db->Query("SELECT `admin_userID` FROM `company` where `id`=("
									"SELECT `subcontractor_company_id` FROM `contracts_sow` WHERE `id`=\"" + sow_id + "\""
								")");
			for(int i = 0; i < affected; ++i)
			{
				user_list.push_back(db->Get(i, 0));
			}

			for(auto &user_id: user_list)
			{
				if(db->InsertQuery(
									"INSERT INTO `users_notification` (`userId`, `title`, `actionTypeId`, `actionId`, `eventTimestamp`) VALUES "
									"(\"" + user_id + "\", \"\", \"" + action_type_id + "\", \"" + sow_id + "\", UNIX_TIMESTAMP())"
									))
				{
					// --- success
				}
				else
				{
					error_message = gettext("SQL syntax issue");
					MESSAGE_ERROR("", "", "fail to insert to db");
					break;
				}
			}
		}
		else
		{
			error_message = gettext("parameters incorrect");
			MESSAGE_ERROR("", "", "action_type_id is empty");
		}
	}
	else
	{
		error_message = gettext("parameters incorrect");
		MESSAGE_ERROR("", "", "sow_id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (error_message is " + error_message + ")");

	return error_message;	
}

bool GeneralNotifySoWContractPartiesAboutChanges(string action, string id, string sow_id, string existing_value, string new_value, CMysql *db, CUser *user)
{
	bool	result = false;

	MESSAGE_DEBUG("", "", "start (action=" + action + ")");

	if(id.length())
	{
		if(action.length())
		{
			// string		actionTypeId;
			auto		notification_description	= ""s;
			auto		sow_list_sql_query			= ""s;
			auto		agency_list_sql_query		= ""s;
			auto		actionTypeId				= ""s;

			// --- definition sow_list affected by action
			tie(notification_description, sow_list_sql_query) = GetNotificationDescriptionAndSoWQuery(action, id, sow_id, existing_value, new_value, db, user);
			
			if(notification_description.length())
			{
				vector<string>		sow_list, agency_list;
				int					affected = 0;

				if(sow_list_sql_query.length())
				{
					// --- collect list sow to notify
					affected = db->Query(sow_list_sql_query);

					for(int i = 0; i < affected; ++i) 
						sow_list.push_back(db->Get(i, "contract_sow_id"));

					if(sow_list.size())
					{
						// --- NOW() can't be used here because NOW()-type is datetime, but CURDATE()-type is date
						// --- for example SELECT NOW()=CURDATE() gives false, 
						// --- reason behind is SELECT "YYYY-MM-DD HH:MM:SS" NOT equal "YYYY-MM-DD" due to type inconsistency
						// --- in comparison opertions "datetime" can't be compared to "date"
						agency_list_sql_query = "SELECT DISTINCT `agency_company_id` FROM `contracts_sow` WHERE `id` IN (" + join(sow_list) + ") AND `start_date`<=CURDATE() AND `end_date`>=CURDATE();";
					}
					else
					{
						MESSAGE_DEBUG("", "", "no active sow at the moment, nobody should be notified.");
					}
				}
				else
				{
					// --- don't notify subcontractors, only agency
					agency_list_sql_query = "SELECT `company_id` AS `agency_company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";";
					MESSAGE_DEBUG("", "", "no subcontractors needs to be notified");
				}

				if(agency_list_sql_query.length())
				{
					affected = db->Query(agency_list_sql_query);

					for(int i = 0; i < affected; ++i) 
						agency_list.push_back(db->Get(i, "agency_company_id"));

					// --- define agency
					// --- ERROR, if more than one agency. Mean that they have shared CU/proj/task - FORBIDDEN
					if((user->GetType() == "agency") && (agency_list.size() > 1))
					{
						MESSAGE_ERROR("", "", "Two or more agencies are sharing entities (either Customer/Project/Task or BTexpense/line templates) (" + action + ":" + id + ") - FORBIDDEN");
					}

					// --- if agency must be notified but no SoW-s singed, then agency_list will be empty
					// --- to avoid ERROR-message in logs nad notify agency itself, below W/A applied
					if((user->GetType() == "agency") && (agency_list.empty())) agency_list.push_back(id);

					if(agency_list.size())
					{
						auto				initiator_company_id = ""s;
						vector<string>		persons_to_notify;
						string				notificator_company_id = "";

						if(user->GetType() == "agency")
						{
							actionTypeId = to_string(NOTIFICATION_GENERAL_FROM_COMPANY);
							if(db->Query("SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\";"))
							{
								notificator_company_id = db->Get(0, "company_id");
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to get company.id from `company_employees`.`user_id`(" + user->GetID() + ")");
							}
						}
						else if(user->GetType() == "subcontractor")
						{
							actionTypeId = to_string(NOTIFICATION_GENERAL_FROM_USER);
							if(db->Query("SELECT `id` FROM `company` WHERE `admin_userID`=\"" + user->GetID() + "\";"))
							{
								notificator_company_id = db->Get(0, "id");
							}
							else
							{
								MESSAGE_ERROR("", "", "fail to get company.id from `company`.`admin_userID`(" + user->GetID() + ")");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "fail to define notification initiator type(" + user->GetType() + ")");
						}

						if(notificator_company_id.length())
						{
							// --- gather agency employee list
							affected = db->Query("SELECT DISTINCT `user_id` FROM `company_employees` WHERE `company_id` IN (" + join(agency_list) + ");");
							for(int i = 0; i < affected; ++i)
								persons_to_notify.push_back(db->Get(i, "user_id"));
							
							// --- gather subcontractor list
							if(sow_list.size())
							{
								affected = db->Query("SELECT DISTINCT `admin_userID` FROM `company` WHERE `id` IN ("
												"SELECT DISTINCT `subcontractor_company_id` FROM `contracts_sow` WHERE `id` IN (" + join(sow_list) + ")"
											");");
								for(int i = 0; i < affected; ++i)
									persons_to_notify.push_back(db->Get(i, "admin_userID"));
							}
							else
							{
								MESSAGE_DEBUG("", "", "no subcontractors need to be notified");
							}

							if(persons_to_notify.size())
							{
								string		sql_query = "";

								for(auto &user_id : persons_to_notify)
								{
									if(sql_query.length()) sql_query += ",";
									sql_query += "(\"" + user_id + "\", \"" + notification_description + "\", \"" + actionTypeId + "\", \"" + notificator_company_id + "\", UNIX_TIMESTAMP())";
								}

								sql_query = "INSERT INTO `users_notification` (`userId`, `title`, `actionTypeId`, `actionId`, `eventTimestamp`) VALUES " + sql_query;

								if(db->InsertQuery(sql_query))
								{
									result = true;
								}
								else
								{
									MESSAGE_ERROR("", "", "fail to insert to notification.table");
								}
							}
							else
							{
								MESSAGE_DEBUG("", "", "users notification array is empty");
							}
						}
						else
						{
							MESSAGE_ERROR("", "", "notificator_company_id is empty");
						}
					}
					else
					{
						MESSAGE_ERROR("", "", "Contract entities list is empty (probably list is empty due to no signed SoW-s yet.)");
					}

				}
				else
				{
					MESSAGE_DEBUG("", "", "empty sql-query, nobody should be notified.");
				}

			}
			else
			{
				MESSAGE_ERROR("", "", "unknown action_type(" + action + ")");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "action is empty");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "id is empty");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + to_string(result) + ")");

	return result;	
}

string GetAgencyEmployeesInJSONFormat(string company_id, CMysql *db, CUser *user)
{
	string	result = "";
	int		affected = 0;
	string	sql_query;

	MESSAGE_DEBUG("", "", "start");

	if(company_id.length())
	{
		sql_query = 
				"SELECT"
					"`company_employees`.`id` as `id`,"
					"`users`.`id` as `user_id`,"
					"`users`.`name` as `name`,"
					"`users`.`nameLast` as `nameLast`,"
					"`company_employees`.`allowed_change_agency_data` as `allowed_change_agency_data`,"
					"`company_employees`.`allowed_change_sow` as `allowed_change_sow`,"
					"`company_position`.`id` as `company_position_id`,"
					"`company_position`.`title` as `company_position_title`"
				"FROM `company_employees`"
				"INNER JOIN `users` ON `users`.`id`=`company_employees`.`user_id`"
				"INNER JOIN `company_position` ON `company_position`.`id`=`company_employees`.`position_id`"
				"WHERE `company_employees`.`company_id`=\"" + company_id + "\";";

		affected = db->Query(sql_query);
		if(affected)
		{
			for(int i = 0; i < affected; ++i)
			{
				if(i) result += ",";
				result += 
					"{"
						"\"id\":\"" + string(db->Get(i, "id")) + "\","
						"\"allowed_change_agency_data\":\"" + db->Get(i, "allowed_change_agency_data") + "\","
						"\"allowed_change_sow\":\"" + db->Get(i, "allowed_change_sow") + "\","
						"\"user\":{"
							"\"id\":\"" + db->Get(i, "user_id") + "\","
							"\"name\":\"" + db->Get(i, "name") + "\","
							"\"nameLast\":\"" + db->Get(i, "nameLast") + "\""
						"},"
						"\"position\":{"
							"\"id\":\"" + db->Get(i, "company_position_id") + "\","
							"\"title\":\"" + db->Get(i, "company_position_title") + "\""
						"}"
					"}"
				;
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "no agency employees found");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "comapny_id is empty");
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

string GetAgencyObjectInJSONFormat(string agency_id, bool include_tasks, bool include_bt, CMysql *db, CUser *user)
{
	string	result = "";
	string	sql_query;

	MESSAGE_DEBUG("", "", "start (agency_id=" + agency_id + ")");

	if(agency_id.length())
	{
		// --- get short info
		result += "{"
						"\"companies\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + agency_id + "\";", db, user) + "]"
						"}";
	}
	else
	{
		// --- get full info about agency, if you are employee
		agency_id = GetAgencyID(user, db);
		if(agency_id.length())
		{
			result += "{"
							"\"companies\":[" + GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=\"" + agency_id + "\";", db, user, true, true) + "],"
							"\"tasks\":[" + ( include_tasks ?
												GetTimecardTasksInJSONFormat("SELECT * FROM `timecard_tasks` WHERE `timecard_projects_id` IN ("
																				"SELECT `id` FROM `timecard_projects` WHERE `timecard_customers_id` IN ("
																					"SELECT `id` FROM `timecard_customers` WHERE `agency_company_id`=\"" + agency_id + "\""
																				")"
																			");", db, user) : "") + "],"
							"\"cost_centers\":[" + ( include_tasks ? GetCostCentersInJSONFormat("SELECT * FROM `cost_centers` WHERE `agency_company_id`=\"" + agency_id + "\";", db, user) : "") + "],"
							"\"cost_center_assignment\":[" + ( include_tasks ? GetCostCentersAssignmentInJSONFormat("SELECT * FROM `cost_center_assignment` WHERE `timecard_customer_id` IN (SELECT `id` FROM `timecard_customers` WHERE `agency_company_id`=\"" + agency_id + "\");", db, user) : "") + "],"
							
							"\"bt_expense_templates\":[" + ( include_bt ? 
																			GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `agency_company_id`=\"" + agency_id + "\";", db, user)
																			 : "") + "]"
						"}";

		}
		else
		{
			MESSAGE_DEBUG("", "", "user.id(" + user->GetID() + ") is not an agency employee");
		}
	}
	
	MESSAGE_DEBUG("", "", "finish (result.length is " + to_string(result.length()) + ")");

	return result;	
}

auto GetNumberOfApprovedTimecardsThisMonth(CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		{
			struct tm	period_start, period_end;

			tie(period_start, period_end) = GetFirstAndLastDateOfThisMonth();

			if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecards` WHERE `status`=\"approved\" AND `period_end`>=\"" + PrintSQLDate(period_start) + "\"  AND `period_end`<=\"" + PrintSQLDate(period_end) + "\" AND `contract_sow_id` IN ("
								"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=("
									"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
								")"
							");"))
			{
				result = db->Get(0, "counter");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfApprovedTimecardsLastMonth(CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		{
			struct tm	period_start, period_end;

			tie(period_start, period_end) = GetFirstAndLastDateOfLastMonth();

			if(db->Query("SELECT COUNT(*) AS `counter` FROM `timecards` WHERE `status`=\"approved\" AND `period_end`>=\"" + PrintSQLDate(period_start) + "\"  AND `period_end`<=\"" + PrintSQLDate(period_end) + "\" AND `contract_sow_id` IN ("
								"SELECT `id` FROM `contracts_sow` WHERE `agency_company_id`=("
									"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
								")"
							");"))
			{
				result = db->Get(0, "counter");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfSoWActiveThisMonth(CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		{
			struct tm	period_start, period_end;

			tie(period_start, period_end) = GetFirstAndLastDateOfThisMonth();

			if(db->Query("SELECT COUNT(*) AS `counter` FROM `contracts_sow` WHERE `status`=\"signed\" AND `end_date`>=\"" + PrintSQLDate(period_start) + "\"  AND `start_date`<=\"" + PrintSQLDate(period_end) + "\" AND `agency_company_id`=("
									"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
								")"))
			{
				result = db->Get(0, "counter");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetNumberOfSoWActiveLastMonth(CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user->GetType() == "agency")
	{
		{
			struct tm	period_start, period_end;

			tie(period_start, period_end) = GetFirstAndLastDateOfLastMonth();

			if(db->Query("SELECT COUNT(*) AS `counter` FROM `contracts_sow` WHERE `status`=\"signed\" AND `end_date`>=\"" + PrintSQLDate(period_start) + "\"  AND `start_date`<=\"" + PrintSQLDate(period_end) + "\" AND `agency_company_id`=("
									"SELECT `company_id` FROM `company_employees` WHERE `user_id`=\"" + user->GetID() + "\""
								")"))
			{
				result = db->Get(0, "counter");
			}
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") have unknown type(" + user->GetType() + ")");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetPSoWIDByTimecardIDAndCostCenterID(string timecard_id, string cost_center_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(user->GetType() == "agency")
		{
			if(db)
			{
				if(db->Query(
					"SELECT `id` FROM `contracts_psow` WHERE `cost_center_id`=\"" + cost_center_id + "\" AND `contract_sow_id`=("
						"SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\""
					")"
				))
				{
					result = db->Get(0, "id");
				}
				else
				{
					MESSAGE_ERROR("", "", "can't define PSoW by timecard.id(" + timecard_id + ") and cost_center.id(" + cost_center_id + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "db not initialized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") is not an agency employee (" + user->GetType() + ")");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetPSoWIDByBTIDAndCostCenterID(string bt_id, string cost_center_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(user->GetType() == "agency")
		{
			if(db)
			{
				if(db->Query(
					"SELECT `id` FROM `contracts_psow` WHERE `cost_center_id`=\"" + cost_center_id + "\" AND `contract_sow_id`=("
						"SELECT `contract_sow_id` FROM `bt` WHERE `id`=\"" + bt_id + "\""
					")"
				))
				{
					result = db->Get(0, "id");
				}
				else
				{
					MESSAGE_ERROR("", "", "can't define PSoW by bt.id(" + bt_id + ") and cost_center.id(" + cost_center_id + ")");
				}
			}
			else
			{
				MESSAGE_ERROR("", "", "db not initialized");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "user.id(" + user->GetID() + ") is not an agency employee (" + user->GetType() + ")");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

auto GetSoWIDByTimecardID(string timecard_id, CMysql *db, CUser *user) -> string
{
	auto	result = ""s;

	MESSAGE_DEBUG("", "", "start");

	if(user)
	{
		if(db)
		{
			if(db->Query("SELECT `contract_sow_id` FROM `timecards` WHERE `id`=\"" + timecard_id + "\";"))
			{
				result = db->Get(0, "contract_sow_id");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "db not initialized");
		}
	}
	else
	{
		MESSAGE_ERROR("", "", "user not initialized");
	}

	MESSAGE_DEBUG("", "", "finish (result is " + result + ")");

	return result;
}

