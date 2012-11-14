/**********************************************************\

  Auto-generated btlauncherAPI.cpp

\**********************************************************/

#define UNICODE
#define _UNICODE

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"
#include "btlauncherAPI.h"
#include <map>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <CoreServices/CoreServices.h>


#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

#define BTLIVE_INFO_PATH "/BTLive.app/Contents/Info.plist"
#define BTLIVE_EXE_PATH "/BTLive.app/Contents/MacOS/BTLive"
#define BTLIVE_DOWNLOAD_URL "http://live-installer.s3.amazonaws.com/live.tar"

#define SOSHARE_INFO_PATH "/SoShare.app/Contents/Info.plist"
#define SOSHARE_EXE_PATH "/SoShare.app/Contents/MacOS/SoShare"
#define SOSHARE_DOWNLOAD_URL "http://download.utorrent.com/mac/SoShare.tar.gz"

#define TORQUE_INFO_PATH "/Torque.app/Contents/Info.plist"
#define TORQUE_EXE_PATH "/Torque.app/Contents/MacOS/Torque"
#define TORQUE_DOWNLOAD_URL "http://download.utorrent.com/mac/Torque.tar.gz"

#ifdef SHARE
#define PLUGIN_DL "http://torque.bittorrent.com/SoShare.pkg"
#endif //SHARE

#ifdef TORQUE
#define PLUGIN_DL "http://torque.bittorrent.com/Torque.pkg"
#endif //TORQUE

#define UNKNOWN_VERSION ""

int btlauncherAPI::GetBSDProcessList(kinfo_proc **procList, size_t *procCount)
// Returns a list of all BSD processes on the system.  This routine
// allocates the list and puts it in *procList and a count of the
// number of entries in *procCount.  You are responsible for freeing
// this list (use "free" from System framework).
// On success, the function returns 0.
// On error, the function returns a BSD errno value.
{
    int                 err;
    kinfo_proc *        result;
    bool                done;
    static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    // Declaring name as const requires us to cast it when passing it to
    // sysctl because the prototype doesn't include the const modifier.
    size_t              length;
	
    assert( procList != NULL);
    assert(*procList == NULL);
    assert(procCount != NULL);
	
    *procCount = 0;
	
    // We start by calling sysctl with result == NULL and length == 0.
    // That will succeed, and set length to the appropriate length.
    // We then allocate a buffer of that size and call sysctl again
    // with that buffer.  If that succeeds, we're done.  If that fails
    // with ENOMEM, we have to throw away our buffer and loop.  Note
    // that the loop causes use to call sysctl with NULL again; this
    // is necessary because the ENOMEM failure case sets length to
    // the amount of data returned, not the amount of data that
    // could have been returned.
	
    result = NULL;
    done = false;
    do {
        assert(result == NULL);
		
        // Call sysctl with a NULL buffer.
		
        length = 0;
        err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
					 NULL, &length,
					 NULL, 0);
        if (err == -1) {
            err = errno;
        }
		
        // Allocate an appropriately sized buffer based on the results
        // from the previous call.
		
        if (err == 0) {
            result = (kinfo_proc *)malloc(length);
            if (result == NULL) {
                err = ENOMEM;
            }
        }
		
        // Call sysctl again with the new buffer.  If we get an ENOMEM
        // error, toss away our buffer and start again.
		
        if (err == 0) {
            err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
						 result, &length,
						 NULL, 0);
            if (err == -1) {
                err = errno;
            }
            if (err == 0) {
                done = true;
            } else if (err == ENOMEM) {
                assert(result != NULL);
                free(result);
                result = NULL;
                err = 0;
            }
        }
    } while (err == 0 && ! done);
	
    // Clean up and establish post conditions.
	
    if (err != 0 && result != NULL) {
        free(result);
        result = NULL;
    }
    *procList = result;
    if (err == 0) {
        *procCount = length / sizeof(kinfo_proc);
    }
	
    assert( (err == 0) == (*procList != NULL) );
	
    return err;
}

