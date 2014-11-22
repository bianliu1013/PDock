struct MYICON_INFO
{
    int     nWidth;
    int     nHeight;
    int     nBitsPerPixel;
};

MYICON_INFO MyGetIconInfo(HICON hIcon);

BOOL OpenLinkedFile(CFile &fileObject, CString fileName, CString &linkedFilePath, HWND hWnd);

HICON GetHighResolutionIcon(LPTSTR pszPath);

