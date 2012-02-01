/**********************************************************\

  Auto-generated btlauncherAPI.h

\**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "../btlauncher.h"
#include "SystemHelpers.h"
#include "SimpleStreamHelper.h"

#ifndef H_btlauncherAPI
#define H_btlauncherAPI

typedef struct kinfo_proc kinfo_proc;

class btlauncherAPI : public FB::JSAPIAuto
{
public:
    btlauncherAPI(const btlauncherPtr& plugin, const FB::BrowserHostPtr& host);
    virtual ~btlauncherAPI();

    btlauncherPtr getPlugin();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
	void downloadProgram(const std::string& val, const std::string& version, const FB::JSObjectPtr& callback);
	void gotDownloadProgram(const FB::JSObjectPtr& callback, 
										std::string& program,
										std::string& version,
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size);
	std::string getInstallPath(const std::string& val);
	std::string getInstallVersion(const std::string& val);
	FB::variant runProgram(const std::string& program, const FB::JSObjectPtr& callback);
	FB::variant isRunning(const std::string& val);
	FB::VariantList stopRunning(const std::string& val);

    // Event helpers
    FB_JSAPI_EVENT(fired, 3, (const FB::variant&, bool, int));
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));
    FB_JSAPI_EVENT(notify, 0, ());

    // Method test-event
    void testEvent(const FB::variant& s);

private:
	int isInstalledAndUpToDate();
	int isLiveRunning();
    btlauncherWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;
	int GetBSDProcessList(kinfo_proc **procList, size_t *procCount);

    pid_t m_live_pid;
	std::string installPath;
};

#endif // H_btlauncherAPI

