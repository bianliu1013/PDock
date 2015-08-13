// XbuttonTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dock.h"
#include "DockDlg.h"
#include "global.h"

#include "DragDialogex.h"
#include "tools.h"
#include "Preference.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern HBITMAP ConvertIconToBitmap(HICON hIcon);


// CXbuttonTestDlg dialog
#define TIMER_SHOW          1   //Timer id
#define TIMER_HIDE          2   //Timer id
#define TIMER_MOVE          3   //Timer id
#define TIME_ELAPSE_HIDE    150
#define TIME_ELAPSE_SHOW    150
#define TIME_ELAPSE_MOVE    50

#define IDH_HOT1            4001  // hot key,  ALT f1

//#define DOCK_PANEL_COLOR RGB(232, 232, 232)
//#define DOCK_PANEL_COLOR RGB(55, 55, 55)
#define DOCK_PANEL_COLOR RGB(255, 128, 128)
#define SHOT_CURT_LABEL_COLOR RGB(255, 255, 255)

CDockDlg::CDockDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CDockDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDockDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_SC, m_shotcutNameLabel);
}

BEGIN_MESSAGE_MAP(CDockDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DROPFILES()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDOK, &CDockDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDockDlg::OnBnClickedCancel)
    //ON_BN_CLICKED(IDC_BUTTON6, &CXbuttonTestDlg::OnBnClickedButton6)
    ON_COMMAND_RANGE(1100, 1200, OnButtonDynamic)
    ON_WM_SIZE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    //    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_SHOWWINDOW()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CXbuttonTestDlg message handlers



void CDockDlg::UpdatePanelSize()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_dockPanelLeft = screenWidth / 20;
    m_dockPanelWidth = screenWidth * 9 / 10;
    m_dockPanelHeight = 60;

    this->SetWindowPos(NULL, m_dockPanelLeft, 0, m_dockPanelWidth, m_dockPanelHeight, SWP_SHOWWINDOW);
    //::SetWindowPos(this->GetSafeHwnd(),(HWND)(-1), 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE);
    //HWND hwndTitle = ::GetDlgItem(this->GetSafeHwnd(), IDC_STATIC);
    //::MoveWindow(hwndTitle, m_dockPanelWidth / 2,  m_dockPanelHeight - 30, 800, 20, FALSE);
}


BOOL CDockDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    Preference::Init();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | 0x80000);
    SetLayeredWindowAttributes(DOCK_PANEL_COLOR, 200, 2);
    // put mainframe at the top most of screen
    //::GetWindowLong((this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_TOPMOST );
    ::SetWindowPos(this->GetSafeHwnd(), (HWND)(-1), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    m_initButtonNumber = SYSTEM_SHORTCUT_NUM;
    m_buttonNumber = SYSTEM_SHORTCUT_NUM;

    m_pDragDialog = NULL;

    m_shortManager.SetShortCutManagerListener(this);
    m_shortManager.Init(this);
    UpdatePanelSize();
    m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);

    SetTimer(TIMER_MOVE, TIME_ELAPSE_MOVE, NULL);
    SetTimer(TIMER_SHOW, TIME_ELAPSE_SHOW, NULL);

    m_hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));

    //m_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), _T("Sky.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (m_hBitmap == NULL)
    {
        MessageBox(_T("Error loading bitmap"));
        return FALSE;
    }

    //Get information about the bitmap..
    GetObject(m_hBitmap, sizeof(m_Bitmap), &m_Bitmap);  // Get info about the bitmap

    CFont *m_Font1 = new CFont;
    m_Font1->CreatePointFont(160, _T("Courier New"));
    m_shotcutNameLabel.SetFont(m_Font1);

    m_pPointBeginDrag.x = -1;
    m_pPointBeginDrag.y = -1;
    m_lastPoint.x = -1;
    m_lastPoint.y = -1;
    m_bEdittingCutItem = false;
    m_bDraggingImage = false;
    m_buttonIndexDragging = -1;

    registerHotKey();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDockDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        /*
        CPaintDC dc(this);
        CRect rect;
        GetClientRect(&rect);
        dc.FillSolidRect(&rect, DOCK_PANEL_COLOR);
        CDialog::OnPaint();
        */

        // device context for painting
