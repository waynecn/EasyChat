#include "client.h"
#include "tools.h"
#include "clientthread.h"

#include <winsock2.h>
#include <QSettings>
#include <QFile>

Client::Client()
{
    WSADATA wsadata;
    WSAStartup(0x202, &wsadata);

    m_bSocketClosed = true;
    m_bIsStoped = false;
}

Client::Client(QObject *parent) {
    m_pParent = parent;
}

Client::~Client() {
    if (!m_bSocketClosed) {
        closesocket(m_iClientSocket);
    }
}

bool Client::Connect() {
    QSettings settings;
    QString remoteHost = settings.value(SETTING_CURRENT_SERVER_HOST, "127.0.0.1").toString();
    m_hostnm = gethostbyname(remoteHost.toStdString().c_str());

    if (m_hostnm == (struct hostent*)0) {
        qDebug() << "gethostbyname failed.";
        return false;
    }

    if ((m_iClientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        qDebug() << "failed to create client socket.";
        return false;
    }
    int tcpPort = settings.value(SETTING_SERVER_PORT).toInt() + 1;
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(tcpPort);
    m_sockAddr.sin_addr.s_addr = *((unsigned long *)m_hostnm->h_addr);

    if (connect(m_iClientSocket, (struct sockaddr *)&m_sockAddr, sizeof(m_sockAddr)) < 0) {
        qDebug() << "Client connect to socket server failed.";
        return false;
    }

    m_bSocketClosed = false;
    m_bIsStoped = false;

    qDebug() << "client connect success.";
    return true;
}

//bool Client::SendFile(QString strFullFilePath, QString strUserName) {
//    if (strFullFilePath.isEmpty() || strUserName.isEmpty()) {
//        qDebug() << "file path or user name is empty.";
//        return false;
//    }
//    strFullFilePath = strFullFilePath.replace("\\", "/");

//    QString fileName = strFullFilePath.mid(strFullFilePath.lastIndexOf("/") + 1);
//    QFile f(strFullFilePath);
//    if (!f.exists()) {
//        qDebug() << "file:" << strFullFilePath << " doesn't exists.";
//        return false;
//    }
//    if (!f.open(QIODevice::ReadOnly)) {
//        qDebug() << "file:" << strFullFilePath << " failed to open.";
//        return false;
//    }
//    f.seek(SEEK_SET);
//    qint64 fileSize = f.size();

//    char chBuff[BUFF_SIZE];
//    memset(chBuff, 0, BUFF_SIZE);

//    memset(chBuff, 0, BUFF_SIZE);
//    QString s = QString("upload|%1|%2|%3|donotremove").arg(fileName).arg(fileSize).arg(strUserName);
//    strcpy(chBuff, s.toStdString().c_str());
//    send(m_iClientSocket, chBuff, strlen(chBuff) + 1, 0);

//    qint64 sendSize = 0;

//    while (sendSize < fileSize) {
//        memset(chBuff, 0, BUFF_SIZE);
//        qint64 iRead = f.read(chBuff, BUFF_SIZE);
//        if (iRead < 0) {
//            qDebug() << "Server read file failed.";
//            f.close();
//            return false;
//        }

//        int iSend = send(m_iClientSocket, chBuff, iRead, 0);
//        if (iSend < 0) {
//            qDebug() << "Server send file content to client failed.";
//            f.close();
//            return false;
//        }
//        int temp = iRead;
//        while (iSend < temp) {
//            iSend = send(m_iClientSocket, chBuff + iSend, temp - iSend, 0);
//            if (iSend < 0) {
//                f.close();
//                return false;
//            }
//            temp = temp - iSend;
//        }

//        sendSize += iSend;
//        qDebug() << "fileSize:" << fileSize << " sendSize:" << sendSize << " iSend:" << iSend;
//        f.seek(sendSize);
//    }

//    f.close();
//    closesocket(m_iClientSocket);
//    m_bSocketClosed = true;
//}

bool Client::SendFileWithParams(NetworkParams &params) {
    QString strFullFilePath = params.filePath;
    QString strUserName = params.userName;
    if (strFullFilePath.isEmpty() || strUserName.isEmpty()) {
        qDebug() << "file path or user name is empty.";
        return false;
    }
    strFullFilePath = strFullFilePath.replace("\\", "/");

    QString fileName = strFullFilePath.mid(strFullFilePath.lastIndexOf("/") + 1);
    QFile f(strFullFilePath);
    if (!f.exists()) {
        qDebug() << "file:" << strFullFilePath << " doesn't exists.";
        return false;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "file:" << strFullFilePath << " failed to open.";
        return false;
    }
    f.seek(SEEK_SET);
    qint64 fileSize = f.size();
    params.totalSize = fileSize;

    char chBuff[BUFF_SIZE];
    memset(chBuff, 0, BUFF_SIZE);

    QString s = QString("upload|%1|%2|%3|donotremove").arg(fileName).arg(fileSize).arg(strUserName);
    strcpy(chBuff, s.toStdString().c_str());
    send(m_iClientSocket, chBuff, strlen(chBuff) + 1, 0);

    qint64 sendSize = 0;

    while (sendSize < fileSize) {
        memset(chBuff, 0, BUFF_SIZE);
        qint64 iRead = f.read(chBuff, BUFF_SIZE);
        if (iRead < 0) {
            qDebug() << "Server read file failed.";
            f.close();
            return false;
        }

        int iSend = send(m_iClientSocket, chBuff, iRead, 0);
        if (iSend < 0) {
            qDebug() << "Server send file content to client failed.";
            f.close();
            return false;
        }
        int temp = iRead;
        while (iSend < temp) {
            iSend = send(m_iClientSocket, chBuff + iSend, temp - iSend, 0);
            if (iSend < 0) {
                f.close();
                return false;
            }
            temp = temp - iSend;
        }

        sendSize += iSend;
        f.seek(sendSize);
        if (m_pParent != nullptr) {
            params.recved = sendSize;
            ((ClientThread*)m_pParent)->UpdateProgress(params);
        }
    }

    params.requestEndTime = tools::GetInstance()->GetCurrentTime2();
    if (m_pParent != nullptr) {
        params.recved = sendSize;
        ((ClientThread*)m_pParent)->UpdateProgress(params);
    }
    f.close();
    closesocket(m_iClientSocket);
    m_bSocketClosed = true;
}

bool Client::DownloadFileWithParams(NetworkParams &params) {
    char *chBuff = new char[BUFF_SIZE];
    memset(chBuff, 0, BUFF_SIZE);

    QString s = QString("download|%1|%2|donotremove").arg(params.fileName).arg(params.userName);
    strcpy(chBuff, s.toStdString().c_str());
    send(m_iClientSocket, chBuff, strlen(chBuff) + 1, 0);

    //receive file name and file size
    int recvLen = recv(m_iClientSocket, chBuff, BUFF_SIZE, 0);
    if (recvLen < 0) {
        qDebug() << "recv data from tcp server failed.";
        closesocket(m_iClientSocket);
        m_bSocketClosed = true;
        delete []chBuff;
        chBuff = nullptr;
        return false;
    }

    QString fileInfoData(chBuff);
    QStringList fileInfoArr = fileInfoData.split("|");
    qDebug() << "fileInfoArr:" << fileInfoArr;
    if (fileInfoArr.size() < 2) {
        qDebug() << "recv data was wrong." << fileInfoData;
        closesocket(m_iClientSocket);
        m_bSocketClosed = true;
        delete []chBuff;
        chBuff = nullptr;
        return false;
    }
    QString fileName = fileInfoArr[0];
    QString fileSizeStr = fileInfoArr[1];
    qulonglong fileSize = fileSizeStr.toULongLong();
    qulonglong recvedSize = 0;
    QFile f(params.saveFileDir + params.saveFileName);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "failed to save file:" << params.saveFileDir + params.saveFileName;
        closesocket(m_iClientSocket);
        m_bSocketClosed = true;
        delete []chBuff;
        chBuff = nullptr;
        return false;
    }
    params.totalSize = fileSize;
    while (recvedSize < fileSize) {
        memset(chBuff, 0, BUFF_SIZE);
        recvLen = recv(m_iClientSocket, chBuff, BUFF_SIZE, 0);
        qint64 writeLen = f.write(chBuff, recvLen);
        while (writeLen < recvLen) {
            char tempBuff[BUFF_SIZE];
            memset(tempBuff, 0, BUFF_SIZE);
            memcpy(tempBuff, chBuff + writeLen, BUFF_SIZE - writeLen);
            writeLen += f.write(tempBuff, recvLen - writeLen);
        }
        if (m_pParent != nullptr) {
            params.recved = recvedSize;
            ((ClientThread*)m_pParent)->UpdateProgress(params);
        }

        recvedSize += recvLen;
    }

    params.requestEndTime = tools::GetInstance()->GetCurrentTime2();
    if (m_pParent != nullptr) {
        params.recved = recvedSize;
        ((ClientThread*)m_pParent)->UpdateProgress(params);
    }
    f.close();
    closesocket(m_iClientSocket);
    m_bSocketClosed = true;
    delete []chBuff;
    chBuff = nullptr;
    qDebug() << "download file success:" << params.fileName << " saved file:" << params.saveFileDir + params.saveFileName;
    return true;
}
