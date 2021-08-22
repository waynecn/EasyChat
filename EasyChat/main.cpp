#include "mainwindow.h"
#include "logindialog.h"
#include "settingdlg.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("easychat");
    QCoreApplication::setOrganizationDomain("easychat.easychat.com");
    QCoreApplication::setApplicationName("easychat");

    APPLICATION_DIR = QCoreApplication::applicationDirPath();

    QSettings settings;
    QString host = settings.value(SETTING_CURRENT_SERVER_HOST, "").toString();
    if (host.isEmpty()) {
        if (settings.value(SETTING_WEBSOCKET_SERVER_HOST, "").toString().contains(",")) {
            host = "";
        } else {
            host = settings.value(SETTING_WEBSOCKET_SERVER_HOST, "").toString();
        }
    }
    QString port = settings.value(SETTING_SERVER_PORT).toString();
    if (host.isEmpty() || port.isEmpty()) {
        SettingDlg *dlg = SettingDlg::GetInstance();
        while (dlg->exec() != QDialog::Accepted) {

        }
    }
    g_serverHost = host;
    g_serverPort = port;

    MainWindow w;
    LoginDialog *dialog = new LoginDialog();
    while (dialog->exec() != QDialog::Accepted) {

    }
    w.show();

    return a.exec();
}
