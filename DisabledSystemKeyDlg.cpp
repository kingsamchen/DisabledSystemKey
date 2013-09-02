/**************************************************

** Project:Disabled System Key

** File:DisabledSystemKeyDlg.cpp

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-31

**************************************************/


#include "stdafx.h"
#include "DisabledSystemKey.h"
#include ".\disabledsystemkeydlg.h"
#include "DlgAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDisabledSystemKeyDlg 对话框

CDisabledSystemKeyDlg::CDisabledSystemKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisabledSystemKeyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



void CDisabledSystemKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CDisabledSystemKeyDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// CheckBox的消息映射
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHKCSE, IDC_CHKCAD, OnCheckStateChange)
	// Menu消息映射
	ON_COMMAND_RANGE(ID_MNUDLG, ID_MNUEXIT, OnPopMenuCommand)
	// Tray的消息映射
	ON_MESSAGE(WM_MY_TRAY, OnTrayProc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CDisabledSystemKeyDlg 消息处理程序

BOOL CDisabledSystemKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// KeyState Class
	// 保存屏蔽按键及程序的状态
	g_pKeyState = new CKeyState;
	ASSERT(g_pKeyState);

	// 检查设置文件是否存在
	if (!g_pKeyState->IsSettingFileExist())
	{
		g_pKeyState->StandardDisable();
	}
	else
	{
		g_pKeyState->GetSetting();
	}

	//安装全局键盘钩子
	ms_hHookWnd = ::SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, AfxGetInstanceHandle(), 0);
	
	if (!ms_hHookWnd)
	{
		AfxMessageBox(_T("钩子安装失败,程序无法继续运行"));
		EndDialog(1);
	}

	// 初始化远线程注入类对象
	m_pInject = new CRemoteInject();
	ASSERT(m_pInject);

	// DLL例行检查
	if (!m_pInject->IsDllFileExit())
	{
		AfxMessageBox(_T("关键DLL不存在, 程序无法继续运行!"));
		EndDialog(TRUE);
	}

	// 添加系统托盘
	m_pTray = new CTray(IDR_MAINFRAME, WM_MY_TRAY);
	ASSERT(m_pTray);
	m_uTaskMsg = m_pTray->Create(this, GetIcon(1), _T("DisabledSystemKey - 已经屏蔽"));

	// 同步复选框
	ReadyCheck();

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}



// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDisabledSystemKeyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}



//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CDisabledSystemKeyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//=========================================
// 函数名: ~CDisabledSystemKeyDlg
// 输  入: -
// 输  出: -
// 功  能: Dialog的析构函数
//=========================================
CDisabledSystemKeyDlg::~CDisabledSystemKeyDlg(void)
{
	delete m_pInject;
	m_pInject = NULL;

	BOOL bRet = ::UnhookWindowsHookEx(ms_hHookWnd);
	ASSERT(bRet);

	delete g_pKeyState;
	g_pKeyState = NULL;

	delete m_pTray;
	m_pTray = NULL;
}



//=========================================
// 函数名: ReadyCheck
// 输  入: -
// 输  出: -
// 功  能: 设置复选框状态
//=========================================
void CDisabledSystemKeyDlg::ReadyCheck()
{
	CButton *pChk;

	// Ctrl+Shift+Esc
	pChk = (CButton*)GetDlgItem(IDC_CHKCSE);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisCtShEsc);

	// LWin
	pChk = (CButton*)GetDlgItem(IDC_CHKLW);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisLWin);
	
	// RWin
	pChk = (CButton*)GetDlgItem(IDC_CHKRW);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisRWin);

	// CapsLock
	pChk = (CButton*)GetDlgItem(IDC_CHKCAL);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisCapsLock);

	// Ctrl+Esc
	pChk = (CButton*)GetDlgItem(IDC_CHKCE);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisCtEs);

	// Ctrl+Spc
	pChk = (CButton*)GetDlgItem(IDC_CHKCSPC);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisCtSpc);

	// Alt+Tap
	pChk = (CButton*)GetDlgItem(IDC_CHKAT);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisAltTab);

	// Ctrl+Shift
	pChk = (CButton*)GetDlgItem(IDC_CHKCS);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisCtShift);

	// All
	pChk = (CButton*)GetDlgItem(IDC_CHKALL);
	ASSERT(pChk);
	pChk->SetCheck(g_pKeyState->m_bDisAll);

	// CTRL+ALT+DELETE
	pChk = (CButton*)GetDlgItem(IDC_CHKCAD);
	ASSERT(pChk);
	pChk->SetCheck(m_pInject->IsDllExist());

	pChk = NULL;
}



