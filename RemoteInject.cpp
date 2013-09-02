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
// 函数名: CRemoteInject
// 输  入: -
// 输  出: -
// 功  能: 构造函数
//=========================================
CRemoteInject::CRemoteInject()
{
	// 得到DLL的相对路径
	GetDllPath();
}



//=========================================
// 函数名: ~CRemoteInject
// 输  入: -
// 输  出: -
// 功  能: 析构函数
//=========================================
CRemoteInject::~CRemoteInject()
{
	// 如果退出时DLL仍存在,则对其进行卸载
	if (IsDllExist())
	{
		UnInject();
	}
}



//=========================================
// 函数名: GetDllPath
// 输  入: -
// 输  出: BOOL - 成功:TRUE, 失败:FALSE
// 功  能: 获取DLL所在的路径
//=========================================
BOOL CRemoteInject::GetDllPath()
{
	TCHAR lpszDllPath[MAX_PATH] = {0};
	BOOL bRet = FALSE;

	// 设置Dll路径
	if (!GetFullPathName(_T("Inject.dll"), MAX_PATH, lpszDllPath, NULL))
	{
		TRACE("GetFullPathName Error: %d\n", GetLastError());
		return bRet;
	}

	m_sDllPath.Format(_T("%s"), lpszDllPath);
	return bRet;
}



//=========================================
// 函数名: EnableDebugPrivilege
// 输  入: bEnable(BOOL) - 提升:TRUE, 恢复:FALSE
// 输  出: BOOL - 成功:TRUE, 失败:FALSE
// 功  能: 提升/恢复权限
//=========================================
BOOL CRemoteInject::EnableDebugPrivilege(BOOL bEnable)
{
	HANDLE hToken = NULL;
	BOOL bRet = FALSE;

	// 打开当前进程的Access Token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		// 指定TOKEN_PRIVILEGES.Privileges的数组大小
		// 因为只修改SE_DEBUG, 所以一个足矣
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;

		// 找到SE_DEBUG的权限属性,并根据需要写入
		// 第一个NULL表示在Local System查找
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			TRACE("LookupPrivilegeValue Error: %d\n", GetLastError());
			CloseHandle(hToken);
			goto Fail;
		}

		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
			
		// 提权
		// 如果不保存先前的权限状态,第四个参数可以是NULL		
		bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, NULL, NULL);
		CloseHandle(hToken);
	}
	
Fail:
	return bRet;
}



