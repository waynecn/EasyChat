#include "mynetworkcontroller.h"
#include "tools.h"
#include "clientthread.h"

#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextCodec>
#include <QSettings>
#include <QProcess>
#include <QApplication>
#include <QHttpMultiPart>
#include <QtMath>
#include <QHttpPart>

MyNetworkController::MyNetworkController() : QNetworkAccessManager(),
    m_pFile(nullptr)
{

}

MyNetworkController::MyNetworkController(NetworkParams &params) : QNetworkAccessManager(),
    m_pFile(nullptr)
{
    m_networkParams = params;
}

MyNetworkController::~MyNetworkController() {

}

void MyNetworkController::SetNetworkParams(NetworkParams &params) {
    m_networkParams = params;
}

void MyNetworkController::StartWork() {
    if (g_serverHost.isEmpty() || g_serverPort.isEmpty()) {
        QSettings settings;
        g_serverHost = settings.value(SETTING_CURRENT_SERVER_HOST).toString();
        g_serverPort = settings.value(SETTING_SERVER_PORT).toString();
    }
    if (m_networkParams.httpRequestType == REQUEST_UPLOAD_FILE) {
        uploadFile();
    } else if (m_networkParams.httpRequestType == REQUEST_GET_UPLOAD_FILES) {
        getUploadFiles();
    } else if (m_networkParams.httpRequestType == REQUEST_DOWNLOAD_FILE) {
        downloadFile();
    } else if (REQUEST_LOGIN == m_networkParams.httpRequestType) {
        login();
    } else if (REQUEST_REGISTER == m_networkParams.httpRequestType) {
        registerUser();
    } else if (REQUEST_DELETE_FILE == m_networkParams.httpRequestType) {
        deleteFile();
    } else if (REQUEST_UPLOAD_CLIENT == m_networkParams.httpRequestType) {
        uploadClient();
    } else if (REQUEST_UPLOAD_FILE_BY_TCP == m_networkParams.httpRequestType) {
        uploadFileByTCP();
    } else if (REQUEST_DOWNLOAD_FILE_BY_TCP == m_networkParams.httpRequestType) {
        downloadFileByTCP();
    }
}

void MyNetworkController::uploadFile() {
    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    m_pFile = new QFile(m_networkParams.filePath);
    m_pFile->open(QIODevice::ReadOnly);

    QString requestUrl = "http://" + g_serverHost + ":" + g_serverPort + "/uploads2";
    QHttpMultiPart *pMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"file\";filename=\"" + m_networkParams.fileName + "\";")));
    filePart.setBodyDevice(m_pFile);
    pMultiPart->append(filePart);

    QUrl url(requestUrl);
    QNetworkRequest req(url);
    req.setRawHeader("UserName", g_userName.toUtf8());
    QNetworkReply *reply = pNetworkAccessManager->post(req, pMultiPart);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(onRequestProgress(qint64, qint64)));
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    m_tStart = QTime::currentTime();
}

void MyNetworkController::getUploadFiles() {
    QString fileListUrl = "http://" + g_serverHost + ":" + g_serverPort + "/uploadfiles2";
    QUrl url(fileListUrl);
    QNetworkRequest req(url);
    req.setRawHeader("token", "20200101");
    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    pNetworkAccessManager->get(req);
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
}

void MyNetworkController::downloadFile() {
    QString strUrl = "http://" + g_serverHost + ":" + g_serverPort + "/uploads/" + m_networkParams.fileName;
    QUrl url(strUrl);
    QNetworkRequest req(url);
    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    QNetworkReply *reply = pNetworkAccessManager->get(req);
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onRequestProgress(qint64, qint64)));
    m_tStart = QTime::currentTime();
}

