#include "chtml.h"

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	CHTML	*obj = (CHTML *)userp;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + "(data len " + to_string(size * nmemb) + string(")[") + to_string(__LINE__) +  "]:start");
	}

	size_t				bufferLength = size * nmemb;
	unique_ptr<char[]>	tempSmartPointer(new char[bufferLength + 1]); // "+1" must host last '\0'
	char				*bufferToWrite = tempSmartPointer.get();

	memcpy(bufferToWrite, contents, bufferLength);
	bufferToWrite[bufferLength] = 0;

	obj->AppendToHTMLPage(bufferToWrite);

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end");
	}

	return bufferLength;
}

CHTML::CHTML()
{
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	// --- Repeated calls to curl_global_init and curl_global_cleanup should be avoided. They should only be called once each.
	curl_global_init(CURL_GLOBAL_ALL);

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end");
	}
}

CHTML::CHTML(string _type_): type(_type_)
{
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("(" + _type_ + ")[") + to_string(__LINE__) +  "]:start");
	}

	// --- Repeated calls to curl_global_init and curl_global_cleanup should be avoided. They should only be called once each.
	curl_global_init(CURL_GLOBAL_ALL);

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end");
	}
}

string CHTML::ExtractContentAttribute(string prefix, string postfix)
{
	string			 	result = "";
	string::size_type	openTagPos, closeTagPos;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("(prefix = ") + prefix + ", postfix = " + postfix + string(")[") + to_string(__LINE__) +  "]:start");
	}

	openTagPos = headTag.find(prefix);
	if(openTagPos != string::npos)
	{
		openTagPos += string(prefix).length();
		closeTagPos = headTag.find(postfix, openTagPos);
		if(closeTagPos != string::npos)
		{
			result = headTag.substr(openTagPos, closeTagPos - openTagPos);
		}
		else
		{
			MESSAGE_DEBUG("", "", "close part not found");
		}

	}
	else
	{
		MESSAGE_DEBUG("", "", "open part not found");
	}

	MESSAGE_DEBUG("", "", "end (return string with len()=" + to_string(result.length()) + ")");

	return result;
}

bool CHTML::ExtractTITLE()
{
	bool			 	result = true;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	metaTitle = ExtractContentAttribute("<title>", "</title>");
	if(metaTitle.length() == 0) metaTitle = ExtractContentAttribute("<meta name=\"title\" content=\"", "\"");
	if(metaTitle.length() == 0) metaTitle = ExtractContentAttribute("<meta name='title' content=\"", "\"");
	if(metaTitle.length() == 0) metaTitle = ExtractContentAttribute("<meta name=title content=\"", "\"");

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (result ? "true" : "false") + ")");
	}

	return result;
}

bool CHTML::isEmbedVideoHostedOnYoutube()
{
	bool			 	result = false;
	regex				regex1("https?://www.youtube.com/(.*)");
	regex				regex2("https?://youtu.be/(.*)");
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	result = (regex_match(embedVideoURL, regex1)) || (regex_match(embedVideoURL, regex2));

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (result ? "true" : "false") + ")");
	}

	return result;
}

bool CHTML::ExtractEmbedVideoURL()
{
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	embedVideoURL = ExtractContentAttribute("<meta property=\"og:video:url\" content=\"", "\"");

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (!embedVideoURL.empty() ? "true" : "false") + ")");
	}

	return !embedVideoURL.empty();
}

bool CHTML::ExtractPreviewImage()
{
	bool	result;
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	metaPreviewImageURL = ExtractContentAttribute("<meta property=\"og:image\" content=\"", "\"");
	result = !metaPreviewImageURL.empty();

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (result ? "true" : "false") + ")");
	}

	return result;
}

bool CHTML::ExtractDESCRIPTION()
{
	bool			 	result = true;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	metaDescription = ExtractContentAttribute("<meta name=\"description\" content=\"", "\"");
	if(metaDescription.length() == 0) metaDescription = ExtractContentAttribute("<meta name='description' content=\"", "\"");
	if(metaDescription.length() == 0) metaDescription = ExtractContentAttribute("<meta name=description content=\"", "\"");

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (result ? "true" : "false") + ")");
	}

	return result;
}

