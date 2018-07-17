#include "stdafx.h"
#include "JLauncher.h"

#pragma comment(lib, "Shlwapi.lib")

// Forward declarations of functions included in this code module:
TCHAR * concat(TCHAR* format, ...);

int APIENTRY _tWinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// Determine current working directory
	TCHAR workingPath[MAX_PATH];
    if(!GetModuleFileName(NULL, workingPath, MAX_PATH))
        return GetLastError();
	TCHAR * endPath = _tcsrchr(workingPath, TEXT('\\'));
	*endPath = '\0';
	TCHAR baseName[MAX_PATH];
	endPath++;
	TCHAR * endBaseName = _tcsrchr(endPath, TEXT('.'));
	*endBaseName = '\0';
	_tcscpy_s(baseName, endPath);

	// Look for any portable versions of Java included in JLauncher's installation folder
	WIN32_FIND_DATA fileData;
	TCHAR jrePath[MAX_PATH] = TEXT("javaw.exe");
	TCHAR jrePathMask[MAX_PATH];
	_tcscpy_s(jrePathMask, workingPath);
	_tcscat_s(jrePathMask, TEXT("\\jre*"));
	HANDLE hFind = FindFirstFile(jrePathMask, &fileData);
	if (hFind != INVALID_HANDLE_VALUE) do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			_tcscpy_s(jrePathMask, fileData.cFileName);
			_tcscat_s(jrePathMask, TEXT("\\bin\\javaw.exe"));
			if (PathFileExists(jrePathMask) == TRUE) {
				_tcscpy_s(jrePath, jrePathMask);
				break;
			}
		}
	} while (FindNextFile(hFind, &fileData) != 0);
	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	TCHAR * arguments = concat(TEXT("%s -Xmx1g -jar \"%s\\%s.jar\" --multiuser --noupdate"), jrePath, workingPath, baseName);

	STARTUPINFO si;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );

	if (!CreateProcess(NULL,   // No module name (use command line)
		arguments,      // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		workingPath,    // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure
	)) {
		printf("CreateProcess failed (%d).\n", GetLastError());
		free(arguments);
		return 0;
	}
	else
		printf("launched '%s'", arguments);

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	free(arguments);

	return 0;
}

TCHAR * concat(TCHAR* format, ...) {
   va_list args;
   int len;
   TCHAR * buffer;

   va_start(args, format);
   len = _vsctprintf(format, args) + 1; // _vscprintf doesn't count terminating '\0'
   buffer = (TCHAR *) malloc(len * sizeof(TCHAR));
   _vstprintf_s(buffer, len, format, args);

   return buffer;
}