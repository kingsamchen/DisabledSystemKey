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


// ȫ�ֱ���
HANDLE hThread = NULL;
HWND hSASWnd = NULL;
long lOldProc = NULL;

// ��ͨ����
BOOL ThreadLoad();
BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam);
LRESULT CALLBACK SASWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//=========================================
// ������: DllMain
// ��  ��: hinstDLL(HINSTANCE) - DLL�ľ��
//				 fdwReason(DWORD) - DLL��ں�������ԭ��
//				 lpvReserved(LPVOID) - ������
// ��  ��: BOOL - �ɹ�1, ʧ��0
// ��  ��: ��ȡ/���浱ǰ�������ڵ�·��
//=========================================
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{ 
		case DLL_PROCESS_ATTACH:
			// �����߳�, �����߳���ɸɾ���-_-!
			// ���ܲ������߳�Ҳ�ܹ�����
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
// ������: ThreadLoad
// ��  ��: -
// ��  ��: BOOL - �ɹ�:TRUE, ʧ��:FALSE
// ��  ��: ��װSAS���ڵ����໯
//=========================================
BOOL ThreadLoad()
{
	HDESK hDeskWnd = NULL;

	// ������
	// ö�ٴ����ҵ�SAS Window. MS����Ҳ����ֱ����FindWindow, ���Լ���������
	hDeskWnd = ::OpenDesktop(_T("winlogon"), 0, FALSE, GENERIC_READ);

	::EnumDesktopWindows(hDeskWnd, (WNDENUMPROC)EnumWindowProc, 0);
	
	if (hSASWnd)
	{
		// ��װ���໯
		lOldProc = (long)::SetWindowLong(hSASWnd, GWL_WNDPROC, (long)SASWndProc);
	}

	// ����
	CloseDesktop(hDeskWnd);
	return TRUE;
}



//=========================================
// ������: EnumWindowProc
// ��  ��: hWnd(HWND) - Top-Level���ڵľ��
//		   lParam(LPARAM) - ����������������ֵ
// ��  ��: BOOL - ����ö��:TRUE, ֹͣ:FALSE
// ��  ��: ��ȡWinlogon���ڵľ��
//=========================================
BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	TCHAR lpszWinText[256] = {0};
	bool bFound = FALSE;
	const int MAXCOUNT = 256;	

	::GetWindowText(hWnd, lpszWinText, MAXCOUNT);

	bFound = (0 == lstrcmpi(lpszWinText, _T("SAS Window")));

	// �ҵ���Ŀ�괰��
	if (bFound)
	{
		hSASWnd = hWnd;
		return FALSE;
	}

	return TRUE;
}



//=========================================
// ������: SASWndProc
// ��  ��: hWnd(HWND)
//				 uMsg(UINT)
//				 wParam(WPARAM)
//				 lParam(LPARAM)
// ��  ��: LRESULT CALLBACK
// ��  ��: SAS����������Ϣ����
//=========================================
LRESULT CALLBACK SASWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// ����CTRL+ALT+DEL
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



