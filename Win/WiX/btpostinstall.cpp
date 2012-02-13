// btpostinstall.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "atlbase.h"
#include <atlstr.h>

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

int _tmain(int argc, _TCHAR* argv[])
{
	int numclosed = shutDownAllWindows(_T("Chrome"));
	if (numclosed > 0) {
		int result2 = reOpen(_T("Google Chrome"));
	}
	return 0;
}