static void child_handler(int sig)
{
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @fn btlauncherAPI::btlauncherAPI(const btlauncherPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
btlauncherAPI::btlauncherAPI(const btlauncherPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{
	FBLOG_INFO("btlauncherAPI()", "START");
	registerMethod("getInstallPath", make_method(this, &btlauncherAPI::getInstallPath));
	registerMethod("getInstallVersion", make_method(this, &btlauncherAPI::getInstallVersion));
	registerMethod("isRunning", make_method(this, &btlauncherAPI::isRunning));
	registerMethod("stopRunning", make_method(this, &btlauncherAPI::stopRunning));
	registerMethod("runProgram", make_method(this, &btlauncherAPI::runProgram));
	registerMethod("downloadProgram", make_method(this, &btlauncherAPI::downloadProgram));
	registerMethod("checkForUpdate", make_method(this, &btlauncherAPI::checkForUpdate));
	registerMethod("ajax", make_method(this, &btlauncherAPI::ajax));
    registerProperty("version", make_property(this, &btlauncherAPI::get_version));
	
	FSRef ref;
    OSType folderType = kApplicationSupportFolderType;
    char path[PATH_MAX];
	
    FSFindFolder( kUserDomain, folderType, kCreateFolder, &ref );
    FSRefMakePath( &ref, (UInt8*)&path, PATH_MAX );
	
	this->installPath = string(path);
    
	/* Establish handler to clean up child processes otherwise Live will live on as a zombie! */
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = child_handler;
	sigaction(SIGCHLD, &sa, NULL);
	FBLOG_INFO("btlauncherAPI()", "END");
}

///////////////////////////////////////////////////////////////////////////////
/// @fn btlauncherAPI::~btlauncherAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
btlauncherAPI::~btlauncherAPI()
{
	FBLOG_INFO("~btlauncherAPI()", "START");
	FBLOG_INFO("~btlauncherAPI()", "END");
}

///////////////////////////////////////////////////////////////////////////////
/// @fn btlauncherPtr btlauncherAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
btlauncherPtr btlauncherAPI::getPlugin()
{
	FBLOG_INFO("getPlugin()", "START");
    btlauncherPtr plugin(m_plugin.lock());
    if (!plugin) {
		FBLOG_INFO("getPlugin()", "ERROR");
        throw FB::script_error("The plugin is invalid");
    }
	FBLOG_INFO("getPlugin()", "END");
    return plugin;
}


// Read-only property version
std::string btlauncherAPI::get_version()
{
	FBLOG_INFO("getPlugin()", "START");
	FBLOG_INFO("getPlugin()", FBSTRING_PLUGIN_VERSION);
	FBLOG_INFO("getPlugin()", "END");
    return FBSTRING_PLUGIN_VERSION;
}

#define bufsz 2048
void btlauncherAPI::gotDownloadProgram(const FB::JSObjectPtr& callback, 
									   std::string& program,
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size) {
	FBLOG_INFO("gotDownloadProgram()", "START");
	FBLOG_INFO("gotDownloadProgram()", program.c_str());
	

	char *tmpname = strdup("/tmp/btlauncherXXXXXX");
	mkstemp(tmpname);
	ofstream f(tmpname);
	
	if (f.fail()) {
		FBLOG_INFO("gotDownloadProgram()", "f.fail");
		callback->InvokeAsync("", FB::variant_list_of(false)(-1));
		return;
	}
	f.write((char *)data.get(), size);
	f.close();
	
	std::string url;
	if (program == "SoShare")
		url = SOSHARE_DOWNLOAD_URL;
	else if (program == "Torque")
		url = TORQUE_DOWNLOAD_URL;
	else
		url = BTLIVE_DOWNLOAD_URL;
	
	FBLOG_INFO("gotDownloadProgram()", url.c_str());
	
	const char *tarFlags = "-xf";
	if (url.find(".gz") != std::string::npos)
		tarFlags = "-xzf";
	
	pid_t tarPid;
	int status;
	switch(tarPid = fork()) 
	{
		case -1:
			FBLOG_INFO("gotDownloadProgram()", "fork failed");
			break;
		case 0:
			FBLOG_INFO("gotDownloadProgram()", "running tar");
			execl("/usr/bin/tar", "tar", tarFlags, tmpname, "-C", this->installPath.c_str(), NULL);
			break;
		default:
			FBLOG_INFO("gotDownloadProgram()", "waitpid");
			waitpid(tarPid, &status, 0);
			break;
	}
	
	runProgram(program, callback);
	FBLOG_INFO("gotDownloadProgram()", "END");
}

void btlauncherAPI::downloadProgram(const std::string& program, const FB::JSObjectPtr& callback) {
	FBLOG_INFO("downloadProgram()", "START");
	std::string url;
	if (program == "SoShare")
		url = SOSHARE_DOWNLOAD_URL;
	else if (program == "Torque")
		url = TORQUE_DOWNLOAD_URL;
	else
		url = BTLIVE_DOWNLOAD_URL;

	FBLOG_INFO("downloadProgram()", url.c_str());
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotDownloadProgram, this, callback, program, _1, _2, _3, _4)
		);
	FBLOG_INFO("downloadProgram()", "END");
}

std::string btlauncherAPI::getInstallVersion(const std::string& program) {
	FBLOG_INFO("getInstallVersion()", "START");
	FBLOG_INFO("getInstallVersion()", program.c_str());
	xmlDocPtr doc;
	xmlNodePtr cur;
	
	const char *infoPath;
	if (program == "SoShare")
		infoPath = SOSHARE_INFO_PATH;
	else if (program == "Torque")
		infoPath = TORQUE_INFO_PATH;
	else
		infoPath = BTLIVE_INFO_PATH;
	FBLOG_INFO("getInstallVersion()", infoPath);
	
	string appInfo = this->installPath + string(infoPath);
	FBLOG_INFO("getInstallVersion()", appInfo.c_str());
	doc = xmlParseFile(appInfo.c_str());
	
	if(!doc) {
		FBLOG_INFO("getInstallVersion()", "unknown version -> no info file");
		FBLOG_INFO("getInstallVersion()", "END");
		return std::string(UNKNOWN_VERSION);
	}
	
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		xmlFreeDoc(doc);
		FBLOG_INFO("getInstallVersion()", "unknown version -> no root element");
		FBLOG_INFO("getInstallVersion()", "END");
		return std::string(UNKNOWN_VERSION);
	}
	
	xmlXPathContextPtr context = xmlXPathNewContext(doc);
	xmlChar * xpath = (xmlChar*)"/plist/dict/key[.='CFBundleShortVersionString']/following-sibling::*[1]";
	xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
	
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlFreeDoc(doc);
		FBLOG_INFO("getInstallVersion()", "unknown version -> node set is empty");
		FBLOG_INFO("getInstallVersion()", "END");
        return std::string(UNKNOWN_VERSION);
	}
	
	xmlNodeSetPtr nodeSetPtr = result->nodesetval;
	xmlNodePtr curNode;
	curNode = nodeSetPtr->nodeTab[0];
	std::string version((char *)curNode->children[0].content);
	xmlFreeDoc(doc);

	FBLOG_INFO("getInstallVersion()", version.c_str());
	FBLOG_INFO("getInstallVersion()", "END");
	return version;
}

