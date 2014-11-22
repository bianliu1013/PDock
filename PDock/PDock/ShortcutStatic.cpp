#include "StdAfx.h"
#include "ShortcutStatic.h"



BEGIN_MESSAGE_MAP(CShortcutStatic, CStatic)
    //{{AFX_MSG_MAP(CColorStatic)
    //ON_WM_CTLCOLOR_REFLECT()
    //}}AFX_MSG_MAP
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CShortcutStatic::CShortcutStatic(void)
{
}

CShortcutStatic::~CShortcutStatic(void)
{
}

BOOL CShortcutStatic::OnEraseBkgnd(CDC *pDC)
{
    return CStatic::OnEraseBkgnd(pDC);
}


HBRUSH CShortcutStatic::CtlColor(CDC *pDC, UINT nCtlColor)
{
    //pDC->SetTextColor(RGB(255, 250, 250));
    //pDC->SetTextColor(RGB(252, 252, 252));
    pDC->SetTextColor(RGB(67, 110, 238)); //436EEE

    //pDC->SetBkColor(RGB(255,255,255));
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}
