/**************************************************

** Project:LogToFile

** File:logfile.cpp

** Edition:v1.0.0 Demo

** Coder:KingsamChen [MDSA Group]

** Last Modify:2010-8-15

**************************************************/

//#include "stdafx.h"	// pre-compile
#include "logfile.h"
#include <stdio.h>
#include <crtdbg.h>	// may vc only
#include <time.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <io.h>

#define TRUE 1
#define FALSE 0


/*
	��  ��: lpszFileName(const wchar_t*) - [in]��־�ļ�·��
	��  ��: -
	��  ��: �����������Ϣд����־�ļ�
*/
void LogErrorToFile(const wchar_t* lpszFileName, wchar_t* lpszFormat,...)
{
	errno_t err;
	va_list args;
	va_start(args, lpszFormat);

	// ��ʽ������Ϊ��׼�ַ���
	const int BUFFER_SIZE = 512;
	wchar_t szBuffer[BUFFER_SIZE];

	// _ASSERT is VC only.If you use gcc,please use assert instead
	int nBuffRet = _vsnwprintf(szBuffer, BUFFER_SIZE, lpszFormat, args);
	_ASSERT(nBuffRet >= 0);	 

	// ʹ�ļ���֧��Unicode�µ�����.
	_wsetlocale(LC_ALL, L"CHS");

	int nWriteRet = WriteFile(lpszFileName, szBuffer);
	_ASSERT(nWriteRet);

	if (!nWriteRet)
	{
		_get_errno(&err);
		wprintf(L"LogFile writes failed.Error code is %d\n", err);
	}

	va_end(args);
}


/*
	��  ��: lpszFileName(const wchar_t*) - [in]��־�ļ�·��
			lpszStr(wchar_t*) - [in]��������
	��  ��: int - TRUE:д���ݳɹ�, FALSE:д����ʧ��
	��  ��: д������Ϣ
*/
static int WriteFile(const wchar_t* lpszFileName, wchar_t* lpszStr)
{
	FILE* pFile =_wfopen(lpszFileName, L"a");
	_ASSERT(pFile != NULL);
	time_t timeNow;

	if (NULL == pFile)
	{
		return FALSE;
	}

	// ��ʽ��ʱ��
	time(&timeNow);
	wchar_t* pTimeNow = _wctime(&timeNow);

	fputws(lpszStr, pFile);
	fputws(L"\n", pFile);
	fputws(pTimeNow, pFile);
	fputws(L"\r\n", pFile);		

	fclose(pFile);
	pFile = NULL;

	return TRUE;
}


/*
	��  ��: lpszFileName(const wchar_t*) - [in]��־�ļ�·��
	��  ��: -
	��  ��: ɾ����־�ļ�
*/
void DelFile(const wchar_t* lpszFileName)
{
	errno_t err;

	int nDelRet = _wremove(lpszFileName);

	if (-1 == nDelRet)
	{
		_get_errno(&err);
		wprintf(L"Cannot delete the file. Error code is %d\n", err);
	}
}


/*
	��  ��: lpszFileName(const wchar_t*) - [in]��־�ļ�·��
	��  ��: -
	��  ��: �����־�ļ�
*/
void ClearFile(const wchar_t* lpszFileName)
{
	// File not found
	if (-1 == _waccess(lpszFileName, 0))
	{
		return;
	}

	// an weird way
	FILE* pFile = _wfopen(lpszFileName, L"w");
	_ASSERT(pFile != NULL);

	fclose(pFile);
	pFile = NULL;
}