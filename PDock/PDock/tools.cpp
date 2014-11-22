// OpenLinkedFile is a "shortcut aware" file open utility.  It takes a named file (fileName)
// and attempts to open it (read only). If the named file cannot be opened as named, a check is
// made to determine if the named file is a shortcut (link) file. If the named file is a shortcut,
// an attempt is made to resolve the shortcut link and open the target (linked) file.
// The function returns TRUE if a file was successfully opened (the opened file can then be
// accessed via fileObject). If a shortcut was detected, resolved and opened, linkedFilePath
// contains the pathname of the target file that was actually opened.
//
// Arguments:
// fileObject: File object to contain opened file.
//             This object is updated if a file is successfully opened.
// fileName: Name of file to be opened. This may be a shortcut file.
// linkedFilePath: Name of target file if fileName is discovered to be a shortcut.
//                 This argument is updated if a shortcut link is successfully resolved.
// hWnd: Handle of window calling this function.
#include "stdafx.h"
#include "tools.h"

#include <tchar.h>
//#include <Windows.h>
#include <afx.h>
#include <windows.h>
#include <shellapi.h>
#include <commoncontrols.h>
#include <objbase.h>
#include <shlobj.h>
#include <shfolder.h>
#include <Winnls.h>

#include "global.h"



BOOL OpenLinkedFile(CFile &fileObject, CString fileName, CString &linkedFilePath, HWND hWnd)
{
    CFileException fileError;
    CFileStatus fileStatus;


    // Attempt to open the file as named
    if (!fileObject.Open(fileName, CFile::modeRead,
                         &fileError))
    {
        TRACE("File could not be opened: %s. Check for shortcut.\n", fileName);


        // The file, as named, couldn't be opened, so check for a shortcut with the same name
        fileName += ".lnk"; // Shortcut filename = filename.lnk

        if (CFile::GetStatus(fileName, fileStatus))
        {
            TRACE("Shortcut detected: %s\n", fileName);


            // A shortcut for the named file has been found.
            // The shortcut link must now be resolved to get the target file name.
            HRESULT hres;
            IShellLink *pShellLink;
            WIN32_FIND_DATA wfd;
            TCHAR targetFilePath[MAX_PATH];

            // Call CoCreateInstance to obtain the IShellLink
            // Interface pointer. This call fails if
            // CoInitialize is not called, so it is assumed that
            // CoInitialize has already been called.
            hres = CoCreateInstance
                   (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                    IID_IShellLink, (LPVOID *)&pShellLink);

            if (SUCCEEDED(hres))
            {
                // A pointer to the IShellLink interface has been obtained.
                // Now, we need a pointer to the IPersistFile interface.
                IPersistFile *pPersistFile;
                hres = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID *)&pPersistFile);

                if (SUCCEEDED(hres))
                {
                    // Pointers to both interfaces have been obtained.
                    // Convert the shortcut filename to a wide character string.
                    WCHAR wideFileName [MAX_PATH];
                    //MultiByteToWideChar (CP_ACP, 0, (LPCSTR)fileName, -1, wideFileName, MAX_PATH);
                    _tcscpy(wideFileName, (LPCTSTR)fileName);


                    // Load the shortcut file from disk.
                    hres = pPersistFile->Load(wideFileName, STGM_READ);

                    if (SUCCEEDED (hres))
                    {
                        // The shortcut file has been loaded. Resolve the shortcut link.
                        hres = pShellLink->Resolve(hWnd, SLR_NO_UI);

                        if
                        (SUCCEEDED(hres))
                        {
                            // The shortcut link has been successfully resolved.
                            // Get the path of the target file that the link points to.

                            hres = pShellLink->GetPath(targetFilePath, MAX_PATH, &wfd, SLGP_SHORTPATH);

                            if (SUCCEEDED(hres))
                            {
                                // The path of the target file has been obtained.
                                // Attempt to open the target (linked) file.
                                linkedFilePath = targetFilePath;

                                if (fileObject.Open(linkedFilePath,
                                                    CFile::modeRead, &fileError))
                                {
                                    // The named file was a shortcut. The shortcut link was successfully
                                    // resolved and the target file was successfully opened.

                                    TRACE("Shortcut detected, resolved and opened: %s->%s\n", fileName, linkedFilePath);
                                    return TRUE;


                                }
                            }
                        }
                    }

                    // Release the IPersistFile interface pointer.
                    pPersistFile->Release();
                }

                // Release the IShellLink interface pointer.
                pShellLink->Release();
            }
        }
        else
        {
            // Named file could not be opened and no shortcut file was detected.
            TRACE("File could not be opened: % s. No shortcut detected.\n", fileName);
            return FALSE;
        }
    }
    else
    {
        // The named file was not a shortcut and was successfully opened.
        return TRUE;
    }

    // The named file was a shortcut, but the shortcut was not successfully resolved or opened.
    TRACE("Shortcut detected, but was not successfully opened: %s->%s\n", fileName, linkedFilePath);
    return FALSE;
}



