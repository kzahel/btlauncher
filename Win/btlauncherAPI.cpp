/**********************************************************\

  Auto-generated btlauncherAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
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


#define USE_CURL 0

#if USE_CURL
#include "curl/curl.h"
#endif

#define bufsz 2048
#define BT_HEXCODE "4823DF041B" // BT4823DF041B0D
#define BTLIVE_CODE "BTLive"
#define INSTALL_REG_PATH _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\")
#define MSIE_ELEVATION _T("Software\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy")
#define MSIE_ELEVATION_GUID "ECC81F59-D6B1-46A4-B5E8-900FB424B95D"

#ifdef LIVE
  #define PLUGIN_DL "http://s3.amazonaws.com/live-installer/LivePlugin.msi"
#endif //LIVE

#ifdef SHARE
	#define PLUGIN_DL "http://torque.bittorrent.com/SoShare.msi"
#endif //SHARE

#ifdef TORQUE
	#define PLUGIN_DL "http://torque.bittorrent.com/Torque.msi"
#endif //TORQUE

#define UT_DL "http://download.utorrent.com/latest/uTorrent.exe"
#define BT_DL "http://download.bittorrent.com/latest/BitTorrent.exe"
#define LV_DL "http://s3.amazonaws.com/live-installer/BTLivePlugin.exe"
#define TORQUE_DL "http://download.utorrent.com/torque/latest/Torque.exe"
#define SOSHARE_DL "http://download.utorrent.com/soshare/latest/SoShare.exe"

#define lenof(x) (sizeof(x)/sizeof(x[0]))

char* PAIRING_DOMAINS[] = { 
	"bittorrent.com", 
	"utorrent.com",
	"soshare.it",
	"soshareit.com",
	"paddleover.com",
	"kumarethemovie.com",
	"btappjs.com",
	"onehash.com",
	"utorrenttoolbox.com"
};

#define LIVE_NAME "BTLive"
#define UTORRENT_NAME "uTorrent"
#define BITTORRENT_NAME "BitTorrent"
#define TORQUE_NAME "Torque"
#define SOSHARE_NAME "SoShare"

#define NOT_SUPPORTED_MESSAGE "This application is not supported."

BOOL write_elevation(const std::wstring& path, const std::wstring& name);

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
	registerMethod("getInstallPath", make_method(this, &btlauncherAPI::getInstallPath));
	registerMethod("getInstallVersion", make_method(this, &btlauncherAPI::getInstallVersion));
	registerMethod("isRunning", make_method(this, &btlauncherAPI::isRunning));
	registerMethod("stopRunning", make_method(this, &btlauncherAPI::stopRunning));
	registerMethod("runProgram", make_method(this, &btlauncherAPI::runProgram));
	registerMethod("downloadProgram", make_method(this, &btlauncherAPI::downloadProgram));
#ifndef CHROME
	registerMethod("checkForUpdate", make_method(this, &btlauncherAPI::checkForUpdate));
#endif //CHROME
	registerMethod("getPID", make_method(this, &btlauncherAPI::getPID));

	registerMethod("ajax", make_method(this, &btlauncherAPI::ajax));
	registerMethod("pair", make_method(this, &btlauncherAPI::pair));

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

void btlauncherAPI::do_callback(const FB::JSObjectPtr& callback, const std::vector<FB::variant>& args) {
	try {
		callback->InvokeAsync("", args);
	} catch (std::exception) {
		// TODO -- only catch the std::runtime_error("Cannot invoke asynchronously"); (FireBreath JSObject.h)
	}
}

std::wstring GetRandomKey() {
	std::wstring possibilities = _T("0123456789ABCDEF");
	wchar_t tmp[40];
	int i;
	for(i = 0; i < 40; i++) {
		tmp[i] = possibilities.c_str()[rand() % possibilities.size()];
	}
	std::wstring ret(tmp, 40);
	assert(ret.size() == 40);
	return ret;
}

int btlauncherAPI::getPID() {
	return GetCurrentProcessId();
}

void btlauncherAPI::gotDownloadProgram(const FB::JSObjectPtr& callback, 
									   std::wstring& program,
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size) {
	OutputDebugString(_T("gotDownloadProgram ENTER"));
										   
	if(!success) {
		do_callback(callback, FB::variant_list_of(false)("getDownloadProgram")(success));
	}

	TCHAR temppath[500];
	DWORD gettempresult = GetTempPath(500, temppath);
	if (! gettempresult) {
		do_callback(callback, FB::variant_list_of(false)("GetTempPath")(GetLastError()));
		OutputDebugString(_T("gotDownloadProgram EXIT"));
		return;
	}
	std::wstring folder(temppath);
	folder.append( program.c_str() );
	folder.append( _T("_") );
	boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	folder.append( boost::uuids::to_wstring(u) );
	BOOL result = CreateDirectory( folder.c_str(), NULL );
	if (! result) {
		do_callback(callback, FB::variant_list_of(false)("error creating directory")(GetLastError()));
		OutputDebugString(_T("gotDownloadProgram EXIT"));
		return;
	}
	std::wstring syspath(folder);
	std::wstring name(program);
	name.append(_T(".exe"));
	syspath.append( _T("\\") );
	syspath.append( name.c_str() );
	HANDLE hFile = CreateFile( syspath.c_str(), GENERIC_WRITE | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL );
	if (hFile == INVALID_HANDLE_VALUE) {
		do_callback(callback, FB::variant_list_of(false)(GetLastError()));
		OutputDebugString(_T("gotDownloadProgram EXIT"));
		return;
	}
	PVOID ptr = (VOID*) data.get();
	DWORD written = 0;
	BOOL RESULT = WriteFile( hFile, ptr, size, &written, NULL);
	CloseHandle(hFile);
	
	if (! RESULT) {
		do_callback(callback, FB::variant_list_of("FILE")(false)(GetLastError()));
		OutputDebugString(_T("gotDownloadProgram EXIT"));
		return;
	}

	BOOL elevated = write_elevation(folder, name);


	std::wstring installcommand = std::wstring(syspath);
	std::wstring args;
	args.append(_T(" /PAIR "));
	std::wstring pairingkey = GetRandomKey();
	args.append(pairingkey);

	STARTUPINFO info;
	PROCESS_INFORMATION procinfo;
	memset(&info,0,sizeof(info));
	info.cb = sizeof(STARTUPINFO);
	 
	/* CreateProcessW can modify installcommand thus we allocate needed memory */ 
	wchar_t * pwszParam = new wchar_t[installcommand.size() + 1]; 
	wcscpy_s(pwszParam, installcommand.size() + 1, installcommand.c_str()); 

	wchar_t * pwszArgs = new wchar_t[args.size() + 1];
	wcscpy_s(pwszArgs, args.size() + 1, args.c_str());

	OutputDebugString(pwszParam);
	OutputDebugString(pwszArgs);

	BOOL bProc = FALSE;
	OutputDebugString(_T("gotDownloadProgram CreateProcess"));
	bProc = CreateProcess(pwszParam, pwszArgs, NULL, NULL, FALSE, 0, NULL, NULL, &info, &procinfo);

	if(bProc) {
		OutputDebugString(_T("gotDownloadProgram SUCCESS"));
		do_callback( callback, FB::variant_list_of("PROCESS")(true)(installcommand.c_str())(GetLastError())(pairingkey));
	} else {
		OutputDebugString(_T("gotDownloadProgram FAILURE"));
		do_callback( callback, FB::variant_list_of("PROCESS")(false)(installcommand.c_str())(GetLastError()));
	}
	OutputDebugString(_T("gotDownloadProgram EXIT"));
}

