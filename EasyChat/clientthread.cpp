#include "clientthread.h"
#include "tools.h"

#include <QFile>

ClientThread::ClientThread(QObject *parent) :
    QThread (parent),
    m_pClient(nullptr)
{

}

//ClientThread::ClientThread(QString fullFilePath, QString userName, QObject *parent) :
//    QThread (parent),
//    m_pClient(nullptr)
//{
//    m_strFullFilePath = fullFilePath;
//    m_strUserName = userName;
//}

ClientThread::ClientThread(NetworkParams &params, QObject *parent) :
    QThread (parent),
    m_pClient(nullptr)
{
    m_networkParams = params;
    m_strFullFilePath = params.filePath;
    m_strUserName = params.userName;
}

ClientThread::~ClientThread() {
    if (m_pClient != nullptr) {
        delete m_pClient;
        m_pClient = nullptr;
    }
}

void ClientThread::run() {
    if (m_pClient == nullptr) {
        m_pClient = new Client(this);
    }
    if (m_networkParams.httpRequestType == REQUEST_UPLOAD_FILE_BY_TCP) {
        if (m_pClient->Connect()) {
            qDebug() << __FUNCTION__ << " ready send file with params";
            m_pClient->SendFileWithParams(m_networkParams);
        }
    } else if (REQUEST_DOWNLOAD_FILE_BY_TCP == m_networkParams.httpRequestType) {
        if (m_pClient->Connect()) {
            qDebug() << __FUNCTION__ << " ready download file with params";
            m_pClient->DownloadFileWithParams(m_networkParams);
        }
    } else if (REQUEST_GET_UPLOAD_FILES == m_networkParams.httpRequestType) {

    }

    qDebug() << "client thread delete later";
    this->deleteLater();
}

void ClientThread::UpdateProgress(NetworkParams &params) {
    emit updateProgressSignal(params.recved, params.totalSize);
}

