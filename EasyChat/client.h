#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

#include <winsock2.h>
#include <QObject>

class Client
{
public:
    Client();
    Client(QObject *parent);
    ~Client();

    bool Connect();

    //bool SendFile(QString strFullFilePath, QString strUserName);

    bool SendFileWithParams(NetworkParams &params);
    bool DownloadFileWithParams(NetworkParams &params);

private:
    uint     m_iClientSocket;
    bool    m_bSocketClosed;

    struct sockaddr_in m_sockAddr;
    struct hostent      *m_hostnm;
    QObject *m_pParent;
};

#endif // CLIENT_H
