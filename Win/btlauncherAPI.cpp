/**********************************************************\

  Auto-generated btlauncherAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"
#include "btlauncherAPI.h"
#include "windows.h"
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <atlbase.h>
#include <atlstr.h>
#include <string.h>

#define bufsz 2048
#define BT_HEXCODE "4823DF041B" // BT4823DF041B0D
#define BTLIVE_CODE "BTLive"
#define INSTALL_REG_PATH _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\")
#define UT_DL "http://download.utorrent.com/3.1/utorrent.exe"
#define BT_DL "http://download.bittorrent.com/dl/BitTorrent-7.6.exe"
#define LV_DL "http://s3.amazonaws.com/live-installer/BTLivePlugin.exe"
#define STANDALONE_DL "http://www.pwmckenna.com/projects/btapp/bittorrent/utorrent.exe"

#define LIVE_NAME "BTLive"
#define UTORRENT_NAME "uTorrent"
#define BITTORRENT_NAME "BitTorrent"
#define TORQUE_NAME "Torque"

//#define UT_DL "http://192.168.56.1:9090/static/utorrent.exe"

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
	registerMethod("checkForUpdate", make_method(this, &btlauncherAPI::checkForUpdate));
    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &btlauncherAPI::get_testString,
                        &btlauncherAPI::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &btlauncherAPI::get_version));
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



// Read/Write property testString
std::string btlauncherAPI::get_testString()
{
    return m_testString;
}
void btlauncherAPI::set_testString(const std::string& val)
{
    m_testString = val;
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

void btlauncherAPI::gotDownloadProgram(const FB::JSObjectPtr& callback, 
									   std::wstring& program,
									   std::string& version,
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size) {
	TCHAR temppath[500];
	DWORD gettempresult = GetTempPath(500, temppath);
	if (! gettempresult) {
		callback->InvokeAsync("", FB::variant_list_of(false)("GetTempPath")(GetLastError()));
		return;
	}
	std::wstring syspath(temppath);
	syspath.append( program.c_str() );
	boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	syspath.append( _T("_") );
	std::wstring wversion;
	wversion.assign( version.begin(), version.end() );
	syspath.append( wversion );
	syspath.append( _T("_") );
	syspath.append( boost::uuids::to_wstring(u) );
	syspath.append(_T(".exe"));

	HANDLE hFile = CreateFile( syspath.c_str(), GENERIC_WRITE | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL );
	if (hFile == INVALID_HANDLE_VALUE) {
		
		callback->InvokeAsync("", FB::variant_list_of(false)(GetLastError()));
		return;
	}
	PVOID ptr = (VOID*) data.get();
	DWORD written = 0;
	BOOL RESULT = WriteFile( hFile, ptr, size, &written, NULL);
	CloseHandle(hFile);
	
	if (! RESULT) {
		callback->InvokeAsync("", FB::variant_list_of("FILE")(false)(GetLastError()));
		return;
	}
	std::wstring installcommand = std::wstring(syspath);
	installcommand.append(_T(" /NOINSTALL /MINIMIZED /HIDE"));
	STARTUPINFO info;
	PROCESS_INFORMATION procinfo;
	memset(&info,0,sizeof(info));
	info.cb = sizeof(STARTUPINFO);
	 
	/* CreateProcessW can modify installcommand thus we allocate needed memory */ 
	wchar_t * pwszParam = new wchar_t[installcommand.size() + 1]; 
	const wchar_t* pchrTemp = installcommand.c_str(); 
    wcscpy_s(pwszParam, installcommand.size() + 1, pchrTemp); 

	BOOL bProc = CreateProcess(NULL, pwszParam, NULL, NULL, FALSE, 0, NULL, NULL, &info, &procinfo);
	if(bProc) {
		callback->InvokeAsync("", FB::variant_list_of("PROCESS")(true)(installcommand.c_str())(GetLastError()));
	} else {
		callback->InvokeAsync("", FB::variant_list_of("PROCESS")(false)(installcommand.c_str())(GetLastError()));
	}

}

