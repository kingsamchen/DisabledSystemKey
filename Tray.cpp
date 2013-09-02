/**************************************************

** Project:Disabled System Key

** File:Tray.cpp

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-31

**************************************************/


#include "StdAfx.h"
#include ".\tray.h"
#include "DisabledSystemKeyDlg.h"


//=========================================
// 函数名: CTray
// 输  入: uID(UINT) - 指示图标ID号
//				 uMessage(UINT) - Tray消息名
// 输  出: -
// 功  能: CTray构造函数
//=========================================
CTray::CTray(UINT uID, UINT uMessage)
{
	m_Icon.cbSize = sizeof(NOTIFYICONDATA);

	// 程序图标的ID号,用于判断是由哪个图标发出的操作
	// 之所以要用参数传入设定,是因为防止在多个图标中
	// 弄乱图标及ID的对应关系
	m_Icon.uID = uID;
	m_Icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// 用来Message Map的消息
	// 无奈,只能采用折中的办法
	m_Icon.uCallbackMessage = uMessage;
}



//=========================================
// 函数名: ~CTray
// 输  入: -
// 输  出: -
// 功  能: CTray析构函数
//=========================================
CTray::~CTray()
{
	DelIcon();	
}



//=========================================
// 函数名: Create
// 输  入: pWnd(CWnd*) - 要创建图标的CWnd对象
//				 hIcon(HICON) - 图标句柄
//				 lpszTipText(LPCTSTR) - 托盘提示
// 输  出: UINT - 任务栏重建消息
// 功  能: 创建托盘
//				 注册任务栏重建消息
//=========================================
UINT CTray::Create(CWnd *pWnd, HICON hIcon, LPCTSTR lpszTipText)
{
	// 之所以不直接用pWnd->m_hWnd是因为
	// GetSafeHwnd()安全.人家都带Safe了- -!
	m_Icon.hWnd = pWnd->GetSafeHwnd();

	// 如果GetSafeHwnd返回NULL,就ASSERT一下
	// 此宏仅对Debug有效
	ASSERT(m_Icon.hWnd);
	ASSERT(hIcon);
	m_Icon.hIcon = hIcon;
	_tcscpy(m_Icon.szTip, lpszTipText);

	// 如果显示图标失败,那也不用玩了
	BOOL bRet =	ShowIcon();
	ASSERT(bRet);
	UINT uRet = RegisterTaskbarMessage();
	ASSERT(uRet);
	
	return uRet;
}



//=========================================
// 函数名: ShowIcon
// 输  入: -
// 输  出: -
// 功  能: 显示图标
//=========================================
BOOL CTray::ShowIcon()
{
	BOOL bRet = Shell_NotifyIcon(NIM_ADD, &m_Icon);
	
	return bRet;
}



//=========================================
// 函数名: DelIcon
// 输  入: -
// 输  出: -
// 功  能: 删除图标
//=========================================
BOOL CTray::DelIcon()
{
	BOOL bRet = Shell_NotifyIcon(NIM_DELETE, &m_Icon);
	
	return bRet;
}



//=========================================
// 函数名: RegisterTaskbarMessage
// 输  入: -
// 输  出: -
// 功  能: 注册任务栏重建消息
//=========================================
UINT CTray::RegisterTaskbarMessage()
{
	return RegisterWindowMessage(_T("TaskBarCreated"));
}



//=========================================
// 函数名: ChangeIcon
// 输  入: hIcon(HICON) - 图标句柄
// 输  出: -
// 功  能: 更改图标
//=========================================
void CTray::ChangeIcon(HICON hIcon, LPCTSTR lpszTipText)
{
	ASSERT(hIcon);
	m_Icon.hIcon = hIcon;
	_tcscpy(m_Icon.szTip, lpszTipText);
	BOOL bRet =	Shell_NotifyIcon(NIM_MODIFY, &m_Icon);
	ASSERT(bRet);
}



//=========================================
// 函数名: OnTrayProc
// 输  入: wParam(WPARAM) - 图标ID消息
//				 lParam(LPARAM) - 鼠标消息
// 输  出: LRESULT
// 功  能: 托盘消息处理函数
//=========================================
LRESULT CTray::OnTrayProc(WPARAM wParam, LPARAM lParam)
{
	if (wParam !=IDR_MAINFRAME) 
		return 0;

	switch (lParam)
	{ 
	  case WM_LBUTTONDOWN:
			if (g_pKeyState->GetWorkingSate())	// 检查程序状态
			{
				HICON hIcon =	AfxGetApp()->LoadIcon(IDR_XMAINFRAME);
				ChangeIcon(hIcon, _T("DisabledSystemKey - 暂停屏蔽"));

				// 切换程序的屏蔽状态
				g_pKeyState->SetWorkingState(FALSE);

				// 同步复选框
				// 注意下使用Dialog类成员函数的方法
				CDisabledSystemKeyDlg *pDlg = (CDisabledSystemKeyDlg*)AfxGetMainWnd();
				pDlg->EnableCheckBox(FALSE);
				pDlg = NULL;
			}
			else
			{
				HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
				ChangeIcon(hIcon, _T("DisabledSystemKey - 已经屏蔽"));

				g_pKeyState->SetWorkingState(TRUE);

				CDisabledSystemKeyDlg *pDlg = (CDisabledSystemKeyDlg*)AfxGetMainWnd();
				pDlg->EnableCheckBox(TRUE);
				pDlg = NULL;
			}

			break;

		case WM_RBUTTONDOWN:
			CMenu mnu;
			
			//加载菜单.注意:这里加载的是[选项]菜单
			if (mnu.LoadMenu(IDR_MNUPOP))	
			{
				// 获取子菜单
				// 注意菜单的级别关系
				CMenu *pMnuOption =	mnu.GetSubMenu(0);
				CPoint	*pPostion = new CPoint;
				ASSERT(pMnuOption);
				ASSERT(pPostion);				

				GetCursorPos(pPostion);

				// 把窗体提前,防止菜单弹出后点击其他区域
				// 菜单不消失的问题
				AfxGetMainWnd()->SetForegroundWindow();
				pMnuOption->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pPostion->x, 
																	 pPostion->y, AfxGetMainWnd());

				delete pPostion;
				pPostion = NULL;
				pMnuOption = NULL;
			}

			// MS MSDN说LoadMenu之后要DestroyMenu
			mnu.DestroyMenu();
			break;

	  //default:
			//break;
	}

	return 1;
}