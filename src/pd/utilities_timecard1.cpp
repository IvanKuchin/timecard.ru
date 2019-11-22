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
		string	act_number_prefix;
		string	act_number;
		string	act_number_postfix;
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

				company.id					= db->Get(i, "id");
				company.type				= db->Get(i, "type");
				company.name				= db->Get(i, "name");
				company.legal_geo_zip_id	= db->Get(i, "legal_geo_zip_id");
				company.legal_address		= db->Get(i, "legal_address");
				company.mailing_geo_zip_id	= db->Get(i, "mailing_geo_zip_id");
				company.mailing_address		= db->Get(i, "mailing_address");
				company.tin					= db->Get(i, "tin");
				company.bank_id				= db->Get(i, "bank_id");
				company.account				= db->Get(i, "account");
				company.kpp					= db->Get(i, "kpp");
				company.ogrn				= db->Get(i, "ogrn");
				company.vat					= db->Get(i, "vat");
				company.link				= db->Get(i, "link");
				company.act_number_prefix	= db->Get(i, "act_number_prefix");
				company.act_number			= db->Get(i, "act_number");
				company.act_number_postfix	= db->Get(i, "act_number_postfix");
				company.admin_userID		= db->Get(i, "admin_userID");
				company.foundationDate		= db->Get(i, "foundationDate");
				company.numberOfEmployee	= db->Get(i, "numberOfEmployee");
				company.webSite				= db->Get(i, "webSite");
				company.description			= db->Get(i, "description");
				company.logo_folder			= db->Get(i, "logo_folder");
				company.logo_filename		= db->Get(i, "logo_filename");
				company.employedUsersList	= "";
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
					ostFinal << "\"act_number_prefix\": \""	<< companiesList[i].act_number_prefix << "\", ";
					ostFinal << "\"act_number\": \""		<< companiesList[i].act_number << "\", ";
					ostFinal << "\"act_number_postfix\": \""<< companiesList[i].act_number_postfix << "\", ";
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

	if(typeID == NOTIFICATION_AGENGY_ABOUT_APPROVER_REGISTRATION)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			if(db->Query("select * from `users` where `id`='" + id + "';"))
			{
				ostResult << "\"notificationFriendUserID\":\"" << db->Get(0, "id") << "\",";
				ostResult << "\"notificationFriendUserName\":\"" << db->Get(0, "name") << "\",";
				ostResult << "\"notificationFriendUserNameLast\":\"" << db->Get(0, "nameLast") << "\",";
				ostResult << "\"item\":[]";
			}
			else
			{
				MESSAGE_ERROR("", "", "user.id(" + id + ") not found");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": approver user_id is empty");
		}
	}

	if(typeID == NOTIFICATION_AGENGY_ABOUT_EMPLOYEE_REGISTRATION)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			if(db->Query("select * from `users` where `id`='" + id + "';"))
			{
				ostResult << "\"notificationFriendUserID\":\"" << db->Get(0, "id") << "\",";
				ostResult << "\"notificationFriendUserName\":\"" << db->Get(0, "name") << "\",";
				ostResult << "\"notificationFriendUserNameLast\":\"" << db->Get(0, "nameLast") << "\",";
				ostResult << "\"item\":[]";
			}
			else
			{
				MESSAGE_ERROR("", "", "user.id(" + id + ") not found");
			}
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": employee user_id is empty");
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

	if(typeID == NOTIFICATION_SUBC_BUILT_ABSENCE_REQUEST)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			ostResult << "\"item\":[" << GetAbsenceListInJSONFormat("SELECT * FROM `absence` WHERE `id`=" + quoted(id) + ";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=(SELECT `company_id` FROM `absence` WHERE `id`=" + quoted(id) + ");", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": subcontractor company_id is empty");
		}
	}
	if(typeID == NOTIFICATION_SUBC_CHANGED_ABSENCE_REQUEST)
	{
		string		id = to_string(actionID);

		if(id.length())
		{
			ostResult << "\"item\":[" << GetAbsenceListInJSONFormat("SELECT * FROM `absence` WHERE `id`=" + quoted(id) + ";", db, user) << "],";
			ostResult << "\"notificationFromCompany\":[" << GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `id`=(SELECT `company_id` FROM `absence` WHERE `id`=" + quoted(id) + ");", db, NULL) << "]";
		}
		else
		{
			MESSAGE_ERROR("", "", "typeID=" + to_string(typeID) + ": subcontractor company_id is empty");
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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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
		MESSAGE_DEBUG("", "", "bt_expenses are not assigned (" + sqlQuery + ")");
	}

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

