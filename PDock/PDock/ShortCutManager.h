#pragma once

#include "afxwin.h"

#include "global.h"
#include "ShortCutItem.h"
#include "ShortCutLayout.h"

#include <vector>


class IShortCutManagerListener
{
public:
    IShortCutManagerListener() {}
    ~IShortCutManagerListener() {}

    virtual void OnAddNewShortCur() {}
};


class CDockDlg;
class ShortCutManager: public IShortCutListener
{
public:
    ShortCutManager();
    ~ShortCutManager(void);

    void Init(CDialog *pParent);
    void SetShortCutManagerListener(IShortCutManagerListener *pListener);

    inline int GetShortCutNums() const
    {
        return m_shortcutList.size();
    }

    virtual void OnMouseOnShortcut(ShortCutItem *);
    virtual void OnShortCurtClicked(ShortCutItem *);

    bool InsertItem(CDialog *,  TCHAR *);
    void InsertSystemItem(int index);
    void RemoveSystemItem(int index);
    void AddItem(ShortCutItem *item);
    void RemoveShortcut(int index);
    void RemoveDraggingShortcut();
    bool CanRemoveDraggingShortcut();
    bool CanRemoveShortcut(int index);

    int GetSystemIconMaxNumberSupport() const;

    bool CanAddShortCut(const int &dockPanelWidth, const int &dockPanelHeight, int shortcutCount);

    TCHAR *GetFilePath(int index);
    bool IsPressed(int index);
    HICON GetIcon(int index);

    // check the index of button when mouse is inside [index]th button's area, if none, return -1
    int IndexOnShotCurtButton(const CPoint &point, float* percent = NULL) const;

    // same with above function, but only check value on Horizontal, return -1 (right of last). 999 (left of first). others (index of shortcut).
    int IndexOnShotCurtButtonH(const CPoint &point, float* percent = NULL) const;

    void LayOutShortCuts(const int &dockPanelWidth, const int &dockPanelHeight);
    void LayOutShortCutsTouching(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point);
    void LayOutShortCutsDragging(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point, int draggingIndex);

    void SaveShortcutToIniFile();
    void DragShortcutItemFromList(int index);
    void SettleDownDraggingCutItem(int index);  // insert before [index]th, if -1, append to end

    void UpdateSystemShortcutBitmap(int systemIconBitMap)
    {
        m_systemIconBitMap = systemIconBitMap;
    }
    int GetSystemShortcutBitmap() const
    {
        return m_systemIconBitMap;
    }

private:
    void initDefaultIcons();
    void LoadReservedShortcut();
    ShortCutItem *MakeSystemShortCut(int systemIndex);


private:
    friend class ShortCutLayout;

    CDialog                    *m_dockPanelDlg;
    IShortCutManagerListener   *m_pListener;
    std::vector<ShortCutItem *>  m_shortcutList;
    HICON                       m_initIcon[SYSTEM_SHORTCUT_NUM];
    CString                     m_initIconTitle[SYSTEM_SHORTCUT_NUM];
    ShortCutItem               *m_pDraggingCutItem;  // temp value, save the object that dragging
    ShortCutLayout              m_layout;
    int                         m_systemIconBitMap;
};
