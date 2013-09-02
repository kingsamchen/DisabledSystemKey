/**************************************************

** Project:Disabled System Key

** File:RemoteInject.cpp

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-12-31

**************************************************/


#include "StdAfx.h"
#include ".\remoteinject.h"
#include <TlHelp32.h>


//=========================================
// ������: CRemoteInject
// ��  ��: -
// ��  ��: -
// ��  ��: ���캯��
//=========================================
CRemoteInject::CRemoteInject()
{
	// �õ�DLL�����·��
	GetDllPath();
}



//=========================================
// ������: ~CRemoteInject
// ��  ��: -
// ��  ��: -
// ��  ��: ��������
//=========================================
CRemoteInject::~CRemoteInject()
{
	// ����˳�ʱDLL�Դ���,��������ж��
	if (IsDllExist())
	{
		UnInject();
	}
}



//=========================================
// ������: GetDllPath
// ��  ��: -
// ��  ��: BOOL - �ɹ�:TRUE, ʧ��:FALSE
// ��  ��: ��ȡDLL���ڵ�·��
//=========================================
BOOL CRemoteInject::GetDllPath()
{
	TCHAR lpszDllPath[MAX_PATH] = {0};
	BOOL bRet = FALSE;

	// ����Dll·��
	if (!GetFullPathName(_T("Inject.dll"), MAX_PATH, lpszDllPath, NULL))
	{
		TRACE("GetFullPathName Error: %d\n", GetLastError());
		return bRet;
	}

	m_sDllPath.Format(_T("%s"), lpszDllPath);
	return bRet;
}



//=========================================
// ������: EnableDebugPrivilege
// ��  ��: bEnable(BOOL) - ����:TRUE, �ָ�:FALSE
// ��  ��: BOOL - �ɹ�:TRUE, ʧ��:FALSE
// ��  ��: ����/�ָ�Ȩ��
//=========================================
BOOL CRemoteInject::EnableDebugPrivilege(BOOL bEnable)
{
	HANDLE hToken = NULL;
	BOOL bRet = FALSE;

	// �򿪵�ǰ���̵�Access Token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		// ָ��TOKEN_PRIVILEGES.Privileges�������С
		// ��Ϊֻ�޸�SE_DEBUG, ����һ������
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;

		// �ҵ�SE_DEBUG��Ȩ������,��������Ҫд��
		// ��һ��NULL��ʾ��Local System����
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			TRACE("LookupPrivilegeValue Error: %d\n", GetLastError());
			CloseHandle(hToken);
			goto Fail;
		}

		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
			
		// ��Ȩ
		// �����������ǰ��Ȩ��״̬,���ĸ�����������NULL		
		bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, NULL, NULL);
		CloseHandle(hToken);
	}
	
Fail:
	return bRet;
}



//=========================================
// ������: GetPIDFromName
// ��  ��: lpszProcessName(LPCTSTR) - ������
// ��  ��: BOOL - �ɹ�:Ŀ�����PIDֵ, ʧ��:FALSE
// ��  ��: ��ȡָ�����̵�PIDֵ
//=========================================
DWORD CRemoteInject::GetPIDFromName(LPCTSTR lpszProcessName)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 ProcessEntry;
	DWORD dwPID = FALSE;
	BOOL bSnapRet = FALSE;

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

	// ������ǰ���̿���
	// CTS����ʧ�ܷ���INVALID_HANDLE_VALUE	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	ASSERT(!(INVALID_HANDLE_VALUE == hProcessSnap));

	// �������̿���,ƥ�������
	bool bFound = FALSE;
	bSnapRet = Process32First(hProcessSnap, &ProcessEntry);

	while (bSnapRet)
	{
		bFound = (0 == lstrcmpi(lpszProcessName, ProcessEntry.szExeFile));

		// ����PID
		if (bFound)
		{
			dwPID = ProcessEntry.th32ProcessID;
			break;
		}

		bSnapRet = Process32Next(hProcessSnap, &ProcessEntry);
	}

	CloseHandle(hProcessSnap);
	return dwPID;
}



