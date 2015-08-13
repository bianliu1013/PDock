#pragma once

class ShortCutManager;
class ShortCutLayout
{
public:
    ShortCutLayout(ShortCutManager *);
    ~ShortCutLayout(void);

    void LayOutShortCutsTouching(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point);
    void LayOutShortCutsDragging(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point, int draggingIndex);
    void LayOutShortCuts(const int &dockPanelWidth, const int &dockPanelHeight);
    void LayOutShortCutsByShotCut(const int &dockPanelWidth, const int &dockPanelHeight, int short_cut_index);
    int GetIndexMouseOn(const CPoint &mousePt, /*out*/ float* percent = NULL) const;
    int GetIndexMouseOnH(const CPoint &mousePt, /*out*/ float* percent = NULL) const;
    bool IsPanelBigEnough(const int &dockPanelWidth, const int &dockPanelHeight, int shortcutCount);

private:
    CSize               m_dockPanelSize;
    RECT                m_shortCutRect[50];
    ShortCutManager    *m_pShortCutManager;
    int                 m_shortcutWidth;
    int                 m_shortcutHeight;

};