#ifndef CHROME
void btlauncherAPI::gotCheckForUpdate(const FB::JSObjectPtr& callback, 
									   bool success,
									   const FB::HeaderMap& headers,
									   const boost::shared_array<uint8_t>& data,
									   const size_t size) {
	if (! success) {
		do_callback(callback, FB::variant_list_of(success));
		return;
	}
	TCHAR temppath[500];
	DWORD gettempresult = GetTempPath(500, temppath);
	if (! gettempresult) {
		do_callback(callback, FB::variant_list_of(false)("GetTempPath")(GetLastError()));
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
		do_callback(callback, FB::variant_list_of(false)("CreateFile")(GetLastError()));
		return;
	}
	PVOID ptr = (VOID*) data.get();
	DWORD written = 0;
	BOOL RESULT = WriteFile( hFile, ptr, size, &written, NULL);
	CloseHandle(hFile);
	
	if (! RESULT) {
		do_callback(callback, FB::variant_list_of("WriteFile")(false)(GetLastError()));
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
		do_callback(callback, FB::variant_list_of("CreateProcess")(true)(installcommand.c_str())(0));
	} else {
		do_callback(callback, FB::variant_list_of("CreateProcess")(false)(installcommand.c_str())(GetLastError()));
	}
}

void btlauncherAPI::checkForUpdate(const FB::JSObjectPtr& callback) {
	std::string url = std::string(PLUGIN_DL);
	url.append( std::string("?v=") );
	url.append( std::string(FBSTRING_PLUGIN_VERSION) );

	
	SYSTEMTIME lpTime;
	GetSystemTime(&lpTime);
	char str[1024];
	sprintf(str,"&_t=%d",(lpTime.wSecond + 1000 * lpTime.wMilliseconds));
	url.append(str);

	//url.append( itoa(lpTime->wMilliseconds, buf, 10) );
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotCheckForUpdate, this, callback, _1, _2, _3, _4), false
		);
}
#endif //CHROME

