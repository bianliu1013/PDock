#pragma once



class CDragDialogEx : public CDialog
{
    DECLARE_DYNAMIC(CDragDialogEx)

public:
    CDragDialogEx(CWnd *pParent, HICON icon);   // standard constructor
    virtual ~CDragDialogEx();

    HBITMAP ConvertIconToBitmap(HICON hIcon);
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);

    // Dialog Data
    enum { IDD = IDD_DIALOG_DRAG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    afx_msg void OnPaint();

private:
    void CreateRegion(HWND hWnd);
    void SetTransparency( float transparencyRatio );


    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);

    //CBitmapPicture* m_pPicture;
    CStatic *m_pStatic;
    HICON m_hIcon;

    BITMAP m_Bitmap;
    HBITMAP m_hBitmap;
};