//=========================================
// 函数名: OnCheckStateChange
// 输  入: uID(UINT) - 控件的ID号
// 输  出: -
// 功  能: 同步复选框状态和相应变量
//=========================================
void CDisabledSystemKeyDlg::OnCheckStateChange(UINT uID)
{
	CButton *pChk = (CButton*)GetDlgItem(uID);
	ASSERT(pChk);
	// 根据ID同步复选框和相应变量
	switch (uID)
	{ 
		case IDC_CHKCSE:
			g_pKeyState->m_bDisCtShEsc = (pChk->GetCheck() != 0);
			break;

	  case IDC_CHKLW:
			g_pKeyState->m_bDisLWin = (pChk->GetCheck() != 0);			
	    break;

		case IDC_CHKRW:
			g_pKeyState->m_bDisRWin = (pChk->GetCheck() != 0);
			break;

		case IDC_CHKCAL:
			g_pKeyState->m_bDisCapsLock = (pChk->GetCheck() != 0);
			break;

		case IDC_CHKCE:
			g_pKeyState->m_bDisCtEs = (pChk->GetCheck() != 0);
			break;
		
		case IDC_CHKCSPC:
			g_pKeyState->m_bDisCtSpc = (pChk->GetCheck() != 0);
			break;
	
		case IDC_CHKAT:
			g_pKeyState->m_bDisAltTab = (pChk->GetCheck() != 0);
			break;

		case IDC_CHKCS:
			g_pKeyState->m_bDisCtShift = (pChk->GetCheck() != 0);
			break;

		case IDC_CHKALL:
			g_pKeyState->m_bDisAll = (pChk->GetCheck() != 0);
			break;
		
		case IDC_CHKCAD:
			// 个人不喜欢把一大段代码写在这里
			RemoteInject((pChk->GetCheck() != 0));
			break;

	  default:
			break;
	}

	pChk = NULL;
}



//=========================================
// 函数名: RemoteInject
// 输  入: bChkStatus(bool) - 选中:TRUE, 未选中:FALSE
// 输  出: -
// 功  能: 注入/卸载远程DLL
//=========================================
void CDisabledSystemKeyDlg::RemoteInject(bool bChkStatus)
{
	BOOL bRet;

	if (bChkStatus)
	{
		bRet = m_pInject->Inject();

		if (!bRet)
		{
			AfxMessageBox(_T("DLL注入失败,请确定MFC运行库正确注册!"));
			return;
		}
	}
	else
	{
		bRet = m_pInject->UnInject();

		if (!bRet)
		{
			AfxMessageBox(_T("由于未知原因,DLL卸载失败,请使用其他工具卸载DLL!"));
			return;
		}
	}
}



//========================================
// 函数名: HookProc
// 输  入: nCode(int) - 钩子动作
//				 wParam(WPARAM) - 消息类型
//				 lParam(LPARAM) - 消息子值
// 输  出: LRESULT CALLBACK
// 功  能: 键盘钩子子程
//========================================
HHOOK CDisabledSystemKeyDlg::ms_hHookWnd;
LRESULT CALLBACK	CDisabledSystemKeyDlg::HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// 用m_bIsWorking作为屏蔽开关
	if (nCode >= 0 && g_pKeyState->GetWorkingSate())
	{
		PKBDLLHOOKSTRUCT pKeyBorad = (PKBDLLHOOKSTRUCT)lParam;

		switch (wParam)
		{
		  case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:

				// All
				// 写在最前面,充分考虑效率-_-!
				if ((pKeyBorad->vkCode) && (g_pKeyState->m_bDisAll))
				{
					return 1;
				}
				// LWin
				else if ((VK_LWIN == pKeyBorad->vkCode) && (g_pKeyState->m_bDisLWin))
				{
					return 1;
				}
				// RWin
				else if ((VK_RWIN == pKeyBorad->vkCode) && (g_pKeyState->m_bDisRWin))
				{
					return 1;
				}
				// CapsLock
				else if	((VK_CAPITAL == pKeyBorad->vkCode) && (g_pKeyState->m_bDisCapsLock))
				{
					return 1;
				}
				// Alt + Tab
				else if ((VK_TAB == pKeyBorad->vkCode) && (pKeyBorad->flags & LLKHF_ALTDOWN) &&
								 (g_pKeyState->m_bDisAltTab))
				{
					return 1;
				}
				// Ctrl + Esc
				// 如果不判断Shift是否按下,那么Ctrl+Shift+Esc也被屏蔽了
				else if ((VK_ESCAPE == pKeyBorad->vkCode) && (GetAsyncKeyState(VK_CONTROL) & KF_UP) &&
								 (!(GetAsyncKeyState(VK_SHIFT) & KF_UP)) && (g_pKeyState->m_bDisCtEs))
				{
					return 1;
				}
				// Ctrl + Spc
				else if ((VK_SPACE == pKeyBorad->vkCode) && (GetAsyncKeyState(VK_CONTROL) & KF_UP) &&
								 (g_pKeyState->m_bDisCtSpc))
				{
					return 1;
				}
				// Ctrl + Shift
				else if ((GetAsyncKeyState(VK_SHIFT) & KF_UP) && (GetAsyncKeyState(VK_CONTROL) & KF_UP) &&
								 (g_pKeyState->m_bDisCtShift))
				{
					return 1;
				}
				// Ctrl + Shift + Esc
				else if ((VK_ESCAPE == pKeyBorad->vkCode) && (GetAsyncKeyState(VK_SHIFT) & KF_UP) &&
								 (GetAsyncKeyState(VK_CONTROL) & KF_UP) && (g_pKeyState->m_bDisCtShEsc))
				{
					return 1;
				}
		    break;

		  default:
				break;
		}

	}

	return ::CallNextHookEx(ms_hHookWnd, nCode, wParam, lParam);
}



