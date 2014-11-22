#include "StdAfx.h"
#include "ShortCutManager.h"
#include "ShortCutItem.h"
#include "tools.h"
#include "resource.h"
#include "global.h"
#include "SettingDlg.h"

extern HBITMAP ConvertIconToBitmap(HICON hIcon);


ShortCutManager::ShortCutManager()
    : m_pDraggingCutItem(NULL)
    , m_layout(this)
{
}

ShortCutManager::~ShortCutManager(void)
{
}


void ShortCutManager::SetShortCutManagerListener(IShortCutManagerListener *pListener)
{
    m_pListener = pListener;
}


void ShortCutManager::Init(CDialog *pParent)
{
    m_dockPanelDlg = pParent;
    initDefaultIcons();
    LoadReservedShortcut();
}


void ShortCutManager::initDefaultIcons()
{
    // get icons from: http://www.softicons.com/free-icons/system-icons/7
    HINSTANCE hDll;
    hDll = LoadLibrary(_T("SHELL32.dll"));
    /*
    m_initIcon[0]   = LoadIcon (hDll , MAKEINTRESOURCE (16)); // my computer
    m_initIcon[1] = LoadIcon (hDll, MAKEINTRESOURCE ( 18 )); // net work
    m_initIcon[2] = LoadIcon (hDll, MAKEINTRESOURCE ( 22 )); // control panel
    m_initIcon[3] = LoadIcon (hDll, MAKEINTRESOURCE ( 21 )); // time data
    m_initIcon[4] = LoadIcon (hDll, MAKEINTRESOURCE ( 33 )); // recycle
    m_initIcon[5] = LoadIcon (hDll, MAKEINTRESOURCE ( 28 )); // exit

    m_initIcon[SYSTEM_SHORTCUT_INDEX_MY_COMPUTER] = (HICON)LoadImage(hDll, MAKEINTRESOURCE(16), IMAGE_ICON, DEFAULT_ICOH_SIZE_WIDHT, DEFAULT_ICOH_SIZE_HEIGHT, LR_DEFAULTCOLOR);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_NETWORK] = (HICON)LoadImage(hDll, MAKEINTRESOURCE(18), IMAGE_ICON, DEFAULT_ICOH_SIZE_WIDHT, DEFAULT_ICOH_SIZE_HEIGHT, LR_DEFAULTCOLOR);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_CONTROL_PANEL] = (HICON)LoadImage(hDll, MAKEINTRESOURCE(22), IMAGE_ICON, DEFAULT_ICOH_SIZE_WIDHT, DEFAULT_ICOH_SIZE_HEIGHT, LR_DEFAULTCOLOR);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_TIME_DATE] = (HICON)LoadImage(hDll, MAKEINTRESOURCE(21), IMAGE_ICON, DEFAULT_ICOH_SIZE_WIDHT, DEFAULT_ICOH_SIZE_HEIGHT, LR_DEFAULTCOLOR);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_RECYCLE] = (HICON)LoadImage(hDll, MAKEINTRESOURCE(33), IMAGE_ICON, DEFAULT_ICOH_SIZE_WIDHT, DEFAULT_ICOH_SIZE_HEIGHT, LR_DEFAULTCOLOR);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_CLOSE] = (HICON)LoadImage(hDll, MAKEINTRESOURCE(28), IMAGE_ICON, DEFAULT_ICOH_SIZE_WIDHT, DEFAULT_ICOH_SIZE_HEIGHT, LR_DEFAULTCOLOR);
    */

    //HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nIconIn), RT_GROUP_ICON);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_TIME_DATE] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_DATETIME_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_MY_COMPUTER] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_COMPUTER_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_NETWORK] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_NETWORK_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_RECYCLE] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_RECYCLEBIN_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_MY_MUSIC] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MUSC_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_USERS_FILE] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_DOCUMENT_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_CLOSE] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_EXIT_48), IMAGE_ICON, 48, 48, 0);
    m_initIcon[SYSTEM_SHORTCUT_INDEX_CONTROL_PANEL] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_CONTROLPANEL_48), IMAGE_ICON, 48, 48, 0);

    //m_initIcon[SYSTEM_SHORTCUT_INDEX_TIME_DATE] = (HICON)LoadImage(0, MAKEINTRESOURCE(IDI_ICON_DATETIME_48), IMAGE_ICON, 48, 48, 0);
    //m_initIcon[] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE());
    //m_initIcon[] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE());
    //m_initIcon[] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE());
    //m_initIcon[] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE());

    // preference
    m_initIcon[SYSTEM_SHORTCUT_INDEX_PREFERENCE] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_ST_48), IMAGE_ICON, 48, 48, 0);

    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_MY_COMPUTER] = _T("My Computer");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_NETWORK] = _T("Net Work");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_CONTROL_PANEL] = _T("Control Panel");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_TIME_DATE] = _T("Time Date");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_RECYCLE] = _T("Recycle");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_USERS_FILE] = _T("User's file");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_MY_MUSIC] = _T("My music");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_CLOSE] = _T("Exit");
    m_initIconTitle[SYSTEM_SHORTCUT_INDEX_PREFERENCE] = _T("Options");
}


