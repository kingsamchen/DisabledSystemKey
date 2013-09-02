// DlgAbout.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DisabledSystemKey.h"
#include "DlgAbout.h"
#include ".\dlgabout.h"


// CDlgAbout �Ի���

IMPLEMENT_DYNAMIC(CDlgAbout, CDialog)
CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAbout::IDD, pParent)
{
}

CDlgAbout::~CDlgAbout()
{
}

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BLOG, m_BlogLink);
}


BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	
	ON_BN_CLICKED(IDC_CMDEXIT, OnBnClickedCmdexit)
END_MESSAGE_MAP()

BOOL CDlgAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s(_T("���� "));	

	SetIcon(AfxGetMainWnd()->GetIcon(TRUE), TRUE);
	
	CStatic *pIcon = (CStatic*)GetDlgItem(IDC_PICICON);
	pIcon->SetIcon(GetIcon(TRUE));

	SetDlgItemText(IDC_CAPNAME, AfxGetApp()->m_pszAppName);
	
	s += AfxGetApp()->m_pszAppName;
	SetWindowText(s);

	m_BlogLink.SetTextColor(RGB(0, 0, 255));
	m_BlogLink.EnableTrack(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgAbout::OnBnClickedCmdexit()
{
	EndDialog(TRUE);
}
