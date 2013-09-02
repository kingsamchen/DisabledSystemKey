/**************************************************

** Project:Disabled System Key

** File:KeyState.h

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-9-17

**************************************************/


#pragma once

class CKeyState
{
	public:
		CKeyState();
		~CKeyState();
		bool IsSettingFileExist();
		void StandardDisable();
		void GetSetting();
		void SaveSetting();
		bool GetWorkingSate();
		void SetWorkingState(bool bState); 

	private:
		bool GetSettingFilePath();
		bool GetValue(LPTSTR lpSection, LPTSTR lpKeyName, int nDefault);
		void SetValue(LPTSTR lpSection, LPTSTR lpKeyName, LPTSTR lpValue);

	public:
		bool m_bDisLWin;
		bool m_bDisRWin;
		bool m_bDisCapsLock;
		bool m_bDisCtEs;
		bool m_bDisCtSpc;
		bool m_bDisAltTab;
		bool m_bDisCtShift;
		bool m_bDisAll;
		bool m_bDisCtShEsc;		

	private:
		CString	m_sPath;
		bool m_bIsWorking;

};