ShortCutItem *ShortCutManager::MakeSystemShortCut(int systemIndex)
{
    MYICON_INFO info = MyGetIconInfo(m_initIcon[systemIndex]);
    ShortCutItem *p = new ShortCutItem(this, m_dockPanelDlg, m_initIconTitle[systemIndex].GetBuffer(), DuplicateIcon(NULL, m_initIcon[systemIndex]), true, systemIndex);
    RECT rc;
    p->Create((CWnd *)m_dockPanelDlg, CRect(10, 10, 210, 30), _T("Nice Work"));
    return p;
}


void ShortCutManager::InsertSystemItem(int index)
{
    std::vector<ShortCutItem *>::iterator itor = m_shortcutList.begin();

    while (itor != m_shortcutList.end())
    {
        ShortCutItem *p = *itor;

        if (p->GetSystemIconIndex() == index)
        {
            // already exist
            return;
        }

        itor++;
    }

    ShortCutItem *p = MakeSystemShortCut(index);
    m_shortcutList.push_back(p);
}


void ShortCutManager::RemoveSystemItem(int index)
{
    std::vector<ShortCutItem *>::iterator itor = m_shortcutList.begin();

    while (itor != m_shortcutList.end())
    {
        ShortCutItem *p = *itor;

        if (p->GetSystemIconIndex() == index)
        {
            m_shortcutList.erase(itor);
            p->ShowWindow(SW_HIDE);
            delete p;
            p = NULL;
            break;
        }
        else
        {
            itor++;
        }
    }
}


int ShortCutManager::GetSystemIconMaxNumberSupport() const
{
    return SYSTEM_SHORTCUT_NUM_EDITABLE;
}


void ShortCutManager::LoadReservedShortcut()
{
    // open .ini file
    TCHAR szIniPath[MAX_PATH] = L"c:\\WXH.ini";
    int pathNumber = GetPrivateProfileInt(L"ShortCutNumber", L"Number", 0, szIniPath);

    int i = 0;
    int systemIconBitMap = 0;

    while (i < pathNumber)
    {
        // get path
        TCHAR szPath[MAX_PATH] = {0};
        TCHAR szPathSeq[10] = {0};
        _stprintf(szPathSeq, _T("%s%02d"), _T("Path"), i);
        GetPrivateProfileString(L"ShortCut", szPathSeq, L"NULL", szPath, MAX_PATH, szIniPath);

        CString str(szPath);

        // system icon begin with system_
        if (str.Left(7) == _T("system_"))
        {
            str.Delete(0, 7);
            int systemIndex = _wtoi(str.GetString());

            if (systemIndex < SYSTEM_SHORTCUT_NUM_EDITABLE)
            {
                InsertSystemItem(systemIndex);
                systemIconBitMap |= 0x00000001 << systemIndex;
            }
        }
        else
        {
            InsertItem(m_dockPanelDlg, szPath);
        }

        m_pListener->OnAddNewShortCur();
        i++;
    }

    InsertSystemItem(SYSTEM_SHORTCUT_INDEX_PREFERENCE);
    InsertSystemItem(SYSTEM_SHORTCUT_INDEX_CLOSE);
    UpdateSystemShortcutBitmap(systemIconBitMap);
}