void btlauncherAPI::gotCheckForUpdate(const FB::JSObjectPtr& callback, 
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size) {
	if (! success) {
		callback->InvokeAsync("", FB::variant_list_of(success));
		return;
	}
	TCHAR temppath[500];
	DWORD gettempresult = GetTempPath(500, temppath);
	if (! gettempresult) {
		callback->InvokeAsync("", FB::variant_list_of(false)("GetTempPath")(GetLastError()));
		return;
	}
	std::wstring syspath(temppath);
	syspath.append( _T("btlaunch") );
	boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	syspath.append( _T("_") );
	syspath.append( boost::uuids::to_wstring(u) );
	syspath.append(_T(".msi"));

	HANDLE hFile = CreateFile( syspath.c_str(), GENERIC_WRITE | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL );
	if (hFile == INVALID_HANDLE_VALUE) {
		callback->InvokeAsync("", FB::variant_list_of(false)("CreateFile")(GetLastError()));
		return;
	}
	PVOID ptr = (VOID*) data.get();
	DWORD written = 0;
	BOOL RESULT = WriteFile( hFile, ptr, size, &written, NULL);
	CloseHandle(hFile);
	
	if (! RESULT) {
		callback->InvokeAsync("", FB::variant_list_of("WriteFile")(false)(GetLastError()));
		return;
	}
	std::wstring installcommand = std::wstring(_T("msiexec.exe /I "));
	installcommand.append( std::wstring(syspath) );
	installcommand.append( _T(" /q") );
	//installcommand.append(_T(" /NOINSTALL /MINIMIZED /HIDE"));
	STARTUPINFO info;
	PROCESS_INFORMATION procinfo;
	memset(&info,0,sizeof(info));
	info.cb = sizeof(STARTUPINFO);
	 
	/* CreateProcessW can modify installcommand thus we allocate needed memory */ 
	wchar_t * pwszParam = new wchar_t[installcommand.size() + 1]; 
	const wchar_t* pchrTemp = installcommand.c_str(); 
    wcscpy_s(pwszParam, installcommand.size() + 1, pchrTemp); 

	BOOL bProc = CreateProcess(NULL, pwszParam, NULL, NULL, FALSE, 0, NULL, NULL, &info, &procinfo);
	if(bProc) {
		callback->InvokeAsync("", FB::variant_list_of("CreateProcess")(true)(installcommand.c_str())(GetLastError()));
	} else {
		callback->InvokeAsync("", FB::variant_list_of("CreateProcess")(false)(installcommand.c_str())(GetLastError()));
	}
}

void btlauncherAPI::checkForUpdate(const FB::JSObjectPtr& callback) {
	std::string url = std::string("http://10.10.90.24:9090/static/btlauncher.msi?v=");
	url.append( std::string(FBSTRING_PLUGIN_VERSION) );

	
	SYSTEMTIME lpTime;
	GetSystemTime(&lpTime);
	char str[1024];
	sprintf(str,"&_t=%d",(lpTime.wSecond + 1000 * lpTime.wMilliseconds));
	url.append(str);

	//url.append( itoa(lpTime->wMilliseconds, buf, 10) );
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotCheckForUpdate, this, callback, _1, _2, _3, _4)
		);

}


void btlauncherAPI::downloadProgram(const std::wstring& program, const std::string& version, const FB::JSObjectPtr& callback) {
	std::string url;

	if (wcsstr(program.c_str(), _T("uTorrent"))) {
		if (version.length() > 0) {
			url = std::string("http://download.utorrent.com/");
			url.append( version.c_str() );
			url.append( "/utorrent.exe" );
		} else {
			url = std::string(UT_DL);
		}
	} else if (wcsstr(program.c_str(), _T("BitTorrent"))) {
		url = std::string(BT_DL);
	} else if (wcsstr(program.c_str(), _T("Standalone"))) {
		url = std::string(STANDALONE_DL);
	} else if (wcsstr(program.c_str(), _T("BTLive"))) { 
		url = std::string(LV_DL);
	} else {
	  return;
	}
	
	//url = version.c_str();
		
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotDownloadProgram, this, callback, program, version, _1, _2, _3, _4)
		);
}


std::wstring getRegStringValue(const std::wstring& path, const std::wstring& key, HKEY parentKey) {
	CRegKey regKey;
	const CString REG_SW_GROUP = path.c_str();
	TCHAR szBuffer[bufsz];
	ULONG cchBuffer = bufsz;
	LONG RESULT;
	RESULT = regKey.Open(parentKey, REG_SW_GROUP, KEY_READ);
	if (RESULT == ERROR_SUCCESS) {
		RESULT = regKey.QueryStringValue( key.c_str(), szBuffer, &cchBuffer );
		regKey.Close();
		if (RESULT == ERROR_SUCCESS) {
			return std::wstring(szBuffer);
		} else {
			return std::wstring(_T(""));
		}
	} else {
		return std::wstring(_T(""));
	}
}

