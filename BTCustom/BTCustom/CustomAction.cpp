
#include "stdafx.h"
#include <shellapi.h>
#include <atlstr.h>
#include "atlbase.h"
#include <vector>
#include <string>
#include "windows.h"



struct callbackdata {
	callbackdata() {
	}
	std::wstring name;
	std::vector<std::wstring> list;
};

#define bufsz 2048
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

BOOL closeWindowByClassName(std::wstring classname) {
	HWND hWnd = FindWindow( std::wstring(classname).c_str(), NULL );

		DWORD pid;
		BOOL result;
		DWORD parent;
		parent = GetWindowThreadProcessId(hWnd, &pid);
		HANDLE pHandle = OpenProcess(PROCESS_TERMINATE, NULL, pid);
		if (! pHandle) {
			result = 0;
			//list.push_back("could not open process");
			//list.push_back(GetLastError());
		} else {
			result = TerminateProcess(pHandle, 0);
			//list.push_back("ok");
			//list.push_back(result);
		}

	//BOOL result = DestroyWindow(hWnd);
	return result;
}

TCHAR classname[50];
BOOL CALLBACK EnumWindowCB(HWND hWnd, LPARAM lParam) {
	GetClassName(hWnd, classname, sizeof(classname));
	callbackdata* cbdata = (callbackdata*) lParam;
	//printf("%s\n", classname);
	WcaLog(LOGMSG_STANDARD, "BTCustom Enum Window : ");
	std::wstring t(classname);
	std::string x(t.begin(), t.end());
    WcaLog(LOGMSG_STANDARD, x.c_str());

	if (wcsstr(classname, cbdata->name.c_str())) {
		cbdata->list.push_back( std::wstring(classname) );
		closeWindowByClassName(std::wstring(classname));
	}
	return TRUE;
}


int shutDownAllWindows(std::wstring program) {
	callbackdata cbdata;
	std::vector<std::wstring> list;
	cbdata.list = list;
	cbdata.name = program.c_str();
	EnumWindows(EnumWindowCB, (LPARAM) &cbdata);
	return cbdata.list.size();
	
}
#define INSTALL_REG_PATH _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\")
int reOpen(std::wstring program) {
	std::wstring reg_group = std::wstring(INSTALL_REG_PATH).append( program );
	HKEY parentKey = HKEY_CURRENT_USER;
	std::wstring installPath = getRegStringValue(reg_group, _T("InstallLocation"), parentKey);
	installPath.append(_T("\\chrome.exe"));
	std::wstring params = std::wstring(_T(" --restore-last-session"));
	HINSTANCE result = ShellExecute(NULL, NULL, installPath.c_str(), params.c_str(), NULL, NULL);
	return 0;
}

UINT __stdcall BTCustomAction(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;

	hr = WcaInitialize(hInstall, "BTCustomAction");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "BTCustomAction Runnning!!!!!! Super L33t.");

	//Stop and restart Chrome and remember open tabs
	int numclosed = shutDownAllWindows(_T("Chrome"));
	if (numclosed > 0) {
		int result2 = reOpen(_T("Google Chrome"));
	}

	//Stop and remove old versions of live
	shutDownAllWindows(_T("BTLive"));
	shutDownAllWindows(_T("SysTrayIconPy"));
	
	//MsiExec.exe /X{85855403-BEC0-47DB-9ED5-55BC30CD8EEF}
	//ShellExecute(NULL, _T("open"), _T("MsiExec.exe"), _T(" /X{85855403-BEC0-47DB-9ED5-55BC30CD8EEF} /quiet"), _T(""), SW_SHOW);
	

	//Remove registry keys and exe
	//Remove uninstall and btlive url registry keys
	//HKEY_USERS\S-1-5-21-3017286179-1443849310-3635124019-1001\Software\Classes\btlive
	//HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{85855403-BEC0-47DB-9ED5-55BC30CD8EEF}

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}


// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
	__in HINSTANCE hInst,
	__in ULONG ulReason,
	__in LPVOID
	)
{
	switch(ulReason)
	{
	case DLL_PROCESS_ATTACH:
		WcaGlobalInitialize(hInst);
		break;

	case DLL_PROCESS_DETACH:
		WcaGlobalFinalize();
		break;
	}

	return TRUE;
}
