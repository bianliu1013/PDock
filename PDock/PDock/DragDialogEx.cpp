// DragDialogEx.cpp : implementation file
//

#include "stdafx.h"
#include "Dock.h"
#include "DragDialogEx.h"

#include "global.h"


// CDragDialogEx dialog

IMPLEMENT_DYNAMIC(CDragDialogEx, CDialog)


HBITMAP IconToBitmap(HICON hIcon, SIZE *pTargetSize);

CDragDialogEx::CDragDialogEx(CWnd *pParent, HICON icon)
    : CDialog(CDragDialogEx::IDD, pParent)
    , m_hIcon(icon)
{
    Create(IDD_DIALOG_DRAG, pParent);
}

CDragDialogEx::~CDragDialogEx()
{
    DestroyIcon(m_hIcon);
}

void CDragDialogEx::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_STATIC, m_Picture);
}


BEGIN_MESSAGE_MAP(CDragDialogEx, CDialog)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()




void CDragDialogEx::SetTransparency( float transparencyRatio )
{
    LONG ExtendedStyle = GetWindowLong( GetSafeHwnd(),
                                        GWL_EXSTYLE );

    if (transparencyRatio > 0.0f && transparencyRatio < 100.01f)
    {
        SetWindowLong( GetSafeHwnd(),
                       GWL_EXSTYLE,
                       ExtendedStyle | WS_EX_LAYERED );

        // Select the transparency percentage.
        // The alpha will be calculated accordingly.
        double TransparencyPercentage = transparencyRatio;

        // Set the alpha for transparency.
        // 0 is transparent and 255 is opaque.
        double fAlpha = TransparencyPercentage * ( 255.0 / 100 );
        BYTE byAlpha = static_cast<BYTE>( fAlpha );
        SetLayeredWindowAttributes( 0,
                                    byAlpha,
                                    LWA_ALPHA );
    }
    else
    {
        SetWindowLong( GetSafeHwnd(),
                       GWL_EXSTYLE,
                       ExtendedStyle & ~WS_EX_LAYERED );
    }
}


void CDragDialogEx::CreateRegion(HWND hWnd)
{
    //#define TRANSPARENTCOLOR RGB(255, 255, 255);  // if background is white (transparent color)
#define TRANSPARENTCOLOR RGB(0, 0, 0);
    if (m_hBitmap == NULL)
    {
        AfxMessageBox( L"Error loading bitmap" );
        return;
    }

    //Get information about the bitmap..
    GetObject(m_hBitmap, sizeof(m_Bitmap), &m_Bitmap);  // Get info about the bitmap
    // Put the bitmap into a memory device context
    //CPaintDC dc(this);
    CDC dcMem;
    //create a compatible dc
    dcMem.CreateCompatibleDC(0);    // Select the bitmap into the in-memory DC
    //Select the bitmap into the dc
    CBitmap *pOldBitmap = dcMem.SelectObject(CBitmap::FromHandle(m_hBitmap));
    //Create a couple of region objects.
    CRgn crRgn, crRgnTmp;
    //create an empty region
    crRgn.CreateRectRgn(0, 0, 0, 0);

    //Create a region from a bitmap with transparency color of Purple
    COLORREF crTransparent = TRANSPARENTCOLOR;
    int iX = 0;

    for (int iY = 0; iY < m_Bitmap.bmHeight; iY++)
    {
        do
        {
            //skip over transparent pixels at start of lines.
            while (iX <= m_Bitmap.bmWidth && dcMem.GetPixel(iX, iY) == crTransparent)
            {
                iX++;
            }

            //remember this pixel
            int iLeftX = iX;

            //now find first non transparent pixel
            while (iX <= m_Bitmap.bmWidth && dcMem.GetPixel(iX, iY) != crTransparent)
            {
                ++iX;
            }

            //create a temp region on this info
            crRgnTmp.CreateRectRgn(iLeftX, iY, iX, iY + 1);
            //combine into main region.
            crRgn.CombineRgn(&crRgn, &crRgnTmp, RGN_OR);
            //delete the temp region for next pass (otherwise you'll get an ASSERT)
            crRgnTmp.DeleteObject();
        }
        while (iX < m_Bitmap.bmWidth);

        iX = 0;
    }

    iX = (GetSystemMetrics(SM_CXSCREEN)) / 2 - (m_Bitmap.bmWidth / 2);
    int iY = (GetSystemMetrics(SM_CYSCREEN)) / 2 - (m_Bitmap.bmHeight / 2);
    //SetWindowPos( &(this->m_hWnd, iX, iY, m_Bitmap.bmWidth, m_Bitmap.bmHeight, NULL );
    ::MoveWindow(hWnd, iX, iY, m_Bitmap.bmWidth, m_Bitmap.bmHeight, FALSE);

    ::SetWindowRgn(hWnd, crRgn, TRUE);

    // Free resources.
    dcMem.SelectObject(pOldBitmap); // Put the original bitmap back (prevents memory leaks)
    dcMem.DeleteDC();
    crRgn.DeleteObject();
}

