// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SettingDlg.h"

#include <vector>

#include "Dock.h"
#include "DockDlg.h"
#include "Preference.h"

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd *pParent, int systemShortcutBitmap)
    : CDialog(CSettingDlg::IDD, pParent)
    , dlg_parent_(pParent)
    , m_systemShortcutBitmap(systemShortcutBitmap)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK_STARTUP,         checkbox_startup_);
    DDX_Control(pDX, IDC_CHECK_SHOW_ALWAYS,     checkbox_always_show);
    DDX_Control(pDX, IDC_CHECK_DELETE_ALERT,    checkbox_alert_before_del_);
    
    DDX_Control(pDX, IDC_CHECK_ST_0,            m_CheckBoxBtnMyComputer);
    DDX_Control(pDX, IDC_CHECK_ST_1,            m_CheckBoxBtnNetWork);
    DDX_Control(pDX, IDC_CHECK_ST_2,            m_CheckBoxBtnControlPanel);
    DDX_Control(pDX, IDC_CHECK_ST_3,            m_CheckBoxBtnTimeDate);
    DDX_Control(pDX, IDC_CHECK_ST_4,            m_CheckBoxBtnRecycle);
    DDX_Control(pDX, IDC_CHECK_ST_5,            m_CheckBoxBtnMyDocument);
    DDX_Control(pDX, IDC_CHECK_ST_6,            m_CheckBoxBtnMyMusic);
    DDX_Control(pDX, IDC_CHECK_SELECT_ALL,      m_CheckBoxBtnSelectAll);
    DDX_Control(pDX, IDC_CHECK_UNSELECT_ALL,    m_CheckBoxBtnUnSelectAll);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CSettingDlg::OnBnClickedButtonOK)
    ON_BN_CLICKED(IDC_CHECK_SELECT_ALL, &CSettingDlg::OnBnClickedSelectAll)
    ON_BN_CLICKED(IDC_CHECK_UNSELECT_ALL, &CSettingDlg::OnBnClickedUnSelectAll)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


void CSettingDlg::OnBnClickedSelectAll()
{
    if (BST_CHECKED == m_CheckBoxBtnSelectAll.GetCheck())
    {
        m_CheckBoxBtnMyComputer.SetCheck(BST_CHECKED);
        m_CheckBoxBtnNetWork.SetCheck(BST_CHECKED);
        m_CheckBoxBtnControlPanel.SetCheck(BST_CHECKED);
        m_CheckBoxBtnTimeDate.SetCheck(BST_CHECKED);
        m_CheckBoxBtnRecycle.SetCheck(BST_CHECKED);
        m_CheckBoxBtnMyDocument.SetCheck(BST_CHECKED);
        m_CheckBoxBtnMyMusic.SetCheck(BST_CHECKED);
        m_CheckBoxBtnUnSelectAll.SetCheck(BST_UNCHECKED);
    }
}


void CSettingDlg::OnBnClickedUnSelectAll()
{
    if (BST_CHECKED == m_CheckBoxBtnUnSelectAll.GetCheck())
    {
        m_CheckBoxBtnMyComputer.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnNetWork.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnControlPanel.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnTimeDate.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnRecycle.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnMyDocument.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnMyMusic.SetCheck(BST_UNCHECKED);
        m_CheckBoxBtnSelectAll.SetCheck(BST_UNCHECKED);
    }
}


void CSettingDlg::OnBnClickedButtonOK()
{
    if (BST_CHECKED == checkbox_startup_.GetCheck())
    {
        TCHAR szCurrentDir[MAX_PATH];
        ::GetModuleFileName(NULL, szCurrentDir, MAX_PATH);

        int rt = 0;
        HKEY hkey = NULL;
        unsigned long dwDisp;

        if (ERROR_SUCCESS != (rt = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
                                                  0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                                                  NULL, &hkey, &dwDisp)))
        {
            //TODO
        }

        if (ERROR_SUCCESS != (rt = RegSetValueEx(hkey, _T("wxhLauncher"), 0, REG_SZ , (BYTE *)szCurrentDir, (wcslen(szCurrentDir) + 1) * 2)))
        {
            //TODO
        }
    }
    else if (BST_UNCHECKED == checkbox_startup_.GetCheck())
    {
        HKEY hkey = NULL;
        int rt = 0;
        unsigned long dwDisp;

        if (ERROR_SUCCESS != (rt = RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
                                                  0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                                                  NULL, &hkey, &dwDisp)))
        {
            //TODO
        }

        RegDeleteValue(hkey, _T("wxhLauncher"));
    }

    Preference::startup             = checkbox_startup_.GetCheck()? 1: 0;
    Preference::alwaysShow          = checkbox_always_show.GetCheck()? 1: 0;
    Preference::alert_before_delete = checkbox_alert_before_del_.GetCheck()? 1: 0;
    Preference::UpdateSetting();

    HandleSystemShortcutSetting();

    this->SendMessage(WM_CLOSE);
}


