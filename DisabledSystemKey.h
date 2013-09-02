/**************************************************

** Project:Disabled System Key

** File:DisabledSystemKey.h

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-19

**************************************************/


#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
#include "KeyState.h"

// CDisabledSystemKeyApp:
// 有关此类的实现，请参阅 DisabledSystemKey.cpp
//

class CDisabledSystemKeyApp : public CWinApp
{
public:
	CDisabledSystemKeyApp();

// 重写
public:
	virtual BOOL InitInstance();

private:
	HANDLE m_hMutex;	

// 实现

	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
};

extern CDisabledSystemKeyApp theApp;
extern CKeyState *g_pKeyState;