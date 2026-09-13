#include <csignal>

#include <QApplication>
#include <QDir>
#include <QTranslator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLocalSocket>
#include <QLocalServer>
#include <QThread>

#include "3rdparty/RunGuard.hpp"
#include "main/NekoGui.hpp"

#include "ui/mainwindow_interface.h"

#ifdef Q_OS_WIN
#include "sys/windows/MiniDump.h"
#endif

void signal_handler(int signum) {
    if (qApp) {
        GetMainWindow()->on_commitDataRequest();
        qApp->exit();
    }
}

QTranslator* trans = nullptr;
QTranslator* trans_qt = nullptr;

void loadTranslate(const QString& locale) {
    if (trans != nullptr) {
        trans->deleteLater();
    }
    if (trans_qt != nullptr) {
        trans_qt->deleteLater();
    }

    trans = new QTranslator;
    trans_qt = new QTranslator;
    QLocale::setDefault(QLocale(locale));

    if (trans->load(":/translations/" + locale + ".qm")) {
        QCoreApplication::installTranslator(trans);
    }
    if (trans_qt->load(QApplication::applicationDirPath() + "/qtbase_" + locale + ".qm")) {
        QCoreApplication::installTranslator(trans_qt);
    }
}

#define LOCAL_SERVER_PREFIX "h-localserver-"

int main(int argc, char* argv[]) {

#ifdef Q_OS_WIN
    Windows_SetCrashHandler();
#endif


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
#endif
    QApplication::setQuitOnLastWindowClosed(false);
    auto preQApp = new QApplication(argc, argv);


    QDir::setCurrent(QApplication::applicationDirPath());
    if (QFile::exists("updater.old")) {
        QFile::remove("updater.old");
    }
#ifndef Q_OS_WIN
    if (!QFile::exists("updater")) {
        QFile::link("launcher", "updater");
    }
#endif


    NekoGui::dataStore->argv = QApplication::arguments();
    if (NekoGui::dataStore->argv.contains("-many")) NekoGui::dataStore->flag_many = true;
    if (NekoGui::dataStore->argv.contains("-appdata")) {
        NekoGui::dataStore->flag_use_appdata = true;
        int appdataIndex = NekoGui::dataStore->argv.indexOf("-appdata");
        if (NekoGui::dataStore->argv.size() > appdataIndex + 1 && !NekoGui::dataStore->argv.at(appdataIndex + 1).startsWith("-")) {
            NekoGui::dataStore->appdataDir = NekoGui::dataStore->argv.at(appdataIndex + 1);
        }
    }
    if (NekoGui::dataStore->argv.contains("-tray")) NekoGui::dataStore->flag_tray = true;
    if (NekoGui::dataStore->argv.contains("-debug")) NekoGui::dataStore->flag_debug = true;
    if (NekoGui::dataStore->argv.contains("-flag_restart_tun_on")) NekoGui::dataStore->flag_restart_tun_on = true;
    if (NekoGui::dataStore->argv.contains("-flag_reorder")) NekoGui::dataStore->flag_reorder = true;
#ifdef NKR_CPP_USE_APPDATA
    NekoGui::dataStore->flag_use_appdata = true;
#endif
#ifdef NKR_CPP_DEBUG
    NekoGui::dataStore->flag_debug = true;
#endif


    auto wd = QDir(QApplication::applicationDirPath());
    if (wd.exists("installed.mode")) NekoGui::dataStore->flag_use_appdata = true;
    if (NekoGui::dataStore->flag_use_appdata) {
        QApplication::setApplicationName("h");
        if (!NekoGui::dataStore->appdataDir.isEmpty()) {
            wd.setPath(NekoGui::dataStore->appdataDir);
        } else {
            wd.setPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        }
    }
    if (!wd.exists()) wd.mkpath(wd.absolutePath());
    if (!wd.exists("config")) wd.mkdir("config");
    QDir::setCurrent(wd.absoluteFilePath("config"));
    QDir("temp").removeRecursively();


    delete preQApp;
    QApplication a(argc, argv);


    DS_cores = new QThread;
    DS_cores->start();


    RunGuard guard("h" + wd.absolutePath());
    quint64 guard_data_in = GetRandomUint64();
    quint64 guard_data_out = 0;
    if (!NekoGui::dataStore->flag_many && !guard.tryToRun(&guard_data_in)) {

        if (guard.isAnotherRunning(&guard_data_out)) {

            QLocalSocket socket;
            socket.connectToServer(LOCAL_SERVER_PREFIX + Int2String(guard_data_out));
            qDebug() << socket.fullServerName();
            if (!socket.waitForConnected(500)) {
                qDebug() << "Failed to wake a running instance.";
                return 0;
            }
            qDebug() << "connected to local server, try to raise another program";
            return 0;
        }

        QMessageBox::warning(nullptr, "h.", "Another h. instance is already running. Use -many to force start.");
        return 0;
    }
    MF_release_runguard = [&] { guard.release(); };


#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    QIcon::setFallbackSearchPaths(QStringList{
        ":/neko",
        ":/icon",
    });
#endif


    if (QIcon::themeName().isEmpty()) {
        QIcon::setThemeName("breeze");
    }


    QDir dir;
    bool dir_success = true;
    if (!dir.exists("profiles")) {
        dir_success &= dir.mkdir("profiles");
    }
    if (!dir.exists("groups")) {
        dir_success &= dir.mkdir("groups");
    }
    if (!dir.exists(ROUTES_PREFIX_NAME)) {
        dir_success &= dir.mkdir(ROUTES_PREFIX_NAME);
    }
    if (!dir_success) {
        QMessageBox::warning(nullptr, "Error", "No permission to write " + dir.absolutePath());
        return 1;
    }


    switch (NekoGui::coreType) {
        case NekoGui::CoreType::SING_BOX:
            NekoGui::dataStore->fn = "groups/h.json";
            break;
        default:
            MessageBoxWarning("Error", "Unknown coreType.");
            return 0;
    }
    auto isLoaded = NekoGui::dataStore->Load();
    if (!isLoaded) {
        NekoGui::dataStore->Save();
    }


    if (NekoGui::dataStore->start_minimal) NekoGui::dataStore->flag_tray = true;


    NekoGui::dataStore->routing = std::make_unique<NekoGui::Routing>();
    NekoGui::dataStore->routing->fn = ROUTES_PREFIX + NekoGui::dataStore->active_routing;
    isLoaded = NekoGui::dataStore->routing->Load();
    if (!isLoaded) {
        NekoGui::dataStore->routing->Save();
    }


    QString locale;
    switch (NekoGui::dataStore->language) {
        case 1:
            break;
        case 2:
            locale = "zh_CN";
            break;
        case 3:
            locale = "fa_IR";
            break;
        case 4:
            locale = "ru_RU";
            break;
        default:
            locale = QLocale().name();
    }
    QGuiApplication::tr("QT_LAYOUT_DIRECTION");
    loadTranslate(locale);


    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);


    QLocalServer server;
    auto server_name = LOCAL_SERVER_PREFIX + Int2String(guard_data_in);
    QLocalServer::removeServer(server_name);
    server.listen(server_name);
    QObject::connect(&server, &QLocalServer::newConnection, &a, [&] {
        auto socket = server.nextPendingConnection();
        qDebug() << "nextPendingConnection:" << server_name << socket;
        socket->deleteLater();

        MW_dialog_message("", "Raise");
    });

    UI_InitMainWindow();
    return QApplication::exec();
}