#if 1 // draw background bitmap
        CPaintDC dc(this);
        HDC hMemDC = ::CreateCompatibleDC(NULL);
        SelectObject(hMemDC, m_hBitmap);
        StretchBlt(dc.m_hDC, 0, 0, m_dockPanelWidth, m_dockPanelHeight, hMemDC, 0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);
        ::DeleteDC(hMemDC);
#endif

#if 0

        // draw mark line
        if (m_markLineX != -1)
        {
            CPen penMarkLine(PS_SOLID, 3, RGB(255, 0, 0));   // Dark gray
            CPen *pOldPen = dc.SelectObject(&penMarkLine);
            dc.MoveTo(m_markLineX, 0);
            dc.LineTo(m_markLineX, 250);
            dc.SelectObject(pOldPen);
        }

#endif
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDockDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CDockDlg::UpdateLabelPosition(int selectedIndex)
{
    m_shotcutNameLabel.ShowWindow(SW_HIDE);
#if 0

    if (selectedIndex >= 0 && selectedIndex < m_shortManager.GetShortCutNums())
    {
        RECT rc;
        this->GetClientRect(&rc);
        m_shotcutNameLabel.SetWindowText(m_shortManager.GetFilePath(selectedIndex));
        m_shotcutNameLabel.MoveWindow(rc.left + (rc.right - rc.left - 500) / 2, rc.bottom - 20, 500, 20, 1);
        m_shotcutNameLabel.ShowWindow(SW_SHOW);
    }

#endif
}

const CPoint &CDockDlg::GetMousePoint() const
{
    static CPoint mouse_pt;
    ::GetCursorPos(&mouse_pt);
    return mouse_pt;
}


void CDockDlg::UpdateLastMousePosition(const CPoint &pt)
{
    m_lastPoint.x = pt.x;
    m_lastPoint.y = pt.y;
}


bool CDockDlg::EnoughMouseMove(const CPoint &pt)
{
    return abs(m_lastPoint.x - pt.x) > 3 || abs(m_lastPoint.y - pt.y) > 3;
}



void CDockDlg::showHideWindowFromShotcut()
{
    static bool bShowWindow = false;

    if (!bShowWindow)
    {
        KillTimer(TIMER_HIDE);
        UpdatePanelSize();
        this->ShowWindow(SW_SHOWNORMAL);
        bShowWindow = true;
        int shot_index = m_shortManager.GetShotcutIndexByShotcut(0);
        m_shortManager.LayOutShortCutsByShotCut(m_dockPanelWidth, m_dockPanelHeight, shot_index);
    }
    else
    {
        this->ShowWindow(SW_HIDE);
        SetTimer(TIMER_HIDE, TIME_ELAPSE_HIDE, NULL);
        bShowWindow = false;
    }
}



void CDockDlg::ChooseByShotcut(int key)
{
    int shot_key = 0;
    if (0x4a == key)  // j
    {
        shot_key = 1;
    }
    else if (0x46 == key)
    {
        shot_key = -1;
    }

    int shot_index = m_shortManager.GetShotcutIndexByShotcut(shot_key);
    m_shortManager.LayOutShortCutsByShotCut(m_dockPanelWidth, m_dockPanelHeight, shot_index);
}


void CDockDlg::registerHotKey()
{
    RegisterHotKey(this->GetSafeHwnd(), IDH_HOT1, MOD_ALT, VK_RETURN);
    //RegisterHotKey(this->GetSafeHwnd(), IDH_HOT1, MOD_ALT, MOD_ALT);
}


bool CDockDlg::IsMouseEnterToActiveArea() const
{
    CPoint pt = GetMousePoint();
    if (pt.x >= m_dockPanelLeft && 
        pt.x <= (m_dockPanelLeft + m_dockPanelWidth)) 
    {
        if (pt.y == 0 || 
           (pt.y < 0 && pt.y > -10)  // -10 < y < 0 is for extend screen (on top of main screen)
           )
        {
            return true;
        }
    }

    return false;
}



