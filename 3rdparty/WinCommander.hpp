





















#ifndef WINCOMMANDER_H
#define WINCOMMANDER_H

#include <QString>
#include <QStringList>

class WinCommander {
public:
    static const int SW_HIDE = 0;
    static const int SW_NORMAL = 1;
    static const int SW_SHOWMINIMIZED = 2;

    static uint runProcessElevated(const QString &path,
                                   const QStringList &parameters = QStringList(),
                                   const QString &workingDir = QString(),
                                   int nShow = SW_SHOWMINIMIZED, bool aWait = true);
};

#endif