std::string btlauncherAPI::getInstallPath(const std::string& program) {
	FBLOG_INFO("getInstallPath()", "START");
	FBLOG_INFO("getInstallPath()", program.c_str());
	FBLOG_INFO("getInstallPath()", this->installPath.c_str());
	FBLOG_INFO("getInstallPath()", "END");
	return this->installPath;
}

FB::variant btlauncherAPI::runProgram(const std::string& program, const FB::JSObjectPtr& callback) {
	FBLOG_INFO("runProgram()", "START");
	string exe = this->installPath;
	if (program == "SoShare")
		exe += SOSHARE_EXE_PATH;
	else if (program == "Torque")
		exe += TORQUE_EXE_PATH;
	else
		exe += BTLIVE_EXE_PATH;
		
	FBLOG_INFO("runProgram()", exe.c_str());

	struct stat st;
	if (stat(exe.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) {
		FBLOG_INFO("runProgram()", "stat check problem");
		callback->InvokeAsync("", FB::variant_list_of(false)(0));
		FBLOG_INFO("runProgram()", "END");
		return 0;
	}
	
	FB::VariantList list = isRunning(program);
	if (list.empty()) {
		switch(fork()) 
		{
			case -1: {
				perror("BTLauncher Run Program Fork");
				FBLOG_INFO("runProgram()", "fork - failure");
				break;
			}
			case 0: {
				FBLOG_INFO("runProgram()", "fork - child process");
				FBLOG_INFO("runProgram()", exe.c_str());
				execlp(exe.c_str(), exe.c_str(), NULL);
				FBLOG_INFO("runProgram()", "child process exit");
				exit(1);
			}
			default: {
				break;
			}
		}
	}
	callback->InvokeAsync("", FB::variant_list_of(true)(1));
	FBLOG_INFO("runProgram()", "END");
	return 0;
}

FB::VariantList btlauncherAPI::stopRunning(const std::string& val) {
	FBLOG_INFO("stopRunning()", "START");
	FBLOG_INFO("stopRunning()", val.c_str());
	FB::VariantList list;
    bool foundIt = false;

    size_t procCount = 0;
    kinfo_proc *procList = NULL;
    GetBSDProcessList(&procList, &procCount);
    size_t i;
    for (i = 0; i < procCount; i++) {
        if (!strcmp(procList[i].kp_proc.p_comm, val.c_str())) {
			FBLOG_INFO("stopRunning()", val.c_str());
            kill(procList[i].kp_proc.p_pid, SIGKILL);
            foundIt = true;
        }
    }

	free(procList);
	
    if (foundIt) {
        list.push_back("ok");
	} else {
        list.push_back("could not open process");
	}
	FBLOG_INFO("stopRunning()", "END");
    return list;
}


FB::VariantList btlauncherAPI::isRunning(const std::string& val) {
	FBLOG_INFO("isRunning()", "START");
	FB::VariantList list;
    size_t procCount = 0;
	kinfo_proc *procList = NULL;
	GetBSDProcessList(&procList, &procCount);
	size_t i;
	for (i = 0; i < procCount; i++) {
		if (!strcmp(procList[i].kp_proc.p_comm, val.c_str())) {
			FBLOG_INFO("isRunning()", val.c_str());
			list.push_back(procList[i].kp_proc.p_comm);
		}
	}
	
	free(procList);
	
	FBLOG_INFO("isRunning()", "END");
	return list;
}

void btlauncherAPI::gotCheckForUpdate(const FB::JSObjectPtr& callback,
                                   bool success,
                                   const FB::HeaderMap& header,
                                   const boost::shared_array<uint8_t>& data,
                                   const size_t size) {
	FBLOG_INFO("gotCheckForUpdate()", "START");
    
	char *tmpname = strdup("/tmp/btlauncherXXXXXX.pkg");
	mkstemp(tmpname);
	ofstream f(tmpname);
	
	if (f.fail()) {
		FBLOG_INFO("gotCheckForUpdate()", "f.fail");
		callback->InvokeAsync("", FB::variant_list_of(false)(-1));
		return;
	}
	f.write((char *)data.get(), size);
	f.close();
    
    switch(fork())
    {
        case -1: {
            FBLOG_INFO("gotCheckForUpdate()", "fork - failure");
            break;
        }
        case 0: {
            FBLOG_INFO("gotCheckForUpdate()", "fork - child process");
            FBLOG_INFO("gotCheckForUpdate()", tmpname);
            execlp("installer", "installer", "-pkg", tmpname, "-target", "CurrentUserHomeDirectory", NULL);
            FBLOG_INFO("gotCheckForUpdate()", "child process exit");
            exit(1);
        }
        default: {
            FBLOG_INFO("gotCheckForUpdate()", "fork - parent process");
            break;
        }
    }
    
    callback->InvokeAsync("", FB::variant_list_of(true)(1));
	FBLOG_INFO("gotCheckForUpdate()", "END");
}

void btlauncherAPI::checkForUpdate(const FB::JSObjectPtr& callback) {
    std::string url = std::string(PLUGIN_DL);
    url.append( std::string("?v=") );
    url.append( std::string(FBSTRING_PLUGIN_VERSION) );
    
    FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url),
                                     boost::bind(&btlauncherAPI::gotCheckForUpdate, this, callback, _1, _2, _3, _4), false);
}

