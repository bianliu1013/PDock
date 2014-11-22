#pragma once
#include "afxwin.h"


// CSettingDlg dialog

class CSettingDlg : public CDialog
{
    DECLARE_DYNAMIC(CSettingDlg)

public:
    CSettingDlg(CWnd *pParent = NULL, int systemShortcutBitmap = 0);   // standard constructor
    virtual ~CSettingDlg();
    void InitSystemShortcutStatus(int bitmap);

    // Dialog Data
    enum { IDD = IDD_DIALOG_SETTING };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    void HandleSystemShortcutSetting();


    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedSelectAll();
    afx_msg void OnBnClickedUnSelectAll();
    virtual BOOL OnInitDialog();

private:
    CWnd    *m_pParentDlg;
    CButton m_startupCheckBoxBtn;
    CButton m_CheckBoxBtnMyComputer;
    CButton m_CheckBoxBtnNetWork;
    CButton m_CheckBoxBtnControlPanel;
    CButton m_CheckBoxBtnTimeDate;
    CButton m_CheckBoxBtnRecycle;
    CButton m_CheckBoxBtnMyDocument;
    CButton m_CheckBoxBtnMyMusic;

    CButton m_CheckBoxBtnSelectAll;
    CButton m_CheckBoxBtnUnSelectAll;
    int     m_systemShortcutBitmap;


    BITMAP  m_Bitmap;       // Struct to hold info about the bitmap
    HBITMAP m_hBitmap;      // Handle of the bitmap
public:
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
};