bool CHTML::ExtractHEAD()
{
	bool			 	result = false;
	string::size_type	openTagPos, closeTagPos;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	openTagPos = htmlPage.find("<head>");
	if(openTagPos == string::npos) openTagPos = htmlPage.find("< head >");
	if(openTagPos == string::npos) openTagPos = htmlPage.find("<HEAD>");
	if(openTagPos == string::npos) openTagPos = htmlPage.find("< HEAD >");
	if(openTagPos == string::npos) openTagPos = htmlPage.find("<head prefix"); // --- yahoo
	if(openTagPos != string::npos)
	{
		closeTagPos = htmlPage.find("</head>", openTagPos + 1);
		if(closeTagPos == string::npos) closeTagPos = htmlPage.find("< /head >", openTagPos + 1);
		if(closeTagPos == string::npos) closeTagPos = htmlPage.find("</HEAD>", openTagPos + 1);
		if(closeTagPos == string::npos) closeTagPos = htmlPage.find("< /HEAD >", openTagPos + 1);
		if(closeTagPos != string::npos)
		{
			headTag = htmlPage.substr(openTagPos, closeTagPos - openTagPos);

			result = true;
			htmlPage = ""; // --- garbage collection
		}
		else
		{
			{
				CLog	log;
				log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: close tag not found");
			}
		}

	}
	else
	{
		{
			CLog	log;
			log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: open tag not found");
		}
	}

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (result ? "true" : "false") + ")");
	}

	return result;
}

int CHTML::GetNumberOfFoldersByType()
{
	int		result = FEEDIMAGE_NUMBER_OF_FOLDERS;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:begin");
	}

	if(type == "feed") result = FEEDIMAGE_NUMBER_OF_FOLDERS;
	else if(type == "temp") result = TEMPIMAGE_NUMBER_OF_FOLDERS;
	else
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR:unknown type(" + type + ")");
	}

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:finish (result = " + to_string(result) + ")");
	}

	return result;
}

string CHTML::GetDirectoryByType()
{
	string		result = IMAGE_FEED_DIRECTORY;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:begin");
	}

	if(type == "feed") result = IMAGE_FEED_DIRECTORY;
	else if(type == "temp") result = IMAGE_TEMP_DIRECTORY;
	else
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR:unknown type(" + type + ")");
	}

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:finish (result = " + result + ")");
	}

	return result;
}

tuple<string, string, string> CHTML::PickFileName()
{
	string		filePrefix = "", fileExtention = "";
	int			folderID;
	string		finalFile, tmpFile2Check, tmpImageJPG;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:begin");
	}

	//--- check image file existing
	do
	{
		ostringstream	ost;
        string          tmp;
        std::size_t  	foundPos;

		folderID = (int)(rand()/(RAND_MAX + 1.0) * GetNumberOfFoldersByType()) + 1;
		filePrefix = GetRandom(20);
        tmp = metaPreviewImageURL;

        if((foundPos = tmp.rfind(".")) != string::npos)
        {
            fileExtention = tmp.substr(foundPos, tmp.length() - foundPos);
            
            // --- filter wrong fileExtension (for ex: .com?action=fake_action) 
            if(fileExtention.find("jpeg")) fileExtention = ".jpeg";
            else if(fileExtention.find("png")) fileExtention = ".png";
            else if(fileExtention.find("gif")) fileExtention = ".gif";
            else if(fileExtention.find("svg")) fileExtention = ".svg";
            else fileExtention = ".jpg";
        }
        else
        {
            fileExtention = ".jpg";
        }

		ost.str("");
		ost << GetDirectoryByType() << "/" << folderID << "/" << filePrefix << ".jpg";
        finalFile = ost.str();

        ost.str("");
        ost << "/tmp/tmp_" << filePrefix << fileExtention;
        tmpFile2Check = ost.str();

        ost.str("");
        ost << "/tmp/" << filePrefix << ".jpg";
        tmpImageJPG = ost.str();
	} while(isFileExists(finalFile) || isFileExists(tmpFile2Check) || isFileExists(tmpImageJPG));

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: folderID: " + to_string(folderID) + ", filePrefix:" + filePrefix + ", fileExtention:" + fileExtention + ")");
	}

	return make_tuple(to_string(folderID), filePrefix, fileExtention);
}

bool CHTML::DownloadFile(string urlPreview, FILE *f)
{
	bool		result = false;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: start");
	}

	curl = curl_easy_init();
	if(curl)
	{
		// --- specify URL to get
		curl_easy_setopt(curl, CURLOPT_URL, urlPreview.c_str());

		// --- send all data to this file
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(f));

		// --- enable HTTP redirects 3xx
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* some servers don't like requests that are made without a user-agent
		 field, so we provide one */
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		curlRes = curl_easy_perform(curl);
		if(curlRes == CURLE_OK)
		{
			result = true;
		}
		else
		{
			{
				CLog	log;
				log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR: curl_easy_perform() returned error (" + curl_easy_strerror(curlRes) + ")");
			}
			result = false;
		}

		curl_easy_cleanup(curl);
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR: curl_easy_init() returned null");
		}
		result = false;
	}

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: end (result: " + (result ? "true" : "false"));
	}

	return result;
}