void MyNetworkController::login() {
    QUrl url(QString("http://%1:%2/loginnew").arg(g_serverHost).arg(g_serverPort));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString loginInfo = "{\"username\":\"" + m_networkParams.userName + "\", \"password\":\"" + m_networkParams.password + "\",\"clientversion\":\"" + APPLICATION_VERSION + "\"}";
    QByteArray bData = codec->fromUnicode(loginInfo);

    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    pNetworkAccessManager->post(req, bData);
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    m_tStart = QTime::currentTime();
}

void MyNetworkController::registerUser() {
    QUrl url(QString("http://%1:%2/register").arg(g_serverHost).arg(g_serverPort));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("token", "20200101");
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString loginInfo = "{\"username\":\"" + m_networkParams.userName + "\", \"password\":\"" + m_networkParams.password + "\", \"mobile\":\"" + m_networkParams.phoneNumber + "\"}";
    QByteArray bData = codec->fromUnicode(loginInfo);

    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    pNetworkAccessManager->post(req, bData);
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    m_tStart = QTime::currentTime();
}

void MyNetworkController::deleteFile() {
    QString deleteFileUrl = "http://" + g_serverHost + ":" + g_serverPort + "/delfile2";

    QUrl url(deleteFileUrl);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("token", "20200101");

    QString strData = "{\"fileName\":\"" + m_networkParams.fileName + "\",\"userName\":\"" + m_networkParams.userName + "\"}";
    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    pNetworkAccessManager->post(req, strData.toLocal8Bit());
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
}

void MyNetworkController::uploadClient() {
    int versionStartIndex = m_networkParams.filePath.lastIndexOf('-');
    if (versionStartIndex == -1) {
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText("文件名不符合要求");
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    int versionEndIndex = m_networkParams.filePath.lastIndexOf('.');
    if (versionEndIndex == -1 || versionEndIndex < versionStartIndex) {
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText("文件名不符合要求");
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }

    QString versionStr = m_networkParams.filePath.mid(versionStartIndex + 1, versionEndIndex - versionStartIndex - 1);
    if (versionStr.length() < 1) {
        QMessageBox box;
        box.setWindowTitle("提示");
        box.setText("文件名未包含版本号");
        box.addButton("确定", QMessageBox::AcceptRole);
        box.exec();
        return;
    }
    QStringList versions = versionStr.split(".");
    int versionNumber = 0;
    for (int i = 0; i < versions.length(); ++i) {
        versionNumber += versions[i].toInt() * qPow(10, versions.length() - i - 1);
    }

    m_pFile = new QFile(m_networkParams.filePath);
    m_pFile->open(QIODevice::ReadOnly);

    QString uploadUrl = "http://" + g_serverHost + ":" + g_serverPort + "/uploadClient";

    QHttpMultiPart *pMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"file\";filename=\"" + m_networkParams.fileName + "\";")));
    filePart.setBodyDevice(m_pFile);

    QHttpPart versionPart;
    versionPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    versionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"versionStr\"")));
    versionPart.setBody(versionStr.toUtf8());

    QHttpPart versionNumberPart;
    versionNumberPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    versionNumberPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"versionNumber\"")));
    versionNumberPart.setBody(QString("%1").arg(versionNumber).toUtf8());

    QHttpPart userNamePart;
    userNamePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    userNamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"userName\"")));
    userNamePart.setBody(g_userName.toUtf8());

    QHttpPart md5Part;
    QString md5Value = tools::GetInstance()->FileMD5(m_networkParams.filePath);
    md5Part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    md5Part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"MD5\"")));
    md5Part.setBody(md5Value.toUtf8());

    pMultiPart->append(versionPart);
    pMultiPart->append(versionNumberPart);
    pMultiPart->append(userNamePart);
    pMultiPart->append(md5Part);
    pMultiPart->append(filePart);

    QUrl url(uploadUrl);
    QNetworkRequest req(url);
    req.setRawHeader("UserName", g_userName.toUtf8());
    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    QNetworkReply *reply = pNetworkAccessManager->post(req, pMultiPart);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(onRequestProgress(qint64, qint64)));
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    m_tStart = QTime::currentTime();
}