HBITMAP ConvertIconToBitmap(HICON hIcon)
{
    CBitmap bmpIcon;
    CDC dcBmp;

    if (dcBmp.CreateCompatibleDC(NULL)) // create memory DC
    {
        if (bmpIcon.CreateCompatibleBitmap(&dcBmp, 16, 16))// 16x16 icon
        {
            CBitmap *poldBmp = dcBmp.SelectObject(&bmpIcon);
            dcBmp.DrawIcon(0, 0, hIcon); // Draw icon (hIcon should be loaded already)
            dcBmp.SelectObject(poldBmp); // Restore DC
        }
    }

    return (HBITMAP)bmpIcon;
}



// =======================================

MYICON_INFO MyGetIconInfo(HICON hIcon)
{
    MYICON_INFO myinfo;
    ZeroMemory(&myinfo, sizeof(myinfo));

    ICONINFO info;
    ZeroMemory(&info, sizeof(info));

    BOOL bRes = FALSE;

    bRes = GetIconInfo(hIcon, &info);

    if (!bRes)
    {
        return myinfo;
    }

    BITMAP bmp;
    ZeroMemory(&bmp, sizeof(bmp));

    if (info.hbmColor)
    {
        const int nWrittenBytes = GetObject(info.hbmColor, sizeof(bmp), &bmp);

        if (nWrittenBytes > 0)
        {
            myinfo.nWidth = bmp.bmWidth;
            myinfo.nHeight = bmp.bmHeight;
            myinfo.nBitsPerPixel = bmp.bmBitsPixel;
        }
    }
    else if (info.hbmMask)
    {
        // Icon has no color plane, image data stored in mask
        const int nWrittenBytes = GetObject(info.hbmMask, sizeof(bmp), &bmp);

        if (nWrittenBytes > 0)
        {
            myinfo.nWidth = bmp.bmWidth;
            myinfo.nHeight = bmp.bmHeight / 2;
            myinfo.nBitsPerPixel = 1;
        }
    }

    if (info.hbmColor)
    {
        DeleteObject(info.hbmColor);
    }

    if (info.hbmMask)
    {
        DeleteObject(info.hbmMask);
    }

    return myinfo;
}


HICON GetHighResolutionIcon(LPTSTR pszPath)
{
    // Get the image list index of the icon
    SHFILEINFO sfi;

    if (!SHGetFileInfo(pszPath, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_LARGEICON))
    {
        return NULL;
    }

    // Get the jumbo image list
    IImageList *piml;

    if (DEFAULT_ICOH_SIZE_WIDHT == 32)
    {
        if (FAILED(SHGetImageList(SHIL_LARGE, IID_IImageList, (void **)(&piml))))
        {
            return NULL;
        }
    }
    else  // 48
    {
        if (FAILED(SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void **)(&piml))))
        {
            return NULL;
        }
    }


    // Extract an icon
    HICON hico;
    piml->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hico);

    int w, h;
    piml->GetIconSize(&w, &h);

    // Clean up
    piml->Release();

    // Return the icon
    return hico;
}