// CDragDialogEx message handlers

BOOL CDragDialogEx::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetTransparency(95);

    //HRGN rgn = BitmapToRegion(ConvertIconToBitmap(m_hIcon), RGB(0, 0, 0), RGB(0, 0, 0));
    //SetWindowRgn(rgn, TRUE);

    m_hBitmap = IconToBitmap(m_hIcon, NULL);
    //m_hBitmap = ConvertIconToBitmap(m_hIcon);
    CreateRegion(m_hWnd);

    return TRUE;
}




HBITMAP IconToBitmap(HICON hIcon, SIZE *pTargetSize)
{
    ICONINFO info = {0};

    if (hIcon == NULL
        || !GetIconInfo(hIcon, &info)
        || !info.fIcon)
    {
        return NULL;
    }

    INT nWidth = 0;
    INT nHeight = 0;

    if (pTargetSize != NULL)
    {
        nWidth = pTargetSize->cx;
        nHeight = pTargetSize->cy;
    }
    else
    {
        if (info.hbmColor != NULL)
        {
            BITMAP bmp = {0};
            GetObject(info.hbmColor, sizeof(bmp), &bmp);

            nWidth = bmp.bmWidth;
            nHeight = bmp.bmHeight;
        }
    }

    if (info.hbmColor != NULL)
    {
        DeleteObject(info.hbmColor);
        info.hbmColor = NULL;
    }

    if (info.hbmMask != NULL)
    {
        DeleteObject(info.hbmMask);
        info.hbmMask = NULL;
    }

    if (nWidth <= 0
        || nHeight <= 0)
    {
        return NULL;
    }

    INT nPixelCount = nWidth * nHeight;

    HDC dc = GetDC(NULL);
    INT *pData = NULL;
    HDC dcMem = NULL;
    HBITMAP hBmpOld = NULL;
    bool *pOpaque = NULL;
    HBITMAP dib = NULL;
    BOOL bSuccess = FALSE;

    do
    {
        BITMAPINFOHEADER bi = {0};
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = nWidth;
        bi.biHeight = -nHeight;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        dib = CreateDIBSection(dc, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (VOID **)&pData, NULL, 0);

        if (dib == NULL)
        {
            break;
        }

        memset(pData, 0, nPixelCount * 4);

        dcMem = CreateCompatibleDC(dc);

        if (dcMem == NULL)
        {
            break;
        }

        hBmpOld = (HBITMAP)SelectObject(dcMem, dib);
        ::DrawIconEx(dcMem, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_MASK);

        pOpaque = new(std::nothrow) bool[nPixelCount];

        if (pOpaque == NULL)
        {
            break;
        }

        for (INT i = 0; i < nPixelCount; ++i)
        {
            pOpaque[i] = !pData[i];
        }

        memset(pData, 0, nPixelCount * 4);
        ::DrawIconEx(dcMem, 0, 0, hIcon, nWidth, nHeight, 0, NULL, DI_NORMAL);

        BOOL bPixelHasAlpha = FALSE;
        UINT *pPixel = (UINT *)pData;

        for (INT i = 0; i < nPixelCount; ++i, ++pPixel)
        {
            if ((*pPixel & 0xff000000) != 0)
            {
                bPixelHasAlpha = TRUE;
                break;
            }
        }

        if (!bPixelHasAlpha)
        {
            pPixel = (UINT *)pData;

            for (INT i = 0; i < nPixelCount; ++i, ++pPixel)
            {
                if (pOpaque[i])
                {
                    *pPixel |= 0xFF000000;
                }
                else
                {
                    *pPixel &= 0x00FFFFFF;
                }
            }
        }

        bSuccess = TRUE;

    }
    while (FALSE);


    if (pOpaque != NULL)
    {
        delete []pOpaque;
        pOpaque = NULL;
    }

    if (dcMem != NULL)
    {
        SelectObject(dcMem, hBmpOld);
        DeleteDC(dcMem);
    }

    ReleaseDC(NULL, dc);

    if (!bSuccess)
    {
        if (dib != NULL)
        {
            DeleteObject(dib);
            dib = NULL;
        }
    }

    return dib;
}