bool CHTML::ParseHTMLPage()
{
	bool	result = true;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	if(ExtractHEAD())
	{
/*
		{
			CLog	log;
			log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: dump head " + headTag);
		}
*/
		if(ExtractTITLE())
		{

		}
		else
		{
			{
				CLog	log;
				log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: TITLE tag not found");
			}
		}

		if(ExtractDESCRIPTION())
		{

		}
		else
		{
			{
				CLog	log;
				log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: DESCRIPTION tag not found");
			}
		}

		if(ExtractEmbedVideoURL() && isEmbedVideoHostedOnYoutube())
		{
			// --- embed video hosted on youtube
			// --- no need to extract previewImage
		}
		else if (ExtractPreviewImage())
		{
			if(metaPreviewImageURL.length() > 10)
			{
				// download it
				FILE	*f;

				tie(folderID, filePrefix, fileExtention) = PickFileName();

	    		{
	    			CLog	log;
	    			ostringstream	ost;

	    			ost << "CHTML::" << __func__ << "[" << __LINE__ << "]: Save preview (" << metaPreviewImageURL << ") to /tmp for checking of image validity [" << "/tmp/tmp_" << filePrefix << fileExtention << "]";
	    			log.Write(DEBUG, ost.str());
	    		}

	            // --- Save file to "/tmp/" for checking of image validity
	            f = fopen(string("/tmp/tmp_" + filePrefix + fileExtention).c_str(), "w");
	            if(f != NULL)
	            {
	            	if(DownloadFile(metaPreviewImageURL, f))
	            	{
	            		// --- download succeed

	            	}
	            	else
	            	{

		                {
		                    CLog            log;
		                    log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: downloading file with curl");
		                }
		                metaPreviewImageURL = "";
		                folderID = "";
		                filePrefix = "";
		                fileExtention = "";
	            	}

		            fclose(f);
	            }
	            else
	            {
	                {
	                    CLog            log;
	                    log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) + "]:ERROR: writing file: /tmp/tmp_" + filePrefix + fileExtention);
	                }
	                metaPreviewImageURL = "";
	                folderID = "";
	                filePrefix = "";
	                fileExtention = "";
	            }
			}
		}
		else
		{
			{
				CLog	log;
				log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: (embed video, preview image) not found");
			}
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]: HEAD tag not found");
		}
	}

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return: " + (result ? "true" : "false") + ")");
	}

	return result;
}


bool CHTML::PerformRequest(string param)
{
	bool		result = false;
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	url = param;
	if(url.length())
	{
		ResetMetaValues();

		curl = curl_easy_init();
		if(curl)
		{
			// --- specify URL to get
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			// --- send all data to this function
			// curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bind(&CHTML::WriteMemoryCallback, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

			curl_easy_setopt(curl, CURLOPT_WRITEDATA, reinterpret_cast<void *>(this));

			// --- enable HTTP redirects 3xx
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			/* some servers don't like requests that are made without a user-agent
			 field, so we provide one */
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

			curlRes = curl_easy_perform(curl);
			if(curlRes == CURLE_OK)
			{
				// --- can't move it behind ParseHTMLPage, beacause ParseHTMLPage also calls culr_easy_cleanup
				curl_easy_cleanup(curl);


				if(isParsingRequired)
				{
					if(ParseHTMLPage())
					{
						result = true;
					}
					else
					{
						{
							CLog	log;
							log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR: page parsing fail");
						}
						result = false;
					}
				}
				else
				{
					// --- take downloaded result from GetContent() func
					result = true;
				}
			}
			else
			{
				{
					CLog	log;
					log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR: curl_easy_perform() returned error (" + curl_easy_strerror(curlRes) + ")");
				}
				result = false;

				curl_easy_cleanup(curl);
			}

		}
		else
		{
			{
				CLog	log;
				log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR: curl_easy_init() returned null");
			}
			result = false;
		}
	}
	else
	{
		{
			CLog	log;
			log.Write(ERROR, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:ERROR: url is empty");
		}
		result = false;
	}

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end (return:" + (result ? "true" : "false") + ")");
	}

	return result;
}

void CHTML::ResetMetaValues()
{
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	htmlPage = "";
	headTag = "";
	metaTitle = "";
	metaDescription = "";
	metaPreviewImageURL = "";
	folderID = "";
	filePrefix = "";
	fileExtention = "";

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end");
	}
}

void CHTML::AppendToHTMLPage(string param)
{
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	htmlPage += param;

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end");
	}
}

CHTML::~CHTML()
{
	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:start");
	}

	// --- Repeated calls to curl_global_init and curl_global_cleanup should be avoided. They should only be called once each.
	curl_global_cleanup();

	{
		CLog	log;
		log.Write(DEBUG, string("CHTML::") + string(__func__) + string("[") + to_string(__LINE__) +  "]:end");
	}
}
