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
#define LATEST_LIVE_VERSION 43107

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
    registerMethod("echo",      make_method(this, &btlauncherAPI::echo));
    registerMethod("testEvent", make_method(this, &btlauncherAPI::testEvent));
	registerMethod("getInstallPath", make_method(this, &btlauncherAPI::getInstallPath));
	registerMethod("getInstallVersion", make_method(this, &btlauncherAPI::getInstallVersion));
	registerMethod("isRunning", make_method(this, &btlauncherAPI::isRunning));
	registerMethod("stopRunning", make_method(this, &btlauncherAPI::stopRunning));
	registerMethod("runProgram", make_method(this, &btlauncherAPI::runProgram));
	registerMethod("downloadProgram", make_method(this, &btlauncherAPI::downloadProgram));
    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &btlauncherAPI::get_version));
	
	this->m_live_pid = NULL;
	
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
    btlauncherPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}


// Read-only property version
std::string btlauncherAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}

// Method echo
FB::variant btlauncherAPI::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo(msg, n++);
    return msg;
}

void btlauncherAPI::testEvent(const FB::variant& var)
{
    fire_fired(var, true, 1);
}


#define bufsz 2048
void btlauncherAPI::gotDownloadProgram(const FB::JSObjectPtr& callback, 
									   std::string& program,
									   std::string& version,
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size) {
	printf("\nIn GotDownloadProgram\n");
	char *tmpname = strdup("/tmp/btliveXXXXXX");
	mkstemp(tmpname);
	ofstream f(tmpname);
	
	if (f.fail()) {
		callback->InvokeAsync("", FB::variant_list_of(false)(-1));
		return;
	}
	
	f.write((char *)data.get(), size);
	
	f.close();
	
	pid_t tarPid;
	switch(tarPid = fork()) 
	{
		case -1:
			perror("BTLauncher Tar Fork");
			exit(1);
			break;
		case 0:
			execl("/usr/bin/tar", "tar", "-xf", tmpname, "-C", this->installPath.c_str(), NULL);
			break;
		default:
			break;
	}
}


void btlauncherAPI::downloadProgram(const std::string& program, const std::string& version, const FB::JSObjectPtr& callback) {

	std::string url;
/*
	if (program == "uTorrent") {
		if (version.length() > 0) {
			url = std::string("http://download.utorrent.com/");
			url.append( version.c_str() );
			url.append( "/utorrent.exe" );
		} else {
			url = std::string(UT_DL);
		}
	} else {
		url = std::string(BT_DL);
	}
	*/
	url = std::string("http://live-installer.s3.amazonaws.com/live.tar");
		
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotDownloadProgram, this, callback, program, version, _1, _2, _3, _4)
		);
	
}

std::string btlauncherAPI::getInstallVersion(const std::string& program) {	
	/*
	std::string reg_group = std::string(INSTALL_REG_PATH).append( program );
	return getRegStringValue( reg_group, _T("DisplayVersion") );
	*/
	return "LATEST_LIVE_VERSION";
}
std::string btlauncherAPI::getInstallPath(const std::string& program) {	
	/*
	std::string reg_group = std::string(INSTALL_REG_PATH).append( program );
	return getRegStringValue( reg_group, _T("InstallLocation") );
	*/
	return this->installPath;
}


int btlauncherAPI::isInstalledAndUpToDate() {
	xmlDocPtr doc;
	xmlNodePtr cur;
	
	string liveInfo = this->installPath + string(BTLIVE_INFO_PATH);
	
	doc = xmlParseFile(liveInfo.c_str());
	
	if(!doc) {
		return 0;
	}
	
	printf("Live exists on OSX");
	
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr,"Live Info.Plist is an empty document\n");
		xmlFreeDoc(doc);
		return 0;
	}
	
	xmlXPathContextPtr context = xmlXPathNewContext(doc);
	xmlChar * xpath = (xmlChar*)"/plist/dict/key[.='CFBundleShortVersionString']/following-sibling::*[1]";
	xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
	
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		printf("Live has no xpath result\n");
		return 0;
	}
	
	xmlNodeSetPtr nodeSetPtr = result->nodesetval;
	xmlNodePtr curNode;
	curNode = nodeSetPtr->nodeTab[0];
	printf("\nXPATH Node child content: %s", (char *)curNode->children[0].content);
	
	string liveVersion((char *)curNode->children[0].content);
	boost::algorithm::erase_all(liveVersion,".");
	
	int liveVersionInt = 0;
	
	try {
		liveVersionInt = boost::lexical_cast<int>( liveVersion.c_str() );
	} catch( boost::bad_lexical_cast const& ) {
		printf("Live could not convert version number to int");
		return 0;
	}
	
	printf("\nLive Version read is %d\n", liveVersionInt);
	if (liveVersionInt < LATEST_LIVE_VERSION) {
		printf("Live is out of date.");
		return 0;
	}
	
	return 1;
}


FB::variant btlauncherAPI::runProgram(const std::string& program, const FB::JSObjectPtr& callback) {
	printf("in runProgram");
	if (this->isInstalledAndUpToDate()) {
		if(!this->isLiveRunning()) {
			
			switch(this->m_live_pid = fork()) 
			{
				case -1: {
					perror("BTLauncher Run Program Fork");
					exit(1);
					break;
				}
				case 0: {
					string liveExe = this->installPath + string(BTLIVE_EXE_PATH);
					printf("Opening %s\n", liveExe.c_str());
					execlp(liveExe.c_str(), liveExe.c_str(), NULL);
					break;
				}
				default: {
					printf("Running Live in child process. %d\n", this->m_live_pid);
					break;
				}
			}
		}
	}
	else {
		this->downloadProgram(program, string(""), callback);
		//downloadProgram(const std::string& program, const std::string& version, const FB::JSObjectPtr& callback)
	}
	return 0;
}

FB::VariantList btlauncherAPI::stopRunning(const std::string& val) {
	FB::VariantList list;
	if (this->isLiveRunning()) {
		printf("Its running, killing Live\n");
		//This dosent work for now because pyinstaller spawns a subprocess that just gets orphaned when the parent is killed
		//So Im just going to system call killall right now
		/*
		int ret;
		ret = killpg(this->m_live_pid, SIGTERM);
		list.push_back(ret);
		this->m_live_pid = 0;
		printf("Sent kill\n");
		*/
		system("killall -9 BTLive");
	}
	
	list.push_back(-1);
	return list;
}

int btlauncherAPI::isLiveRunning() {
	size_t procCount = 0;
	kinfo_proc *procList = NULL;
	GetBSDProcessList(&procList, &procCount);
	int i;
	for (i = 0; i < procCount; i++) {
		if (!strcmp(procList[i].kp_proc.p_comm, "BTLive")) {
			return 1;
		}
	}
	return 0;
}

FB::variant btlauncherAPI::isRunning(const std::string& val) {
	FB::VariantList list;
	
	if (this->isLiveRunning()) list.push_back(true);
	return list;
}