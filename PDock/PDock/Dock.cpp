// PDock.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Dock.h"
#include "DockDlg.h"

#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
using namespace Gdiplus;


#include "LimitSingleInstance.h"
CLimitSingleInstance g_SingleInstanceObj(TEXT("Global\\{2194ABA1-BFFA-4e6b-8C26-D191BB16F9E6}"));

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CXbuttonTestApp

BEGIN_MESSAGE_MAP(CDockApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CXbuttonTestApp construction

CDockApp::CDockApp()
{
}


// The one and only CXbuttonTestApp object

CDockApp theApp;


// CXbuttonTestApp initialization

BOOL CDockApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));


    // single instance check
    if (g_SingleInstanceObj.IsAnotherInstanceRunning())
    {
        return FALSE;
    }

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    CDockDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    GdiplusShutdown(gdiplusToken);


    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
