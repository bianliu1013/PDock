#include "StdAfx.h"

#include <windows.h>
#include "ShortCutItem.h"
#include "tools.h"
#include "resource.h"
#include "BitmapEx.h"

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

#include <stdio.h>



extern HBITMAP IconToBitmap(HICON hIcon, SIZE *pTargetSize);

BEGIN_MESSAGE_MAP(ShortCutItem, CWnd)
    ON_WM_MOUSEMOVE()
    //ON_WM_MOUSEHOVER() 
    //ON_WM_MOUSELEAVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


static const int SC_IMAGE_SIZE_NORMAL   = 37;
static const int SC_IMAGE_SIZE_NEARBY   = 48;
static const int SC_IMAGE_SIZE_ROAMING  = 60;


ShortCutItem::ShortCutItem(IShortCutListener *pListener, CWnd *pParent, TCHAR *path, HICON icon, BOOL bSystem, int systemIconIndex)
    : m_pShortCutManager(pListener)
    , m_pParent(pParent)
    , m_bSystemIcon(bSystem)
    , m_iSystemIconIndex(systemIconIndex)
    , m_pToolTip(NULL)
    , isMouseIn(false)
    , isPressed(false)
    , state_(SC_STATE_NONE)
{
    percent_    = 0.0f;
    szClassName = AfxRegisterWndClass(0);
    m_hBitmap   = IconToBitmap(icon, NULL);
    pos_magic_  = 0;

    m_imgBitmap_ =  CBitmap::FromHandle(m_hBitmap);

    bitmapEx_ = new CBitmapEx();
    bitmapEx_->Create(SC_IMAGE_SIZE_ROAMING, SC_IMAGE_SIZE_ROAMING);
    bitmapEx_->Load(m_hBitmap);

    UpdateState(state_);
    m_hIcon = icon;
    gdiplug_bitmap_ = new Gdiplus::Bitmap(m_hIcon);

    m_pFilePath = new TCHAR[MAX_PATH];
    _tcscpy(m_pFilePath, path);
}


bool ShortCutItem::Create(CWnd *pParent, CRect rc, CString text, DWORD id /* = 0 */, DWORD style /* = WS_VISIBLE|WS_CHILD */)
{
    BOOL ret = CWnd::CreateEx(0, szClassName, text, style, rc, pParent, id);

    LONG ExtendedStyle = GetWindowLong( GetSafeHwnd(),
                                        GWL_EXSTYLE );
    SetWindowLong( GetSafeHwnd(),
                   GWL_EXSTYLE,
                   ExtendedStyle | WS_EX_LAYERED );

    // Set the alpha for transparency.
    // 0 is transparent and 255 is opaque.
    double fAlpha = 255 * ( 255.0 / 100 );
    BYTE byAlpha = static_cast<BYTE>( fAlpha );
    SetLayeredWindowAttributes( 0,
                                byAlpha,
                                LWA_ALPHA );

    m_pToolTip = new CToolTipCtrl;

    if (!m_pToolTip->Create(this, TTS_ALWAYSTIP))
    {
        TRACE("Unable To create ToolTip\n");
        return TRUE;
    }

    // color for tooltip doesn't work, why? TODO
    //m_pToolTip->SetTipTextColor(RGB(67, 110, 238));
    //m_pToolTip->SetTipBkColor(RGB(67, 110, 238));
    m_pToolTip->SetDelayTime(TTDT_INITIAL, 100);

    if (m_pToolTip->AddTool(this, m_pFilePath))
    {
        TRACE("Unable to add Dialog to the tooltip\n");
    }


    return ret ? true : false;
}

ShortCutItem::~ShortCutItem(void)
{
    //if (m_pButton)
    //{
    //    delete m_pButton;
    //    m_pButton = NULL;
    //}
    delete m_pToolTip;
    m_pToolTip = NULL;
    m_pParent = NULL;
    m_pShortCutManager = NULL;
}


void ShortCutItem::PostClickEvent()
{
    CWnd *parent = GetParent();

    if (parent != NULL)
    {
        WPARAM wp = MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED);
        LPARAM lp = (LPARAM) m_hWnd;
        parent->PostMessage(WM_COMMAND, wp, lp);

        if (m_pShortCutManager)
        {
            m_pShortCutManager->OnShortCurtClicked(this);
        }
    }
}


