#ifndef TOOLS_H
#define TOOLS_H
#include "common.h"

#include <QString>

class tools
{
public:
    static tools *m_pTools;
    static tools *GetInstance() {
        if (m_pTools == nullptr) {
            m_pTools = new tools();
        }
        return m_pTools;
    }

    QString GetCurrentTime2();   //YYYY-MM-DD HH:mm:SS
    QByteArray GenerateWebsocketMsg(MessageStruct &msg);
    QByteArray GenerateOnlineUserMessage();
    QString GenerateRandomID();
    QString FileMD5(QString &filePath);
    QString FileSizeToString(qulonglong fileSize);
private:
    tools();
};

#endif // TOOLS_H