//=========================================
// 函数名: OnTrayProc
// 输  入: wParam(WPARAM)
//				 lParam(LPARAM)
// 输  出: LRESULT
// 功  能: 获取/保存当前程序所在的路径
//=========================================
LRESULT CDisabledSystemKeyDlg::OnTrayProc(WPARAM wParam, LPARAM lParam)
{
	return m_pTray->OnTrayProc(wParam, lParam);
}



//=========================================
// 函数名: WindowProc
// 输  入: message(UINT)
//				 wParam(WPARAM)
//				 lParam(LPARAM)
// 输  出: LRESULT
// 功  能: 获取/保存当前程序所在的路径
//=========================================
LRESULT CDisabledSystemKeyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_uTaskMsg == message)
	{
		m_pTray->ShowIcon();
	}	
	else if ((WM_SYSCOMMAND == message) && (SC_MINIMIZE == wParam))
	{
		ShowWindow(SW_HIDE);

		// 这个一定要加,否则消息到了CDialog,窗口又会被Show出来
		return TRUE;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}



//=========================================
// 函数名: EnableCheckBox
// 输  入: bEnable(bool) - TRUE:启用, FALSE:禁用
// 输  出: -
// 功  能: 启用或禁用复选框
//=========================================
void CDisabledSystemKeyDlg::EnableCheckBox(bool bEnable)
{
	CButton *pChk;

	for (int n = IDC_CHKCSE; n <= IDC_CHKCAD; n++)
	{
		pChk = (CButton*)GetDlgItem(n);
		ASSERT(pChk);
		pChk->EnableWindow(bEnable);
	}
	
	pChk = NULL;
}



//=========================================
// 函数名: OnPopMenuCommand
// 输  入: uID(UINT) - 菜单的ID号
// 输  出: -
// 功  能: 弹出式菜单
//=========================================
void CDisabledSystemKeyDlg::OnPopMenuCommand(UINT uID)
{
	switch (uID)
	{ 
		case ID_MNUDLG:
			if (!IsWindowVisible())	// 已隐藏
			{
				ShowWindow(SW_SHOWNORMAL);	
			}
			else
			{
				ShowWindow(SW_HIDE);
			}
	    break;

		case ID_MNUABOUT:
		{
			CDlgAbout	DlgAbout;
			DlgAbout.DoModal();
			break;
		}
		
		case ID_MNUEXIT:
			EndDialog(TRUE);
			break;			
	}

}



//=========================================
// 函数名: OnOK
// 输  入: -
// 输  出: -
// 功  能: 屏蔽Enter退出消息
//=========================================
void CDisabledSystemKeyDlg::OnOK()
{
	// 屏蔽按Enter退出Dialog
	// 但是我用同样的方法屏蔽OnCancel之后
	// 发现按窗体的X也不能退出-_-!
	// 如果你项全部屏蔽的话,请监听窗体的按键消息

	//CDialog::OnOK();
}

