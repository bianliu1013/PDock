#pragma once


class Preference
{
public:
    Preference(void);
    ~Preference(void);

    static void Init();
    static void Load();
    static void Save();
    static void UpdateSetting();

    static int startup;
    static int alwaysShow;
    static int alert_before_delete;
};