string	GetApproversInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool include_sow)
{
	struct ItemClass
	{
		string	id;
		string	approver_user_id;
		string	contract_sow_id;
		string	approver_order;
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
			item.approver_user_id	= db->Get(i, "approver_user_id");
			item.contract_sow_id	= db->Get(i, "contract_sow_id");
			item.approver_order		= db->Get(i, "approver_order");
			item.auto_approve		= db->Get(i, "auto_approve");
			item.type				= db->Get(i, "type");
			item.rate				= db->Get(i, "rate");

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
			result += "\"approver_order\":\"" + item.approver_order + "\",";
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
		string				payed_date;
		string				invoice_filename;
		string				eventTimestamp;
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
			item.period_start = db->Get(i, "period_start");
			item.period_end = db->Get(i, "period_end");
			item.submit_date = db->Get(i, "submit_date");
			item.approve_date = db->Get(i, "approve_date");
			item.payed_date = db->Get(i, "payed_date");
			item.invoice_filename = db->Get(i, "invoice_filename");
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
			result += "\"payed_date\":\"" + item.payed_date + "\",";
			result += "\"invoice_filename\":\"" + item.invoice_filename + "\",";
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

	MESSAGE_DEBUG("", "", "finish");

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
		string	payed_date;
		string	invoice_filename;
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
			item.payed_date = db->Get(i, "payed_date");
			item.invoice_filename = db->Get(i, "invoice_filename");
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
			result += "\"payed_date\":\"" + item.payed_date + "\",";
			result += "\"invoice_filename\":\"" + item.invoice_filename + "\",";
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

	MESSAGE_DEBUG("", "", "finish");

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
		MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

	return result;
}

auto	GetSOWInJSONFormat(string sqlQuery, CMysql *db, CUser *user, bool include_tasks, bool include_bt, bool include_cost_centers, bool include_subc_company) -> string
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
		string	status;
		string	agreement_filename;
		string	eventTimestamp;
	};
	vector<ItemClass>		itemsList;

	MESSAGE_DEBUG("", "", "start (include_bt = " + to_string(include_bt) + ", include_tasks = " + to_string(include_tasks) + ", include_cost_centers = " + to_string(include_cost_centers) + ", include_subc_company = " + to_string(include_subc_company) + ")");

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
			result += "\"subcontractor_company\":[" + (include_subc_company ? GetCompanyListInJSONFormat("SELECT * FROM `company` WHERE `isBlocked`='N' AND `id`=\"" + item.subcontractor_company_id + "\";", db, user) : "") + "],";
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
			result += "\"bt_expense_templates\":[";
			if(include_bt)
				result += GetBTExpenseTemplatesInJSONFormat("SELECT * FROM `bt_expense_templates` WHERE `id` IN (SELECT `bt_expense_template_id` FROM `bt_sow_assignment` WHERE `sow_id`=\"" + item.id + "\");", db, user);
			result +=  "],";

			result += "\"tasks\":[";
			if(include_tasks)
				result += GetTimecardTasksInJSONFormat(
					"SELECT * FROM `timecard_tasks` WHERE "
							"`id` IN (SELECT `timecard_tasks_id` FROM `timecard_task_assignment` WHERE `contract_sow_id`=\"" + item.id + "\" "
									");", db, user);
			result +=  "],";

			result += "\"bt_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `bt_approvers` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
			result += "\"timecard_approvers\":[" + GetApproversInJSONFormat("SELECT * FROM `timecard_approvers` WHERE `contract_sow_id`=\"" + item.id + "\";", db, user, DO_NOT_INCLUDE_SOW_INFO) + "],";
			result += "\"subcontractor_create_tasks\":\"" + item.subcontractor_create_tasks + "\",";
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
		MESSAGE_DEBUG("", "", "user (" + user->GetID() + ") timecard is empty");
	}

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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
		MESSAGE_DEBUG("", "", "no bt items assigned");
	}

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

			result		+= "\"id\":\"" + item.id + "\",";
			result		+= "\"bt_expense_template_id\":\"" + item.bt_expense_template_id + "\",";
			result		+= "\"dom_type\":\"" + item.dom_type + "\",";
			if(item.dom_type == "allowance")
				result	+= "\"allowances\":[" + GetBTAllowanceInJSONFormat(
													"SELECT * FROM `bt_allowance` WHERE `agency_company_id`=("
														"SELECT `agency_company_id` FROM `bt_expense_templates` WHERE `id`=("
															"SELECT `bt_expense_template_id` FROM `bt_expense_line_templates` WHERE `id`=" + quoted(item.id) + 
														")"
													");", db, user) + "],";

			result		+= "\"title\":\"" + item.title + "\",";
			result		+= "\"description\":\"" + item.description + "\",";
			result		+= "\"tooltip\":\"" + item.tooltip + "\",";
			result		+= "\"payment\":\"" + item.payment + "\",";
			result		+= "\"required\":\"" + item.required + "\"";

			result +=	"}";
		}
	}
	else
	{
		{
			MESSAGE_DEBUG("", "", "no bt line templates assigned");
		}
	}

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "finish");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

	if(customer.length() && (customer.find("ыберите") == string::npos))
	{
		if(project.length() && (project.find("ыберите") == string::npos))
		{
			if(task.length() && (task.find("ыберите") == string::npos))
			{
				if(agency_id.length())
				{
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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

	MESSAGE_DEBUG("", "", "start");

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