HBITMAP CDragDialogEx::ConvertIconToBitmap(HICON hIcon)
{
    CClientDC clientDC(this);
    CDC dc;
    dc.CreateCompatibleDC(&clientDC);

    CRect rectButton;
    GetClientRect (&rectButton);
    CSize sizeButton = rectButton.Size();

    // bitmap size:
    int cx = 0, cy = 0;

    if (sizeButton.cy <= 16)
    {
        // bitmap size:
        cx = 12;
        cy = 12;
    }
    else if (sizeButton.cy > 16)
    {
        // bitmap size:
        //cx = 16;
        //cy = 16;
        cx = 48;
        cy = 48;
    }

    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&clientDC, cx, cy);
    CBitmap *pOldBmp = (CBitmap *)dc.SelectObject(&bmp);

    ::DrawIconEx( dc.GetSafeHdc(), 0, 0, hIcon, cx, cy, 0, (HBRUSH)RGB(192, 192, 192), DI_NORMAL);
    dc.SelectObject( pOldBmp );
    dc.DeleteDC();

    // return the image
    return ((HBITMAP)::CopyImage((HANDLE)((HBITMAP)bmp), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
}



BOOL CDragDialogEx::OnEraseBkgnd(CDC *pDC)
{
    // no flick.
    //return TRUE;
    return CDialog::OnEraseBkgnd(pDC);
}




void CDragDialogEx::OnPaint()
{
    CClientDC dc(this);
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap *pOldBitmap = memDC.SelectObject(CBitmap::FromHandle(m_hBitmap));

    CRect rect;
    GetClientRect(&rect);
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBitmap);

    CDialog::OnPaint();
}