void btlauncherAPI::ajax(const std::string& url, const FB::JSObjectPtr& callback) {
	OutputDebugString(_T("ajax ENTER"));
#if !USE_CURL
	if (FB::URI::fromString(url).domain != "127.0.0.1") {
		FB::VariantMap response;
		response["allowed"] = false;
		response["success"] = false;
		do_callback(callback, FB::variant_list_of(response));
		return;
	}
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), 
		boost::bind(&btlauncherAPI::gotajax, this, callback, _1, _2, _3, _4), false
		);
	OutputDebugString(_T("ajax EXIT"));
#else

    CURL *curl;
    CURLcode res;
 
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://firebreath.com");
        res = curl_easy_perform(curl);
 
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
 
    //return res;

#endif
}

void btlauncherAPI::gotajax(const FB::JSObjectPtr& callback, 
							bool success,
						    const FB::HeaderMap& headers,
						    const boost::shared_array<uint8_t>& data,
						    const size_t size) {

	OutputDebugString(_T("gotajax ENTER"));

	FB::VariantMap response;
	response["allowed"] = true;
	response["success"] = success;

	if(!success) {
		do_callback(callback, FB::variant_list_of(response));
		OutputDebugString(_T("gotajax FAILURE"));
		OutputDebugString(_T("gotajax EXIT"));
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
	do_callback(callback, FB::variant_list_of(response));

	OutputDebugString(_T("gotajax SUCCESS"));
	OutputDebugString(_T("gotajax EXIT"));
}

void btlauncherAPI::downloadProgram(const std::wstring& program, const FB::JSObjectPtr& callback) {
	OutputDebugString(_T("downloadProgram ENTER"));
	OutputDebugString(program.c_str());

	std::string url;

	if (wcsstr(program.c_str(), _T("uTorrent"))) {
		url = std::string(UT_DL);
	} else if (wcsstr(program.c_str(), _T("BitTorrent"))) {
		url = std::string(BT_DL);
    } else if (wcsstr(program.c_str(), _T("Torque"))) {
		url = std::string(TORQUE_DL);
    } else if (wcsstr(program.c_str(), _T("SoShare"))) {
		url = std::string(SOSHARE_DL);
	} else if (wcsstr(program.c_str(), _T("BTLive"))) { 
		url = std::string(LV_DL);
	} else {
		//its a rebranded Torque build...just use torque but go with the flow
		url = std::string(TORQUE_DL);
	}
	
	//url = version.c_str();
		
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url),
		boost::bind(&btlauncherAPI::gotDownloadProgram, this, callback, program, _1, _2, _3, _4), false
	);
	OutputDebugString(_T("downloadProgram EXIT"));
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

