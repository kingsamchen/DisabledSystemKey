/**************************************************

** Project:LogToFile

** File:logfile.h

** Edition:v1.0.0 Demo

** Coder:KingsamChen [MDSA Group]

** Last Modify:2010-8-14

**************************************************/

#if _MSC_VER > 1000
#pragma  once
#endif

#ifndef _LOGFILE_C6A57B5F_C91D_4fc0_9B38_10524313FAE1
#define _LOGFILE_C6A57B5F_C91D_4fc0_9B38_10524313FAE1

const wchar_t ERRLOG_FILE_NAME[] = L"ErrorLog.log";

void LogErrorToFile(const wchar_t* lpszFileName, wchar_t* lpszFormat, ...);
void ClearFile(const wchar_t* lpszFileName);
void DelFile(const wchar_t* lpszFileName);

static int WriteFile(const wchar_t* lpszFileName, wchar_t* lpszStr);

#endif