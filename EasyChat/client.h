#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

#include <winsock2.h>

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

    bool    m_bIsStoped;

private:
    uint     m_iClientSocket;
    //char    m_chBuff[BUFF_SIZE];
    bool    m_bSocketClosed;

    struct sockaddr_in m_sockAddr;
    struct hostent      *m_hostnm;
    QObject *m_pParent;
};

#endif // CLIENT_H