/*
[ShortCutNumber]
Number=2
[ShortCut]
Path05=C:\Users\19007180\Desktop\PDock\PDock.sln
*/
void ShortCutManager::SaveShortcutToIniFile()
{
    TCHAR szIniPath[MAX_PATH] = L"c:\\WXH.ini";
    int number = m_shortcutList.size();
    CString strNumver;
    strNumver.Format(_T("%d"), number);
    WritePrivateProfileString(L"ShortCutNumber", L"Number", strNumver, szIniPath);

    int i = 0;
    std::vector<ShortCutItem *>::iterator itor = m_shortcutList.begin();

    while (itor != m_shortcutList.end())
    {
        ShortCutItem *p = *itor;

        if (!p->IsSytem())
        {
            TCHAR szPathSeq[10] = {0};
            _stprintf(szPathSeq, _T("%s%02d"), _T("Path"), i);
            WritePrivateProfileString(L"ShortCut", szPathSeq, p->GetPath(), szIniPath);
        }
        else  // system ICON
        {
            TCHAR szPathSeq[10] = {0};
            _stprintf(szPathSeq, _T("%s%02d"), _T("Path"), i);
            CString str;
            str.Format(_T("%s_%d"), _T("system"), p->GetSystemIconIndex());
            WritePrivateProfileString(L"ShortCut", szPathSeq, str, szIniPath);
        }

        i++;
        itor++;
    }
}


bool ShortCutManager::InsertItem(CDialog *pParent, TCHAR *path)
{
    std::vector<ShortCutItem *>::iterator itor = m_shortcutList.begin();

    while (itor != m_shortcutList.end())
    {
        ShortCutItem *p = *itor;

        if (0 == _tcscmp(p->GetPath(), path))
        {
            // already exist
            return false;
        }

        itor++;
    }

    HICON hIcon = GetHighResolutionIcon(path);
    MYICON_INFO info = MyGetIconInfo(hIcon);
    ShortCutItem *p = new ShortCutItem(this, pParent, path, hIcon, false, -1);
    RECT rc;
    p->Create(pParent, CRect(10, 10, 210, 30), _T("Nice Work"));
    std::vector<ShortCutItem *>::iterator it = m_shortcutList.end();
    m_shortcutList.insert(it, p);
}


void ShortCutManager::AddItem(ShortCutItem *item)
{
    m_shortcutList.push_back(item);
}


TCHAR *ShortCutManager::GetFilePath(int index)
{
    return m_shortcutList[index]->GetPath();
}


bool ShortCutManager::IsPressed(int index)
{
    return m_shortcutList[index]->IsPressed();
}


HICON ShortCutManager::GetIcon(int index)
{
    return m_shortcutList[index]->GetIcon();
}


int ShortCutManager::IndexOnShotCurtButton(const CPoint &point, float* percent) const
{
    return m_layout.GetIndexMouseOn(point, percent);
}


int ShortCutManager::IndexOnShotCurtButtonH(const CPoint &point, float* percent) const
{
    return m_layout.GetIndexMouseOnH(point, percent);
}


void ShortCutManager::LayOutShortCutsTouching(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point)
{
    m_layout.LayOutShortCutsTouching(dockPanelWidth, dockPanelHeight, point);
}

void ShortCutManager::LayOutShortCutsDragging(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point, int draggingIndex)
{
    m_layout.LayOutShortCutsDragging(dockPanelWidth, dockPanelHeight, point, draggingIndex);
}


void ShortCutManager::LayOutShortCuts(const int &dockPanelWidth, const int &dockPanelHeight)
{
    m_layout.LayOutShortCuts(dockPanelWidth, dockPanelHeight);
}


void ShortCutManager::RemoveShortcut(int index)
{
    int i = 0;

    for (std::vector<ShortCutItem *>::iterator iter = m_shortcutList.begin(); iter != m_shortcutList.end(); ++iter)
    {
        if (i++ == index)
        {
            ShortCutItem *p = *iter;
            m_shortcutList.erase(iter);
            p->ShowWindow(SW_HIDE);
            delete p;
            return;
        }
    }
}


void ShortCutManager::RemoveDraggingShortcut()
{
    if (m_pDraggingCutItem)
    {
        m_pDraggingCutItem->ShowWindow(SW_HIDE);
    }
}


