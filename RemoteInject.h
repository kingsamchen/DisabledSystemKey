/**************************************************

** Project:Disabled System Key

** File:RemoteInject.h

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-27

**************************************************/


#pragma once

class CRemoteInject
{
	public:
		CRemoteInject();
		~CRemoteInject();
		BOOL Inject();
		BOOL UnInject();
		bool IsDllExist();
		bool IsDllFileExit();

	private:
		BOOL GetDllPath();
		BOOL EnableDebugPrivilege(BOOL bEnable);
		DWORD GetPIDFromName(LPCTSTR lpszProcessName);
		DWORD	GetPrevDllBaseAddr(DWORD dwPID);

	public:

	private:
		CString m_sDllPath;
};
