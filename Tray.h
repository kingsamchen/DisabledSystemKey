/**************************************************

** Project:Disabled System Key

** File:Tray.h

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-9-30

**************************************************/

#pragma once

#include "DisabledSystemKey.h"

class CTray
{
	public:
		CTray(UINT uID, UINT uMessage);
		~CTray();

	public:
		// 图标消息的处理函数
		LRESULT OnTrayProc(WPARAM wParam, LPARAM lParam);
		UINT Create(CWnd *pWnd, HICON hIcon, LPCTSTR lpszTipText);
		BOOL DelIcon();		
		BOOL ShowIcon();
		void ChangeIcon(HICON hIcon, LPCTSTR lpszTipText);

	private:
		UINT RegisterTaskbarMessage();
			
	private:
		/*typedef struct _NOTIFYICONDATA {
		    DWORD cbSize;
		    HWND hWnd;
		    UINT uID;
		    UINT uFlags;
		    UINT uCallbackMessage;
		    HICON hIcon;
		    TCHAR szTip[64];
		    DWORD dwState;
		    DWORD dwStateMask;
		    TCHAR szInfo[256];
		    union {
		        UINT uTimeout;
		        UINT uVersion;
		    };
		    TCHAR szInfoTitle[64];
		    DWORD dwInfoFlags;
		    GUID guidItem;
		} NOTIFYICONDATA, *PNOTIFYICONDATA;*/

		// 新版本的NOTIFYICONDATA结构,你可以参考MSDN,对此类进行加工
		NOTIFYICONDATA m_Icon;
};
