#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include "client.h"

#include <QThread>
#include <QObject>

class ClientThread : public QThread
{
    Q_OBJECT

public:
    ClientThread(QObject *parent = nullptr);
    ClientThread(QString fullFilePath, QString userName, QObject *parent = nullptr);
    ClientThread(NetworkParams &params, QObject*parent = nullptr);
    ~ClientThread();

    void run() override;

    void UpdateProgress(NetworkParams &params);

private:
    Client *m_pClient;
    QString m_strFullFilePath;
    QString m_strUserName;
    NetworkParams m_networkParams;

signals:
    void updateProgressSignal(qint64 recved, qint64 totalSize);
};

#endif // CLIENTTHREAD_H
