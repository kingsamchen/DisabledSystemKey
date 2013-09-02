/**************************************************

** Project:Disabled System Key

** File:KeyState.cpp

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-20

**************************************************/


#include "StdAfx.h"
#include ".\keystate.h"


//=========================================
// ������: CKeyState
// ��  ��: -
// ��  ��: -
// ��  ��: CKeyState���캯��
//=========================================
CKeyState::CKeyState()
{
	// ��ȡ·��
	bool bRet = GetSettingFilePath();
	ASSERT(bRet);
	// ��ʼΪTRUE
	m_bIsWorking = 1;
}



//=========================================
// ������: ~CKeyState
// ��  ��: -
// ��  ��: -
// ��  ��: CKeyState��������
//=========================================
CKeyState::~CKeyState()
{
	// ����ʱ���水����������
	// �����������Public.�����������Ҫʱ��������
	SaveSetting();
}



//=========================================
// ������: GetSettingFilePath
// ��  ��: -
// ��  ��: bool - �ɹ�TRUE, ʧ��FALSE
// ��  ��: ��ȡ/���������ļ���·��
//=========================================
bool CKeyState::GetSettingFilePath()
{
	TCHAR sFileName[MAX_PATH];

	if (!GetFullPathName(_T("Config.cfg"), MAX_PATH, sFileName, NULL))
	{
		return FALSE;		
	}	

	// ���������ļ�·��
	m_sPath.Format(_T("%s"), sFileName);

	return TRUE;
}



//=========================================
// ������: IsSettingFileExist
// ��  ��: -
// ��  ��: bool - ����TRUE, ������FALSE
// ��  ��: ��������ļ��Ƿ����
//=========================================
bool CKeyState::IsSettingFileExist()
{
	CFileFind SettingFile;

	// �����������,��˵���ļ�������
	// Ҳ����ʹ��CRT����_access,������Ҫ��Ӧ��io.h
	// ע��:FindFile֮����ʹ��Close�ر�����
	BOOL bRet = SettingFile.FindFile(m_sPath);
	SettingFile.Close();
	
	if (!bRet)
	{
		return FALSE;
	}
	
	return TRUE;
}



//=========================================
// ������: StandardDisable
// ��  ��: -
// ��  ��: -
// ��  ��: ʹ�ñ�׼��������ѡ��
//=========================================
void CKeyState::StandardDisable()
{
	// ��׼���ΰ���ѡ��
	// ����Ը�����Ҫ����
	m_bDisLWin = 1;
	m_bDisRWin = 1;
	m_bDisCapsLock = 1;
	m_bDisCtEs = 1;
	m_bDisCtSpc = 0;
	m_bDisCtShift = 0;
	m_bDisAltTab = 0;
	m_bDisAll = 0;
	m_bDisCtShEsc = 0;
}



//=========================================
// ������: GetWorkingSate
// ��  ��: -
// ��  ��: bool - ������TRUE, ��ͣFALSE
// ��  ��: ��ȡ����״̬
//=========================================
bool CKeyState::GetWorkingSate()
{
	return m_bIsWorking;
}



//=========================================
// ������: SetWorkingState
// ��  ��: bState(bool) - ����״̬����
// ��  ��: -
// ��  ��: ��������״̬
//=========================================
void CKeyState::SetWorkingState(bool bState)
{
	if (!bState)
		m_bIsWorking = 0;
	else
		m_bIsWorking = 1;		
}



//=========================================
// ������: GetValue
// ��  ��: lpSection(LPTSTR) - Section����
//         lpKeyName(LPTSTR) - Key����
//				 nDefault(int) - δ�ҵ�ʱ��Ĭ��ֵ
// ��  ��: bool - ����1, ͣ��0
// ��  ��: ��ȡ��Ӧ��������ѡ���ֵ
//=========================================
bool CKeyState::GetValue(LPTSTR lpSection, LPTSTR lpKeyName, int nDefault)
{
	// ��˵CWinApp::GetPrivateProfileInt�е㲻�ɾ�-��-||
	UINT uValue = ::GetPrivateProfileInt(lpSection, lpKeyName, nDefault, m_sPath);

	if (!uValue)
		return 0;	
	else
		return 1;
}



//=========================================
// ������: SetValue
// ��  ��: lpSection(LPTSTR) - Section����
//				 lpKeyName(LPTSTR) - Key����
//				 lpValue(LPTSTR) - ѡ��ֵ
// ��  ��: -
// ��  ��: ������Ӧ��������ѡ���ֵ
//=========================================
void CKeyState::SetValue(LPTSTR lpSection, LPTSTR lpKeyName, LPTSTR lpValue)
{
	::WritePrivateProfileString(lpSection, lpKeyName, lpValue, m_sPath);
}



//=========================================
// ������: SaveSetting
// ��  ��: -
// ��  ��: -
// ��  ��: �������а�����������
//=========================================
void CKeyState::SaveSetting()
{
	TCHAR *lpszSecName = _T("Setting");	// MS����Ӳ���const����ν��
	TCHAR chS[2] = {0};

	SetValue(lpszSecName, _T("DisabledLWin"), _itot(m_bDisLWin, chS, 10));
	SetValue(lpszSecName, _T("DisabledRWin"), _itot(m_bDisRWin, chS, 10));
	SetValue(lpszSecName, _T("DisabledCapsLock"), _itot(m_bDisCapsLock, chS, 10));
	SetValue(lpszSecName, _T("DisabledCtrlEsc"), _itot(m_bDisCtEs, chS, 10));
	SetValue(lpszSecName, _T("DisabledCtrlSpc"), _itot(m_bDisCtSpc, chS, 10));	
	SetValue(lpszSecName, _T("DisabledAltTab"), _itot(m_bDisAltTab, chS, 10));
	SetValue(lpszSecName, _T("DisabledCtrlShift"), _itot(m_bDisCtShift, chS, 10));
	SetValue(lpszSecName, _T("DisabledCtrlShiftEsc"), _itot(m_bDisCtShEsc, chS, 10));
	SetValue(lpszSecName, _T("DisabledAll"), _itot(m_bDisAll, chS, 10));
}



//=========================================
// ������: GetSetting
// ��  ��: -
// ��  ��: -
// ��  ��: ��ȡ���а�����������
//=========================================
void CKeyState::GetSetting()
{
	TCHAR *lpszSecName = _T("Setting");

	// Ĭ������
	m_bDisLWin = GetValue(lpszSecName, _T("DisabledLWin"), 1);
	m_bDisRWin = GetValue(lpszSecName, _T("DisabledRWin"), 1);
	m_bDisCapsLock = GetValue(lpszSecName, _T("DisabledCapsLock"), 1);
	m_bDisCtEs = GetValue(lpszSecName, _T("DisabledCtrlEsc"), 1);

	// Ĭ�ϲ�����
	m_bDisCtSpc = GetValue(lpszSecName, _T("DisabledCtrlSpc"), 0);
	m_bDisAltTab = GetValue(lpszSecName, _T("DisabledAltTab"), 0);
	m_bDisCtShift = GetValue(lpszSecName, _T("DisabledCtrlShift"), 0);
	m_bDisCtShEsc = GetValue(lpszSecName, _T("DisabledCtrlShiftEsc"), 0);
	m_bDisAll = GetValue(lpszSecName, _T("DisabledAll"), 0);
}