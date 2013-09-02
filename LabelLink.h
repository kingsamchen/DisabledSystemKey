/**************************************************

** Project:AutorunLoadViewer

** File:LabelLink.h

** Edition:v1.0.0 Demo

** Coder:KingsamChen [MDSA Group]

** Last Modify:2009-7-6

**************************************************/

#pragma once

class CLabelLink : public CStatic
{
	DECLARE_DYNAMIC(CLabelLink)

	public:
		CLabelLink();
		virtual ~CLabelLink();
		void SetTextColor(COLORREF clrText);
		void SetBackgroundColor(COLORREF clrBackground);
		void EnableTrack(BOOL bEnable = TRUE);

	protected:
		virtual void PreSubclassWindow();
		afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()

	protected:
		COLORREF m_clrTextColor;
		COLORREF m_clrBackgroundColor;
		CBrush m_Brush;
		BOOL m_bHover;
		BOOL m_bTrack;	
public:
	afx_msg void OnStnClicked();
};