HRGN CDragDialogEx::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
    HRGN hRgn = NULL;

    if (hBmp)
    {
        HDC hMemDC = CreateCompatibleDC(NULL);

        if (hMemDC)
        {
            BITMAP bm;
            GetObject(hBmp, sizeof(bm), &bm);

            BITMAPINFOHEADER RGB32BITSBITMAPINFO =
            {
                sizeof(BITMAPINFOHEADER),       // biSize
                bm.bmWidth,                 // biWidth;
                bm.bmHeight,                // biHeight;
                1,                          // biPlanes;
                32,                         // biBitCount
                BI_RGB,                     // biCompression;
                0,                          // biSizeImage;
                0,                          // biXPelsPerMeter;
                0,                          // biYPelsPerMeter;
                0,                          // biClrUsed;
                0                           // biClrImportant;
            };
            VOID *pbits32;
            HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);

            if (hbm32)
            {
                HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

                // Create a DC just to copy the bitmap into the memory DC
                HDC hDC = CreateCompatibleDC(hMemDC);

                if (hDC)
                {
                    // Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
                    BITMAP bm32;
                    GetObject(hbm32, sizeof(bm32), &bm32);

                    while (bm32.bmWidthBytes % 4)
                    {
                        bm32.bmWidthBytes++;
                    }

                    // Copy the bitmap into the memory DC
                    HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
                    BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

                    // For better performances, we will use the ExtCreateRegion() function to create the
                    // region. This function take a RGNDATA structure on entry. We will add rectangles by
                    // amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT  100
                    DWORD maxRects = ALLOC_UNIT;
                    HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
                    RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
                    pData->rdh.dwSize = sizeof(RGNDATAHEADER);
                    pData->rdh.iType = RDH_RECTANGLES;
                    pData->rdh.nCount = pData->rdh.nRgnSize = 0;
                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

                    // Keep on hand highest and lowest values for the "transparent" pixels
                    BYTE lr = GetRValue(cTransparentColor);
                    BYTE lg = GetGValue(cTransparentColor);
                    BYTE lb = GetBValue(cTransparentColor);
                    BYTE hr = min(0xff, lr + GetRValue(cTolerance));
                    BYTE hg = min(0xff, lg + GetGValue(cTolerance));
                    BYTE hb = min(0xff, lb + GetBValue(cTolerance));

                    // Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
                    BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;

                    for (int y = 0; y < bm.bmHeight; y++)
                    {
                        // Scan each bitmap pixel from left to right
                        for (int x = 0; x < bm.bmWidth; x++)
                        {
                            // Search for a continuous range of "non transparent pixels"
                            int x0 = x;
                            LONG *p = (LONG *)p32 + x;

                            while (x < bm.bmWidth)
                            {
                                BYTE b = GetRValue(*p);

                                if (b >= lr && b <= hr)
                                {
                                    b = GetGValue(*p);

                                    if (b >= lg && b <= hg)
                                    {
                                        b = GetBValue(*p);

                                        if (b >= lb && b <= hb)
                                            // This pixel is "transparent"
                                        {
                                            break;
                                        }
                                    }
                                }

                                p++;
                                x++;
                            }

                            if (x > x0)
                            {
                                // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
                                if (pData->rdh.nCount >= maxRects)
                                {
                                    GlobalUnlock(hData);
                                    maxRects += ALLOC_UNIT;
                                    hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
                                    pData = (RGNDATA *)GlobalLock(hData);
                                }

                                RECT *pr = (RECT *)&pData->Buffer;
                                SetRect(&pr[pData->rdh.nCount], x0, y, x, y + 1);

                                if (x0 < pData->rdh.rcBound.left)
                                {
                                    pData->rdh.rcBound.left = x0;
                                }

                                if (y < pData->rdh.rcBound.top)
                                {
                                    pData->rdh.rcBound.top = y;
                                }

                                if (x > pData->rdh.rcBound.right)
                                {
                                    pData->rdh.rcBound.right = x;
                                }

                                if (y + 1 > pData->rdh.rcBound.bottom)
                                {
                                    pData->rdh.rcBound.bottom = y + 1;
                                }

                                pData->rdh.nCount++;

                                // On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
                                // large (ie: > 4000). Therefore, we have to create the region by multiple steps.
                                if (pData->rdh.nCount == 2000)
                                {
                                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);

                                    if (hRgn)
                                    {
                                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                                        DeleteObject(h);
                                    }
                                    else
                                    {
                                        hRgn = h;
                                    }

                                    pData->rdh.nCount = 0;
                                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
                                }
                            }
                        }

                        // Go to next row (remember, the bitmap is inverted vertically)
                        p32 -= bm32.bmWidthBytes;
                    }

                    // Create or extend the region with the remaining rectangles
                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);

                    if (hRgn)
                    {
                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                        DeleteObject(h);
                    }
                    else
                    {
                        hRgn = h;
                    }

                    // Clean up
                    GlobalFree(hData);
                    SelectObject(hDC, holdBmp);
                    DeleteDC(hDC);
                }

                DeleteObject(SelectObject(hMemDC, holdBmp));
            }

            DeleteDC(hMemDC);
        }
    }

    return hRgn;
}
