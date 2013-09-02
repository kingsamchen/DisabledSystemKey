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
// 函数名: CKeyState
// 输  入: -
// 输  出: -
// 功  能: CKeyState构造函数
//=========================================
CKeyState::CKeyState()
{
	// 获取路径
	bool bRet = GetSettingFilePath();
	ASSERT(bRet);
	// 初始为TRUE
	m_bIsWorking = 1;
}



//=========================================
// 函数名: ~CKeyState
// 输  入: -
// 输  出: -
// 功  能: CKeyState析构函数
//=========================================
CKeyState::~CKeyState()
{
	// 析构时保存按键屏蔽设置
	// 这个函数仍是Public.即你可以在需要时保存设置
	SaveSetting();
}



//=========================================
// 函数名: GetSettingFilePath
// 输  入: -
// 输  出: bool - 成功TRUE, 失败FALSE
// 功  能: 获取/保存设置文件的路径
//=========================================
bool CKeyState::GetSettingFilePath()
{
	TCHAR sFileName[MAX_PATH];

	if (!GetFullPathName(_T("Config.cfg"), MAX_PATH, sFileName, NULL))
	{
		return FALSE;		
	}	

	// 保存设置文件路径
	m_sPath.Format(_T("%s"), sFileName);

	return TRUE;
}



//=========================================
// 函数名: IsSettingFileExist
// 输  入: -
// 输  出: bool - 存在TRUE, 不存在FALSE
// 功  能: 检查设置文件是否存在
//=========================================
bool CKeyState::IsSettingFileExist()
{
	CFileFind SettingFile;

	// 如果搜索不到,则说明文件不存在
	// 也可以使用CRT函数_access,不过需要相应的io.h
	// 注意:FindFile之后需使用Close关闭请求
	BOOL bRet = SettingFile.FindFile(m_sPath);
	SettingFile.Close();
	
	if (!bRet)
	{
		return FALSE;
	}
	
	return TRUE;
}



//=========================================
// 函数名: StandardDisable
// 输  入: -
// 输  出: -
// 功  能: 使用标准按键屏蔽选项
//=========================================
void CKeyState::StandardDisable()
{
	// 标准屏蔽按键选项
	// 你可以根据需要更改
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
// 函数名: GetWorkingSate
// 输  入: -
// 输  出: bool - 已屏蔽TRUE, 暂停FALSE
// 功  能: 获取屏蔽状态
//=========================================
bool CKeyState::GetWorkingSate()
{
	return m_bIsWorking;
}



//=========================================
// 函数名: SetWorkingState
// 输  入: bState(bool) - 屏蔽状态参数
// 输  出: -
// 功  能: 设置屏蔽状态
//=========================================
void CKeyState::SetWorkingState(bool bState)
{
	if (!bState)
		m_bIsWorking = 0;
	else
		m_bIsWorking = 1;		
}



//=========================================
// 函数名: GetValue
// 输  入: lpSection(LPTSTR) - Section项名
//         lpKeyName(LPTSTR) - Key项名
//				 nDefault(int) - 未找到时的默认值
// 输  出: bool - 启用1, 停用0
// 功  能: 获取相应按键屏蔽选项的值
//=========================================
bool CKeyState::GetValue(LPTSTR lpSection, LPTSTR lpKeyName, int nDefault)
{
	// 据说CWinApp::GetPrivateProfileInt有点不干净-。-||
	UINT uValue = ::GetPrivateProfileInt(lpSection, lpKeyName, nDefault, m_sPath);

	if (!uValue)
		return 0;	
	else
		return 1;
}



//=========================================
// 函数名: SetValue
// 输  入: lpSection(LPTSTR) - Section项名
//				 lpKeyName(LPTSTR) - Key项名
//				 lpValue(LPTSTR) - 选项值
// 输  出: -
// 功  能: 设置相应按键屏蔽选项的值
//=========================================
void CKeyState::SetValue(LPTSTR lpSection, LPTSTR lpKeyName, LPTSTR lpValue)
{
	::WritePrivateProfileString(lpSection, lpKeyName, lpValue, m_sPath);
}



//=========================================
// 函数名: SaveSetting
// 输  入: -
// 输  出: -
// 功  能: 保存所有按键屏蔽设置
//=========================================
void CKeyState::SaveSetting()
{
	TCHAR *lpszSecName = _T("Setting");	// MS这里加不加const无所谓了
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
// 函数名: GetSetting
// 输  入: -
// 输  出: -
// 功  能: 获取所有按键屏蔽设置
//=========================================
void CKeyState::GetSetting()
{
	TCHAR *lpszSecName = _T("Setting");

	// 默认屏蔽
	m_bDisLWin = GetValue(lpszSecName, _T("DisabledLWin"), 1);
	m_bDisRWin = GetValue(lpszSecName, _T("DisabledRWin"), 1);
	m_bDisCapsLock = GetValue(lpszSecName, _T("DisabledCapsLock"), 1);
	m_bDisCtEs = GetValue(lpszSecName, _T("DisabledCtrlEsc"), 1);

	// 默认不屏蔽
	m_bDisCtSpc = GetValue(lpszSecName, _T("DisabledCtrlSpc"), 0);
	m_bDisAltTab = GetValue(lpszSecName, _T("DisabledAltTab"), 0);
	m_bDisCtShift = GetValue(lpszSecName, _T("DisabledCtrlShift"), 0);
	m_bDisCtShEsc = GetValue(lpszSecName, _T("DisabledCtrlShiftEsc"), 0);
	m_bDisAll = GetValue(lpszSecName, _T("DisabledAll"), 0);
}