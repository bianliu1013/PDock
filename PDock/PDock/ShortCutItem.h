#pragma once
//#include <Windows.h>

//#include "btnst.h"

//////////////////////////////////////////////////////////////////////////
/*
      1. normal case           
              client rect      
      -------------------------
      |                       |
      |      ----------       |
      |     |          |      |
      |     |   img    |      |
      |     |          |      |
      |     |          |      |
      |      ----------       |
      |                       |
      -------------------------

      2. "near by" case       
            client rect       
      -------------------------
      |                       |
      |   ----------------    |
      |   |               |   |
      |   |     img       |   |
      |   |               |   |
      |   |               |   |
      |   ----------------    |
      |                       |
      -------------------------

      3. "roaming" case        
            client rect        
      -------------------------
      | ----------------------|
      | |                   | |
      | |                   | |
      | |       img         | |
      | |                   | |
      | |                   | |
      | |                   | |
      | ----------------------|
      -------------------------

*/

class ShortCutItem;
class CBitmapEx;

class IShortCutListener
{
public:
    IShortCutListener() {};
    virtual ~IShortCutListener() {};

    virtual void OnMouseOnShortCurt(ShortCutItem *) {}
    virtual void OnShortCurtClicked(ShortCutItem *) {}
};



namespace Gdiplus
{
    class Bitmap;
}
class ShortCutItem: public CWnd
{
public:
    typedef enum{
        SC_STATE_NONE = -1,
        SC_STATE_NORMAL = 0,
        SC_STATE_NEARBY,
        SC_STATE_ROAMING,
    }SC_STATE;

    //ShortCutItem();
    //~ShortCutItem();
    bool Create(CWnd *pParent, CRect rc, CString text, DWORD id = 0, DWORD style = WS_VISIBLE | WS_CHILD);


public:
    DECLARE_MESSAGE_MAP()

public:
    ShortCutItem(IShortCutListener *pListener, CWnd *pParent, TCHAR *pPath, HICON hIcon, BOOL bSystem, int systemIconIndex);
    ~ShortCutItem(void);

    //BOOL Create(DWORD dwStyle, const RECT &rcLayout, CWnd *pParentWnd, UINT nID);

    TCHAR* GetPath() const
    {
        return m_pFilePath;
    }

    bool IsPressed() const;
    HICON GetIcon() const;
    void GetRect(/*out*/RECT * rect) const;
    void UpdatePosition(int x, int y, int width, int height);
    void UpdateState(const SC_STATE& sc, float percent = 0.0f);

    bool IsSytem() const
    {
        return m_bSystemIcon;
    }
    int GetSystemIconIndex() const
    {
        return m_iSystemIconIndex;
    }

protected:
    CString szClassName;
    bool isMouseIn;
    bool isPressed;


protected:
    void PostClickEvent();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseHover(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg void OnPaint();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);

private:
    void PaintBk(CDC *pDC);
    void _reRender();

private:
    IShortCutListener*  m_pShortCutManager;
    CWnd*               m_pParent;
    TCHAR*              m_pFilePath;//[MAX_PATH];
    HICON               m_hIcon;
    bool                m_bSystemIcon;  // do not allowed to remove.
    int                 m_iSystemIconIndex;
    HBITMAP             m_hBitmap;
    HBITMAP             hBitmap_scale_;

    CToolTipCtrl*       m_pToolTip;
    SC_STATE            state_;
    float               percent_;

    CDC         m_dcBk;
    CBitmap     m_bmpBk;
    CBitmap*    m_pbmpOldBk;
    CBitmap*    m_imgBitmap_;
    CBitmapEx*  bitmapEx_;
    RECT        bitmap_show_rc_;
    Gdiplus::Bitmap*     gdiplug_bitmap_;
    long long   pos_magic_;

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
