#include "StdAfx.h"
#include "ShortCutLayout.h"

#include "ShortCutManager.h"

ShortCutLayout::ShortCutLayout(ShortCutManager *pManager)
    : m_pShortCutManager(pManager)
    , m_shortcutWidth(60)
    , m_shortcutHeight(60)
{
}

ShortCutLayout::~ShortCutLayout(void)
{
}


int ShortCutLayout::GetIndexMouseOn(const CPoint &mousePt, /*out*/ float* percent) const
{
    int shortCutNumber = m_pShortCutManager->GetShortCutNums();

    if (0 == shortCutNumber)
    {
        return -1;
    }

    int left = (m_dockPanelSize.cx - m_shortcutWidth * shortCutNumber) / 2; //+ m_rectLeft;
    RECT rect = {left, 0, left + m_shortcutWidth * shortCutNumber, 60};

    if (!PtInRect(&rect, mousePt))
    {
        return -1;
    }
    else
    {
        if (NULL != percent)
        {
            int w = m_shortcutWidth / 2;
            int offset = (mousePt.x - left) % m_shortcutWidth;
            offset = offset > w? m_shortcutWidth - offset: offset;
            *percent = offset / (float)w;
            TRACE(_T("percent = %f\n"), *percent);
        }
        
        return (mousePt.x - left) / m_shortcutWidth;
    }
}


// only used in dragging
int ShortCutLayout::GetIndexMouseOnH(const CPoint &mousePt, /*out*/ float* percent) const
{
    int shortCutNumber = m_pShortCutManager->GetShortCutNums();

    if (0 == shortCutNumber)
    {
        return -1;
    }

    int left = (m_dockPanelSize.cx - m_shortcutWidth * (shortCutNumber + 1)) / 2; //+ m_rectLeft;
    RECT rect = {left, 0, left + m_shortcutWidth *(shortCutNumber + 1), 60};

    if (mousePt.x < left)
    {
        return 999;
    }
    else if (mousePt.x > rect.right)
    {
        return -1;
    }
    else
    {
        if (NULL != percent)
        {
            int w = m_shortcutWidth / 2;
            int offset = (mousePt.x - left) % m_shortcutWidth;
            offset = offset > w? m_shortcutWidth - offset: offset;
            *percent = offset / (float)w;
        }

        return (mousePt.x - left) / m_shortcutWidth;
    }
}


bool ShortCutLayout::IsPanelBigEnough(const int &dockPanelWidth, const int &dockPanelHeight, int shortcutCount)
{
    if (dockPanelWidth - m_shortcutWidth * shortcutCount <= 0)
    {
        return false;
    }

    return true;
}


void ShortCutLayout::LayOutShortCuts(const int &dockPanelWidth, const int &dockPanelHeight)
{
    TRACE(_T("LayOutShortCuts\n"));
    m_dockPanelSize.cx = dockPanelWidth;
    m_dockPanelSize.cy = dockPanelHeight;
    int shortCutNumber = m_pShortCutManager->GetShortCutNums();

    // coordinate offset from Dlg, not screen.
    int left = (dockPanelWidth - m_shortcutWidth * shortCutNumber) / 2; //+ m_rectLeft;

    for (int i = 0; i < shortCutNumber; i++)
    {
        m_shortCutRect[i].left = left + m_shortcutWidth * i;
        m_shortCutRect[i].top = SHOTCUT_SHOW_MARGIN_TOP;
        m_shortCutRect[i].right = m_shortCutRect[i].left + m_shortcutWidth;
        m_shortCutRect[i].bottom = m_shortCutRect[i].top + m_shortcutHeight;
        m_pShortCutManager->m_shortcutList[i]->UpdateState(ShortCutItem::SC_STATE_NORMAL);
    }

    for (int i = 0; i < shortCutNumber; i++)
    {
        m_pShortCutManager->m_shortcutList[i]->UpdatePosition(
            m_shortCutRect[i].left,
            m_shortCutRect[i].top,
            m_shortCutRect[i].right - m_shortCutRect[i].left,
            m_shortCutRect[i].bottom - m_shortCutRect[i].top);
    }
}


void ShortCutLayout::LayOutShortCutsByShotCut(const int &dockPanelWidth, const int &dockPanelHeight, int short_cut_index)
{
    LayOutShortCuts(dockPanelWidth, dockPanelHeight);
    m_pShortCutManager->m_shortcutList[short_cut_index]->UpdateState(ShortCutItem::SC_STATE_ROAMING, 1);
}


