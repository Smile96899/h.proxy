#pragma once

class ThemeManager {
public:
    QString system_style_name = "";
    QString current_theme = "0";

    void ApplyTheme(const QString &theme);
};

extern ThemeManager *themeManager;