void ShortCutItem::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!isMouseIn)
    {
        isMouseIn = true;

        TRACKMOUSEEVENT evt = { sizeof(evt), TME_LEAVE, m_hWnd, 0 };
        TrackMouseEvent(&evt);

        OnMouseHover(0, CPoint());


        //if(m_pShortCutManager)
        //{
        //    m_pShortCutManager->OnMouseOnShortCurt(this);
        //}
    }
}


void ShortCutItem::OnMouseHover(UINT nFlags, CPoint point)
{
    // mouse enter
    if (m_pShortCutManager)
    {
        m_pShortCutManager->OnMouseOnShortCurt(this);
    }

    //Invalidate();
}


void ShortCutItem::OnMouseLeave()
{
    isMouseIn = false;
    isPressed = false;
    //Invalidate();
}

void ShortCutItem::OnLButtonDown(UINT nFlags, CPoint point)
{
    isPressed = true;
    //Invalidate();
}


void ShortCutItem::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (isPressed)
    {
        isPressed = false;
        //Invalidate();

        PostClickEvent();
    }
}


void ShortCutItem::PaintBk(CDC *pDC)
{
    CClientDC clDC(GetParent());
    CRect rect;
    CRect rect1;

    GetClientRect(rect);

    GetWindowRect(rect1);
    GetParent()->ScreenToClient(rect1);

    if (m_dcBk.m_hDC == NULL)
    {
        m_dcBk.CreateCompatibleDC(&clDC);
        m_bmpBk.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
        m_pbmpOldBk = m_dcBk.SelectObject(&m_bmpBk);
        m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
    } // if

    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcBk, 0, 0, SRCCOPY);
} // End of PaintBk


void TransparentBlt( HDC hdcDest, int nXDest, int nYDest, int nWidth, 
                    int nHeight, CDC* pDcSrc, int nXSrc, int nYSrc,
                    COLORREF colorTransparent,
                    int nWidthDest/* = -1*/, int nHeightDest/* = -1*/)
{
    CDC dc, memDC, maskDC;
    dc.Attach( hdcDest );
    maskDC.CreateCompatibleDC(&dc);
    CBitmap maskBitmap;

    int cx = nWidthDest == -1 ? nWidth : nWidthDest;
    int cy = nHeightDest == -1 ? nHeight : nHeightDest;

    //add these to store return of SelectObject() calls
    CBitmap* pOldMemBmp = NULL;
    CBitmap* pOldMaskBmp = NULL;

    memDC.CreateCompatibleDC(&dc);
    CBitmap bmpImage;
    bmpImage.CreateCompatibleBitmap( &dc, cx, cy);
    pOldMemBmp = memDC.SelectObject( &bmpImage );

    if (nWidthDest == -1 || (nWidthDest == nWidth && nHeightDest == nHeight))
    {
        memDC.BitBlt( 0,0,nWidth, nHeight, pDcSrc, nXSrc, nYSrc, SRCCOPY);
    }
    else
    {
        memDC.StretchBlt (0,0, nWidthDest, nHeightDest, pDcSrc, 
            nXSrc, nYSrc, nWidth, nHeight, SRCCOPY);
    }

    // Create monochrome bitmap for the mask
    maskBitmap.CreateBitmap (cx, cy, 1, 1, NULL );
    pOldMaskBmp = maskDC.SelectObject( &maskBitmap );
    memDC.SetBkColor( colorTransparent );

    // Create the mask from the memory DC
    maskDC.BitBlt (0, 0, cx, cy, &memDC, 0, 0, SRCCOPY);

    // Set the background in memDC to black. Using SRCPAINT with black 
    // and any other color results in the other color, thus making 
    // black the transparent color
    memDC.SetBkColor(RGB(0,0,0));
    memDC.SetTextColor(RGB(255,255,255));
    memDC.BitBlt(0, 0, cx, cy, &maskDC, 0, 0, SRCAND);

    // Set the foreground to black. See comment above.
    dc.SetBkColor(RGB(255,255,255));
    dc.SetTextColor(RGB(0,0,0));

    dc.BitBlt (nXDest, nYDest, cx, cy, &maskDC, 0, 0, SRCAND);

    // Combine the foreground with the background
    dc.BitBlt(nXDest, nYDest, cx, cy, &memDC, 
        0, 0, SRCPAINT);

    if (pOldMaskBmp)
        maskDC.SelectObject( pOldMaskBmp );
    if (pOldMemBmp)
        memDC.SelectObject( pOldMemBmp );

    dc.Detach();
}