void CDockDlg::OnTimer(UINT_PTR nIDEvent)
{
    static int s_lastTouchingIndex = -1;

    if (nIDEvent == TIMER_HIDE && !Preference::alwaysShow)
    {
        static int iCountHide = 0;

        if (IsMouseEnterToActiveArea()) 
        {
            if (++iCountHide == 2) {
                KillTimer(TIMER_HIDE);
                UpdatePanelSize();
                this->ShowWindow(SW_SHOWNORMAL);
            }
        }
        else {
            iCountHide = 0;
        }
    }
    else if (nIDEvent == TIMER_SHOW && !Preference::alwaysShow)
    {
        //Check mouse coordinates and hide if the mouse haven't been in the window for a few seconds
        RECT lpRect;
        ::GetWindowRect(m_hWnd, &lpRect);
        POINT pt;
        ::GetCursorPos(&pt);
        static int iCountShow = -5;

        if (PtInRect(&lpRect, pt) == FALSE && !m_bDraggingImage){ // when dragging shortcut, not hide
            if (++iCountShow == 1) {
                this->ShowWindow(SW_HIDE);
                SetTimer(TIMER_HIDE, TIME_ELAPSE_HIDE, NULL);
            }
        }
        else {
            iCountShow = 0;
        }
    }
    else if (nIDEvent == TIMER_MOVE)
    {
        CPoint point;
        ::GetCursorPos(&point);
        ScreenToClient(&point);

        if (m_bDraggingImage)  // dragging
        {
            int j = ButtonIndexMouseOnH();
            TRACE(_T("dragging j= %d\n"), j);

            if (j >= 0 && EnoughMouseMove(point))
            {
                s_lastTouchingIndex = j;
                m_shortManager.LayOutShortCutsDragging(m_dockPanelWidth, m_dockPanelHeight, point, m_buttonIndexDragging);
                //Invalidate();
                UpdateLastMousePosition(point);
            }
        }
        else  // roaming
        {
            int j = ButtonIndexMouseOn();

            if (j >= 0 && EnoughMouseMove(point))  // roaming
            {
                s_lastTouchingIndex = j;
                m_shortManager.LayOutShortCutsTouching(m_dockPanelWidth, m_dockPanelHeight, point);
                //Invalidate();
                UpdateLastMousePosition(point);
                UpdateLabelPosition(j);
            }
            else if (-1 == j && -1 != s_lastTouchingIndex && EnoughMouseMove(point)) // prevent to window flick, roll back to normal position
            {
                m_shotcutNameLabel.ShowWindow(SW_HIDE);
                m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
                //Invalidate();
                s_lastTouchingIndex = -1;
                UpdateLastMousePosition(point);
            }
        }
    }

    CDialog::OnTimer(nIDEvent);
}


BOOL CDockDlg::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
    //return FALSE;
    //return CDialog::OnEraseBkgnd(pDC);
}

void CDockDlg::OnClose()
{
    m_shortManager.SaveShortcutToIniFile();

    DeleteObject(m_hBitmap);//not really need but what the heck.
    KillTimer(TIMER_HIDE);
    KillTimer(TIMER_SHOW);
    KillTimer(TIMER_MOVE);
    CDialog::OnClose();
}

void CDockDlg::OnBnClickedOk()
{
    KillTimer(TIMER_HIDE);
    KillTimer(TIMER_SHOW);
    KillTimer(TIMER_MOVE);
    OnOK();
}

void CDockDlg::OnBnClickedCancel()
{
    KillTimer(TIMER_HIDE);
    KillTimer(TIMER_SHOW);
    KillTimer(TIMER_MOVE);
    OnCancel();
}


void CDockDlg::AddNewButton(TCHAR *pFilePath)
{
    if (!m_shortManager.CanAddShortCut(m_dockPanelWidth, m_dockPanelHeight, m_shortManager.GetShortCutNums() + 1))
    {
        return;
    }

    if (!m_shortManager.InsertItem(this, pFilePath))
    {
        return; //already exist
    }

    UpdatePanelSize();  // enlarge working area if needed
    m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
    Invalidate();

    m_shortManager.SaveShortcutToIniFile();
}


void CDockDlg::OnDropFiles(HDROP hDropInfo)
{
    CDialog::OnDropFiles(hDropInfo);
    UINT        uNumFiles;
    TCHAR       szNextFile [MAX_PATH];
    LVFINDINFO  rlvFind = { LVFI_STRING, szNextFile };
    CString strItem;

    uNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );

    for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
    {
        // Get the next filename from the HDROP info.
        if ( DragQueryFile ( hDropInfo, uFile, szNextFile, MAX_PATH ) > 0 )
        {
            OutputDebugString(szNextFile);
            AddNewButton(szNextFile);
        }
    }

    DragFinish ( hDropInfo );
}


void CDockDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
}


void CDockDlg::OnPostEraseBkgnd(CDC *pDC)
{
}


void CDockDlg::OnButtonDynamic(UINT nID)
{
    return;
}


int CDockDlg::FindDraggingImageIndex()
{
    for (int i = 0; i < m_shortManager.GetShortCutNums(); i++)
    {
        if (m_shortManager.IsPressed(i))
        {
            return i;
        }
    }

    return -1;
}



void CDockDlg::PreHandleMouseMove()
{
    //OutputDebugString(_T("CXbuttonTestDlg::PreHandleMouseMove\n"));
    if (-1 != m_buttonIndexDragging && !m_bDraggingImage)
    {
        POINT pointNew;
        ::GetCursorPos(&pointNew);

        // enough drag distance is needed. otherwise it will too sensitivity.to drag icon instead "click" button
        if (abs(pointNew.x - m_pPointBeginDrag.x) > 5 ||
            abs(pointNew.y - m_pPointBeginDrag.y) > 5 )
        {
            m_bDraggingImage = true;
            PrepareDraggingDialog();
            UpdateLastMousePosition(pointNew);
        }
    }

    if (m_bDraggingImage)
    {
        if (m_buttonIndexDragging >= 0)
        {
            POINT pointNew;
            ::GetCursorPos(&pointNew);

            // enough drag distance is needed. otherwise it will too sensitivity.to drag icon instead "click" button
            //if (abs(pointNew.x - m_pPointBeginDrag.x) > 5 ||
            //    abs(pointNew.y - m_pPointBeginDrag.y) > 5 )
            {
                m_pDragDialog->ShowWindow(SW_SHOW);
                m_pDragDialog->MoveWindow(pointNew.x, pointNew.y, DEFAULT_DRAG_DIALOG_WIDTH, DEFAULT_DRAG_DIALOG_HEIGHT, FALSE);
                //m_pDragDialog->MoveWindow(pointNew.x, pointNew.y, -1, -1);
                SetCapture();

                if (!m_bEdittingCutItem)
                {
                    m_shortManager.DragShortcutItemFromList(m_buttonIndexDragging);
                    m_bEdittingCutItem = true;
                }
            }
        }
    }
    else
    {
        ReleaseCapture();
    }
}


void CDockDlg::OnReleaseDraggingAction()
{
    RECT rc;
    this->GetWindowRect(&rc);
    POINT pt;
    ::GetCursorPos(&pt);

    CRect rc2(rc.left, rc.top, rc.right, rc.bottom);

    if (!rc2.PtInRect(pt))
    {
        // if cursor is out of dock panel, to remove shortcut
        if (m_shortManager.CanRemoveDraggingShortcut())
        {
            if(Preference::alert_before_delete && IDCANCEL == MessageBox(_T("Remove this shortcut?"), _T(""), MB_OKCANCEL))
            {
                return;
            }

            m_shortManager.RemoveDraggingShortcut();
            m_shortManager.SaveShortcutToIniFile();

            UpdatePanelSize();
            m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
            Invalidate();
        }
        else  // rollback system icon to old position
        {
            int index = m_buttonIndexDragging;

            if (m_buttonIndexDragging == m_shortManager.GetShortCutNums())  // last one
            {
                index = - 1;    // roll back to old position
            }

            m_shortManager.SettleDownDraggingCutItem(index);
            UpdatePanelSize();
            m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
            Invalidate();
        }
    }
    else
    {
        // if cursor is in dock panel, consider change position of shortcut
        if (m_bEdittingCutItem)
        {
            if (-1 != m_buttonIndexDragging)
            {
                int index = ButtonIndexMouseOnH();

                if (999 == index)
                {
                    index = 0;
                }
                else if (m_buttonIndexDragging == m_shortManager.GetShortCutNums())  // last one
                {
                    index = - 1;    // roll back to old position
                }

                m_shortManager.SettleDownDraggingCutItem(index);
                UpdatePanelSize();
                m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
                Invalidate();
            }
        }
    }
}



