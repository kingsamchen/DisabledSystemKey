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

	// ����֧��
	// MS COLOR_3DFACE��͸����?
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
// ������: SetTextColor
// ��  ��: clrText(COLORREF) - Ҫ���õ���ɫ
// ��  ��: -
// ��  ��: ����������ɫ����
//=========================================
void CLabelLink::SetTextColor(COLORREF clrText)
{
	m_clrTextColor = clrText;
	// ǿ�������ػ�
	Invalidate(TRUE);
}



//=========================================
// ������: SetBackgroundColor
// ��  ��: clrBackground(COLORREF) - Ҫ���õ���ɫ
// ��  ��: -
// ��  ��: ���ñ�����ɫ����
//=========================================
void CLabelLink::SetBackgroundColor(COLORREF clrBackground)
{
	m_clrBackgroundColor = clrBackground;
	Invalidate(TRUE);
}



//=========================================
// ������: EnableTrack
// ��  ��: bEnable(BOOL) - TRUE:����, FALSE:����
// ��  ��: -
// ��  ��: ���û�����׷��
//=========================================
void CLabelLink::EnableTrack(BOOL bEnable /* = TRUE */)
{
	m_bTrack = bEnable;
	Invalidate(TRUE);
}



//=========================================
// ������: CtlColor
// ��  ��: pDC(CDC*) - GDI����ָ��
//				 nCtlColor(UINT) - �ؼ�����
// ��  ��: HBRUSH - �ؼ�������ˢָ��
// ��  ��: �ؼ���ɫ��Ϣ����
//=========================================
HBRUSH CLabelLink::CtlColor(CDC *pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_clrTextColor);
	pDC->SetBkColor(m_clrBackgroundColor);
	
	return (HBRUSH)m_Brush;
}



//=========================================
// ������: OnMouseMove
// ��  ��: nFlags(UINT) - ָʾ����
//				 point(CPoint) - ���λ��
// ��  ��: -
// ��  ��: ����ƶ���Ϣӳ��
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

			// ����뿪��LabelLink
			if (!rec.PtInRect(point))
			{
				m_bHover = FALSE;
				::ReleaseCapture();
				Invalidate(TRUE);
			}
		} 
		// ������LabelLink
		else
		{
			m_bHover = TRUE;
			SetCapture();

			// #define IDC_HAND  MAKEINTRESOURCE(32649)
			// ֱ������IDC_HAND��Ҫ���ͷ�ļ�
			::SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649)));
			Invalidate(TRUE);
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}



//=========================================
// ������: OnMouseLeave
// ��  ��: wParam(WPARAM)
//				 lParam(LPARAM)
// ��  ��: LRESULT
// ��  ��: ����뿪��ǩ����Ϣӳ��
//=========================================
LRESULT CLabelLink::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bHover = FALSE;
	::ReleaseCapture();
	Invalidate(TRUE);

	return NULL;
}



//=========================================
// ������: PreSubclassWindow
// ��  ��: -
// ��  ��: -
// ��  ��: ���ÿؼ���ʽ
//=========================================
void CLabelLink::PreSubclassWindow()
{
	// ʹ�ؼ�����Ϣ���͸�������
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	CStatic::PreSubclassWindow();
}



//=========================================
// ������: OnStnClicked
// ��  ��: -
// ��  ��: -
// ��  ��: ������Ϣӳ��
//=========================================
void CLabelLink::OnStnClicked()
{
	LPCTSTR lpszUrl = _T("http://blog.kingsamchen.cn");
	ShellExecute(NULL, _T("OPEN"), lpszUrl, NULL, NULL, SW_SHOWNORMAL);
}