void btlauncherAPI::ajax(const std::string& url, const FB::JSObjectPtr& callback) {
	FBLOG_INFO("ajax()", "START");
	FBLOG_INFO("ajax()", url.c_str());
	if (FB::URI::fromString(url).domain != "127.0.0.1") {
		FB::VariantMap response;
		response["allowed"] = false;
		response["success"] = false;
		callback->InvokeAsync("", FB::variant_list_of(response));
		return;
	}
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotajax, this, callback, _1, _2, _3, _4)
		);
	FBLOG_INFO("ajax()", "END");
}

void btlauncherAPI::gotajax(const FB::JSObjectPtr& callback, 
							bool success,
						    const FB::HeaderMap& headers,
						    const boost::shared_array<uint8_t>& data,
						    const size_t size) {
	FBLOG_INFO("gotajax()", "START");

	FB::VariantMap response;
	response["allowed"] = true;
	response["success"] = success;
	
	if(!success) {
		FBLOG_INFO("gotajax()", "unsuccessful");
		callback->InvokeAsync("", FB::variant_list_of(response));
		FBLOG_INFO("gotajax()", "END");
		return;
	}
	
	FB::VariantMap outHeaders;
	for (FB::HeaderMap::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        if (headers.count(it->first) > 1) {
            if (outHeaders.find(it->first) != outHeaders.end()) {
                outHeaders[it->first].cast<FB::VariantList>().push_back(it->second);
            } else {
                outHeaders[it->first] = FB::VariantList(FB::variant_list_of(it->second));
            }
        } else {
            outHeaders[it->first] = it->second;
        }
    }
	response["headers"] = outHeaders;
	response["size"] = size;
	std::string result = std::string((const char*) data.get(), size);
	response["data"] = result;
	
	callback->InvokeAsync("", FB::variant_list_of(response));
	FBLOG_INFO("gotajax()", "END");
}