void DrawTransparentBitmap(CDC *pDC, int nXDest, int nYDest, int nWidth, int nHeight, CDC *pSDC, int SX, int SY, COLORREF colorTransparent)
{
    CDC  MemDC, maskDC;
    CBitmap MemBmp, maskBmp, * pOldBmp;

    MemDC.CreateCompatibleDC(pDC);
    maskDC.CreateCompatibleDC(pSDC);

    MemBmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);
    maskBmp.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

    pOldBmp = MemDC.SelectObject(&MemBmp);
    maskDC.SelectObject(&maskBmp);

    MemDC.BitBlt(0, 0, nWidth, nHeight, pSDC, SX, SY, SRCCOPY);
    MemDC.SetBkColor(colorTransparent);
    maskDC.BitBlt(0, 0, nWidth, nHeight, &MemDC, 0, 0, SRCCOPY);

    MemDC.SetBkColor(RGB(0, 0, 0));
    MemDC.SetTextColor(RGB(255, 255, 255));
    MemDC.BitBlt(0, 0, nWidth, nHeight, &maskDC, 0, 0, SRCAND);

    ASSERT(pDC->m_hDC);
    pDC->SetBkColor(RGB(255, 255, 255));
    pDC->SetTextColor(RGB(0, 0, 0));
    pDC->BitBlt(nXDest, nYDest, nWidth, nHeight, &maskDC, 0, 0, SRCAND);
    pDC->BitBlt(nXDest, nYDest, nWidth, nHeight, &MemDC, 0, 0, SRCPAINT);

    MemDC.SelectObject (pOldBmp);

    MemBmp.DeleteObject();
    maskBmp.DeleteObject();
    MemDC.DeleteDC();
    maskDC.DeleteDC();
}



void ShortCutItem::OnPaint()
{


    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);

    //PaintBk(&dc);

    // gdi+
    Gdiplus::Graphics graphics(dc);
    Gdiplus::Rect rc(bitmap_show_rc_.left, bitmap_show_rc_.top, bitmap_show_rc_.right - bitmap_show_rc_.left, bitmap_show_rc_.bottom - bitmap_show_rc_.top);
    graphics.DrawImage(gdiplug_bitmap_, rc);

    // way 1
    //CDC memDC;
    //memDC.CreateCompatibleDC(&dc);
    //CBitmap *pOldBitmap = memDC.SelectObject(CBitmap::FromHandle(hBitmap_scale_));
    //DrawTransparentBitmap(&dc
    //    , bitmap_show_rc_.left
    //    , bitmap_show_rc_.top
    //    , bitmap_show_rc_.right - bitmap_show_rc_.left
    //    , bitmap_show_rc_.bottom - bitmap_show_rc_.top
    //    , &memDC, 0, 0, RGB(0, 0, 0));

    // way 2
    //CDC memDC;
    //memDC.CreateCompatibleDC(&dc);
    //CBitmap *pOldBitmap = memDC.SelectObject(CBitmap::FromHandle(m_hBitmap));
    //TransparentBlt(dc.GetSafeHdc(), 0, 0,48, 48, &memDC, 0, 0, RGB(0, 0, 0),  rect.Width()/2, rect.Height() / 2);

    // way 3
    //DrawTransparentBitmap(&dc, 0, 0, rect.Width(), rect.Height() / 2, &memDC, 0, 0, RGB(0, 0, 0));


    //memDC.SelectObject(pOldBitmap);

    //CImage* img = new CImage();
    //img->Attach(m_hBitmap);
    //img->Load(_T("D://1.bmp"));
    //img->Load(_T("D://1.ico"));
    //img->Load(_T("D://arrow_down37.png"));
    //img->Draw(dc, 0, 0, rect.Width(), rect.Height(), 0, 0, rect.Width(), rect.Height());



    CWnd::OnPaint();
}


bool ShortCutItem::IsPressed() const
{
    //return m_pButton->IsPressed();
    return 0;
}


HICON ShortCutItem::GetIcon() const
{
    return m_hIcon;
}


void ShortCutItem::GetRect(/*out*/RECT *rect) const
{
    //m_pButton->GetWindowRect(rect);
}


