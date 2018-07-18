/*
	Part of this code was inspired from Negatron-Bootstrap
*/
#include "stdafx.h"

#include "JLauncher.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib ,"user32.lib")

DWORD currentPath(TCHAR* workingPath, TCHAR* baseName)
{
	// get working path from current executable path
	if (!GetModuleFileName(NULL, workingPath, MAX_PATH))
		return GetLastError();

	// search where workingpath should really end (ie: up to last backslash)
	TCHAR * endPath = _tcsrchr(workingPath, TEXT('\\'));
	*endPath = '\0';

	// get basename past workingpath's nul string stopping to last dot char
	endPath++;
	TCHAR * endBaseName = _tcsrchr(endPath, TEXT('.'));
	*endBaseName = '\0';
	_tcscpy_s(baseName, MAX_PATH, endPath);

	return 0;
}

TCHAR * format(TCHAR* format, ...) {
	va_list args;
	va_start(args, format);
	int len = _vsctprintf(format, args) + 1;
	TCHAR* buffer = new TCHAR[len];
	_vstprintf_s(buffer, len, format, args);
	return buffer;
}

int APIENTRY _tWinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	TCHAR workingPath[MAX_PATH]{}, jrePath[MAX_PATH], baseName[MAX_PATH];
	TCHAR jre[] = TEXT("javaw.exe");
	DWORD error = currentPath(workingPath, baseName);
	if (error != 0)
		return error;

	if(SearchPath(NULL, jre, NULL, MAX_PATH, jrePath, NULL))
	{
		DWORD binaryType = 0;
		TCHAR* mem = TEXT("1g");
		if (!GetBinaryType(jrePath, &binaryType))
		{
			MessageBox(0, format(TEXT("Failed to get type of %s"), jre), TEXT("Error"), MB_OK);
			return -2;
		}
		if (binaryType == SCS_64BIT_BINARY)
			mem = TEXT("2g");
		if (PathFileExists(format(TEXT("%s\\%s.jar"), workingPath, baseName)))
		{
			TCHAR * args = format(TEXT("-Xms512k -Xmx%s -jar \"%s.jar\" --multiuser --noupdate"), mem, baseName);
			PROCESS_INFORMATION pnfo{};
			STARTUPINFO snfo{};
			snfo.cb = sizeof(STARTUPINFO);
			if (!CreateProcess(jrePath /* application */, args /* cmdline */, NULL /* processAttributes */, NULL /* ThreadAttributes*/, FALSE /* InheritHandles */, 0 /* creation flags */, NULL /* env */, workingPath /* current process path */, &snfo, &pnfo))
			{
				MessageBox(0, format(TEXT("CreateProcess failed (%d)."), GetLastError()), TEXT("Error"), MB_OK);
				return 0;
			}
			WaitForSingleObject(pnfo.hProcess, INFINITE);
			CloseHandle(pnfo.hProcess);
			CloseHandle(pnfo.hThread);
		}
		else
		{
			MessageBox(0, format(TEXT("%s.jar is missing, it should be located at :\n\"%s\"\nwhere %s.exe (this executable) currently reside..."), baseName, workingPath, baseName), TEXT("Error"), MB_OK);
			return -3;
		}
	}
	else
	{
		MessageBox(0, TEXT("javaw.exe was not found in your PATH environment variable.\nYou must install Java (1.8.0 or later)!"), TEXT("Error"), MB_OK);
		return -1;
	}
	return 0;
}