void MyNetworkController::uploadFileByTCP() {
    m_tStart = QTime::currentTime();
    QFile f(m_networkParams.filePath);
    f.open(QIODevice::ReadOnly);
    qint64 fileSize = f.size();
    m_networkParams.totalSize = fileSize;
    ClientThread *thread = new ClientThread(m_networkParams);
    connect(thread, SIGNAL(updateProgressSignal(qint64, qint64)), this, SLOT(onRequestProgress(qint64, qint64)));
    thread->start();
}

void MyNetworkController::downloadFileByTCP() {
    m_tStart = QTime::currentTime();
    ClientThread *thread = new ClientThread(m_networkParams);
    connect(thread, SIGNAL(updateProgressSignal(qint64, qint64)), this, SLOT(onRequestProgress(qint64, qint64)));
    thread->start();
}

void MyNetworkController::upLoadError(QNetworkReply::NetworkError err) {
    qDebug() << "request:" << m_networkParams.httpRequestType << " upload file:" << m_networkParams.fileName << " error. err:" << err;
}

void MyNetworkController::onRequestProgress(qint64 recved, qint64 total) {
    m_networkParams.recved = recved;
    if (m_networkParams.totalSize <= 0) {
        m_networkParams.totalSize = total;
    }
    QTime curTime = QTime::currentTime();
    int msecTo = m_tStart.msecsTo(curTime);
    //计算下载剩余的内容所需的时间
    qint64 timeLeft;
    if (recved == 0 || total == 0) {
        timeLeft = 0;
    } else {
        timeLeft = qAbs((total - recved) * msecTo / recved / 1000);
    }
    m_networkParams.timeLeft = timeLeft;

    //计算下载速度
    qint64 downloadSpeed = 0;
    if (msecTo == 0) {
        downloadSpeed = 0;
    } else {
        downloadSpeed = qAbs(recved / (msecTo * 1024 / 1000));
    }
    m_networkParams.speed = downloadSpeed;
    if (recved == total) {
        m_networkParams.requestEndTime = tools::GetInstance()->GetCurrentTime2();
    }
    if (m_networkParams.httpRequestType == REQUEST_UPLOAD_FILE || REQUEST_UPLOAD_CLIENT == m_networkParams.httpRequestType ||
            m_networkParams.httpRequestType == REQUEST_DOWNLOAD_FILE || m_networkParams.httpRequestType == REQUEST_DOWNLOAD_CLIENT ||
            m_networkParams.httpRequestType == REQUEST_UPLOAD_FILE_BY_TCP || REQUEST_DOWNLOAD_FILE_BY_TCP == m_networkParams.httpRequestType) {
        emit updateRequestProcess(m_networkParams);
    }
    if (REQUEST_UPLOAD_FILE_BY_TCP == m_networkParams.httpRequestType && !m_networkParams.msgSend &&
            m_networkParams.recved > 0 && m_networkParams.recved == m_networkParams.totalSize) {
        m_networkParams.msgSend = true;
        emit requestFinished(m_networkParams);
    }
}

