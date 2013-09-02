/**************************************************

** Project:Disabled System Key

** File:DisabledSystemKey.h

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-19

**************************************************/


#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������
#include "KeyState.h"

// CDisabledSystemKeyApp:
// �йش����ʵ�֣������ DisabledSystemKey.cpp
//

class CDisabledSystemKeyApp : public CWinApp
{
public:
	CDisabledSystemKeyApp();

// ��д
public:
	virtual BOOL InitInstance();

private:
	HANDLE m_hMutex;	

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
};

extern CDisabledSystemKeyApp theApp;
extern CKeyState *g_pKeyState;