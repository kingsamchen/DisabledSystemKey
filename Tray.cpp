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
// ������: CTray
// ��  ��: uID(UINT) - ָʾͼ��ID��
//				 uMessage(UINT) - Tray��Ϣ��
// ��  ��: -
// ��  ��: CTray���캯��
//=========================================
CTray::CTray(UINT uID, UINT uMessage)
{
	m_Icon.cbSize = sizeof(NOTIFYICONDATA);

	// ����ͼ���ID��,�����ж������ĸ�ͼ�귢���Ĳ���
	// ֮����Ҫ�ò��������趨,����Ϊ��ֹ�ڶ��ͼ����
	// Ū��ͼ�꼰ID�Ķ�Ӧ��ϵ
	m_Icon.uID = uID;
	m_Icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	// ����Message Map����Ϣ
	// ����,ֻ�ܲ������еİ취
	m_Icon.uCallbackMessage = uMessage;
}



//=========================================
// ������: ~CTray
// ��  ��: -
// ��  ��: -
// ��  ��: CTray��������
//=========================================
CTray::~CTray()
{
	DelIcon();	
}



//=========================================
// ������: Create
// ��  ��: pWnd(CWnd*) - Ҫ����ͼ���CWnd����
//				 hIcon(HICON) - ͼ����
//				 lpszTipText(LPCTSTR) - ������ʾ
// ��  ��: UINT - �������ؽ���Ϣ
// ��  ��: ��������
//				 ע���������ؽ���Ϣ
//=========================================
UINT CTray::Create(CWnd *pWnd, HICON hIcon, LPCTSTR lpszTipText)
{
	// ֮���Բ�ֱ����pWnd->m_hWnd����Ϊ
	// GetSafeHwnd()��ȫ.�˼Ҷ���Safe��- -!
	m_Icon.hWnd = pWnd->GetSafeHwnd();

	// ���GetSafeHwnd����NULL,��ASSERTһ��
	// �˺����Debug��Ч
	ASSERT(m_Icon.hWnd);
	ASSERT(hIcon);
	m_Icon.hIcon = hIcon;
	_tcscpy(m_Icon.szTip, lpszTipText);

	// �����ʾͼ��ʧ��,��Ҳ��������
	BOOL bRet =	ShowIcon();
	ASSERT(bRet);
	UINT uRet = RegisterTaskbarMessage();
	ASSERT(uRet);
	
	return uRet;
}



//=========================================
// ������: ShowIcon
// ��  ��: -
// ��  ��: -
// ��  ��: ��ʾͼ��
//=========================================
BOOL CTray::ShowIcon()
{
	BOOL bRet = Shell_NotifyIcon(NIM_ADD, &m_Icon);
	
	return bRet;
}



//=========================================
// ������: DelIcon
// ��  ��: -
// ��  ��: -
// ��  ��: ɾ��ͼ��
//=========================================
BOOL CTray::DelIcon()
{
	BOOL bRet = Shell_NotifyIcon(NIM_DELETE, &m_Icon);
	
	return bRet;
}



//=========================================
// ������: RegisterTaskbarMessage
// ��  ��: -
// ��  ��: -
// ��  ��: ע���������ؽ���Ϣ
//=========================================
UINT CTray::RegisterTaskbarMessage()
{
	return RegisterWindowMessage(_T("TaskBarCreated"));
}



//=========================================
// ������: ChangeIcon
// ��  ��: hIcon(HICON) - ͼ����
// ��  ��: -
// ��  ��: ����ͼ��
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
// ������: OnTrayProc
// ��  ��: wParam(WPARAM) - ͼ��ID��Ϣ
//				 lParam(LPARAM) - �����Ϣ
// ��  ��: LRESULT
// ��  ��: ������Ϣ������
//=========================================
LRESULT CTray::OnTrayProc(WPARAM wParam, LPARAM lParam)
{
	if (wParam !=IDR_MAINFRAME) 
		return 0;

	switch (lParam)
	{ 
	  case WM_LBUTTONDOWN:
			if (g_pKeyState->GetWorkingSate())	// ������״̬
			{
				HICON hIcon =	AfxGetApp()->LoadIcon(IDR_XMAINFRAME);
				ChangeIcon(hIcon, _T("DisabledSystemKey - ��ͣ����"));

				// �л����������״̬
				g_pKeyState->SetWorkingState(FALSE);

				// ͬ����ѡ��
				// ע����ʹ��Dialog���Ա�����ķ���
				CDisabledSystemKeyDlg *pDlg = (CDisabledSystemKeyDlg*)AfxGetMainWnd();
				pDlg->EnableCheckBox(FALSE);
				pDlg = NULL;
			}
			else
			{
				HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
				ChangeIcon(hIcon, _T("DisabledSystemKey - �Ѿ�����"));

				g_pKeyState->SetWorkingState(TRUE);

				CDisabledSystemKeyDlg *pDlg = (CDisabledSystemKeyDlg*)AfxGetMainWnd();
				pDlg->EnableCheckBox(TRUE);
				pDlg = NULL;
			}

			break;

		case WM_RBUTTONDOWN:
			CMenu mnu;
			
			//���ز˵�.ע��:������ص���[ѡ��]�˵�
			if (mnu.LoadMenu(IDR_MNUPOP))	
			{
				// ��ȡ�Ӳ˵�
				// ע��˵��ļ����ϵ
				CMenu *pMnuOption =	mnu.GetSubMenu(0);
				CPoint	*pPostion = new CPoint;
				ASSERT(pMnuOption);
				ASSERT(pPostion);				

				GetCursorPos(pPostion);

				// �Ѵ�����ǰ,��ֹ�˵�����������������
				// �˵�����ʧ������
				AfxGetMainWnd()->SetForegroundWindow();
				pMnuOption->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pPostion->x, 
																	 pPostion->y, AfxGetMainWnd());

				delete pPostion;
				pPostion = NULL;
				pMnuOption = NULL;
			}

			// MS MSDN˵LoadMenu֮��ҪDestroyMenu
			mnu.DestroyMenu();
			break;

	  //default:
			//break;
	}

	return 1;
}