void ShortCutItem::_reRender()
{
    // erase background and repaint this window, do not call invalid directly, it will not erase background
    RECT rc_client;
    this->GetWindowRect(&rc_client);
    GetParent()->ScreenToClient(&rc_client);
    GetParent()->InvalidateRect(&rc_client, TRUE);
}


void ShortCutItem::UpdatePosition(int x, int y, int width, int height)
{
    //m_pButton->SetWindowPos(m_pParent, x, y, width, height, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
    //SetWindowPos(m_pParent, x, y, width, height, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
    //::MoveWindow(this->GetSafeHwnd(), x, y, width, height, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

    long long matic_temp = (x * y) | width | height;
    if (matic_temp != pos_magic_)
    {
        ::MoveWindow(this->GetSafeHwnd(), x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
        pos_magic_ = matic_temp;

        _reRender();
    }
}


void ShortCutItem::UpdateState(const SC_STATE& sc , float percent)
{
    if (state_ == sc && percent_ == percent)
    {
        return;
    }

    percent_ = percent;
    state_ = sc;
    bitmapEx_->Clear();
    bitmapEx_->Load(m_hBitmap);
    int offset =  0;
    switch(state_)
    {
    case SC_STATE_NORMAL:
        bitmapEx_->Scale2(SC_IMAGE_SIZE_NORMAL, SC_IMAGE_SIZE_NORMAL);
        bitmap_show_rc_.top     = (60 - SC_IMAGE_SIZE_NORMAL) / 2;
        bitmap_show_rc_.left    = (60 - SC_IMAGE_SIZE_NORMAL) / 2;
        bitmap_show_rc_.right   = bitmap_show_rc_.left + SC_IMAGE_SIZE_NORMAL;
        bitmap_show_rc_.bottom  = bitmap_show_rc_.top + SC_IMAGE_SIZE_NORMAL;
        break;
    case SC_STATE_NEARBY:
        bitmapEx_->Scale2(SC_IMAGE_SIZE_NEARBY, SC_IMAGE_SIZE_NEARBY);
        bitmap_show_rc_.top     = (60 - SC_IMAGE_SIZE_NEARBY) / 2;
        bitmap_show_rc_.left    = (60 - SC_IMAGE_SIZE_NEARBY) / 2;
        bitmap_show_rc_.right   = bitmap_show_rc_.left + SC_IMAGE_SIZE_NEARBY;
        bitmap_show_rc_.bottom  = bitmap_show_rc_.top + SC_IMAGE_SIZE_NEARBY;
        break;
    case SC_STATE_ROAMING:
        bitmapEx_->Scale2(SC_IMAGE_SIZE_ROAMING, SC_IMAGE_SIZE_ROAMING);
        offset = SC_IMAGE_SIZE_ROAMING - (SC_IMAGE_SIZE_ROAMING - SC_IMAGE_SIZE_NORMAL) * (float)(1 - (float)percent);
        bitmap_show_rc_.top     = (60 - offset) / 2;
        bitmap_show_rc_.left    = (60 - offset) / 2;
        bitmap_show_rc_.right   = bitmap_show_rc_.left + offset;
        bitmap_show_rc_.bottom  = bitmap_show_rc_.top + offset;
        break;
    default:
        bitmapEx_->Scale2(SC_IMAGE_SIZE_NORMAL, SC_IMAGE_SIZE_NORMAL);
        bitmap_show_rc_.top     = (60 - SC_IMAGE_SIZE_NORMAL) / 2;
        bitmap_show_rc_.left    = (60 - SC_IMAGE_SIZE_NORMAL) / 2;
        bitmap_show_rc_.right   = bitmap_show_rc_.left + SC_IMAGE_SIZE_NORMAL;
        bitmap_show_rc_.bottom  = bitmap_show_rc_.top + SC_IMAGE_SIZE_NORMAL;
        break;
    }
    hBitmap_scale_ = NULL;
    bitmapEx_->Save(hBitmap_scale_);

    _reRender();
}


int ShortCutItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    return 0;
}


BOOL ShortCutItem::PreTranslateMessage(MSG *pMsg)
{
    if (NULL != m_pToolTip)
    {
        m_pToolTip->RelayEvent(pMsg);
    }

    return CWnd::PreTranslateMessage(pMsg);
}


BOOL ShortCutItem::OnEraseBkgnd(CDC *pDC)
{
    //return CWnd::OnEraseBkgnd(pDC);
    return TRUE;
}


HBRUSH ShortCutItem::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
    return NULL;
}