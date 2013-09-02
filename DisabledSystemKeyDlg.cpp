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


// CDisabledSystemKeyDlg �Ի���

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
	// CheckBox����Ϣӳ��
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHKCSE, IDC_CHKCAD, OnCheckStateChange)
	// Menu��Ϣӳ��
	ON_COMMAND_RANGE(ID_MNUDLG, ID_MNUEXIT, OnPopMenuCommand)
	// Tray����Ϣӳ��
	ON_MESSAGE(WM_MY_TRAY, OnTrayProc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CDisabledSystemKeyDlg ��Ϣ�������

BOOL CDisabledSystemKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// KeyState Class
	// �������ΰ����������״̬
	g_pKeyState = new CKeyState;
	ASSERT(g_pKeyState);

	// ��������ļ��Ƿ����
	if (!g_pKeyState->IsSettingFileExist())
	{
		g_pKeyState->StandardDisable();
	}
	else
	{
		g_pKeyState->GetSetting();
	}

	//��װȫ�ּ��̹���
	ms_hHookWnd = ::SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, AfxGetInstanceHandle(), 0);
	
	if (!ms_hHookWnd)
	{
		AfxMessageBox(_T("���Ӱ�װʧ��,�����޷���������"));
		EndDialog(1);
	}

	// ��ʼ��Զ�߳�ע�������
	m_pInject = new CRemoteInject();
	ASSERT(m_pInject);

	// DLL���м��
	if (!m_pInject->IsDllFileExit())
	{
		AfxMessageBox(_T("�ؼ�DLL������, �����޷���������!"));
		EndDialog(TRUE);
	}

	// ���ϵͳ����
	m_pTray = new CTray(IDR_MAINFRAME, WM_MY_TRAY);
	ASSERT(m_pTray);
	m_uTaskMsg = m_pTray->Create(this, GetIcon(1), _T("DisabledSystemKey - �Ѿ�����"));

	// ͬ����ѡ��
	ReadyCheck();

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}



// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDisabledSystemKeyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}



//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CDisabledSystemKeyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//=========================================
// ������: ~CDisabledSystemKeyDlg
// ��  ��: -
// ��  ��: -
// ��  ��: Dialog����������
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
// ������: ReadyCheck
// ��  ��: -
// ��  ��: -
// ��  ��: ���ø�ѡ��״̬
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
// ������: OnCheckStateChange
// ��  ��: uID(UINT) - �ؼ���ID��
// ��  ��: -
// ��  ��: ͬ����ѡ��״̬����Ӧ����
//=========================================
void CDisabledSystemKeyDlg::OnCheckStateChange(UINT uID)
{
	CButton *pChk = (CButton*)GetDlgItem(uID);
	ASSERT(pChk);
	// ����IDͬ����ѡ�����Ӧ����
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
			// ���˲�ϲ����һ��δ���д������
			RemoteInject((pChk->GetCheck() != 0));
			break;

	  default:
			break;
	}

	pChk = NULL;
}



//=========================================
// ������: RemoteInject
// ��  ��: bChkStatus(bool) - ѡ��:TRUE, δѡ��:FALSE
// ��  ��: -
// ��  ��: ע��/ж��Զ��DLL
//=========================================
void CDisabledSystemKeyDlg::RemoteInject(bool bChkStatus)
{
	BOOL bRet;

	if (bChkStatus)
	{
		bRet = m_pInject->Inject();

		if (!bRet)
		{
			AfxMessageBox(_T("DLLע��ʧ��,��ȷ��MFC���п���ȷע��!"));
			return;
		}
	}
	else
	{
		bRet = m_pInject->UnInject();

		if (!bRet)
		{
			AfxMessageBox(_T("����δ֪ԭ��,DLLж��ʧ��,��ʹ����������ж��DLL!"));
			return;
		}
	}
}



//========================================
// ������: HookProc
// ��  ��: nCode(int) - ���Ӷ���
//				 wParam(WPARAM) - ��Ϣ����
//				 lParam(LPARAM) - ��Ϣ��ֵ
// ��  ��: LRESULT CALLBACK
// ��  ��: ���̹����ӳ�
//========================================
HHOOK CDisabledSystemKeyDlg::ms_hHookWnd;
LRESULT CALLBACK	CDisabledSystemKeyDlg::HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// ��m_bIsWorking��Ϊ���ο���
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
				// д����ǰ��,��ֿ���Ч��-_-!
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
				// ������ж�Shift�Ƿ���,��ôCtrl+Shift+EscҲ��������
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
// ������: OnTrayProc
// ��  ��: wParam(WPARAM)
//				 lParam(LPARAM)
// ��  ��: LRESULT
// ��  ��: ��ȡ/���浱ǰ�������ڵ�·��
//=========================================
LRESULT CDisabledSystemKeyDlg::OnTrayProc(WPARAM wParam, LPARAM lParam)
{
	return m_pTray->OnTrayProc(wParam, lParam);
}



//=========================================
// ������: WindowProc
// ��  ��: message(UINT)
//				 wParam(WPARAM)
//				 lParam(LPARAM)
// ��  ��: LRESULT
// ��  ��: ��ȡ/���浱ǰ�������ڵ�·��
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

		// ���һ��Ҫ��,������Ϣ����CDialog,�����ֻᱻShow����
		return TRUE;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}



//=========================================
// ������: EnableCheckBox
// ��  ��: bEnable(bool) - TRUE:����, FALSE:����
// ��  ��: -
// ��  ��: ���û���ø�ѡ��
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
// ������: OnPopMenuCommand
// ��  ��: uID(UINT) - �˵���ID��
// ��  ��: -
// ��  ��: ����ʽ�˵�
//=========================================
void CDisabledSystemKeyDlg::OnPopMenuCommand(UINT uID)
{
	switch (uID)
	{ 
		case ID_MNUDLG:
			if (!IsWindowVisible())	// ������
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
// ������: OnOK
// ��  ��: -
// ��  ��: -
// ��  ��: ����Enter�˳���Ϣ
//=========================================
void CDisabledSystemKeyDlg::OnOK()
{
	// ���ΰ�Enter�˳�Dialog
	// ��������ͬ���ķ�������OnCancel֮��
	// ���ְ������XҲ�����˳�-_-!
	// �������ȫ�����εĻ�,���������İ�����Ϣ

	//CDialog::OnOK();
}