bool ShortCutManager::CanRemoveShortcut(int index)
{
    return true;
#if 0
    int i = 0;

    for (std::vector<ShortCutItem *>::iterator iter = m_shortcutList.begin(); iter != m_shortcutList.end(); ++iter)
    {
        if (i++ == index)
        {
            ShortCutItem *p = *iter;

            if (p->IsSytem())
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }

    return false;
#endif
}


bool ShortCutManager::CanRemoveDraggingShortcut()
{
    // can not remove "preference" and "close"
    if (SYSTEM_SHORTCUT_INDEX_CLOSE == m_pDraggingCutItem->GetSystemIconIndex() ||
        SYSTEM_SHORTCUT_INDEX_PREFERENCE == m_pDraggingCutItem->GetSystemIconIndex())
    {
        return false;
    }

    return true;
}


void ShortCutManager::OnMouseOnShortcut(ShortCutItem *pShortCut)
{
}


void ShortCutManager::OnShortCurtClicked(ShortCutItem *pShortCut)
{
    //console.cpl
    //timedate.cpl
    //desk.cpl
    //for my document


    //int indexSelected = GetIndex(pShortCut);
    int indexSelected = pShortCut->GetSystemIconIndex();

    if (indexSelected == SYSTEM_SHORTCUT_INDEX_MY_COMPUTER)
    {
        //My Computer
        ShellExecute(NULL, _T("open"), _T("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"), NULL, NULL, SW_SHOWNORMAL);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_NETWORK)
    {
        //Network
        ShellExecute(NULL, _T("open"), _T("::{208D2C60-3AEA-1069-A2D7-08002B30309D}") , NULL, NULL, SW_SHOWNORMAL);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_CONTROL_PANEL)
    {
        //Control Panel
        ShellExecute(NULL, _T("open"), _T("rundll32.exe"), _T("shell32.dll,Control_RunDLL"), NULL, SW_SHOW);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_TIME_DATE)
    {
        // time date
        ShellExecute(NULL, _T("open"), _T("control.exe"), _T("timedate.cpl"), _T("c:\\windows\\system32\\"), SW_NORMAL);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_RECYCLE)
    {
        // recycle
        ShellExecute(NULL, _T("open"),  _T("::{645FF040-5081-101B-9F08-00AA002F954E}"), NULL, NULL, SW_SHOWNORMAL);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_USERS_FILE)
    {
        // my document
        TCHAR str[MAX_PATH] = {0};
        SHGetSpecialFolderPath(NULL, str, CSIDL_PERSONAL, FALSE);
        ShellExecute(NULL, _T("open"), str, NULL, NULL, SW_SHOWNORMAL);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_MY_MUSIC)
    {
        // my music
        TCHAR str[MAX_PATH] = {0};
        SHGetSpecialFolderPath(NULL, str, CSIDL_MYMUSIC, FALSE);
        ShellExecute(NULL, _T("open"), str, NULL, NULL, SW_SHOWNORMAL);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_CLOSE)  // close
    {
        AfxGetMainWnd()->SendMessage(WM_CLOSE);
    }
    else if (indexSelected == SYSTEM_SHORTCUT_INDEX_PREFERENCE)  // preference
    {
        CSettingDlg pSettingDlg(m_dockPanelDlg, m_systemIconBitMap);
        pSettingDlg.DoModal();
    }
    else
    {
        TCHAR *pFilePath = pShortCut->GetPath();
        ShellExecute(NULL, _T("open"), pFilePath, _T(""), _T(""), SW_SHOW );
    }
}


void ShortCutManager::DragShortcutItemFromList(int index)
{
    int i = 0;

    for (std::vector<ShortCutItem *>::iterator iter = m_shortcutList.begin(); iter != m_shortcutList.end(); ++iter)
    {
        if (i++ == index)
        {
            m_pDraggingCutItem = *iter;
            m_shortcutList.erase(iter);
            //m_pDraggingCutItem->GetButton()->ShowWindow(SW_HIDE);
            m_pDraggingCutItem->ShowWindow(SW_HIDE);
            //delete p;
            return;
        }
    }
}


// index = 0,   insert the head of list
//         -1,  insert to the tail of list
void ShortCutManager::SettleDownDraggingCutItem(int index)
{
    if (NULL == m_pDraggingCutItem)
    {
        return;
    }

    if (-1 == index || index == m_shortcutList.size())
    {
        m_pDraggingCutItem->ShowWindow(SW_NORMAL);
        m_shortcutList.push_back(m_pDraggingCutItem);
    }
    else if (0 == index || 999 == index)
    {
        m_pDraggingCutItem->ShowWindow(SW_NORMAL);
        m_shortcutList.insert(m_shortcutList.begin(), m_pDraggingCutItem);
    }
    else
    {
        int i = 0;

        for (std::vector<ShortCutItem *>::iterator iter = m_shortcutList.begin(); iter != m_shortcutList.end(); iter++)
        {
            if (i++ == index)
            {
                m_pDraggingCutItem->ShowWindow(SW_NORMAL);
                m_shortcutList.insert(iter, m_pDraggingCutItem);
                return;
            }
        }
    }
}


bool ShortCutManager::CanAddShortCut(const int &dockPanelWidth, const int &dockPanelHeight, int shortcutCount)
{
    return m_layout.IsPanelBigEnough(dockPanelWidth, dockPanelHeight, shortcutCount);
}
