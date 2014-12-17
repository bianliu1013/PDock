// DockDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "ShortCutManager.h"
#include "ShortcutStatic.h"

#include <vector>
#include "shortcutstatic.h"

class CDragDialogEx;
class CDockDlg : public CDialog, public IShortCutManagerListener
{
    // Construction
public:
    CDockDlg(CWnd *pParent = NULL); // standard constructor

    // Dialog Data
    enum { IDD = IDD_XBUTTONTEST_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

    // short cut added by otherways, not draged into this dialog
    virtual void OnAddNewShortCur();

    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    void OnTimer(UINT_PTR nIDEvent);
    void OnButtonDynamic(UINT nID);
    void UpdateWindowSize();
    int FindDraggingImageIndex();

public:
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg void OnClose();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    void PreHandleMouseMove();
    void PreHandleMouseLButtonUp();
    void PreHandleMouseLButtonDown();

    void AddNewButton(TCHAR *pFilePath);
    void UpdatePanelSize();
    int ButtonIndexMouseOn();
    int ButtonIndexMouseOnH();  // only used in dragging action

    int GetDockPanelWidth() const
    {
        return m_dockPanelWidth;
    }
    void UpdateSystemIconList(int iconListMap);

private:
    void OnPostEraseBkgnd(CDC *pDC);
    void UpdateLabelPosition(int selectedIndex);
    bool IsMouseEnterToActiveArea() const;

    void OnReleaseDraggingAction();
    void DrawMarkLine(const CPoint &pt);
    void UnDrawMarkLine();
    void PrepareDraggingDialog();
    const CPoint &GetMousePoint() const;  // client
    void UpdateLastMousePosition(const CPoint &pt);
    bool EnoughMouseMove(const CPoint &pt);

public:
    //    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);


private:
    int m_dockPanelLeft;
    int m_dockPanelWidth;
    int m_dockPanelHeight;

    int m_buttonNumber;
    int m_initButtonNumber;

    BITMAP  m_Bitmap;       // Struct to hold info about the bitmap
    HBITMAP m_hBitmap;      // Handle of the bitmap
    int  m_buttonIndexDragging;
    bool m_bDraggingImage;

    int                     m_markLineX;
    int                     m_markLineY;

    ShortCutManager         m_shortManager;
    CDragDialogEx          *m_pDragDialog;
    CPoint                  m_pPointBeginDrag;

    POINT                   m_lastPoint;
    CShortcutStatic         m_shotcutNameLabel;
    bool                    m_bEdittingCutItem;

public:
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};
