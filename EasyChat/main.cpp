#include "mainwindow.h"
#include "logindialog.h"
#include "settingdlg.h"
#include "tools.h"

#include <QApplication>
#include <QMutex>

#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>

//用于捕获异常
static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    //And output crash information
    EXCEPTION_RECORD *record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16));
    QString errAddr(QString::number((size_t)record->ExceptionAddress, 16));
    QString errFlag(QString::number(record->ExceptionFlags, 16));
    QString errPara(QString::number(record->NumberParameters, 16));
    qDebug()<<"errCode: "<<errCode;
    qDebug()<<"errAddr: "<<errAddr;
    qDebug()<<"errFlag: "<<errFlag;
    qDebug()<<"errPara: "<<errPara;
    //Create the dump file
    QString currentTime = tools::GetInstance()->GetCurrentTime2();
    currentTime = currentTime.replace(" ", "-");
    QString crashFileName = "/" + currentTime + "-crash.dmp";
    qDebug() << "crashFileName:" << APPLICATION_DIR + crashFileName;
    HANDLE hDumpFile = CreateFile((LPCWSTR)QString(APPLICATION_DIR + crashFileName).utf16(),
             GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(hDumpFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
        CloseHandle(hDumpFile);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

//用日志文件保存打印信息，供定位
void outputMessage(QtMsgType type, const QMessageLogContext &/*context*/, const QString &msg) {
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch (type) {
    case QtDebugMsg: {
        text = "Debug:";
        break;
    }
    case QtWarningMsg: {
        text = "Warning:";
        break;
    }
    case QtCriticalMsg: {
        text = "Critical:";
        break;
    }
    case QtFatalMsg: {
        text = "Fatal:";
        break;
    }
    default: {
        text = "Info:";
        break;
    }
    }

    //QString fileInfo = QString("File(%1)  Line(%2):").arg(QString(context.file)).arg(context.line);
    QString curDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = QString("%1 %2 %3").arg(curTime).arg(text).arg(msg);

    if (!APPLICATION_DIR.isEmpty()) {
        QString logFile(APPLICATION_DIR + QString("/EasyChat_%1.log").arg(curDate));
        QFile file(logFile);
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream tStream(&file);
        tStream << message << "\r\n";
        file.flush();
        file.close();
    }
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //注册捕获异常的函数
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

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

    //注册日志模块
#ifdef QT_NO_DEBUG
    qInstallMessageHandler(outputMessage);
#else
    qDebug() << "DEBUG TYPE";
#endif
    MainWindow w;
    LoginDialog *dialog = new LoginDialog();
    while (dialog->exec() != QDialog::Accepted) {

    }
    w.show();

    return a.exec();
}