void CDockDlg::PrepareDraggingDialog()
{
    if (NULL != m_pDragDialog)
    {
        delete m_pDragDialog;
        m_pDragDialog = NULL;
    }

    m_pDragDialog = new CDragDialogEx(NULL, DuplicateIcon(NULL, m_shortManager.GetIcon(m_buttonIndexDragging)));
    m_pDragDialog->ShowWindow(SW_HIDE);
}


void CDockDlg::PreHandleMouseLButtonDown()
{
    OutputDebugString(_T("CXbuttonTestDlg::PreHandleMouseLButtonDown\n"));
    int j = ButtonIndexMouseOn();

    if (j >= 0)
    {
        CPoint point;
        ::GetCursorPos(&point);
        m_pPointBeginDrag = point;
        m_buttonIndexDragging = j;  // prepare to drag.
    }
}


void CDockDlg::PreHandleMouseLButtonUp()
{
    OutputDebugString(_T("CXbuttonTestDlg::PreHandleMouseLButtonUp\n"));

    if (m_bDraggingImage)
    {
        if (m_pDragDialog)
        {
            m_pDragDialog->ShowWindow(SW_HIDE);
        }

        OnReleaseDraggingAction();
    }

    m_bDraggingImage = false;
    m_pPointBeginDrag.x = -1;
    m_pPointBeginDrag.y = -1;
    m_bEdittingCutItem = false;
    m_buttonIndexDragging = -1;
}


BOOL CDockDlg::PreTranslateMessage(MSG *pMsg)
{
    if (pMsg->message == WM_MOUSEMOVE)
    {
        PreHandleMouseMove();
    }
    else if (pMsg->message == WM_LBUTTONDOWN)
    {
        PreHandleMouseLButtonDown();
    }
    else if (pMsg->message == WM_LBUTTONUP)
    {
        PreHandleMouseLButtonUp();
    }
    else if (pMsg->message == WM_HOTKEY)
    {
        switch (pMsg->wParam)
        {
        case IDH_HOT1:
            showHideWindowFromShotcut();
            return TRUE;
        }
    }
    else if (pMsg->message == WM_KEYDOWN)
    {
        switch (pMsg->wParam)
        {
        case 0x46:
            ChooseByShotcut(0x46);
            break;
        case 0x4a:
            ChooseByShotcut(0x4a);
            break;
        case VK_RETURN:
            m_shortManager.OpenShotcutByShotkey();
            break;
        }
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}


void CDockDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    if (bShow)  // auto hide timer
    {
        //::MoveWindow(m_hWnd, lpRectTop.left, lpRectTop.top, lpRectTop.right-lpRectTop.left, lpRectTop.bottom-lpRectTop.top, TRUE);
        UpdatePanelSize();
        //SetTimer(TIMER_SHOW, 500, NULL);
    }
    else  // auto show timer
    {
        //SetTimer(TIMER_HIDE, 1000, NULL);
    }
}


int CDockDlg::ButtonIndexMouseOn()
{
    CPoint point;
    ::GetCursorPos(&point);
    this->ScreenToClient(&point);
    return m_shortManager.IndexOnShotCurtButton(point);
}


int CDockDlg::ButtonIndexMouseOnH()
{
    CPoint point;
    ::GetCursorPos(&point);
    this->ScreenToClient(&point);
    point.y = 10;  // a little trick
    return m_shortManager.IndexOnShotCurtButtonH(point);
}


HBRUSH CDockDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    //HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    //return hbr;
    return NULL;
}


void CDockDlg::OnAddNewShortCur()
{
    UpdatePanelSize();  // enlarge working area if needed
    m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
    Invalidate();
}


void CDockDlg::UpdateSystemIconList(int iconListMap)
{
    m_shortManager.UpdateSystemShortcutBitmap(iconListMap);

    // remove unselected icons
    int count = m_shortManager.GetSystemIconMaxNumberSupport();

    for (int i = 0; i < count; i++)
    {
        bool bExit = (iconListMap & (0x00000001 << i)) != 0 ? true : false;

        if (bExit)
        {
            m_shortManager.InsertSystemItem(i);
        }
        else
        {
            m_shortManager.RemoveSystemItem(i);
        }
    }

    UpdatePanelSize();
    m_shortManager.LayOutShortCuts(m_dockPanelWidth, m_dockPanelHeight);
    // add selected icons
}
