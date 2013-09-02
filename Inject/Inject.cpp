/**************************************************

** Project:Disabled System Key

** File:Inject.cpp

** Edition:v1.0.0 Final

** Coder:KingsamChen [MDSA Group]

** Last Modify:2009-1-1

**************************************************/


#include <Windows.h>
#include <windef.h>
#include <tchar.h>
#include <WinBase.h>
#include "logfile.h"


// 全局变量
HANDLE hThread = NULL;
HWND hSASWnd = NULL;
long lOldProc = NULL;

// 普通函数
BOOL ThreadLoad();
BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam);
LRESULT CALLBACK SASWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//=========================================
// 函数名: DllMain
// 输  入: hinstDLL(HINSTANCE) - DLL的句柄
//				 fdwReason(DWORD) - DLL入口函数调用原因
//				 lpvReserved(LPVOID) - 保留符
// 输  出: BOOL - 成功1, 失败0
// 功  能: 获取/保存当前程序所在的路径
//=========================================
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{ 
		case DLL_PROCESS_ATTACH:
			// 创建线程, 在新线程里干干净点-_-!
			// 可能不创建线程也能够做到
			hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadLoad, NULL, 0, NULL);
			break;
	   
		case DLL_PROCESS_DETACH:
			if (lOldProc)
			{
				SetWindowLong(hSASWnd, GWL_WNDPROC, (long)lOldProc);
			}
			CloseHandle(hThread);
			break; 
	}

	return TRUE;
}


//=========================================
// 函数名: ThreadLoad
// 输  入: -
// 输  出: BOOL - 成功:TRUE, 失败:FALSE
// 功  能: 安装SAS窗口的子类化
//=========================================
BOOL ThreadLoad()
{
	HDESK hDeskWnd = NULL;

	// 打开桌面
	// 枚举窗口找到SAS Window. MS这里也可以直接用FindWindow, 你自己可以试下
	hDeskWnd = ::OpenDesktop(_T("winlogon"), 0, FALSE, GENERIC_READ);

	::EnumDesktopWindows(hDeskWnd, (WNDENUMPROC)EnumWindowProc, 0);
	
	if (hSASWnd)
	{
		// 安装子类化
		lOldProc = (long)::SetWindowLong(hSASWnd, GWL_WNDPROC, (long)SASWndProc);
	}

	// 清理
	CloseDesktop(hDeskWnd);
	return TRUE;
}



//=========================================
// 函数名: EnumWindowProc
// 输  入: hWnd(HWND) - Top-Level窗口的句柄
//		   lParam(LPARAM) - 其他函数传过来的值
// 输  出: BOOL - 继续枚举:TRUE, 停止:FALSE
// 功  能: 获取Winlogon窗口的句柄
//=========================================
BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	TCHAR lpszWinText[256] = {0};
	bool bFound = FALSE;
	const int MAXCOUNT = 256;	

	::GetWindowText(hWnd, lpszWinText, MAXCOUNT);

	bFound = (0 == lstrcmpi(lpszWinText, _T("SAS Window")));

	// 找到了目标窗口
	if (bFound)
	{
		hSASWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}



//=========================================
// 函数名: SASWndProc
// 输  入: hWnd(HWND)
//				 uMsg(UINT)
//				 wParam(WPARAM)
//				 lParam(LPARAM)
// 输  出: LRESULT CALLBACK
// 功  能: SAS窗口子类消息过程
//=========================================
LRESULT CALLBACK SASWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 屏蔽CTRL+ALT+DEL
	if (WM_HOTKEY == uMsg)
	{
		DWORD dwKey = HIWORD(lParam);
		DWORD dwModify = LOWORD(lParam);
		bool IsCtrlDown  = ((dwModify & VK_CONTROL) != 0);
		bool IsAltDown   = ((dwModify & VK_MENU) != 0);
    
		if ((VK_DELETE == dwKey) && IsCtrlDown && IsAltDown)
		{
			return 1;  // kill the message
		}
	}

	return CallWindowProc((WNDPROC)lOldProc, hWnd, uMsg, wParam, lParam);
}