//=========================================
// ������: GetPrevDllBaseAddr
// ��  ��: dwPID(DWORD) - Ŀ����̵�PIDֵ
// ��  ��: DWORD - �ɹ�:DLL��BaseAddr, ʧ��:FALSE
// ��  ��: �õ�Ŀ���������ǰע���DLL��BaseAddr
//=========================================
DWORD CRemoteInject::GetPrevDllBaseAddr(DWORD dwPID)
{
	HANDLE hSnapShot = NULL; 
	MODULEENTRY32 ModEntry;
	BOOL bRet = FALSE;
	DWORD dwRet = FALSE;

	// MSö��Winlogon��DLLģ����Ȼ��ҪSE_DEBUGȨ��
	// �����CreateToolhelp32Snapshot����Ϊ�ܾ����ʳ���
	EnableDebugPrivilege(TRUE);

	// ͬ�����ȳ�ʼ����С
	ModEntry.dwSize = sizeof(MODULEENTRY32);
	ASSERT(dwPID);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	ASSERT(!(INVALID_HANDLE_VALUE == hSnapShot));
	// ȡ�ص�һ������
	bRet = Module32First(hSnapShot, &ModEntry);
	bool bFound = FALSE;

	while (bRet)
	{
		bFound = (0 == lstrcmpi((LPCTSTR)m_sDllPath, ModEntry.szExePath));

		if (bFound)
		{
			dwRet = (DWORD)ModEntry.modBaseAddr;
			break;
		}

		bRet = Module32Next(hSnapShot, &ModEntry);
	}

	CloseHandle(hSnapShot);
	EnableDebugPrivilege(FALSE);
	return dwRet;
}



//=========================================
// ������: IsDllFileExit
// ��  ��: -
// ��  ��: bool - ����:TRUE, ������:FALSE
// ��  ��: ��鵱ǰĿ¼���Ƿ����DLL�ļ�
//=========================================
bool CRemoteInject::IsDllFileExit()
{
	CFileFind DllFile;	
	bool bRet = FALSE;

	// ��鵱ǰĿ¼���Ƿ����Dll
	bRet = (0 == !DllFile.FindFile(m_sDllPath));
	DllFile.Close();

	return bRet;
}



//=========================================
// ������: IsDllExist
// ��  ��: -
// ��  ��: bool - �Ѵ���:TRUE, ������:FALSE 
// ��  ��: ���Winlogon���Ƿ����ע���DLL
//=========================================
bool CRemoteInject::IsDllExist()
{
	bool bExist = FALSE;

	if (GetPrevDllBaseAddr(GetPIDFromName(_T("winlogon.exe"))))
	{
		bExist = TRUE;
	}
	else
	{
		bExist = FALSE;
	}

	return bExist;
}