//=========================================
// 函数名: GetPIDFromName
// 输  入: lpszProcessName(LPCTSTR) - 进程名
// 输  出: BOOL - 成功:目标进程PID值, 失败:FALSE
// 功  能: 获取指定进程的PID值
//=========================================
DWORD CRemoteInject::GetPIDFromName(LPCTSTR lpszProcessName)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 ProcessEntry;
	DWORD dwPID = FALSE;
	BOOL bSnapRet = FALSE;

	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

	// 创建当前进程快照
	// CTS函数失败返回INVALID_HANDLE_VALUE	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	ASSERT(!(INVALID_HANDLE_VALUE == hProcessSnap));

	// 遍历进程快照,匹配进程名
	bool bFound = FALSE;
	bSnapRet = Process32First(hProcessSnap, &ProcessEntry);

	while (bSnapRet)
	{
		bFound = (0 == lstrcmpi(lpszProcessName, ProcessEntry.szExeFile));

		// 返回PID
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
// 函数名: GetPrevDllBaseAddr
// 输  入: dwPID(DWORD) - 目标进程的PID值
// 输  出: DWORD - 成功:DLL的BaseAddr, 失败:FALSE
// 功  能: 得到目标进程中先前注入的DLL的BaseAddr
//=========================================
DWORD CRemoteInject::GetPrevDllBaseAddr(DWORD dwPID)
{
	HANDLE hSnapShot = NULL; 
	MODULEENTRY32 ModEntry;
	BOOL bRet = FALSE;
	DWORD dwRet = FALSE;

	// MS枚举Winlogon的DLL模块仍然需要SE_DEBUG权限
	// 否则会CreateToolhelp32Snapshot会因为拒绝访问出错
	EnableDebugPrivilege(TRUE);

	// 同样得先初始化大小
	ModEntry.dwSize = sizeof(MODULEENTRY32);
	ASSERT(dwPID);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	ASSERT(!(INVALID_HANDLE_VALUE == hSnapShot));
	// 取回第一条快照
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
// 函数名: IsDllFileExit
// 输  入: -
// 输  出: bool - 存在:TRUE, 不存在:FALSE
// 功  能: 检查当前目录下是否存在DLL文件
//=========================================
bool CRemoteInject::IsDllFileExit()
{
	CFileFind DllFile;	
	bool bRet = FALSE;

	// 检查当前目录下是否存在Dll
	bRet = (0 == !DllFile.FindFile(m_sDllPath));
	DllFile.Close();

	return bRet;
}



//=========================================
// 函数名: IsDllExist
// 输  入: -
// 输  出: bool - 已存在:TRUE, 不存在:FALSE 
// 功  能: 检查Winlogon中是否存在注入的DLL
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
// 函数名: Inject
// 输  入: -
// 输  出: BOOL - 成功:TRUE, 失败:FALSE
// 功  能: 远线程注入DLL到Winlogon
//=========================================
BOOL CRemoteInject::Inject()
{
	HANDLE hRemoProcess = NULL;
	HANDLE hRemoThread = NULL;
	LPTSTR lpszRemoRam = NULL;
	BOOL bRet = FALSE;

	// 提权至SE_DEBUG
	bRet = EnableDebugPrivilege(TRUE);
	ASSERT(bRet);
	
	// 打开Winlogon
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

	// 计算Dll路径的字节数, 包括最后的\0.
	// 并在远线程中分配虚拟空间
	int nSize = (m_sDllPath.GetLength() + 1) * sizeof(TCHAR);
	lpszRemoRam = (LPTSTR)VirtualAllocEx(hRemoProcess, NULL, nSize, MEM_COMMIT, PAGE_READWRITE);

	if (!lpszRemoRam)
	{
		TRACE("VirtualAllocEx Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
	}

	// 把DLL的路径写到在远程线程中分配的空间中
  // 此时对于目标进程应具有PROCESS_VM_OPERATION和PROCESS_VM_WRITE
  bRet = WriteProcessMemory(hRemoProcess, lpszRemoRam, (LPCVOID)m_sDllPath, nSize, NULL);
  
  if (!bRet)
  {
		TRACE("WriteProcessMemory Error: %d\n", GetLastError());
		goto Fail;
  }

	// 获取LoadLibrary的函数地址
  // GetProcAddress中可能必须指定A版还是W版
  LPTHREAD_START_ROUTINE lpfnThreadRun = (LPTHREAD_START_ROUTINE)
    GetProcAddress(GetModuleHandle(_T("Kernel32")), "LoadLibraryW");

  if (!lpfnThreadRun)
  {
		TRACE("GetProcAddress Error: %d\n", GetLastError());
		bRet = FALSE;
		goto Fail;
  }

  // 创建远程线程,利用LoadLibrary加载DLL
  // CreateRemoteThread的第四个参数要求一个定义自LPTHREAD_START_ROUTINE的函数
  // 这个函数会在远程线程启动地址空间时调用
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
	
	// 恢复至普通权限
	EnableDebugPrivilege(FALSE);
	return bRet;		
}



//=========================================
// 函数名: UnInject
// 输  入: -
// 输  出: BOOL - 成功:TRUE, 失败:FALSE
// 功  能: 卸载注入到远线程的DLL
//=========================================
BOOL CRemoteInject::UnInject()
{
	HANDLE hRemoProcess = NULL;
	HANDLE hRemoThread = NULL;
	DWORD dwPreDllBaseAdd = NULL;
	BOOL bRet = FALSE;

	// 提权
	bRet = EnableDebugPrivilege(TRUE);
	ASSERT(bRet);

	// 打开目标进程
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

	// 得到注入的DLL的BaseAddress
	dwPreDllBaseAdd = GetPrevDllBaseAddr(GetPIDFromName(_T("winlogon.exe")));

	if (!dwPreDllBaseAdd)
	{
		bRet = FALSE;
		goto Fail;
	}

	// 用FreeLibrary卸载DLL
  // FreeLibrary有且仅有一个版本
  LPTHREAD_START_ROUTINE lpfnThreadRun = (LPTHREAD_START_ROUTINE)
    GetProcAddress(::GetModuleHandle(_T("Kernel32")), "FreeLibrary");

  if (!lpfnThreadRun)
  {
		TRACE("GetProcAddresss Error: %d\n", GetLastError());
    bRet = FALSE;
    goto Fail;
  }

	// 创建远线程
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

	// 恢复至普通权限
	EnableDebugPrivilege(FALSE);
	return bRet;
}
