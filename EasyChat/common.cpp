#include "common.h"

#include <QProcess>

QString g_userID;
QString g_userName;
QString g_password;
QString g_loginTime;
QString g_toUserID;
QString g_toUserName;
QString g_serverHost;
QString g_serverPort;
QString g_Token;

QWebSocket g_WebSocket;

QString APPLICATION_DIR = "";

QString APPLICATION_IMAGE_DIR = "";

QString APPLICATION_TMPIMAGE_DIR = "";

void RestartApp() {
    QString exeFile = APPLICATION_DIR + "/EasyChat.exe";
    QProcess::startDetached(exeFile, QStringList());
}
