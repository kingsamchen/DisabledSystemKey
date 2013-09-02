/**************************************************

** Project:Disabled System Key

** File:DisabledSystemKeyDlg.h

** Edition:v1.0.0 Beta

** Coder:KingsamChen [MDSA Group]

** Last Modify:2008-10-3

**************************************************/


#pragma once

#include "Tray.h"
#include "RemoteInject.h"

// CDisabledSystemKeyDlg 对话框
class CDisabledSystemKeyDlg : public CDialog
{
// 构造
public:
	CDisabledSystemKeyDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CDisabledSystemKeyDlg();
// 对话框数据
	enum { IDD = IDD_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCheckStateChange(UINT uID);
	afx_msg LRESULT OnTrayProc(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPopMenuCommand(UINT uID);
	DECLARE_MESSAGE_MAP()

private:
	CTray	*m_pTray;
	UINT	m_uTaskMsg; 
	static HHOOK ms_hHookWnd;
	CRemoteInject *m_pInject;

private:
	static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam); 
	void ReadyCheck();
	void RemoteInject(bool bChkStatus);

public:
	void EnableCheckBox(bool bEnable);	
};