BOOL CSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // open .ini file
    int startUp = GetPrivateProfileInt(L"General", L"StartupWithSystem", 1, CONFIG_INI_FILE);
    checkbox_startup_.SetCheck(startUp == 1 ? BST_CHECKED : BST_UNCHECKED);

    int alwaysShow = GetPrivateProfileInt(L"General", L"AlwaysShow", 1, CONFIG_INI_FILE);
    checkbox_always_show.SetCheck(alwaysShow == 1 ? BST_CHECKED : BST_UNCHECKED);

    int alert_before_delete = GetPrivateProfileInt(L"General", L"AlertBeforeDelete", 1, CONFIG_INI_FILE);
    checkbox_alert_before_del_.SetCheck(alert_before_delete == 1 ? BST_CHECKED : BST_UNCHECKED);

    InitSystemShortcutStatus(m_systemShortcutBitmap);


    m_hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));

    if (m_hBitmap == NULL)
    {
        MessageBox(_T("Error loading bitmap"));
        return FALSE;
    }

    GetObject(m_hBitmap, sizeof(m_Bitmap), &m_Bitmap);
    return TRUE;
}


void CSettingDlg::HandleSystemShortcutSetting()
{
    CButton ;
    int systemShortBitMap = 0;

    if (BST_CHECKED == m_CheckBoxBtnMyComputer.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 0);
    }

    if (BST_CHECKED == m_CheckBoxBtnNetWork.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 1);
    }

    if (BST_CHECKED == m_CheckBoxBtnControlPanel.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 2);
    }

    if (BST_CHECKED == m_CheckBoxBtnTimeDate.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 3);
    }

    if (BST_CHECKED == m_CheckBoxBtnRecycle.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 4);
    }

    if (BST_CHECKED == m_CheckBoxBtnMyDocument.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 5);
    }

    if (BST_CHECKED == m_CheckBoxBtnMyMusic.GetCheck())
    {
        systemShortBitMap |= (0x00000001 << 6);
    }

    CDockDlg *pDlg = (CDockDlg *)dlg_parent_;
    pDlg->UpdateSystemIconList(systemShortBitMap);
}


void CSettingDlg::InitSystemShortcutStatus(int bitmap)
{
    int count = 6; //TODO

    if (bitmap & (0x00000001 << 0))
    {
        m_CheckBoxBtnMyComputer.SetCheck(BST_CHECKED);
    }

    if (bitmap & (0x00000001 << 1))
    {
        m_CheckBoxBtnNetWork.SetCheck(BST_CHECKED);
    }

    if (bitmap & (0x00000001 << 2))
    {
        m_CheckBoxBtnControlPanel.SetCheck(BST_CHECKED);
    }

    if (bitmap & (0x00000001 << 3))
    {
        m_CheckBoxBtnTimeDate.SetCheck(BST_CHECKED);
    }

    if (bitmap & (0x00000001 << 4))
    {
        m_CheckBoxBtnRecycle.SetCheck(BST_CHECKED);
    }

    if (bitmap & (0x00000001 << 5))
    {
        m_CheckBoxBtnMyDocument.SetCheck(BST_CHECKED);
    }

    if (bitmap & (0x00000001 << 6))
    {
        m_CheckBoxBtnMyMusic.SetCheck(BST_CHECKED);
    }
}


void CSettingDlg::OnPaint()
{
    CPaintDC dc(this);
    //HDC hMemDC = ::CreateCompatibleDC(NULL);
    //SelectObject(hMemDC, m_hBitmap);
    //RECT rc;
    //this->GetClientRect(&rc);
    //StretchBlt(dc.m_hDC, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hMemDC, 0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);
    //::DeleteDC(hMemDC);
}


HBRUSH CSettingDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    int iID = pWnd->GetDlgCtrlID();
    if (IDC_CHECK_SHOW_ALWAYS == iID ||
        IDC_CHECK_STARTUP == iID ||
        IDC_CHECK_ST_0 == iID ||
        IDC_CHECK_ST_1 == iID ||
        IDC_CHECK_ST_2 == iID ||
        IDC_CHECK_ST_3 == iID ||
        IDC_CHECK_ST_4 == iID ||
        IDC_CHECK_ST_5 == iID ||
        IDC_CHECK_ST_6 == iID ||
        IDC_CHECK_SELECT_ALL == iID ||
        IDC_CHECK_UNSELECT_ALL == iID)
    {
        pDC->SetBkMode(TRANSPARENT);
        CRect rc;
        pWnd->GetWindowRect(&rc);
        ScreenToClient(&rc);
        CDC *dc = GetDC();
        pDC->BitBlt(0, 0, rc.Width(), rc.Height(), dc, rc.left, rc.top, SRCCOPY);

        pDC->SetTextColor(RGB(0, 0, 255));
        pDC->SetBkMode(TRANSPARENT);
        ReleaseDC(dc);

        hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH);
    }

    return hbr;
}