std::wstring btlauncherAPI::getInstallVersion(const std::wstring& program) {	
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	HKEY parentKey = HKEY_LOCAL_MACHINE;
	if (program == _T("BTLive")) {
		parentKey = HKEY_CURRENT_USER;
	}
	return getRegStringValue( reg_group, _T("DisplayVersion"), parentKey );
}
std::wstring btlauncherAPI::getInstallPath(const std::wstring& program) {	
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	HKEY parentKey = HKEY_LOCAL_MACHINE;
	if (program == _T("BTLive")) {
		parentKey = HKEY_CURRENT_USER;
	}
	return getRegStringValue( reg_group, _T("InstallLocation"), parentKey );
}

std::wstring getExecutablePath(const std::wstring& program) {
	HKEY parentKey = HKEY_LOCAL_MACHINE;
	if (program == _T("BTLive")) {
		parentKey = HKEY_CURRENT_USER;
	}
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	std::wstring location = getRegStringValue( reg_group, _T("InstallLocation"), parentKey );
	location.append( _T("\\") );
	location.append( program );
	location.append( _T(".exe") );
	return location;
}



HINSTANCE launch_program(const std::wstring& program) {
	HINSTANCE result = ShellExecute(NULL, NULL, getExecutablePath(program).c_str(), NULL, NULL, NULL);
	return result;
}

FB::variant btlauncherAPI::runProgram(const std::wstring& program, const FB::JSObjectPtr& callback) {
	HINSTANCE ret = (HINSTANCE)0;
	if (isRunning(program).size() == 0) {
		ret = launch_program(program);
		callback->InvokeAsync("", FB::variant_list_of(false)(ret));
	}
	return ret;
}

struct callbackdata {
	callbackdata() {
		found = FALSE;
	}
	BOOL found;
	std::wstring name;
	FB::VariantList list;
};

TCHAR classname[50];
BOOL CALLBACK EnumWindowCB(HWND hWnd, LPARAM lParam) {
	GetClassName(hWnd, classname, sizeof(classname));
	callbackdata* cbdata = (callbackdata*) lParam;
	// BT4823 see gui/wndmain.cpp for the _utorrent_classname (begins with 4823)
	if (wcsstr(classname, cbdata->name.c_str())) {	
		//FB::JSObjectPtr& callback = *((FB::JSObjectPtr*)lParam);
		//cbdata->found = true;
		cbdata->list.push_back( std::wstring(classname) );
		//return FALSE;
	}
	return TRUE;
}

FB::VariantList btlauncherAPI::stopRunning(const std::wstring& val) {
	FB::VariantList list;
	if (wcsstr(val.c_str(), _T(BT_HEXCODE)) || wcsstr(val.c_str(), _T(BTLIVE_CODE))) {
		HWND hWnd = FindWindow( val.c_str(), NULL );
		DWORD pid;
		DWORD parent;
		parent = GetWindowThreadProcessId(hWnd, &pid);
		HANDLE pHandle = OpenProcess(PROCESS_TERMINATE, NULL, pid);
		if (! pHandle) {
			list.push_back("could not open process");
			list.push_back(GetLastError());
		} else {
			BOOL result = TerminateProcess(pHandle, 0);
			list.push_back("ok");
			list.push_back(result);
		}
	}
	return list;
}

FB::VariantList btlauncherAPI::isRunning(const std::wstring& val) {
	FB::VariantList list;
	callbackdata cbdata;
	cbdata.list = list;
	cbdata.name = val;
	if (wcsstr(val.c_str(), _T(BT_HEXCODE)) || wcsstr(val.c_str(), _T(BTLIVE_CODE))) {
		EnumWindows(EnumWindowCB, (LPARAM) &cbdata);
	}
	return cbdata.list;	
}

bool isSupported(std::string program) {
	if (program == LIVE_NAME) {
		return true;
	}
	return false;
}


//FB::variant btlauncherAPI::launchClient(const std::
