#include "stdafx.h"
#include "Preference.h"

#include <Windows.h>
#include <Shlwapi.h>

#include "global.h"


int Preference::startup                 = 0;
int Preference::alwaysShow              = 0;
int Preference::alert_before_delete     = 1;

Preference::Preference(void)
{
}


Preference::~Preference(void)
{
}

void Preference::Init()
{
    startup = 0;
    alwaysShow = 0;
    alert_before_delete = 1;
    Load();
}

void Preference::Load()
{
    // open .ini file
    startup             = GetPrivateProfileInt(L"General", L"StartupWithSystem", 0, CONFIG_INI_FILE);
    alwaysShow          = GetPrivateProfileInt(L"General", L"AlwaysShow",        0, CONFIG_INI_FILE);
    alert_before_delete = GetPrivateProfileInt(L"General", L"AlertBeforeDelete", 1, CONFIG_INI_FILE);
}


void Preference::Save()
{
    WritePrivateProfileString(L"General", 
        L"StartupWithSystem", 
        startup? _T("1"): _T("0"),
        CONFIG_INI_FILE);
    WritePrivateProfileString(L"General", 
        L"AlwaysShow", 
        alwaysShow? _T("1"): _T("0"),
        CONFIG_INI_FILE);
    WritePrivateProfileString(L"General", 
        L"AlertBeforeDelete", 
        alert_before_delete? _T("1"): _T("0"),
        CONFIG_INI_FILE);
}


void Preference::UpdateSetting()
{
    Save();
}
