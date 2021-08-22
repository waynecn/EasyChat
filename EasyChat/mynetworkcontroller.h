#ifndef MYNETWORKCONTROLLER_H
#define MYNETWORKCONTROLLER_H

#include "common.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class MyNetworkController : public QNetworkAccessManager
{
    Q_OBJECT

public:
    MyNetworkController();
    ~MyNetworkController();
    MyNetworkController(NetworkParams &params);
    void SetNetworkParams(NetworkParams &params);
    void StartWork();

private:
    void uploadFile();
    void getUploadFiles();
    void downloadFile();
    void login();
    void registerUser();
    void deleteFile();
    void uploadClient();

private:
    NetworkParams m_networkParams;
    QTime                   m_tStart;
    QFile                   *m_pFile;
    NetworkParams m_downloadNewClientParams;

private slots:
    void upLoadError(QNetworkReply::NetworkError err);
    void onRequestProgress(qint64 recved, qint64 total);
    void onReplyFinished(QNetworkReply*reply);
    void downloadNewClient();
    void onInstallClient();

signals:
    void requestFinished(NetworkParams &params);
    void fileListRequestFinished(NetworkParams &params, QJsonArray &array);
    void updateRequestProcess(NetworkParams &params);
    void loginFailed(QString &msg);
    void loginSuccess();
    void registerFailed(QString &msg);
    void registerSuccess();
    void deleteFileSuccess();
    void deleteFileFailed(NetworkParams &params, QString &msg);
    void uploadFileFailed(NetworkParams &params, QString &msg);
    void downloadFileFailed(NetworkParams &params, QString &msg);
    void uploadClientSuccess();
    void uploadClientFailed(NetworkParams &params, QString &msg);
};

#endif // MYNETWORKCONTROLLER_H