//=========================================
// ������: Inject
// ��  ��: -
// ��  ��: BOOL - �ɹ�:TRUE, ʧ��:FALSE
// ��  ��: Զ�߳�ע��DLL��Winlogon
//=========================================
BOOL CRemoteInject::Inject()
{
	HANDLE hRemoProcess = NULL;
	HANDLE hRemoThread = NULL;
	LPTSTR lpszRemoRam = NULL;
	BOOL bRet = FALSE;

	// ��Ȩ��SE_DEBUG
	bRet = EnableDebugPrivilege(TRUE);
	ASSERT(bRet);
	
	// ��Winlogon
	hRemoProcess = OpenProcess(PROCESS_CREATE_THREAD |  
														 PROCESS_VM_OPERATION |     
														 PROCESS_VM_WRITE |
														 PROCESS_VM_READ |        
														 PROCESS_QUERY_INFORMATION,
														 NULL, GetPIDFromName(_T("winlogon.exe")));

	if (!hRemoProcess)
	{
		TRACE("OpenProcess Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
	}

	// ����Dll·�����ֽ���, ��������\0.
	// ����Զ�߳��з�������ռ�
	int nSize = (m_sDllPath.GetLength() + 1) * sizeof(TCHAR);
	lpszRemoRam = (LPTSTR)VirtualAllocEx(hRemoProcess, NULL, nSize, MEM_COMMIT, PAGE_READWRITE);

	if (!lpszRemoRam)
	{
		TRACE("VirtualAllocEx Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
	}

	// ��DLL��·��д����Զ���߳��з���Ŀռ���
  // ��ʱ����Ŀ�����Ӧ����PROCESS_VM_OPERATION��PROCESS_VM_WRITE
  bRet = WriteProcessMemory(hRemoProcess, lpszRemoRam, (LPCVOID)m_sDllPath, nSize, NULL);
  
  if (!bRet)
  {
		TRACE("WriteProcessMemory Error: %d\n", GetLastError());
		goto Fail;
  }

	// ��ȡLoadLibrary�ĺ�����ַ
  // GetProcAddress�п��ܱ���ָ��A�滹��W��
  LPTHREAD_START_ROUTINE lpfnThreadRun = (LPTHREAD_START_ROUTINE)
    GetProcAddress(GetModuleHandle(_T("Kernel32")), "LoadLibraryW");

  if (!lpfnThreadRun)
  {
		TRACE("GetProcAddress Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
  }

  // ����Զ���߳�,����LoadLibrary����DLL
  // CreateRemoteThread�ĵ��ĸ�����Ҫ��һ��������LPTHREAD_START_ROUTINE�ĺ���
  // �����������Զ���߳�������ַ�ռ�ʱ����
  hRemoThread = CreateRemoteThread(hRemoProcess, NULL, 0, lpfnThreadRun, lpszRemoRam, 0, NULL);

	if (!hRemoThread)
	{
		TRACE("CreateRemoteThread Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
	}

	// Now, everything is ok
	WaitForSingleObject(hRemoThread, INFINITE);
	bRet = TRUE;

Fail:
	if (lpszRemoRam)
	{
		VirtualFreeEx(hRemoProcess, lpszRemoRam, NULL, MEM_RELEASE);
	}

	if (hRemoThread)
	{
		CloseHandle(hRemoThread);
	}

	if (hRemoProcess)
	{
		CloseHandle(hRemoProcess);
	}
	
	// �ָ�����ͨȨ��
	EnableDebugPrivilege(FALSE);
	return bRet;		
}



//=========================================
// ������: UnInject
// ��  ��: -
// ��  ��: BOOL - �ɹ�:TRUE, ʧ��:FALSE
// ��  ��: ж��ע�뵽Զ�̵߳�DLL
//=========================================
BOOL CRemoteInject::UnInject()
{
	HANDLE hRemoProcess = NULL;
	HANDLE hRemoThread = NULL;
	DWORD dwPreDllBaseAdd = NULL;
	BOOL bRet = FALSE;

	// ��Ȩ
	bRet = EnableDebugPrivilege(TRUE);
	ASSERT(bRet);

	// ��Ŀ�����
	hRemoProcess = OpenProcess(PROCESS_CREATE_THREAD |  
														 PROCESS_VM_OPERATION |     
														 PROCESS_VM_WRITE |
														 PROCESS_VM_READ |        
														 PROCESS_QUERY_INFORMATION,
														 NULL, GetPIDFromName(_T("winlogon.exe")));

	if (!hRemoProcess)
	{
		TRACE("OpenProcess Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
	}

	// �õ�ע���DLL��BaseAddress
	dwPreDllBaseAdd = GetPrevDllBaseAddr(GetPIDFromName(_T("winlogon.exe")));

	if (!dwPreDllBaseAdd)
	{
		bRet = FALSE;
		goto Fail;
	}

	// ��FreeLibraryж��DLL
  // FreeLibrary���ҽ���һ���汾
  LPTHREAD_START_ROUTINE lpfnThreadRun = (LPTHREAD_START_ROUTINE)
    GetProcAddress(::GetModuleHandle(_T("Kernel32")), "FreeLibrary");

  if (!lpfnThreadRun)
  {
		TRACE("GetProcAddresss Error: %d\n", GetLastError());
    bRet = FALSE;
    goto Fail;
  }

	// ����Զ�߳�
  hRemoThread = CreateRemoteThread(hRemoProcess, NULL, 0, lpfnThreadRun, (LPVOID)dwPreDllBaseAdd, 0, NULL);

	if (!hRemoThread)
	{
		TRACE("CreateRemoteThread Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
	}

	// Now, everyting is Ok
	WaitForSingleObject(hRemoThread, INFINITE);
	bRet = TRUE;

Fail:
	if (hRemoThread)
	{
		CloseHandle(hRemoThread);
	}

	if (hRemoProcess)
	{
		CloseHandle(hRemoProcess);
	}

	// �ָ�����ͨȨ��
	EnableDebugPrivilege(FALSE);
	return bRet;
}
