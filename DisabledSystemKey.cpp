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


// CDisabledSystemKeyApp ����

CDisabledSystemKeyApp::CDisabledSystemKeyApp()
{
	m_hMutex = NULL;
}


// Ψһ��һ�� CDisabledSystemKeyApp ����

CDisabledSystemKeyApp theApp;

// ȫ�ֵ�, �����޷��ڹ��ӵĻص������з���
CKeyState *g_pKeyState;

// CDisabledSystemKeyApp ��ʼ��

BOOL CDisabledSystemKeyApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	// ��������ֹ�ظ�����
	// ����������GUID����
	LPCTSTR lpszMutexName = _T("A4137FC8-DABC-4A2F-9B1C-226D075BB02C");

	m_hMutex = ::CreateMutex(NULL, FALSE, lpszMutexName);
  DWORD dwRet = ::GetLastError();

	if (m_hMutex)
	{
		// �ظ�����
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			::CloseHandle(m_hMutex);
			AfxMessageBox(_T("�Ѿ���һ������������!"));
			return FALSE;
		}
	}
	else
	{
		::CloseHandle(m_hMutex);
		AfxMessageBox(_T("�����������ʧ��!"));
		return FALSE;
	}

	// ����Dialog

	CDisabledSystemKeyDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



//=========================================
// ������: ExitInstance
// ��  ��: -
// ��  ��: int
// ��  ��: �˳�ʵ��ʱ����ȫ����Դ
//=========================================
int CDisabledSystemKeyApp::ExitInstance()
{
	::CloseHandle(m_hMutex);

	return CWinApp::ExitInstance();
}