void MyNetworkController::onReplyFinished(QNetworkReply *reply) {
    qDebug() << "reply finished:" << m_networkParams.httpRequestType << " " << m_networkParams.fileName;
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() == QNetworkReply::NoError && statusCode == 200) {
        if (m_networkParams.httpRequestType == REQUEST_UPLOAD_FILE) {
            emit requestFinished(m_networkParams);
            if (nullptr != m_pFile) {
                m_pFile->close();
                delete m_pFile;
                m_pFile = nullptr;
            }
            m_networkParams.requestEndTime = tools::GetInstance()->GetCurrentTime2();
            emit updateRequestProcess(m_networkParams);
        } else if (m_networkParams.httpRequestType == REQUEST_GET_UPLOAD_FILES) {
            QByteArray baData = reply->readAll();
            QJsonParseError jsonErr;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(baData, &jsonErr);
            if (jsonErr.error != QJsonParseError::NoError) {
                QString msg = "解析响应数据发生错误";
                QMessageBox box;
                box.setWindowTitle("EasyChat");
                box.setText(msg);
                box.addButton("确定", QMessageBox::AcceptRole);
                box.exec();
                reply->deleteLater();
                return;
            }
            bool bRet = jsonDoc["Success"].toBool();
            if (!bRet) {
                QString msg = "解析响应数据发生错误";
                QMessageBox box;
                box.setWindowTitle("EasyChat");
                box.setText(msg);
                box.addButton("确定", QMessageBox::AcceptRole);
                box.exec();
                reply->deleteLater();
                return;
            }
            QJsonArray files = jsonDoc["Files"].toArray();
            emit fileListRequestFinished(files);
        } else if (REQUEST_DOWNLOAD_FILE == m_networkParams.httpRequestType) {
            if (m_networkParams.saveFileDir.isEmpty()) {
                reply->deleteLater();
                return;
            }
            QFile f(m_networkParams.saveFileDir + m_networkParams.saveFileName);
            if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                f.write(reply->readAll());
            }
            f.close();
            qDebug() << "文件下载完毕:" << m_networkParams.saveFileDir + m_networkParams.saveFileName;
            m_networkParams.requestEndTime = tools::GetInstance()->GetCurrentTime2();
            emit updateRequestProcess(m_networkParams);
        } else if (m_networkParams.httpRequestType == REQUEST_LOGIN) {
            QByteArray baData = reply->readAll();
            QJsonParseError jsonErr;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(baData, &jsonErr);
            if (jsonErr.error != QJsonParseError::NoError) {
                qDebug() << "response:" << baData;
                QString msg = "解析响应数据发生错误:" + jsonErr.errorString();
                qDebug() << msg;
                emit loginFailed(msg);
                reply->deleteLater();
                return;
            }

            qDebug() << "jsonDoc:" << jsonDoc;
            Q_ASSERT(jsonDoc.isObject());
            bool bRet = jsonDoc["Success"].toBool();
            if (!bRet) {
                QString msg = "登陆失败：" + jsonDoc["Msg"].toString();
                qDebug() << msg;
                emit loginFailed(msg);
                reply->deleteLater();
                return;
            }
            g_userName = jsonDoc["Username"].toString();
            g_userID = QString::number(jsonDoc["Id"].toInt());
            g_loginTime = tools::GetInstance()->GetCurrentTime2();
            emit loginSuccess();
            g_WebSocket.sendTextMessage(tools::GetInstance()->GenerateOnlineUserMessage());
            if (jsonDoc["NewClient"].toObject()["Flag"].toBool()) {
                QMessageBox box;
                box.setWindowTitle("提示");
                box.setText("发现新版本客户端，是否更新");
                box.addButton("确定", QMessageBox::AcceptRole);
                box.addButton("取消", QMessageBox::RejectRole);
                QString clientFileName = jsonDoc["NewClient"].toObject()["FileName"].toString();
                qDebug() << "client:" << clientFileName;
                m_downloadNewClientParams.paramID = tools::GetInstance()->GenerateRandomID();
                m_downloadNewClientParams.httpRequestType = REQUEST_DOWNLOAD_CLIENT;
                m_downloadNewClientParams.fileName = clientFileName;
                m_downloadNewClientParams.requestTime = tools::GetInstance()->GetCurrentTime2();
                QSettings settings;
                QString saveFileDir = settings.value(SETTING_SAVE_FILE_DIR, "C:/").toString();
                m_downloadNewClientParams.saveFileDir = saveFileDir;
                m_downloadNewClientParams.saveFileName = clientFileName;
                connect(&box, SIGNAL(accepted()), this, SLOT(downloadNewClient()));
                box.exec();
            }
        } else if (REQUEST_REGISTER == m_networkParams.httpRequestType) {
            QByteArray baData = reply->readAll();
            QJsonParseError jsonErr;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(baData, &jsonErr);
            if (jsonErr.error != QJsonParseError::NoError) {
                QString msg = "解析rsponse数据发生错误:" + jsonErr.errorString();
                qDebug() << msg;
                emit registerFailed(msg);
                reply->deleteLater();
                return;
            }

            Q_ASSERT(jsonDoc.isObject());
            bool bRet = jsonDoc["Success"].toBool();
            if (!bRet) {
                QString msg = jsonDoc["Msg"].toString();
                qDebug() << msg;
                emit registerFailed(msg);
                reply->deleteLater();
                return;
            }

            emit registerSuccess();
        } else if (REQUEST_DELETE_FILE == m_networkParams.httpRequestType) {
            qDebug() << "file:" << m_networkParams.fileName << " has been deleted.";
            emit deleteFileSuccess();
        } else if (REQUEST_DOWNLOAD_CLIENT == m_networkParams.httpRequestType) {
            if (m_networkParams.saveFileDir.isEmpty()) {
                reply->deleteLater();
                return;
            }
            QFile f(m_networkParams.saveFileDir + m_networkParams.saveFileName);
            if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                f.write(reply->readAll());
            }
            f.close();
            qDebug() << "新的客户端下载完毕:" << m_networkParams.saveFileDir + m_networkParams.saveFileName;

            QMessageBox box;
            box.setWindowTitle("提示");
            box.setText("客户端下载完毕，是否现在安装？");
            box.addButton("确定", QMessageBox::AcceptRole);
            box.addButton("取消", QMessageBox::RejectRole);
            connect(&box, SIGNAL(accepted()), this, SLOT(onInstallClient()));
            box.exec();
        } else if (REQUEST_UPLOAD_CLIENT == m_networkParams.httpRequestType) {
            qDebug() << "上传客户端:" << m_networkParams.fileName << " 成功";
            if (nullptr != m_pFile) {
                m_pFile->close();
                delete m_pFile;
                m_pFile = nullptr;
            }
            m_networkParams.requestEndTime = tools::GetInstance()->GetCurrentTime2();
            emit updateRequestProcess(m_networkParams);
            emit uploadClientSuccess();
        }
    } else {
        QString msg = "http request failed:" + reply->errorString();
        qDebug() << msg;
        if (m_networkParams.httpRequestType == REQUEST_LOGIN) {
            emit loginFailed(msg);
        } else if (REQUEST_DELETE_FILE == m_networkParams.httpRequestType) {
            emit deleteFileFailed(m_networkParams, msg);
        } else if (REQUEST_UPLOAD_FILE == m_networkParams.httpRequestType) {
            emit uploadFileFailed(m_networkParams, msg);
            if (nullptr != m_pFile) {
                m_pFile->close();
                delete m_pFile;
                m_pFile = nullptr;
            }
        } else if (REQUEST_DOWNLOAD_FILE == m_networkParams.httpRequestType) {
            emit downloadFileFailed(m_networkParams, msg);
        } else if (REQUEST_UPLOAD_CLIENT == m_networkParams.httpRequestType) {
            emit uploadClientFailed(m_networkParams, msg);
        }
    }
    reply->deleteLater();
}

void MyNetworkController::downloadNewClient() {
    m_networkParams = m_downloadNewClientParams;
    QString strUrl = "http://" + g_serverHost + ":" + g_serverPort + "/clients/" + m_downloadNewClientParams.fileName;
    QUrl url(strUrl);
    QNetworkRequest req(url);
    QNetworkAccessManager *pNetworkAccessManager = new QNetworkAccessManager();
    QNetworkReply *reply = pNetworkAccessManager->get(req);
    connect(pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onRequestProgress(qint64, qint64)));
    m_tStart = QTime::currentTime();
    emit updateRequestProcess(m_downloadNewClientParams);
}

void MyNetworkController::onInstallClient() {
    QProcess::startDetached(m_networkParams.saveFileDir + m_networkParams.saveFileName, QStringList());
    QApplication::exit(-1);
}

