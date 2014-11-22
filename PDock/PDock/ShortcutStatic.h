#pragma once
#include "afxwin.h"

class CShortcutStatic :
    public CStatic
{
public:
    CShortcutStatic(void);
    ~CShortcutStatic(void);


protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
};