void ShortCutLayout::LayOutShortCutsTouching(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point)
{
    m_dockPanelSize.cx = dockPanelWidth;
    m_dockPanelSize.cy = dockPanelHeight;
    int shortCutCount = m_pShortCutManager->GetShortCutNums();

    int leftFrom = (dockPanelWidth - m_shortcutWidth * shortCutCount) / 2; //+ m_rectLeft;
    float percent = 0.0f;
    int indexMouseOn = m_pShortCutManager->IndexOnShotCurtButton(point, &percent);
    if (-1 == indexMouseOn)
    {
        // TODO why?
        return;
    }
    

    for (int i = 0; i < shortCutCount; i++)
    {
        m_shortCutRect[i].left = leftFrom + m_shortcutWidth * i;
        m_shortCutRect[i].right  = m_shortCutRect[i].left + m_shortcutWidth;
        m_shortCutRect[i].top = SHOTCUT_SHOW_MARGIN_TOP;
        m_shortCutRect[i].bottom = m_shortCutRect[i].top + m_shortcutHeight;

        m_pShortCutManager->m_shortcutList[i]->UpdateState(ShortCutItem::SC_STATE_NORMAL);
    }

    // drop down effect
    m_shortCutRect[indexMouseOn].bottom = m_shortCutRect[indexMouseOn].top + m_shortcutHeight + 50;
    m_pShortCutManager->m_shortcutList[indexMouseOn]->UpdateState(ShortCutItem::SC_STATE_ROAMING, percent);

    //if (indexMouseOn >= 1)  m_pShortCutManager->m_shortcutList[indexMouseOn - 1]->UpdateState(ShortCutItem::SC_STATE_NEARBY);
    //if (indexMouseOn > shortCutNumber - 1)  m_pShortCutManager->m_shortcutList[indexMouseOn + 1]->UpdateState(ShortCutItem::SC_STATE_NEARBY);

    //RECT rc;
    //this->GetRect(rc);
    //int w = (60 - 48) / 2;
    //int h = (60 - 48) / 2;
    //rc.left -= w;
    //rc.top  -= h;
    //rc.right += w;
    //rc.bottom += h;
    //m_pButton->SetWindowPos(m_pParent, x, y, width, height,
    //    SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

    for (int i = 0; i < shortCutCount; i++)
    {
        m_pShortCutManager->m_shortcutList[i]->UpdatePosition(
            m_shortCutRect[i].left,
            m_shortCutRect[i].top,
            m_shortCutRect[i].right - m_shortCutRect[i].left,
            m_shortCutRect[i].bottom - m_shortCutRect[i].top);
    }
}


void ShortCutLayout::LayOutShortCutsDragging(const int &dockPanelWidth, const int &dockPanelHeight, const CPoint &point, int draggingIndex)
{
    TRACE(_T("LayOutShortCutsDragging dragginInex == %d\n"), draggingIndex);
    // dragging
#if 1
    // virtual button (space)
    static int s_lastvirtualButtonIndex = -1;
    int virtualButtonWidth = m_shortcutWidth;
    int virtualButtonHeight = m_shortcutHeight;
    int virtualButtonIndex = -1;
    int indexMouseOn = m_pShortCutManager->IndexOnShotCurtButtonH(point);


    int shortCutNumber = m_pShortCutManager->GetShortCutNums();
    int originalleftFrom = (dockPanelWidth - m_shortcutWidth * (shortCutNumber + 1)) / 2;
    //float pctY = 0;
    //if (point.y >= 0 && point.y < m_shortcutHeight)
    //{
    //    pctY = ((float)(m_shortcutHeight - point.y)) / m_shortcutHeight;
    //}

    if (draggingIndex == indexMouseOn)
    {
        virtualButtonIndex = indexMouseOn;
        virtualButtonWidth = m_shortcutWidth;
        s_lastvirtualButtonIndex = s_lastvirtualButtonIndex;
    }
    else
    {
        if (999 == indexMouseOn)  // left of first
        {
            virtualButtonIndex = 0;
            virtualButtonWidth = m_shortcutWidth;
        }
        else
        {
            float pctX = (float)((point.x - originalleftFrom) % m_shortcutWidth) / m_shortcutWidth;
            virtualButtonIndex = indexMouseOn;

            if (pctX <= 0.5)  // insert space before [position] button
            {
                if (s_lastvirtualButtonIndex > indexMouseOn)  // move to left
                {
                    if (pctX > 0.4 && pctX < 0.5)
                    {
                        virtualButtonWidth = m_shortcutWidth * 1.5;
                    }
                    else
                    {
                        virtualButtonWidth = m_shortcutWidth;
                    }

                    virtualButtonIndex = indexMouseOn;
                }
            }
            else if (pctX > 0.5)
            {
                if (s_lastvirtualButtonIndex < indexMouseOn)  // move to right
                {
                    if (pctX > 0.5 && pctX < 0.6)
                    {
                        virtualButtonWidth = m_shortcutWidth;
                    }
                    else
                    {
                        virtualButtonWidth = m_shortcutWidth;
                    }

                    virtualButtonIndex = indexMouseOn + 1;
                }
            }

            TRACE(_T("pctX = %f, virtualButtonWidth = %d\n"), pctX, virtualButtonWidth);
        }

        s_lastvirtualButtonIndex = virtualButtonIndex;
    }

    TRACE(_T("virtualButtonIndex = %d, virtualButtonWidth = %d\n"), virtualButtonIndex, virtualButtonWidth);

    {
        //left part
        for (int i = 0; i < virtualButtonIndex; i++)
        {
            m_shortCutRect[i].left = originalleftFrom;
            m_shortCutRect[i].right  = m_shortCutRect[i].left + m_shortcutWidth;
            m_shortCutRect[i].top = SHOTCUT_SHOW_MARGIN_TOP;
            m_shortCutRect[i].bottom = m_shortCutRect[i].top + m_shortcutHeight;
            originalleftFrom = m_shortCutRect[i].right;
        }

        // virtual button
        originalleftFrom += virtualButtonWidth;

        // right part
        for (int i = virtualButtonIndex; i < shortCutNumber; i++)
        {
            m_shortCutRect[i].left = originalleftFrom;
            m_shortCutRect[i].right  = m_shortCutRect[i].left + m_shortcutWidth;
            m_shortCutRect[i].top = SHOTCUT_SHOW_MARGIN_TOP;
            m_shortCutRect[i].bottom = m_shortCutRect[i].top + m_shortcutHeight;
            originalleftFrom = m_shortCutRect[i].right;
        }
    }

    for (int i = 0; i < shortCutNumber; i++)
    {
        m_pShortCutManager->m_shortcutList[i]->UpdatePosition(
            m_shortCutRect[i].left,
            m_shortCutRect[i].top,
            m_shortCutRect[i].right - m_shortCutRect[i].left,
            m_shortCutRect[i].bottom - m_shortCutRect[i].top);
    }

    return;
#endif
}