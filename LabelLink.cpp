/**************************************************

** Project:AutorunLoadViewer

** File:LabelLink.cpp

** Edition:v1.0.0 Demo

** Coder:KingsamChen [MDSA Group]

** Last Modify:2009-7-6

**************************************************/

#include "stdafx.h"
#include "DisabledSystemKey.h"
#include "LabelLink.h"
#include ".\labellink.h"


// CLabelLink

IMPLEMENT_DYNAMIC(CLabelLink, CStatic)
CLabelLink::CLabelLink() : m_bTrack(FALSE), m_bHover(FALSE)
{
	m_clrTextColor = RGB(0, 0, 0);

	// 背景支持
	// MS COLOR_3DFACE是透明的?
	m_clrBackgroundColor = ::GetSysColor(COLOR_3DFACE);
	m_Brush.CreateSolidBrush(m_clrBackgroundColor);
}

CLabelLink::~CLabelLink()
{
}


BEGIN_MESSAGE_MAP(CLabelLink, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
END_MESSAGE_MAP()



//=========================================
// 函数名: SetTextColor
// 输  入: clrText(COLORREF) - 要设置的颜色
// 输  出: -
// 功  能: 设置字体颜色参数
//=========================================
void CLabelLink::SetTextColor(COLORREF clrText)
{
	m_clrTextColor = clrText;
	// 强制引起重绘
	Invalidate(TRUE);
}



//=========================================
// 函数名: SetBackgroundColor
// 输  入: clrBackground(COLORREF) - 要设置的颜色
// 输  出: -
// 功  能: 设置背景颜色参数
//=========================================
void CLabelLink::SetBackgroundColor(COLORREF clrBackground)
{
	m_clrBackgroundColor = clrBackground;
	Invalidate(TRUE);
}



//=========================================
// 函数名: EnableTrack
// 输  入: bEnable(BOOL) - TRUE:启用, FALSE:禁用
// 输  出: -
// 功  能: 启用或禁鼠标追踪
//=========================================
void CLabelLink::EnableTrack(BOOL bEnable /* = TRUE */)
{
	m_bTrack = bEnable;
	Invalidate(TRUE);
}



//=========================================
// 函数名: CtlColor
// 输  入: pDC(CDC*) - GDI对象指针
//				 nCtlColor(UINT) - 控件类型
// 输  出: HBRUSH - 控件背景画刷指针
// 功  能: 控件颜色消息反射
//=========================================
HBRUSH CLabelLink::CtlColor(CDC *pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_clrTextColor);
	pDC->SetBkColor(m_clrBackgroundColor);
	
	return (HBRUSH)m_Brush;
}



//=========================================
// 函数名: OnMouseMove
// 输  入: nFlags(UINT) - 指示按键
//				 point(CPoint) - 鼠标位置
// 输  出: -
// 功  能: 鼠标移动消息映射
//=========================================
void CLabelLink::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTrack)
	{
		TRACKMOUSEEVENT MouseEvent;
		MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
		MouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
		MouseEvent.hwndTrack = GetSafeHwnd();
		MouseEvent.dwHoverTime = HOVER_DEFAULT;

		TrackMouseEvent(&MouseEvent);		

		if (m_bHover)
		{
			CRect rec;
			GetClientRect(&rec);

			// 鼠标离开了LabelLink
			if (!rec.PtInRect(point))
			{
				m_bHover = FALSE;
				::ReleaseCapture();
				Invalidate(TRUE);
			}
		} 
		// 鼠标进入LabelLink
		else
		{
			m_bHover = TRUE;
			SetCapture();

			// #define IDC_HAND  MAKEINTRESOURCE(32649)
			// 直接引用IDC_HAND需要添加头文件
			::SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649)));
			Invalidate(TRUE);
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}



//=========================================
// 函数名: OnMouseLeave
// 输  入: wParam(WPARAM)
//				 lParam(LPARAM)
// 输  出: LRESULT
// 功  能: 鼠标离开标签的消息映射
//=========================================
LRESULT CLabelLink::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bHover = FALSE;
	::ReleaseCapture();
	Invalidate(TRUE);

	return NULL;
}



//=========================================
// 函数名: PreSubclassWindow
// 输  入: -
// 输  出: -
// 功  能: 设置控件样式
//=========================================
void CLabelLink::PreSubclassWindow()
{
	// 使控件将消息发送给父窗体
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	CStatic::PreSubclassWindow();
}



//=========================================
// 函数名: OnStnClicked
// 输  入: -
// 输  出: -
// 功  能: 单击消息映射
//=========================================
void CLabelLink::OnStnClicked()
{
	LPCTSTR lpszUrl = _T("http://blog.kingsamchen.cn");
	ShellExecute(NULL, _T("OPEN"), lpszUrl, NULL, NULL, SW_SHOWNORMAL);
}