BOOL setRegStringValue(const std::wstring& path, const std::wstring& key, const std::wstring& value, HKEY parentKey) {
	CRegKey regKey;
	const CString REG_SW_GROUP = path.c_str();
	LONG res;
	res = regKey.Create(parentKey, REG_SW_GROUP, REG_NONE);
	res = regKey.Open(parentKey, REG_SW_GROUP, KEY_READ | KEY_WRITE);
	DWORD err = GetLastError();
	if (res == ERROR_SUCCESS) {
		res = regKey.SetValue( value.c_str(), key.c_str() );
		regKey.Close();
		if (res == ERROR_SUCCESS) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

BOOL setRegDwordValue(const std::wstring& path, const std::wstring& key, DWORD value, HKEY parentKey) {
	CRegKey regKey;
	const CString REG_SW_GROUP = path.c_str();
	LONG res;
	res = regKey.Create(parentKey, REG_SW_GROUP, REG_NONE);
	res = regKey.Open(parentKey, REG_SW_GROUP, KEY_READ | KEY_WRITE);
	DWORD err = GetLastError();
	if (res == ERROR_SUCCESS) {
		res = regKey.SetValue( value, key.c_str() );
		regKey.Close();
		if (res == ERROR_SUCCESS) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

std::wstring btlauncherAPI::getInstallVersion(const std::wstring& program) {	
	OutputDebugString(_T("getInstallVersion ENTER"));
	if (!this->isSupported(program)) {
		return _T(NOT_SUPPORTED_MESSAGE);
	}
	OutputDebugString(program.c_str());
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	OutputDebugString(reg_group.c_str());
	std::wstring ret = getRegStringValue( reg_group, _T("DisplayVersion"), HKEY_LOCAL_MACHINE );
	if (ret.empty()) {
		ret = getRegStringValue( reg_group, _T("DisplayVersion"), HKEY_CURRENT_USER );
	}
	OutputDebugString(ret.c_str());
	OutputDebugString(_T("getInstallVersion EXIT"));
	return ret;
}

std::wstring get_install_path(const std::wstring& program) {
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	std::wstring ret = getRegStringValue( reg_group, _T("InstallLocation"), HKEY_LOCAL_MACHINE );
	if (ret.empty()) {
		ret = getRegStringValue( reg_group, _T("InstallLocation"), HKEY_CURRENT_USER );
	}
	return ret;
}

std::wstring btlauncherAPI::getInstallPath(const std::wstring& program) {
	OutputDebugString(_T("getInstallPath ENTER"));
	if (!this->isSupported(program)) {
		OutputDebugString(_T("getInstallPath EXIT"));
		return _T(NOT_SUPPORTED_MESSAGE);
	}
	std::wstring ret = get_install_path(program);
	OutputDebugString(ret.c_str());
	OutputDebugString(_T("getInstallPath EXIT"));
	return ret;
}

std::wstring getExecutablePath(const std::wstring& program) {
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	std::wstring location = getRegStringValue( reg_group, _T("InstallLocation"), HKEY_LOCAL_MACHINE );
	if(location.empty()) {
		location = getRegStringValue( reg_group, _T("InstallLocation"), HKEY_CURRENT_USER ); 
	}
	location.append( _T("\\") );
	location.append( program );
	location.append( _T(".exe") );
	return location;
}

BOOL write_elevation(const std::wstring& path, const std::wstring& name) {
	std::wstring reg_group = std::wstring(MSIE_ELEVATION);
	reg_group.append(_T("\\{"));
	boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	//reg_group.append( boost::uuids::to_wstring(u) );
	reg_group.append(_T(MSIE_ELEVATION_GUID));
	reg_group.append(_T("}"));
	BOOL ret = setRegStringValue( reg_group, _T("AppPath"), path, HKEY_CURRENT_USER );
	ret = setRegStringValue( reg_group, _T("AppName"), name, HKEY_CURRENT_USER );
	ret = setRegDwordValue( reg_group, _T("Policy"), 3, HKEY_CURRENT_USER );
	return ret;
}

BOOL launch_program(const std::wstring& program, const std::wstring& switches, bool wait=false) {
	//HINSTANCE result = ShellExecute(NULL, NULL, getExecutablePath(program).c_str(), NULL, NULL, NULL);
	// pops up a security dialog in IE

	// try to write to IE security dialog...
	BOOL result = write_elevation(get_install_path(program), _T("SoShare.exe"));

	std::wstring installcommand = getExecutablePath(program).c_str();
	if (switches.length() > 0) {
		installcommand.append( switches );
	}
	STARTUPINFO info;
	PROCESS_INFORMATION procinfo;
	memset(&info,0,sizeof(info));
	info.cb = sizeof(STARTUPINFO);

	wchar_t * pwszParam = new wchar_t[installcommand.size() + 1]; 
	const wchar_t* pchrTemp = installcommand.c_str(); 
    wcscpy_s(pwszParam, installcommand.size() + 1, pchrTemp); 
	BOOL bProc = FALSE;
	bProc = CreateProcess(NULL, pwszParam, NULL, NULL, FALSE, 0, NULL, NULL, &info, &procinfo);
	if(wait) {
		WaitForSingleObject(procinfo.hProcess, INFINITE);
	}
	return bProc;
}

FB::variant btlauncherAPI::pair(const std::wstring& program) {
	OutputDebugString(_T("pair ENTER"));
	if (!this->isSupported(program)) {
		return _T(NOT_SUPPORTED_MESSAGE);
	}
	std::string location = m_host->getDOMWindow()->getLocation();
	FB::URI uri = FB::URI::fromString(location);
	OutputDebugStringA(location.c_str());

#ifndef CHROME && VERIFY_PAIRING
	bool allowed = false;
	for(int i = 0; i < lenof(PAIRING_DOMAINS); i++) {
		allowed |= (uri.domain.find(PAIRING_DOMAINS[i])!=std::string::npos);
	}
	if (allowed) {
		OutputDebugString(_T("access granted"));
	} else {
		OutputDebugString(_T("access denied"));
		OutputDebugString(_T("pair EXIT"));
		return _T("access denied");
	}
#endif //CHROME && VERIFY_PAIRING
	//std::string location = w->getLocation();
	BOOL ret = FALSE;
	std::wstring key = GetRandomKey();
	std::wstring switches = std::wstring(_T(" /PAIR "));
	switches.append(key);
	ret = launch_program(program, switches, true);
	OutputDebugString(_T("pair EXIT"));
	return key;
}

FB::variant btlauncherAPI::runProgram(const std::wstring& program, const FB::JSObjectPtr& callback) {
	OutputDebugString(_T("runProgram ENTER"));
	OutputDebugString(program.c_str());
	if (!this->isSupported(program)) {
		return _T(NOT_SUPPORTED_MESSAGE);
	}
	
	//HINSTANCE ret = (HINSTANCE)0;
	BOOL ret = FALSE;
	if (!isRunning(program).cast<BOOL>()) {
		ret = launch_program(program, _T(""), false);
		if(ret) {
			OutputDebugString(_T("runProgram CALLBACK SUCCESS"));
		} else {
			OutputDebugString(_T("runProgram CALLBACK FAILURE"));
		}
		do_callback(callback, FB::variant_list_of(false)(ret));
	}
	OutputDebugString(_T("runProgram EXIT"));
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
	if (wcsstr(classname, cbdata->name.c_str()) && 
		(wcsstr(classname, _T(BT_HEXCODE)) || wcsstr(classname, _T(BTLIVE_CODE)))) {	
		//FB::JSObjectPtr& callback = *((FB::JSObjectPtr*)lParam);
		//cbdata->found = true;
		OutputDebugString(_T("EnumWindowCB SUCCESS"));
		OutputDebugString(classname);

		cbdata->list.push_back( std::wstring(classname) );
		//return FALSE;
	}
	return TRUE;
}

FB::variant btlauncherAPI::stopRunning(const std::wstring& val) {
	if (wcsstr(val.c_str(), _T(BT_HEXCODE)) || wcsstr(val.c_str(), _T(BTLIVE_CODE))) {
		HWND hWnd = FindWindow( val.c_str(), NULL );
		DWORD pid;
		DWORD parent;
		parent = GetWindowThreadProcessId(hWnd, &pid);
		HANDLE pHandle = OpenProcess(PROCESS_TERMINATE, NULL, pid);
		if (! pHandle) {
			return FALSE;
		} else {
			BOOL result = TerminateProcess(pHandle, 0);
			return result;
		}
	}
	return FALSE;
}

FB::variant btlauncherAPI::isRunning(const std::wstring& val) {
	OutputDebugString(_T("isRunning ENTER"));
	OutputDebugString(val.c_str());
	FB::VariantList list;
	callbackdata cbdata;
	cbdata.list = list;
	cbdata.name = val;
	EnumWindows(EnumWindowCB, (LPARAM) &cbdata);
	OutputDebugString(_T("isRunning EXIT"));
	return (cbdata.list.size() > 0) ? TRUE : FALSE;	
}

bool btlauncherAPI::isSupported(std::wstring program) {

	if (program == _T(LIVE_NAME) || program == _T(UTORRENT_NAME) || program == _T(BITTORRENT_NAME) || program == _T(TORQUE_NAME) || program == _T(SOSHARE_NAME)) {
		return true;
	}
	return false;
}


//FB::variant btlauncherAPI::launchClient(const std::
