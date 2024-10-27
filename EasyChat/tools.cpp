#include "tools.h"

#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QTime>
#include <QFile>

tools* tools::m_pTools = nullptr;
tools::tools()
{

}

QString tools::GetCurrentTime2() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

QString tools::GetCurrentTime3() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd-HHmmsszzz");
}

QByteArray tools::GenerateWebsocketMsg(MessageStruct &msg) {
    QJsonObject obj;
    obj["username"] = msg.userName;
    obj["userid"] = msg.userID;
    obj["message"] = msg.msg;
    obj["time"] = msg.sendTime;
    obj["filelink"] = msg.fileUrl;
    obj["filename"] = msg.fileName;
    QJsonObject finalMsg;
    if (msg.toUserID.isEmpty()) {
        finalMsg["message"] = obj;
    } else {
        finalMsg[msg.toUserID] = obj;
    }
    QJsonDocument jsonDoc(finalMsg);
    return jsonDoc.toJson();
}

QByteArray tools::GenerateOnlineUserMessage() {
    QJsonObject jsonObj;
    jsonObj["username"] = g_userName;
    jsonObj["userid"] = g_userID;
    QJsonObject onlineObj;
    onlineObj["online"] = jsonObj;
    QJsonDocument jsonDoc(onlineObj);
    return jsonDoc.toJson(QJsonDocument::Compact);
}

QString tools::GenerateRandomID() {
    qsrand(QDateTime::currentMSecsSinceEpoch());

    static char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    uint len = sizeof(letters);

    QString result;
    for (int i = 0; i < 6; ++i) {
        result += letters[rand() % (len - 1)];
    }

    return result;
}

QString tools::FileMD5(QString &filePath) {
    QFile sourceFile(filePath);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(QCryptographicHash::Md5);

        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}

QString tools::FileSizeToString(qulonglong fileSize) {
    //return QString("%1byte").arg(fileSize);
    static qulonglong size1 = 1024;
    static qulonglong size2 = size1 * size1;
    static qulonglong size3 = size2 * size1;
    static qulonglong size4 = size3 * size1;
    static qulonglong size5 = size4 * size1;
    if (fileSize / size1 == 0) {
        return QString("%1b").arg(fileSize);
    } else if ((fileSize / size2) == 0) {
        return QString("%1K").arg((fileSize * 1.0) / size1);
    } else if ((fileSize / size3) == 0) {
        return QString("%1M").arg((fileSize * 1.0) / (size2 * 1.0));
    } else if ((fileSize / size4) == 0) {
        return QString("%1G").arg((fileSize * 1.0) / (size3 * 1.0));
    } else if ((fileSize / size5) == 0) {
        return QString("%1T").arg((fileSize * 1.0) / (size4 * 1.0));
    } else {
        return QString("%1b").arg(fileSize);
    }
}
