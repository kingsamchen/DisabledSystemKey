/**************************************************

** Project:Disabled System Key

** File:DisabledSystemKey.cpp

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-19

**************************************************/


#include "stdafx.h"
#include "DisabledSystemKey.h"
#include "DisabledSystemKeyDlg.h"
#include ".\disabledsystemkey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDisabledSystemKeyApp

BEGIN_MESSAGE_MAP(CDisabledSystemKeyApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDisabledSystemKeyApp 构造

CDisabledSystemKeyApp::CDisabledSystemKeyApp()
{
	m_hMutex = NULL;
}


// 唯一的一个 CDisabledSystemKeyApp 对象

CDisabledSystemKeyApp theApp;

// 全局的, 否则无法在钩子的回调函数中访问
CKeyState *g_pKeyState;

// CDisabledSystemKeyApp 初始化

BOOL CDisabledSystemKeyApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// 互斥对象防止重复运行
	// 互斥名采用GUID产生
	LPCTSTR lpszMutexName = _T("A4137FC8-DABC-4A2F-9B1C-226D075BB02C");

	m_hMutex = ::CreateMutex(NULL, FALSE, lpszMutexName);
  DWORD dwRet = ::GetLastError();

	if (m_hMutex)
	{
		// 重复运行
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			::CloseHandle(m_hMutex);
			AfxMessageBox(_T("已经有一个程序在运行!"));
			return FALSE;
		}
	}
	else
	{
		::CloseHandle(m_hMutex);
		AfxMessageBox(_T("创建互斥对象失败!"));
		return FALSE;
	}

	// 创建Dialog

	CDisabledSystemKeyDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}



//=========================================
// 函数名: ExitInstance
// 输  入: -
// 输  出: int
// 功  能: 退出实例时清理全局资源
//=========================================
int CDisabledSystemKeyApp::ExitInstance()
{
	::CloseHandle(m_hMutex);

	return CWinApp::ExitInstance();
}
